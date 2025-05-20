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

SIScalarRef SIScalarCreateWithOCString(OCStringRef string, OCStringRef *error)
{
    if(error) if(*error) return NULL;

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

    OCArrayRef openParentheses = OCStringCreateArrayWithFindResults(mutString,STR("("),OCRangeMake(0,OCStringGetLength(mutString)),0);
    if(openParentheses) {
        OCMutableStringRef  mutStringNew = OCStringCreateMutableCopy ( mutString);
        for(int index = OCArrayGetCount(openParentheses)-1; index>=0;index--) {
            OCRange *range = (OCRange *) OCArrayGetValueAtIndex(openParentheses,index);
            if(range->location>0 && range->location<OCStringGetLength(mutString)) {
                uint32_t previousCharacter = OCStringGetCharacterAtIndex(mutString,range->location-1);
                // Don't insert asterisk if it's a string inside [ ]
                bool closeSquareBracket = false;
                bool skipThis = false;
                for(int j=range->location-1; j>=0;j--) {
                    uint32_t scanChar = OCStringGetCharacterAtIndex(mutString,j);
                    if(scanChar=='[') {
                        if(!closeSquareBracket) skipThis = true;
                    }
                    if(scanChar==']') closeSquareBracket = true;
                }

                if(!skipThis && characterIsDigitOrDecimalPoint(previousCharacter)) OCStringInsert(mutStringNew, range->location, STR("*"));
            }
        }
        OCRelease(mutString);
        mutString = mutStringNew;
        OCRelease(openParentheses);
    }

    OCArrayRef closeParentheses = OCStringCreateArrayWithFindResults(mutString,STR(")"),OCRangeMake(0,OCStringGetLength(mutString)),0);
    if(closeParentheses) {
        OCMutableStringRef  mutStringNew = OCStringCreateMutableCopy ( mutString);
        for(OCIndex index = OCArrayGetCount(closeParentheses)-1; index>=0;index--) {
            OCRange *range = (OCRange *) OCArrayGetValueAtIndex(closeParentheses,index);
            if(range->location<OCStringGetLength(mutString)-1) {
                uint32_t nextCharacter = OCStringGetCharacterAtIndex(mutString,range->location+1);
                // Don't insert asterisk if it's a string inside [ ]
                bool openSquareBracket = false;
                bool skipThis = false;
                for(OCIndex j=range->location+1; j<OCStringGetLength(mutString);j++) {
                    uint32_t scanChar = OCStringGetCharacterAtIndex(mutString,j);
                    if(scanChar==']') {
                        if(!openSquareBracket) skipThis = true;
                    }
                    if(scanChar=='[') openSquareBracket = true;
                }
                if(!skipThis) {
                    if(nextCharacter !='+' && nextCharacter !='-'
                    && nextCharacter !='*' && nextCharacter !='/'
                    && nextCharacter !='^'  && nextCharacter !=')'
                    && nextCharacter !=8226) OCStringInsert(mutStringNew, range->location+1, STR("*"));
                }
                
            }
        }
        OCRelease(mutString);
        mutString = mutStringNew;
        OCRelease(closeParentheses);
   }

// Ready to Parse  

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
