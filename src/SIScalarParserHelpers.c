#include "SIScalarParser.h"
#include "../../OCTypes/src/OCLibrary.h"
#include <ctype.h>
#include "SIScalar.h"
#include "SIDimensionality.h"
#include "SIUnitParser.h"

extern bool sis_syntax_error;
extern SIScalarRef result;

extern int sis_scan_string(const char *);
extern int sisparse(void);
extern void sislex_destroy(void);
extern int sislex(void);

OCStringRef scalarErrorString;

SIScalarRef
SIScalarCreateWithOCString(OCStringRef string, OCStringRef *error)
{
    if (OCStringCompare(string, kSIQuantityDimensionless, kOCCompareCaseInsensitive)
        == kOCCompareEqualTo)
    {
        return NULL;
    }

    OCMutableStringRef mutString = OCStringCreateMutableCopy(string);

    /* perform all Unicode-aware replacements */
    OCStringFindAndReplace(mutString, STR("*"), STR("•"),
        OCRangeMake(0, OCStringGetLength(mutString)), 0);
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

    // Insert '*' for implicit multiplication
    {
        OCMutableStringRef tmp = OCStringCreateMutableCopy(mutString);
        OCRelease(mutString);
        mutString = tmp;

        // Now drive *both* the lookups and inserts on `mutString`
        size_t len = OCStringGetLength(mutString);
        for (int idx = (int)len - 2; idx >= 0; --idx) {
            char prev = OCStringGetCharacterAtIndex(mutString, idx);
            char next = OCStringGetCharacterAtIndex(mutString, idx + 1);
            if (characterIsDigitOrDecimalPoint(prev) && isalpha((unsigned char)next)) {
                OCStringInsert(mutString, idx + 1, STR("*"));
            }
        }

    }

    OCStringFindAndReplace(mutString, STR(")("), STR(")*("),
                           OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("Ɑ"), STR("α"),
                           OCRangeMake(0, OCStringGetLength(mutString)), 0);

    // Handle implicit multiplication around parentheses
    OCArrayRef openParentheses = OCStringCreateArrayWithFindResults(
        mutString, STR("("), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    if (openParentheses) {
        OCMutableStringRef mutStringNew = OCStringCreateMutableCopy(mutString);
        for (int index = OCArrayGetCount(openParentheses) - 1; index >= 0; index--) {
            OCRange *range = (OCRange *)OCArrayGetValueAtIndex(openParentheses, index);
            if (range->location > 0 && range->location < OCStringGetLength(mutString)) {
                char prev = OCStringGetCharacterAtIndex(mutString, range->location - 1);
                bool closeBr = false, skip = false;
                for (int j = range->location - 1; j >= 0; j--) {
                    char c = OCStringGetCharacterAtIndex(mutString, j);
                    if (c == '[') { if (!closeBr) skip = true; }
                    if (c == ']') closeBr = true;
                }
                if (!skip && characterIsDigitOrDecimalPoint(prev)) {
                    OCStringInsert(mutStringNew, range->location, STR("*"));
                }
            }
        }
        OCRelease(mutString);
        mutString = mutStringNew;
        OCRelease(openParentheses);
    }

    OCArrayRef closeParentheses = OCStringCreateArrayWithFindResults(
        mutString, STR(")"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    if (closeParentheses) {
        OCMutableStringRef mutStringNew = OCStringCreateMutableCopy(mutString);
        for (int index = OCArrayGetCount(closeParentheses) - 1; index >= 0; index--) {
            OCRange *range = (OCRange *)OCArrayGetValueAtIndex(closeParentheses, index);
            if (range->location < OCStringGetLength(mutString) - 1) {
                unsigned char next = (unsigned char)OCStringGetCharacterAtIndex(mutString, range->location + 1);
                bool openBr = false, skip = false;
                for (int j = range->location + 1; j < OCStringGetLength(mutString); j++) {
                    unsigned char c = (unsigned char)OCStringGetCharacterAtIndex(mutString, j);
                    if (c == ']') { if (!openBr) skip = true; }
                    if (c == '[') openBr = true;
                }
                if (!skip) {
                    if (next != '+' && next != '-' && next != '*' && next != '/' && next != '^' && next != ')' && next != 0xE2) {
                        OCStringInsert(mutStringNew, range->location + 1, STR("*"));
                    }
                }
            }
        }
        OCRelease(mutString);
        mutString = mutStringNew;
        OCRelease(closeParentheses);
    }

// Ready to Parse    /* check for and get the final conversion unit */
    double unit_multiplier = 1.0;
    SIUnitRef finalUnit = ConversionWithDefinedUnit(mutString, &unit_multiplier, &scalarErrorString);

    sis_syntax_error = false;
    const char *cString = OCStringGetCString(mutString);
    if (cString) {
        sis_scan_string(cString);
        sisparse();
        sislex_destroy();
        OCRelease(mutString);
    }
    if (error) {
        if (scalarErrorString) *error = scalarErrorString;
        if (*error) {
            if (result) OCRelease(result);
            return NULL;
        }
    }

    if (result) {
        /* unit conversion and real-part extraction logic */
        if (finalUnit) {
            if (!SIScalarConvertToUnit((SIMutableScalarRef)result, finalUnit, error)) {
                OCRelease(result);
                return NULL;
            }
        }
        // If the result is real-only, extract its real component
        if (SIScalarIsReal(result)) {
            SIScalarRef realResult = SIScalarCreateByTakingComplexPart(result, kSIRealPart);
            OCRelease(result);
            return realResult;
        }
    } else {
        if (error) *error = STR("Syntax Error");
    }
    return result;
}

void
siserror(char *s, ...)
{
    scalarErrorString = STR("Syntax Error");
    sis_syntax_error = true;
}
