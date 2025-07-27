//
//  SIUnit.c
//  SITypes
//
//  Created by philip on 6/7/17.
//  Copyright © 2017 PhySy Ltd. All rights reserved.
//
#include <locale.h>
#include <math.h>  // For floor, isnan, erf, erfc, log, sqrt, pow, fabs, log10
#include <stdlib.h>
#include <string.h>
#include "SIDimensionalityPrivate.h"
#include "SILibrary.h"
#include "SIUnitKey.h"

#define UNIT_NOT_FOUND -1
// SIUnit Opaque Type
struct impl_SIUnit {
    OCBase base;
    // SIUnit Type attributes
    SIDimensionalityRef dimensionality; // Every SIUnit has a dimensionality
    // Attributes needed to describe all Derived SI Units, including
    // Coherent SI Base Units, SI Base Units, & Derived Coherent SI Units
    SIPrefix num_prefix[BASE_DIMENSION_COUNT];
    SIPrefix den_prefix[BASE_DIMENSION_COUNT];
    // Attributes needed to describe Special SI Units and Non-SI units
    OCStringRef root_name;
    OCStringRef root_plural_name;
    OCStringRef root_symbol;
    SIPrefix root_symbol_prefix;
    bool is_special_si_symbol;
    bool allows_si_prefix;
    // Attributes needed to describe Non-SI units
    // unit must have a symbol for this value to have meaning.
    double scale_to_coherent_si;

    OCStringRef symbol; // Symbol of unit is generated.
    OCStringRef key; // Key for unit in library
};


static OCTypeID kSIUnitID = kOCNotATypeID;
OCTypeID SIUnitGetTypeID(void) {
    if (kSIUnitID == kOCNotATypeID)
        kSIUnitID = OCRegisterType("SIUnit");
    return kSIUnitID;
}
#pragma mark Static Utility Functions
#include <stdbool.h>
#include <stddef.h>
// ——————————————————————————————————————————
// 1) Master definition lists
// ——————————————————————————————————————————
#define SIPREFIX_DEFINITIONS             \
    X(kSIPrefixYotta, 24, "Y", "yotta")  \
    X(kSIPrefixZetta, 21, "Z", "zetta")  \
    X(kSIPrefixExa, 18, "E", "exa")      \
    X(kSIPrefixPeta, 15, "P", "peta")    \
    X(kSIPrefixTera, 12, "T", "tera")    \
    X(kSIPrefixGiga, 9, "G", "giga")     \
    X(kSIPrefixMega, 6, "M", "mega")     \
    X(kSIPrefixKilo, 3, "k", "kilo")     \
    X(kSIPrefixHecto, 2, "h", "hecto")   \
    X(kSIPrefixDeca, 1, "da", "deca")    \
    X(kSIPrefixNone, 0, "", "")          \
    X(kSIPrefixDeci, -1, "d", "deci")    \
    X(kSIPrefixCenti, -2, "c", "centi")  \
    X(kSIPrefixMilli, -3, "m", "milli")  \
    X(kSIPrefixMicro, -6, "µ", "micro")  \
    X(kSIPrefixNano, -9, "n", "nano")    \
    X(kSIPrefixPico, -12, "p", "pico")   \
    X(kSIPrefixFemto, -15, "f", "femto") \
    X(kSIPrefixAtto, -18, "a", "atto")   \
    X(kSIPrefixZepto, -21, "z", "zepto") \
    X(kSIPrefixYocto, -24, "y", "yocto")
#define SIBASE_DEFINITIONS                          \
    X(kSILengthIndex, "meter", "meters", "m")       \
    X(kSIMassIndex, "gram", "grams", "g")           \
    X(kSITimeIndex, "second", "seconds", "s")       \
    X(kSICurrentIndex, "ampere", "amperes", "A")    \
    X(kSITemperatureIndex, "kelvin", "kelvin", "K") \
    X(kSIAmountIndex, "mole", "moles", "mol")       \
    X(kSILuminousIntensityIndex, "candela", "candelas", "cd")


#define VALIDATE_PREFIX(prefix, label_str)                            \
    do {                                                               \
        if (!isValidSIPrefix(prefix)) {                                \
            OCRelease(theUnit);                                        \
            fprintf(stderr,                                            \
                    "*** ERROR - %s %s: invalid prefix for %s = %d\n", \
                    __FILE__, __func__,                                \
                    (label_str),                                       \
                    (int)(prefix));                                    \
            return NULL;                                               \
        }                                                              \
    } while (0)
    
// ——————————————————————————————————————————
// 2) Numeric tables (descending exponent order)
// ——————————————————————————————————————————
static const SIPrefix _prefixValues[] = {
#define X(pref, exp, sym, name) pref,
    SIPREFIX_DEFINITIONS
#undef X
};
static const int _prefixExponents[] = {
#define X(pref, exp, sym, name) exp,
    SIPREFIX_DEFINITIONS
#undef X
};
enum { _PREFIX_COUNT = sizeof(_prefixValues) / sizeof(_prefixValues[0]) };
// ——————————————————————————————————————————
// 3) Pure-table helpers
// ——————————————————————————————————————————
static const char *si_label_names[BASE_DIMENSION_COUNT] = {
    "length", "mass", "time", "current", "temperature", "amount", "luminous_intensity"};
static bool
isValidSIPrefix(SIPrefix input) {
    for (int i = 0; i < _PREFIX_COUNT; ++i)
        if (_prefixValues[i] == input)
            return true;
    return false;
}
static SIPrefix
findClosestPrefix(int input) {
    // clamp top/bottom
    if (input >= _prefixExponents[0]) return _prefixValues[0];
    if (input <= _prefixExponents[_PREFIX_COUNT - 1]) return _prefixValues[_PREFIX_COUNT - 1];
    // find first exponent ≤ input
    for (int i = 1; i < _PREFIX_COUNT; ++i) {
        if (input >= _prefixExponents[i])
            return _prefixValues[i];
    }
    return _prefixValues[_PREFIX_COUNT - 1];
}
// ——————————————————————————————————————————
// 4) String mappings via X-macro switches
// ——————————————————————————————————————————
static OCStringRef
prefixSymbolForSIPrefix(SIPrefix prefix) {
    switch (prefix) {
#define X(pref, exp, sym, name) \
    case pref:                  \
        return STR(sym);
        SIPREFIX_DEFINITIONS
#undef X
        default:
            return NULL;
    }
}
static OCStringRef
prefixNameForSIPrefix(SIPrefix prefix) {
    switch (prefix) {
#define X(pref, exp, sym, name) \
    case pref:                  \
        return STR(name);
        SIPREFIX_DEFINITIONS
#undef X
        default:
            return NULL;
    }
}
// ——————————————————————————————————————————
// 5) SI-base unit mappings via X-macro switches
// ——————————————————————————————————————————
static OCStringRef
baseUnitRootName(uint8_t idx) {
    switch (idx) {
#define X(i, root, plur, sym) \
    case i:                   \
        return STR(root);
        SIBASE_DEFINITIONS
#undef X
        default:
            return NULL;
    }
}
static OCStringRef
baseUnitPluralRootName(uint8_t idx) {
    switch (idx) {
#define X(i, root, plur, sym) \
    case i:                   \
        return STR(plur);
        SIBASE_DEFINITIONS
#undef X
        default:
            return NULL;
    }
}
static OCStringRef
baseUnitRootSymbol(uint8_t idx) {
    switch (idx) {
#define X(i, root, plur, sym) \
    case i:                   \
        return STR(sym);
        SIBASE_DEFINITIONS
#undef X
        default:
            return NULL;
    }
}
// Special-case kilogram (“gram” → “kilogram” / “kilograms” / “kg”)
static OCStringRef
baseUnitName(uint8_t idx) {
    return (idx == kSIMassIndex
                ? STR("kilogram")
                : baseUnitRootName(idx));
}
static OCStringRef
baseUnitPluralName(uint8_t idx) {
    return (idx == kSIMassIndex
                ? STR("kilograms")
                : baseUnitPluralRootName(idx));
}
static OCStringRef
baseUnitSymbol(uint8_t idx) {
    return (idx == kSIMassIndex
                ? STR("kg")
                : baseUnitRootSymbol(idx));
}
bool impl_SIUnitEqual(const void *theType1, const void *theType2) {
    // 1) Null‐object guard
    if (!theType1 || !theType2)
        return false;
    // 2) Quick pointer‐equality
    if (theType1 == theType2)
        return true;
    // 3) Ensure same OCTypes type
    if (OCGetTypeID(theType1) != OCGetTypeID(theType2))
        return false;
    // 4) Cast now that types match
    SIUnitRef u1 = (SIUnitRef)theType1;
    SIUnitRef u2 = (SIUnitRef)theType2;
    // 5) Dimensionality must match
    if (!OCTypeEqual(u1->dimensionality, u2->dimensionality))
        return false;
    // 6) Compare prefix arrays in one go
    if (memcmp(u1->num_prefix, u2->num_prefix, sizeof u1->num_prefix) != 0 ||
        memcmp(u1->den_prefix, u2->den_prefix, sizeof u1->den_prefix) != 0) {
        return false;
    }
    // 7) Compare all the OCStringRef fields
    {
        OCTypeRef const *fields1[] = {
            (OCTypeRef *)&u1->symbol,
            (OCTypeRef *)&u1->root_name,
            (OCTypeRef *)&u1->root_plural_name,
            (OCTypeRef *)&u1->root_symbol};
        OCTypeRef const *fields2[] = {
            (OCTypeRef *)&u2->symbol,
            (OCTypeRef *)&u2->root_name,
            (OCTypeRef *)&u2->root_plural_name,
            (OCTypeRef *)&u2->root_symbol};
        for (size_t i = 0; i < sizeof fields1 / sizeof *fields1; ++i) {
            if (!OCTypeEqual(*fields1[i], *fields2[i]))
                return false;
        }
    }
    // 8) Simple value‐type fields
    if (u1->root_symbol_prefix != u2->root_symbol_prefix ||
        u1->is_special_si_symbol != u2->is_special_si_symbol ||
        u1->allows_si_prefix != u2->allows_si_prefix) {
        return false;
    }
    // 9) Numeric scale
    if (OCCompareDoubleValues(u1->scale_to_coherent_si,
                              u2->scale_to_coherent_si) != kOCCompareEqualTo) {
        return false;
    }
    // All tests passed
    return true;
}
void impl_SIUnitFinalize(const void *theType) {
    if (NULL == theType) return;
    SIUnitRef theUnit = (SIUnitRef)theType;
    if (theUnit->dimensionality)
        OCRelease(theUnit->dimensionality);
    if (theUnit->symbol)
        OCRelease(theUnit->symbol);
    if (theUnit->key)
        OCRelease(theUnit->key);
    if (theUnit->root_name)
        OCRelease(theUnit->root_name);
    if (theUnit->root_plural_name)
        OCRelease(theUnit->root_plural_name);
    if (theUnit->root_symbol)
        OCRelease(theUnit->root_symbol);
}
static OCStringRef impl_SIUnitCopyFormattingDescription(OCTypeRef theType) {
    if (!theType)
        return NULL;
    SIUnitRef theUnit = (SIUnitRef)theType;
    if (theUnit->symbol) {
        return OCStringCreateCopy(theUnit->symbol);
    }
    // Provide a fallback string for better diagnostics
    return OCStringCreateWithCString("<SIUnit>");
}
static struct impl_SIUnit *SIUnitAllocate(void);
static void *impl_SIUnitDeepCopy(const void *obj) {
    if (!obj)
        return NULL;
    const SIUnitRef src = (SIUnitRef)obj;
    struct impl_SIUnit *copy = SIUnitAllocate();
    if (!copy)
        return NULL;
    // Copy dimensionality
    copy->dimensionality = OCTypeDeepCopy(src->dimensionality);
    // Copy prefixes
    for (int i = 0; i < BASE_DIMENSION_COUNT; ++i) {
        copy->num_prefix[i] = src->num_prefix[i];
        copy->den_prefix[i] = src->den_prefix[i];
    }
    // Copy simple fields
    copy->scale_to_coherent_si = src->scale_to_coherent_si;
    copy->allows_si_prefix = src->allows_si_prefix;
    copy->is_special_si_symbol = src->is_special_si_symbol;
    copy->root_symbol_prefix = src->root_symbol_prefix;
    // Copy strings (OCStringRef)
    if (src->symbol) copy->symbol = OCStringCreateCopy(src->symbol);
    if (src->root_name) copy->root_name = OCStringCreateCopy(src->root_name);
    if (src->root_plural_name) copy->root_plural_name = OCStringCreateCopy(src->root_plural_name);
    if (src->root_symbol) copy->root_symbol = OCStringCreateCopy(src->root_symbol);
    return (void *)copy;
}
static void *impl_SIUnitDeepCopyMutable(const void *obj) {
    // SIUnit is immutable; just return a standard deep copy
    return impl_SIUnitDeepCopy(obj);
}
static cJSON *impl_SIUnitCopyJSON(const void *obj) {
    return SIUnitCreateJSON((SIUnitRef)obj);
}
static struct impl_SIUnit *SIUnitAllocate() {
    struct impl_SIUnit *obj = OCTypeAlloc(struct impl_SIUnit,
                                          SIUnitGetTypeID(),
                                          impl_SIUnitFinalize,
                                          impl_SIUnitEqual,
                                          impl_SIUnitCopyFormattingDescription,
                                          impl_SIUnitCopyJSON,
                                          impl_SIUnitDeepCopy,
                                          impl_SIUnitDeepCopyMutable);
    if (!obj) {
        fprintf(stderr, "SIUnitAllocate: OCTypeAlloc failed.\n");
        return NULL;
    }
    // Initialize type-specific fields
    obj->dimensionality = NULL;
    for (int i = 0; i < BASE_DIMENSION_COUNT; i++) {
        obj->num_prefix[i] = kSIPrefixNone;
        obj->den_prefix[i] = kSIPrefixNone;
    }
    obj->scale_to_coherent_si = 1.0;
    obj->allows_si_prefix = false;
    obj->is_special_si_symbol = false;
    obj->root_symbol_prefix = kSIPrefixNone;
    obj->symbol = NULL;
    obj->root_name = NULL;
    obj->root_plural_name = NULL;
    obj->root_symbol = NULL;
    return obj;
}
/*
 @function SIUnitCreate
 @abstract Creates a new SIUnit object
 @param dimensionality dimensionality of the unit
 @param length_num_prefix integer exponent associated with SI prefix of length coherent base unit
 @param length_den_prefix integer exponent associated with SI prefix of length coherent base unit
 @param mass_num_prefix integer exponent associated with SI prefix of mass coherent base unit
 @param mass_den_prefix integer exponent associated with SI prefix of mass coherent base unit
 @param time_num_prefix integer exponent associated with SI prefix of time coherent base unit
 @param time_den_prefix integer exponent associated with SI prefix of time coherent base unit
 @param current_num_prefix integer exponent associated with SI prefix of current coherent base unit
 @param current_den_prefix integer exponent associated with SI prefix of current coherent base unit
 @param temperature_num_prefix integer exponent associated with SI prefix of temperature coherent base unit
 @param temperature_den_prefix integer exponent associated with SI prefix of temperature coherent base unit
 @param amount_num_prefix integer exponent associated with SI prefix of amount coherent base unit
 @param amount_den_prefix integer exponent associated with SI prefix of amount coherent base unit
 @param luminous_intensity_num_prefix integer exponent associated with SI prefix of luminous intensity coherent base unit
 @param luminous_intensity_den_prefix integer exponent associated with SI prefix of luminous intensity coherent base unit
 @param root_name the root name for the unit, such as Kelvin or mole, which can be prefixed with prefixes such as kilo, milli, etc.
 @param root_plural_name the plural version of the root name for the unit.
 @param root_symbol the symbol for the root name for the unit.
 @param root_symbol_prefix the integer exponent associated with SI prefix for the root unit.
 @param allows_si_prefix true if SI prefix can be used with root symbol
 @param is_special_si_symbol true for valid SI base symbols, false if base_symbol is non-SI symbol or if the base_name is NULL
 @param scale_to_coherent_si scaling of the Non-SI root unit
 to the coherent SI base unit or coherent derived SI unit with the
 same dimensionality.
 @result SIUnit object
 @discussion If unit is given a root name and symbol, then prefixes are allowed for this name and symbol, and the prefixes associated
 with the 7 dimensions are ignored.
 */


OCMutableStringRef SIUnitCreateNormalizedExpression(OCStringRef expression, bool forLibraryLookup) {
    if (!expression) return NULL;
    
    OCMutableStringRef normalized = OCStringCreateMutableCopy(expression);
    if (!normalized) return NULL;
    
    // Replace Unicode multiplication and division symbols with ASCII equivalents
    OCStringFindAndReplace2(normalized, STR("×"), STR("*"));
    OCStringFindAndReplace2(normalized, STR("·"), STR("*"));
    OCStringFindAndReplace2(normalized, STR("⋅"), STR("*"));  // Dot operator
    OCStringFindAndReplace2(normalized, STR("∙"), STR("*"));  // Bullet operator
    OCStringFindAndReplace2(normalized, STR("÷"), STR("/"));
    OCStringFindAndReplace2(normalized, STR("∕"), STR("/"));  // Division slash
    OCStringFindAndReplace2(normalized, STR("⁄"), STR("/"));  // Fraction slash
    
    // Normalize Unicode mu characters to standard micro sign (µ = U+00B5)
    OCStringFindAndReplace2(normalized, STR("μ"), STR("µ"));  // Greek Small Letter Mu (U+03BC)
    OCStringFindAndReplace2(normalized, STR("Μ"), STR("µ"));  // Greek Capital Letter Mu (U+039C) 
    OCStringFindAndReplace2(normalized, STR("ɥ"), STR("µ"));  // Latin Small Letter Turned H (U+0265) - sometimes confused
    OCStringFindAndReplace2(normalized, STR("𝜇"), STR("µ"));  // Mathematical Italic Small Mu (U+1D707)
    OCStringFindAndReplace2(normalized, STR("𝝁"), STR("µ"));  // Mathematical Bold Small Mu (U+1D741)
    OCStringFindAndReplace2(normalized, STR("𝝻"), STR("µ"));  // Mathematical Bold Italic Small Mu (U+1D77B)

    // Remove spaces around operators for consistent formatting
    OCStringFindAndReplace2(normalized, STR(" * "), STR("*"));
    OCStringFindAndReplace2(normalized, STR(" / "), STR("/"));
    OCStringFindAndReplace2(normalized, STR(" ^ "), STR("^"));
    
    // Trim leading/trailing whitespace
    OCStringTrimWhitespace(normalized);
    
    if (forLibraryLookup) {
        // For library lookup, replace * with • for consistent symbol representation
        OCStringFindAndReplace2(normalized, STR("*"), STR("•"));
    }
    else {
        // For display, replace • with * for multiplication
        OCStringFindAndReplace2(normalized, STR("•"), STR("*"));
    }
    
    return normalized;
}

static SIUnitRef SIUnitCreate(SIDimensionalityRef dimensionality,
                              const SIPrefix num_prefix[BASE_DIMENSION_COUNT], const SIPrefix den_prefix[BASE_DIMENSION_COUNT],
                              OCStringRef root_name, OCStringRef root_plural_name,
                              OCStringRef root_symbol, SIPrefix root_symbol_prefix,
                              bool allows_si_prefix, bool is_special_si_symbol,
                              double scale_to_coherent_si) {
    struct impl_SIUnit *theUnit = SIUnitAllocate();
    if (!theUnit) return NULL;
    theUnit->allows_si_prefix = allows_si_prefix;
    // Validate root symbol conditions
    if (root_symbol == NULL)
    {
        // Only derived SI units are allowed to have no symbol
        if (is_special_si_symbol)
        {
            // Can't be valid SI symbol if there's no symbol
            OCRelease(theUnit);
            fprintf(stderr, "*** ERROR - %s %s\n", __FILE__, __func__);
            fprintf(stderr, "          - can't be valid SI symbol if there's no symbol.\n");
            fprintf(stderr, "          - is_special_si_symbol = %d instead of 0\n", is_special_si_symbol);
            return NULL;
        }
        if (scale_to_coherent_si != 1.)
        {
            // non-SI units are not allowed to have no symbol
            OCRelease(theUnit);
            //            SIUnitRelease(kOCAllocatorDefault,theUnit);
            fprintf(stderr, "*** ERROR - %s %s\n", __FILE__, __func__);
            fprintf(stderr, "          - Only derived SI units are allowed to have no symbol.\n");
            fprintf(stderr, "          - scale_to_coherent_si = %g instead of 1.\n", scale_to_coherent_si);
            return NULL;
        }
        if (root_symbol_prefix)
        {
            // no prefix possible if no symbol
            OCRelease(theUnit);
            //            SIUnitRelease(kOCAllocatorDefault,theUnit);
            fprintf(stderr, "*** ERROR - %s %s\n", __FILE__, __func__);
            fprintf(stderr, "          - Trying to use SI prefix with no unit symbol.\n");
            return NULL;
        }
        theUnit->is_special_si_symbol = false;
        theUnit->root_symbol_prefix = 0;
        theUnit->scale_to_coherent_si = 1.0;
    }
    else {
        if (!isValidSIPrefix(root_symbol_prefix)) {
            OCRelease(theUnit);
            fprintf(stderr, "*** ERROR - %s %s: Invalid root_symbol_prefix = %d\n", __FILE__, __func__, root_symbol_prefix);
            return NULL;
        }
        theUnit->is_special_si_symbol = is_special_si_symbol;
        theUnit->root_symbol_prefix = root_symbol_prefix;
        theUnit->scale_to_coherent_si = is_special_si_symbol ? 1.0 : scale_to_coherent_si;
    }
    // Validate all prefixes
    for (int i = 0; i < BASE_DIMENSION_COUNT; i++) {
        VALIDATE_PREFIX(num_prefix[i], si_label_names[i]);
        VALIDATE_PREFIX(den_prefix[i], si_label_names[i]);
    }
    // Set attributes
    if (root_name) theUnit->root_name = OCStringCreateCopy(root_name);
    if (root_plural_name) theUnit->root_plural_name = OCStringCreateCopy(root_plural_name);
    if (root_symbol) theUnit->root_symbol = OCStringCreateCopy(root_symbol);
    theUnit->dimensionality = dimensionality;
    memcpy(theUnit->num_prefix, num_prefix, sizeof(SIPrefix) * BASE_DIMENSION_COUNT);
    memcpy(theUnit->den_prefix, den_prefix, sizeof(SIPrefix) * BASE_DIMENSION_COUNT);
    // Construct unit symbol and key
    theUnit->symbol = SIUnitCreateSymbol(theUnit);
    theUnit->key = SIUnitCreateLibraryKey(theUnit->symbol);
    return (SIUnitRef)theUnit;
}
// –––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
//  SIUnitCopyDictionary
//     Pack every field of an SIUnit into a CF‐dictionary,
//     using OCIndexArray for the fixed‐length SIPrefix arrays.
// –––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
static OCDictionaryRef SIUnitCopyDictionary(const void *obj) {
    if (!obj) return NULL;
    SIUnitRef u = (SIUnitRef)obj;
    OCMutableDictionaryRef dict = OCDictionaryCreateMutable(0);
    // 1) dimensionality → dictionary
    OCDictionaryRef dimDict = SIDimensionalityCopyDictionary(u->dimensionality);
    OCDictionaryAddValue(dict, STR("dimensionality"), dimDict);
    OCRelease(dimDict);
    // 2) num_prefix & den_prefix → OCIndexArray
    OCMutableIndexArrayRef numArr = OCIndexArrayCreateMutable(BASE_DIMENSION_COUNT);
    OCMutableIndexArrayRef denArr = OCIndexArrayCreateMutable(BASE_DIMENSION_COUNT);
    for (int i = 0; i < BASE_DIMENSION_COUNT; i++) {
        OCIndexArrayAppendValue(numArr, (OCIndex)u->num_prefix[i]);
        OCIndexArrayAppendValue(denArr, (OCIndex)u->den_prefix[i]);
    }
    OCDictionaryAddValue(dict, STR("num_prefix"), numArr);
    OCDictionaryAddValue(dict, STR("den_prefix"), denArr);
    OCRelease(numArr);
    OCRelease(denArr);
    // 3) all the OCStringRef fields
    OCDictionaryAddValue(dict, STR("symbol"), OCStringCreateCopy(u->symbol));
    OCDictionaryAddValue(dict, STR("key"), OCStringCreateCopy(u->key));
    OCDictionaryAddValue(dict, STR("root_name"), OCStringCreateCopy(u->root_name));
    OCDictionaryAddValue(dict, STR("root_plural_name"), OCStringCreateCopy(u->root_plural_name));
    OCDictionaryAddValue(dict, STR("root_symbol"), OCStringCreateCopy(u->root_symbol));
    // 4) primitive scalars (box as OCNumber)
    OCDictionaryAddValue(dict,
                         STR("root_symbol_prefix"),
                         OCNumberCreateWithInt((int)u->root_symbol_prefix));
    OCDictionaryAddValue(dict,
                         STR("allows_si_prefix"),
                         OCBooleanGetWithBool(u->allows_si_prefix));
    OCDictionaryAddValue(dict,
                         STR("is_special_si_symbol"),
                         OCBooleanGetWithBool(u->is_special_si_symbol));
    OCDictionaryAddValue(dict,
                         STR("scale_to_coherent_si"),
                         OCNumberCreateWithDouble(u->scale_to_coherent_si));
    return dict;
}
// –––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
//  SIUnitCreateFromDictionary
//     Consume an OCDictionary (as above) and re‐create/intern the singleton SIUnit.
// –––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
SIUnitRef
SIUnitCreateFromDictionary(OCDictionaryRef dict) {
    if (!dict) return NULL;
    // 1) dimensionality
    OCDictionaryRef dimDict = (OCDictionaryRef)OCDictionaryGetValue(dict, STR("dimensionality"));
    SIDimensionalityRef dim = SIDimensionalityFromDictionary(dimDict, NULL);
    if (!dim) return NULL;
    // 2) prefix arrays
    SIPrefix num_prefix[BASE_DIMENSION_COUNT], den_prefix[BASE_DIMENSION_COUNT];
    OCIndexArrayRef numArr = (OCIndexArrayRef)OCDictionaryGetValue(dict, STR("num_prefix"));
    OCIndexArrayRef denArr = (OCIndexArrayRef)OCDictionaryGetValue(dict, STR("den_prefix"));
    for (int i = 0; i < BASE_DIMENSION_COUNT; i++) {
        num_prefix[i] = (SIPrefix)OCIndexArrayGetValueAtIndex(numArr, i);
        den_prefix[i] = (SIPrefix)OCIndexArrayGetValueAtIndex(denArr, i);
    }
    // 3) strings
    OCStringRef root_name = OCStringCreateCopy((OCStringRef)OCDictionaryGetValue(dict, STR("root_name")));
    OCStringRef root_plural_name = OCStringCreateCopy((OCStringRef)OCDictionaryGetValue(dict, STR("root_plural_name")));
    OCStringRef root_symbol = OCStringCreateCopy((OCStringRef)OCDictionaryGetValue(dict, STR("root_symbol")));
    // 4) scalars
    OCNumberRef number = (OCNumberRef)OCDictionaryGetValue(dict, STR("root_symbol_prefix"));
    int value;
    OCNumberTryGetInt(number, &value);
    SIPrefix root_prefix = (SIPrefix) value;
    bool allows = OCBooleanGetValue((OCBooleanRef) OCDictionaryGetValue(dict, STR("allows_si_prefix")));
    bool special = OCBooleanGetValue((OCBooleanRef)OCDictionaryGetValue(dict, STR("is_special_si_symbol")));
    number = (OCNumberRef)OCDictionaryGetValue(dict, STR("scale_to_coherent_si"));
    double scale = 1.0;
    OCNumberTryGetDouble(number, &scale);
    SIUnitRef u = SIUnitCreate(dim, num_prefix, den_prefix, root_name, root_plural_name, root_symbol, root_prefix, allows, special, scale);
    // clean up temp copies…
    OCRelease(root_name);
    OCRelease(root_plural_name);
    OCRelease(root_symbol);
    return u;
}

bool SIUnitIsCoherentSIBaseUnit(SIUnitRef theUnit)
{
    IF_NO_OBJECT_EXISTS_RETURN(theUnit,false);
    
    // Non-SI units return false
    if(theUnit->scale_to_coherent_si != 1.) return false;
    
    // To be an SI base unit all the denominator exponents must be 0
    // and all numerator exponents are zero except one, which is 1
	if(theUnit->root_symbol==NULL) {
		for(int i=0;i<7;i++) if(SIDimensionalityGetDenExpAtIndex(theUnit->dimensionality, i) !=0) return false;
        int count = 0;
        int index = -1;
		for(int i=0;i<7;i++) {
            uint8_t numeratorExponent = SIDimensionalityGetNumExpAtIndex(theUnit->dimensionality, i);
            if(numeratorExponent>1) return false;
            if(numeratorExponent<0) return false;
            if(numeratorExponent==1) {
                index = i;
                count++;
            }
        }
        // To be a coherent base unit ...
        // All prefixes must be kSIPrefixNone, except mass, which is kSIPrefixKilo (for kilogram)
        if(index==1 && count==1 && theUnit->num_prefix[index]==kSIPrefixKilo) return true;
		else if(count==1 && theUnit->num_prefix[index]==kSIPrefixNone) return true;
	}
	return false;
}



/**
 * Only true if it’s a pure SI base unit (m, kg, s, A, K, mol, cd); false otherwise.
 */
bool SIUnitIsSIBaseUnit(SIUnitRef theUnit) {
    // 1) Must be a valid object
    if (theUnit == NULL) {
        return false;
    }
    // 2) Only coherent SI units have scale 1.0
    if (theUnit->scale_to_coherent_si != 1.0) {
        return false;
    }
    // 3) Must not have a custom symbol override
    if (theUnit->root_symbol != NULL) {
        return false;
    }
    // 4) All denominator exponents must be zero
    for (uint8_t i = 0; i < BASE_DIMENSION_COUNT; ++i) {
        if (SIDimensionalityGetDenExpAtIndex(theUnit->dimensionality, i) != 0) {
            return false;
        }
    }
    // 5) Count numerator exponents: exactly one must be 1 (and none >1)
    int countOnes = 0;
    for (uint8_t i = 0; i < BASE_DIMENSION_COUNT; ++i) {
        uint8_t numExp = SIDimensionalityGetNumExpAtIndex(theUnit->dimensionality, i);
        if (numExp > 1) {
            // Any exponent >1 disqualifies it
            return false;
        }
        if (numExp == 1) {
            ++countOnes;
        }
    }
    // Exactly one base dimension with exponent 1 => a base unit
    return (countOnes == 1);
}
#pragma mark Accessors
SIDimensionalityRef SIUnitGetDimensionality(SIUnitRef theUnit) {
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, NULL);
    return theUnit->dimensionality;
}
OCStringRef SIUnitCopyRootName(SIUnitRef theUnit) {
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, NULL);
    OCStringRef result = NULL;
    if (SIUnitIsSIBaseUnit(theUnit)) {
        for (uint8_t i = 0; i <= 6; i++) {
            if (SIDimensionalityGetNumExpAtIndex(theUnit->dimensionality, i)) {
                result = OCStringCreateCopy(baseUnitRootName(i));
                break;
            }
        }
    } else {
        if (theUnit->root_name) {
            result = OCStringCreateCopy(theUnit->root_name);
        }
    }
    if (!result) {
        result = STR("");
    }
    return result;
}
OCStringRef SIUnitCopyRootPluralName(SIUnitRef theUnit) {
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, NULL);
    if (SIUnitIsSIBaseUnit(theUnit)) {
        for (uint8_t i = 0; i < BASE_DIMENSION_COUNT; i++)
            if (SIDimensionalityGetNumExpAtIndex(theUnit->dimensionality, i))
                return OCStringCreateCopy(baseUnitPluralRootName(i));
    } else {
        if (theUnit->root_plural_name)
            return OCStringCreateCopy(theUnit->root_plural_name);
    }
    return STR("");
}
OCStringRef SIUnitCopyRootSymbol(SIUnitRef theUnit) {
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, NULL);
    if (SIUnitIsSIBaseUnit(theUnit)) {
        for (uint8_t i = 0; i < BASE_DIMENSION_COUNT; i++)
            if (SIDimensionalityGetNumExpAtIndex(theUnit->dimensionality, i))
                return OCStringCreateCopy(baseUnitRootSymbol(i));
    } else {
        if (theUnit->root_symbol)
            return OCStringCreateCopy(theUnit->root_symbol);
    }
    return NULL;
}
bool SIUnitAllowsSIPrefix(SIUnitRef theUnit) {
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, false);
    return theUnit->allows_si_prefix;
}
SIPrefix SIUnitGetRootSymbolPrefix(SIUnitRef theUnit) {
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, 0);
    return theUnit->root_symbol_prefix;
}
double SIUnitGetScaleNonSIToCoherentSI(SIUnitRef theUnit) {
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, 0);
    return theUnit->scale_to_coherent_si;
}
bool SIUnitGetIsSpecialSISymbol(SIUnitRef theUnit) {
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, false);
    return theUnit->is_special_si_symbol;
}
SIPrefix SIUnitGetNumeratorPrefixAtIndex(SIUnitRef theUnit, const uint8_t index) {
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, 0);
    return theUnit->num_prefix[index];
}
SIPrefix SIUnitGetDenominatorPrefixAtIndex(SIUnitRef theUnit, const uint8_t index) {
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, 0);
    return theUnit->den_prefix[index];
}
bool SIUnitIsDimensionlessAndUnderived(SIUnitRef theUnit) {
    if (!SIDimensionalityIsDimensionlessAndNotDerived(theUnit->dimensionality))
        return false;
    if (theUnit->root_name != NULL)
        return false;
    return true;
}
OCStringRef SIUnitCopySymbol(SIUnitRef theUnit) {
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, NULL);
    // Construct the symbol of the unit from root_symbol and prefix.
    if (SIUnitIsDimensionlessAndUnderived(theUnit)) {
        return OCStringCreateCopy(STR(" "));
    }
    // Symbol should be generated and saved when unit is created.
    return OCStringCreateCopy(theUnit->symbol);
}
OCStringRef SIUnitCreateSymbol(SIUnitRef unit) {
    // 1) Sanity check
    IF_NO_OBJECT_EXISTS_RETURN(unit, NULL);
    if (!unit->dimensionality) return NULL;
    // 2) If there's a special root_symbol at all (Ω, lb, Å, etc), just
    //    return prefix + that symbol.
    if (unit->root_symbol) {
        OCMutableStringRef s = OCStringCreateMutable(0);
        OCStringAppend(s, prefixSymbolForSIPrefix(unit->root_symbol_prefix));
        OCStringAppend(s, unit->root_symbol);
        return s;
    }

    // 3) If this is a true SI base unit (kg, m, s, A, K, mol, cd)
    //    or a *prefixed* base unit (cm, km, etc), we want just
    //    [prefix][single‐letter root].
    //    We detect that by: scale_to_coherent_si != 1 only for prefixes,
    //    exactly one numerator exponent == 1, all others 0, all den exps == 0.
    {
        bool allDenZero = true;
        int numOnes = 0;
        int whichIdx = -1;
        for (int i = 0; i < BASE_DIMENSION_COUNT; i++) {
            if (SIDimensionalityGetDenExpAtIndex(unit->dimensionality, i) != 0) {
                allDenZero = false;
                break;
            }
            uint8_t n = SIDimensionalityGetNumExpAtIndex(unit->dimensionality, i);
            if (n > 1) {
                allDenZero = false;
                break;
            }
            if (n == 1) {
                numOnes++;
                whichIdx = i;
            }
        }
        if (allDenZero && numOnes == 1) {
            // it's either the pure base unit (scale_to_coherent_si==1)
            // or something like cm, mm, etc (scale_to_coherent_si!=1).
            OCMutableStringRef s = OCStringCreateMutable(0);
            // numerator prefix (for cm, mm, etc) comes from num_prefix[whichIdx]
            OCStringAppend(s, prefixSymbolForSIPrefix(unit->num_prefix[whichIdx]));
            // then the one base‐unit letter
            OCStringAppend(s, baseUnitRootSymbol(whichIdx));
            return s;
        }
    }
    // 4) Otherwise we need the full derived form:
    //    build numerator and denominator strings out of the seven dims.
    OCMutableStringRef num = OCStringCreateMutable(0);
    OCMutableStringRef den = OCStringCreateMutable(0);
    bool multiDen = false;
// Helper lambda–style macro
#define APPEND_PART(sb, sym, exp, sep)                                      \
    do {                                                                    \
        if ((exp) != 1)                                                     \
            OCStringAppendFormat((sb), STR("%@%@^%d"), prefix, sym, (exp)); \
        else                                                                \
            OCStringAppendFormat((sb), STR("%@%@"), prefix, sym);           \
    } while (0)
    // a) Numerator
    for (int i = 0; i < BASE_DIMENSION_COUNT; i++) {
        uint8_t e = SIDimensionalityGetNumExpAtIndex(unit->dimensionality, i);
        if (e == 0) continue;
        OCStringRef prefix = prefixSymbolForSIPrefix(
            SIUnitGetNumeratorPrefixAtIndex(unit, i));
        OCStringRef root = baseUnitRootSymbol(i);
        if (OCStringGetLength(num) == 0) {
            APPEND_PART(num, root, e, "");
        } else {
            OCStringAppend(num, STR("•"));
            APPEND_PART(num, root, e, "•");
        }
    }
    // b) Denominator
    for (int i = 0; i < BASE_DIMENSION_COUNT; i++) {
        uint8_t e = SIDimensionalityGetDenExpAtIndex(unit->dimensionality, i);
        if (e == 0) continue;
        OCStringRef prefix = prefixSymbolForSIPrefix(
            SIUnitGetDenominatorPrefixAtIndex(unit, i));
        OCStringRef root = baseUnitRootSymbol(i);
        if (OCStringGetLength(den) == 0) {
            APPEND_PART(den, root, e, "");
        } else {
            multiDen = true;
            OCStringAppend(den, STR("•"));
            APPEND_PART(den, root, e, "•");
        }
    }
    // 5) Combine num/den into final symbol
    OCStringRef finalSym = NULL;
    if (OCStringGetLength(num) > 0) {
        if (OCStringGetLength(den) > 0) {
            // both parts
            if (multiDen)
                finalSym = OCStringCreateWithFormat(STR("%@/(%@)"), num, den);
            else
                finalSym = OCStringCreateWithFormat(STR("%@/%@"), num, den);
        } else {
            // only numerator
            finalSym = num;  // take ownership
        }
    } else {
        if (OCStringGetLength(den) > 0) {
            // only denominator
            if (multiDen)
                finalSym = OCStringCreateWithFormat(STR("(1/(%@))"), den);
            else
                finalSym = OCStringCreateWithFormat(STR("(1/%@)"), den);
        } else {
            // dimensionless
            finalSym = OCStringCreateMutable(0);  // empty string
        }
    }
    // clean up temporaries
    if (finalSym != num) OCRelease(num);
    if (finalSym != den) OCRelease(den);
#undef APPEND_PART
    return finalSym;
}
#pragma mark Operations
double SIUnitScaleToCoherentSIUnit(SIUnitRef theUnit) {
    /*
     *	This method calculates the scaling factor needed to transform a number with this unit
     *	into a number with the coherent si unit of the same dimensionality
     */
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, 0);
    // If this is one of the 7 SI base unit - or -
    // if the symbol is NULL then this must be a derived SI Unit
    // Either way calculate scale that returns to coherent derived unit
    // using dimension exponents and prefixes.
    if (theUnit->root_symbol == NULL) {
        // This method calculates the scaling back to a coherent derived unit
        // based solely on prefix and exponent for each of the seven dimensions.
        double scaling = 1.0;
        for (int i = 0; i < BASE_DIMENSION_COUNT; i++) {
            double numerator_power = SIDimensionalityGetNumExpAtIndex(theUnit->dimensionality, i);
            double denominator_power = SIDimensionalityGetDenExpAtIndex(theUnit->dimensionality, i);
            SIPrefix num_prefix = theUnit->num_prefix[i];
            SIPrefix den_prefix = theUnit->den_prefix[i];
            if (i == 1) {
                num_prefix -= kSIPrefixKilo;  // Since kilogram is the base unit
                den_prefix -= kSIPrefixKilo;  // Since kilogram is the base unit
            }
            scaling *= pow(10., (num_prefix * numerator_power - den_prefix * denominator_power));
        }
        return scaling;
    }
    // If symbol exists and it is is_special_si_symbol then return scale using symbol prefix
    // to return to base special SI symbol unit.
    if (theUnit->is_special_si_symbol)
        return pow(10., theUnit->root_symbol_prefix);
    // If symbol exists but is not is_special_si_symbol, then
    // scale_to_coherent_si is scale from base non-SI symbol to coherent SI base unit with same dimensionality
    // symbol prefix gives scale from prefixed non-SI unit to non-SI root unit.
    return theUnit->scale_to_coherent_si * pow(10., theUnit->root_symbol_prefix);
}
bool SIUnitAreEquivalentUnits(SIUnitRef theUnit1, SIUnitRef theUnit2) {
    IF_NO_OBJECT_EXISTS_RETURN(theUnit1, false);
    IF_NO_OBJECT_EXISTS_RETURN(theUnit2, false);
    if (theUnit1 == theUnit2)
        return true;
    // If true, these two units can be substituted for each other without modifying
    // the quantity's numerical value.
    if (!SIDimensionalityEqual(theUnit1->dimensionality, theUnit2->dimensionality))
        return false;
    if (OCCompareDoubleValues(SIUnitScaleToCoherentSIUnit(theUnit1), SIUnitScaleToCoherentSIUnit(theUnit2)) != kOCCompareEqualTo)
        return false;
    return true;
}
bool SIUnitIsCoherentDerivedUnit(SIUnitRef u) {
    // 1) Sanity
    if (!u) return false;
    // 2) must be exactly coherent (no scale factor)
    if (fabs(u->scale_to_coherent_si - 1.0) > 1e-12)
        return false;
    // 3) must *not* be any kind of “special” or prefixed base unit
    if (u->is_special_si_symbol || u->root_symbol_prefix != kSIPrefixNone)
        return false;
    // 4) must *not* be one of the pure base units (m, kg, ...)
    //    (so that “derived” really means “combination of bases”)
    if (SIUnitIsSIBaseUnit(u))
        return false;
    // 5) check all the dimension‐prefix arrays in one loop:
    for (int i = 0; i < BASE_DIMENSION_COUNT; i++) {
        // numerator prefix: only mass gets kilo, all others none
        SIPrefix wantNum = (i == kSIMassIndex) ? kSIPrefixKilo : kSIPrefixNone;
        if (u->num_prefix[i] != wantNum)
            return false;
        // denominator prefix: always none
        if (u->den_prefix[i] != kSIPrefixNone)
            return false;
    }
    // passed every test
    return true;
}
OCArrayRef SIUnitCreateArrayOfEquivalentUnits(SIUnitRef theUnit) {
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, NULL);
    OCArrayRef candidates = SIUnitCreateArrayOfUnitsForDimensionality(SIUnitGetDimensionality(theUnit));
    if (candidates) {
        OCMutableArrayRef result = OCArrayCreateMutable(0, &kOCTypeArrayCallBacks);
        for (uint64_t index = 0; index < OCArrayGetCount(candidates); index++) {
            SIUnitRef unit = OCArrayGetValueAtIndex(candidates, index);
            if (SIUnitAreEquivalentUnits(unit, theUnit))
                OCArrayAppendValue(result, unit);
        }
        OCRelease(candidates);
        return result;
    }
    return NULL;
}
double SIUnitConversion(SIUnitRef initialUnit, SIUnitRef finalUnit) {
    IF_NO_OBJECT_EXISTS_RETURN(initialUnit, 0);
    IF_NO_OBJECT_EXISTS_RETURN(finalUnit, 0);
    if (SIDimensionalityHasSameReducedDimensionality(initialUnit->dimensionality, finalUnit->dimensionality))
        return SIUnitScaleToCoherentSIUnit(initialUnit) / SIUnitScaleToCoherentSIUnit(finalUnit);
    return 0;
}
OCStringRef SIUnitCreateName(SIUnitRef theUnit) {
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, NULL);
    // Construct the name of the unit from root_name and prefix.
    if (SIUnitIsDimensionlessAndUnderived(theUnit))
        return kSIQuantityDimensionless;
    if (SIUnitIsSIBaseUnit(theUnit)) {
        // The root_name attribute is empty for the seven base units, so we need to ask
        // baseUnitRootName for its root_name
        for (int i = 0; i < BASE_DIMENSION_COUNT; i++) {
            // Only one numerator_exponent will be non-zero (and 1).
            if (SIDimensionalityGetNumExpAtIndex(theUnit->dimensionality, i)) {
                return OCStringCreateWithFormat(STR("%@%@"), prefixNameForSIPrefix(theUnit->num_prefix[i]), baseUnitRootName(i));
            }
        }
    } else {
        if (theUnit->root_name == NULL)
            return NULL;
        OCStringRef rootName = SIUnitCopyRootName(theUnit);
        OCStringRef name = OCStringCreateWithFormat(STR("%@%@"), prefixNameForSIPrefix(theUnit->root_symbol_prefix), rootName);
        OCRelease(rootName);
        return name;
    }
    return NULL;
}
OCStringRef SIUnitCreatePluralName(SIUnitRef theUnit) {
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, NULL);
    // Construct the plural name of the unit from root_plural_name and prefix.
    if (SIUnitIsSIBaseUnit(theUnit)) {
        // The root_plural_name attribute is empty for the seven base units, so we need to ask
        // baseUnitPluralRootName for its root_plural_name
        for (int i = 0; i < BASE_DIMENSION_COUNT; i++) {
            // Only one numerator_exponent will be non-zero (and 1).
            if (SIDimensionalityGetNumExpAtIndex(theUnit->dimensionality, i)) {
                OCMutableStringRef name = OCStringCreateMutable(64);
                OCStringRef prefix_string = prefixNameForSIPrefix(theUnit->num_prefix[i]);
                OCStringRef name_string = baseUnitPluralRootName(i);
                OCStringAppend(name, prefix_string);
                OCStringAppend(name, name_string);
                return name;
            }
        }
    } else {
        if (theUnit->root_plural_name == NULL)
            return NULL;
        OCMutableStringRef name = OCStringCreateMutable(64);
        OCStringRef prefix_string = prefixNameForSIPrefix(theUnit->root_symbol_prefix);
        OCStringAppend(name, prefix_string);
        OCStringAppend(name, theUnit->root_plural_name);
        return name;
    }
    return NULL;
}
static OCComparisonResult unitSort(const void *val1, const void *val2, void *context) {
    SIUnitRef unit1 = (SIUnitRef)val1;
    SIUnitRef unit2 = (SIUnitRef)val2;
    double scale1 = SIUnitScaleToCoherentSIUnit(unit1);
    double scale2 = SIUnitScaleToCoherentSIUnit(unit2);
    if (scale1 < scale2)
        return kOCCompareLessThan;
    else if (scale1 > scale2)
        return kOCCompareGreaterThan;
    else {
        OCStringRef symbol1 = SIUnitCopySymbol((SIUnitRef)val1);
        OCStringRef symbol2 = SIUnitCopySymbol((SIUnitRef)val2);
        OCComparisonResult result = OCStringCompare(symbol1, symbol2, kOCCompareCaseInsensitive);
        OCRelease(symbol1);
        OCRelease(symbol2);
        return result;
    }
}
static OCComparisonResult unitNameSort(const void *val1, const void *val2, void *context) {
    OCStringRef name1 = SIUnitCopyRootName((SIUnitRef)val1);
    OCStringRef name2 = SIUnitCopyRootName((SIUnitRef)val2);
    OCComparisonResult result = OCStringCompare(name1, name2, kOCCompareCaseInsensitive);
    OCRelease(name1);
    OCRelease(name2);
    return result;
}
OCArrayRef SIUnitCreateArrayOfConversionUnits(SIUnitRef theUnit) {
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, NULL);
    OCArrayRef result = SIUnitCreateArrayOfUnitsForSameReducedDimensionality(SIUnitGetDimensionality(theUnit));
    OCMutableArrayRef sorted = OCArrayCreateMutableCopy(result);
    OCArraySortValues(sorted, OCRangeMake(0, OCArrayGetCount(result)), unitSort, NULL);
    OCRelease(result);
    return sorted;
}
static SIUnitRef SIUnitFindEquivalentDerivedSIUnit(SIUnitRef input) {
    IF_NO_OBJECT_EXISTS_RETURN(input, NULL);
    if (input->root_symbol == NULL)
        return input;
    SIDimensionalityRef theDim = SIUnitGetDimensionality(input);
    OCArrayRef candidates = SIUnitCreateArrayOfUnitsForDimensionality(theDim);
    if (candidates) {
        int64_t closest = -1;
        double bestScaling = 100;
        for (int64_t index = 0; index < OCArrayGetCount(candidates); index++) {
            SIUnitRef unit = OCArrayGetValueAtIndex(candidates, index);
            if (SIUnitIsCoherentDerivedUnit(unit)) {  // was unit->root_symbol==NULL
                double scaling = fabs(log10(SIUnitScaleToCoherentSIUnit(unit) / SIUnitScaleToCoherentSIUnit(input)));
                if (fabs(log(SIUnitScaleToCoherentSIUnit(unit) / SIUnitScaleToCoherentSIUnit(input))) < bestScaling) {
                    bestScaling = scaling;
                    closest = index;
                }
            }
        }
        if (closest == -1) {
            OCRelease(candidates);
            return input;
        }
        SIUnitRef result = OCArrayGetValueAtIndex(candidates, closest);
        OCRelease(candidates);
        return result;
    }
    return input;
}
bool SIUnitIsDimensionless(SIUnitRef theUnit) {
    if (!SIDimensionalityIsDimensionless(theUnit->dimensionality))
        return false;
    return true;
}
#pragma mark Tests
bool SIUnitEqual(SIUnitRef theUnit1, SIUnitRef theUnit2) {
    // if true, then Units are equal in every way
    IF_NO_OBJECT_EXISTS_RETURN(theUnit1, false);
    IF_NO_OBJECT_EXISTS_RETURN(theUnit2, false);
    if (theUnit1 == theUnit2)
        return true;
    if (!SIUnitAreEquivalentUnits(theUnit1, theUnit2))
        return false;
    if (theUnit1->root_name == NULL && theUnit2->root_name != NULL)
        return false;
    if (theUnit1->root_name != NULL && theUnit2->root_name == NULL)
        return false;
    if (theUnit1->root_name != NULL && theUnit2->root_name != NULL) {
        if (OCStringCompare(theUnit1->root_name, theUnit2->root_name, 0) != kOCCompareEqualTo)
            return false;
    }
    if (theUnit1->root_plural_name == NULL && theUnit2->root_plural_name != NULL)
        return false;
    if (theUnit1->root_plural_name != NULL && theUnit2->root_plural_name == NULL)
        return false;
    if (theUnit1->root_plural_name != NULL && theUnit2->root_plural_name != NULL) {
        if (OCStringCompare(theUnit1->root_plural_name, theUnit2->root_plural_name, 0) != kOCCompareEqualTo)
            return false;
    }
    if (theUnit1->root_symbol == NULL && theUnit2->root_symbol != NULL)
        return false;
    if (theUnit1->root_symbol != NULL && theUnit2->root_symbol == NULL)
        return false;
    if (theUnit1->root_symbol != NULL && theUnit2->root_symbol != NULL) {
        if (OCStringCompare(theUnit1->root_symbol, theUnit2->root_symbol, 0) != kOCCompareEqualTo)
            return false;
    }
    if (theUnit1->root_symbol_prefix != theUnit2->root_symbol_prefix)
        return false;
    if (theUnit1->is_special_si_symbol != theUnit2->is_special_si_symbol)
        return false;
    if (theUnit1->scale_to_coherent_si != theUnit2->scale_to_coherent_si)
        return false;
    OCStringRef symbol1 = SIUnitCreateSymbol(theUnit1);
    OCStringRef symbol2 = SIUnitCreateSymbol(theUnit2);
    if (OCStringCompare(symbol1, symbol2, 0) != kOCCompareEqualTo) {
        OCRelease(symbol1);
        OCRelease(symbol2);
        return false;
    }
    OCRelease(symbol1);
    OCRelease(symbol2);
    return true;
}
OCStringRef SIUnitGuessQuantityName(SIUnitRef theUnit) {
    OCStringRef quantityName = NULL;
    SIDimensionalityRef dimensionality = SIUnitGetDimensionality(theUnit);
    OCArrayRef quantityNames = SIDimensionalityCreateArrayOfQuantityNames(dimensionality);
    if (quantityNames) {
        if (OCArrayGetCount(quantityNames) > 0) {
            quantityName = OCArrayGetValueAtIndex(quantityNames, 0);  // Borrowed
        }
        OCRelease(quantityNames);
        return quantityName;
    } else {
        // Fall back to dimensionality symbol
        return SIDimensionalityGetSymbol(dimensionality);
        // Optional logic (disabled below)
        // double multiplier = 1;
        // SIUnitRef reduced = SIUnitByReducing(theUnit, &multiplier);
        // if (reduced != theUnit) return SIUnitGuessQuantityName(reduced);
        // else return SIDimensionalityGetSymbol(dimensionality);
    }
}


static OCMutableDictionaryRef unitsLibrary = NULL;
static OCMutableDictionaryRef unitsQuantitiesLibrary = NULL;
static OCMutableDictionaryRef unitsDimensionalitiesLibrary = NULL;
static OCMutableArrayRef unitsNamesLibrary = NULL;
static bool imperialVolumes = false;
bool SIUnitsCreateLibraries(void);
OCMutableDictionaryRef SIUnitGetUnitsLib(void) {
    if(NULL==unitsLibrary) SIUnitsCreateLibraries();
    return unitsLibrary;
}
OCMutableDictionaryRef SIUnitGetQuantitiesLib(void) {
    if(NULL==unitsQuantitiesLibrary) SIUnitsCreateLibraries();
    return unitsQuantitiesLibrary;
}
OCMutableDictionaryRef SIUnitGetDimensionalitiesLib(void) {
    if(NULL==unitsDimensionalitiesLibrary) SIUnitsCreateLibraries();
    return unitsDimensionalitiesLibrary;
}
OCArrayRef SIUnitGetSortedNamesLib(void) {
    if (!SIUnitsCreateLibraries()) return NULL;
    return unitsNamesLibrary;
}
static bool
AddAllSIPrefixedUnitsToLibrary(SIUnitRef rootUnit, OCStringRef quantity);

static SIUnitRef AddUnitForQuantityToLibrary(OCStringRef quantity,
                                             SIPrefix length_num_prefix, SIPrefix length_den_prefix,
                                             SIPrefix mass_num_prefix, SIPrefix mass_den_prefix,
                                             SIPrefix time_num_prefix, SIPrefix time_den_prefix,
                                             SIPrefix current_num_prefix, SIPrefix current_den_prefix,
                                             SIPrefix temperature_num_prefix, SIPrefix temperature_den_prefix,
                                             SIPrefix amount_num_prefix, SIPrefix amount_den_prefix,
                                             SIPrefix luminous_intensity_num_prefix, SIPrefix luminous_intensity_den_prefix,
                                             OCStringRef root_name,
                                             OCStringRef root_plural_name,
                                             OCStringRef root_symbol,
                                             SIPrefix root_symbol_prefix,
                                             bool is_special_si_symbol,
                                             double scale_to_coherent_si,
                                             bool allows_si_prefix) {
    SIDimensionalityRef theDim = SIDimensionalityForQuantity(quantity, NULL);
    // Pack the individual prefixes into two small C-arrays:
    SIPrefix num_prefixes[BASE_DIMENSION_COUNT] = {
        length_num_prefix,
        mass_num_prefix,
        time_num_prefix,
        current_num_prefix,
        temperature_num_prefix,
        amount_num_prefix,
        luminous_intensity_num_prefix};
    SIPrefix den_prefixes[BASE_DIMENSION_COUNT] = {
        length_den_prefix,
        mass_den_prefix,
        time_den_prefix,
        current_den_prefix,
        temperature_den_prefix,
        amount_den_prefix,
        luminous_intensity_den_prefix};
    // Now call the two‐array prototype:
    SIUnitRef unit = SIUnitCreate(theDim, num_prefixes, den_prefixes, root_name, root_plural_name, root_symbol, root_symbol_prefix, allows_si_prefix, is_special_si_symbol, scale_to_coherent_si);
    if (NULL == unit) {
        fprintf(stderr, "ERROR - Could not create unit for quantity: ");
        OCStringShow(quantity);
        return NULL;
    }
    // In this function we override the SIUnitCreateLibraryKey and use the symbol as the key.
    // This is because this function is only called by for the predefined units and not by the user.
    OCRelease(unit->key);  // release the key created by SIUnitCreate

    struct impl_SIUnit *mutable_unit = (struct impl_SIUnit *)unit;
    mutable_unit->key = OCStringCreateCopy(unit->symbol);  // use the symbol as the key

    OCMutableDictionaryRef unitsLib = SIUnitGetUnitsLib();
    // Add unit to units library dictionary
    if (OCDictionaryContainsKey(unitsLib, unit->key)) {
        fprintf(stderr, "WARNING - Cannot add unit to library because symbol already is present: ");
        OCStringShow(unit->symbol);
        fprintf(stderr, "\n");
        OCRelease(unit);
        return OCDictionaryGetValue(unitsLib, unit->key);
    }
    OCTypeSetStaticInstance(unit, true);
    OCDictionaryAddValue(unitsLib, unit->key, unit);
    OCRelease(unit);
    // Append unit to mutable array value associated with quantity key inside quanity library dictionary
    {
        OCMutableDictionaryRef unitsQuantitiesLib = SIUnitGetQuantitiesLib();
        OCMutableArrayRef units = (OCMutableArrayRef)OCDictionaryGetValue(unitsQuantitiesLib, quantity);
        if (units)
            OCArrayAppendValue(units, unit);
        else {
            units = OCArrayCreateMutable(0, &kOCTypeArrayCallBacks);
            OCArrayAppendValue(units, unit);
            OCDictionaryAddValue(unitsQuantitiesLib, quantity, units);
            OCRelease(units);
        }
    }
    // Append unit to mutable array value associated with dimensionality key inside dimensionality library dictionary
    {
        OCStringRef dimensionalitySymbol = SIDimensionalityGetSymbol(theDim);
        OCMutableDictionaryRef unitsDimensionalitiesLib = SIUnitGetDimensionalitiesLib();
        OCMutableArrayRef units = (OCMutableArrayRef)OCDictionaryGetValue(unitsDimensionalitiesLib, dimensionalitySymbol);
        if (units)
            OCArrayAppendValue(units, unit);
        else {
            units = OCArrayCreateMutable(0, &kOCTypeArrayCallBacks);
            OCArrayAppendValue(units, unit);
            OCDictionaryAddValue(unitsDimensionalitiesLib, dimensionalitySymbol, units);
            OCRelease(units);
        }
    }
    if (allows_si_prefix)
        AddAllSIPrefixedUnitsToLibrary(unit, quantity);
    return unit;
}

static bool
AddAllSIPrefixedUnitsToLibrary(SIUnitRef rootUnit, OCStringRef quantity) {
    if (!rootUnit || !quantity) return false;
    // ————————————————————————————————————————
    // We list _only_ the exponents defined in the enum.
    // If you ever change the enum, you only update this list.
    static const int kSIPrefixExponents[] = {
        -24, -21, -18, -15, -12, -9, -6, -3, -2, -1,
        0,
        1, 2, 3, 6, 9, 12, 15, 18, 21, 24};
    const size_t kPrefixCount = sizeof(kSIPrefixExponents) / sizeof(kSIPrefixExponents[0]);
    const bool isBase = SIUnitIsSIBaseUnit(rootUnit);
    SIDimensionalityRef dim = isBase
                                  ? SIUnitGetDimensionality(rootUnit)
                                  : NULL;
    for (size_t i = 0; i < kPrefixCount; ++i) {
        SIPrefix p = (SIPrefix)kSIPrefixExponents[i];
        if (p == kSIPrefixNone)  // skip the “no-prefix” if you don’t want to re-add it
            continue;
        // Build per-dimension prefix arrays
        SIPrefix num[BASE_DIMENSION_COUNT];
        SIPrefix den[BASE_DIMENSION_COUNT];
        for (int8_t d = 0; d < BASE_DIMENSION_COUNT; ++d) {
            if (isBase) {
                // only apply to dimensions with exponent 1
                num[d] = (SIDimensionalityGetNumExpAtIndex(dim, d) == 1)
                             ? p
                             : kSIPrefixNone;
                den[d] = kSIPrefixNone;
            } else {
                num[d] = rootUnit->num_prefix[d];
                den[d] = rootUnit->den_prefix[d];
            }
        }
        // Single call for both base and derived
        AddUnitForQuantityToLibrary(
            quantity,
            num[kSILengthIndex], den[kSILengthIndex],
            num[kSIMassIndex], den[kSIMassIndex],
            num[kSITimeIndex], den[kSITimeIndex],
            num[kSICurrentIndex], den[kSICurrentIndex],
            num[kSITemperatureIndex], den[kSITemperatureIndex],
            num[kSIAmountIndex], den[kSIAmountIndex],
            num[kSILuminousIntensityIndex],
            den[kSILuminousIntensityIndex],
            rootUnit->root_name,  // symbol/name come straight from rootUnit
            rootUnit->root_plural_name,
            rootUnit->root_symbol,
            isBase
                ? rootUnit->root_symbol_prefix  // base-unit keeps its own symbol-prefix
                : p,                            // derived gets the new prefix
            rootUnit->is_special_si_symbol,
            rootUnit->scale_to_coherent_si,
            false);
    }
    return true;
}

static void AddSIBaseUnitToLibrary(OCStringRef quantity) {
    // all seven dims get no per-dim prefix
    SIPrefix none = kSIPrefixNone;
    AddUnitForQuantityToLibrary(
        quantity,
        none, none,  // length
        none, none,  // mass
        none, none,  // time
        none, none,  // current
        none, none,  // temperature
        none, none,  // amount
        none, none,  // luminous intensity
        /* root_name */ NULL,
        /* root_plural_name */ NULL,
        /* root_symbol */ NULL,
        /* root_symbol_prefix */ none,
        /* is_special_si_symbol */ false,
        /* allows_si_prefix */ true,
        /* scale_to_coherent_si */ 1.0);
}
/// Add a pure SI (base or derived) unit that should accept SI prefixes.
///   quantity – the SI‐quantity (e.g. kSIQuantitySpeed)
///   name     – the singular English name (“meter per second”)
///   plural   – the plural (“meters per second”)
///   symbol   – the symbol (“m/s”)
static void AddSIUnitWithPrefixesToLibrary(OCStringRef quantity,
                                           OCStringRef name,
                                           OCStringRef plural,
                                           OCStringRef symbol) {
    // no per‐dimension SIPrefix at all:
    SIPrefix none[BASE_DIMENSION_COUNT] = {
        kSIPrefixNone, kSIPrefixNone, kSIPrefixNone, kSIPrefixNone,
        kSIPrefixNone, kSIPrefixNone, kSIPrefixNone};
    AddUnitForQuantityToLibrary(
        quantity,
        // length       mass        time        current
        none[kSILengthIndex], none[kSILengthIndex],
        none[kSIMassIndex], none[kSIMassIndex],
        none[kSITimeIndex], none[kSITimeIndex],
        none[kSICurrentIndex], none[kSICurrentIndex],
        // temperature  amount      luminous
        none[kSITemperatureIndex], none[kSITemperatureIndex],
        none[kSIAmountIndex], none[kSIAmountIndex],
        none[kSILuminousIntensityIndex], none[kSILuminousIntensityIndex],
        /* root_name */ name,
        /* root_plural_name */ plural,
        /* root_symbol */ symbol,
        /* root_symbol_prefix */ kSIPrefixNone,
        /* is_special_si_symbol */ false,
        /* scale_to_coherent_si */ 1.0,
        /* allows_si_prefix */ true);
}
/// A convenience for “true” special‐SI units (Bq, Gy, Sv, etc),
/// which always are coherent, always allow SI prefixes,
/// and always have is_special_si_symbol==true.
static SIUnitRef AddSpecialSIUnit(OCStringRef quantity,
                                  OCStringRef root_name,
                                  OCStringRef root_plural_name,
                                  OCStringRef root_symbol) {
    // 1) look up the “pure” dimensionality for that quantity
    SIDimensionalityRef dim = SIDimensionalityForQuantity(quantity, NULL);
    if (!dim) return NULL;
    // 2) build the seven-element prefix arrays:
    //    * mass gets kilo, all other dims none, both numerator & denominator
    SIPrefix num[BASE_DIMENSION_COUNT] = {
        /* length */ kSIPrefixNone,
        /* mass */ kSIPrefixKilo,
        /* time */ kSIPrefixNone,
        /* current */ kSIPrefixNone,
        /* temperature */ kSIPrefixNone,
        /* amount */ kSIPrefixNone,
        /* luminous */ kSIPrefixNone};
    SIPrefix den[BASE_DIMENSION_COUNT] = {
        kSIPrefixNone, kSIPrefixNone, kSIPrefixNone,
        kSIPrefixNone, kSIPrefixNone, kSIPrefixNone,
        kSIPrefixNone};
    // 3) call the “real” factory
    return AddUnitForQuantityToLibrary(
        quantity,
        num[kSILengthIndex], den[kSILengthIndex],
        num[kSIMassIndex], den[kSIMassIndex],
        num[kSITimeIndex], den[kSITimeIndex],
        num[kSICurrentIndex], den[kSICurrentIndex],
        num[kSITemperatureIndex], den[kSITemperatureIndex],
        num[kSIAmountIndex], den[kSIAmountIndex],
        num[kSILuminousIntensityIndex], den[kSILuminousIntensityIndex],
        root_name,
        root_plural_name,
        root_symbol,
        /*root_symbol_prefix*/ kSIPrefixNone,
        /*is_special_si_symbol*/ true,
        /*scale_to_coherent_si*/ 1.0,
        /*allows_si_prefix*/ true);
}
/// Add a non-SI unit (accepted for use with SI) **with** SI prefixes.
///   quantity             – the SI quantity (e.g. kSIQuantityVolume)
///   name/pluralName      – human-readable names (“liter”/“liters”)
///   symbol               – the unit symbol (“L”)
///   scale_to_coherent_si – how to convert this unit into the coherent SI base
static void
AddNonSIUnitWithPrefixesToLibrary(OCStringRef quantity,
                                  OCStringRef name,
                                  OCStringRef pluralName,
                                  OCStringRef symbol,
                                  double scale_to_coherent_si) {
    // “no prefix” on all seven base dimensions:
    SIPrefix none[BASE_DIMENSION_COUNT] = {
        kSIPrefixNone,  // length
        kSIPrefixNone,  // mass
        kSIPrefixNone,  // time
        kSIPrefixNone,  // current
        kSIPrefixNone,  // temperature
        kSIPrefixNone,  // amount
        kSIPrefixNone   // luminous intensity
    };
    AddUnitForQuantityToLibrary(
        quantity,
        // per-dimension numerator/denominator prefixes:
        none[kSILengthIndex], none[kSILengthIndex],
        none[kSIMassIndex], none[kSIMassIndex],
        none[kSITimeIndex], none[kSITimeIndex],
        none[kSICurrentIndex], none[kSICurrentIndex],
        none[kSITemperatureIndex], none[kSITemperatureIndex],
        none[kSIAmountIndex], none[kSIAmountIndex],
        none[kSILuminousIntensityIndex], none[kSILuminousIntensityIndex],
        // root-name, plural-name, symbol:
        name,
        pluralName,
        symbol,
        // no “special SI-symbol” prefix on the root symbol itself:
        kSIPrefixNone,  // root_symbol_prefix
        false,          // is_special_si_symbol
        // how to convert this to the coherent SI base:
        scale_to_coherent_si,
        // **allow** mL, µL, kL, etc.:
        true  // allows_si_prefix
    );
}
static void AddNonSIUnitToLibrary(OCStringRef quantity, OCStringRef name, OCStringRef pluralName, OCStringRef symbol, double scale_to_coherent_si) {
    AddUnitForQuantityToLibrary(quantity,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                name, pluralName, symbol, kSIPrefixNone, false, scale_to_coherent_si, false);
}
SIUnitRef SIUnitFindWithName(OCStringRef input) {
    if (NULL == unitsLibrary)
        SIUnitsCreateLibraries();
    IF_NO_OBJECT_EXISTS_RETURN(unitsLibrary, NULL);
    int64_t count = OCDictionaryGetCount(unitsLibrary);
    OCStringRef keys[count];
    SIUnitRef units[count];
    OCDictionaryGetKeysAndValues(unitsLibrary, (const void **)keys, (const void **)units);
    for (int64_t index = 0; index < count; index++) {
        OCStringRef name = SIUnitCreateName(units[index]);
        if (name)
            if (OCStringCompare(name, input, 0) == kOCCompareEqualTo) {
                OCRelease(name);
                SIUnitRef theUnit = units[index];
                return theUnit;
            }
        if (name)
            OCRelease(name);
        name = SIUnitCreatePluralName(units[index]);
        if (name)
            if (OCStringCompare(name, input, 0) == kOCCompareEqualTo) {
                OCRelease(name);
                SIUnitRef theUnit = units[index];
                return theUnit;
            }
        if (name)
            OCRelease(name);
    }
    return NULL;
}
static OCComparisonResult unitNameLengthSort(const void *val1, const void *val2, void *context) {
    OCStringRef name1 = SIUnitCreateName((SIUnitRef)val1);
    OCStringRef name2 = SIUnitCreateName((SIUnitRef)val2);
    if (name1 == NULL && name2 == NULL)
        return kOCCompareEqualTo;
    if (name1 == NULL) {
        OCRelease(name2);
        return kOCCompareGreaterThan;
    }
    if (name2 == NULL) {
        OCRelease(name1);
        return kOCCompareLessThan;
    }
    OCComparisonResult result = kOCCompareEqualTo;
    if (OCStringGetLength(name1) > OCStringGetLength(name2))
        result = kOCCompareLessThan;
    if (OCStringGetLength(name1) < OCStringGetLength(name2))
        result = kOCCompareGreaterThan;
    OCRelease(name1);
    OCRelease(name2);
    return result;
}
SIUnitRef SIUnitFromExpression(OCStringRef expression, double *unit_multiplier, OCStringRef *error) {
    if (error && *error) return NULL;
    
    OCMutableDictionaryRef unitsLib = SIUnitGetUnitsLib();
    if (!unitsLib) {
        fprintf(stderr, "ERROR: couldn't initialize units library - out of memory?\n");
        return NULL;
    }
    
    if (NULL == expression) {
        if (error) {
            *error = STR("Unknown unit symbol");
        }
        return NULL;
    }
    
    IF_NO_OBJECT_EXISTS_RETURN(unitsLibrary, NULL);
    
    if (OCStringCompare(expression, STR(" "), 0) == kOCCompareEqualTo) {
        if (unit_multiplier) *unit_multiplier = 1.0;
        return SIUnitDimensionlessAndUnderived();
    }
    
    // Try library lookup first
    OCStringRef key = SIUnitCreateLibraryKey(expression);
    if (!key) {
        // NULL key indicates fractional power error
        if (error) {
            *error = STR("Fractional powers are not allowed in unit expressions");
        }
        return NULL;
    }
    SIUnitRef unit = OCDictionaryGetValue(unitsLibrary, key);
    if (unit) {
        if (unit_multiplier) *unit_multiplier = 1.0;
        OCRelease(key);
        return unit;
    }

    // Parse the expression if not found in library
    double multiplier = 1.0;  // Default multiplier
    OCStringRef canonical_expr = SIUnitCreateLibraryKey(expression);
    if (!canonical_expr) {
        // NULL canonical_expr indicates fractional power error
        if (error) {
            *error = STR("Fractional powers are not allowed in unit expressions");
        }
        OCRelease(key);
        return NULL;
    }
    unit = SIUnitFromExpressionInternal(canonical_expr, &multiplier, error);
    if(unit && multiplier != 1.0) {
        SIDimensionalityRef dimensionality = SIUnitGetDimensionality(unit);
        OCStringRef dimensionalitySymbol = SIDimensionalityGetSymbol(dimensionality);
        AddNonSIUnitToLibrary(dimensionalitySymbol, NULL, NULL, key, multiplier);
        unit = (SIUnitRef) OCDictionaryGetValue(unitsLibrary, key);
        if (unit_multiplier) *unit_multiplier = 1.0;
        OCRelease(key);
        OCRelease(canonical_expr);
        return unit;
    }
    if (unit_multiplier) *unit_multiplier = multiplier;

    OCRelease(key);
    OCRelease(canonical_expr);
    // error returned from SIUnitFromExpressionInternal
    return unit;
}

// Add a cleanup function for static dictionaries and array
void cleanupUnitsLibraries(void) {
    if (!unitsLibrary)
        return;
    if (unitsQuantitiesLibrary) {
        OCRelease(unitsQuantitiesLibrary);
        unitsQuantitiesLibrary = NULL;
    }
    if (unitsDimensionalitiesLibrary) {
        OCRelease(unitsDimensionalitiesLibrary);
        unitsDimensionalitiesLibrary = NULL;
    }
    if (unitsNamesLibrary) {
        OCRelease(unitsNamesLibrary);
        unitsNamesLibrary = NULL;
    }
    
    // Simple approach: clear static flags on all units, then let OCDictionary handle cleanup
    // OCDictionary should only release each unique pointer once, regardless of how many keys point to it
    OCArrayRef keys = OCDictionaryCreateArrayWithAllKeys((OCDictionaryRef)unitsLibrary);
    if (keys) {
        for (uint64_t i = 0; i < OCArrayGetCount(keys); i++) {
            OCStringRef key = (OCStringRef)OCArrayGetValueAtIndex(keys, i);
            SIUnitRef unit = (SIUnitRef)OCDictionaryGetValue(unitsLibrary, key);
            OCDictionaryRemoveValue(unitsLibrary, key);
            OCTypeSetStaticInstance(unit, false);
            OCRelease(unit);
        }
        OCRelease(keys);
    }
    
    // OCDictionary should handle duplicate pointers correctly during its own cleanup
    OCRelease(unitsLibrary);
    unitsLibrary = NULL;
}
SIUnitRef SIUnitFindWithUnderivedSymbol(OCStringRef symbol) {
    if (NULL == symbol) {
        return NULL;
    }
    if (NULL == unitsLibrary) SIUnitsCreateLibraries();
    IF_NO_OBJECT_EXISTS_RETURN(unitsLibrary, NULL);
    OCStringRef key = SIUnitCreateLibraryKey(symbol);
    SIUnitRef unit = OCDictionaryGetValue(unitsLibrary, key);
    OCRelease(key);
    return unit;
}
bool SIUnitsLibraryRemoveUnitWithSymbol(OCStringRef symbol) {
    if (NULL == unitsLibrary) SIUnitsCreateLibraries();
    OCStringRef key = SIUnitCreateLibraryKey(symbol);
    if (OCDictionaryContainsKey(unitsLibrary, key)) {
        SIUnitRef unit = (SIUnitRef)OCDictionaryGetValue(unitsLibrary, key);
        OCDictionaryRemoveValue(unitsLibrary, key);
        OCTypeSetStaticInstance(unit, false);
        OCRelease(unit);
        OCRelease(key);
        return true;
    }
    OCRelease(key);
    return false;
}
static SIUnitRef SIUnitWithParameters(SIDimensionalityRef dimensionality,
                                      const SIPrefix num_prefix[BASE_DIMENSION_COUNT],
                                      const SIPrefix den_prefix[BASE_DIMENSION_COUNT],
                                      OCStringRef root_name,
                                      OCStringRef root_plural_name,
                                      OCStringRef root_symbol,
                                      SIPrefix root_symbol_prefix,
                                      bool allows_si_prefix,
                                      bool is_special_si_symbol,
                                      double scale_to_coherent_si) {
    
    if (NULL == unitsLibrary) SIUnitsCreateLibraries();
    
    // Create a temporary unit to get its key, then check if equivalent exists
    SIUnitRef tempUnit = SIUnitCreate(dimensionality, num_prefix, den_prefix, 
                                      root_name, root_plural_name, root_symbol, 
                                      root_symbol_prefix, allows_si_prefix, 
                                      is_special_si_symbol, scale_to_coherent_si);
    if (NULL == tempUnit)
        return NULL;
    
    // Check if unit with this key already exists
    if (OCDictionaryContainsKey(unitsLibrary, tempUnit->key)) {
        SIUnitRef existingUnit = OCDictionaryGetValue(unitsLibrary, tempUnit->key);
        OCRelease(tempUnit);  // Discard the temporary unit
        return existingUnit;
    }
    
    // No existing unit found, so add this fresh unit to library
    OCTypeSetStaticInstance(tempUnit, true);
    OCDictionaryAddValue(unitsLibrary, tempUnit->key, tempUnit);
    OCRelease(tempUnit);
    return tempUnit;
}
SIUnitRef SIUnitFindEquivalentUnitWithShortestSymbol(SIUnitRef theUnit) {
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, NULL);
    if (SIUnitIsDimensionlessAndUnderived(theUnit))
        return theUnit;
    OCArrayRef candidates = SIUnitCreateArrayOfEquivalentUnits(theUnit);
    if (!candidates)
        return theUnit;
    if (OCArrayGetCount(candidates) == 0) {
        OCRelease(candidates);
        return theUnit;
    }
    SIUnitRef best = theUnit;
    OCStringRef symbol = SIUnitCopySymbol(theUnit);
    OCStringRef rootSymbol = SIUnitCopyRootSymbol(theUnit);
    if (rootSymbol) {
        // clean up everything before returning
        OCRelease(symbol);
        OCRelease(rootSymbol);
        OCRelease(candidates);
        return theUnit;
    }
    // no rootSymbol → use symbol’s length as baseline
    int64_t length = OCStringGetLength(symbol);
    OCRelease(symbol);
    for (uint64_t i = 0, cnt = OCArrayGetCount(candidates); i < cnt; i++) {
        SIUnitRef candidate = OCArrayGetValueAtIndex(candidates, i);
        OCStringRef candidateSymbol = SIUnitCopySymbol(candidate);
        OCStringRef candidateRootSym = SIUnitCopyRootSymbol(candidate);
        if (candidateRootSym) {
            best = candidate;
        } else if (OCStringGetLength(candidateSymbol) < length) {
            best = candidate;
            length = OCStringGetLength(candidateSymbol);
        }
        OCRelease(candidateSymbol);
        OCRelease(candidateRootSym);
    }
    OCRelease(candidates);
    return best;
}
SIUnitRef SIUnitByReducingSymbol(SIUnitRef theUnit, OCStringRef *error) {
    // Propagate any pending error
    if (error && *error) return NULL;
    
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, NULL);
    
    // Get the current symbol
    OCStringRef originalSymbol = SIUnitCopySymbol(theUnit);
    // Reduce the expression algebraically
    OCStringRef reducedSymbol = SIUnitReduceExpression(originalSymbol);
    if (!reducedSymbol) {
        if (error) {
            *error = STR("Failed to reduce unit expression");
        }
        OCRelease(originalSymbol);
        return NULL;
    }
    
    // Check if the symbol was actually reduced
    bool wasReduced = (OCStringCompare(reducedSymbol, originalSymbol, 0) != kOCCompareEqualTo);
    OCRelease(originalSymbol);
    
    if (!wasReduced) {
        OCRelease(reducedSymbol);
        // No reduction needed, return equivalent unit with shortest symbol
        return SIUnitFindEquivalentUnitWithShortestSymbol(theUnit);
    }
    
    // Parse the reduced symbol to get the new unit
    OCStringRef parsing_error = NULL;
    double parsing_multiplier = 1.0;
    SIUnitRef reducedUnit = SIUnitFromExpression(reducedSymbol, &parsing_multiplier, &parsing_error);
    
    if (parsing_error) {
        if (error) {
            *error = parsing_error;
        } else {
            OCRelease(parsing_error);
        }
        OCRelease(reducedSymbol);
        return NULL;
    }
    
    if (!reducedUnit) {
        if (error) {
            *error = STR("Failed to parse reduced unit expression");
        }
        OCRelease(reducedSymbol);
        return NULL;
    }
    
    // Calculate the scale factor between original and reduced units
    double originalScale = SIUnitScaleToCoherentSIUnit(theUnit);
    double reducedScale = SIUnitScaleToCoherentSIUnit(reducedUnit);
    double totalMultiplier = (originalScale / reducedScale) * parsing_multiplier;
    
    // For algebraic reduction, the multiplier should always be 1.0
    // If it's not, this indicates an unexpected issue in the reduction process
    if (fabs(totalMultiplier - 1.0) > 1e-12) {
        if (error) {
            *error = STR("Unit reduction resulted in scaling factor - algebraic reduction should preserve unit equivalence");
        }
        OCRelease(reducedSymbol);
        return NULL;
    }
    
    OCRelease(reducedSymbol);
    return reducedUnit;
}

SIUnitRef SIUnitByReducing(SIUnitRef theUnit, double *unit_multiplier) {
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, NULL);
    SIDimensionalityRef dimensionality = SIDimensionalityByReducing(theUnit->dimensionality);
    if (SIDimensionalityEqual(dimensionality, theUnit->dimensionality))
        return SIUnitFindEquivalentUnitWithShortestSymbol(theUnit);
    SIUnitRef reducedUnit = SIUnitWithParameters(
        dimensionality,
        theUnit->num_prefix,
        theUnit->den_prefix,
        /* root_name, root_plural_name, root_symbol */ NULL, NULL, NULL,
        /* root_symbol_prefix */ kSIPrefixNone,
        /* allows_si_prefix */ false,
        /* is_special_si_symbol */ false,
        /* scale_to_coherent_si */ 1.0);
    if (unit_multiplier) {
        if (*unit_multiplier == 0.0)
            *unit_multiplier = 1.0;
        *unit_multiplier *= SIUnitScaleToCoherentSIUnit(theUnit) / SIUnitScaleToCoherentSIUnit(reducedUnit);
    }
    return SIUnitFindEquivalentUnitWithShortestSymbol(reducedUnit);
}
SIUnitRef SIUnitByMultiplyingWithoutReducing(SIUnitRef theUnit1,
                                             SIUnitRef theUnit2,
                                             double *unit_multiplier,
                                             OCStringRef *error) {
    // 1) Propagate any pending error
    if (error && *error) return NULL;
    // 2) Null checks
    IF_NO_OBJECT_EXISTS_RETURN(theUnit1, NULL);
    IF_NO_OBJECT_EXISTS_RETURN(theUnit2, NULL);
    // 3) Shortcut: squaring the same unit
    if (theUnit1 == theUnit2) {
        return SIUnitByRaisingToPowerWithoutReducing(theUnit1,
                                                     2,
                                                     unit_multiplier,
                                                     error);
    }
    // 4) Identity: multiplying by dimensionless does nothing
    SIUnitRef dimless = SIUnitDimensionlessAndUnderived();
    if (theUnit1 == dimless) return theUnit2;
    if (theUnit2 == dimless) return theUnit1;
    // 5) Work in “pure” derived form
    SIUnitRef d1 = SIUnitFindEquivalentDerivedSIUnit(theUnit1);
    SIUnitRef d2 = SIUnitFindEquivalentDerivedSIUnit(theUnit2);
    // 6) Compute new dimensionality
    SIDimensionalityRef dim = SIDimensionalityByMultiplyingWithoutReducing(
        d1->dimensionality, d2->dimensionality, error);
    if (error && *error) return NULL;
    // 7) Build per-dimension prefix arrays
    SIPrefix num_prefix[BASE_DIMENSION_COUNT];
    SIPrefix den_prefix[BASE_DIMENSION_COUNT];
    for (uint8_t i = 0; i < BASE_DIMENSION_COUNT; ++i) {
        // Numerator prefix
        uint8_t exp = SIDimensionalityGetNumExpAtIndex(dim, i);
        uint8_t e1 = SIDimensionalityGetNumExpAtIndex(d1->dimensionality, i);
        uint8_t e2 = SIDimensionalityGetNumExpAtIndex(d2->dimensionality, i);
        int raw_prefix = d1->num_prefix[i] * e1 + d2->num_prefix[i] * e2;
        num_prefix[i] = exp
                            ? raw_prefix / exp
                            : (i == kSIMassIndex ? kSIPrefixKilo : kSIPrefixNone);
        if (!isValidSIPrefix(num_prefix[i]))
            num_prefix[i] = findClosestPrefix(raw_prefix);
        // Denominator prefix
        exp = SIDimensionalityGetDenExpAtIndex(dim, i);
        uint8_t d1d = SIDimensionalityGetDenExpAtIndex(d1->dimensionality, i);
        uint8_t d2d = SIDimensionalityGetDenExpAtIndex(d2->dimensionality, i);
        raw_prefix = d1->den_prefix[i] * d1d + d2->den_prefix[i] * d2d;
        den_prefix[i] = exp
                            ? raw_prefix / exp
                            : kSIPrefixNone;
        if (!isValidSIPrefix(den_prefix[i]))
            den_prefix[i] = findClosestPrefix(raw_prefix);
    }
    // 8) Special case: both operands have their own symbol (e.g. “hp”)
    if (d1->root_symbol && d2->root_symbol) {
        OCMutableStringRef sym = OCStringCreateMutable(0);
        OCStringAppend(sym, prefixSymbolForSIPrefix(d1->root_symbol_prefix));
        OCStringAppend(sym, d1->root_symbol);
        OCStringAppend(sym, STR("•"));
        OCStringAppend(sym, prefixSymbolForSIPrefix(d2->root_symbol_prefix));
        OCStringAppend(sym, d2->root_symbol);
        double newScale =
            SIUnitScaleToCoherentSIUnit(theUnit1) *
            SIUnitScaleToCoherentSIUnit(theUnit2);
        SIUnitRef out = SIUnitWithParameters(
            dim,
            num_prefix, den_prefix,
            /*root_name*/ NULL,
            /*root_plural_name*/ NULL,
            /*root_symbol*/ sym,
            /*root_symbol_prefix*/ kSIPrefixNone,
            /*allows_si_prefix*/ false,
            /*is_special_si_symbol*/ false,
            /*scale_to_coherent_si*/ newScale);
        OCRelease(sym);
        if (unit_multiplier) {
            if (*unit_multiplier == 0.0) *unit_multiplier = 1.0;
            double s1 = SIUnitScaleToCoherentSIUnit(theUnit1);
            double s2 = SIUnitScaleToCoherentSIUnit(theUnit2);
            double s = SIUnitScaleToCoherentSIUnit(out);
            *unit_multiplier *= (s1 * s2) / s;
        }
        return out;
    }
    // 9) General derived-unit fallback
    SIUnitRef out = SIUnitWithParameters(
        dim,
        num_prefix, den_prefix,
        /*root_name*/ NULL,
        /*root_plural_name*/ NULL,
        /*root_symbol*/ NULL,
        /*root_symbol_prefix*/ 0,
        /*allows_si_prefix*/ false,
        /*is_special_si_symbol*/ false,
        /*scale_to_coherent_si*/ 1.0);
    if (unit_multiplier) {
        if (*unit_multiplier == 0.0) *unit_multiplier = 1.0;
        double s1 = SIUnitScaleToCoherentSIUnit(theUnit1);
        double s2 = SIUnitScaleToCoherentSIUnit(theUnit2);
        double s = SIUnitScaleToCoherentSIUnit(out);
        *unit_multiplier *= (s1 * s2) / s;
    }
    return out;
}
SIUnitRef SIUnitByMultiplying(SIUnitRef theUnit1, SIUnitRef theUnit2, double *unit_multiplier, OCStringRef *error) {
    if (error)
        if (*error)
            return NULL;
    SIUnitRef unit = SIUnitByMultiplyingWithoutReducing(theUnit1, theUnit2, unit_multiplier, error);
    return SIUnitByReducing(unit, unit_multiplier);
}
SIUnitRef SIUnitByDividingWithoutReducing(SIUnitRef theUnit1,
                                          SIUnitRef theUnit2,
                                          double *unit_multiplier) {
    // 1) Sanity checks
    IF_NO_OBJECT_EXISTS_RETURN(theUnit1, NULL);
    IF_NO_OBJECT_EXISTS_RETURN(theUnit2, NULL);
    // 2) Convert to equivalent “pure” derived SI units
    SIUnitRef derived1 = SIUnitFindEquivalentDerivedSIUnit(theUnit1);
    SIUnitRef derived2 = SIUnitFindEquivalentDerivedSIUnit(theUnit2);
    // 3) Compute the new dimensionality
    SIDimensionalityRef dimensionality =
        SIDimensionalityByDividingWithoutReducing(derived1->dimensionality,
                                                  derived2->dimensionality);
    // 4) Build per-dimension SIPrefix arrays
    SIPrefix num_prefix[BASE_DIMENSION_COUNT];
    SIPrefix den_prefix[BASE_DIMENSION_COUNT];
    for (uint8_t i = 0; i < BASE_DIMENSION_COUNT; i++) {
        uint8_t numExp = SIDimensionalityGetNumExpAtIndex(dimensionality, i);
        uint8_t aNum = SIDimensionalityGetNumExpAtIndex(derived1->dimensionality, i);
        uint8_t bDen = SIDimensionalityGetDenExpAtIndex(derived2->dimensionality, i);
        num_prefix[i] =
            derived1->num_prefix[i] * aNum + derived2->den_prefix[i] * bDen;
        if (numExp)
            num_prefix[i] /= numExp;
        else
            num_prefix[i] = (i == kSIMassIndex ? kSIPrefixKilo : kSIPrefixNone);
        if (!isValidSIPrefix(num_prefix[i]))
            num_prefix[i] = findClosestPrefix(num_prefix[i]);
        uint8_t denExp = SIDimensionalityGetDenExpAtIndex(dimensionality, i);
        uint8_t aDen = SIDimensionalityGetDenExpAtIndex(derived1->dimensionality, i);
        uint8_t bNum = SIDimensionalityGetNumExpAtIndex(derived2->dimensionality, i);
        den_prefix[i] =
            derived1->den_prefix[i] * aDen + derived2->num_prefix[i] * bNum;
        if (denExp)
            den_prefix[i] /= denExp;
        else
            den_prefix[i] = kSIPrefixNone;
        if (!isValidSIPrefix(den_prefix[i]))
            den_prefix[i] = findClosestPrefix(den_prefix[i]);
    }
    // 5) Special-case: both operands have their own named symbol (e.g. “hp”)
    if (derived1->root_symbol && derived2->root_symbol) {
        // Build "prefix1+symbol1/prefix2+symbol2"
        OCMutableStringRef sym = OCStringCreateMutable(0);
        OCStringAppend(sym, prefixSymbolForSIPrefix(derived1->root_symbol_prefix));
        OCStringAppend(sym, derived1->root_symbol);
        OCStringAppend(sym, STR("/"));
        OCStringAppend(sym, prefixSymbolForSIPrefix(derived2->root_symbol_prefix));
        OCStringAppend(sym, derived2->root_symbol);
        // Compute new coherent-SI scale = scale1 / scale2
        double newScale = SIUnitScaleToCoherentSIUnit(theUnit1) / SIUnitScaleToCoherentSIUnit(theUnit2);
        // Create the brand-new SIUnit with our custom symbol
        SIUnitRef theUnit = SIUnitWithParameters(
            dimensionality,
            num_prefix, den_prefix,
            /*root_name*/ NULL,
            /*root_plural_name*/ NULL,
            /*root_symbol*/ sym,
            /*root_symbol_prefix*/ kSIPrefixNone,
            /*allows_si_prefix*/ false,
            /*is_special_si_symbol*/ false,
            /*scale_to_coherent_si*/ newScale);
        OCRelease(sym);
        // Adjust the external multiplier: (scale1/scale2) / theUnit->scale
        if (unit_multiplier) {
            if (*unit_multiplier == 0.0) *unit_multiplier = 1.0;
            double s1 = SIUnitScaleToCoherentSIUnit(theUnit1);
            double s2 = SIUnitScaleToCoherentSIUnit(theUnit2);
            double s = SIUnitScaleToCoherentSIUnit(theUnit);
            *unit_multiplier *= (s1 / s2) / s;
        }
        return theUnit;
    }
    // 6) General derived-unit fallback
    SIUnitRef theUnit = SIUnitWithParameters(
        dimensionality,
        num_prefix, den_prefix,
        /*root_name*/ NULL,
        /*root_plural_name*/ NULL,
        /*root_symbol*/ NULL,
        /*root_symbol_prefix*/ 0,
        /*allows_si_prefix*/ false,
        /*is_special_si_symbol*/ false,
        /*scale_to_coherent_si*/ 1.0);
    // 7) And finally adjust the multiplier as before
    if (unit_multiplier) {
        if (*unit_multiplier == 0.0) *unit_multiplier = 1.0;
        double s1 = SIUnitScaleToCoherentSIUnit(theUnit1);
        double s2 = SIUnitScaleToCoherentSIUnit(theUnit2);
        double s = SIUnitScaleToCoherentSIUnit(theUnit);
        *unit_multiplier *= (s1 / s2) / s;
    }
    return theUnit;
}
SIUnitRef SIUnitByDividing(SIUnitRef theUnit1, SIUnitRef theUnit2, double *unit_multiplier) {
    SIUnitRef unit = SIUnitByDividingWithoutReducing(theUnit1, theUnit2, unit_multiplier);
    return SIUnitByReducing(unit, unit_multiplier);
}

SIUnitRef SIUnitByTakingNthRoot(SIUnitRef input,
                                uint8_t root,
                                double *unit_multiplier,
                                OCStringRef *error) {
    if (error && *error) return NULL;
    IF_NO_OBJECT_EXISTS_RETURN(input, NULL);

    // Step 1: Find the canonical SI dimensionality for the result
    SIUnitRef derivedUnit = SIUnitFindEquivalentDerivedSIUnit(input);
    SIDimensionalityRef dimensionality =
        SIDimensionalityByTakingNthRoot(derivedUnit->dimensionality, root, error);
    if (error && *error) return NULL;

    // Step 2: Try to find a standard SI unit with this dimensionality
    SIUnitRef result_unit = SIUnitFindCoherentSIUnitWithDimensionality(dimensionality);
    if (result_unit) {
        // adjust multiplier for the change in unit scale
        if (unit_multiplier) {
            if (*unit_multiplier == 0.0) *unit_multiplier = 1.0;
            *unit_multiplier *= pow(SIUnitScaleToCoherentSIUnit(input), 1.0 / root) / SIUnitScaleToCoherentSIUnit(result_unit);
        }
        return result_unit;
    }

    // Step 3: If no SI unit matches, make a new derived unit
    SIUnitRef theUnit = SIUnitWithParameters(
        dimensionality,
        derivedUnit->num_prefix, derivedUnit->den_prefix,
        NULL, NULL, NULL, kSIPrefixNone, false, false, 1.0);
    if (unit_multiplier) {
        if (*unit_multiplier == 0.0) *unit_multiplier = 1.0;
        *unit_multiplier *= pow(SIUnitScaleToCoherentSIUnit(input), 1.0 / root);
    }
    return theUnit;
}
SIUnitRef SIUnitByRaisingToPowerWithoutReducing(SIUnitRef input,
                                                int power,
                                                double *unit_multiplier,
                                                OCStringRef *error) {
    if (error && *error)  // if an earlier error is pending, bail out
        return NULL;
    IF_NO_OBJECT_EXISTS_RETURN(input, NULL);
    if(power == 0) {
        return SIUnitDimensionlessAndUnderived();
    }

    // Convert int power to double for internal calculations
    double power_double = (double)power;
    
    // 1) Compute the new dimensionality
    SIDimensionalityRef dimensionality =
        SIDimensionalityByRaisingToPowerWithoutReducing(input->dimensionality,
                                                        power,
                                                        error);
    if (error && *error)
        return NULL;
    // 2) SPECIAL-CASE: if this unit has its own root_symbol (e.g. “hp”), just build “hp^n”
    if (input->root_symbol) {
        // build [prefix][root]^[power]
        OCMutableStringRef sym = OCStringCreateMutable(0);
        OCStringAppend(sym, prefixSymbolForSIPrefix(input->root_symbol_prefix));
        OCStringAppend(sym, input->root_symbol);
        if (power != 1) {
            OCStringAppendFormat(sym, STR("^%d"), power);
        }
        // new scale = (old scale)^power
        double newScale = pow(input->scale_to_coherent_si, power_double);
        // create a brand-new SIUnit with exactly the same per-dimensional prefixes
        SIUnitRef theUnit = SIUnitWithParameters(
            dimensionality,
            input->num_prefix, input->den_prefix,
            /*root_name=*/NULL,
            /*root_plural_name=*/NULL,
            /*root_symbol=*/sym,
            /*root_symbol_prefix=*/input->root_symbol_prefix,
            /*allows_si_prefix=*/false,
            /*is_special_si_symbol=*/input->is_special_si_symbol,
            /*scale_to_coherent_si=*/newScale);
        OCRelease(sym);
        // adjust the numerical multiplier
        if (unit_multiplier) {
            if (*unit_multiplier == 0.0) *unit_multiplier = 1.0;
            *unit_multiplier *= pow(SIUnitScaleToCoherentSIUnit(input), power) / SIUnitScaleToCoherentSIUnit(theUnit);
        }
        return theUnit;
    }
    // 3) Otherwise fall back to your existing derived-unit logic:
    SIUnitRef derived = SIUnitFindEquivalentDerivedSIUnit(input);
    SIUnitRef result;
    if (power > 0) {
        result = SIUnitWithParameters(
            dimensionality,
            derived->num_prefix, derived->den_prefix,
            NULL, NULL, NULL, 0, false, false, 1.0);
    } else {
        result = SIUnitWithParameters(
            dimensionality,
            derived->den_prefix, derived->num_prefix,
            NULL, NULL, NULL, 0, false, false, 1.0);
    }
    if (unit_multiplier) {
        if (*unit_multiplier == 0.0) *unit_multiplier = 1.0;
        *unit_multiplier *= pow(SIUnitScaleToCoherentSIUnit(input), power_double) / SIUnitScaleToCoherentSIUnit(result);
    }
    return result;
}
SIUnitRef SIUnitByRaisingToPower(SIUnitRef input, int power, double *unit_multiplier, OCStringRef *error) {
    if (error)
        if (*error)
            return NULL;
    SIUnitRef unit = SIUnitByRaisingToPowerWithoutReducing(input, power, unit_multiplier, error);
    return SIUnitByReducing(unit, unit_multiplier);
}
void SIUnitShow(SIUnitRef theUnit) {
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, );
    OCStringShow(theUnit->symbol);
    fprintf(stdout, "\n");
    return;
}
SIUnitRef SIUnitFindCoherentSIUnitWithDimensionality(SIDimensionalityRef theDim) {
    IF_NO_OBJECT_EXISTS_RETURN(theDim, NULL);
    SIPrefix num[BASE_DIMENSION_COUNT] = {kSIPrefixNone,kSIPrefixKilo,kSIPrefixNone,kSIPrefixNone,kSIPrefixNone,kSIPrefixNone,kSIPrefixNone};
    SIPrefix den[BASE_DIMENSION_COUNT] = {kSIPrefixNone,kSIPrefixKilo,kSIPrefixNone,kSIPrefixNone,kSIPrefixNone,kSIPrefixNone,kSIPrefixNone};
    return SIUnitWithParameters(theDim,
                                num,den,
                                NULL,
                                NULL,
                                NULL,
                                kSIPrefixNone,
                                false,
                                false,
                                1.0);
}
OCArrayRef SIUnitCreateArrayOfUnitsForQuantity(OCStringRef quantity) {
    IF_NO_OBJECT_EXISTS_RETURN(quantity, NULL);
    if (NULL == unitsLibrary)
        SIUnitsCreateLibraries();
    if (OCDictionaryContainsKey(unitsQuantitiesLibrary, quantity)) {
        OCMutableArrayRef array = (OCMutableArrayRef)OCDictionaryGetValue(unitsQuantitiesLibrary, quantity);
        return OCArrayCreateCopy(array);
    }
    return NULL;
}
OCArrayRef SIUnitCreateArrayOfUnitsForDimensionality(SIDimensionalityRef theDim) {
    IF_NO_OBJECT_EXISTS_RETURN(theDim, NULL);
    if (NULL == unitsLibrary)
        SIUnitsCreateLibraries();
    OCStringRef symbol = SIDimensionalityGetSymbol(theDim);
    if (OCDictionaryContainsKey(unitsDimensionalitiesLibrary, symbol)) {
        OCMutableArrayRef array = (OCMutableArrayRef)OCDictionaryGetValue(unitsDimensionalitiesLibrary, symbol);
        return OCArrayCreateCopy(array);
    }
    return NULL;
}
OCArrayRef SIUnitCreateArrayOfUnitsForSameReducedDimensionality(SIDimensionalityRef theDim) {
    IF_NO_OBJECT_EXISTS_RETURN(theDim, NULL);
    if (NULL == unitsLibrary)
        SIUnitsCreateLibraries();
    OCArrayRef dimensionalities = SIDimensionalityCreateArrayWithSameReducedDimensionality(theDim);
    OCMutableArrayRef result = OCArrayCreateMutable(0, &kOCTypeArrayCallBacks);
    for (uint64_t index = 0; index < OCArrayGetCount(dimensionalities); index++) {
        SIDimensionalityRef dimensionality = OCArrayGetValueAtIndex(dimensionalities, index);
        OCStringRef symbol = SIDimensionalityGetSymbol(dimensionality);
        if (OCDictionaryContainsKey(unitsDimensionalitiesLibrary, symbol)) {
            OCMutableArrayRef array = (OCMutableArrayRef)OCDictionaryGetValue(unitsDimensionalitiesLibrary, symbol);
            OCArrayAppendArray(result, array, OCRangeMake(0, OCArrayGetCount(array)));
        }
    }
    OCRelease(dimensionalities);
    return result;
}
SIUnitRef SIUnitDimensionlessAndUnderived(void) {
    
    SIPrefix num[BASE_DIMENSION_COUNT] = {kSIPrefixNone,kSIPrefixKilo,kSIPrefixNone,kSIPrefixNone,kSIPrefixNone,kSIPrefixNone,kSIPrefixNone};
    SIPrefix den[BASE_DIMENSION_COUNT] = {kSIPrefixNone,kSIPrefixKilo,kSIPrefixNone,kSIPrefixNone,kSIPrefixNone,kSIPrefixNone,kSIPrefixNone};

    return SIUnitWithParameters(SIDimensionalityDimensionless(),
                                num,den,
                                NULL,
                                NULL,
                                NULL,
                                kSIPrefixNone,
                                false,
                                false,
                                1.0);
}
bool SIUnitsLibraryRemovePlainVolumeUnits(void) {
    bool success = true;
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("gal"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("qt"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("pt"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("cup"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("gi"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("floz"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("tbsp"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("tsp"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("halftsp"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("quartertsp"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("(1/gal)"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("(1/qt)"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("(1/pt)"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("(1/cup)"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("(1/gi)"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("(1/floz)"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("(1/tbsp)"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("(1/tsp)"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("mi/gal"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("gal/h"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("gal/min"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("gal/s"));
    return success;
}
bool SIUnitRemoveUSLabeledVolumeUnits(void) {
    bool success = true;
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("galUS"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("qtUS"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("ptUS"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("cupUS"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("giUS"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("flozUS"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("tbspUS"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("tspUS"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("halftspUS"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("quartertspUS"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("(1/galUS)"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("(1/qtUS)"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("(1/ptUS)"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("(1/cupUS)"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("(1/giUS)"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("(1/flozUS)"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("(1/tbspUS)"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("(1/tspUS)"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("mi/galUS"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("galUS/h"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("galUS/min"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("galUS/s"));
    return success;
}
bool SIUnitRemoveUKLabeledVolumeUnits(void) {
    bool success = true;
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("galUK"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("qtUK"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("ptUK"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("cupUK"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("giUK"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("flozUK"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("tbspUK"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("tspUK"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("halftspUK"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("quartertspUK"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("(1/galUK)"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("(1/qtUK)"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("(1/ptUK)"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("(1/cupUK)"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("(1/giUK)"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("(1/flozUK)"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("(1/tbspUK)"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("(1/tspUK)"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("mi/galUK"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("galUK/h"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("galUK/min"));
    success *= SIUnitsLibraryRemoveUnitWithSymbol(STR("galUK/s"));
    return success;
}
void SIUnitAddUSLabeledVolumeUnits(void) {
    // Define US Volume units
    // Volume
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("US gallon"), STR("US gallons"), STR("galUS"), 0.003785411784);
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("US quart"), STR("US quarts"), STR("qtUS"), 0.003785411784 / 4);
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("US pint"), STR("US pints"), STR("ptUS"), 0.003785411784 / 8);
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("US cup"), STR("US cups"), STR("cupUS"), 0.003785411784 / 16);
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("US gill"), STR("US gills"), STR("giUS"), 0.003785411784 / 32);
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("US fluid ounce"), STR("US fluid ounces"), STR("flozUS"), 0.003785411784 / 128);
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("US tablespoon"), STR("US tablespoons"), STR("tbspUS"), 0.003785411784 / 256);
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("US teaspoon"), STR("US teaspoons"), STR("tspUS"), 0.003785411784 / 768);
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("US half teaspoon"), STR("US half teaspoons"), STR("halftspUS"), 0.003785411784 / 768 / 2.);
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("US quarter teaspoon"), STR("US quarter teaspoons"), STR("quartertspUS"), 0.003785411784 / 768 / 4.);
    // Inverse Volume
    AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse US gallon"), STR("inverse US gallons"), STR("(1/galUS)"), 1. / 0.003785411784);
    AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse US quart"), STR("inverse US quarts"), STR("(1/qtUS)"), 1. / (0.003785411784 / 4));
    AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse US pint"), STR("inverse US pints"), STR("(1/ptUS)"), 1. / (0.003785411784 / 8));
    AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse US cup"), STR("inverse US cups"), STR("(1/cupUS)"), 1. / (0.003785411784 / 16));
    AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse US gill"), STR("inverse US gills"), STR("(1/giUS)"), 1. / (0.003785411784 / 32));
    AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse US fluid ounce"), STR("inverse US fluid ounces"), STR("(1/flozUS)"), 1. / (0.003785411784 / 128));
    AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse US tablespoon"), STR("inverse US tablespoons"), STR("(1/tbspUS)"), 1. / (0.003785411784 / 256));
    AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse US teaspoon"), STR("inverse US teaspoon"), STR("(1/tspUS)"), 1. / (0.003785411784 / 768));
    // Distance / Volume
    AddNonSIUnitToLibrary(kSIQuantityLengthPerVolume, STR("mile per US gallon"), STR("miles per US gallon"), STR("mi/galUS"), 1609.344 / 0.003785411784);
    AddNonSIUnitToLibrary(kSIQuantityVolumetricFlowRate, STR("US gallon per hour"), STR("US gallons per hour"), STR("galUS/h"), 0.003785411784 / 3600.);
    AddNonSIUnitToLibrary(kSIQuantityVolumetricFlowRate, STR("US gallon per minute"), STR("US gallons per minute"), STR("galUS/min"), 0.003785411784 / 60.);
    AddNonSIUnitToLibrary(kSIQuantityVolumetricFlowRate, STR("US gallon per second"), STR("US gallons per second"), STR("galUS/s"), 0.003785411784);
}
void SIUnitsSetUSPlainVolumeUnits(void) {
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("gallon"), STR("gallons"), STR("gal"), 0.003785411784);
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("quart"), STR("quarts"), STR("qt"), 0.003785411784 / 4);
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("pint"), STR("pints"), STR("pt"), 0.003785411784 / 8);
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("cup"), STR("cups"), STR("cup"), 0.003785411784 / 16);
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("gill"), STR("gill"), STR("gi"), 0.003785411784 / 32);
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("fluid ounce"), STR("fluid ounces"), STR("floz"), 0.003785411784 / 128);
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("tablespoon"), STR("tablespoons"), STR("tbsp"), 0.003785411784 / 256);
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("teaspoon"), STR("teaspoons"), STR("tsp"), 0.003785411784 / 768);
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("half teaspoon"), STR("half teaspoons"), STR("halftsp"), 0.003785411784 / 768 / 2);
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("quarter teaspoon"), STR("quarter teaspoons"), STR("quartertsp"), 0.003785411784 / 768 / 4);
    // Inverse Volume
    AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse gallon"), STR("inverse gallons"), STR("(1/gal)"), 1. / 0.003785411784);
    AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse quart"), STR("inverse quarts"), STR("(1/qt)"), 1. / (0.003785411784 / 4));
    AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse pint"), STR("inverse pints"), STR("(1/pt)"), 1. / (0.003785411784 / 8));
    AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse cup"), STR("inverse cups"), STR("(1/cup)"), 1. / (0.003785411784 / 16));
    AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse gill"), STR("inverse gill"), STR("(1/gi)"), 1. / (0.003785411784 / 32));
    AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse fluid ounce"), STR("inverse fluid ounces"), STR("(1/floz)"), 1. / (0.003785411784 / 128));
    AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse tablespoon"), STR("inverse tablespoons"), STR("(1/tbsp)"), 1. / (0.003785411784 / 256));
    AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse teaspoon"), STR("inverse teaspoon"), STR("(1/tsp)"), 1. / (0.003785411784 / 768));
    // Distance / Volume
    AddNonSIUnitToLibrary(kSIQuantityLengthPerVolume, STR("mile per gallon"), STR("miles per gallon"), STR("mi/gal"), 1609.344 / 0.003785411784);
    AddNonSIUnitToLibrary(kSIQuantityVolumetricFlowRate, STR("gallon per hour"), STR("gallons per hour"), STR("gal/h"), 0.003785411784 / 3600.);
    AddNonSIUnitToLibrary(kSIQuantityVolumetricFlowRate, STR("gallon per minute"), STR("gallons per minute"), STR("gal/min"), 0.003785411784 / 60.);
    AddNonSIUnitToLibrary(kSIQuantityVolumetricFlowRate, STR("gallon per second"), STR("gallons per second"), STR("gal/s"), 0.003785411784);
}
void SIUnitSetUKPlainVolumeUnits(void) {
    // Volume
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("gallon"), STR("gallons"), STR("gal"), 0.00454609);
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("quart"), STR("quarts"), STR("qt"), 0.00454609 / 4);
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("pint"), STR("pints"), STR("pt"), 0.00454609 / 8);
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("cup"), STR("cups"), STR("cup"), 0.00454609 / 16);
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("gill"), STR("gill"), STR("gi"), 0.00454609 / 32);
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("fluid ounce"), STR("fluid ounces"), STR("floz"), 0.00454609 / 160);
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("tablespoon"), STR("tablespoons"), STR("tbsp"), 0.00454609 / 256);
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("teaspoon"), STR("teaspoons"), STR("tsp"), 0.00454609 / 768);
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("half teaspoon"), STR("half teaspoons"), STR("halftsp"), 0.00454609 / 768 / 2.);
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("quarter teaspoon"), STR("quarter teaspoons"), STR("quartertsp"), 0.00454609 / 768 / 4.);
    // Inverse Volume
    AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse gallon"), STR("inverse gallons"), STR("(1/gal)"), 1. / 0.00454609);
    AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse quart"), STR("inverse quarts"), STR("(1/qt)"), 1. / (0.00454609 / 4));
    AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse pint"), STR("inverse pints"), STR("(1/pt)"), 1. / (0.00454609 / 8));
    AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse cup"), STR("inverse cups"), STR("(1/cup)"), 1. / (0.00454609 / 16));
    AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse gill"), STR("inverse gills"), STR("(1/gi)"), 1. / (0.00454609 / 32));
    AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse fluid ounce"), STR("inverse fluid ounces"), STR("(1/floz)"), 1. / (0.00454609 / 160));
    AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse tablespoon"), STR("inverse tablespoons"), STR("(1/tbsp)"), 1. / (0.00454609 / 256));
    AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse teaspoon"), STR("inverse teaspoon"), STR("(1/tsp)"), 1. / (0.00454609 / 768));
    // Distance / Volume
    AddNonSIUnitToLibrary(kSIQuantityLengthPerVolume, STR("mile per gallon"), STR("miles per gallon"), STR("mi/gal"), 1609.344 / 0.00454609);
    AddNonSIUnitToLibrary(kSIQuantityVolumetricFlowRate, STR("gallon per hour"), STR("gallons per hour"), STR("gal/h"), 0.00454609 / 3600.);
    AddNonSIUnitToLibrary(kSIQuantityVolumetricFlowRate, STR("gallon per minute"), STR("gallons per minute"), STR("gal/min"), 0.00454609 / 60.);
    AddNonSIUnitToLibrary(kSIQuantityVolumetricFlowRate, STR("gallon per second"), STR("gallons per second"), STR("gal/s"), 0.00454609);
}
void SIUnitAddUKLabeledVolumeUnits(void) {
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("imperial gallon"), STR("imperial gallons"), STR("galUK"), 0.00454609);
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("imperial quart"), STR("imperial quarts"), STR("qtUK"), 0.00454609 / 4);
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("imperial pint"), STR("imperial pints"), STR("ptUK"), 0.00454609 / 8);
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("imperial cup"), STR("imperial cups"), STR("cupUK"), 0.00454609 / 16);
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("imperial gill"), STR("imperial gill"), STR("giUK"), 0.00454609 / 32);
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("imperial fluid ounce"), STR("imperial fluid ounces"), STR("flozUK"), 0.00454609 / 160);
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("imperial tablespoon"), STR("imperial tablespoons"), STR("tbspUK"), 0.00454609 / 256);
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("imperial teaspoon"), STR("imperial teaspoons"), STR("tspUK"), 0.00454609 / 768);
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("imperial half teaspoon"), STR("imperial half teaspoons"), STR("halftspUK"), 0.00454609 / 768 / 2);
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("imperial quarter teaspoon"), STR("imperial quarter teaspoons"), STR("quartertspUK"), 0.00454609 / 768 / 4);
    // Inverse Volume
    AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse imperial gallon"), STR("inverse imperial gallons"), STR("(1/galUK)"), 1. / 0.00454609);
    AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse imperial quart"), STR("inverse imperial quarts"), STR("(1/qtUK)"), 1. / (0.00454609 / 4));
    AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse imperial pint"), STR("inverse imperial pints"), STR("(1/ptUK)"), 1. / (0.00454609 / 8));
    AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse imperial cup"), STR("inverse imperial cups"), STR("(1/cupUK)"), 1. / (0.00454609 / 16));
    AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse imperial gill"), STR("inverse imperial gills"), STR("(1/giUK)"), 1. / (0.00454609 / 32));
    AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse imperial fluid ounce"), STR("inverse imperial fluid ounces"), STR("(1/flozUK)"), 1. / (0.00454609 / 160));
    AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse imperial tablespoon"), STR("inverse imperial tablespoons"), STR("(1/tbspUK)"), 1. / (0.00454609 / 256));
    AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse imperial teaspoon"), STR("inverse imperial teaspoon"), STR("(1/tspUK)"), 1. / (0.00454609 / 768));
    // Distance / Volume
    AddNonSIUnitToLibrary(kSIQuantityLengthPerVolume, STR("mile per imperial gallon"), STR("miles per imperial gallon"), STR("mi/galUK"), 1609.344 / 0.00454609);
    AddNonSIUnitToLibrary(kSIQuantityVolumetricFlowRate, STR("imperial gallon per hour"), STR("imperial gallons per hour"), STR("galUK/h"), 0.00454609 / 3600.);
    AddNonSIUnitToLibrary(kSIQuantityVolumetricFlowRate, STR("imperial gallon per minute"), STR("imperial gallons per minute"), STR("galUK/min"), 0.00454609 / 60.);
    AddNonSIUnitToLibrary(kSIQuantityVolumetricFlowRate, STR("imperial gallon per second"), STR("imperial gallons per second"), STR("galUK/s"), 0.00454609);
}
void SIUnitsLibrarySetImperialVolumes(bool value) {
    if (imperialVolumes == value) return;
    SIUnitsLibraryRemovePlainVolumeUnits();
    if (value) {
        SIUnitRemoveUKLabeledVolumeUnits();  // Remove Imperial Volumes
        SIUnitAddUSLabeledVolumeUnits();     // Define US Volume units
        SIUnitSetUKPlainVolumeUnits();       // Define UK Volume units
    } else {
        SIUnitRemoveUSLabeledVolumeUnits();  // Remove US Volumes
        SIUnitsSetUSPlainVolumeUnits();      // Define US Volume units
        SIUnitAddUKLabeledVolumeUnits();     // Define UK Volume units
    }
    imperialVolumes = value;
}
bool SIUnitsCreateLibraries(void) {
    setlocale(LC_ALL, "");
    const struct lconv *const currentlocale = localeconv();
    unitsLibrary = OCDictionaryCreateMutable(0);
    unitsQuantitiesLibrary = OCDictionaryCreateMutable(0);
    unitsDimensionalitiesLibrary = OCDictionaryCreateMutable(0);
    IF_NO_OBJECT_EXISTS_RETURN(unitsLibrary, false);
    IF_NO_OBJECT_EXISTS_RETURN(unitsQuantitiesLibrary, false);
    IF_NO_OBJECT_EXISTS_RETURN(unitsDimensionalitiesLibrary, false);
    // Derived Constants
//
#pragma mark Derived Constants
    double hbar = kSIPlanckConstant / (2 * kSIPi);
    // (1/(4•π•ε_0))•q_e^2/(c_0•h_P/(2•π))
    double alpha = (1. / (4. * kSIPi * kSIElectricConstant)) * kSIElementaryCharge * kSIElementaryCharge / (kSISpeedOfLight * kSIPlanckConstant / (2 * kSIPi));
    double lightYear = (double)kSIYear * (double)kSISpeedOfLight;
    // E_h = m_e • (q_e^2/(2•ε_0•h_P))^2
    double E_h = kSIElectronMass * (kSIElementaryCharge * kSIElementaryCharge / (2 * kSIElectricConstant * kSIPlanckConstant)) * (kSIElementaryCharge * kSIElementaryCharge / (2 * kSIElectricConstant * kSIPlanckConstant));
    // a_0 = ε_0•h_P^2/(π*m_e•q_e^2)
    double a_0 = kSIElectricConstant * kSIPlanckConstant * kSIPlanckConstant / (kSIPi * kSIElectronMass * kSIElementaryCharge * kSIElementaryCharge);
    double R_H = kSIElectronMass * kSIElementaryCharge * kSIElementaryCharge * kSIElementaryCharge * kSIElementaryCharge / (8 * kSIElectricConstant * kSIElectricConstant * kSIPlanckConstant * kSIPlanckConstant * kSIPlanckConstant * kSISpeedOfLight);
    // R_H = m_e•q_e^4/(8•ε_0^2•h_P^3•c_0)
    double Ry = kSIPlanckConstant * kSISpeedOfLight * R_H;
    double Λ_0 = E_h / (kSIElementaryCharge * a_0 * a_0);
    double G_0 = 2 * kSIElementaryCharge * kSIElementaryCharge / kSIPlanckConstant;
    double mu_N = kSIElementaryCharge * hbar / (2 * kSIProtonMass);
    double mu_e = kSIElementaryCharge * hbar / (2 * kSIElectronMass);
    double planckTime = sqrt(hbar * (double)kSIGravitationalConstant / ((double)kSISpeedOfLight * kSISpeedOfLight * kSISpeedOfLight * kSISpeedOfLight * kSISpeedOfLight));
    // sqrt(h_P * &G/(2*π*c_0^3))
    double planckLength = sqrt(hbar * (double)kSIGravitationalConstant / ((double)kSISpeedOfLight * kSISpeedOfLight * kSISpeedOfLight));
    double planckMass = sqrt(hbar * kSISpeedOfLight / kSIGravitationalConstant);
    double planckTemperature = planckMass * kSISpeedOfLight * kSISpeedOfLight / kSIBoltmannConstant;
//
#pragma mark kSIQuantityDimensionless
    AddNonSIUnitToLibrary(kSIQuantityDimensionless, STR("dimensionless"), STR("dimensionless"), STR(" "), 1);
    AddNonSIUnitToLibrary(kSIQuantityDimensionless, STR("percent"), STR("percent"), STR("%"), 0.01);
    AddNonSIUnitToLibrary(kSIQuantityDimensionless, STR("part per thousand"), STR("parts per thousand"), STR("‰"), 0.001);
    AddNonSIUnitToLibrary(kSIQuantityDimensionless, STR("part per ten thousand"), STR("parts per ten thousand"), STR("‱"), 0.0001);
    AddNonSIUnitToLibrary(kSIQuantityDimensionless, STR("part per million"), STR("parts per million"), STR("ppm"), 1.e-6);
    AddNonSIUnitToLibrary(kSIQuantityDimensionless, STR("part per billion"), STR("parts per billion"), STR("ppb"), 1.e-9);
    AddNonSIUnitToLibrary(kSIQuantityDimensionless, STR("part per trillion"), STR("parts per trillion"), STR("ppt"), 1.e-12);
    AddNonSIUnitToLibrary(kSIQuantityDimensionless, STR("part per quadrillion"), STR("parts per quadrillion"), STR("ppq"), 1.e-15);
    AddNonSIUnitToLibrary(kSIQuantityDimensionless, STR("euler number"), STR("euler number"), STR("exp(1)"), kSIEulersNumber);
//
#pragma mark kSIQuantityFineStructureConstant
    AddNonSIUnitToLibrary(kSIQuantityFineStructureConstant, STR("fine structure constant"), STR("fine structure constant"), STR("α"), alpha);
    AddNonSIUnitToLibrary(kSIQuantityFineStructureConstant, STR("inverse fine structure constant"), STR("inverse fine structure constant"), STR("(1/α)"), 1 / alpha);
//
#pragma mark kSIQuantityLength
    AddSIBaseUnitToLibrary(kSIQuantityLength);
    AddNonSIUnitToLibrary(kSIQuantityLength, STR("astronomical unit"), STR("astronomical units"), STR("ua"), 1.49597870691e11);
    AddNonSIUnitToLibrary(kSIQuantityLength, STR("light year"), STR("light years"), STR("ly"), lightYear);
    AddNonSIUnitToLibrary(kSIQuantityLength, STR("ångström"), STR("ångströms"), STR("Å"), 1.e-10);
    AddNonSIUnitToLibrary(kSIQuantityLength, STR("atomic unit of length"), STR("atomic unit of length"), STR("a_0"), a_0);
    //  AddNonSIUnitToLibrary(kSIQuantityLength, STR("nautical mile"), STR("nautical miles"), STR("M"),1852.);
    AddNonSIUnitToLibrary(kSIQuantityLength, STR("fathom"), STR("fathoms"), STR("ftm"), 2 * 1609.344 / 1760);
    AddNonSIUnitToLibrary(kSIQuantityLength, STR("inch"), STR("inches"), STR("in"), 1609.344 / 63360);
    AddNonSIUnitToLibrary(kSIQuantityLength, STR("foot"), STR("feet"), STR("ft"), 1609.344 / 5280);
    AddNonSIUnitToLibrary(kSIQuantityLength, STR("yard"), STR("yards"), STR("yd"), 1609.344 / 1760);
    AddNonSIUnitToLibrary(kSIQuantityLength, STR("mile"), STR("miles"), STR("mi"), 1609.344);
    AddNonSIUnitToLibrary(kSIQuantityLength, STR("link"), STR("links"), STR("li"), 1609.344 / 5280 * 33 / 50);
    AddNonSIUnitToLibrary(kSIQuantityLength, STR("rod"), STR("rods"), STR("rod"), 1609.344 / 5280 * 16.5);
    AddNonSIUnitToLibrary(kSIQuantityLength, STR("chain"), STR("chains"), STR("ch"), 1609.344 / 5280 * 16.5 * 4);
    AddNonSIUnitToLibrary(kSIQuantityLength, STR("furlong"), STR("furlongs"), STR("fur"), 1609.344 / 5280 * 16.5 * 4 * 10);
    AddNonSIUnitToLibrary(kSIQuantityLength, STR("league"), STR("leagues"), STR("lea"), 1609.344 * 3);
    AddNonSIUnitToLibrary(kSIQuantityLength, STR("compton wavelength"), STR("compton wavelengths"), STR("λ_C"), kSIPlanckConstant / (kSIElectronMass * kSISpeedOfLight));
    AddNonSIUnitToLibrary(kSIQuantityLength, STR("natural unit of length"), STR("natural units of length"), STR("ƛ_C"), 386.15926764e-15);
    AddNonSIUnitToLibrary(kSIQuantityLength, STR("planck length"), STR("planck length"), STR("l_P"), planckLength);
//
#pragma mark kSIQuantityWavenumber, kSIQuantityInverseLength
    AddNonSIUnitToLibrary(kSIQuantityWavenumber, STR("inverse meter"), STR("inverse meters"), STR("(1/m)"), 1);
    AddNonSIUnitToLibrary(kSIQuantityWavenumber, STR("inverse centimeter"), STR("inverse centimeters"), STR("(1/cm)"), 100);
    AddNonSIUnitToLibrary(kSIQuantityWavenumber, STR("inverse ångström"), STR("inverse ångströms"), STR("(1/Å)"), 1.e10);
    AddNonSIUnitToLibrary(kSIQuantityWavenumber, STR("inverse mile"), STR("inverse miles"), STR("(1/mi)"), 1. / 1609.344);
    AddNonSIUnitToLibrary(kSIQuantityWavenumber, STR("inverse yard"), STR("inverse yards"), STR("(1/yd)"), 1. / (1609.344 / 1760));
    AddNonSIUnitToLibrary(kSIQuantityWavenumber, STR("inverse foot"), STR("inverse feet"), STR("(1/ft)"), 1. / (1609.344 / 5280));
    AddNonSIUnitToLibrary(kSIQuantityWavenumber, STR("inverse inch"), STR("inverse inches"), STR("(1/in)"), 1. / (1609.344 / 63360));
    AddNonSIUnitToLibrary(kSIQuantityWavenumber, STR("rydberg constant"), STR("rydberg constant"), STR("R_∞"), R_H);
    AddNonSIUnitToLibrary(kSIQuantityWavenumber, STR("inverse atomic unit of length"), STR("inverse atomic unit of length"), STR("(1/a_0)"), 1 / a_0);
    OCMutableArrayRef units = (OCMutableArrayRef)OCDictionaryGetValue(unitsQuantitiesLibrary, kSIQuantityWavenumber);
    OCDictionaryAddValue(unitsQuantitiesLibrary, kSIQuantityInverseLength, units);
//
#pragma mark kSIQuantityLengthRatio, kSIQuantityPlaneAngle
    AddSpecialSIUnit(kSIQuantityLengthRatio, STR("meter per meter"), STR("meters per meter"), STR("m/m"));
    AddSpecialSIUnit(kSIQuantityLengthRatio, STR("radian"), STR("radians"), STR("rad"));
    AddNonSIUnitToLibrary(kSIQuantityLengthRatio, STR("degree"), STR("degrees"), STR("°"), kSIPi / 180.);
    AddNonSIUnitToLibrary(kSIQuantityLengthRatio, STR("pi"), STR("pi"), STR("π"), kSIPi);
    units = (OCMutableArrayRef)OCDictionaryGetValue(unitsQuantitiesLibrary, kSIQuantityLengthRatio);
    OCDictionaryAddValue(unitsQuantitiesLibrary, kSIQuantityPlaneAngle, units);
//
#pragma mark kSIQuantityMass
    AddSIBaseUnitToLibrary(kSIQuantityMass);
    AddNonSIUnitToLibrary(kSIQuantityMass, STR("microgram"), STR("micrograms"), STR("mcg"), 1e-9);
    AddNonSIUnitToLibrary(kSIQuantityMass, STR("tonne"), STR("tonnes"), STR("t"), 1e3);
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantityMass, STR("dalton"), STR("daltons"), STR("Da"), kSIAtomicMassConstant);
    AddNonSIUnitToLibrary(kSIQuantityMass, STR("atomic mass unit"), STR("atomic mass units"), STR("u"), kSIAtomicMassConstant);
    AddNonSIUnitToLibrary(kSIQuantityMass, STR("atomic mass constant"), STR("atomic mass constant"), STR("m_u"), kSIAtomicMassConstant);
    AddNonSIUnitToLibrary(kSIQuantityMass, STR("electron mass"), STR("electron mass"), STR("m_e"), kSIElectronMass);
    AddNonSIUnitToLibrary(kSIQuantityMass, STR("proton mass"), STR("proton mass"), STR("m_p"), kSIProtonMass);
    AddNonSIUnitToLibrary(kSIQuantityMass, STR("neutron mass"), STR("neutron mass"), STR("m_n"), kSINeutronMass);
    AddNonSIUnitToLibrary(kSIQuantityMass, STR("alpha particle mass"), STR("alpha particle mass"), STR("m_a"), kSIAlphaParticleMass);
    AddNonSIUnitToLibrary(kSIQuantityMass, STR("muon mass"), STR("muon mass"), STR("m_µ"), kSIMuonMass);
    AddNonSIUnitToLibrary(kSIQuantityMass, STR("ton"), STR("tons"), STR("ton"), 0.45359237 * 2000);
    AddNonSIUnitToLibrary(kSIQuantityMass, STR("hundredweight"), STR("hundredweight"), STR("cwt"), 0.45359237 * 100);
    AddNonSIUnitToLibrary(kSIQuantityMass, STR("pound"), STR("pounds"), STR("lb"), 0.45359237);
    AddNonSIUnitToLibrary(kSIQuantityMass, STR("stone"), STR("stones"), STR("st"), 6.35029318);
    AddNonSIUnitToLibrary(kSIQuantityMass, STR("ounce"), STR("ounces"), STR("oz"), 0.028349523125);
    AddNonSIUnitToLibrary(kSIQuantityMass, STR("grain"), STR("grains"), STR("gr"), 0.45359237 / 7000);
    AddNonSIUnitToLibrary(kSIQuantityMass, STR("dram"), STR("drams"), STR("dr"), 0.45359237 / 256);
    AddNonSIUnitToLibrary(kSIQuantityMass, STR("tonUK"), STR("tonsUK"), STR("tonUK"), 0.45359237 * 2240);
    AddNonSIUnitToLibrary(kSIQuantityMass, STR("hundredweightUK"), STR("hundredweightUK"), STR("cwtUK"), 0.45359237 * 112);
    AddNonSIUnitToLibrary(kSIQuantityMass, STR("planck mass"), STR("planck mass"), STR("m_P"), planckMass);
//
#pragma mark kSIQuantityInverseMass
    AddNonSIUnitToLibrary(kSIQuantityInverseMass, STR("inverse kilogram"), STR("inverse kilograms"), STR("(1/kg)"), 1.);
    AddNonSIUnitToLibrary(kSIQuantityInverseMass, STR("inverse tonne"), STR("inverse tonnes"), STR("(1/t)"), 1. / 1e3);
    AddNonSIUnitToLibrary(kSIQuantityInverseMass, STR("inverse ton"), STR("inverse tons"), STR("(1/ton)"), 1. / (0.45359237 * 2000));
    AddNonSIUnitToLibrary(kSIQuantityInverseMass, STR("inverse stone"), STR("inverse stones"), STR("(1/st)"), 1. / 6.35029318);
    AddNonSIUnitToLibrary(kSIQuantityInverseMass, STR("inverse pound"), STR("inverse pounds"), STR("(1/lb)"), 1. / 0.45359237);
    AddNonSIUnitToLibrary(kSIQuantityInverseMass, STR("inverse ounce"), STR("inverse ounces"), STR("(1/oz)"), 1. / 0.028349523125);
//
#pragma mark kSIQuantityMassRatio
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantityMassRatio, STR("gram per kilogram"), STR("grams per kilogram"), STR("g/kg"), 0.001);
//
#pragma mark kSIQuantityTime
    AddSIBaseUnitToLibrary(kSIQuantityTime);
    AddNonSIUnitToLibrary(kSIQuantityTime, STR("minute"), STR("minutes"), STR("min"), kSIMinute);
    AddNonSIUnitToLibrary(kSIQuantityTime, STR("hour"), STR("hours"), STR("h"), kSIHour);
    AddNonSIUnitToLibrary(kSIQuantityTime, STR("day"), STR("days"), STR("d"), kSIDay);
    AddNonSIUnitToLibrary(kSIQuantityTime, STR("week"), STR("weeks"), STR("wk"), kSIWeek);
    AddNonSIUnitToLibrary(kSIQuantityTime, STR("month"), STR("months"), STR("mo"), kSIMonth);
    AddNonSIUnitToLibrary(kSIQuantityTime, STR("year"), STR("years"), STR("yr"), kSIYear);
    AddNonSIUnitToLibrary(kSIQuantityTime, STR("decade"), STR("decades"), STR("dayr"), kSIDecade);
    AddNonSIUnitToLibrary(kSIQuantityTime, STR("century"), STR("centuries"), STR("hyr"), kSICentury);
    AddNonSIUnitToLibrary(kSIQuantityTime, STR("millennium"), STR("millennia"), STR("kyr"), kSIMillennium);
    AddNonSIUnitToLibrary(kSIQuantityTime, STR("atomic unit of time"), STR("atomic units of time"), STR("ℏ/E_h"), hbar / E_h);
    AddNonSIUnitToLibrary(kSIQuantityTime, STR("natural unit of time"), STR("natural units of time"), STR("ℏ/(m_e•c_0^2)"), hbar / (kSIElectronMass * kSISpeedOfLight * kSISpeedOfLight));
    AddNonSIUnitToLibrary(kSIQuantityTime, STR("planck time"), STR("planck time"), STR("t_P"), planckTime);
//
#pragma mark kSIQuantityInverseTime
    AddNonSIUnitToLibrary(kSIQuantityInverseTime, STR("inverse seconds"), STR("inverse seconds"), STR("(1/s)"), 1.);
    AddNonSIUnitToLibrary(kSIQuantityInverseTime, STR("inverse minute"), STR("inverse minutes"), STR("(1/min)"), 1. / 60.);
    AddNonSIUnitToLibrary(kSIQuantityInverseTime, STR("inverse hour"), STR("inverse hours"), STR("(1/h)"), 1. / (60. * 60.));
    AddNonSIUnitToLibrary(kSIQuantityInverseTime, STR("inverse day"), STR("inverse days"), STR("(1/d)"), 1. / (60. * 60 * 24.));
    AddNonSIUnitToLibrary(kSIQuantityInverseTime, STR("inverse week"), STR("inverse weeks"), STR("(1/wk)"), 1. / (60. * 60 * 24. * 7.));
    AddNonSIUnitToLibrary(kSIQuantityInverseTime, STR("inverse month"), STR("inverse months"), STR("(1/month)"), 1. / (365.25 * 86400 / 12.));
    AddNonSIUnitToLibrary(kSIQuantityInverseTime, STR("inverse year"), STR("inverse years"), STR("(1/yr)"), 1. / (365.25 * 86400));
//
#pragma mark kSIQuantityFrequency
    AddSpecialSIUnit(kSIQuantityFrequency, STR("hertz"), STR("hertz"), STR("Hz"));
//
#pragma mark kSIQuantityRadioactivity
    AddSpecialSIUnit(kSIQuantityRadioactivity, STR("becquerel"), STR("becquerels"), STR("Bq"));
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantityRadioactivity, STR("curie"), STR("curies"), STR("Ci"), 3.7e10);
//
#pragma mark kSIQuantityFrequencyRatio
    AddSpecialSIUnit(kSIQuantityFrequencyRatio, STR("hertz per hertz"), STR("hertz per hertz"), STR("Hz/Hz"));
//
#pragma mark kSIQuantityTimeRatio
    AddSpecialSIUnit(kSIQuantityTimeRatio, STR("second per second"), STR("seconds per second"), STR("s/s"));
//
#pragma mark kSIQuantityInverseTimeSquared
    AddNonSIUnitToLibrary(kSIQuantityInverseTimeSquared, STR("inverse millisecond squared"), STR("inverse milliseconds squared"), STR("(1/ms^2)"), 1000000.);
    AddNonSIUnitToLibrary(kSIQuantityInverseTimeSquared, STR("inverse second squared"), STR("inverse seconds squared"), STR("(1/s^2)"), 1.);
    AddNonSIUnitToLibrary(kSIQuantityInverseTimeSquared, STR("inverse hour inverse second"), STR("inverse hour inverse seconds"), STR("(1/(h•s))"), 1. / 3600.);
    AddNonSIUnitToLibrary(kSIQuantityInverseTimeSquared, STR("inverse hour inverse minute"), STR("inverse hour inverse minutes"), STR("(1/(h•min))"), 1. / 3600. / 60.);
//
#pragma mark kSIQuantityLinearMomentum
    AddNonSIUnitToLibrary(kSIQuantityLinearMomentum, STR("natural unit of momentum"), STR("natural units of momentum"), STR("m_e•c_0"), kSIElectronMass * kSISpeedOfLight);
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantityLinearMomentum, STR("gram meter per second"), STR("gram meters per second"), STR("m•g/s"), 0.001);
    AddSpecialSIUnit(kSIQuantityLinearMomentum, STR("newton second"), STR("newton seconds"), STR("N•s"));
    AddNonSIUnitToLibrary(kSIQuantityLinearMomentum, STR("atomic unit of momentum"), STR("atomic units of momentum"), STR("ℏ/a_0"), hbar / a_0);
//
#pragma mark kSIQuantityEnergy
    AddNonSIUnitToLibrary(kSIQuantityEnergy, STR("natural unit of energy"), STR("natural units of energy"), STR("m_e•c_0^2"), kSIElectronMass * kSISpeedOfLight * kSISpeedOfLight);
    AddSpecialSIUnit(kSIQuantityEnergy, STR("joule"), STR("joules"), STR("J"));
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantityEnergy, STR("watt hour"), STR("watt hour"), STR("W•h"), 3.6e3);
    AddNonSIUnitToLibrary(kSIQuantityEnergy, STR("rydberg"), STR("rydbergs"), STR("Ry"), Ry);
    AddNonSIUnitToLibrary(kSIQuantityEnergy, STR("alpha particle mass energy"), STR("alpha particle mass energy"), STR("m_a•c_0^2"), kSIAlphaParticleMass * kSISpeedOfLight * kSISpeedOfLight);
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantityEnergy, STR("electronvolt"), STR("electronvolts"), STR("eV"), kSIElementaryCharge);
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantityEnergy, STR("erg"), STR("ergs"), STR("erg"), 1e-7);
    AddNonSIUnitToLibrary(kSIQuantityEnergy, STR("calorie"), STR("calories"), STR("cal"), 4.1868);
    AddNonSIUnitToLibrary(kSIQuantityEnergy, STR("kilocalorie"), STR("kilocalories"), STR("kcal"), 4.1868 * 1000.);
    AddNonSIUnitToLibrary(kSIQuantityEnergy, STR("atomic unit of energy"), STR("atomic unit of energy"), STR("E_h"), E_h);
    AddNonSIUnitToLibrary(kSIQuantityEnergy, STR("british thermal unit"), STR("british thermal units"), STR("Btu"), 1055.05585257348);
//
#pragma mark kSIQuantityTemperature
    AddNonSIUnitToLibrary(kSIQuantityTemperature, STR("planck temperature"), STR("planck temperature"), STR("T_P"), planckTemperature);
    AddSIBaseUnitToLibrary(kSIQuantityTemperature);
    AddNonSIUnitToLibrary(kSIQuantityTemperature, STR("rankine"), STR("rankines"), STR("°R"), 0.555555555555556);
    AddNonSIUnitToLibrary(kSIQuantityTemperature, STR("fahrenheit"), STR("fahrenheit"), STR("°F"), 0.555555555555556);
    AddNonSIUnitToLibrary(kSIQuantityTemperature, STR("celsius"), STR("celsius"), STR("°C"), 1);
//
#pragma mark kSIQuantityCurrent
    AddSIBaseUnitToLibrary(kSIQuantityCurrent);
    AddNonSIUnitToLibrary(kSIQuantityCurrent, STR("atomic unit of current"), STR("atomic unit of current"), STR("q_e•E_h/ℏ"), kSIElementaryCharge * E_h / hbar);
//
#pragma mark kSIQuantityInverseCurrent
    AddNonSIUnitToLibrary(kSIQuantityInverseCurrent, STR("inverse ampere"), STR("inverse amperes"), STR("(1/A)"), 1.);
//
#pragma mark kSIQuantityCurrentRatio
    AddSpecialSIUnit(kSIQuantityCurrentRatio, STR("ampere per ampere"), STR("ampere per ampere"), STR("A/A"));
//
#pragma mark kSIQuantityInverseTemperature
    AddNonSIUnitToLibrary(kSIQuantityInverseTemperature, STR("inverse kelvin"), STR("inverse kelvin"), STR("(1/K)"), 1.);
//
#pragma mark kSIQuantityTemperatureRatio
    AddSpecialSIUnit(kSIQuantityTemperatureRatio, STR("kelvin per kelvin"), STR("kelvin per kelvin"), STR("K/K"));
//
#pragma mark kSIQuantityTemperatureGradient
    AddNonSIUnitToLibrary(kSIQuantityTemperatureGradient, STR("kelvin per meter"), STR("kelvin per meter"), STR("K/m"), 1);
    AddNonSIUnitToLibrary(kSIQuantityTemperatureGradient, STR("celsius per meter"), STR("celsius per meter"), STR("°C/m"), 1);
    AddNonSIUnitToLibrary(kSIQuantityTemperatureGradient, STR("fahrenheit per foot"), STR("fahrenheit per foot"), STR("°F/ft"), 0.555555555555556 / (1609.344 / 5280));
    AddNonSIUnitToLibrary(kSIQuantityTemperatureGradient, STR("rankine per foot"), STR("rankines per foot"), STR("°R/ft"), 0.555555555555556 / (1609.344 / 5280));
//
#pragma mark kSIQuantityAmount
    AddSIBaseUnitToLibrary(kSIQuantityAmount);
//
#pragma mark kSIQuantityInverseAmount
    AddNonSIUnitToLibrary(kSIQuantityInverseAmount, STR("inverse mole"), STR("inverse moles"), STR("(1/mol)"), 1.);
    AddNonSIUnitToLibrary(kSIQuantityInverseAmount, STR("avogadro constant"), STR("avogadro constant"), STR("N_A"), kSIAvogadroConstant);
//
#pragma mark kSIQuantityAmountRatio
    AddSpecialSIUnit(kSIQuantityAmountRatio, STR("mole per mole"), STR("moles per mole"), STR("mol/mol"));
//
#pragma mark kSIQuantityLuminousIntensity
    AddSIBaseUnitToLibrary(kSIQuantityLuminousIntensity);
//
#pragma mark kSIQuantityInverseLuminousIntensity
    AddNonSIUnitToLibrary(kSIQuantityInverseLuminousIntensity, STR("inverse candela"), STR("inverse candelas"), STR("(1/cd)"), 1.);
//
#pragma mark kSIQuantityLuminousIntensityRatio
    AddSpecialSIUnit(kSIQuantityLuminousIntensityRatio, STR("candela per candela"), STR("candelas per candela"), STR("cd/cd"));
//
#pragma mark kSIQuantityArea
    AddNonSIUnitToLibrary(kSIQuantityArea, STR("hectare"), STR("hectares"), STR("ha"), 1e4);
    AddNonSIUnitToLibrary(kSIQuantityArea, STR("barn"), STR("barns"), STR("b"), 1.e-28);
    AddNonSIUnitToLibrary(kSIQuantityArea, STR("square meter"), STR("square meters"), STR("m^2"), 1);
    AddNonSIUnitToLibrary(kSIQuantityArea, STR("square centimeter"), STR("square centimeters"), STR("cm^2"), 0.0001);
    AddNonSIUnitToLibrary(kSIQuantityArea, STR("square kilometer"), STR("square kilometers"), STR("km^2"), 1000000);
    AddNonSIUnitToLibrary(kSIQuantityArea, STR("square inch"), STR("square inches"), STR("in^2"), 0.00064516);
    AddNonSIUnitToLibrary(kSIQuantityArea, STR("square foot"), STR("square feet"), STR("ft^2"), 0.09290304);
    AddNonSIUnitToLibrary(kSIQuantityArea, STR("square yard"), STR("square yards"), STR("yd^2"), 0.83612736);
    AddNonSIUnitToLibrary(kSIQuantityArea, STR("square mile"), STR("square miles"), STR("mi^2"), 2589988.110336);
    AddNonSIUnitToLibrary(kSIQuantityArea, STR("square rod"), STR("square rods"), STR("rod^2"), 5.029210 * 5.029210);
    AddNonSIUnitToLibrary(kSIQuantityArea, STR("square chain"), STR("square chains"), STR("ch^2"), 5.029210 * 5.029210 * 16);
    AddNonSIUnitToLibrary(kSIQuantityArea, STR("acre"), STR("acres"), STR("ac"), 4046.8564224);
    AddNonSIUnitToLibrary(kSIQuantityArea, STR("township"), STR("townships"), STR("twp"), 2589988.110336 * 36.);
//
#pragma mark kSIQuantityInverseArea
    AddNonSIUnitToLibrary(kSIQuantityInverseArea, STR("inverse hectare"), STR("inverse hectares"), STR("(1/ha)"), 1e-4);
    AddNonSIUnitToLibrary(kSIQuantityInverseArea, STR("inverse barn"), STR("inverse barns"), STR("(1/b)"), 1.e28);
    AddNonSIUnitToLibrary(kSIQuantityInverseArea, STR("inverse square kilometer"), STR("inverse square kilometer"), STR("(1/km^2)"), 1. / 1000000);
    AddNonSIUnitToLibrary(kSIQuantityInverseArea, STR("inverse square centimeter"), STR("inverse square centimeters"), STR("(1/cm^2)"), 1. / 0.0001);
    AddNonSIUnitToLibrary(kSIQuantityInverseArea, STR("inverse square inch"), STR("inverse square inches"), STR("(1/in^2)"), 1. / 0.00064516);
    AddNonSIUnitToLibrary(kSIQuantityInverseArea, STR("inverse square foot"), STR("inverse square feet"), STR("(1/ft^2)"), 1. / 0.09290304);
    AddNonSIUnitToLibrary(kSIQuantityInverseArea, STR("inverse square yard"), STR("inverse square yards"), STR("(1/yd^2)"), 1. / 0.83612736);
    AddNonSIUnitToLibrary(kSIQuantityInverseArea, STR("inverse acre"), STR("inverse acres"), STR("(1/ac)"), 1. / 4046.8564224);
    AddNonSIUnitToLibrary(kSIQuantityInverseArea, STR("inverse square mile"), STR("inverse square miles"), STR("(1/mi^2)"), 1. / 2589988.110336);
//
#pragma mark kSIQuantityRockPermeability
    AddNonSIUnitToLibrary(kSIQuantityRockPermeability, STR("darcy"), STR("darcys"), STR("Dc"), 9.869233e-13);
    AddNonSIUnitToLibrary(kSIQuantityRockPermeability, STR("millidarcy"), STR("millidarcys"), STR("mDc"), 9.869233e-16);
    AddNonSIUnitToLibrary(kSIQuantityRockPermeability, STR("microdarcy"), STR("microdarcys"), STR("µDc"), 9.869233e-19);
    AddNonSIUnitToLibrary(kSIQuantityRockPermeability, STR("nanodarcy"), STR("nanodarcys"), STR("nDc"), 9.869233e-21);
//
#pragma mark kSIQuantitySolidAngle
    AddSpecialSIUnit(kSIQuantitySolidAngle, STR("steradian"), STR("steradians"), STR("sr"));
//
#pragma mark kSIQuantityAreaRatio
    AddNonSIUnitToLibrary(kSIQuantityAreaRatio, STR("square meter per square meter"), STR("square meters per square meter"), STR("m^2/m^2"), 1);
//
#pragma mark kSIQuantityVolume
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantityVolume, STR("liter"), STR("liters"), STR("L"), 1e-3);
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("cubic inch"), STR("cubic inches"), STR("in^3"), (1609.344 / 63360) * (1609.344 / 63360) * (1609.344 / 63360));
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("cubic foot"), STR("cubic feet"), STR("ft^3"), (1609.344 / 5280) * (1609.344 / 5280) * (1609.344 / 5280));
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("cubic yard"), STR("cubic yards"), STR("yd^3"), (1609.344 / 1760) * (1609.344 / 1760) * (1609.344 / 1760));
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("acre foot"), STR("acre feet"), STR("ac•ft"), 1609.344 / 5280 * 4046.8564224);
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("oil barrel"), STR("oil barrels"), STR("bbl"), 0.158987295);
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("thousand oil barrels"), STR("thousand oil barrels"), STR("Mbbl"), 0.158987295e3);
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("million oil barrels"), STR("million oil barrels"), STR("MMbbl"), 0.158987295e6);
//
#pragma mark kSIQuantityInverseVolume
    AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse liter"), STR("inverse liters"), STR("(1/L)"), 1. / 1e-3);
    AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse milliliter"), STR("inverse milliliters"), STR("(1/mL)"), 1. / 1e-6);
    AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse cubic inch"), STR("inverse cubic inches"), STR("(1/in^3)"), 1. / ((1609.344 / 63360) * (1609.344 / 63360) * (1609.344 / 63360)));
    AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse cubic foot"), STR("inverse cubic feet"), STR("(1/ft^3)"), 1. / ((1609.344 / 5280) * (1609.344 / 5280) * (1609.344 / 5280)));
    AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse cubic yard"), STR("inverse cubic yards"), STR("(1/yd^3)"), 1. / ((1609.344 / 1760) * (1609.344 / 1760) * (1609.344 / 1760)));
    AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse cubic centimeter"), STR("inverse cubic centimeters"), STR("(1/cm^3)"), 1000000.);
    AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse cubic millimeter"), STR("inverse cubic millimeters"), STR("(1/mm^3)"), 1000000000.);
    AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse cubic micrometer"), STR("inverse cubic micrometers"), STR("(1/µm^3)"), 1e+18);
    AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse cubic ångström"), STR("inverse cubic ångströms"), STR("(1/Å^3)"), 1e+30);
//
#pragma mark kSIQuantityVolumeRatio
    AddNonSIUnitToLibrary(kSIQuantityVolumeRatio, STR("cubic meter per cubic meter"), STR("cubic meters per cubic meter"), STR("m^3/m^3"), 1);
//
#pragma mark kSIQuantitySurfaceAreaToVolumeRatio
    AddNonSIUnitToLibrary(kSIQuantitySurfaceAreaToVolumeRatio, STR("square meter per cubic meter"), STR("square meters per cubic meter"), STR("m^2/m^3"), 1);
    AddNonSIUnitToLibrary(kSIQuantitySurfaceAreaToVolumeRatio, STR("square meter per liter"), STR("square meters per liter"), STR("m^2/L"), 1000);
//
#pragma mark kSIQuantitySpeed
    AddSpecialSIUnit(kSIQuantitySpeed, STR("meter per second"), STR("meters per second"), STR("m/s"));
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantitySpeed, STR("meter per minute"), STR("meters per minute"), STR("m/min"), 1. / 60.);
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantitySpeed, STR("meter per hour"), STR("meters per hour"), STR("m/h"), 1. / 3600.);
    AddNonSIUnitToLibrary(kSIQuantitySpeed, STR("knot"), STR("knots"), STR("kn"), 0.514444444444444);
    AddNonSIUnitToLibrary(kSIQuantitySpeed, STR("speed of light"), STR("speed of light"), STR("c_0"), kSISpeedOfLight);
    AddNonSIUnitToLibrary(kSIQuantitySpeed, STR("inch per second"), STR("inches per second"), STR("in/s"), 1609.344 / 63360);
    AddNonSIUnitToLibrary(kSIQuantitySpeed, STR("inch per minute"), STR("inches per minute"), STR("in/min"), 1609.344 / 63360 / 60.);
    AddNonSIUnitToLibrary(kSIQuantitySpeed, STR("inch per hour"), STR("inches per hour"), STR("in/h"), 1609.344 / 63360 / 3600.);
    AddNonSIUnitToLibrary(kSIQuantitySpeed, STR("foot per second"), STR("feet per second"), STR("ft/s"), 1609.344 / 5280);
    AddNonSIUnitToLibrary(kSIQuantitySpeed, STR("foot per minute"), STR("feet per minute"), STR("ft/min"), 1609.344 / 5280 / 60.);
    AddNonSIUnitToLibrary(kSIQuantitySpeed, STR("foot per hour"), STR("feet per hour"), STR("ft/h"), 1609.344 / 5280 / 3600.);
    AddNonSIUnitToLibrary(kSIQuantitySpeed, STR("mile per second"), STR("miles per second"), STR("mi/s"), 1609.344);
    AddNonSIUnitToLibrary(kSIQuantitySpeed, STR("mile per minute"), STR("miles per minute"), STR("mi/min"), 1609.344 / 60.);
    AddNonSIUnitToLibrary(kSIQuantitySpeed, STR("mile per hour"), STR("miles per hour"), STR("mi/h"), 1609.344 / 3600.);
    AddNonSIUnitToLibrary(kSIQuantitySpeed, STR("atomic unit of velocity"), STR("atomic units of velocity"), STR("a_0•E_h/ℏ"), a_0 * E_h / hbar);
    units = (OCMutableArrayRef)OCDictionaryGetValue(unitsQuantitiesLibrary, kSIQuantitySpeed);
    OCDictionaryAddValue(unitsQuantitiesLibrary, kSIQuantityVelocity, units);
//
#pragma mark kSIQuantityAngularMomentum
    AddSpecialSIUnit(kSIQuantityAngularMomentum, STR("joule second"), STR("joules second"), STR("J•s"));
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantityAngularMomentum, STR("gram square meter per second"), STR("gram square meters per second"), STR("g•m^2/s"), 0.001);
//
#pragma mark kSIQuantityAction
    AddNonSIUnitToLibrary(kSIQuantityAction, STR("planck constant"), STR("planck constant"), STR("h_P"), kSIPlanckConstant);
//
#pragma mark kSIQuantityReducedAction
    AddNonSIUnitToLibrary(kSIQuantityReducedAction, STR("reduced planck constant"), STR("reduced planck constant"), STR("ℏ"), hbar);
//
#pragma mark kSIQuantityCirculation
    AddNonSIUnitToLibrary(kSIQuantityCirculation, STR("quantum of circulation"), STR("quantum of circulation"), STR("h_P/(2•m_e)"), hbar);
//
#pragma mark kSIQuantitySecondRadiationConstant
    AddNonSIUnitToLibrary(kSIQuantitySecondRadiationConstant, STR("second radiation constant"), STR("second radiation constant"), STR("h_P•c_0/k_B"), kSIPlanckConstant * kSISpeedOfLight / kSIBoltmannConstant);
//
#pragma mark kSIQuantityElectricResistance
    AddNonSIUnitToLibrary(kSIQuantityElectricResistance, STR("von klitzing constant"), STR("von klitzing constant"), STR("h_P/(q_e^2)"), kSIPlanckConstant / (kSIElementaryCharge * kSIElementaryCharge));
    AddSpecialSIUnit(kSIQuantityElectricResistance, STR("ohm"), STR("ohms"), STR("Ω"));
    AddNonSIUnitToLibrary(kSIQuantityElectricResistance, STR("inverse conductance quantum"), STR("inverse conductance quantum"), STR("(1/G_0)"), 1 / G_0);
    AddNonSIUnitToLibrary(kSIQuantityElectricResistance, STR("characteristic impedance of vacuum"), STR("characteristic impedance of vacuum"), STR("Z_0"), 4 * kSIPi * 1.e-7 * kSISpeedOfLight);
//
#pragma mark kSIQuantityAcceleration
    AddSpecialSIUnit(kSIQuantityAcceleration, STR("meter per square second"), STR("meters per square second"), STR("m/s^2"));
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantityAcceleration, STR("meter per hour per second"), STR("meters per hour per second"), STR("m/(h•s)"), 1. / 3600.);
    AddNonSIUnitToLibrary(kSIQuantityAcceleration, STR("gravity acceleration"), STR("gravity acceleration"), STR("g_0"), kSIGravityAcceleration);
    AddNonSIUnitToLibrary(kSIQuantityAcceleration, STR("mile per square second"), STR("miles per square second"), STR("mi/s^2"), 1609.344);
    AddNonSIUnitToLibrary(kSIQuantityAcceleration, STR("foot per square second"), STR("feet per square second"), STR("ft/s^2"), 1609.344 / 5280);
    AddNonSIUnitToLibrary(kSIQuantityAcceleration, STR("inch per square second"), STR("inches per square second"), STR("in/s^2"), 1609.344 / 63360);
    AddNonSIUnitToLibrary(kSIQuantityAcceleration, STR("mile per square minute"), STR("miles per square minute"), STR("mi/min^2"), 1609.344 / 60. / 60.);
    AddNonSIUnitToLibrary(kSIQuantityAcceleration, STR("foot per square minute"), STR("feet per square minute"), STR("ft/min^2"), 1609.344 / 5280 / 60. / 60.);
    AddNonSIUnitToLibrary(kSIQuantityAcceleration, STR("inch per square minute"), STR("inches per square minute"), STR("in/min^2"), 1609.344 / 63360 / 60. / 60.);
    AddNonSIUnitToLibrary(kSIQuantityAcceleration, STR("mile per hour per second"), STR("miles per hour per second"), STR("mi/(h•s)"), 1609.344 / 60. / 60.);
    AddNonSIUnitToLibrary(kSIQuantityAcceleration, STR("knot per second"), STR("knots per second"), STR("kn/s"), 0.514444444444444);
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantityAcceleration, STR("galileo"), STR("galileo"), STR("Gal"), 1e-2);
//
#pragma mark kSIQuantityDensity
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantityDensity, STR("gram per cubic meter"), STR("grams per cubic meter"), STR("g/m^3"), 1e-3);
//
#pragma mark kSIQuantityMassFlowRate
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantityMassFlowRate, STR("gram per second"), STR("grams per second"), STR("g/s"), 1e-3);
//
#pragma mark kSIQuantityMassFlux
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantityMassFlux, STR("gram per square meter per second"), STR("grams per square meter per second"), STR("g/(m^2•s)"), 1e-3);
//
#pragma mark kSIQuantitySurfaceDensity
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantitySurfaceDensity, STR("gram per square meter"), STR("grams per square meter"), STR("g/m^2"), 1e-3);
//
#pragma mark kSIQuantityCurrentDensity
    AddSpecialSIUnit(kSIQuantityCurrentDensity, STR("ampere per square meter"), STR("amperes per square meter"), STR("A/m^2"));
//
#pragma mark kSIQuantityAmountConcentration
    AddSpecialSIUnit(kSIQuantityAmountConcentration, STR("mole per cubic meter"), STR("moles per cubic meter"), STR("mol/m^3"));
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantityAmountConcentration, STR("mole per liter"), STR("moles per liter"), STR("mol/L"), 1000.);
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantityAmountConcentration, STR("mole per milliliter"), STR("moles per milliliter"), STR("mol/mL"), 1000000.);
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantityAmountConcentration, STR("mole per microliter"), STR("moles per microliter"), STR("mol/µL"), 1000000000.);
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantityAmountConcentration, STR("mole per liter"), STR("moles per liter"), STR("M"), 1000.);
//
#pragma mark kSIQuantityMassConcentration
    AddSpecialSIUnit(kSIQuantityMassConcentration, STR("gram per liter"), STR("grams per liter"), STR("g/L"));
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantityMassConcentration, STR("gram per milliliter"), STR("grams per milliliter"), STR("g/mL"), 1e3);
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantityMassConcentration, STR("gram per cubic centimeter"), STR("grams per cubic centimeter"), STR("g/cm^3"), 1e3);
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantityMassConcentration, STR("gram per microliter"), STR("grams per microliter"), STR("g/µL"), 1e6);
//
#pragma mark kSIQuantityForce
    AddSpecialSIUnit(kSIQuantityForce, STR("newton"), STR("newtons"), STR("N"));
    AddNonSIUnitToLibrary(kSIQuantityForce, STR("atomic unit of force"), STR("atomic units of force"), STR("E_h/a_0"), E_h / a_0);
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantityForce, STR("dyne"), STR("dynes"), STR("dyn"), 1e-5);
    AddNonSIUnitToLibrary(kSIQuantityForce, STR("pound force"), STR("pounds force"), STR("lbf"), 4.4482216152605);
    AddNonSIUnitToLibrary(kSIQuantityForce, STR("ounce force"), STR("ounces force"), STR("ozf"), 4.4482216152605 / 16.);
    AddNonSIUnitToLibrary(kSIQuantityForce, STR("kilogram force"), STR("kilograms force"), STR("kgf"), 9.80665);
//
#pragma mark kSIQuantityTorque
    AddSpecialSIUnit(kSIQuantityTorque, STR("newton meter per radian"), STR("newton meters per radian"), STR("N•m/rad"));
    AddSpecialSIUnit(kSIQuantityTorque, STR("joule per radian"), STR("joules per radian"), STR("J/rad"));
    AddNonSIUnitToLibrary(kSIQuantityTorque, STR("pound force foot per radian"), STR("pound force feet per radian"), STR("lbf•ft/rad"), 1.3558179483314);
    AddNonSIUnitToLibrary(kSIQuantityTorque, STR("pound force inch per radian"), STR("pound force inches per radian"), STR("lbf•in/rad"), 1.3558179483314 / 12.);
    AddNonSIUnitToLibrary(kSIQuantityTorque, STR("kilogram force meter per radian"), STR("kilogram force meters per radian"), STR("kgf•m/rad"), 9.80665);
    AddNonSIUnitToLibrary(kSIQuantityTorque, STR("kilogram force centimeter per radian"), STR("kilogram force centimeters per radian"), STR("kgf•cm/rad"), 0.0980665);
//
#pragma mark kSIQuantityMomentOfInertia
    AddNonSIUnitToLibrary(kSIQuantityMomentOfInertia, STR("meter squared kilogram"), STR("meters squared kilogram"), STR("m^2•kg"), 1);
    AddNonSIUnitToLibrary(kSIQuantityMomentOfInertia, STR("meter squared gram"), STR("meters squared gram"), STR("m^2•g"), 1.e-3);
    AddNonSIUnitToLibrary(kSIQuantityMomentOfInertia, STR("centimeter squared kilogram"), STR("centimeter squared kilogram"), STR("cm^2•kg"), 0.0001);
    AddNonSIUnitToLibrary(kSIQuantityMomentOfInertia, STR("centimeter squared gram"), STR("centimeter squared gram"), STR("cm^2•g"), 1.e-7);
//
#pragma mark kSIQuantityPressure, kSIQuantityStress, kSIQuantityElasticModulus
    AddSpecialSIUnit(kSIQuantityPressure, STR("pascal"), STR("pascals"), STR("Pa"));
    AddNonSIUnitToLibrary(kSIQuantityPressure, STR("pound force per square inch"), STR("pounds force per square inch"), STR("lbf/in^2"), 6894.75729);
    AddNonSIUnitToLibrary(kSIQuantityPressure, STR("pound force per square inch"), STR("pounds force per square inch"), STR("psi"), 6894.75729);
    AddNonSIUnitToLibrary(kSIQuantityPressure, STR("pound force per square foot"), STR("pounds force per square feet"), STR("lbf/ft^2"), 47.880259);
    AddNonSIUnitToLibrary(kSIQuantityPressure, STR("torr"), STR("torrs"), STR("Torr"), 1.01325e5 / 760);
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantityPressure, STR("bar"), STR("bars"), STR("bar"), 1e5);
    AddNonSIUnitToLibrary(kSIQuantityPressure, STR("millimeter of Hg"), STR("millimeters of Hg"), STR("mmHg"), 133.322);
    AddNonSIUnitToLibrary(kSIQuantityPressure, STR("atmosphere"), STR("atmospheres"), STR("atm"), 1.01325e5);
    AddSpecialSIUnit(kSIQuantityPressure, STR("newton per square meter"), STR("newtons per square meter"), STR("N/m^2"));
    units = (OCMutableArrayRef)OCDictionaryGetValue(unitsQuantitiesLibrary, kSIQuantityPressure);
    OCDictionaryAddValue(unitsQuantitiesLibrary, kSIQuantityStress, units);
    OCDictionaryAddValue(unitsQuantitiesLibrary, kSIQuantityElasticModulus, units);
//
#pragma mark kSIQuantityCompressibility
    AddNonSIUnitToLibrary(kSIQuantityCompressibility, STR("inverse pascal"), STR("inverse pascals"), STR("1/Pa"), 1);
//
#pragma mark kSIQuantityStressOpticCoefficient
    AddNonSIUnitToLibrary(kSIQuantityStressOpticCoefficient, STR("brewster"), STR("brewsters"), STR("B"), 1.e-12);
//
#pragma mark kSIQuantityPressureGradient
    AddSpecialSIUnit(kSIQuantityPressureGradient, STR("pascal per meter"), STR("pascals per meter"), STR("Pa/m"));
    AddNonSIUnitToLibrary(kSIQuantityPressureGradient, STR("pound force per square inch per foot"), STR("pounds force per square inch per foot"), STR("psi/ft"), 6894.75729 / (1609.344 / 5280));
//
#pragma mark kSIQuantitySpectralRadiantEnergy
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantitySpectralRadiantEnergy, STR("joule per nanometer"), STR("joules per nanometer"), STR("J/nm"), 1.e9);
//
#pragma mark kSIQuantityPower, kSIQuantityRadiantFlux
    AddSpecialSIUnit(kSIQuantityPower, STR("watt"), STR("watts"), STR("W"));
    AddSpecialSIUnit(kSIQuantityPower, STR("Joule per second"), STR("Joules per second"), STR("J/s"));
    AddNonSIUnitToLibrary(kSIQuantityPower, STR("calorie per second"), STR("calories per second"), STR("cal/s"), 4.1868);
    AddNonSIUnitToLibrary(kSIQuantityPower, STR("calorie per minute"), STR("calories per minute"), STR("cal/min"), 4.1868 / 60.);
    AddNonSIUnitToLibrary(kSIQuantityPower, STR("calorie per hour"), STR("calories per hour"), STR("cal/h"), 4.1868 / 3600.);
    AddNonSIUnitToLibrary(kSIQuantityPower, STR("horsepower"), STR("horsepower"), STR("hp"), 745.699872);
    AddNonSIUnitToLibrary(kSIQuantityPower, STR("british thermal unit per hour"), STR("british thermal unit per hour"), STR("Btu/h"), 1055.05585257348 / 3600.);
    AddNonSIUnitToLibrary(kSIQuantityPower, STR("british thermal unit per minute"), STR("british thermal unit per minute"), STR("Btu/min"), 1055.05585257348 / 60);
    AddNonSIUnitToLibrary(kSIQuantityPower, STR("british thermal unit per second"), STR("british thermal unit per second"), STR("Btu/s"), 1055.05585257348);
    AddNonSIUnitToLibrary(kSIQuantityPower, STR("erg per second"), STR("ergs per second"), STR("erg/s"), 1e-7);
    AddNonSIUnitToLibrary(kSIQuantityPower, STR("foot pound force per hour"), STR("feet pound force per hour"), STR("ft•lbf/h"), (1609.344 / 5280) * 4.4482216152605 / 3600.);
    AddNonSIUnitToLibrary(kSIQuantityPower, STR("foot pound force per minute"), STR("feet pound force per minute"), STR("ft•lbf/min"), (1609.344 / 5280) * 4.4482216152605 / 60.);
    AddNonSIUnitToLibrary(kSIQuantityPower, STR("foot pound force per second"), STR("feet pound force per second"), STR("ft•lbf/s"), (1609.344 / 5280) * 4.4482216152605);
    AddNonSIUnitToLibrary(kSIQuantityPower, STR("inch pound force per hour"), STR("inches pound force per hour"), STR("in•lbf/h"), 1.3558179483314 / 12. / 3600.);
    AddNonSIUnitToLibrary(kSIQuantityPower, STR("inch pound force per minute"), STR("inches pound force per minute"), STR("in•lbf/min"), 1.3558179483314 / 12. / 60.);
    AddNonSIUnitToLibrary(kSIQuantityPower, STR("inch pound force per second"), STR("inches pound force per second"), STR("in•lbf/s"), 1.3558179483314 / 12.);
    units = (OCMutableArrayRef)OCDictionaryGetValue(unitsQuantitiesLibrary, kSIQuantityPower);
    OCDictionaryAddValue(unitsQuantitiesLibrary, kSIQuantityRadiantFlux, units);
//
#pragma mark kSIQuantitySpectralPower
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantitySpectralPower, STR("watt per nanometer"), STR("watts per nanometer"), STR("W/nm"), 1.e9);
//
#pragma mark kSIQuantityVolumePowerDensity
    AddSpecialSIUnit(kSIQuantityVolumePowerDensity, STR("watt per cubic meter"), STR("watts per cubic meter"), STR("W/m^3"));
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantityVolumePowerDensity, STR("watt per cubic centimeter"), STR("watts per cubic centimeter"), STR("W/cm^3"), 100000.);
//
#pragma mark kSIQuantitySpecificPower
    AddSpecialSIUnit(kSIQuantitySpecificPower, STR("watt per kilogram"), STR("watts per kilogram"), STR("W/kg"));
    AddNonSIUnitToLibrary(kSIQuantitySpecificPower, STR("horse power per pound"), STR("horse power per pound"), STR("hp/lb"), 1643.986806920936);
    AddNonSIUnitToLibrary(kSIQuantitySpecificPower, STR("horse power per ounce"), STR("horse power per ounce"), STR("hp/oz"), 26303.78891073498);
//
#pragma mark kSIQuantityElectricCharge, kSIQuantityAmountOfElectricity
    AddSpecialSIUnit(kSIQuantityElectricCharge, STR("coulomb"), STR("coulombs"), STR("C"));
    AddNonSIUnitToLibrary(kSIQuantityElectricCharge, STR("elementary charge"), STR("elementary charge"), STR("q_e"), kSIElementaryCharge);
    units = (OCMutableArrayRef)OCDictionaryGetValue(unitsQuantitiesLibrary, kSIQuantityElectricCharge);
    OCDictionaryAddValue(unitsQuantitiesLibrary, kSIQuantityAmountOfElectricity, units);
//
#pragma mark kSIQuantityElectricPotentialDifference, kSIQuantityElectromotiveForce, kSIQuantityVoltage
    AddSpecialSIUnit(kSIQuantityElectricPotentialDifference, STR("volt"), STR("volts"), STR("V"));
    AddNonSIUnitToLibrary(kSIQuantityElectricPotentialDifference, STR("atomic unit of electric potential"), STR("atomic units of electric potential"), STR("E_h/q_e"), E_h / kSIElementaryCharge);
    units = (OCMutableArrayRef)OCDictionaryGetValue(unitsQuantitiesLibrary, kSIQuantityElectricPotentialDifference);
    OCDictionaryAddValue(unitsQuantitiesLibrary, kSIQuantityElectromotiveForce, units);
    OCDictionaryAddValue(unitsQuantitiesLibrary, kSIQuantityVoltage, units);
//
#pragma mark kSIQuantityElectricFieldGradient
    AddSpecialSIUnit(kSIQuantityElectricFieldGradient, STR("volt per square meter"), STR("volts per square meter"), STR("V/m^2"));
    AddNonSIUnitToLibrary(kSIQuantityElectricFieldGradient, STR("atomic unit of electric field gradient"), STR("atomic unit of electric field gradient"), STR("Λ_0"), Λ_0);
    AddNonSIUnitToLibrary(kSIQuantityElectricFieldGradient, STR("atomic unit of electric field gradient"), STR("atomic unit of electric field gradient"), STR("E_h/(q_e•a_0^2)"), Λ_0);
//
#pragma mark kSIQuantityCapacitance
    AddSpecialSIUnit(kSIQuantityCapacitance, STR("farad"), STR("farads"), STR("F"));
//
#pragma mark kSIQuantityElectricResistancePerLength
    AddSpecialSIUnit(kSIQuantityElectricResistancePerLength, STR("ohm per meter"), STR("ohms per meter"), STR("Ω/m"));
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantityElectricResistancePerLength, STR("ohm per feet"), STR("ohms per feet"), STR("Ω/ft"), 1. / (1609.344 / 5280));
//
#pragma mark kSIQuantityElectricResistivity
    AddSpecialSIUnit(kSIQuantityElectricResistivity, STR("ohm meter"), STR("ohms meter"), STR("Ω•m"));
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantityElectricResistivity, STR("ohm centimeter"), STR("ohms centimeter"), STR("Ω•cm"), 0.01);
//
#pragma mark kSIQuantityElectricConductance
    AddSpecialSIUnit(kSIQuantityElectricConductance, STR("siemen"), STR("siemens"), STR("S"));
    AddNonSIUnitToLibrary(kSIQuantityElectricConductance, STR("conductance quantum"), STR("conductance quantum"), STR("G_0"), G_0);
//
#pragma mark kSIQuantityElectricConductivity
    AddSpecialSIUnit(kSIQuantityElectricConductivity, STR("siemen per meter"), STR("siemens per meter"), STR("S/m"));
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantityElectricConductivity, STR("siemen per centimeter"), STR("siemens per centimeter"), STR("S/cm"), 100.);
//
#pragma mark kSIQuantityMolarConductivity
    AddSpecialSIUnit(kSIQuantityMolarConductivity, STR("siemen meter squared per mole"), STR("siemens meter squared per mole"), STR("S•m^2/mol"));
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantityMolarConductivity, STR("siemen centimeter squared per mole"), STR("siemens centimeter squared per mole"), STR("S•cm^2/mol"), 0.0001);
//
#pragma mark kSIQuantityGyromagneticRatio
    AddSpecialSIUnit(kSIQuantityGyromagneticRatio, STR("radian per second per tesla"), STR("radians per second per tesla"), STR("rad/(s•T)"));
//
#pragma mark kSIQuantityMagneticDipoleMoment
    AddSpecialSIUnit(kSIQuantityMagneticDipoleMoment, STR("ampere square meter"), STR("ampere square meters"), STR("A•m^2"));
    AddSpecialSIUnit(kSIQuantityMagneticDipoleMoment, STR("joule per tesla"), STR("joules per tesla"), STR("J/T"));
    AddNonSIUnitToLibrary(kSIQuantityMagneticDipoleMoment, STR("nuclear magneton"), STR("nuclear magnetons"), STR("µ_N"), mu_N);
    AddNonSIUnitToLibrary(kSIQuantityMagneticDipoleMoment, STR("bohr magneton"), STR("bohr magnetons"), STR("µ_B"), mu_e);
    AddNonSIUnitToLibrary(kSIQuantityMagneticDipoleMoment, STR("proton magnetic moment"), STR("proton magnetic moment"), STR("µ_p"), kSIProtonMagneticMoment);
    AddNonSIUnitToLibrary(kSIQuantityMagneticDipoleMoment, STR("neutron magnetic moment"), STR("neutron magnetic moment"), STR("µ_n"), kSINeutronMagneticMoment);
    AddNonSIUnitToLibrary(kSIQuantityMagneticDipoleMoment, STR("electron magnetic moment"), STR("electron magnetic moment"), STR("µ_e"), kSIElectronMagneticMoment);
    AddNonSIUnitToLibrary(kSIQuantityMagneticDipoleMoment, STR("muon magnetic moment"), STR("muon magnetic moment"), STR("µ_µ"), kSIMuonMagneticMoment);
    AddNonSIUnitToLibrary(kSIQuantityMagneticDipoleMoment, STR("atomic unit of magnetic dipole moment"), STR("atomic units of magnetic dipole moment"), STR("ℏ•q_e/m_e"), hbar * kSIElementaryCharge / kSIElectronMass);
//
#pragma mark kSIQuantityMagneticDipoleMomentRatio
    AddNonSIUnitToLibrary(kSIQuantityMagneticDipoleMomentRatio, STR("proton g factor"), STR("proton g factor"), STR("g_p"), kSIProtonGFactor);
    AddNonSIUnitToLibrary(kSIQuantityMagneticDipoleMomentRatio, STR("neutron g factor"), STR("neutron g factor"), STR("g_n"), kSINeutronGFactor);
    AddNonSIUnitToLibrary(kSIQuantityMagneticDipoleMomentRatio, STR("electron g factor"), STR("electron g factor"), STR("g_e"), kSIElectronGFactor);
    AddNonSIUnitToLibrary(kSIQuantityMagneticDipoleMomentRatio, STR("muon g factor"), STR("muon g factor"), STR("g_µ"), kSIMuonGFactor);
//
#pragma mark kSIQuantityMagneticFlux
    AddSpecialSIUnit(kSIQuantityMagneticFlux, STR("weber"), STR("webers"), STR("Wb"));
    AddNonSIUnitToLibrary(kSIQuantityMagneticFlux, STR("magnetic flux quantum"), STR("magnetic flux quantum"), STR("Φ_0"), kSIPlanckConstant / (2 * kSIElementaryCharge));
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantityMagneticFlux, STR("maxwell"), STR("maxwells"), STR("Mx"), 1e-8);
//
#pragma mark kSIQuantityMagneticFluxDensity
    AddSpecialSIUnit(kSIQuantityMagneticFluxDensity, STR("tesla"), STR("tesla"), STR("T"));
    AddNonSIUnitToLibrary(kSIQuantityMagneticFluxDensity, STR("atomic unit of magnetic flux density"), STR("atomic units of magnetic flux density"), STR("ℏ/(q_e•a_0^2)"), hbar / (kSIElementaryCharge * a_0 * a_0));
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantityMagneticFluxDensity, STR("gauss"), STR("gauss"), STR("G"), 1e-4);
//
#pragma mark kSIQuantityMagneticFieldGradient
    AddSpecialSIUnit(kSIQuantityMagneticFieldGradient, STR("tesla per meter"), STR("tesla per meter"), STR("T/m"));
    AddNonSIUnitToLibrary(kSIQuantityMagneticFieldGradient, STR("tesla per centimeter"), STR("tesla per centimeter"), STR("T/cm"), 100.);
    AddNonSIUnitToLibrary(kSIQuantityMagneticFieldGradient, STR("gauss per centimeter"), STR("gauss per centimeter"), STR("G/cm"), 0.01);
//
#pragma mark kSIQuantityMolarMagneticSusceptibility
    AddNonSIUnitToLibrary(kSIQuantityMolarMagneticSusceptibility, STR("cubic meter per mole"), STR("cubic meters per mole"), STR("m^3/mol"), 1.);
    AddNonSIUnitToLibrary(kSIQuantityMolarMagneticSusceptibility, STR("cubic centimeter per mole"), STR("cubic centimeters per mole"), STR("cm^3/mol"), 1e-06);
//
#pragma mark kSIQuantityInductance
    AddSpecialSIUnit(kSIQuantityInductance, STR("henry"), STR("henries"), STR("H"));
//
#pragma mark kSIQuantityLuminousFlux
    AddSpecialSIUnit(kSIQuantityLuminousFlux, STR("lumen"), STR("lumens"), STR("lm"));
    AddSpecialSIUnit(kSIQuantityLuminousFlux, STR("candela steradian"), STR("candela steradians"), STR("cd•sr"));
//
#pragma mark kSIQuantityLuminousFluxDensity
    AddSpecialSIUnit(kSIQuantityLuminousFluxDensity, STR("lumen per square meter"), STR("lumens per square meter"), STR("lm/m^2"));
    AddNonSIUnitToLibrary(kSIQuantityLuminousFluxDensity, STR("lumen per square foot"), STR("lumens per square foot"), STR("lm/ft^2"), 10.76391041670972);
//
#pragma mark kSIQuantityLuminousEnergy
    AddSpecialSIUnit(kSIQuantityLuminousEnergy, STR("lumen second"), STR("lumen seconds"), STR("lm•s"));
//
#pragma mark kSIQuantityIlluminance
    AddSpecialSIUnit(kSIQuantityIlluminance, STR("lux"), STR("lux"), STR("lx"));
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantityIlluminance, STR("phot"), STR("phots"), STR("ph"), 1e4);
//
#pragma mark kSIQuantityAbsorbedDose
    AddSpecialSIUnit(kSIQuantityAbsorbedDose, STR("gray"), STR("grays"), STR("Gy"));
//
#pragma mark kSIQuantityDoseEquivalent
    AddSpecialSIUnit(kSIQuantityDoseEquivalent, STR("sievert"), STR("sieverts"), STR("Sv"));
//
#pragma mark kSIQuantityCatalyticActivity
    AddSpecialSIUnit(kSIQuantityCatalyticActivity, STR("mole per second"), STR("moles per second"), STR("mol/s"));
    AddSpecialSIUnit(kSIQuantityCatalyticActivity, STR("katal"), STR("katals"), STR("kat"));
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantityCatalyticActivity, STR("mole per minute"), STR("moles per minute"), STR("mol/min"), 1. / 60.);
//
#pragma mark kSIQuantityCatalyticActivityConcentration
    AddSpecialSIUnit(kSIQuantityCatalyticActivityConcentration, STR("katal per cubic meter"), STR("katals per cubic meter"), STR("kat/m^3"));
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantityCatalyticActivityConcentration, STR("katal per liter"), STR("katals per liter"), STR("kat/L"), 1000.);
//
#pragma mark kSIQuantityCatalyticActivityContent
    AddSpecialSIUnit(kSIQuantityCatalyticActivityContent, STR("katal per kilogram"), STR("katals per kilogram"), STR("kat/kg"));
//
#pragma mark kSIQuantityRatePerAmountConcentrationPerTime
    AddNonSIUnitToLibrary(kSIQuantityRatePerAmountConcentrationPerTime, STR("liter per mole per second"), STR("liter per mole per second"), STR("L/(mol•s)"), 0.001);
//
#pragma mark kSIQuantityRefractiveIndex
    AddSpecialSIUnit(kSIQuantityRefractiveIndex, STR("meter second per meter second"), STR("meter seconds per meter second"), STR("m•s/(m•s)"));
//
#pragma mark kSIQuantityElectricQuadrupoleMoment
    AddNonSIUnitToLibrary(kSIQuantityElectricQuadrupoleMoment, STR("atomic unit of electric quadrupole moment"), STR("atomic units of electric quadrupole moment"), STR("q_e•a_0^2"), kSIElementaryCharge * a_0 * a_0);
//
#pragma mark kSIQuantityMagnetizability
    AddNonSIUnitToLibrary(kSIQuantityMagnetizability, STR("atomic unit of magnetizability"), STR("atomic units of magnetizability"), STR("q_e•a_0^2/m_e"), kSIElementaryCharge * a_0 * a_0 / kSIElectronMass);
//
#pragma mark kSIQuantityPermittivity
    AddNonSIUnitToLibrary(kSIQuantityPermittivity, STR("atomic unit of permittivity"), STR("atomic units of permittivity"), STR("q_e^2/(a_0•E_h)"), kSIElementaryCharge * kSIElementaryCharge / (a_0 * E_h));
    AddSpecialSIUnit(kSIQuantityPermittivity, STR("farad per meter"), STR("farads per meter"), STR("F/m"));
    AddSpecialSIUnit(kSIQuantityPermittivity, STR("coulomb per volt meter"), STR("coulombs per volt meter"), STR("C/(V•m)"));
    AddNonSIUnitToLibrary(kSIQuantityPermittivity, STR("electric constant"), STR("electric constant"), STR("ε_0"), kSIElectricConstant);
//
#pragma mark kSIQuantityElectricPolarizability
    AddNonSIUnitToLibrary(kSIQuantityElectricPolarizability, STR("atomic unit of electric polarizability"), STR("atomic units of electric polarizability"), STR("q_e^2•a_0^2/E_h"), kSIElementaryCharge * kSIElementaryCharge * a_0 * a_0 / (E_h));
//
#pragma mark kSIQuantityFirstHyperPolarizability
    AddNonSIUnitToLibrary(kSIQuantityFirstHyperPolarizability, STR("atomic unit of 1st polarizability"), STR("atomic units of 1st polarizability"), STR("q_e^3•a_0^3/E_h^2"), kSIElementaryCharge * kSIElementaryCharge * kSIElementaryCharge * a_0 * a_0 * a_0 / (E_h * E_h));
//
#pragma mark kSIQuantitySecondHyperPolarizability
    AddNonSIUnitToLibrary(kSIQuantitySecondHyperPolarizability, STR("atomic unit of 2nd polarizability"), STR("atomic units of 2nd polarizability"), STR("q_e^4•a_0^4/E_h^3"), kSIElementaryCharge * kSIElementaryCharge * kSIElementaryCharge * kSIElementaryCharge * a_0 * a_0 * a_0 * a_0 / (E_h * E_h * E_h));
//
#pragma mark kSIQuantitySpecificVolume
    AddNonSIUnitToLibrary(kSIQuantitySpecificVolume, STR("cubic meter per kilogram"), STR("cubic meters per kilogram"), STR("m^3/kg"), 1.);
//
#pragma mark kSIQuantityMassToChargeRatio
    AddNonSIUnitToLibrary(kSIQuantityMassToChargeRatio, STR("thomson"), STR("thomson"), STR("Th"), kSIAtomicMassConstant / kSIElementaryCharge);
//
#pragma mark kSIQuantityChargeToMassRatio
    AddNonSIUnitToLibrary(kSIQuantityChargeToMassRatio, STR("inverse thomson"), STR("inverse thomson"), STR("(1/Th)"), kSIElementaryCharge / kSIAtomicMassConstant);
//
#pragma mark kSIQuantityDynamicViscosity
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantityDynamicViscosity, STR("poise"), STR("poises"), STR("P"), 0.1);
    AddSpecialSIUnit(kSIQuantityDynamicViscosity, STR("pascal second"), STR("pascal seconds"), STR("Pa•s"));
    AddSpecialSIUnit(kSIQuantityDynamicViscosity, STR("newton second per square meter"), STR("newton seconds per square meter"), STR("N•s/m^2"));
//
#pragma mark kSIQuantityKinematicViscosity
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantityKinematicViscosity, STR("stokes"), STR("stokes"), STR("St"), 1e-4);
//
#pragma mark kSIQuantityDiffusionCoefficient
    AddNonSIUnitToLibrary(kSIQuantityDiffusionCoefficient, STR("square meter per second"), STR("square meters per second"), STR("m^2/s"), 1);
    AddNonSIUnitToLibrary(kSIQuantityDiffusionCoefficient, STR("square centimeter per second"), STR("square centimeters per second"), STR("cm^2/s"), 0.0001);
    AddNonSIUnitToLibrary(kSIQuantityDiffusionCoefficient, STR("square millimeter per second"), STR("square millimeters per second"), STR("mm^2/s"), 1e-6);
//
#pragma mark kSIQuantityLuminance
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantityLuminance, STR("stilb"), STR("stilbs"), STR("sb"), 1e4);
    AddSpecialSIUnit(kSIQuantityLuminance, STR("lumen per square meter per steradian"), STR("lumens per square meter per steradian"), STR("lm/(m^2•sr)"));
//
#pragma mark kSIQuantityInverseMagneticFluxDensity
    AddNonSIUnitToLibrary(kSIQuantityInverseMagneticFluxDensity, STR("inverse gauss"), STR("inverse gauss"), STR("(1/G)"), 1.);
//
#pragma mark kSIQuantityMagneticFieldStrength
    AddSpecialSIUnit(kSIQuantityMagneticFieldStrength, STR("ampere per meter"), STR("amperes per meter"), STR("A/m"));
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantityMagneticFieldStrength, STR("œrsted"), STR("œrsteds"), STR("Oe"), 79.577471545947674);
//
#pragma mark kSIQuantityMomentOfForce
    AddSpecialSIUnit(kSIQuantityMomentOfForce, STR("newton meter"), STR("newton meters"), STR("N•m"));
    AddNonSIUnitToLibrary(kSIQuantityMomentOfForce, STR("foot pound force"), STR("feet pound force"), STR("ft•lbf"), 1.3558179483314);
    AddNonSIUnitToLibrary(kSIQuantityMomentOfForce, STR("inch pound force"), STR("inch pound force"), STR("in•lbf"), 1.3558179483314 / 12.);
    AddNonSIUnitToLibrary(kSIQuantityMomentOfForce, STR("inch ounce force"), STR("inch ounce force"), STR("in•ozf"), 1.3558179483314 / 12. / 16.);
    AddNonSIUnitToLibrary(kSIQuantityMomentOfForce, STR("pound force foot"), STR("pound force feet"), STR("lbf•ft"), 1.3558179483314);
    AddNonSIUnitToLibrary(kSIQuantityMomentOfForce, STR("pound force inch"), STR("pound force inches"), STR("lbf•in"), 1.3558179483314 / 12.);
    AddNonSIUnitToLibrary(kSIQuantityMomentOfForce, STR("ounce force inch"), STR("ounce force inches"), STR("ozf•in"), 1.3558179483314 / 12. / 16.);
//
#pragma mark kSIQuantitySurfaceTension
    AddSpecialSIUnit(kSIQuantitySurfaceTension, STR("newton per meter"), STR("newtons per meter"), STR("N/m"));
    AddNonSIUnitToLibrary(kSIQuantitySurfaceTension, STR("dyne per centimeter"), STR("dynes per centimeter"), STR("dyn/cm"), 0.001);
//
#pragma mark kSIQuantitySurfaceEnergy
    AddSpecialSIUnit(kSIQuantitySurfaceEnergy, STR("joule per square meter"), STR("joules per square meter"), STR("J/m^2"));
    AddNonSIUnitToLibrary(kSIQuantitySurfaceEnergy, STR("dyne per square centimeter"), STR("dynes per square centimeter"), STR("dyn/cm^2"), 0.1);
//
#pragma mark kSIQuantityAngularSpeed, kSIQuantityAngularVelocity
    AddSpecialSIUnit(kSIQuantityAngularSpeed, STR("radian per second"), STR("radians per second"), STR("rad/s"));
    units = (OCMutableArrayRef)OCDictionaryGetValue(unitsQuantitiesLibrary, kSIQuantityAngularSpeed);
    OCDictionaryAddValue(unitsQuantitiesLibrary, kSIQuantityAngularVelocity, units);
//
#pragma mark kSIQuantityAngularAcceleration
    AddSpecialSIUnit(kSIQuantityAngularAcceleration, STR("radian per square second"), STR("radians per square second"), STR("rad/s^2"));
//
#pragma mark kSIQuantityHeatFluxDensity, kSIQuantityIrradiance
    AddSpecialSIUnit(kSIQuantityHeatFluxDensity, STR("watt per square meter"), STR("watts per square meter"), STR("W/m^2"));
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantityHeatFluxDensity, STR("watt per square centimeter"), STR("watts per square centimeter"), STR("W/cm^2"), 10000.);
    AddNonSIUnitToLibrary(kSIQuantityHeatFluxDensity, STR("watt per square foot"), STR("watts per square foot"), STR("W/ft^2"), 10.76391041670972);
    AddNonSIUnitToLibrary(kSIQuantityHeatFluxDensity, STR("watt per square inch"), STR("watts per square inch"), STR("W/in^2"), 10.76391041670972 / 12.);
    units = (OCMutableArrayRef)OCDictionaryGetValue(unitsQuantitiesLibrary, kSIQuantityHeatFluxDensity);
    OCDictionaryAddValue(unitsQuantitiesLibrary, kSIQuantityIrradiance, units);
//
#pragma mark kSIQuantitySpectralRadiantFluxDensity
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantitySpectralRadiantFluxDensity, STR("watt per square meter per nanometer"), STR("watts per square meter per nanometer"), STR("W/(m^2•nm)"), 1.e9);
//
#pragma mark kSIQuantityEntropy, kSIQuantityHeatCapacity
    AddSpecialSIUnit(kSIQuantityEntropy, STR("joule per kelvin"), STR("joules per kelvin"), STR("J/K"));
    AddNonSIUnitToLibrary(kSIQuantityEntropy, STR("boltzmann constant"), STR("boltzmann constant"), STR("k_B"), kSIBoltmannConstant);
    units = (OCMutableArrayRef)OCDictionaryGetValue(unitsQuantitiesLibrary, kSIQuantityEntropy);
    OCDictionaryAddValue(unitsQuantitiesLibrary, kSIQuantityHeatCapacity, units);
//
#pragma mark kSIQuantitySpecificHeatCapacity, kSIQuantitySpecificEntropy
    AddSpecialSIUnit(kSIQuantitySpecificHeatCapacity, STR("joule per kilogram kelvin"), STR("joules per kilogram kelvin"), STR("J/(kg•K)"));
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantitySpecificHeatCapacity, STR("joule per gram kelvin"), STR("joules per gram kelvin"), STR("J/(g•K)"), 1000.);
    AddNonSIUnitToLibrary(kSIQuantitySpecificHeatCapacity, STR("calorie per gram per kelvin"), STR("calories per gram per kelvin"), STR("cal/(g•K)"), 4186.8);
    units = (OCMutableArrayRef)OCDictionaryGetValue(unitsQuantitiesLibrary, kSIQuantitySpecificHeatCapacity);
    OCDictionaryAddValue(unitsQuantitiesLibrary, kSIQuantitySpecificEntropy, units);
//
#pragma mark kSIQuantityMolarMass
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantityMolarMass, STR("gram per mole"), STR("grams per mole"), STR("g/mol"), 1e-3);
//
#pragma mark kSIQuantityMolality
    AddSpecialSIUnit(kSIQuantityMolality, STR("mole per kilogram"), STR("moles per kilogram"), STR("mol/kg"));
//
#pragma mark kSIQuantitySpecificEnergy
    AddSpecialSIUnit(kSIQuantitySpecificEnergy, STR("joule per kilogram"), STR("joules per kilogram"), STR("J/kg"));
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantitySpecificEnergy, STR("joule per gram"), STR("joules per gram"), STR("J/g"), 1e3);
//
#pragma mark kSIQuantityThermalConductance
    AddSpecialSIUnit(kSIQuantityThermalConductance, STR("watt per kelvin"), STR("watts per kelvin"), STR("W/K"));
    AddNonSIUnitToLibrary(kSIQuantityThermalConductance, STR("Btu per hour per rankine"), STR("Btus per hour per rankine"), STR("Btu/(h•°R)"), 0.5275279262867396);
    AddNonSIUnitToLibrary(kSIQuantityThermalConductance, STR("calorie per hour per kelvin"), STR("calories per hour per kelvin"), STR("cal/(h•K)"), 1.163e-3);
    AddNonSIUnitToLibrary(kSIQuantityThermalConductance, STR("kilocalorie per hour per kelvin"), STR("kilocalories per hour per kelvin"), STR("kcal/(h•K)"), 1.163);
//
#pragma mark kSIQuantityThermalConductivity
    AddSpecialSIUnit(kSIQuantityThermalConductivity, STR("watt per meter kelvin"), STR("watts per meter kelvin"), STR("W/(m•K)"));
    AddNonSIUnitToLibrary(kSIQuantityThermalConductivity, STR("Btu per hour per foot per rankine"), STR("Btus per hour per foot per rankine"), STR("Btu/(h•ft•°R)"), 1.730734666295077);
    AddNonSIUnitToLibrary(kSIQuantityThermalConductivity, STR("calorie per hour per meter per kelvin"), STR("calories per hour per meter per kelvin"), STR("cal/(h•m•K)"), 1.163e-3);
    AddNonSIUnitToLibrary(kSIQuantityThermalConductivity, STR("kilocalorie per hour per meter per kelvin"), STR("kilocalories per hour per meter per kelvin"), STR("kcal/(h•m•K)"), 1.163);
//
#pragma mark kSIQuantityEnergyDensity
    AddSpecialSIUnit(kSIQuantityEnergyDensity, STR("joule per cubic meter"), STR("joules per cubic meter"), STR("J/m^3"));
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantityEnergyDensity, STR("joule per liter"), STR("joules per liter"), STR("J/L"), 1000.);
//
#pragma mark kSIQuantityElectricDipoleMoment
    AddSpecialSIUnit(kSIQuantityElectricDipoleMoment, STR("coulomb meter"), STR("coulomb meters"), STR("C•m"));
    AddNonSIUnitToLibrary(kSIQuantityElectricDipoleMoment, STR("debye"), STR("debyes"), STR("D"), 3.335640951816991e-30);
    AddNonSIUnitToLibrary(kSIQuantityElectricDipoleMoment, STR("atomic unit of electric dipole moment"), STR("atomic unit of electric dipole moment"), STR("q_e•a_0"), kSIElementaryCharge * a_0);
//
#pragma mark kSIQuantityElectricFieldStrength
    AddSpecialSIUnit(kSIQuantityElectricFieldStrength, STR("volt per meter"), STR("volts per meter"), STR("V/m"));
    AddNonSIUnitToLibrary(kSIQuantityElectricFieldStrength, STR("atomic unit of electric field"), STR("atomic unit of electric field"), STR("E_h/(q_e•a_0)"), E_h / (kSIElementaryCharge * a_0));
    AddSpecialSIUnit(kSIQuantityElectricFieldStrength, STR("volt per centimeter"), STR("volts per centimeter"), STR("V/cm"));
    AddSpecialSIUnit(kSIQuantityElectricFieldStrength, STR("newton per coulomb"), STR("newtons per coulomb"), STR("N/C"));
//
#pragma mark kSIQuantityElectricFlux
    AddSpecialSIUnit(kSIQuantityElectricFlux, STR("volt meter"), STR("volts meter"), STR("V•m"));
//
#pragma mark kSIQuantityElectricChargeDensity
    AddSpecialSIUnit(kSIQuantityElectricChargeDensity, STR("coulomb per cubic meter"), STR("coulombs per cubic meter"), STR("C/m^3"));
    AddNonSIUnitToLibrary(kSIQuantityElectricChargeDensity, STR("charge density"), STR("charge density"), STR("A•h/L"), 3600000);
    AddNonSIUnitToLibrary(kSIQuantityElectricChargeDensity, STR("atomic unit of charge density"), STR("atomic unit of charge density"), STR("q_e/a_0^3"), kSIElementaryCharge / (a_0 * a_0 * a_0));
//
#pragma mark kSIQuantitySurfaceChargeDensity, kSIQuantityElectricFluxDensity, kSIQuantityElectricDisplacement
    AddSpecialSIUnit(kSIQuantitySurfaceChargeDensity, STR("coulomb per square meter"), STR("coulombs per square meter"), STR("C/m^2"));
    units = (OCMutableArrayRef)OCDictionaryGetValue(unitsQuantitiesLibrary, kSIQuantitySurfaceChargeDensity);
    OCDictionaryAddValue(unitsQuantitiesLibrary, kSIQuantityElectricFluxDensity, units);
    OCDictionaryAddValue(unitsQuantitiesLibrary, kSIQuantityElectricDisplacement, units);
//
#pragma mark kSIQuantityPermeability
    AddSpecialSIUnit(kSIQuantityPermeability, STR("henry per meter"), STR("henries per meter"), STR("H/m"));
    AddSpecialSIUnit(kSIQuantityPermeability, STR("newton per square ampere"), STR("newtons per square ampere"), STR("N/A^2"));
    AddSpecialSIUnit(kSIQuantityPermeability, STR("tesla meter per ampere"), STR("tesla meter per ampere"), STR("T•m/A"));
    AddSpecialSIUnit(kSIQuantityPermeability, STR("weber per ampere meter"), STR("webers per ampere meter"), STR("Wb/(A•m)"));
    AddNonSIUnitToLibrary(kSIQuantityPermeability, STR("magnetic constant"), STR("magnetic constant"), STR("µ_0"), 4 * kSIPi * 1.e-7);
//
#pragma mark kSIQuantityMolarEntropy, kSIQuantityMolarHeatCapacity
    AddSpecialSIUnit(kSIQuantityMolarEntropy, STR("joule per mole kelvin"), STR("joules per mole kelvin"), STR("J/(mol•K)"));
    AddNonSIUnitToLibrary(kSIQuantityMolarEntropy, STR("gas constant"), STR("gas constant"), STR("R"), kSIBoltmannConstant * kSIAvogadroConstant);
    units = (OCMutableArrayRef)OCDictionaryGetValue(unitsQuantitiesLibrary, kSIQuantityMolarEntropy);
    OCDictionaryAddValue(unitsQuantitiesLibrary, kSIQuantityMolarHeatCapacity, units);
//
#pragma mark kSIQuantityMolarEnergy
    AddSpecialSIUnit(kSIQuantityMolarEnergy, STR("joule per mole"), STR("joules per mole"), STR("J/mol"));
    AddNonSIUnitToLibrary(kSIQuantityMolarEnergy, STR("calorie per mole"), STR("calories per mole"), STR("cal/mol"), 4.1868);
    AddNonSIUnitToLibrary(kSIQuantityMolarEnergy, STR("kilocalorie per mole"), STR("kilocalories per mole"), STR("kcal/mol"), 4.1868 * 1000.);
//
#pragma mark kSIQuantityRadiationExposure
    AddSpecialSIUnit(kSIQuantityRadiationExposure, STR("coulomb per kilogram"), STR("coulombs per kilogram"), STR("C/kg"));
//
#pragma mark kSIQuantityAbsorbedDoseRate
    AddSpecialSIUnit(kSIQuantityAbsorbedDoseRate, STR("gray per second"), STR("grays per second"), STR("Gy/s"));
//
#pragma mark kSIQuantityRadiantIntensity
    AddSpecialSIUnit(kSIQuantityRadiantIntensity, STR("watt per steradian"), STR("watts per steradian"), STR("W/sr"));
//
#pragma mark kSIQuantitySpectralRadiantIntensity
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantitySpectralRadiantIntensity, STR("watt per steradian per nanometer"), STR("watts per steradian per nanometer"), STR("W/(sr•nm)"), 1.e9);
//
#pragma mark kSIQuantityRadiance
    AddSpecialSIUnit(kSIQuantityRadiance, STR("watt per square meter per steradian"), STR("watts per square meter per steradian"), STR("W/(m^2•sr)"));
//
#pragma mark kSIQuantitySpectralRadiance
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantitySpectralRadiance, STR("watt per square meter per steradian per nanometer"), STR("watts per square meter steradian per nanometer"), STR("W/(m^2•sr•nm)"), 1.e9);
//
#pragma mark kSIQuantityFrequencyPerMagneticFluxDensity
    AddSpecialSIUnit(kSIQuantityFrequencyPerMagneticFluxDensity, STR("hertz per tesla"), STR("hertz per tesla"), STR("Hz/T"));
//
#pragma mark kSIQuantityLengthPerVolume
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantityLengthPerVolume, STR("meter per liter"), STR("meters per liter"), STR("m/L"), 1. / 1.e-3);
//
#pragma mark kSIQuantityPowerPerLuminousFlux
    AddSpecialSIUnit(kSIQuantityPowerPerLuminousFlux, STR("watt per lumen"), STR("watts per lumen"), STR("W/lm"));
//
#pragma mark kSIQuantityLuminousEfficacy
    AddSpecialSIUnit(kSIQuantityLuminousEfficacy, STR("lumen per watt"), STR("lumens per watt"), STR("lm/W"));
//
#pragma mark kSIQuantityHeatTransferCoefficient
    AddSpecialSIUnit(kSIQuantityHeatTransferCoefficient, STR("watt per square meter per kelvin"), STR("watts per square meter per kelvin"), STR("W/(m^2•K)"));
    AddNonSIUnitToLibrary(kSIQuantityHeatTransferCoefficient, STR("Btu per hour per square foot per rankine"), STR("Btus per hour per square foot per rankine"), STR("Btu/(h•ft^2•°R)"), 5.678263340863113);
    AddNonSIUnitToLibrary(kSIQuantityHeatTransferCoefficient, STR("calorie per hour per square meter per kelvin"), STR("calories per hour per square meter per kelvin"), STR("cal/(h•m^2•K)"), 1.163e-3);
    AddNonSIUnitToLibrary(kSIQuantityHeatTransferCoefficient, STR("kilocalorie per hour per square meter per kelvin"), STR("kilocalories per hour per square meter per kelvin"), STR("kcal/(h•m^2•K)"), 1.163);
//
#pragma mark kSIQuantityChargePerAmount
    AddNonSIUnitToLibrary(kSIQuantityChargePerAmount, STR("faraday constant"), STR("faraday constant"), STR("&F"), kSIElementaryCharge * kSIAvogadroConstant);
    AddNonSIUnitToLibrary(kSIQuantityChargePerAmount, STR("coulomb per mole"), STR("coulombs per mole"), STR("C/mol"), 1.0);
//
#pragma mark kSIQuantityGravitationalConstant
    AddNonSIUnitToLibrary(kSIQuantityGravitationalConstant, STR("gravitational constant"), STR("gravitational constant"), STR("&G"), kSIGravitationalConstant);
//
#pragma mark kSIQuantityVolumePerLength
    AddNonSIUnitToLibrary(kSIQuantityVolumePerLength, STR("liter per 100 kilometers"), STR("liters per 100 kilometers"), STR("L/(100 km)"), 1e-3 / 100000.);
//
#pragma mark kSIQuantityVolumetricFlowRate
    AddNonSIUnitToLibrary(kSIQuantityVolumetricFlowRate, STR("cubic meter per hour"), STR("cubic meters per hour"), STR("m^3/h"), 1. / 3600.);
    AddNonSIUnitToLibrary(kSIQuantityVolumetricFlowRate, STR("cubic meter per minute"), STR("cubic meters per minute"), STR("m^3/min"), 1. / 60.);
    AddNonSIUnitToLibrary(kSIQuantityVolumetricFlowRate, STR("cubic meter per second"), STR("cubic meters per second"), STR("m^3/s"), 1.);
    AddNonSIUnitToLibrary(kSIQuantityVolumetricFlowRate, STR("cubic centimeter per hour"), STR("cubic centimeters per hour"), STR("cm^3/h"), 1e-6 / 3600.);
    AddNonSIUnitToLibrary(kSIQuantityVolumetricFlowRate, STR("cubic centimeter per minute"), STR("cubic centimeters per minute"), STR("cm^3/min"), 1e-6 / 60.);
    AddNonSIUnitToLibrary(kSIQuantityVolumetricFlowRate, STR("cubic centimeter per second"), STR("cubic centimeters per second"), STR("cm^3/s"), 1e-6);
    AddNonSIUnitToLibrary(kSIQuantityVolumetricFlowRate, STR("cubic foot per hour"), STR("cubic feet per hour"), STR("ft^3/h"), (1609.344 / 5280) * (1609.344 / 5280) * (1609.344 / 5280) / 3600.);
    AddNonSIUnitToLibrary(kSIQuantityVolumetricFlowRate, STR("cubic foot per minute"), STR("cubic feet per minute"), STR("ft^3/min"), (1609.344 / 5280) * (1609.344 / 5280) * (1609.344 / 5280) / 60.);
    AddNonSIUnitToLibrary(kSIQuantityVolumetricFlowRate, STR("cubic foot per second"), STR("cubic feet per second"), STR("ft^3/s"), (1609.344 / 5280) * (1609.344 / 5280) * (1609.344 / 5280));
//
#pragma mark kSIQuantityPowerPerAreaPerTemperatureToFourthPower
    AddNonSIUnitToLibrary(kSIQuantityPowerPerAreaPerTemperatureToFourthPower, STR("stefan-boltzmann constant"), STR("stefan-boltzmann constant"), STR("σ"), kSIStefanBoltzmannConstant);
//
#pragma mark kSIQuantityWavelengthDisplacementConstant
    AddNonSIUnitToLibrary(kSIQuantityWavelengthDisplacementConstant, STR("wien wavelength displacement constant"), STR("wien wavelength displacement constant"), STR("b_λ"), kSIWeinDisplacementConstant);
//
#pragma mark kSIQuantityGasPermeance
    AddNonSIUnitToLibrary(kSIQuantityGasPermeance, STR("gas permeance unit"), STR("gas permeance unit"), STR("GPU"), 3.35e-10);

    // fprintf(stderr,"\nSIUnitLibrary: %d units loaded.\n", (int)OCDictionaryGetCount(unitsLibrary));

    // Set to UK or US units.
    imperialVolumes = true;
    SIUnitsLibrarySetImperialVolumes(false);
    if (currentlocale->currency_symbol) {
        if (strcmp(currentlocale->currency_symbol, "£") == 0) {
            imperialVolumes = false;
            SIUnitsLibrarySetImperialVolumes(true);
        }
    }
    OCArrayRef allUnits = OCDictionaryCreateArrayWithAllValues(unitsLibrary);
    unitsNamesLibrary = OCArrayCreateMutableCopy(allUnits);
    OCRelease(allUnits);
    OCArraySortValues(unitsNamesLibrary, OCRangeMake(0, OCArrayGetCount(unitsNamesLibrary)), unitNameLengthSort, NULL);
    return true;
}

//------------------------------------------------------------------------------
// full JSON serializer for SIUnitRef
//------------------------------------------------------------------------------
cJSON *SIUnitCreateJSON(SIUnitRef unit) {
    if (!unit) return cJSON_CreateNull();

    cJSON *json = cJSON_CreateObject();

    // 1) dimensionality
    cJSON_AddItemToObject(json,
                          "dimensionality",
                          SIDimensionalityCreateJSON(unit->dimensionality));

    // 2) coherent‐SI scale factor
    cJSON_AddNumberToObject(json,
                            "scale_to_coherent_si",
                            unit->scale_to_coherent_si);

    // 3) per–base‐dimension prefixes
    //    we have exactly BASE_DIMENSION_COUNT entries in each array
    cJSON *numArr = cJSON_CreateIntArray((const int *)unit->num_prefix,
                                         BASE_DIMENSION_COUNT);
    cJSON_AddItemToObject(json, "num_prefix", numArr);

    cJSON *denArr = cJSON_CreateIntArray((const int *)unit->den_prefix,
                                         BASE_DIMENSION_COUNT);
    cJSON_AddItemToObject(json, "den_prefix", denArr);

    // 4) optional root name / plural name
    if (unit->root_name)
        cJSON_AddStringToObject(json,
                                "root_name",
                                OCStringGetCString(unit->root_name));
    else
        cJSON_AddNullToObject(json, "root_name");

    if (unit->root_plural_name)
        cJSON_AddStringToObject(json,
                                "root_plural_name",
                                OCStringGetCString(unit->root_plural_name));
    else
        cJSON_AddNullToObject(json, "root_plural_name");

    // 5) the “symbol” and its own prefix (Ω, lb, Å, etc)
    if(unit->symbol)
        cJSON_AddStringToObject(json,
                                "symbol",
                                OCStringGetCString(unit->symbol));
    else
        cJSON_AddNullToObject(json, "symbol");

    if(unit->key)
        cJSON_AddStringToObject(json,
                                "key",
                                OCStringGetCString(unit->key));
    else
        cJSON_AddNullToObject(json, "key");

    if (unit->root_symbol)
        cJSON_AddStringToObject(json,
                                "root_symbol",
                                OCStringGetCString(unit->root_symbol));
    else
        cJSON_AddNullToObject(json, "root_symbol");

    cJSON_AddNumberToObject(json,
                            "root_symbol_prefix",
                            unit->root_symbol_prefix);

    // 6) flags
    cJSON_AddBoolToObject(json,
                         "allows_si_prefix",
                         unit->allows_si_prefix);
    cJSON_AddBoolToObject(json,
                         "is_special_si_symbol",
                         unit->is_special_si_symbol);

    return json;
}

//------------------------------------------------------------------------------
// full JSON deserializer for SIUnitRef
//------------------------------------------------------------------------------
SIUnitRef SIUnitFromJSON(cJSON *json) {
    if (!json || !cJSON_IsObject(json)) return NULL;

    // 1) dimensionality
    cJSON *dimNode = cJSON_GetObjectItem(json, "dimensionality");
    SIDimensionalityRef dim = SIDimensionalityFromJSON(dimNode);
    if (!dim) return NULL;

    // 2) scale
    cJSON *scaleNode = cJSON_GetObjectItem(json, "scale_to_coherent_si");
    double scale = cJSON_IsNumber(scaleNode)
                   ? scaleNode->valuedouble
                   : 1.0;

    // 3) prefix arrays
    SIPrefix num_prefix[BASE_DIMENSION_COUNT];
    SIPrefix den_prefix[BASE_DIMENSION_COUNT];
    cJSON *numArr = cJSON_GetObjectItem(json, "num_prefix");
    cJSON *denArr = cJSON_GetObjectItem(json, "den_prefix");

    for (int i = 0; i < BASE_DIMENSION_COUNT; i++) {
        cJSON *n = cJSON_GetArrayItem(numArr, i);
        cJSON *d = cJSON_GetArrayItem(denArr, i);
        num_prefix[i] = (n && cJSON_IsNumber(n))
                        ? (SIPrefix) n->valueint
                        : kSIPrefixNone;
        den_prefix[i] = (d && cJSON_IsNumber(d))
                        ? (SIPrefix) d->valueint
                        : kSIPrefixNone;
    }

    // 4) root name / plural name
    OCStringRef root_name = NULL, root_plural = NULL;
    cJSON *rn = cJSON_GetObjectItem(json, "root_name");
    if (rn && cJSON_IsString(rn) && rn->valuestring)
        root_name = OCStringCreateWithCString(rn->valuestring);

    cJSON *rp = cJSON_GetObjectItem(json, "root_plural_name");
    if (rp && cJSON_IsString(rp) && rp->valuestring)
        root_plural = OCStringCreateWithCString(rp->valuestring);

    // 5) symbol + key
    OCStringRef symbol = NULL, key = NULL;
    cJSON *s = cJSON_GetObjectItem(json, "symbol");
    if (s && cJSON_IsString(s) && s->valuestring)
        symbol = OCStringCreateWithCString(s->valuestring);
    
    cJSON *k = cJSON_GetObjectItem(json, "key");
    if (k && cJSON_IsString(k) && k->valuestring)
        key = OCStringCreateWithCString(k->valuestring);

    // 5) root symbol + prefix
    OCStringRef root_symbol = NULL;
    cJSON *rs = cJSON_GetObjectItem(json, "root_symbol");
    if (rs && cJSON_IsString(rs) && rs->valuestring)
        root_symbol = OCStringCreateWithCString(rs->valuestring);

    SIPrefix root_symbol_prefix = 0;
    cJSON *rsp = cJSON_GetObjectItem(json, "root_symbol_prefix");
    if (rsp && cJSON_IsNumber(rsp))
        root_symbol_prefix = (SIPrefix)rsp->valueint;

    // 6) flags
    cJSON *asp = cJSON_GetObjectItem(json, "allows_si_prefix");
    bool allows_si_prefix = asp && cJSON_IsBool(asp) && (asp->valueint != 0);

    cJSON *iss = cJSON_GetObjectItem(json, "is_special_si_symbol");
    bool is_special_si_symbol = iss && cJSON_IsBool(iss) && (iss->valueint != 0);

    // 7) build the new unit
    SIUnitRef u = SIUnitWithParameters(
        dim,
        num_prefix, den_prefix,
        root_name, root_plural,
        root_symbol, root_symbol_prefix,
        allows_si_prefix,
        is_special_si_symbol,
        scale
    );
// Cast away const to override the auto-generated symbol and key
    struct impl_SIUnit *mutable_u = (struct impl_SIUnit *)u;
    if (mutable_u->symbol) OCRelease(mutable_u->symbol);
    if (mutable_u->key) OCRelease(mutable_u->key);
    mutable_u->symbol = symbol;
    mutable_u->key = key;

    // 8) clean up temporary OCStringRefs
    if (root_name)        OCRelease(root_name);
    if (root_plural)      OCRelease(root_plural);
    if (root_symbol)      OCRelease(root_symbol);

    return u;
}


