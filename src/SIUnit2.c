//
//  SIUnit.c
//  SITypes
//
//  Created by philip on 6/7/17.
//  Copyright © 2017 PhySy Ltd. All rights reserved.
//
#include <locale.h>
#include <math.h>  // For floor, isnan, erf, erfc, log, sqrt, pow, fabs, log10
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "SIDimensionalityPrivate.h"
#include "SILibrary.h"
/**
 * @file SIUnit2.c
 * @brief Implementation of the SIUnit2 type and related functionality.
 *
 * This file contains the definition of the SIUnit2 type, along with functions
 * for creating, manipulating, and destroying SIUnit2 instances.
 *
 * @section SI Unit Prefixes
 *
 * The SIUnit2 system uses a set of prefixes in front of the root unit symbol to
 * indicate a power of ten multiplier, e.g., k (kilo) for 10^3, m (milli) for 10^-3,
 * etc.  Only SI Units, the SI Special Units, and a few Non-SI units are allowed to
 * have SI prefixes.  These are detailed below.
 *
 * @section Units
 *
 * The SIUnit2 system recognizes several categories of units:
 *
 * @subsection Coherent SI Base Units
 * A set of seven base units, given by the symbols: m, kg, s, A, K, mol, cd
 *
 * @subsection Coherent Derived SI Units
 * Units derived from the Coherent SI Base Units, such as m/s, kg·m/s², etc.
 * These units are coherent because they are expressed entirely in terms of the
 * Coherent SI Base Units, i.e., multiplied or divided. For each dimensionality,
 * there is only one coherent derived unit, formed from the Coherent SI Base Units,
 * such as kg•m/s, etc.  Every SIUnit2 has a scale_to_coherent_si value, which is the
 * scaling factor needed to convert from that unit back to its corresponding
 * coherent SI unit.
 *
 * @subsection SI Base Root Units
 * A set of seven root units, given by the symbols: m, g, s, A, K, mol, cd.
 * This set is defined because of the minor complication that the coherent
 * base unit for mass is the kilogram (kg), while the root unit is the gram (g).
 *
 * @subsection SI Base Units
 * Include the seven Coherent SI Base Units and all decimal multiples
 * of the root units, created using the 20 SI prefix symbols indicating
 * the different powers of ten multipliers, e.g., pm, nm, µm, mm,
 * cm, m, km, Mg, Gg, Tg, Pg, Eg, Zg, Yg, etc.
 *
 * @subsection Derived SI Units
 * Units formed from the SI Base Root Units, such as µm/s,
 * g•km/s², etc. Among the Derived SI Units, we have the derived
 * dimensionless units, which are formed by taking the ratio of
 * two quantities with the same dimensionality, such as the
 * radian (rad = m/m) and the steradian (sr = m^2/m^2). These units
 * are distinct from the dimensionless and underived dimensionless unit.
 *
 * @subsection Special SI Units
 * There are 22 Special SI Unit symbols (and names), which are equivalent to
 * their corresponding coherent derived SI Units, e.g., V = m²•kg/s³,
 * W = m²•kg/s³, J = m²•kg/s², etc. They are allowed to have SI prefixes.
 *
 * @subsection Non-SI Units with SI prefixes
 * There are a few units that are not part of the SI system
 * but are still allowed by the SI, these are the liter (L),
 * electron volt (eV), tonne (t), the bar.  The package allows
 * for units that are not part of the SI system, such as those
 * from the cgs system, e.g., erg, dyn, etc.
 *
 * @subsection Non-SI Units without SI prefixes
 * The package allows for units that are not part of the SI
 * system, the imperial system, e.g., lbf, floz, lb, gal, etc.
 * These units are not allowed to have SI prefixes.
 *
 * @subsection Physical Constants
 * This package also treats symbols for physical constants as
 * unit symbols, although one has to be careful to avoid
 * symbol collisions. There are conﬂicts between unit symbols
 * and commonly accepted symbols for physical constants, e.g.,
 * the planck constant and hour (h), or the newton gravitational
 * constant and gauss (G).  In such cases, precedence is given to the
 * unit symbol, and the physical constant is given a different symbol.
 * These physical constant units are not allowed to have SI prefixes.
 *
 */
/**
 * @section SIUnit2 Implementation Approach
 *
 * In defining the SIUnit2 type, we use the following approach:
 *
 * Each SIUnit2 has a dimensionality, which is described by a SIDimensionalityRef.
 *
 * @subsection derived_units Derived Units
 * Derived units, i.e., units that are formed from the multiplication, division, or exponentiation of
 * units with root_symbols, will not have a root_symbol, i.e., root_symbol will be NULL and
 * allows_si_prefix = false.  In this case, the symbol will be formed from the symbols of the
 * constituent units, e.g., µm/ps^2, lbf^3•floz/min^2. This symbol will be generated at creation.
 * The creation of these units occurs mainly through the unit expression parser, which, in turn, calls
 * functions, to multiply, divide, raise to a power, or take the nth root of the constituent units.
 * In this case, the expression will be normalized to form the derived unit symbol, and the scale_to_coherent_si
 * value will be calculated based on the constituent units.  There are two ways to create a derived unit symbol:
 * (1) If the constituent units do not have derived dimensionalities, then order
 * the numerator and denominator terms in base dimension order, e.g., L,M,T,I,Θ,N,J.
 * (2) If the constituent units have derived dimensionalities, then order the numerator and denominator
 * terms in alphabetical order of the constituent units root_symbols.
 */
#define UNIT_NOT_FOUND -1
// SIUnit2 Opaque Type
struct impl_SIUnit2 {
    OCBase base;
    // SIUnit2 Type attributes
    SIDimensionalityRef dimensionality;  // required: dimensionality of the unit
    double scale_to_coherent_si;         // required: Scale factor to convert from this unit to its coherent SI unit
    OCStringRef symbol;                  // required: Symbol of the unit, e.g., "m", "kg", "s", etc.
    OCStringRef name;                    // optional: name of the unit, e.g., "meter", "gram", "second", etc.
    OCStringRef plural_name;             // optional: Plural name of the unit, e.g., "meters", "grams", "seconds", etc.
};
static struct impl_SIUnit2 *SIUnit2Allocate();
static OCTypeID kSIUnit2ID = kOCNotATypeID;
OCTypeID SIUnit2GetTypeID(void) {
    if (kSIUnit2ID == kOCNotATypeID)
        kSIUnit2ID = OCRegisterType("SIUnit2");
    return kSIUnit2ID;
}
bool impl_SIUnit2Equal(const void *theType1, const void *theType2) {
    if (!theType1 || !theType2) return false;
    if (theType1 == theType2) return true;
    if (OCGetTypeID(theType1) != OCGetTypeID(theType2)) return false;
    SIUnit2Ref u1 = (SIUnit2Ref)theType1;
    SIUnit2Ref u2 = (SIUnit2Ref)theType2;
    if (!OCTypeEqual(u1->dimensionality, u2->dimensionality)) return false;
    {
        OCStringRef const *fields1[] = {
            (OCStringRef *)&u1->symbol,
            (OCStringRef *)&u1->name,
            (OCStringRef *)&u1->plural_name};
        OCStringRef const *fields2[] = {
            (OCStringRef *)&u2->symbol,
            (OCStringRef *)&u2->name,
            (OCStringRef *)&u2->plural_name};
        for (size_t i = 0; i < sizeof fields1 / sizeof *fields1; ++i) {
            if (!OCStringEqual(*fields1[i], *fields2[i]))
                return false;
        }
    }
    // Numeric scale
    if (OCCompareDoubleValues(u1->scale_to_coherent_si,
                              u2->scale_to_coherent_si) != kOCCompareEqualTo) {
        return false;
    }
    return true;
}
void impl_SIUnit2Finalize(const void *theType) {
    if (NULL == theType) return;
    SIUnit2Ref theUnit = (SIUnit2Ref)theType;
    if (theUnit->dimensionality) OCRelease(theUnit->dimensionality);
    if (theUnit->symbol) OCRelease(theUnit->symbol);
    if (theUnit->name) OCRelease(theUnit->name);
    if (theUnit->plural_name) OCRelease(theUnit->plural_name);
}
static OCStringRef impl_SIUnit2CopyFormattingDescription(OCTypeRef theType) {
    if (!theType) return NULL;
    SIUnit2Ref theUnit = (SIUnit2Ref)theType;
    if (theUnit->symbol) return OCStringCreateCopy(theUnit->symbol);
    return OCStringCreateWithCString("<SIUnit2>");
}
static void *impl_SIUnit2DeepCopy(const void *obj) {
    if (!obj) return NULL;
    const SIUnit2Ref src = (SIUnit2Ref)obj;
    struct impl_SIUnit2 *copy = SIUnit2Allocate();
    if (!copy) return NULL;
    copy->dimensionality = OCTypeDeepCopy(src->dimensionality);
    // Copy simple fields
    copy->scale_to_coherent_si = src->scale_to_coherent_si;
    // Copy strings (OCStringRef)
    if (src->symbol) copy->symbol = OCStringCreateCopy(src->symbol);
    if (src->name) copy->name = OCStringCreateCopy(src->name);
    if (src->plural_name) copy->plural_name = OCStringCreateCopy(src->plural_name);
    return (void *)copy;
}
static void *impl_SIUnit2DeepCopyMutable(const void *obj) {
    // SIUnit2 is immutable; just return a standard deep copy
    return impl_SIUnit2DeepCopy(obj);
}
static cJSON *impl_SIUnit2CopyJSON(const void *obj) {
    if (!obj) return cJSON_CreateNull();
    cJSON *json = cJSON_CreateObject();
    const SIUnit2Ref unit = (SIUnit2Ref)obj;
    cJSON_AddItemToObject(json, "dimensionality", SIDimensionalityCreateJSON(unit->dimensionality));
    cJSON_AddNumberToObject(json, "scale_to_coherent_si", unit->scale_to_coherent_si);
    if (unit->name)
        cJSON_AddStringToObject(json, "name", OCStringGetCString(unit->name));
    else
        cJSON_AddNullToObject(json, "root_name");
    if (unit->plural_name)
        cJSON_AddStringToObject(json, "plural_name", OCStringGetCString(unit->plural_name));
    else
        cJSON_AddNullToObject(json, "plural_name");
    if (unit->symbol)
        cJSON_AddStringToObject(json, "symbol", OCStringGetCString(unit->symbol));
    else
        cJSON_AddNullToObject(json, "symbol");
    return json;
}
static struct impl_SIUnit2 *SIUnit2Allocate() {
    return OCTypeAlloc(struct impl_SIUnit2,
                       SIUnit2GetTypeID(),
                       impl_SIUnit2Finalize,
                       impl_SIUnit2Equal,
                       impl_SIUnit2CopyFormattingDescription,
                       impl_SIUnit2CopyJSON,
                       impl_SIUnit2DeepCopy,
                       impl_SIUnit2DeepCopyMutable);
}
static SIUnit2Ref SIUnit2Create(SIDimensionalityRef dimensionality,
                                OCStringRef name,
                                OCStringRef plural_name,
                                OCStringRef symbol,
                                double scale_to_coherent_si) {
    struct impl_SIUnit2 *theUnit = SIUnit2Allocate();
    if (!theUnit) return NULL;
    theUnit->dimensionality = OCRetain(dimensionality);
    theUnit->scale_to_coherent_si = scale_to_coherent_si;
    if (name)
        theUnit->name = OCStringCreateCopy(name);
    else
        theUnit->name = NULL;
    if (plural_name)
        theUnit->plural_name = OCStringCreateCopy(plural_name);
    else
        theUnit->plural_name = NULL;
    if (symbol)
        theUnit->symbol = OCStringCreateCopy(symbol);
    else
        theUnit->symbol = NULL;
    return (SIUnit2Ref)theUnit;
}
// Accessor functions for SIUnit2
SIDimensionalityRef SIUnit2GetDimensionality(SIUnit2Ref theUnit) {
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, NULL);
    return theUnit->dimensionality;
}
OCStringRef SIUnit2GetSymbol(SIUnit2Ref theUnit) {
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, NULL);
    return theUnit->symbol;
}
OCStringRef SIUnit2GetName(SIUnit2Ref theUnit) {
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, NULL);
    return theUnit->name;
}
OCStringRef SIUnit2GetPluralName(SIUnit2Ref theUnit) {
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, NULL);
    return theUnit->plural_name;
}
double SIUnit2ScaleToCoherentSIUnit(SIUnit2Ref theUnit) {
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, 0);
    return theUnit->scale_to_coherent_si;
}
bool SIUnit2AreEquivalentUnits(SIUnit2Ref theUnit1, SIUnit2Ref theUnit2) {
    IF_NO_OBJECT_EXISTS_RETURN(theUnit1, false);
    IF_NO_OBJECT_EXISTS_RETURN(theUnit2, false);
    if (theUnit1 == theUnit2) return true;
    if (!SIDimensionalityEqual(theUnit1->dimensionality, theUnit2->dimensionality)) return false;
    if (OCCompareDoubleValues(SIUnit2ScaleToCoherentSIUnit(theUnit1), SIUnit2ScaleToCoherentSIUnit(theUnit2)) != kOCCompareEqualTo)
        return false;
    return true;
}
//
static OCMutableDictionaryRef units2Library = NULL;
static OCMutableDictionaryRef units2QuantitiesLibrary = NULL;
static OCMutableDictionaryRef units2DimensionalitiesLibrary = NULL;
static OCMutableArrayRef units2NamesLibrary = NULL;
static bool imperialVolumes = false;
// Function prototypes
static bool SIUnit2CreateLibraries(void);
// Library accessor functions
static OCMutableDictionaryRef SIUnit2GetUnitsLib(void) {
    if (NULL == units2Library) SIUnit2CreateLibraries();
    return units2Library;
}
static OCMutableDictionaryRef SIUnit2GetQuantitiesLib(void) {
    if (NULL == units2QuantitiesLibrary) SIUnit2CreateLibraries();
    return units2QuantitiesLibrary;
}
static OCMutableDictionaryRef SIUnit2GetDimensionalitiesLib(void) {
    if (NULL == units2DimensionalitiesLibrary) SIUnit2CreateLibraries();
    return units2DimensionalitiesLibrary;
}
static OCMutableArrayRef SIUnit2GetNamesLib(void) {
    if (NULL == units2NamesLibrary) SIUnit2CreateLibraries();
    return units2NamesLibrary;
}
static SIUnit2Ref AddToLib(
    OCStringRef quantity,
    OCStringRef name,
    OCStringRef plural_name,
    OCStringRef symbol,
    double scale_to_coherent_si,
    OCStringRef *error) {
    if (!quantity || !symbol) return NULL;
    SIDimensionalityRef dimensionality = SIDimensionalityForQuantity(quantity, error);
    if (NULL == dimensionality) {
        if (error && *error == NULL) {
            *error = STR("ERROR: Could not find dimensionality for quantity");
        }
        return NULL;
    }
    SIUnit2Ref theUnit = SIUnit2Create(dimensionality, name, plural_name, symbol, scale_to_coherent_si);
    if (NULL == theUnit) {
        if (error && *error == NULL) {
            *error = STR("ERROR: Could not create SIUnit");
        }
        return NULL;
    }
    OCMutableDictionaryRef unitsLib = SIUnit2GetUnitsLib();
    // Add unit to units library dictionary
    if (OCDictionaryContainsKey(unitsLib, theUnit->symbol)) {
        OCRelease(theUnit);
        return OCDictionaryGetValue(unitsLib, theUnit->symbol);
    }
    OCTypeSetStaticInstance(theUnit, true);
    OCDictionaryAddValue(unitsLib, theUnit->symbol, theUnit);
    OCRelease(theUnit);  // This is a no-op for static instance, but helps AI not get confused
    // Append unit to mutable array value associated with quantity key inside quanity library dictionary
    {
        OCMutableDictionaryRef unitsQuantitiesLib = SIUnit2GetQuantitiesLib();
        OCMutableArrayRef units = (OCMutableArrayRef)OCDictionaryGetValue(unitsQuantitiesLib, quantity);
        if (units)
            OCArrayAppendValue(units, theUnit);
        else {
            units = OCArrayCreateMutable(0, &kOCTypeArrayCallBacks);
            OCArrayAppendValue(units, theUnit);
            OCDictionaryAddValue(unitsQuantitiesLib, quantity, units);
            OCRelease(units);
        }
    }
    // Append unit to mutable array value associated with dimensionality key inside dimensionality library dictionary
    {
        OCStringRef dimensionalitySymbol = SIDimensionalityGetSymbol(dimensionality);
        OCMutableDictionaryRef unitsDimensionalitiesLib = SIUnit2GetDimensionalitiesLib();
        OCMutableArrayRef units = (OCMutableArrayRef)OCDictionaryGetValue(unitsDimensionalitiesLib, dimensionalitySymbol);
        if (units)
            OCArrayAppendValue(units, theUnit);
        else {
            units = OCArrayCreateMutable(0, &kOCTypeArrayCallBacks);
            OCArrayAppendValue(units, theUnit);
            OCDictionaryAddValue(unitsDimensionalitiesLib, dimensionalitySymbol, units);
            OCRelease(units);
        }
    }
    return theUnit;
}
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
#define VALIDATE_PREFIX(prefix, label_str)                             \
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
// Numeric tables (descending exponent order)
// ——————————————————————————————————————————
static const SIPrefix _prefixValues[] = {
#define X(pref, exp, sym, name) pref,
    SIPREFIX_DEFINITIONS
#undef X
};
enum { _PREFIX_COUNT = sizeof(_prefixValues) / sizeof(_prefixValues[0]) };
static OCStringRef prefixSymbolForSIPrefix(SIPrefix prefix) {
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
static OCStringRef prefixNameForSIPrefix(SIPrefix prefix) {
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
static double
prefixValueForSIPrefix(SIPrefix prefix) {
    switch (prefix) {
#define X(pref, exp, sym, name) \
    case pref:                  \
        return pow(10.0, exp);
        SIPREFIX_DEFINITIONS
#undef X
        default:
            return 1.0;  // Default to no scaling for invalid prefix
    }
}
static bool AddToLibPrefixed(
    OCStringRef quantity,
    OCStringRef name,
    OCStringRef plural_name,
    OCStringRef symbol,
    double scale_to_coherent_si,
    OCStringRef *error) {
    // Loop through all SI prefixes and add the prefixed unit to the library
    // by prefixing the unit name, plural_name, and symbol with the SI prefix.
    OCMutableStringRef prefixedName = OCStringCreateMutableCopy(name);
    OCMutableStringRef prefixedPluralName = OCStringCreateMutableCopy(plural_name);
    OCMutableStringRef prefixedSymbol = OCStringCreateMutableCopy(symbol);
    if (!prefixedName || !prefixedPluralName || !prefixedSymbol) {
        if (error && *error == NULL) {
            *error = STR("ERROR: Could not create mutable strings for SI prefixes");
        }
        OCRelease(prefixedName);
        OCRelease(prefixedPluralName);
        OCRelease(prefixedSymbol);
        return false;
    }
    static const int kSIPrefixExponents[] = {
        -24, -21, -18, -15, -12, -9, -6, -3, -2, -1,
        0,
        1, 2, 3, 6, 9, 12, 15, 18, 21, 24};
    const size_t kPrefixCount = sizeof(kSIPrefixExponents) / sizeof(kSIPrefixExponents[0]);
    for (OCIndex index = 0; index < kPrefixCount; ++index) {
        SIPrefix prefix = (SIPrefix)kSIPrefixExponents[index];
        OCStringRef prefixSymbol = prefixSymbolForSIPrefix(prefix);
        OCStringRef prefixName = prefixNameForSIPrefix(prefix);
        if (!prefixSymbol || !prefixName) {
            if (error && *error == NULL) {
                *error = STR("ERROR: Could not get prefix symbol or name");
            }
            OCRelease(prefixedName);
            OCRelease(prefixedPluralName);
            OCRelease(prefixedSymbol);
            return false;
        }
        // Reset the mutable strings to original values for each iteration
        OCStringDelete(prefixedName, OCRangeMake(0, OCStringGetLength(prefixedName)));
        OCStringDelete(prefixedPluralName, OCRangeMake(0, OCStringGetLength(prefixedPluralName)));
        OCStringDelete(prefixedSymbol, OCRangeMake(0, OCStringGetLength(prefixedSymbol)));
        OCStringAppend(prefixedName, name);
        OCStringAppend(prefixedPluralName, plural_name);
        OCStringAppend(prefixedSymbol, symbol);
        // Prefix the names and symbol
        OCStringInsert(prefixedName, 0, prefixName);
        OCStringInsert(prefixedPluralName, 0, prefixName);
        OCStringInsert(prefixedSymbol, 0, prefixSymbol);
        // Add the prefixed unit to the library
        SIUnit2Ref theUnit = AddToLib(quantity, prefixedName, prefixedPluralName, prefixedSymbol, scale_to_coherent_si * prefixValueForSIPrefix(prefix), error);
        if (!theUnit) {
            OCRelease(prefixedName);
            OCRelease(prefixedPluralName);
            OCRelease(prefixedSymbol);
            return false;
        }
    }
    return true;
}
static OCComparisonResult unitNameLengthSort(const void *val1, const void *val2, void *context) {
    SIUnit2Ref unit1 = (SIUnit2Ref)val1;
    SIUnit2Ref unit2 = (SIUnit2Ref)val2;
    OCStringRef name1 = unit1->name;
    OCStringRef name2 = unit2->name;
    if (name1 == NULL && name2 == NULL)
        return kOCCompareEqualTo;
    if (name1 == NULL) {
        return kOCCompareGreaterThan;
    }
    if (name2 == NULL) {
        return kOCCompareLessThan;
    }
    OCComparisonResult result = kOCCompareEqualTo;
    if (OCStringGetLength(name1) > OCStringGetLength(name2))
        result = kOCCompareLessThan;
    if (OCStringGetLength(name1) < OCStringGetLength(name2))
        result = kOCCompareGreaterThan;
    return result;
}
static bool SIUnit2CreateLibraries(void) {
    setlocale(LC_ALL, "");
    const struct lconv *const currentlocale = localeconv();
    units2Library = OCDictionaryCreateMutable(0);
    units2QuantitiesLibrary = OCDictionaryCreateMutable(0);
    units2DimensionalitiesLibrary = OCDictionaryCreateMutable(0);
    IF_NO_OBJECT_EXISTS_RETURN(units2Library, false);
    IF_NO_OBJECT_EXISTS_RETURN(units2QuantitiesLibrary, false);
    IF_NO_OBJECT_EXISTS_RETURN(units2DimensionalitiesLibrary, false);

    // Initialize all units by including the definitions
#include "SIUnit2Definitions.h"

    // fprintf(stderr,"\nSIUnitLibrary: %d units loaded.\n", (int)OCDictionaryGetCount(units2Library));
    // Set to UK or US units.
    imperialVolumes = true;
    SIUnitsLibrarySetImperialVolumes(false);
    if (currentlocale->currency_symbol) {
        if (strcmp(currentlocale->currency_symbol, "£") == 0) {
            imperialVolumes = false;
            SIUnitsLibrarySetImperialVolumes(true);
        }
    }
    OCArrayRef allUnits = OCDictionaryCreateArrayWithAllValues(units2Library);
    units2NamesLibrary = OCArrayCreateMutableCopy(allUnits);
    OCRelease(allUnits);
    OCArraySortValues(units2NamesLibrary, OCRangeMake(0, OCArrayGetCount(units2NamesLibrary)), unitNameLengthSort, NULL);
    return true;
}
// Add a cleanup function for static dictionaries and array
void SIUnit2LibrariesRelease(void) {
    if (!units2Library)
        return;
    if (units2QuantitiesLibrary) {
        OCRelease(units2QuantitiesLibrary);
        units2QuantitiesLibrary = NULL;
    }
    if (units2DimensionalitiesLibrary) {
        OCRelease(units2DimensionalitiesLibrary);
        units2DimensionalitiesLibrary = NULL;
    }
    if (units2NamesLibrary) {
        OCRelease(units2NamesLibrary);
        units2NamesLibrary = NULL;
    }
    
    // Simple approach: clear static flags on all units, then let OCDictionary handle cleanup
    // OCDictionary should only release each unique pointer once, regardless of how many keys point to it
    OCArrayRef keys = OCDictionaryCreateArrayWithAllKeys((OCDictionaryRef)units2Library);
    if (keys) {
        for (uint64_t i = 0; i < OCArrayGetCount(keys); i++) {
            OCStringRef key = (OCStringRef)OCArrayGetValueAtIndex(keys, i);
            SIUnitRef unit = (SIUnitRef)OCDictionaryGetValue(units2Library, key);
            OCDictionaryRemoveValue(units2Library, key);
            OCTypeSetStaticInstance(unit, false);
            OCRelease(unit);
        }
        OCRelease(keys);
    }
    
    // OCDictionary should handle duplicate pointers correctly during its own cleanup
    OCRelease(units2Library);
    units2Library = NULL;
}

SIUnit2Ref SIUnit2WithSymbol(OCStringRef symbol) {
    if (NULL == symbol) {
        return NULL;
    }
    if (NULL == units2Library) SIUnit2CreateLibraries();
    IF_NO_OBJECT_EXISTS_RETURN(units2Library, NULL);
    SIUnit2Ref unit = OCDictionaryGetValue(units2Library, symbol);
    return unit;
}
static bool SIUnit2LibraryRemoveUnitWithSymbol(OCStringRef symbol) {
    if (NULL == units2Library) SIUnit2CreateLibraries();
    if (OCDictionaryContainsKey(units2Library, symbol)) {
        SIUnit2Ref unit = (SIUnit2Ref)OCDictionaryGetValue(units2Library, symbol);
        OCDictionaryRemoveValue(units2Library, symbol);
        OCTypeSetStaticInstance(unit, false);
        OCRelease(unit);
        return true;
    }
    return false;
}

static bool SIUnit2LibraryRemovePlainVolumeUnits(void) {
    bool success = true;
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("gal"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("qt"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("pt"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("cup"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("gi"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("floz"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("tbsp"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("tsp"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("halftsp"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("quartertsp"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("(1/gal)"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("(1/qt)"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("(1/pt)"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("(1/cup)"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("(1/gi)"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("(1/floz)"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("(1/tbsp)"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("(1/tsp)"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("mi/gal"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("gal/h"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("gal/min"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("gal/s"));
    return success;
}
static bool SIUnit2LibraryRemoveUSLabeledVolumeUnits(void) {
    bool success = true;
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("galUS"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("qtUS"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("ptUS"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("cupUS"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("giUS"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("flozUS"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("tbspUS"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("tspUS"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("halftspUS"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("quartertspUS"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("(1/galUS)"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("(1/qtUS)"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("(1/ptUS)"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("(1/cupUS)"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("(1/giUS)"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("(1/flozUS)"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("(1/tbspUS)"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("(1/tspUS)"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("mi/galUS"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("galUS/h"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("galUS/min"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("galUS/s"));
    return success;
}
static bool SIUnit2LibraryRemoveUKLabeledVolumeUnits(void) {
    bool success = true;
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("galUK"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("qtUK"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("ptUK"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("cupUK"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("giUK"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("flozUK"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("tbspUK"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("tspUK"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("halftspUK"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("quartertspUK"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("(1/galUK)"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("(1/qtUK)"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("(1/ptUK)"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("(1/cupUK)"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("(1/giUK)"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("(1/flozUK)"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("(1/tbspUK)"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("(1/tspUK)"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("mi/galUK"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("galUK/h"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("galUK/min"));
    success *= SIUnit2LibraryRemoveUnitWithSymbol(STR("galUK/s"));
    return success;
}

static bool SIUnit2LibraryAddUSLabeledVolumeUnits(OCStringRef *error) {
    // Define US Volume units
    // Volume
    AddToLib(kSIQuantityVolume, STR("US gallon"), STR("US gallons"), STR("galUS"), 0.003785411784, error);
    AddToLib(kSIQuantityVolume, STR("US quart"), STR("US quarts"), STR("qtUS"), 0.003785411784 / 4, error);
    AddToLib(kSIQuantityVolume, STR("US pint"), STR("US pints"), STR("ptUS"), 0.003785411784 / 8, error);
    AddToLib(kSIQuantityVolume, STR("US cup"), STR("US cups"), STR("cupUS"), 0.003785411784 / 16, error);
    AddToLib(kSIQuantityVolume, STR("US gill"), STR("US gills"), STR("giUS"), 0.003785411784 / 32, error);
    AddToLib(kSIQuantityVolume, STR("US fluid ounce"), STR("US fluid ounces"), STR("flozUS"), 0.003785411784 / 128, error);
    AddToLib(kSIQuantityVolume, STR("US tablespoon"), STR("US tablespoons"), STR("tbspUS"), 0.003785411784 / 256, error);
    AddToLib(kSIQuantityVolume, STR("US teaspoon"), STR("US teaspoons"), STR("tspUS"), 0.003785411784 / 768, error);
    AddToLib(kSIQuantityVolume, STR("US half teaspoon"), STR("US half teaspoons"), STR("halftspUS"), 0.003785411784 / 768 / 2., error);
    AddToLib(kSIQuantityVolume, STR("US quarter teaspoon"), STR("US quarter teaspoons"), STR("quartertspUS"), 0.003785411784 / 768 / 4., error);
    // Inverse Volume
    AddToLib(kSIQuantityInverseVolume, STR("inverse US gallon"), STR("inverse US gallons"), STR("(1/galUS)"), 1. / 0.003785411784, error);
    AddToLib(kSIQuantityInverseVolume, STR("inverse US quart"), STR("inverse US quarts"), STR("(1/qtUS)"), 1. / (0.003785411784 / 4), error);
    AddToLib(kSIQuantityInverseVolume, STR("inverse US pint"), STR("inverse US pints"), STR("(1/ptUS)"), 1. / (0.003785411784 / 8), error);
    AddToLib(kSIQuantityInverseVolume, STR("inverse US cup"), STR("inverse US cups"), STR("(1/cupUS)"), 1. / (0.003785411784 / 16), error);
    AddToLib(kSIQuantityInverseVolume, STR("inverse US gill"), STR("inverse US gills"), STR("(1/giUS)"), 1. / (0.003785411784 / 32), error);
    AddToLib(kSIQuantityInverseVolume, STR("inverse US fluid ounce"), STR("inverse US fluid ounces"), STR("(1/flozUS)"), 1. / (0.003785411784 / 128), error);
    AddToLib(kSIQuantityInverseVolume, STR("inverse US tablespoon"), STR("inverse US tablespoons"), STR("(1/tbspUS)"), 1. / (0.003785411784 / 256), error);
    AddToLib(kSIQuantityInverseVolume, STR("inverse US teaspoon"), STR("inverse US teaspoon"), STR("(1/tspUS)"), 1. / (0.003785411784 / 768), error);
    // Distance / Volume
    AddToLib(kSIQuantityLengthPerVolume, STR("mile per US gallon"), STR("miles per US gallon"), STR("mi/galUS"), 1609.344 / 0.003785411784, error);
    AddToLib(kSIQuantityVolumetricFlowRate, STR("US gallon per hour"), STR("US gallons per hour"), STR("galUS/h"), 0.003785411784 / 3600., error);
    AddToLib(kSIQuantityVolumetricFlowRate, STR("US gallon per minute"), STR("US gallons per minute"), STR("galUS/min"), 0.003785411784 / 60., error);
    AddToLib(kSIQuantityVolumetricFlowRate, STR("US gallon per second"), STR("US gallons per second"), STR("galUS/s"), 0.003785411784, error);
}
static bool SIUnitsSetUSPlainVolumeUnits(OCStringRef *error) {
    AddToLib(kSIQuantityVolume, STR("gallon"), STR("gallons"), STR("gal"), 0.003785411784, error);
    AddToLib(kSIQuantityVolume, STR("quart"), STR("quarts"), STR("qt"), 0.003785411784 / 4, error);
    AddToLib(kSIQuantityVolume, STR("pint"), STR("pints"), STR("pt"), 0.003785411784 / 8, error);
    AddToLib(kSIQuantityVolume, STR("cup"), STR("cups"), STR("cup"), 0.003785411784 / 16, error);
    AddToLib(kSIQuantityVolume, STR("gill"), STR("gill"), STR("gi"), 0.003785411784 / 32, error);
    AddToLib(kSIQuantityVolume, STR("fluid ounce"), STR("fluid ounces"), STR("floz"), 0.003785411784 / 128, error);
    AddToLib(kSIQuantityVolume, STR("tablespoon"), STR("tablespoons"), STR("tbsp"), 0.003785411784 / 256, error);
    AddToLib(kSIQuantityVolume, STR("teaspoon"), STR("teaspoons"), STR("tsp"), 0.003785411784 / 768, error);
    AddToLib(kSIQuantityVolume, STR("half teaspoon"), STR("half teaspoons"), STR("halftsp"), 0.003785411784 / 768 / 2, error);
    AddToLib(kSIQuantityVolume, STR("quarter teaspoon"), STR("quarter teaspoons"), STR("quartertsp"), 0.003785411784 / 768 / 4, error);
    // Inverse Volume
    AddToLib(kSIQuantityInverseVolume, STR("inverse gallon"), STR("inverse gallons"), STR("(1/gal)"), 1. / 0.003785411784, error);
    AddToLib(kSIQuantityInverseVolume, STR("inverse quart"), STR("inverse quarts"), STR("(1/qt)"), 1. / (0.003785411784 / 4), error);
    AddToLib(kSIQuantityInverseVolume, STR("inverse pint"), STR("inverse pints"), STR("(1/pt)"), 1. / (0.003785411784 / 8), error);
    AddToLib(kSIQuantityInverseVolume, STR("inverse cup"), STR("inverse cups"), STR("(1/cup)"), 1. / (0.003785411784 / 16), error);
    AddToLib(kSIQuantityInverseVolume, STR("inverse gill"), STR("inverse gill"), STR("(1/gi)"), 1. / (0.003785411784 / 32), error);
    AddToLib(kSIQuantityInverseVolume, STR("inverse fluid ounce"), STR("inverse fluid ounces"), STR("(1/floz)"), 1. / (0.003785411784 / 128), error);
    AddToLib(kSIQuantityInverseVolume, STR("inverse tablespoon"), STR("inverse tablespoons"), STR("(1/tbsp)"), 1. / (0.003785411784 / 256), error);
    AddToLib(kSIQuantityInverseVolume, STR("inverse teaspoon"), STR("inverse teaspoon"), STR("(1/tsp)"), 1. / (0.003785411784 / 768), error);
    // Distance / Volume
    AddToLib(kSIQuantityLengthPerVolume, STR("mile per gallon"), STR("miles per gallon"), STR("mi/gal"), 1609.344 / 0.003785411784, error);
    AddToLib(kSIQuantityVolumetricFlowRate, STR("gallon per hour"), STR("gallons per hour"), STR("gal/h"), 0.003785411784 / 3600., error);
    AddToLib(kSIQuantityVolumetricFlowRate, STR("gallon per minute"), STR("gallons per minute"), STR("gal/min"), 0.003785411784 / 60., error);
    AddToLib(kSIQuantityVolumetricFlowRate, STR("gallon per second"), STR("gallons per second"), STR("gal/s"), 0.003785411784, error);
}
static bool SIUnitSetUKPlainVolumeUnits(OCStringRef *error) {
    // Volume
    AddToLib(kSIQuantityVolume, STR("gallon"), STR("gallons"), STR("gal"), 0.00454609, error);
    AddToLib(kSIQuantityVolume, STR("quart"), STR("quarts"), STR("qt"), 0.00454609 / 4, error);
    AddToLib(kSIQuantityVolume, STR("pint"), STR("pints"), STR("pt"), 0.00454609 / 8, error);
    AddToLib(kSIQuantityVolume, STR("cup"), STR("cups"), STR("cup"), 0.00454609 / 16, error);
    AddToLib(kSIQuantityVolume, STR("gill"), STR("gill"), STR("gi"), 0.00454609 / 32, error);
    AddToLib(kSIQuantityVolume, STR("fluid ounce"), STR("fluid ounces"), STR("floz"), 0.00454609 / 160, error);
    AddToLib(kSIQuantityVolume, STR("tablespoon"), STR("tablespoons"), STR("tbsp"), 0.00454609 / 256, error);
    AddToLib(kSIQuantityVolume, STR("teaspoon"), STR("teaspoons"), STR("tsp"), 0.00454609 / 768, error);
    AddToLib(kSIQuantityVolume, STR("half teaspoon"), STR("half teaspoons"), STR("halftsp"), 0.00454609 / 768 / 2., error);
    AddToLib(kSIQuantityVolume, STR("quarter teaspoon"), STR("quarter teaspoons"), STR("quartertsp"), 0.00454609 / 768 / 4., error);
    // Inverse Volume
    AddToLib(kSIQuantityInverseVolume, STR("inverse gallon"), STR("inverse gallons"), STR("(1/gal)"), 1. / 0.00454609, error);
    AddToLib(kSIQuantityInverseVolume, STR("inverse quart"), STR("inverse quarts"), STR("(1/qt)"), 1. / (0.00454609 / 4), error);
    AddToLib(kSIQuantityInverseVolume, STR("inverse pint"), STR("inverse pints"), STR("(1/pt)"), 1. / (0.00454609 / 8), error);
    AddToLib(kSIQuantityInverseVolume, STR("inverse cup"), STR("inverse cups"), STR("(1/cup)"), 1. / (0.00454609 / 16), error);
    AddToLib(kSIQuantityInverseVolume, STR("inverse gill"), STR("inverse gills"), STR("(1/gi)"), 1. / (0.00454609 / 32), error);
    AddToLib(kSIQuantityInverseVolume, STR("inverse fluid ounce"), STR("inverse fluid ounces"), STR("(1/floz)"), 1. / (0.00454609 / 160), error);
    AddToLib(kSIQuantityInverseVolume, STR("inverse tablespoon"), STR("inverse tablespoons"), STR("(1/tbsp)"), 1. / (0.00454609 / 256), error);
    AddToLib(kSIQuantityInverseVolume, STR("inverse teaspoon"), STR("inverse teaspoon"), STR("(1/tsp)"), 1. / (0.00454609 / 768), error);
    // Distance / Volume
    AddToLib(kSIQuantityLengthPerVolume, STR("mile per gallon"), STR("miles per gallon"), STR("mi/gal"), 1609.344 / 0.00454609, error);
    AddToLib(kSIQuantityVolumetricFlowRate, STR("gallon per hour"), STR("gallons per hour"), STR("gal/h"), 0.00454609 / 3600., error);
    AddToLib(kSIQuantityVolumetricFlowRate, STR("gallon per minute"), STR("gallons per minute"), STR("gal/min"), 0.00454609 / 60., error);
    AddToLib(kSIQuantityVolumetricFlowRate, STR("gallon per second"), STR("gallons per second"), STR("gal/s"), 0.00454609, error);
}
static bool SIUnitAddUKLabeledVolumeUnits(OCStringRef *error) {
    AddToLib(kSIQuantityVolume, STR("imperial gallon"), STR("imperial gallons"), STR("galUK"), 0.00454609, error);
    AddToLib(kSIQuantityVolume, STR("imperial quart"), STR("imperial quarts"), STR("qtUK"), 0.00454609 / 4, error);
    AddToLib(kSIQuantityVolume, STR("imperial pint"), STR("imperial pints"), STR("ptUK"), 0.00454609 / 8, error);
    AddToLib(kSIQuantityVolume, STR("imperial cup"), STR("imperial cups"), STR("cupUK"), 0.00454609 / 16, error);
    AddToLib(kSIQuantityVolume, STR("imperial gill"), STR("imperial gill"), STR("giUK"), 0.00454609 / 32, error);
    AddToLib(kSIQuantityVolume, STR("imperial fluid ounce"), STR("imperial fluid ounces"), STR("flozUK"), 0.00454609 / 160, error);
    AddToLib(kSIQuantityVolume, STR("imperial tablespoon"), STR("imperial tablespoons"), STR("tbspUK"), 0.00454609 / 256, error);
    AddToLib(kSIQuantityVolume, STR("imperial teaspoon"), STR("imperial teaspoons"), STR("tspUK"), 0.00454609 / 768, error);
    AddToLib(kSIQuantityVolume, STR("imperial half teaspoon"), STR("imperial half teaspoons"), STR("halftspUK"), 0.00454609 / 768 / 2, error);
    AddToLib(kSIQuantityVolume, STR("imperial quarter teaspoon"), STR("imperial quarter teaspoons"), STR("quartertspUK"), 0.00454609 / 768 / 4, error);
    // Inverse Volume
    AddToLib(kSIQuantityInverseVolume, STR("inverse imperial gallon"), STR("inverse imperial gallons"), STR("(1/galUK)"), 1. / 0.00454609, error);
    AddToLib(kSIQuantityInverseVolume, STR("inverse imperial quart"), STR("inverse imperial quarts"), STR("(1/qtUK)"), 1. / (0.00454609 / 4), error);
    AddToLib(kSIQuantityInverseVolume, STR("inverse imperial pint"), STR("inverse imperial pints"), STR("(1/ptUK)"), 1. / (0.00454609 / 8), error);
    AddToLib(kSIQuantityInverseVolume, STR("inverse imperial cup"), STR("inverse imperial cups"), STR("(1/cupUK)"), 1. / (0.00454609 / 16), error);
    AddToLib(kSIQuantityInverseVolume, STR("inverse imperial gill"), STR("inverse imperial gills"), STR("(1/giUK)"), 1. / (0.00454609 / 32), error);
    AddToLib(kSIQuantityInverseVolume, STR("inverse imperial fluid ounce"), STR("inverse imperial fluid ounces"), STR("(1/flozUK)"), 1. / (0.00454609 / 160), error);
    AddToLib(kSIQuantityInverseVolume, STR("inverse imperial tablespoon"), STR("inverse imperial tablespoons"), STR("(1/tbspUK)"), 1. / (0.00454609 / 256), error);
    AddToLib(kSIQuantityInverseVolume, STR("inverse imperial teaspoon"), STR("inverse imperial teaspoon"), STR("(1/tspUK)"), 1. / (0.00454609 / 768), error);
    // Distance / Volume
    AddToLib(kSIQuantityLengthPerVolume, STR("mile per imperial gallon"), STR("miles per imperial gallon"), STR("mi/galUK"), 1609.344 / 0.00454609, error);
    AddToLib(kSIQuantityVolumetricFlowRate, STR("imperial gallon per hour"), STR("imperial gallons per hour"), STR("galUK/h"), 0.00454609 / 3600., error);
    AddToLib(kSIQuantityVolumetricFlowRate, STR("imperial gallon per minute"), STR("imperial gallons per minute"), STR("galUK/min"), 0.00454609 / 60., error);
    AddToLib(kSIQuantityVolumetricFlowRate, STR("imperial gallon per second"), STR("imperial gallons per second"), STR("galUK/s"), 0.00454609, error);
}
void SIUnit2LibrarySetImperialVolumes(bool value) {
    if (imperialVolumes == value) return;
    OCStringRef error = NULL;
    SIUnit2LibraryRemovePlainVolumeUnits();
    if (value) {
        SIUnit2LibraryRemoveUKLabeledVolumeUnits();  // Remove Imperial Volumes
        SIUnit2LibraryAddUSLabeledVolumeUnits(&error);     // Define US Volume units
        SIUnitSetUKPlainVolumeUnits(&error);       // Define UK Volume units
    } else {
        SIUnit2LibraryRemoveUSLabeledVolumeUnits();  // Remove US Volumes
        SIUnitsSetUSPlainVolumeUnits(&error);      // Define US Volume units
        SIUnitAddUKLabeledVolumeUnits(&error);     // Define UK Volume units
    }
    imperialVolumes = value;
}


static SIUnit2Ref SIUnit2WithParameters(SIDimensionalityRef dimensionality,
                                        OCStringRef name,
                                        OCStringRef plural_name,
                                        OCStringRef symbol,
                                        double scale_to_coherent_si) {
    if (NULL == units2Library) SIUnit2CreateLibraries();
    // Create a temporary unit to get its symbol, then check if equivalent exists
    SIUnit2Ref tempUnit = SIUnit2Create(dimensionality, name, plural_name, symbol, scale_to_coherent_si);
    if (NULL == tempUnit)
        return NULL;
    // Check if unit with this symbol already exists
    if (OCDictionaryContainsKey(units2Library, tempUnit->symbol)) {
        SIUnit2Ref existingUnit = OCDictionaryGetValue(units2Library, tempUnit->symbol);
        OCRelease(tempUnit);  // Discard the temporary unit
        return existingUnit;
    }
    // No existing unit found, so add this fresh unit to library
    OCTypeSetStaticInstance(tempUnit, true);
    OCDictionaryAddValue(units2Library, tempUnit->symbol, tempUnit);
    OCRelease(tempUnit);
    return tempUnit;
}
SIUnit2Ref SIUnit2WithCoherentUnit(SIDimensionalityRef dimensionality) {
    OCMutableStringRef symbol = OCStringCreateMutableCopy(SIDimensionalityGetSymbol(dimensionality));
    // Create Coherent Unit symbol by simply replacing base dimensionality symbols with base SI symbols
    OCStringFindAndReplace2(symbol, STR("L"), STR("m"));
    OCStringFindAndReplace2(symbol, STR("M"), STR("kg"));
    OCStringFindAndReplace2(symbol, STR("T"), STR("s"));
    OCStringFindAndReplace2(symbol, STR("I"), STR("A"));
    OCStringFindAndReplace2(symbol, STR("N"), STR("mol"));
    OCStringFindAndReplace2(symbol, STR("J"), STR("cd"));
    return SIUnit2WithParameters(dimensionality, NULL, NULL, symbol, 1);
}
OCArrayRef SIUnit2CreateArrayOfUnitsForQuantity(OCStringRef quantity) {
    IF_NO_OBJECT_EXISTS_RETURN(quantity, NULL);
    if (NULL == units2Library) SIUnit2CreateLibraries();
    if (OCDictionaryContainsKey(units2QuantitiesLibrary, quantity)) {
        OCMutableArrayRef array = (OCMutableArrayRef)OCDictionaryGetValue(units2QuantitiesLibrary, quantity);
        return OCArrayCreateCopy(array);
    }
    return NULL;
}
OCArrayRef SIUnit2CreateArrayOfUnitsForDimensionality(SIDimensionalityRef theDim) {
    IF_NO_OBJECT_EXISTS_RETURN(theDim, NULL);
    if (NULL == units2Library) SIUnit2CreateLibraries();
    OCStringRef symbol = SIDimensionalityGetSymbol(theDim);
    if (OCDictionaryContainsKey(units2DimensionalitiesLibrary, symbol)) {
        OCMutableArrayRef array = (OCMutableArrayRef)OCDictionaryGetValue(units2DimensionalitiesLibrary, symbol);
        return OCArrayCreateCopy(array);
    }
    return NULL;
}
OCArrayRef SIUnit2CreateArrayOfUnitsForSameReducedDimensionality(SIDimensionalityRef theDim) {
    IF_NO_OBJECT_EXISTS_RETURN(theDim, NULL);
    if (NULL == units2Library) SIUnit2CreateLibraries();
    OCArrayRef dimensionalities = SIDimensionalityCreateArrayWithSameReducedDimensionality(theDim);
    OCMutableArrayRef result = OCArrayCreateMutable(0, &kOCTypeArrayCallBacks);
    for (uint64_t index = 0; index < OCArrayGetCount(dimensionalities); index++) {
        SIDimensionalityRef dimensionality = OCArrayGetValueAtIndex(dimensionalities, index);
        OCStringRef symbol = SIDimensionalityGetSymbol(dimensionality);
        if (OCDictionaryContainsKey(units2DimensionalitiesLibrary, symbol)) {
            OCMutableArrayRef array = (OCMutableArrayRef)OCDictionaryGetValue(units2DimensionalitiesLibrary, symbol);
            OCArrayAppendArray(result, array, OCRangeMake(0, OCArrayGetCount(array)));
        }
    }
    OCRelease(dimensionalities);
    return result;
}
SIUnit2Ref SIUnit2DimensionlessAndUnderived(void) {
    return SIUnit2WithParameters(SIDimensionalityDimensionless(), NULL, NULL, STR(" "), 1);
}
//
// Common helper function for finding best matching unit
static SIUnit2Ref SIUnit2FindBestMatchingUnit(SIDimensionalityRef dimensionality, double target_scale) {
    OCArrayRef candidates = SIUnit2CreateArrayOfUnitsForDimensionality(dimensionality);
    if (!candidates || OCArrayGetCount(candidates) == 0) {
        if (candidates) OCRelease(candidates);
        return NULL;
    }
    
    SIUnit2Ref best_match = NULL;
    OCStringRef shortest_symbol = NULL;
    
    for (OCIndex i = 0; i < OCArrayGetCount(candidates); i++) {
        SIUnit2Ref candidate = (SIUnit2Ref)OCArrayGetValueAtIndex(candidates, i);
        double scale_diff = fabs(candidate->scale_to_coherent_si - target_scale);
        
        // Consider it a close match if within 1% relative error
        if (scale_diff < 0.01 * target_scale) {
            if (!best_match ||
                OCStringGetLength(candidate->symbol) < OCStringGetLength(shortest_symbol)) {
                best_match = candidate;
                shortest_symbol = candidate->symbol;
            }
        }
    }
    
    OCRelease(candidates);
    return best_match;
}

// Common helper function for creating and simplifying symbols
static OCStringRef SIUnit2CreateSimplifiedSymbol(OCStringRef raw_symbol, bool reduce, OCStringRef *error) {
    OCStringRef simplified_symbol;
    if (reduce) {
        simplified_symbol = SIUnitReduceExpression(raw_symbol);
    } else {
        simplified_symbol = SIUnitCreateLibraryKey(raw_symbol);
    }
    
    if (!simplified_symbol) {
        simplified_symbol = raw_symbol;  // Fallback to raw symbol
        OCRetain(simplified_symbol);     // Balance the retain for cleanup
    }
    
    return simplified_symbol;
}

// Power operation
static SIUnit2Ref SIUnit2ByRaisingToPowerInternal(SIUnit2Ref input,
                                                  int power,
                                                  double *unit_multiplier,
                                                  bool reduce,
                                                  OCStringRef *error) {
    if (error && *error) return NULL;
    IF_NO_OBJECT_EXISTS_RETURN(input, NULL);
    if (power == 0) return SIUnit2DimensionlessAndUnderived();
    if (power == 1) return input;
    
    // Choose the appropriate dimensionality function based on parameter
    SIDimensionalityRef dimensionality;
    if (reduce) {
        dimensionality = SIDimensionalityByRaisingToPower(input->dimensionality, power, error);
    } else {
        dimensionality = SIDimensionalityByRaisingToPowerWithoutReducing(input->dimensionality, power, error);
    }
    if (!dimensionality) return NULL;
    
    // Calculate the new scale factor
    double scale = pow(*unit_multiplier, power);
    *unit_multiplier = scale;
    
    // First approach: Look for existing units with matching dimensionality
    SIUnit2Ref best_match = SIUnit2FindBestMatchingUnit(dimensionality, scale);
    if (best_match) {
        return best_match;
    }
    
    // Second approach: Create new symbol by wrapping input symbol with "( )^power"
    OCMutableStringRef new_symbol = OCStringCreateMutable(0);
    if (!new_symbol) {
        if (error && !*error) *error = STR("ERROR: Could not create mutable string for new symbol");
        return NULL;
    }
    
    OCStringAppend(new_symbol, STR("("));
    OCStringAppend(new_symbol, input->symbol);
    OCStringAppend(new_symbol, STR(")^"));
    
    // Add the power
    char power_str[32];
    snprintf(power_str, sizeof(power_str), "%d", power);
    OCStringRef power_string = OCStringCreateWithCString(power_str);
    OCStringAppend(new_symbol, power_string);
    OCRelease(power_string);
    
    // Simplify the symbol
    OCStringRef simplified_symbol = SIUnit2CreateSimplifiedSymbol(new_symbol, reduce, error);
    
    // Create new unit with the simplified symbol and calculated scale
    SIUnit2Ref result = SIUnit2WithParameters(dimensionality,NULL,NULL,simplified_symbol,scale);
    
    OCRelease(new_symbol);
    OCRelease(simplified_symbol);
    return result;
}

// Multiplication operation
static SIUnit2Ref SIUnit2ByMultiplyingInternal(SIUnit2Ref theUnit1,
                                               SIUnit2Ref theUnit2,
                                               double *unit_multiplier,
                                               bool reduce,
                                               OCStringRef *error) {
    if (error && *error) return NULL;
    IF_NO_OBJECT_EXISTS_RETURN(theUnit1, NULL);
    IF_NO_OBJECT_EXISTS_RETURN(theUnit2, NULL);
    
    // multiplying by dimensionless does nothing
    SIUnit2Ref dimless = SIUnit2DimensionlessAndUnderived();
    if (theUnit1 == dimless) return theUnit2;
    if (theUnit2 == dimless) return theUnit1;
    if (SIUnit2AreEquivalentUnits(theUnit1, theUnit2)) {
        return SIUnit2ByRaisingToPowerInternal(theUnit1, 2, unit_multiplier, reduce, error);
    }
    
    // Compute new dimensionality
    SIDimensionalityRef dimensionality;
    if (reduce) {
        dimensionality = SIDimensionalityByMultiplying(theUnit1->dimensionality, theUnit2->dimensionality, error);
    } else {
        dimensionality = SIDimensionalityByMultiplyingWithoutReducing(theUnit1->dimensionality, theUnit2->dimensionality, error);
    }
    if (error && *error) return NULL;
    
    // Calculate the new scale factor
    double scale = theUnit1->scale_to_coherent_si * theUnit2->scale_to_coherent_si;
    *unit_multiplier = scale;
    
    // First approach: Look for existing units with matching dimensionality
    SIUnit2Ref best_match = SIUnit2FindBestMatchingUnit(dimensionality, scale);
    if (best_match) {
        return best_match;
    }
    
    // Second approach: Create new symbol by wrapping input symbols with "(unit1)*(unit2)"
    OCMutableStringRef new_symbol = OCStringCreateMutable(0);
    if (!new_symbol) {
        if (error && !*error) *error = STR("ERROR: Could not create mutable string for new symbol");
        return NULL;
    }
    
    OCStringAppend(new_symbol, STR("("));
    OCStringAppend(new_symbol, theUnit1->symbol);
    OCStringAppend(new_symbol, STR(")*("));
    OCStringAppend(new_symbol, theUnit2->symbol);
    OCStringAppend(new_symbol, STR(")"));
    
    // Simplify the symbol
    OCStringRef simplified_symbol = SIUnit2CreateSimplifiedSymbol(new_symbol, reduce, error);
    
    // Create new unit with the simplified symbol and calculated scale
    SIUnit2Ref result = SIUnit2WithParameters(dimensionality,NULL,NULL,simplified_symbol,scale);
    
    OCRelease(new_symbol);
    OCRelease(simplified_symbol);
    return result;
}

// Public API functions - now more intuitive with 'reduce' parameter
SIUnit2Ref SIUnit2ByRaisingToPowerWithoutReducing(SIUnit2Ref input,
                                                  int power,
                                                  double *unit_multiplier,
                                                  OCStringRef *error) {
    return SIUnit2ByRaisingToPowerInternal(input, power, unit_multiplier, false, error);  // reduce = false
}

SIUnit2Ref SIUnit2ByRaisingToPower(SIUnit2Ref input,
                                   int power,
                                   double *unit_multiplier,
                                   OCStringRef *error) {
    return SIUnit2ByRaisingToPowerInternal(input, power, unit_multiplier, true, error);   // reduce = true
}

SIUnit2Ref SIUnit2ByMultiplyingWithoutReducing(SIUnit2Ref theUnit1,
                                               SIUnit2Ref theUnit2,
                                               double *unit_multiplier,
                                               OCStringRef *error) {
    return SIUnit2ByMultiplyingInternal(theUnit1, theUnit2, unit_multiplier, false, error);  // reduce = false
}

SIUnit2Ref SIUnit2ByMultiplying(SIUnit2Ref theUnit1,
                                SIUnit2Ref theUnit2,
                                double *unit_multiplier,
                                OCStringRef *error) {
    return SIUnit2ByMultiplyingInternal(theUnit1, theUnit2, unit_multiplier, true, error);   // reduce = true
}

// Division operation
static SIUnit2Ref SIUnit2ByDividingInternal(SIUnit2Ref theUnit1,
                                            SIUnit2Ref theUnit2,
                                            double *unit_multiplier,
                                            bool reduce,
                                            OCStringRef *error) {
    if (error && *error) return NULL;
    IF_NO_OBJECT_EXISTS_RETURN(theUnit1, NULL);
    IF_NO_OBJECT_EXISTS_RETURN(theUnit2, NULL);
    
    // dividing by dimensionless does nothing
    SIUnit2Ref dimless = SIUnit2DimensionlessAndUnderived();
    if (theUnit2 == dimless) return theUnit1;
    if (SIUnit2AreEquivalentUnits(theUnit1, theUnit2)) {
        return SIUnit2DimensionlessAndUnderived();
    }
    
    // Compute new dimensionality
    SIDimensionalityRef dimensionality;
    if (reduce) {
        dimensionality = SIDimensionalityByDividing(theUnit1->dimensionality, theUnit2->dimensionality);
    } else {
        dimensionality = SIDimensionalityByDividingWithoutReducing(theUnit1->dimensionality, theUnit2->dimensionality);
    }
    if (error && *error) return NULL;
    
    // Calculate the new scale factor
    double scale = theUnit1->scale_to_coherent_si / theUnit2->scale_to_coherent_si;
    *unit_multiplier = scale;
    
    // First approach: Look for existing units with matching dimensionality
    SIUnit2Ref best_match = SIUnit2FindBestMatchingUnit(dimensionality, scale);
    if (best_match) {
        return best_match;
    }
    
    // Second approach: Create new symbol by wrapping input symbols with "(unit1)/(unit2)"
    OCMutableStringRef new_symbol = OCStringCreateMutable(0);
    if (!new_symbol) {
        if (error && !*error) *error = STR("ERROR: Could not create mutable string for new symbol");
        return NULL;
    }
    
    OCStringAppend(new_symbol, STR("("));
    OCStringAppend(new_symbol, theUnit1->symbol);
    OCStringAppend(new_symbol, STR(")/("));
    OCStringAppend(new_symbol, theUnit2->symbol);
    OCStringAppend(new_symbol, STR(")"));
    
    // Simplify the symbol
    OCStringRef simplified_symbol = SIUnit2CreateSimplifiedSymbol(new_symbol, reduce, error);
    
    // Create new unit with the simplified symbol and calculated scale
    SIUnit2Ref result = SIUnit2WithParameters(dimensionality,NULL,NULL,simplified_symbol,scale);
    
    OCRelease(new_symbol);
    OCRelease(simplified_symbol);
    return result;
}

// Public API functions
SIUnit2Ref SIUnit2ByDividingWithoutReducing(SIUnit2Ref theUnit1,
                                            SIUnit2Ref theUnit2,
                                            double *unit_multiplier,
                                            OCStringRef *error) {
    return SIUnit2ByDividingInternal(theUnit1, theUnit2, unit_multiplier, false, error);  // reduce = false
}

SIUnit2Ref SIUnit2ByDividing(SIUnit2Ref theUnit1,
                             SIUnit2Ref theUnit2,
                             double *unit_multiplier,
                             OCStringRef *error) {
    return SIUnit2ByDividingInternal(theUnit1, theUnit2, unit_multiplier, true, error);   // reduce = true
}

// Nth root operation
SIUnit2Ref SIUnit2ByTakingNthRoot(SIUnit2Ref theUnit,
                                                 int root,
                                                 double *unit_multiplier,
                                                 OCStringRef *error) {
    if (error && *error) return NULL;
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, NULL);
    if (root == 0) {
        if (error && !*error) *error = STR("ERROR: Cannot take 0th root of a unit");
        return NULL;
    }
    if (root == 1) return theUnit;
    
    // Compute new dimensionality
    SIDimensionalityRef dimensionality;
    dimensionality = SIDimensionalityByTakingNthRoot(theUnit->dimensionality, root, error);
    if (!dimensionality) return NULL;
    
    // Calculate the new scale factor
    double scale = pow(*unit_multiplier, 1.0 / root);
    *unit_multiplier = scale;
    
    // First approach: Look for existing units with matching dimensionality
    SIUnit2Ref best_match = SIUnit2FindBestMatchingUnit(dimensionality, scale);
    if (best_match) {
        return best_match;
    }
    
    // Second approach: Create new symbol by wrapping input symbol with "( )^(1/root)"
    OCMutableStringRef new_symbol = OCStringCreateMutable(0);
    if (!new_symbol) {
        if (error && !*error) *error = STR("ERROR: Could not create mutable string for new symbol");
        return NULL;
    }
    
    OCStringAppend(new_symbol, STR("("));
    OCStringAppend(new_symbol, theUnit->symbol);
    OCStringAppend(new_symbol, STR(")^(1/"));
    
    // Add the root
    char root_str[32];
    snprintf(root_str, sizeof(root_str), "%d", root);
    OCStringRef root_string = OCStringCreateWithCString(root_str);
    OCStringAppend(new_symbol, root_string);
    OCRelease(root_string);
    
    OCStringAppend(new_symbol, STR(")"));
    
    // Simplify the symbol
    OCStringRef simplified_symbol = SIUnit2CreateSimplifiedSymbol(new_symbol, false, error);
    
    // Create new unit with the simplified symbol and calculated scale
    SIUnit2Ref result = SIUnit2WithParameters(dimensionality,
                                              NULL, 
                                              NULL,
                                              simplified_symbol,
                                              scale);
    
    OCRelease(new_symbol);
    OCRelease(simplified_symbol);
    return result;
}

