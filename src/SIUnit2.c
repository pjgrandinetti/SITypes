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
        cJSON_AddNullToObject(json, "root_plural_name");
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
    double naturalTimeUnit = hbar / (kSIElectronMass * kSISpeedOfLight * kSISpeedOfLight);
    double naturalMomentumUnit = kSIElectronMass * kSISpeedOfLight;
    double naturalEnergyUnit = kSIElectronMass * kSISpeedOfLight * kSISpeedOfLight;
    double alphaParticleMassEnergy = kSIAlphaParticleMass * kSISpeedOfLight * kSISpeedOfLight;
    double secondRadiationConstant = kSIPlanckConstant * kSISpeedOfLight / kSIBoltmannConstant;
    double vonKlitzingConstant = kSIPlanckConstant / (kSIElementaryCharge * kSIElementaryCharge);
    OCStringRef error = NULL;
    AddToLibPrefixed(kSIQuantityLength, STR("meter"), STR("meters"), STR("m"), 1, &error);
    AddToLibPrefixed(kSIQuantityMass, STR("gram"), STR("grams"), STR("g"), 1, &error);
    AddToLibPrefixed(kSIQuantityTime, STR("second"), STR("seconds"), STR("s"), 1, &error);
    AddToLibPrefixed(kSIQuantityCurrent, STR("ampere"), STR("amperes"), STR("A"), 1, &error);
    AddToLibPrefixed(kSIQuantityTemperature, STR("kelvin"), STR("kelvin"), STR("K"), 1, &error);
    AddToLibPrefixed(kSIQuantityAmount, STR("mole"), STR("moles"), STR("mol"), 1, &error);
    AddToLibPrefixed(kSIQuantityLuminousIntensity, STR("candela"), STR("candelas"), STR("cd"), 1, &error);
#pragma mark kSIQuantityDimensionless
    AddToLib(kSIQuantityDimensionless, STR("dimensionless"), STR("dimensionless"), STR(" "), 1, &error);
    AddToLib(kSIQuantityDimensionless, STR("percent"), STR("percent"), STR("%"), 0.01, &error);
    AddToLib(kSIQuantityDimensionless, STR("part per thousand"), STR("parts per thousand"), STR("‰"), 0.001, &error);
    AddToLib(kSIQuantityDimensionless, STR("part per ten thousand"), STR("parts per ten thousand"), STR("‱"), 0.0001, &error);
    AddToLib(kSIQuantityDimensionless, STR("part per million"), STR("parts per million"), STR("ppm"), 1.e-6, &error);
    AddToLib(kSIQuantityDimensionless, STR("part per billion"), STR("parts per billion"), STR("ppb"), 1.e-9, &error);
    AddToLib(kSIQuantityDimensionless, STR("part per trillion"), STR("parts per trillion"), STR("ppt"), 1.e-12, &error);
    AddToLib(kSIQuantityDimensionless, STR("part per quadrillion"), STR("parts per quadrillion"), STR("ppq"), 1.e-15, &error);
    AddToLib(kSIQuantityDimensionless, STR("euler number"), STR("euler number"), STR("exp(1)"), kSIEulersNumber, &error);
//
#pragma mark kSIQuantityFineStructureConstant
    AddToLib(kSIQuantityFineStructureConstant, STR("fine structure constant"), STR("fine structure constant"), STR("α"), alpha, &error);
    AddToLib(kSIQuantityFineStructureConstant, STR("inverse fine structure constant"), STR("inverse fine structure constant"), STR("(1/α)"), 1 / alpha, &error);
//
#pragma mark kSIQuantityLength
    AddToLib(kSIQuantityLength, STR("astronomical unit"), STR("astronomical units"), STR("ua"), 1.49597870691e11, &error);
    AddToLib(kSIQuantityLength, STR("light year"), STR("light years"), STR("ly"), lightYear, &error);
    AddToLib(kSIQuantityLength, STR("ångström"), STR("ångströms"), STR("Å"), 1.e-10, &error);
    AddToLib(kSIQuantityLength, STR("atomic unit of length"), STR("atomic unit of length"), STR("a_0"), a_0, &error);
    //  AddToLib(kSIQuantityLength, STR("nautical mile"), STR("nautical miles"), STR("M"),1852., &error);
    AddToLib(kSIQuantityLength, STR("fathom"), STR("fathoms"), STR("ftm"), 2 * 1609.344 / 1760, &error);
    AddToLib(kSIQuantityLength, STR("inch"), STR("inches"), STR("in"), 1609.344 / 63360, &error);
    AddToLib(kSIQuantityLength, STR("foot"), STR("feet"), STR("ft"), 1609.344 / 5280, &error);
    AddToLib(kSIQuantityLength, STR("yard"), STR("yards"), STR("yd"), 1609.344 / 1760, &error);
    AddToLib(kSIQuantityLength, STR("mile"), STR("miles"), STR("mi"), 1609.344, &error);
    AddToLib(kSIQuantityLength, STR("link"), STR("links"), STR("li"), 1609.344 / 5280 * 33 / 50, &error);
    AddToLib(kSIQuantityLength, STR("rod"), STR("rods"), STR("rod"), 1609.344 / 5280 * 16.5, &error);
    AddToLib(kSIQuantityLength, STR("chain"), STR("chains"), STR("ch"), 1609.344 / 5280 * 16.5 * 4, &error);
    AddToLib(kSIQuantityLength, STR("furlong"), STR("furlongs"), STR("fur"), 1609.344 / 5280 * 16.5 * 4 * 10, &error);
    AddToLib(kSIQuantityLength, STR("league"), STR("leagues"), STR("lea"), 1609.344 * 3, &error);
    AddToLib(kSIQuantityLength, STR("compton wavelength"), STR("compton wavelengths"), STR("λ_C"), kSIPlanckConstant / (kSIElectronMass * kSISpeedOfLight), &error);
    AddToLib(kSIQuantityLength, STR("natural unit of length"), STR("natural units of length"), STR("ƛ_C"), 386.15926764e-15, &error);
    AddToLib(kSIQuantityLength, STR("planck length"), STR("planck length"), STR("l_P"), planckLength, &error);
//
#pragma mark kSIQuantityWavenumber, kSIQuantityInverseLength
    AddToLib(kSIQuantityWavenumber, STR("inverse meter"), STR("inverse meters"), STR("(1/m)"), 1, &error);
    AddToLib(kSIQuantityWavenumber, STR("inverse centimeter"), STR("inverse centimeters"), STR("(1/cm)"), 100, &error);
    AddToLib(kSIQuantityWavenumber, STR("inverse ångström"), STR("inverse ångströms"), STR("(1/Å)"), 1.e10, &error);
    AddToLib(kSIQuantityWavenumber, STR("inverse mile"), STR("inverse miles"), STR("(1/mi)"), 1. / 1609.344, &error);
    AddToLib(kSIQuantityWavenumber, STR("inverse yard"), STR("inverse yards"), STR("(1/yd)"), 1. / (1609.344 / 1760), &error);
    AddToLib(kSIQuantityWavenumber, STR("inverse foot"), STR("inverse feet"), STR("(1/ft)"), 1. / (1609.344 / 5280), &error);
    AddToLib(kSIQuantityWavenumber, STR("inverse inch"), STR("inverse inches"), STR("(1/in)"), 1. / (1609.344 / 63360), &error);
    AddToLib(kSIQuantityWavenumber, STR("rydberg constant"), STR("rydberg constant"), STR("R_∞"), R_H, &error);
    AddToLib(kSIQuantityWavenumber, STR("inverse atomic unit of length"), STR("inverse atomic unit of length"), STR("(1/a_0)"), 1 / a_0, &error);
    OCMutableArrayRef units = (OCMutableArrayRef)OCDictionaryGetValue(units2QuantitiesLibrary, kSIQuantityWavenumber);
    OCDictionaryAddValue(units2QuantitiesLibrary, kSIQuantityInverseLength, units);
//
#pragma mark kSIQuantityLengthRatio, kSIQuantityPlaneAngle
    AddToLibPrefixed(kSIQuantityLengthRatio, STR("meter per meter"), STR("meters per meter"), STR("m/m"), 1.0, &error);
    AddToLibPrefixed(kSIQuantityLengthRatio, STR("radian"), STR("radians"), STR("rad"), 1.0, &error);
    AddToLibPrefixed(kSIQuantityLengthRatio, STR("degree"), STR("degrees"), STR("°"), kSIPi / 180., &error);
    AddToLib(kSIQuantityLengthRatio, STR("pi"), STR("pi"), STR("π"), kSIPi, &error);
    units = (OCMutableArrayRef)OCDictionaryGetValue(units2QuantitiesLibrary, kSIQuantityLengthRatio);
    OCDictionaryAddValue(units2QuantitiesLibrary, kSIQuantityPlaneAngle, units);
//
#pragma mark kSIQuantityMass
    AddToLib(kSIQuantityMass, STR("microgram"), STR("micrograms"), STR("mcg"), 1e-9, &error);
    AddToLibPrefixed(kSIQuantityMass, STR("tonne"), STR("tonnes"), STR("t"), 1e3, &error);
    AddToLibPrefixed(kSIQuantityMass, STR("dalton"), STR("daltons"), STR("Da"), kSIAtomicMassConstant, &error);
    AddToLib(kSIQuantityMass, STR("atomic mass unit"), STR("atomic mass units"), STR("u"), kSIAtomicMassConstant, &error);
    AddToLib(kSIQuantityMass, STR("atomic mass constant"), STR("atomic mass constant"), STR("m_u"), kSIAtomicMassConstant, &error);
    AddToLib(kSIQuantityMass, STR("electron mass"), STR("electron mass"), STR("m_e"), kSIElectronMass, &error);
    AddToLib(kSIQuantityMass, STR("proton mass"), STR("proton mass"), STR("m_p"), kSIProtonMass, &error);
    AddToLib(kSIQuantityMass, STR("neutron mass"), STR("neutron mass"), STR("m_n"), kSINeutronMass, &error);
    AddToLib(kSIQuantityMass, STR("alpha particle mass"), STR("alpha particle mass"), STR("m_a"), kSIAlphaParticleMass, &error);
    AddToLib(kSIQuantityMass, STR("muon mass"), STR("muon mass"), STR("m_µ"), kSIMuonMass, &error);
    AddToLib(kSIQuantityMass, STR("ton"), STR("tons"), STR("ton"), 0.45359237 * 2000, &error);
    AddToLib(kSIQuantityMass, STR("hundredweight"), STR("hundredweight"), STR("cwt"), 0.45359237 * 100, &error);
    AddToLib(kSIQuantityMass, STR("pound"), STR("pounds"), STR("lb"), 0.45359237, &error);
    AddToLib(kSIQuantityMass, STR("stone"), STR("stones"), STR("st"), 6.35029318, &error);
    AddToLib(kSIQuantityMass, STR("ounce"), STR("ounces"), STR("oz"), 0.028349523125, &error);
    AddToLib(kSIQuantityMass, STR("grain"), STR("grains"), STR("gr"), 0.45359237 / 7000, &error);
    AddToLib(kSIQuantityMass, STR("dram"), STR("drams"), STR("dr"), 0.45359237 / 256, &error);
    AddToLib(kSIQuantityMass, STR("tonUK"), STR("tonsUK"), STR("tonUK"), 0.45359237 * 2240, &error);
    AddToLib(kSIQuantityMass, STR("hundredweightUK"), STR("hundredweightUK"), STR("cwtUK"), 0.45359237 * 112, &error);
    AddToLib(kSIQuantityMass, STR("planck mass"), STR("planck mass"), STR("m_P"), planckMass, &error);
//
#pragma mark kSIQuantityInverseMass
    AddToLib(kSIQuantityInverseMass, STR("inverse kilogram"), STR("inverse kilograms"), STR("(1/kg)"), 1., &error);
    AddToLib(kSIQuantityInverseMass, STR("inverse tonne"), STR("inverse tonnes"), STR("(1/t)"), 1. / 1e3, &error);
    AddToLib(kSIQuantityInverseMass, STR("inverse ton"), STR("inverse tons"), STR("(1/ton)"), 1. / (0.45359237 * 2000), &error);
    AddToLib(kSIQuantityInverseMass, STR("inverse stone"), STR("inverse stones"), STR("(1/st)"), 1. / 6.35029318, &error);
    AddToLib(kSIQuantityInverseMass, STR("inverse pound"), STR("inverse pounds"), STR("(1/lb)"), 1. / 0.45359237, &error);
    AddToLib(kSIQuantityInverseMass, STR("inverse ounce"), STR("inverse ounces"), STR("(1/oz)"), 1. / 0.028349523125, &error);
//
#pragma mark kSIQuantityMassRatio
    AddToLibPrefixed(kSIQuantityMassRatio, STR("gram per kilogram"), STR("grams per kilogram"), STR("g/kg"), 0.001, &error);
//
#pragma mark kSIQuantityTime
    AddToLib(kSIQuantityTime, STR("minute"), STR("minutes"), STR("min"), kSIMinute, &error);
    AddToLib(kSIQuantityTime, STR("hour"), STR("hours"), STR("h"), kSIHour, &error);
    AddToLib(kSIQuantityTime, STR("day"), STR("days"), STR("d"), kSIDay, &error);
    AddToLib(kSIQuantityTime, STR("week"), STR("weeks"), STR("wk"), kSIWeek, &error);
    AddToLib(kSIQuantityTime, STR("month"), STR("months"), STR("mo"), kSIMonth, &error);
    AddToLib(kSIQuantityTime, STR("year"), STR("years"), STR("yr"), kSIYear, &error);
    AddToLib(kSIQuantityTime, STR("decade"), STR("decades"), STR("dayr"), kSIDecade, &error);
    AddToLib(kSIQuantityTime, STR("century"), STR("centuries"), STR("hyr"), kSICentury, &error);
    AddToLib(kSIQuantityTime, STR("millennium"), STR("millennia"), STR("kyr"), kSIMillennium, &error);
    AddToLib(kSIQuantityTime, STR("atomic unit of time"), STR("atomic units of time"), STR("ℏ/E_h"), hbar / E_h, &error);
    AddToLib(kSIQuantityTime, STR("natural unit of time"), STR("natural units of time"), STR("ℏ/(m_e•c_0^2)"), naturalTimeUnit, &error);
    AddToLib(kSIQuantityTime, STR("planck time"), STR("planck time"), STR("t_P"), planckTime, &error);
//
#pragma mark kSIQuantityInverseTime
    AddToLib(kSIQuantityInverseTime, STR("inverse seconds"), STR("inverse seconds"), STR("(1/s)"), 1., &error);
    AddToLib(kSIQuantityInverseTime, STR("inverse minute"), STR("inverse minutes"), STR("(1/min)"), 1. / 60., &error);
    AddToLib(kSIQuantityInverseTime, STR("inverse hour"), STR("inverse hours"), STR("(1/h)"), 1. / (60. * 60.), &error);
    AddToLib(kSIQuantityInverseTime, STR("inverse day"), STR("inverse days"), STR("(1/d)"), 1. / (60. * 60 * 24.), &error);
    AddToLib(kSIQuantityInverseTime, STR("inverse week"), STR("inverse weeks"), STR("(1/wk)"), 1. / (60. * 60 * 24. * 7.), &error);
    AddToLib(kSIQuantityInverseTime, STR("inverse month"), STR("inverse months"), STR("(1/month)"), 1. / (365.25 * 86400 / 12.), &error);
    AddToLib(kSIQuantityInverseTime, STR("inverse year"), STR("inverse years"), STR("(1/yr)"), 1. / (365.25 * 86400), &error);
//
#pragma mark kSIQuantityFrequency
    AddToLibPrefixed(kSIQuantityFrequency, STR("hertz"), STR("hertz"), STR("Hz"), 1.0, &error);
//
#pragma mark kSIQuantityRadioactivity
    AddToLibPrefixed(kSIQuantityRadioactivity, STR("becquerel"), STR("becquerels"), STR("Bq"), 1.0, &error);
    AddToLibPrefixed(kSIQuantityRadioactivity, STR("curie"), STR("curies"), STR("Ci"), 3.7e10, &error);
//
#pragma mark kSIQuantityFrequencyRatio
    AddToLibPrefixed(kSIQuantityFrequencyRatio, STR("hertz per hertz"), STR("hertz per hertz"), STR("Hz/Hz"), 1., &error);
//
#pragma mark kSIQuantityTimeRatio
    AddToLibPrefixed(kSIQuantityTimeRatio, STR("second per second"), STR("seconds per second"), STR("s/s"), 1., &error);
//
#pragma mark kSIQuantityInverseTimeSquared
    AddToLib(kSIQuantityInverseTimeSquared, STR("inverse millisecond squared"), STR("inverse milliseconds squared"), STR("(1/ms^2)"), 1000000., &error);
    AddToLib(kSIQuantityInverseTimeSquared, STR("inverse second squared"), STR("inverse seconds squared"), STR("(1/s^2)"), 1., &error);
    AddToLib(kSIQuantityInverseTimeSquared, STR("inverse hour inverse second"), STR("inverse hour inverse seconds"), STR("(1/(h•s))"), 1. / 3600., &error);
    AddToLib(kSIQuantityInverseTimeSquared, STR("inverse hour inverse minute"), STR("inverse hour inverse minutes"), STR("(1/(h•min))"), 1. / 3600. / 60., &error);
//
#pragma mark kSIQuantityLinearMomentum
    AddToLib(kSIQuantityLinearMomentum, STR("natural unit of momentum"), STR("natural units of momentum"), STR("m_e•c_0"), naturalMomentumUnit, &error);
    AddToLibPrefixed(kSIQuantityLinearMomentum, STR("gram meter per second"), STR("gram meters per second"), STR("g•m/s"), 0.001, &error);
    AddToLibPrefixed(kSIQuantityLinearMomentum, STR("newton second"), STR("newton seconds"), STR("N•s"), 1, &error);
    AddToLib(kSIQuantityLinearMomentum, STR("atomic unit of momentum"), STR("atomic units of momentum"), STR("ℏ/a_0"), hbar / a_0, &error);
//
#pragma mark kSIQuantityEnergy
    AddToLib(kSIQuantityEnergy, STR("natural unit of energy"), STR("natural units of energy"), STR("m_e•c_0^2"), naturalEnergyUnit, &error);
    AddToLibPrefixed(kSIQuantityEnergy, STR("joule"), STR("joules"), STR("J"), 1., &error);
    AddToLibPrefixed(kSIQuantityEnergy, STR("watt hour"), STR("watt hour"), STR("W•h"), 3.6e3, &error);
    AddToLib(kSIQuantityEnergy, STR("rydberg"), STR("rydbergs"), STR("Ry"), Ry, &error);
    AddToLib(kSIQuantityEnergy, STR("alpha particle mass energy"), STR("alpha particle mass energy"), STR("m_a•c_0^2"), alphaParticleMassEnergy, &error);
    AddToLibPrefixed(kSIQuantityEnergy, STR("electronvolt"), STR("electronvolts"), STR("eV"), kSIElementaryCharge, &error);
    AddToLibPrefixed(kSIQuantityEnergy, STR("erg"), STR("ergs"), STR("erg"), 1e-7, &error);
    AddToLibPrefixed(kSIQuantityEnergy, STR("calorie"), STR("calories"), STR("cal"), 4.1868, &error);
    AddToLib(kSIQuantityEnergy, STR("atomic unit of energy"), STR("atomic unit of energy"), STR("E_h"), E_h, &error);
    AddToLib(kSIQuantityEnergy, STR("british thermal unit"), STR("british thermal units"), STR("Btu"), 1055.05585257348, &error);
//
#pragma mark kSIQuantityTemperature
    AddToLib(kSIQuantityTemperature, STR("planck temperature"), STR("planck temperature"), STR("T_P"), planckTemperature, &error);
    AddToLib(kSIQuantityTemperature, STR("rankine"), STR("rankines"), STR("°R"), 0.555555555555556, &error);
    AddToLib(kSIQuantityTemperature, STR("fahrenheit"), STR("fahrenheit"), STR("°F"), 0.555555555555556, &error);
    AddToLib(kSIQuantityTemperature, STR("celsius"), STR("celsius"), STR("°C"), 1, &error);
//
#pragma mark kSIQuantityCurrent
    AddToLib(kSIQuantityCurrent, STR("atomic unit of current"), STR("atomic unit of current"), STR("q_e•E_h/ℏ"), kSIElementaryCharge * E_h / hbar, &error);
//
#pragma mark kSIQuantityInverseCurrent
    AddToLib(kSIQuantityInverseCurrent, STR("inverse ampere"), STR("inverse amperes"), STR("(1/A)"), 1., &error);
//
#pragma mark kSIQuantityCurrentRatio
    AddToLibPrefixed(kSIQuantityCurrentRatio, STR("ampere per ampere"), STR("ampere per ampere"), STR("A/A"), 1., &error);
//
#pragma mark kSIQuantityInverseTemperature
    AddToLib(kSIQuantityInverseTemperature, STR("inverse kelvin"), STR("inverse kelvin"), STR("(1/K)"), 1., &error);
//
#pragma mark kSIQuantityTemperatureRatio
    AddToLibPrefixed(kSIQuantityTemperatureRatio, STR("kelvin per kelvin"), STR("kelvin per kelvin"), STR("K/K"), 1., &error);
//
#pragma mark kSIQuantityTemperatureGradient
    AddToLibPrefixed(kSIQuantityTemperatureGradient, STR("kelvin per meter"), STR("kelvin per meter"), STR("K/m"), 1., &error);
    AddToLib(kSIQuantityTemperatureGradient, STR("celsius per meter"), STR("celsius per meter"), STR("°C/m"), 1, &error);
    AddToLib(kSIQuantityTemperatureGradient, STR("fahrenheit per foot"), STR("fahrenheit per foot"), STR("°F/ft"), 0.555555555555556 / (1609.344 / 5280), &error);
    AddToLib(kSIQuantityTemperatureGradient, STR("rankine per foot"), STR("rankines per foot"), STR("°R/ft"), 0.555555555555556 / (1609.344 / 5280), &error);
//
#pragma mark kSIQuantityInverseAmount
    AddToLib(kSIQuantityInverseAmount, STR("inverse mole"), STR("inverse moles"), STR("(1/mol)"), 1., &error);
    AddToLib(kSIQuantityInverseAmount, STR("avogadro constant"), STR("avogadro constant"), STR("N_A"), kSIAvogadroConstant, &error);
//
#pragma mark kSIQuantityAmountRatio
    AddToLibPrefixed(kSIQuantityAmountRatio, STR("mole per mole"), STR("moles per mole"), STR("mol/mol"), 1., &error);
//
#pragma mark kSIQuantityInverseLuminousIntensity
    AddToLib(kSIQuantityInverseLuminousIntensity, STR("inverse candela"), STR("inverse candelas"), STR("(1/cd)"), 1., &error);
//
#pragma mark kSIQuantityLuminousIntensityRatio
    AddToLibPrefixed(kSIQuantityLuminousIntensityRatio, STR("candela per candela"), STR("candelas per candela"), STR("cd/cd"), 1., &error);
//
#pragma mark kSIQuantityArea
    AddToLib(kSIQuantityArea, STR("hectare"), STR("hectares"), STR("ha"), 1e4, &error);
    AddToLib(kSIQuantityArea, STR("barn"), STR("barns"), STR("b"), 1.e-28, &error);
    AddToLib(kSIQuantityArea, STR("square meter"), STR("square meters"), STR("m^2"), 1, &error);
    AddToLib(kSIQuantityArea, STR("square centimeter"), STR("square centimeters"), STR("cm^2"), 0.0001, &error);
    AddToLib(kSIQuantityArea, STR("square kilometer"), STR("square kilometers"), STR("km^2"), 1000000, &error);
    AddToLib(kSIQuantityArea, STR("square inch"), STR("square inches"), STR("in^2"), 0.00064516, &error);
    AddToLib(kSIQuantityArea, STR("square foot"), STR("square feet"), STR("ft^2"), 0.09290304, &error);
    AddToLib(kSIQuantityArea, STR("square yard"), STR("square yards"), STR("yd^2"), 0.83612736, &error);
    AddToLib(kSIQuantityArea, STR("square mile"), STR("square miles"), STR("mi^2"), 2589988.110336, &error);
    AddToLib(kSIQuantityArea, STR("square rod"), STR("square rods"), STR("rod^2"), 5.029210 * 5.029210, &error);
    AddToLib(kSIQuantityArea, STR("square chain"), STR("square chains"), STR("ch^2"), 5.029210 * 5.029210 * 16, &error);
    AddToLib(kSIQuantityArea, STR("acre"), STR("acres"), STR("ac"), 4046.8564224, &error);
    AddToLib(kSIQuantityArea, STR("township"), STR("townships"), STR("twp"), 2589988.110336 * 36., &error);
//
#pragma mark kSIQuantityInverseArea
    AddToLib(kSIQuantityInverseArea, STR("inverse hectare"), STR("inverse hectares"), STR("(1/ha)"), 1e-4, &error);
    AddToLib(kSIQuantityInverseArea, STR("inverse barn"), STR("inverse barns"), STR("(1/b)"), 1.e28, &error);
    AddToLib(kSIQuantityInverseArea, STR("inverse square kilometer"), STR("inverse square kilometer"), STR("(1/km^2)"), 1. / 1000000, &error);
    AddToLib(kSIQuantityInverseArea, STR("inverse square centimeter"), STR("inverse square centimeters"), STR("(1/cm^2)"), 1. / 0.0001, &error);
    AddToLib(kSIQuantityInverseArea, STR("inverse square inch"), STR("inverse square inches"), STR("(1/in^2)"), 1. / 0.00064516, &error);
    AddToLib(kSIQuantityInverseArea, STR("inverse square foot"), STR("inverse square feet"), STR("(1/ft^2)"), 1. / 0.09290304, &error);
    AddToLib(kSIQuantityInverseArea, STR("inverse square yard"), STR("inverse square yards"), STR("(1/yd^2)"), 1. / 0.83612736, &error);
    AddToLib(kSIQuantityInverseArea, STR("inverse acre"), STR("inverse acres"), STR("(1/ac)"), 1. / 4046.8564224, &error);
    AddToLib(kSIQuantityInverseArea, STR("inverse square mile"), STR("inverse square miles"), STR("(1/mi^2)"), 1. / 2589988.110336, &error);
//
#pragma mark kSIQuantityRockPermeability
    AddToLibPrefixed(kSIQuantityRockPermeability, STR("darcy"), STR("darcys"), STR("Dc"), 9.869233e-13, &error);
//
#pragma mark kSIQuantitySolidAngle
    AddToLib(kSIQuantitySolidAngle, STR("steradian"), STR("steradians"), STR("sr"), 1, &error);
//
#pragma mark kSIQuantityAreaRatio
    AddToLib(kSIQuantityAreaRatio, STR("square meter per square meter"), STR("square meters per square meter"), STR("m^2/m^2"), 1, &error);
//
#pragma mark kSIQuantityVolume
    AddToLibPrefixed(kSIQuantityVolume, STR("liter"), STR("liters"), STR("L"), 1e-3, &error);
    AddToLib(kSIQuantityVolume, STR("cubic inch"), STR("cubic inches"), STR("in^3"), (1609.344 / 63360) * (1609.344 / 63360) * (1609.344 / 63360), &error);
    AddToLib(kSIQuantityVolume, STR("cubic foot"), STR("cubic feet"), STR("ft^3"), (1609.344 / 5280) * (1609.344 / 5280) * (1609.344 / 5280), &error);
    AddToLib(kSIQuantityVolume, STR("cubic yard"), STR("cubic yards"), STR("yd^3"), (1609.344 / 1760) * (1609.344 / 1760) * (1609.344 / 1760), &error);
    AddToLib(kSIQuantityVolume, STR("acre foot"), STR("acre feet"), STR("ac•ft"), 1609.344 / 5280 * 4046.8564224, &error);
    AddToLib(kSIQuantityVolume, STR("oil barrel"), STR("oil barrels"), STR("bbl"), 0.158987295, &error);
    AddToLib(kSIQuantityVolume, STR("thousand oil barrels"), STR("thousand oil barrels"), STR("Mbbl"), 0.158987295e3, &error);
    AddToLib(kSIQuantityVolume, STR("million oil barrels"), STR("million oil barrels"), STR("MMbbl"), 0.158987295e6, &error);
//
#pragma mark kSIQuantityInverseVolume
    AddToLib(kSIQuantityInverseVolume, STR("inverse liter"), STR("inverse liters"), STR("(1/L)"), 1. / 1e-3, &error);
    AddToLib(kSIQuantityInverseVolume, STR("inverse milliliter"), STR("inverse milliliters"), STR("(1/mL)"), 1. / 1e-6, &error);
    AddToLib(kSIQuantityInverseVolume, STR("inverse cubic inch"), STR("inverse cubic inches"), STR("(1/in^3)"), 1. / ((1609.344 / 63360) * (1609.344 / 63360) * (1609.344 / 63360)), &error);
    AddToLib(kSIQuantityInverseVolume, STR("inverse cubic foot"), STR("inverse cubic feet"), STR("(1/ft^3)"), 1. / ((1609.344 / 5280) * (1609.344 / 5280) * (1609.344 / 5280)), &error);
    AddToLib(kSIQuantityInverseVolume, STR("inverse cubic yard"), STR("inverse cubic yards"), STR("(1/yd^3)"), 1. / ((1609.344 / 1760) * (1609.344 / 1760) * (1609.344 / 1760)), &error);
    AddToLib(kSIQuantityInverseVolume, STR("inverse cubic centimeter"), STR("inverse cubic centimeters"), STR("(1/cm^3)"), 1000000., &error);
    AddToLib(kSIQuantityInverseVolume, STR("inverse cubic millimeter"), STR("inverse cubic millimeters"), STR("(1/mm^3)"), 1000000000., &error);
    AddToLib(kSIQuantityInverseVolume, STR("inverse cubic micrometer"), STR("inverse cubic micrometers"), STR("(1/µm^3)"), 1e+18, &error);
    AddToLib(kSIQuantityInverseVolume, STR("inverse cubic ångström"), STR("inverse cubic ångströms"), STR("(1/Å^3)"), 1e+30, &error);
//
#pragma mark kSIQuantityVolumeRatio
    AddToLib(kSIQuantityVolumeRatio, STR("cubic meter per cubic meter"), STR("cubic meters per cubic meter"), STR("m^3/m^3"), 1, &error);
//
#pragma mark kSIQuantitySurfaceAreaToVolumeRatio
    AddToLib(kSIQuantitySurfaceAreaToVolumeRatio, STR("square meter per cubic meter"), STR("square meters per cubic meter"), STR("m^2/m^3"), 1, &error);
    AddToLib(kSIQuantitySurfaceAreaToVolumeRatio, STR("square meter per liter"), STR("square meters per liter"), STR("m^2/L"), 1000, &error);
//
#pragma mark kSIQuantitySpeed
    AddToLibPrefixed(kSIQuantitySpeed, STR("meter per second"), STR("meters per second"), STR("m/s"), 1, &error);
    AddToLibPrefixed(kSIQuantitySpeed, STR("meter per minute"), STR("meters per minute"), STR("m/min"), 1. / 60., &error);
    AddToLibPrefixed(kSIQuantitySpeed, STR("meter per hour"), STR("meters per hour"), STR("m/h"), 1. / 3600., &error);
    AddToLib(kSIQuantitySpeed, STR("knot"), STR("knots"), STR("kn"), 0.514444444444444, &error);
    AddToLib(kSIQuantitySpeed, STR("speed of light"), STR("speed of light"), STR("c_0"), kSISpeedOfLight, &error);
    AddToLib(kSIQuantitySpeed, STR("inch per second"), STR("inches per second"), STR("in/s"), 1609.344 / 63360, &error);
    AddToLib(kSIQuantitySpeed, STR("inch per minute"), STR("inches per minute"), STR("in/min"), 1609.344 / 63360 / 60., &error);
    AddToLib(kSIQuantitySpeed, STR("inch per hour"), STR("inches per hour"), STR("in/h"), 1609.344 / 63360 / 3600., &error);
    AddToLib(kSIQuantitySpeed, STR("foot per second"), STR("feet per second"), STR("ft/s"), 1609.344 / 5280, &error);
    AddToLib(kSIQuantitySpeed, STR("foot per minute"), STR("feet per minute"), STR("ft/min"), 1609.344 / 5280 / 60., &error);
    AddToLib(kSIQuantitySpeed, STR("foot per hour"), STR("feet per hour"), STR("ft/h"), 1609.344 / 5280 / 3600., &error);
    AddToLib(kSIQuantitySpeed, STR("mile per second"), STR("miles per second"), STR("mi/s"), 1609.344, &error);
    AddToLib(kSIQuantitySpeed, STR("mile per minute"), STR("miles per minute"), STR("mi/min"), 1609.344 / 60., &error);
    AddToLib(kSIQuantitySpeed, STR("mile per hour"), STR("miles per hour"), STR("mi/h"), 1609.344 / 3600., &error);
    AddToLib(kSIQuantitySpeed, STR("atomic unit of velocity"), STR("atomic units of velocity"), STR("a_0•E_h/ℏ"), a_0 * E_h / hbar, &error);
    units = (OCMutableArrayRef)OCDictionaryGetValue(units2QuantitiesLibrary, kSIQuantitySpeed);
    OCDictionaryAddValue(units2QuantitiesLibrary, kSIQuantityVelocity, units);
//
#pragma mark kSIQuantityAngularMomentum
    AddToLibPrefixed(kSIQuantityAngularMomentum, STR("joule second"), STR("joules second"), STR("J•s"), 1, &error);
    AddToLibPrefixed(kSIQuantityAngularMomentum, STR("gram square meter per second"), STR("gram square meters per second"), STR("g•m^2/s"), 0.001, &error);
//
#pragma mark kSIQuantityAction
    AddToLib(kSIQuantityAction, STR("planck constant"), STR("planck constant"), STR("h_P"), kSIPlanckConstant, &error);
//
#pragma mark kSIQuantityReducedAction
    AddToLib(kSIQuantityReducedAction, STR("reduced planck constant"), STR("reduced planck constant"), STR("ℏ"), hbar, &error);
//
#pragma mark kSIQuantityCirculation
    AddToLib(kSIQuantityCirculation, STR("quantum of circulation"), STR("quantum of circulation"), STR("h_P/(2•m_e)"), hbar, &error);
//
#pragma mark kSIQuantitySecondRadiationConstant
    AddToLib(kSIQuantitySecondRadiationConstant, STR("second radiation constant"), STR("second radiation constant"), STR("h_P•c_0/k_B"), secondRadiationConstant, &error);
//
#pragma mark kSIQuantityElectricResistance
    AddToLib(kSIQuantityElectricResistance, STR("von klitzing constant"), STR("von klitzing constant"), STR("h_P/(q_e^2)"), vonKlitzingConstant, &error);
    AddToLib(kSIQuantityElectricResistance, STR("ohm"), STR("ohms"), STR("Ω"), 1., &error);
    AddToLib(kSIQuantityElectricResistance, STR("inverse conductance quantum"), STR("inverse conductance quantum"), STR("(1/G_0)"), 1 / G_0, &error);
    AddToLib(kSIQuantityElectricResistance, STR("characteristic impedance of vacuum"), STR("characteristic impedance of vacuum"), STR("Z_0"), 4 * kSIPi * 1.e-7 * kSISpeedOfLight, &error);
//
#pragma mark kSIQuantityAcceleration
    AddToLibPrefixed(kSIQuantityAcceleration, STR("meter per square second"), STR("meters per square second"), STR("m/s^2"), 1, &error);
    AddToLibPrefixed(kSIQuantityAcceleration, STR("meter per hour per second"), STR("meters per hour per second"), STR("m/(h•s)"), 1. / 3600., &error);
    AddToLib(kSIQuantityAcceleration, STR("gravity acceleration"), STR("gravity acceleration"), STR("g_0"), kSIGravityAcceleration, &error);
    AddToLib(kSIQuantityAcceleration, STR("mile per square second"), STR("miles per square second"), STR("mi/s^2"), 1609.344, &error);
    AddToLib(kSIQuantityAcceleration, STR("foot per square second"), STR("feet per square second"), STR("ft/s^2"), 1609.344 / 5280, &error);
    AddToLib(kSIQuantityAcceleration, STR("inch per square second"), STR("inches per square second"), STR("in/s^2"), 1609.344 / 63360, &error);
    AddToLib(kSIQuantityAcceleration, STR("mile per square minute"), STR("miles per square minute"), STR("mi/min^2"), 1609.344 / 60. / 60., &error);
    AddToLib(kSIQuantityAcceleration, STR("foot per square minute"), STR("feet per square minute"), STR("ft/min^2"), 1609.344 / 5280 / 60. / 60., &error);
    AddToLib(kSIQuantityAcceleration, STR("inch per square minute"), STR("inches per square minute"), STR("in/min^2"), 1609.344 / 63360 / 60. / 60., &error);
    AddToLib(kSIQuantityAcceleration, STR("mile per hour per second"), STR("miles per hour per second"), STR("mi/(h•s)"), 1609.344 / 60. / 60., &error);
    AddToLib(kSIQuantityAcceleration, STR("knot per second"), STR("knots per second"), STR("kn/s"), 0.514444444444444, &error);
    AddToLib(kSIQuantityAcceleration, STR("galileo"), STR("galileo"), STR("Gal"), 1e-2, &error);
//
#pragma mark kSIQuantityDensity
    AddToLibPrefixed(kSIQuantityDensity, STR("gram per cubic meter"), STR("grams per cubic meter"), STR("g/m^3"), 1e-3, &error);
//
#pragma mark kSIQuantityMassFlowRate
    AddToLibPrefixed(kSIQuantityMassFlowRate, STR("gram per second"), STR("grams per second"), STR("g/s"), 1e-3, &error);
//
#pragma mark kSIQuantityMassFlux
    AddToLibPrefixed(kSIQuantityMassFlux, STR("gram per square meter per second"), STR("grams per square meter per second"), STR("g/(m^2•s)"), 1e-3, &error);
//
#pragma mark kSIQuantitySurfaceDensity
    AddToLibPrefixed(kSIQuantitySurfaceDensity, STR("gram per square meter"), STR("grams per square meter"), STR("g/m^2"), 1e-3, &error);
//
#pragma mark kSIQuantityCurrentDensity
    AddToLibPrefixed(kSIQuantityCurrentDensity, STR("ampere per square meter"), STR("amperes per square meter"), STR("A/m^2"), 1, &error);
//
#pragma mark kSIQuantityAmountConcentration
    AddToLibPrefixed(kSIQuantityAmountConcentration, STR("mole per cubic meter"), STR("moles per cubic meter"), STR("mol/m^3"), 1., &error);
    AddToLibPrefixed(kSIQuantityAmountConcentration, STR("mole per liter"), STR("moles per liter"), STR("mol/L"), 1000., &error);
    AddToLibPrefixed(kSIQuantityAmountConcentration, STR("mole per milliliter"), STR("moles per milliliter"), STR("mol/mL"), 1000000., &error);
    AddToLibPrefixed(kSIQuantityAmountConcentration, STR("mole per microliter"), STR("moles per microliter"), STR("mol/µL"), 1000000000., &error);
    AddToLibPrefixed(kSIQuantityAmountConcentration, STR("mole per liter"), STR("moles per liter"), STR("M"), 1000., &error);
//
#pragma mark kSIQuantityMassConcentration
    AddToLibPrefixed(kSIQuantityMassConcentration, STR("gram per liter"), STR("grams per liter"), STR("g/L"), 1, &error);
    AddToLibPrefixed(kSIQuantityMassConcentration, STR("gram per milliliter"), STR("grams per milliliter"), STR("g/mL"), 1e3, &error);
    AddToLibPrefixed(kSIQuantityMassConcentration, STR("gram per cubic centimeter"), STR("grams per cubic centimeter"), STR("g/cm^3"), 1e3, &error);
    AddToLibPrefixed(kSIQuantityMassConcentration, STR("gram per microliter"), STR("grams per microliter"), STR("g/µL"), 1e6, &error);
//
#pragma mark kSIQuantityForce
    AddToLibPrefixed(kSIQuantityForce, STR("newton"), STR("newtons"), STR("N"), 1, &error);
    AddToLib(kSIQuantityForce, STR("atomic unit of force"), STR("atomic units of force"), STR("E_h/a_0"), E_h / a_0, &error);
    AddToLibPrefixed(kSIQuantityForce, STR("dyne"), STR("dynes"), STR("dyn"), 1e-5, &error);
    AddToLib(kSIQuantityForce, STR("pound force"), STR("pounds force"), STR("lbf"), 4.4482216152605, &error);
    AddToLib(kSIQuantityForce, STR("ounce force"), STR("ounces force"), STR("ozf"), 4.4482216152605 / 16., &error);
    AddToLibPrefixed(kSIQuantityForce, STR("gram force"), STR("grams force"), STR("gf"), 0.00980665, &error);
//
#pragma mark kSIQuantityTorque
    AddToLibPrefixed(kSIQuantityTorque, STR("newton meter per radian"), STR("newton meters per radian"), STR("N•m/rad"), 1., &error);
    AddToLibPrefixed(kSIQuantityTorque, STR("joule per radian"), STR("joules per radian"), STR("J/rad"), 1., &error);
    AddToLib(kSIQuantityTorque, STR("pound force foot per radian"), STR("pound force feet per radian"), STR("lbf•ft/rad"), 1.3558179483314, &error);
    AddToLib(kSIQuantityTorque, STR("pound force inch per radian"), STR("pound force inches per radian"), STR("lbf•in/rad"), 1.3558179483314 / 12., &error);
    AddToLibPrefixed(kSIQuantityTorque, STR("gram force meter per radian"), STR("gram force meters per radian"), STR("gf•m/rad"), 0.00980665, &error);
    AddToLibPrefixed(kSIQuantityTorque, STR("gram force centimeter per radian"), STR("gram force centimeters per radian"), STR("gf•cm/rad"), 9.80665e-05, &error);
//
#pragma mark kSIQuantityMomentOfInertia
    AddToLib(kSIQuantityMomentOfInertia, STR("meter squared kilogram"), STR("meters squared kilogram"), STR("m^2•kg"), 1, &error);
    AddToLib(kSIQuantityMomentOfInertia, STR("meter squared gram"), STR("meters squared gram"), STR("m^2•g"), 1.e-3, &error);
    AddToLib(kSIQuantityMomentOfInertia, STR("centimeter squared kilogram"), STR("centimeter squared kilogram"), STR("cm^2•kg"), 0.0001, &error);
    AddToLib(kSIQuantityMomentOfInertia, STR("centimeter squared gram"), STR("centimeter squared gram"), STR("cm^2•g"), 1.e-7, &error);
//
#pragma mark kSIQuantityPressure, kSIQuantityStress, kSIQuantityElasticModulus
    AddToLibPrefixed(kSIQuantityPressure, STR("pascal"), STR("pascals"), STR("Pa"), 1, &error);
    AddToLib(kSIQuantityPressure, STR("pound force per square inch"), STR("pounds force per square inch"), STR("lbf/in^2"), 6894.75729, &error);
    AddToLib(kSIQuantityPressure, STR("pound force per square inch"), STR("pounds force per square inch"), STR("psi"), 6894.75729, &error);
    AddToLib(kSIQuantityPressure, STR("pound force per square foot"), STR("pounds force per square feet"), STR("lbf/ft^2"), 47.880259, &error);
    AddToLibPrefixed(kSIQuantityPressure, STR("torr"), STR("torrs"), STR("Torr"), 1.01325e5 / 760, &error);
    AddToLibPrefixed(kSIQuantityPressure, STR("bar"), STR("bars"), STR("bar"), 1e5, &error);
    AddToLib(kSIQuantityPressure, STR("millimeter of Hg"), STR("millimeters of Hg"), STR("mmHg"), 133.322, &error);
    AddToLib(kSIQuantityPressure, STR("atmosphere"), STR("atmospheres"), STR("atm"), 1.01325e5, &error);
    AddToLibPrefixed(kSIQuantityPressure, STR("newton per square meter"), STR("newtons per square meter"), STR("N/m^2"), 1, &error);
    units = (OCMutableArrayRef)OCDictionaryGetValue(units2QuantitiesLibrary, kSIQuantityPressure);
    OCDictionaryAddValue(units2QuantitiesLibrary, kSIQuantityStress, units);
    OCDictionaryAddValue(units2QuantitiesLibrary, kSIQuantityElasticModulus, units);
//
#pragma mark kSIQuantityCompressibility
    AddToLib(kSIQuantityCompressibility, STR("inverse pascal"), STR("inverse pascals"), STR("1/Pa"), 1, &error);
//
#pragma mark kSIQuantityStressOpticCoefficient
    AddToLib(kSIQuantityStressOpticCoefficient, STR("brewster"), STR("brewsters"), STR("B"), 1.e-12, &error);
//
#pragma mark kSIQuantityPressureGradient
    AddToLibPrefixed(kSIQuantityPressureGradient, STR("pascal per meter"), STR("pascals per meter"), STR("Pa/m"), 1, &error);
    AddToLib(kSIQuantityPressureGradient, STR("pound force per square inch per foot"), STR("pounds force per square inch per foot"), STR("psi/ft"), 6894.75729 / (1609.344 / 5280), &error);
//
#pragma mark kSIQuantitySpectralRadiantEnergy
    AddToLibPrefixed(kSIQuantitySpectralRadiantEnergy, STR("joule per nanometer"), STR("joules per nanometer"), STR("J/nm"), 1.e9, &error);
//
#pragma mark kSIQuantityPower, kSIQuantityRadiantFlux
    AddToLibPrefixed(kSIQuantityPower, STR("watt"), STR("watts"), STR("W"), 1, &error);
    AddToLibPrefixed(kSIQuantityPower, STR("Joule per second"), STR("Joules per second"), STR("J/s"), 1, &error);
    AddToLibPrefixed(kSIQuantityPower, STR("calorie per second"), STR("calories per second"), STR("cal/s"), 4.1868, &error);
    AddToLibPrefixed(kSIQuantityPower, STR("calorie per minute"), STR("calories per minute"), STR("cal/min"), 4.1868 / 60., &error);
    AddToLibPrefixed(kSIQuantityPower, STR("calorie per hour"), STR("calories per hour"), STR("cal/h"), 4.1868 / 3600., &error);
    AddToLib(kSIQuantityPower, STR("horsepower"), STR("horsepower"), STR("hp"), 745.699872, &error);
    AddToLib(kSIQuantityPower, STR("british thermal unit per hour"), STR("british thermal unit per hour"), STR("Btu/h"), 1055.05585257348 / 3600., &error);
    AddToLib(kSIQuantityPower, STR("british thermal unit per minute"), STR("british thermal unit per minute"), STR("Btu/min"), 1055.05585257348 / 60, &error);
    AddToLib(kSIQuantityPower, STR("british thermal unit per second"), STR("british thermal unit per second"), STR("Btu/s"), 1055.05585257348, &error);
    AddToLibPrefixed(kSIQuantityPower, STR("erg per second"), STR("ergs per second"), STR("erg/s"), 1e-7, &error);
    AddToLib(kSIQuantityPower, STR("foot pound force per hour"), STR("feet pound force per hour"), STR("ft•lbf/h"), (1609.344 / 5280) * 4.4482216152605 / 3600., &error);
    AddToLib(kSIQuantityPower, STR("foot pound force per minute"), STR("feet pound force per minute"), STR("ft•lbf/min"), (1609.344 / 5280) * 4.4482216152605 / 60., &error);
    AddToLib(kSIQuantityPower, STR("foot pound force per second"), STR("feet pound force per second"), STR("ft•lbf/s"), (1609.344 / 5280) * 4.4482216152605, &error);
    AddToLib(kSIQuantityPower, STR("inch pound force per hour"), STR("inches pound force per hour"), STR("in•lbf/h"), 1.3558179483314 / 12. / 3600., &error);
    AddToLib(kSIQuantityPower, STR("inch pound force per minute"), STR("inches pound force per minute"), STR("in•lbf/min"), 1.3558179483314 / 12. / 60., &error);
    AddToLib(kSIQuantityPower, STR("inch pound force per second"), STR("inches pound force per second"), STR("in•lbf/s"), 1.3558179483314 / 12., &error);
    units = (OCMutableArrayRef)OCDictionaryGetValue(units2QuantitiesLibrary, kSIQuantityPower);
    OCDictionaryAddValue(units2QuantitiesLibrary, kSIQuantityRadiantFlux, units);
//
#pragma mark kSIQuantitySpectralPower
    AddToLibPrefixed(kSIQuantitySpectralPower, STR("watt per nanometer"), STR("watts per nanometer"), STR("W/nm"), 1.e9, &error);
//
#pragma mark kSIQuantityVolumePowerDensity
    AddToLibPrefixed(kSIQuantityVolumePowerDensity, STR("watt per cubic meter"), STR("watts per cubic meter"), STR("W/m^3"), 1, &error);
    AddToLibPrefixed(kSIQuantityVolumePowerDensity, STR("watt per cubic centimeter"), STR("watts per cubic centimeter"), STR("W/cm^3"), 100000., &error);
//
#pragma mark kSIQuantitySpecificPower
    AddToLibPrefixed(kSIQuantitySpecificPower, STR("watt per kilogram"), STR("watts per kilogram"), STR("W/kg"), 1, &error);
    AddToLib(kSIQuantitySpecificPower, STR("horse power per pound"), STR("horse power per pound"), STR("hp/lb"), 1643.986806920936, &error);
    AddToLib(kSIQuantitySpecificPower, STR("horse power per ounce"), STR("horse power per ounce"), STR("hp/oz"), 26303.78891073498, &error);
//
#pragma mark kSIQuantityElectricCharge, kSIQuantityAmountOfElectricity
    AddToLibPrefixed(kSIQuantityElectricCharge, STR("coulomb"), STR("coulombs"), STR("C"), 1, &error);
    AddToLib(kSIQuantityElectricCharge, STR("elementary charge"), STR("elementary charge"), STR("q_e"), kSIElementaryCharge, &error);
    units = (OCMutableArrayRef)OCDictionaryGetValue(units2QuantitiesLibrary, kSIQuantityElectricCharge);
    OCDictionaryAddValue(units2QuantitiesLibrary, kSIQuantityAmountOfElectricity, units);
//
#pragma mark kSIQuantityElectricPotentialDifference, kSIQuantityElectromotiveForce, kSIQuantityVoltage
    AddToLibPrefixed(kSIQuantityElectricPotentialDifference, STR("volt"), STR("volts"), STR("V"), 1, &error);
    AddToLib(kSIQuantityElectricPotentialDifference, STR("atomic unit of electric potential"), STR("atomic units of electric potential"), STR("E_h/q_e"), E_h / kSIElementaryCharge, &error);
    units = (OCMutableArrayRef)OCDictionaryGetValue(units2QuantitiesLibrary, kSIQuantityElectricPotentialDifference);
    OCDictionaryAddValue(units2QuantitiesLibrary, kSIQuantityElectromotiveForce, units);
    OCDictionaryAddValue(units2QuantitiesLibrary, kSIQuantityVoltage, units);
//
#pragma mark kSIQuantityElectricFieldGradient
    AddToLibPrefixed(kSIQuantityElectricFieldGradient, STR("volt per square meter"), STR("volts per square meter"), STR("V/m^2"), 1, &error);
    AddToLib(kSIQuantityElectricFieldGradient, STR("atomic unit of electric field gradient"), STR("atomic unit of electric field gradient"), STR("Λ_0"), Λ_0, &error);
    AddToLib(kSIQuantityElectricFieldGradient, STR("atomic unit of electric field gradient"), STR("atomic unit of electric field gradient"), STR("E_h/(q_e•a_0^2)"), Λ_0, &error);
//
#pragma mark kSIQuantityCapacitance
    AddToLibPrefixed(kSIQuantityCapacitance, STR("farad"), STR("farads"), STR("F"), 1, &error);
//
#pragma mark kSIQuantityElectricResistancePerLength
    AddToLibPrefixed(kSIQuantityElectricResistancePerLength, STR("ohm per meter"), STR("ohms per meter"), STR("Ω/m"), 1, &error);
    AddToLibPrefixed(kSIQuantityElectricResistancePerLength, STR("ohm per feet"), STR("ohms per feet"), STR("Ω/ft"), 1. / (1609.344 / 5280), &error);
//
#pragma mark kSIQuantityElectricResistivity
    AddToLibPrefixed(kSIQuantityElectricResistivity, STR("ohm meter"), STR("ohms meter"), STR("Ω•m"), 1, &error);
    AddToLibPrefixed(kSIQuantityElectricResistivity, STR("ohm centimeter"), STR("ohms centimeter"), STR("Ω•cm"), 0.01, &error);
//
#pragma mark kSIQuantityElectricConductance
    AddToLibPrefixed(kSIQuantityElectricConductance, STR("siemen"), STR("siemens"), STR("S"), 1, &error);
    AddToLib(kSIQuantityElectricConductance, STR("conductance quantum"), STR("conductance quantum"), STR("G_0"), G_0, &error);
//
#pragma mark kSIQuantityElectricConductivity
    AddToLibPrefixed(kSIQuantityElectricConductivity, STR("siemen per meter"), STR("siemens per meter"), STR("S/m"), 1, &error);
    AddToLibPrefixed(kSIQuantityElectricConductivity, STR("siemen per centimeter"), STR("siemens per centimeter"), STR("S/cm"), 100., &error);
//
#pragma mark kSIQuantityMolarConductivity
    AddToLibPrefixed(kSIQuantityMolarConductivity, STR("siemen meter squared per mole"), STR("siemens meter squared per mole"), STR("S•m^2/mol"), 1, &error);
    AddToLibPrefixed(kSIQuantityMolarConductivity, STR("siemen centimeter squared per mole"), STR("siemens centimeter squared per mole"), STR("S•cm^2/mol"), 0.0001, &error);
//
#pragma mark kSIQuantityGyromagneticRatio
    AddToLib(kSIQuantityGyromagneticRatio, STR("radian per second per tesla"), STR("radians per second per tesla"), STR("rad/(s•T)"), 1, &error);
//
#pragma mark kSIQuantityMagneticDipoleMoment
    AddToLibPrefixed(kSIQuantityMagneticDipoleMoment, STR("ampere square meter"), STR("ampere square meters"), STR("A•m^2"), 1, &error);
    AddToLibPrefixed(kSIQuantityMagneticDipoleMoment, STR("joule per tesla"), STR("joules per tesla"), STR("J/T"), 1, &error);
    AddToLib(kSIQuantityMagneticDipoleMoment, STR("nuclear magneton"), STR("nuclear magnetons"), STR("µ_N"), mu_N, &error);
    AddToLib(kSIQuantityMagneticDipoleMoment, STR("bohr magneton"), STR("bohr magnetons"), STR("µ_B"), mu_e, &error);
    AddToLib(kSIQuantityMagneticDipoleMoment, STR("proton magnetic moment"), STR("proton magnetic moment"), STR("µ_p"), kSIProtonMagneticMoment, &error);
    AddToLib(kSIQuantityMagneticDipoleMoment, STR("neutron magnetic moment"), STR("neutron magnetic moment"), STR("µ_n"), kSINeutronMagneticMoment, &error);
    AddToLib(kSIQuantityMagneticDipoleMoment, STR("electron magnetic moment"), STR("electron magnetic moment"), STR("µ_e"), kSIElectronMagneticMoment, &error);
    AddToLib(kSIQuantityMagneticDipoleMoment, STR("muon magnetic moment"), STR("muon magnetic moment"), STR("µ_µ"), kSIMuonMagneticMoment, &error);
    AddToLib(kSIQuantityMagneticDipoleMoment, STR("atomic unit of magnetic dipole moment"), STR("atomic units of magnetic dipole moment"), STR("ℏ•q_e/m_e"), hbar * kSIElementaryCharge / kSIElectronMass, &error);
//
#pragma mark kSIQuantityMagneticDipoleMomentRatio
    AddToLib(kSIQuantityMagneticDipoleMomentRatio, STR("proton g factor"), STR("proton g factor"), STR("g_p"), kSIProtonGFactor, &error);
    AddToLib(kSIQuantityMagneticDipoleMomentRatio, STR("neutron g factor"), STR("neutron g factor"), STR("g_n"), kSINeutronGFactor, &error);
    AddToLib(kSIQuantityMagneticDipoleMomentRatio, STR("electron g factor"), STR("electron g factor"), STR("g_e"), kSIElectronGFactor, &error);
    AddToLib(kSIQuantityMagneticDipoleMomentRatio, STR("muon g factor"), STR("muon g factor"), STR("g_µ"), kSIMuonGFactor, &error);
//
#pragma mark kSIQuantityMagneticFlux
    AddToLibPrefixed(kSIQuantityMagneticFlux, STR("weber"), STR("webers"), STR("Wb"), 1, &error);
    AddToLib(kSIQuantityMagneticFlux, STR("magnetic flux quantum"), STR("magnetic flux quantum"), STR("Φ_0"), kSIPlanckConstant / (2 * kSIElementaryCharge), &error);
    AddToLib(kSIQuantityMagneticFlux, STR("maxwell"), STR("maxwells"), STR("Mx"), 1e-8, &error);
//
#pragma mark kSIQuantityMagneticFluxDensity
    AddToLibPrefixed(kSIQuantityMagneticFluxDensity, STR("tesla"), STR("tesla"), STR("T"), 1, &error);
    AddToLib(kSIQuantityMagneticFluxDensity, STR("atomic unit of magnetic flux density"), STR("atomic units of magnetic flux density"), STR("ℏ/(q_e•a_0^2)"), hbar / (kSIElementaryCharge * a_0 * a_0), &error);
    AddToLibPrefixed(kSIQuantityMagneticFluxDensity, STR("gauss"), STR("gauss"), STR("G"), 1e-4, &error);
//
#pragma mark kSIQuantityMagneticFieldGradient
    AddToLibPrefixed(kSIQuantityMagneticFieldGradient, STR("tesla per meter"), STR("tesla per meter"), STR("T/m"), 1, &error);
    AddToLibPrefixed(kSIQuantityMagneticFieldGradient, STR("tesla per centimeter"), STR("tesla per centimeter"), STR("T/cm"), 100., &error);
    AddToLibPrefixed(kSIQuantityMagneticFieldGradient, STR("gauss per centimeter"), STR("gauss per centimeter"), STR("G/cm"), 0.01, &error);
//
#pragma mark kSIQuantityMolarMagneticSusceptibility
    AddToLib(kSIQuantityMolarMagneticSusceptibility, STR("cubic meter per mole"), STR("cubic meters per mole"), STR("m^3/mol"), 1., &error);
    AddToLib(kSIQuantityMolarMagneticSusceptibility, STR("cubic centimeter per mole"), STR("cubic centimeters per mole"), STR("cm^3/mol"), 1e-06, &error);
//
#pragma mark kSIQuantityInductance
    AddToLibPrefixed(kSIQuantityInductance, STR("henry"), STR("henries"), STR("H"), 1, &error);
//
#pragma mark kSIQuantityLuminousFlux
    AddToLibPrefixed(kSIQuantityLuminousFlux, STR("lumen"), STR("lumens"), STR("lm"), 1., &error);
    AddToLibPrefixed(kSIQuantityLuminousFlux, STR("candela steradian"), STR("candela steradians"), STR("cd•sr"), 1, &error);
//
#pragma mark kSIQuantityLuminousFluxDensity
    AddToLibPrefixed(kSIQuantityLuminousFluxDensity, STR("lumen per square meter"), STR("lumens per square meter"), STR("lm/m^2"), 1., &error);
    AddToLibPrefixed(kSIQuantityLuminousFluxDensity, STR("lumen per square foot"), STR("lumens per square foot"), STR("lm/ft^2"), 10.76391041670972, &error);
//
#pragma mark kSIQuantityLuminousEnergy
    AddToLibPrefixed(kSIQuantityLuminousEnergy, STR("lumen second"), STR("lumen seconds"), STR("lm•s"), 1, &error);
//
#pragma mark kSIQuantityIlluminance
    AddToLibPrefixed(kSIQuantityIlluminance, STR("lux"), STR("lux"), STR("lx"), 1, &error);
    AddToLibPrefixed(kSIQuantityIlluminance, STR("phot"), STR("phots"), STR("ph"), 1e4, &error);
//
#pragma mark kSIQuantityAbsorbedDose
    AddToLibPrefixed(kSIQuantityAbsorbedDose, STR("gray"), STR("grays"), STR("Gy"), 1, &error);
//
#pragma mark kSIQuantityDoseEquivalent
    AddToLibPrefixed(kSIQuantityDoseEquivalent, STR("sievert"), STR("sieverts"), STR("Sv"), 1, &error);
//
#pragma mark kSIQuantityCatalyticActivity
    AddToLibPrefixed(kSIQuantityCatalyticActivity, STR("mole per second"), STR("moles per second"), STR("mol/s"), 1, &error);
    AddToLibPrefixed(kSIQuantityCatalyticActivity, STR("katal"), STR("katals"), STR("kat"), 1, &error);
    AddToLibPrefixed(kSIQuantityCatalyticActivity, STR("mole per minute"), STR("moles per minute"), STR("mol/min"), 1. / 60., &error);
//
#pragma mark kSIQuantityCatalyticActivityConcentration
    AddToLibPrefixed(kSIQuantityCatalyticActivityConcentration, STR("katal per cubic meter"), STR("katals per cubic meter"), STR("kat/m^3"), 1, &error);
    AddToLibPrefixed(kSIQuantityCatalyticActivityConcentration, STR("katal per liter"), STR("katals per liter"), STR("kat/L"), 1000., &error);
//
#pragma mark kSIQuantityCatalyticActivityContent
    AddToLibPrefixed(kSIQuantityCatalyticActivityContent, STR("katal per kilogram"), STR("katals per kilogram"), STR("kat/kg"), 1, &error);
//
#pragma mark kSIQuantityRatePerAmountConcentrationPerTime
    AddToLibPrefixed(kSIQuantityRatePerAmountConcentrationPerTime, STR("liter per mole per second"), STR("liter per mole per second"), STR("L/(mol•s)"), 0.001, &error);
//
#pragma mark kSIQuantityRefractiveIndex
    AddToLibPrefixed(kSIQuantityRefractiveIndex, STR("meter second per meter second"), STR("meter seconds per meter second"), STR("m•s/(m•s)"), 1, &error);
//
#pragma mark kSIQuantityElectricQuadrupoleMoment
    AddToLib(kSIQuantityElectricQuadrupoleMoment, STR("atomic unit of electric quadrupole moment"), STR("atomic units of electric quadrupole moment"), STR("q_e•a_0^2"), kSIElementaryCharge * a_0 * a_0, &error);
//
#pragma mark kSIQuantityMagnetizability
    AddToLib(kSIQuantityMagnetizability, STR("atomic unit of magnetizability"), STR("atomic units of magnetizability"), STR("q_e•a_0^2/m_e"), kSIElementaryCharge * a_0 * a_0 / kSIElectronMass, &error);
//
#pragma mark kSIQuantityPermittivity
    AddToLib(kSIQuantityPermittivity, STR("atomic unit of permittivity"), STR("atomic units of permittivity"), STR("q_e^2/(a_0•E_h)"), kSIElementaryCharge * kSIElementaryCharge / (a_0 * E_h), &error);
    AddToLibPrefixed(kSIQuantityPermittivity, STR("farad per meter"), STR("farads per meter"), STR("F/m"), 1, &error);
    AddToLibPrefixed(kSIQuantityPermittivity, STR("coulomb per volt meter"), STR("coulombs per volt meter"), STR("C/(V•m)"), 1, &error);
    AddToLib(kSIQuantityPermittivity, STR("electric constant"), STR("electric constant"), STR("ε_0"), kSIElectricConstant, &error);
//
#pragma mark kSIQuantityElectricPolarizability
    AddToLib(kSIQuantityElectricPolarizability, STR("atomic unit of electric polarizability"), STR("atomic units of electric polarizability"), STR("q_e^2•a_0^2/E_h"), kSIElementaryCharge * kSIElementaryCharge * a_0 * a_0 / (E_h), &error);
//
#pragma mark kSIQuantityFirstHyperPolarizability
    AddToLib(kSIQuantityFirstHyperPolarizability, STR("atomic unit of 1st polarizability"), STR("atomic units of 1st polarizability"), STR("q_e^3•a_0^3/E_h^2"), kSIElementaryCharge * kSIElementaryCharge * kSIElementaryCharge * a_0 * a_0 * a_0 / (E_h * E_h), &error);
//
#pragma mark kSIQuantitySecondHyperPolarizability
    AddToLib(kSIQuantitySecondHyperPolarizability, STR("atomic unit of 2nd polarizability"), STR("atomic units of 2nd polarizability"), STR("q_e^4•a_0^4/E_h^3"), kSIElementaryCharge * kSIElementaryCharge * kSIElementaryCharge * kSIElementaryCharge * a_0 * a_0 * a_0 * a_0 / (E_h * E_h * E_h), &error);
//
#pragma mark kSIQuantitySpecificVolume
    AddToLib(kSIQuantitySpecificVolume, STR("cubic meter per kilogram"), STR("cubic meters per kilogram"), STR("m^3/kg"), 1., &error);
//
#pragma mark kSIQuantityMassToChargeRatio
    AddToLib(kSIQuantityMassToChargeRatio, STR("thomson"), STR("thomson"), STR("Th"), kSIAtomicMassConstant / kSIElementaryCharge, &error);
//
#pragma mark kSIQuantityChargeToMassRatio
    AddToLib(kSIQuantityChargeToMassRatio, STR("inverse thomson"), STR("inverse thomson"), STR("(1/Th)"), kSIElementaryCharge / kSIAtomicMassConstant, &error);
//
#pragma mark kSIQuantityDynamicViscosity
    AddToLibPrefixed(kSIQuantityDynamicViscosity, STR("poise"), STR("poises"), STR("P"), 0.1, &error);
    AddToLib(kSIQuantityDynamicViscosity, STR("pascal second"), STR("pascal seconds"), STR("Pa•s"), 1, &error);
    AddToLib(kSIQuantityDynamicViscosity, STR("newton second per square meter"), STR("newton seconds per square meter"), STR("N•s/m^2"), 1, &error);
//
#pragma mark kSIQuantityKinematicViscosity
    AddToLibPrefixed(kSIQuantityKinematicViscosity, STR("stokes"), STR("stokes"), STR("St"), 1e-4, &error);
//
#pragma mark kSIQuantityDiffusionCoefficient
    AddToLib(kSIQuantityDiffusionCoefficient, STR("square meter per second"), STR("square meters per second"), STR("m^2/s"), 1, &error);
    AddToLib(kSIQuantityDiffusionCoefficient, STR("square centimeter per second"), STR("square centimeters per second"), STR("cm^2/s"), 0.0001, &error);
    AddToLib(kSIQuantityDiffusionCoefficient, STR("square millimeter per second"), STR("square millimeters per second"), STR("mm^2/s"), 1e-6, &error);
//
#pragma mark kSIQuantityLuminance
    AddToLibPrefixed(kSIQuantityLuminance, STR("stilb"), STR("stilbs"), STR("sb"), 1e4, &error);
    AddToLibPrefixed(kSIQuantityLuminance, STR("lumen per square meter per steradian"), STR("lumens per square meter per steradian"), STR("lm/(m^2•sr)"), 1, &error);
//
#pragma mark kSIQuantityInverseMagneticFluxDensity
    AddToLib(kSIQuantityInverseMagneticFluxDensity, STR("inverse gauss"), STR("inverse gauss"), STR("(1/G)"), 1., &error);
//
#pragma mark kSIQuantityMagneticFieldStrength
    AddToLibPrefixed(kSIQuantityMagneticFieldStrength, STR("ampere per meter"), STR("amperes per meter"), STR("A/m"), 1, &error);
    AddToLibPrefixed(kSIQuantityMagneticFieldStrength, STR("œrsted"), STR("œrsteds"), STR("Oe"), 79.577471545947674, &error);
//
#pragma mark kSIQuantityMomentOfForce
    AddToLibPrefixed(kSIQuantityMomentOfForce, STR("newton meter"), STR("newton meters"), STR("N•m"), 1, &error);
    AddToLib(kSIQuantityMomentOfForce, STR("foot pound force"), STR("feet pound force"), STR("ft•lbf"), 1.3558179483314, &error);
    AddToLib(kSIQuantityMomentOfForce, STR("inch pound force"), STR("inch pound force"), STR("in•lbf"), 1.3558179483314 / 12., &error);
    AddToLib(kSIQuantityMomentOfForce, STR("inch ounce force"), STR("inch ounce force"), STR("in•ozf"), 1.3558179483314 / 12. / 16., &error);
    AddToLib(kSIQuantityMomentOfForce, STR("pound force foot"), STR("pound force feet"), STR("lbf•ft"), 1.3558179483314, &error);
    AddToLib(kSIQuantityMomentOfForce, STR("pound force inch"), STR("pound force inches"), STR("lbf•in"), 1.3558179483314 / 12., &error);
    AddToLib(kSIQuantityMomentOfForce, STR("ounce force inch"), STR("ounce force inches"), STR("ozf•in"), 1.3558179483314 / 12. / 16., &error);
//
#pragma mark kSIQuantitySurfaceTension
    AddToLibPrefixed(kSIQuantitySurfaceTension, STR("newton per meter"), STR("newtons per meter"), STR("N/m"), 1, &error);
    AddToLibPrefixed(kSIQuantitySurfaceTension, STR("dyne per centimeter"), STR("dynes per centimeter"), STR("dyn/cm"), 0.001, &error);
//
#pragma mark kSIQuantitySurfaceEnergy
    AddToLibPrefixed(kSIQuantitySurfaceEnergy, STR("joule per square meter"), STR("joules per square meter"), STR("J/m^2"), 1, &error);
    AddToLibPrefixed(kSIQuantitySurfaceEnergy, STR("dyne per square centimeter"), STR("dynes per square centimeter"), STR("dyn/cm^2"), 0.1, &error);
//
#pragma mark kSIQuantityAngularSpeed, kSIQuantityAngularVelocity
    AddToLib(kSIQuantityAngularSpeed, STR("radian per second"), STR("radians per second"), STR("rad/s"), 1, &error);
    units = (OCMutableArrayRef)OCDictionaryGetValue(units2QuantitiesLibrary, kSIQuantityAngularSpeed);
    OCDictionaryAddValue(units2QuantitiesLibrary, kSIQuantityAngularVelocity, units);
//
#pragma mark kSIQuantityAngularAcceleration
    AddToLib(kSIQuantityAngularAcceleration, STR("radian per square second"), STR("radians per square second"), STR("rad/s^2"), 1, &error);
//
#pragma mark kSIQuantityHeatFluxDensity, kSIQuantityIrradiance
    AddToLibPrefixed(kSIQuantityHeatFluxDensity, STR("watt per square meter"), STR("watts per square meter"), STR("W/m^2"), 1, &error);
    AddToLibPrefixed(kSIQuantityHeatFluxDensity, STR("watt per square centimeter"), STR("watts per square centimeter"), STR("W/cm^2"), 10000., &error);
    AddToLibPrefixed(kSIQuantityHeatFluxDensity, STR("watt per square foot"), STR("watts per square foot"), STR("W/ft^2"), 10.76391041670972, &error);
    AddToLibPrefixed(kSIQuantityHeatFluxDensity, STR("watt per square inch"), STR("watts per square inch"), STR("W/in^2"), 10.76391041670972 / 12., &error);
    units = (OCMutableArrayRef)OCDictionaryGetValue(units2QuantitiesLibrary, kSIQuantityHeatFluxDensity);
    OCDictionaryAddValue(units2QuantitiesLibrary, kSIQuantityIrradiance, units);
//
#pragma mark kSIQuantitySpectralRadiantFluxDensity
    AddToLibPrefixed(kSIQuantitySpectralRadiantFluxDensity, STR("watt per square meter per nanometer"), STR("watts per square meter per nanometer"), STR("W/(m^2•nm)"), 1.e9, &error);
//
#pragma mark kSIQuantityEntropy, kSIQuantityHeatCapacity
    AddToLibPrefixed(kSIQuantityEntropy, STR("joule per kelvin"), STR("joules per kelvin"), STR("J/K"), 1, &error);
    AddToLib(kSIQuantityEntropy, STR("boltzmann constant"), STR("boltzmann constant"), STR("k_B"), kSIBoltmannConstant, &error);
    units = (OCMutableArrayRef)OCDictionaryGetValue(units2QuantitiesLibrary, kSIQuantityEntropy);
    OCDictionaryAddValue(units2QuantitiesLibrary, kSIQuantityHeatCapacity, units);
//
#pragma mark kSIQuantitySpecificHeatCapacity, kSIQuantitySpecificEntropy
    AddToLibPrefixed(kSIQuantitySpecificHeatCapacity, STR("joule per kilogram kelvin"), STR("joules per kilogram kelvin"), STR("J/(kg•K)"), 1, &error);
    AddToLibPrefixed(kSIQuantitySpecificHeatCapacity, STR("joule per gram kelvin"), STR("joules per gram kelvin"), STR("J/(g•K)"), 1000., &error);
    AddToLibPrefixed(kSIQuantitySpecificHeatCapacity, STR("calorie per gram per kelvin"), STR("calories per gram per kelvin"), STR("cal/(g•K)"), 4186.8, &error);
    units = (OCMutableArrayRef)OCDictionaryGetValue(units2QuantitiesLibrary, kSIQuantitySpecificHeatCapacity);
    OCDictionaryAddValue(units2QuantitiesLibrary, kSIQuantitySpecificEntropy, units);
//
#pragma mark kSIQuantityMolarMass
    AddToLibPrefixed(kSIQuantityMolarMass, STR("gram per mole"), STR("grams per mole"), STR("g/mol"), 1e-3, &error);
//
#pragma mark kSIQuantityMolality
    AddToLibPrefixed(kSIQuantityMolality, STR("mole per kilogram"), STR("moles per kilogram"), STR("mol/kg"), 1, &error);
//
#pragma mark kSIQuantitySpecificEnergy
    AddToLibPrefixed(kSIQuantitySpecificEnergy, STR("joule per kilogram"), STR("joules per kilogram"), STR("J/kg"), 1, &error);
    AddToLibPrefixed(kSIQuantitySpecificEnergy, STR("joule per gram"), STR("joules per gram"), STR("J/g"), 1e3, &error);
//
#pragma mark kSIQuantityThermalConductance
    AddToLibPrefixed(kSIQuantityThermalConductance, STR("watt per kelvin"), STR("watts per kelvin"), STR("W/K"), 1, &error);
    AddToLib(kSIQuantityThermalConductance, STR("Btu per hour per rankine"), STR("Btus per hour per rankine"), STR("Btu/(h•°R)"), 0.5275279262867396, &error);
    AddToLibPrefixed(kSIQuantityThermalConductance, STR("calorie per hour per kelvin"), STR("calories per hour per kelvin"), STR("cal/(h•K)"), 1.163e-3, &error);
//
#pragma mark kSIQuantityThermalConductivity
    AddToLibPrefixed(kSIQuantityThermalConductivity, STR("watt per meter kelvin"), STR("watts per meter kelvin"), STR("W/(m•K)"), 1, &error);
    AddToLib(kSIQuantityThermalConductivity, STR("Btu per hour per foot per rankine"), STR("Btus per hour per foot per rankine"), STR("Btu/(h•ft•°R)"), 1.730734666295077, &error);
    AddToLibPrefixed(kSIQuantityThermalConductivity, STR("calorie per hour per meter per kelvin"), STR("calories per hour per meter per kelvin"), STR("cal/(h•m•K)"), 1.163e-3, &error);
//
#pragma mark kSIQuantityEnergyDensity
    AddToLibPrefixed(kSIQuantityEnergyDensity, STR("joule per cubic meter"), STR("joules per cubic meter"), STR("J/m^3"), 1, &error);
    AddToLibPrefixed(kSIQuantityEnergyDensity, STR("joule per liter"), STR("joules per liter"), STR("J/L"), 1000., &error);
//
#pragma mark kSIQuantityElectricDipoleMoment
    AddToLibPrefixed(kSIQuantityElectricDipoleMoment, STR("coulomb meter"), STR("coulomb meters"), STR("C•m"), 1, &error);
    AddToLib(kSIQuantityElectricDipoleMoment, STR("debye"), STR("debyes"), STR("D"), 3.335640951816991e-30, &error);
    AddToLib(kSIQuantityElectricDipoleMoment, STR("atomic unit of electric dipole moment"), STR("atomic unit of electric dipole moment"), STR("q_e•a_0"), kSIElementaryCharge * a_0, &error);
//
#pragma mark kSIQuantityElectricFieldStrength
    AddToLibPrefixed(kSIQuantityElectricFieldStrength, STR("volt per meter"), STR("volts per meter"), STR("V/m"), 1, &error);
    AddToLib(kSIQuantityElectricFieldStrength, STR("atomic unit of electric field"), STR("atomic unit of electric field"), STR("E_h/(q_e•a_0)"), E_h / (kSIElementaryCharge * a_0), &error);
    AddToLibPrefixed(kSIQuantityElectricFieldStrength, STR("volt per centimeter"), STR("volts per centimeter"), STR("V/cm"), 1, &error);
    AddToLibPrefixed(kSIQuantityElectricFieldStrength, STR("newton per coulomb"), STR("newtons per coulomb"), STR("N/C"), 1, &error);
//
#pragma mark kSIQuantityElectricFlux
    AddToLibPrefixed(kSIQuantityElectricFlux, STR("volt meter"), STR("volts meter"), STR("V•m"), 1, &error);
//
#pragma mark kSIQuantityElectricChargeDensity
    AddToLibPrefixed(kSIQuantityElectricChargeDensity, STR("coulomb per cubic meter"), STR("coulombs per cubic meter"), STR("C/m^3"), 1, &error);
    AddToLib(kSIQuantityElectricChargeDensity, STR("charge density"), STR("charge density"), STR("A•h/L"), 3600000, &error);
    AddToLib(kSIQuantityElectricChargeDensity, STR("atomic unit of charge density"), STR("atomic unit of charge density"), STR("q_e/a_0^3"), kSIElementaryCharge / (a_0 * a_0 * a_0), &error);
//
#pragma mark kSIQuantitySurfaceChargeDensity, kSIQuantityElectricFluxDensity, kSIQuantityElectricDisplacement
    AddToLibPrefixed(kSIQuantitySurfaceChargeDensity, STR("coulomb per square meter"), STR("coulombs per square meter"), STR("C/m^2"), 1, &error);
    units = (OCMutableArrayRef)OCDictionaryGetValue(units2QuantitiesLibrary, kSIQuantitySurfaceChargeDensity);
    OCDictionaryAddValue(units2QuantitiesLibrary, kSIQuantityElectricFluxDensity, units);
    OCDictionaryAddValue(units2QuantitiesLibrary, kSIQuantityElectricDisplacement, units);
//
#pragma mark kSIQuantityPermeability
    AddToLibPrefixed(kSIQuantityPermeability, STR("henry per meter"), STR("henries per meter"), STR("H/m"), 1., &error);
    AddToLibPrefixed(kSIQuantityPermeability, STR("newton per square ampere"), STR("newtons per square ampere"), STR("N/A^2"), 1, &error);
    AddToLibPrefixed(kSIQuantityPermeability, STR("tesla meter per ampere"), STR("tesla meter per ampere"), STR("T•m/A"), 1, &error);
    AddToLibPrefixed(kSIQuantityPermeability, STR("weber per ampere meter"), STR("webers per ampere meter"), STR("Wb/(A•m)"), 1, &error);
    AddToLib(kSIQuantityPermeability, STR("magnetic constant"), STR("magnetic constant"), STR("µ_0"), 4 * kSIPi * 1.e-7, &error);
//
#pragma mark kSIQuantityMolarEntropy, kSIQuantityMolarHeatCapacity
    AddToLibPrefixed(kSIQuantityMolarEntropy, STR("joule per mole kelvin"), STR("joules per mole kelvin"), STR("J/(mol•K)"), 1, &error);
    AddToLib(kSIQuantityMolarEntropy, STR("gas constant"), STR("gas constant"), STR("R"), kSIBoltmannConstant * kSIAvogadroConstant, &error);
    units = (OCMutableArrayRef)OCDictionaryGetValue(units2QuantitiesLibrary, kSIQuantityMolarEntropy);
    OCDictionaryAddValue(units2QuantitiesLibrary, kSIQuantityMolarHeatCapacity, units);
//
#pragma mark kSIQuantityMolarEnergy
    AddToLibPrefixed(kSIQuantityMolarEnergy, STR("joule per mole"), STR("joules per mole"), STR("J/mol"), 1, &error);
    AddToLibPrefixed(kSIQuantityMolarEnergy, STR("calorie per mole"), STR("calories per mole"), STR("cal/mol"), 4.1868, &error);
//
#pragma mark kSIQuantityRadiationExposure
    AddToLibPrefixed(kSIQuantityRadiationExposure, STR("coulomb per kilogram"), STR("coulombs per kilogram"), STR("C/kg"), 1, &error);
//
#pragma mark kSIQuantityAbsorbedDoseRate
    AddToLibPrefixed(kSIQuantityAbsorbedDoseRate, STR("gray per second"), STR("grays per second"), STR("Gy/s"), 1, &error);
//
#pragma mark kSIQuantityRadiantIntensity
    AddToLibPrefixed(kSIQuantityRadiantIntensity, STR("watt per steradian"), STR("watts per steradian"), STR("W/sr"), 1, &error);
//
#pragma mark kSIQuantitySpectralRadiantIntensity
    AddToLibPrefixed(kSIQuantitySpectralRadiantIntensity, STR("watt per steradian per nanometer"), STR("watts per steradian per nanometer"), STR("W/(sr•nm)"), 1.e9, &error);
//
#pragma mark kSIQuantityRadiance
    AddToLibPrefixed(kSIQuantityRadiance, STR("watt per square meter per steradian"), STR("watts per square meter per steradian"), STR("W/(m^2•sr)"), 1, &error);
//
#pragma mark kSIQuantitySpectralRadiance
    AddToLibPrefixed(kSIQuantitySpectralRadiance, STR("watt per square meter per steradian per nanometer"), STR("watts per square meter steradian per nanometer"), STR("W/(m^2•sr•nm)"), 1.e9, &error);
//
#pragma mark kSIQuantityFrequencyPerMagneticFluxDensity
    AddToLibPrefixed(kSIQuantityFrequencyPerMagneticFluxDensity, STR("hertz per tesla"), STR("hertz per tesla"), STR("Hz/T"), 1, &error);
//
#pragma mark kSIQuantityLengthPerVolume
    AddToLibPrefixed(kSIQuantityLengthPerVolume, STR("meter per liter"), STR("meters per liter"), STR("m/L"), 1. / 1.e-3, &error);
//
#pragma mark kSIQuantityPowerPerLuminousFlux
    AddToLibPrefixed(kSIQuantityPowerPerLuminousFlux, STR("watt per lumen"), STR("watts per lumen"), STR("W/lm"), 1, &error);
//
#pragma mark kSIQuantityLuminousEfficacy
    AddToLibPrefixed(kSIQuantityLuminousEfficacy, STR("lumen per watt"), STR("lumens per watt"), STR("lm/W"), 1, &error);
//
#pragma mark kSIQuantityHeatTransferCoefficient
    AddToLibPrefixed(kSIQuantityHeatTransferCoefficient, STR("watt per square meter per kelvin"), STR("watts per square meter per kelvin"), STR("W/(m^2•K)"), 1, &error);
    AddToLib(kSIQuantityHeatTransferCoefficient, STR("Btu per hour per square foot per rankine"), STR("Btus per hour per square foot per rankine"), STR("Btu/(h•ft^2•°R)"), 5.678263340863113, &error);
    AddToLibPrefixed(kSIQuantityHeatTransferCoefficient, STR("calorie per hour per square meter per kelvin"), STR("calories per hour per square meter per kelvin"), STR("cal/(h•m^2•K)"), 1.163e-3, &error);
//
#pragma mark kSIQuantityChargePerAmount
    AddToLib(kSIQuantityChargePerAmount, STR("faraday constant"), STR("faraday constant"), STR("&F"), kSIElementaryCharge * kSIAvogadroConstant, &error);
    AddToLibPrefixed(kSIQuantityChargePerAmount, STR("coulomb per mole"), STR("coulombs per mole"), STR("C/mol"), 1.0, &error);
//
#pragma mark kSIQuantityGravitationalConstant
    AddToLib(kSIQuantityGravitationalConstant, STR("gravitational constant"), STR("gravitational constant"), STR("&G"), kSIGravitationalConstant, &error);
//
#pragma mark kSIQuantityVolumePerLength
    AddToLibPrefixed(kSIQuantityVolumePerLength, STR("liter per 100 kilometers"), STR("liters per 100 kilometers"), STR("L/(100 km)"), 1e-3 / 100000., &error);
//
#pragma mark kSIQuantityVolumetricFlowRate
    AddToLib(kSIQuantityVolumetricFlowRate, STR("cubic meter per hour"), STR("cubic meters per hour"), STR("m^3/h"), 1. / 3600., &error);
    AddToLib(kSIQuantityVolumetricFlowRate, STR("cubic meter per minute"), STR("cubic meters per minute"), STR("m^3/min"), 1. / 60., &error);
    AddToLib(kSIQuantityVolumetricFlowRate, STR("cubic meter per second"), STR("cubic meters per second"), STR("m^3/s"), 1., &error);
    AddToLib(kSIQuantityVolumetricFlowRate, STR("cubic centimeter per hour"), STR("cubic centimeters per hour"), STR("cm^3/h"), 1e-6 / 3600., &error);
    AddToLib(kSIQuantityVolumetricFlowRate, STR("cubic centimeter per minute"), STR("cubic centimeters per minute"), STR("cm^3/min"), 1e-6 / 60., &error);
    AddToLib(kSIQuantityVolumetricFlowRate, STR("cubic centimeter per second"), STR("cubic centimeters per second"), STR("cm^3/s"), 1e-6, &error);
    AddToLib(kSIQuantityVolumetricFlowRate, STR("cubic foot per hour"), STR("cubic feet per hour"), STR("ft^3/h"), (1609.344 / 5280) * (1609.344 / 5280) * (1609.344 / 5280) / 3600., &error);
    AddToLib(kSIQuantityVolumetricFlowRate, STR("cubic foot per minute"), STR("cubic feet per minute"), STR("ft^3/min"), (1609.344 / 5280) * (1609.344 / 5280) * (1609.344 / 5280) / 60., &error);
    AddToLib(kSIQuantityVolumetricFlowRate, STR("cubic foot per second"), STR("cubic feet per second"), STR("ft^3/s"), (1609.344 / 5280) * (1609.344 / 5280) * (1609.344 / 5280), &error);
//
#pragma mark kSIQuantityPowerPerAreaPerTemperatureToFourthPower
    AddToLib(kSIQuantityPowerPerAreaPerTemperatureToFourthPower, STR("stefan-boltzmann constant"), STR("stefan-boltzmann constant"), STR("σ"), kSIStefanBoltzmannConstant, &error);
//
#pragma mark kSIQuantityWavelengthDisplacementConstant
    AddToLib(kSIQuantityWavelengthDisplacementConstant, STR("wien wavelength displacement constant"), STR("wien wavelength displacement constant"), STR("b_λ"), kSIWeinDisplacementConstant, &error);
//
#pragma mark kSIQuantityGasPermeance
    AddToLib(kSIQuantityGasPermeance, STR("gas permeance unit"), STR("gas permeance unit"), STR("GPU"), 3.35e-10, &error);
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
SIUnit2Ref SIUnit2WithSymbol(OCStringRef symbol) {
    if (NULL == symbol) {
        return NULL;
    }
    if (NULL == units2Library) SIUnit2CreateLibraries();
    IF_NO_OBJECT_EXISTS_RETURN(units2Library, NULL);
    SIUnit2Ref unit = OCDictionaryGetValue(units2Library, symbol);
    return unit;
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
    SIUnit2Ref result = SIUnit2WithParameters(dimensionality,
                                              input->name,         // Keep original name for now
                                              input->plural_name,  // Keep original plural name
                                              simplified_symbol,
                                              scale);
    
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
    SIUnit2Ref result = SIUnit2WithParameters(dimensionality,
                                              theUnit1->name,         // Use first unit's name
                                              theUnit1->plural_name,  // Use first unit's plural name
                                              simplified_symbol,
                                              scale);
    
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
        dimensionality = SIDimensionalityByDividing(theUnit1->dimensionality, theUnit2->dimensionality, error);
    } else {
        dimensionality = SIDimensionalityByDividingWithoutReducing(theUnit1->dimensionality, theUnit2->dimensionality, error);
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
    SIUnit2Ref result = SIUnit2WithParameters(dimensionality,
                                              theUnit1->name,         // Use first unit's name
                                              theUnit1->plural_name,  // Use first unit's plural name
                                              simplified_symbol,
                                              scale);
    
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
static SIUnit2Ref SIUnit2ByTakingNthRootInternal(SIUnit2Ref theUnit,
                                                 int root,
                                                 double *unit_multiplier,
                                                 bool reduce,
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
    if (reduce) {
        dimensionality = SIDimensionalityByTakingNthRoot(theUnit->dimensionality, root, error);
    } else {
        dimensionality = SIDimensionalityByTakingNthRootWithoutReducing(theUnit->dimensionality, root, error);
    }
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
    OCStringRef simplified_symbol = SIUnit2CreateSimplifiedSymbol(new_symbol, reduce, error);
    
    // Create new unit with the simplified symbol and calculated scale
    SIUnit2Ref result = SIUnit2WithParameters(dimensionality,
                                              theUnit->name,         // Keep original name
                                              theUnit->plural_name,  // Keep original plural name
                                              simplified_symbol,
                                              scale);
    
    OCRelease(new_symbol);
    OCRelease(simplified_symbol);
    return result;
}

// Public API functions
SIUnit2Ref SIUnit2ByTakingNthRootWithoutReducing(SIUnit2Ref theUnit,
                                                 int root,
                                                 double *unit_multiplier,
                                                 OCStringRef *error) {
    return SIUnit2ByTakingNthRootInternal(theUnit, root, unit_multiplier, false, error);  // reduce = false
}

SIUnit2Ref SIUnit2ByTakingNthRoot(SIUnit2Ref theUnit,
                                  int root,
                                  double *unit_multiplier,
                                  OCStringRef *error) {
    return SIUnit2ByTakingNthRootInternal(theUnit, root, unit_multiplier, true, error);   // reduce = true
}