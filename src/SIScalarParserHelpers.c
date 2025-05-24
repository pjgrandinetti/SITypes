#include "SIScalarParser.h"
#include <OCTypes/OCLibrary.h> // Corrected include path
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "SIScalar.h"
#include "SIDimensionality.h"
#include "SIUnitParser.h"

extern bool sis_syntax_error;
extern ScalarNodeRef sis_root;

extern SIScalarRef result;

extern int sis_scan_string(const char *);
extern int sisparse(void);
extern void sislex_destroy(void);
extern int sislex(void);

OCStringRef scalarErrorString;


// ——— UTF-8 iterator: decode next code-point and advance pointer ———
static uint32_t utf8_next(const char **p) {
    const unsigned char *s = (const unsigned char *)*p;
    uint32_t ch;
    if (s[0] < 0x80) {
        // 1-byte ASCII
        ch = s[0];
        *p += 1;
    } else if ((s[0] & 0xE0) == 0xC0 && (s[1] & 0xC0) == 0x80) {
        // 2-byte sequence
        ch = ((s[0] & 0x1F) << 6) |
              (s[1] & 0x3F);
        *p += 2;
    } else if ((s[0] & 0xF0) == 0xE0 &&
               (s[1] & 0xC0) == 0x80 &&
               (s[2] & 0xC0) == 0x80) {
        // 3-byte sequence
        ch = ((s[0] & 0x0F) << 12) |
             ((s[1] & 0x3F) <<  6) |
              (s[2] & 0x3F);
        *p += 3;
    } else if ((s[0] & 0xF8) == 0xF0 &&
               (s[1] & 0xC0) == 0x80 &&
               (s[2] & 0xC0) == 0x80 &&
               (s[3] & 0xC0) == 0x80) {
        // 4-byte sequence
        ch = ((s[0] & 0x07) << 18) |
             ((s[1] & 0x3F) << 12) |
             ((s[2] & 0x3F) <<  6) |
              (s[3] & 0x3F);
        *p += 4;
    } else {
        // Invalid UTF-8: emit U+FFFD and skip one byte
        ch = 0xFFFD;
        *p += 1;
    }
    return ch;
}



/**
 * @brief  Produce a new OCMutableString in which implicit multiplications
 *         around “(” and “)” have “*” inserted.
 * @param  original   The original mutable string (unchanged).
 * @return A brand-new OCMutableStringRef; caller must release it.
 */
OCMutableStringRef
insertAsterisks(OCMutableStringRef original)
{
    if (!original) return NULL;
    const char *src = OCStringGetCString((OCStringRef)original);
    if (!src)       return NULL;

    // 1) Measure code-points & collect code-point values
    size_t   cp_count  = OCStringGetLength((OCStringRef)original);
    uint32_t *cps      = malloc(cp_count * sizeof(uint32_t));
    size_t   *byte_off = malloc((cp_count + 1) * sizeof(size_t));

    const char *p = src;
    size_t       bo = 0;
    for (size_t i = 0; i < cp_count; i++) {
        byte_off[i] = bo;
        cps[i]      = utf8_next(&p);
        bo          = p - src;
    }
    byte_off[cp_count] = bo;               // one past final byte
    size_t orig_bytes = bo;

    // 2) Determine where to insert “*”
    bool *ins_before = calloc(cp_count, sizeof(bool));
    bool *ins_after  = calloc(cp_count, sizeof(bool));

    // Track square-bracket nesting
    int    bracket_level = 0;
    int   *levels        = malloc(cp_count * sizeof(int));
    for (size_t i = 0; i < cp_count; i++) {
        if (cps[i] == '[') {
            bracket_level++;
        }
        levels[i] = bracket_level;
        if (cps[i] == ']') {
            bracket_level = (bracket_level > 0 ? bracket_level - 1 : 0);
        }
    }

    for (size_t i = 0; i < cp_count; i++) {
        // Insert before “(” if preceded by digit/point and not inside [ ]
        if (cps[i] == '(' && i > 0 && levels[i-1] == 0) {
            if (characterIsDigitOrDecimalPoint(cps[i-1])) {
                ins_before[i] = true;
            }
        }
        // Insert after “)” if followed by a “term” and not inside [ ]
        else if (cps[i] == ')' && i+1 < cp_count && levels[i] == levels[i+1]) {
            uint32_t next = cps[i+1];
            if ( next != '+'  && next != '-'  && next != '*' &&
                 next != '/'  && next != '^'  && next != ')' &&
                 next != 0x2022 /* bullet */ )
            {
                ins_after[i] = true;
            }
        }
    }

    // 3) Count total insertions, allocate output buffer
    size_t insertions = 0;
    for (size_t i = 0; i < cp_count; i++) {
        if (ins_before[i]) insertions++;
        if (ins_after[i])  insertions++;
    }
    size_t new_bytes = orig_bytes + insertions * /* strlen("*") == */ 1;
    char *out = malloc(new_bytes + 1);
    char *d   = out;

    // 4) Build the new UTF-8 in one pass
    for (size_t i = 0; i < cp_count; i++) {
        if (ins_before[i]) {
            *d++ = '*';
        }
        size_t chunk_len = byte_off[i+1] - byte_off[i];
        memcpy(d, src + byte_off[i], chunk_len);
        d += chunk_len;
        if (ins_after[i]) {
            *d++ = '*';
        }
    }
    *d = '\0';

    // 5) Wrap into an OCMutableStringRef
    OCMutableStringRef result = OCMutableStringCreateWithCString(out);

    // 6) Cleanup
    free(cps);
    free(levels);
    free(ins_before);
    free(ins_after);
    free(byte_off);
    free(out);

    return result;
}


/**
 * @brief  Produce a new string in which implicit multiplications
 *         around “(” and “)” have “*” inserted.
 * @param  original   The original mutable string (not modified).
 * @return A brand‐new OCMutableStringRef; caller must release it.
 */
// OCMutableStringRef
// insertAsterisks(OCMutableStringRef original)
// {
//     // 1) Make a copy; we will insert into this
//     OCMutableStringRef result =
//         OCStringCreateMutableCopy((OCStringRef)original);
//     if (!result) return NULL;

//     // 2) Insert before ‘(’ where appropriate
//     OCRange range = { .location = 0,
//                       .length   = OCStringGetLength((OCStringRef)result) };
//     OCArrayRef openParens = OCStringCreateArrayWithFindResults(
//         (OCStringRef)result,
//         STR("("),
//         range,
//         0
//     );
//     if (openParens) {
//         for (int i = OCArrayGetCount(openParens) - 1; i >= 0; --i) {
//             OCRange *r = (OCRange *)OCArrayGetValueAtIndex(openParens, i);
//             // Only if preceded by a digit or decimal point, and not inside [ … ]
//             if (r->location > 0 &&
//                 r->location < OCStringGetLength((OCStringRef)result))
//             {
//                 uint32_t prevCp =
//                     OCStringGetCharacterAtIndex(result, r->location - 1);
//                 bool closeSq = false, skip = false;
//                 for (int j = (int)r->location - 1; j >= 0; --j) {
//                     uint32_t cp = OCStringGetCharacterAtIndex(result, j);
//                     if (cp == '[' && !closeSq) { skip = true; }
//                     if (cp == ']') { closeSq = true; }
//                 }
//                 if (!skip &&
//                     characterIsDigitOrDecimalPoint(prevCp))
//                 {
//                     OCStringInsert(result, r->location, STR("*"));
//                 }
//             }
//         }
//         OCRelease(openParens);
//     }

//     // 3) Insert after ‘)’ where appropriate
//     range.length = OCStringGetLength((OCStringRef)result);
//     OCArrayRef closeParens = OCStringCreateArrayWithFindResults(
//         (OCStringRef)result,
//         STR(")"),
//         range,
//         0
//     );
//     if (closeParens) {
//         for (int i = OCArrayGetCount(closeParens) - 1; i >= 0; --i) {
//             OCRange *r = (OCRange *)OCArrayGetValueAtIndex(closeParens, i);
//             if (r->location < OCStringGetLength((OCStringRef)result) - 1) {
//                 uint32_t nextCp =
//                     OCStringGetCharacterAtIndex(result, r->location + 1);
//                 // Don’t insert if it’s + - * / ^ ) or a bullet (U+2022)
//                 bool openSq = false, skip = false;
//                 for (uint64_t j = r->location + 1;
//                      j < OCStringGetLength((OCStringRef)result);
//                      ++j)
//                 {
//                     uint32_t cp = OCStringGetCharacterAtIndex(result, j);
//                     if (cp == ']' && !openSq) { skip = true; }
//                     if (cp == '[')               { openSq = true; }
//                 }
//                 if (!skip &&
//                     nextCp != '+'  &&
//                     nextCp != '-'  &&
//                     nextCp != '*'  &&
//                     nextCp != '/'  &&
//                     nextCp != '^'  &&
//                     nextCp != ')'  &&
//                     nextCp != 0x2022)
//                 {
//                     OCStringInsert(result, r->location + 1, STR("*"));
//                 }
//             }
//         }
//         OCRelease(closeParens);
//     }

//     return result;
// }

SIScalarRef SIScalarCreateWithOCString(OCStringRef string, OCStringRef *error)
{
    if(error) if(*error) return NULL;

    // 1) Create a local autorelease pool
    OCAutoreleasePoolRef pool = OCAutoreleasePoolCreate();

    if (OCStringCompare(string, kSIQuantityDimensionless, kOCCompareCaseInsensitive) == kOCCompareEqualTo) return NULL;

    OCMutableStringRef mutString = OCStringCreateMutableCopy(string);

    /* perform all Unicode-aware replacements */
    OCStringFindAndReplace(mutString, STR("*"), STR("•"),
        OCRangeMake(0, OCStringGetLength(mutString)), 0);

    result = NULL;
    scalarErrorString = NULL;
    // check for and get the final conversion unit
    double unit_multiplier = 1.0;
    SIUnitRef finalUnit = ConversionWithDefinedUnit(mutString, &unit_multiplier, error);

    OCStringFindAndReplace(mutString, STR("•"), STR("*"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("×"), STR("*"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("÷"), STR("/"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("−"), STR("-"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("\n"), STR(""), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("+"), STR("+"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("μ"), STR("µ"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("γ"), STR("𝛾"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("º"), STR("°"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("h_p"), STR("h_P"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("ɣ"), STR("𝛾"), OCRangeMake(0, OCStringGetLength(mutString)), 0);

    OCStringFindAndReplace(mutString, STR("√"), STR("sqrt"), OCRangeMake(0, OCStringGetLength(mutString)), 0);

    OCStringFindAndReplace(mutString, STR("∛"), STR("cbrt"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("∜"), STR("qtrt"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR(" "), STR(""), OCRangeMake(0, OCStringGetLength(mutString)), 0);

    // Quick fix for quartertsp
    OCStringFindAndReplace(mutString, STR("qtertsp"), STR("quartertsp"), OCRangeMake(0, OCStringGetLength(mutString)), kOCCompareCaseInsensitive);

    OCMutableStringRef newMutString = insertAsterisks(mutString);
    OCRelease(mutString);
    mutString = newMutString;

// Ready to Parse  
    const char *cString = OCStringGetCString(mutString);

    SIScalarRef out = NULL;
    if (cString) {
        sis_syntax_error = false;
        sis_scan_string(cString);
        sisparse();
        sislex_destroy();

        if (!sis_syntax_error && sis_root) {
            out = result;
        }

        /* whether parse succeeded or not, free the tree once here */
        if (sis_root) {
            ScalarNodeFree(sis_root);
            sis_root = NULL;
        }

        OCRelease(mutString);



    }
    if (error) {
        if (scalarErrorString) *error = scalarErrorString;
        if (*error) {
            if (out) OCRelease(out);
            OCAutoreleasePoolRelease(pool);
            return NULL;
        }
    }

    if (out) {
        /* unit conversion and real-part extraction logic */
        if (finalUnit) {
            if (!SIScalarConvertToUnit((SIMutableScalarRef)out, finalUnit, error)) {
                OCRelease(out);
                OCAutoreleasePoolRelease(pool);
                return NULL;
            }
        }
        // If the result is real-only, extract its real component
        if (SIScalarIsReal(out)) {
            SIScalarRef realResult = SIScalarCreateByTakingComplexPart(out, kSIRealPart);
            OCRelease(out);
            OCAutoreleasePoolRelease(pool);
            return realResult;
        }
    } else {
        if (error) *error = STR("Syntax Error");
    }
    OCRetain(out);
    OCAutoreleasePoolRelease(pool);
    return out;
}

void
siserror(char *s, ...)
{
    scalarErrorString = STR("Syntax Error");
    sis_syntax_error = true;
}
