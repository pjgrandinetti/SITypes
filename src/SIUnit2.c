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
#include "SIUnitKey.h"
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
static SIUnit2Ref SIUnit2Create(SIDimensionalityRef dimensionality, OCStringRef name, OCStringRef plural_name, OCStringRef symbol, double scale_to_coherent_si) {
    struct impl_SIUnit2 *theUnit = SIUnit2Allocate();
    if (!theUnit) return NULL;
    theUnit->dimensionality = dimensionality;
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
static OCMutableDictionaryRef unitsLibrary = NULL;
static OCMutableDictionaryRef unitsQuantitiesLibrary = NULL;
static OCMutableDictionaryRef unitsDimensionalitiesLibrary = NULL;
static OCMutableArrayRef unitsNamesLibrary = NULL;
static bool imperialVolumes = false;
bool SIUnitsCreateLibraries(void);
OCMutableDictionaryRef SIUnitGetUnitsLib(void) {
    if (NULL == unitsLibrary) SIUnitsCreateLibraries();
    return unitsLibrary;
}
OCMutableDictionaryRef SIUnitGetQuantitiesLib(void) {
    if (NULL == unitsQuantitiesLibrary) SIUnitsCreateLibraries();
    return unitsQuantitiesLibrary;
}
OCMutableDictionaryRef SIUnitGetDimensionalitiesLib(void) {
    if (NULL == unitsDimensionalitiesLibrary) SIUnitsCreateLibraries();
    return unitsDimensionalitiesLibrary;
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
    OCMutableDictionaryRef unitsLib = SIUnitGetUnitsLib();
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
        OCMutableDictionaryRef unitsQuantitiesLib = SIUnitGetQuantitiesLib();
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
        OCMutableDictionaryRef unitsDimensionalitiesLib = SIUnitGetDimensionalitiesLib();
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
static const int _prefixExponents[] = {
#define X(pref, exp, sym, name) exp,
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
static void AddToLibWithSIPrefixes(
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
        return NULL;
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
            return NULL;
        }
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
            return NULL;
        }
    }
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
    OCStringRef error = NULL;
    AddToLibWithSIPrefixes(kSIQuantityLength, STR("meter"), STR("meters"), STR("m"), 1, &error);
    AddToLibWithSIPrefixes(kSIQuantityMass, STR("gram"), STR("grams"), STR("g"), 1, &error);
    AddToLibWithSIPrefixes(kSIQuantityTime, STR("second"), STR("seconds"), STR("s"), 1, &error);
    AddToLibWithSIPrefixes(kSIQuantityCurrent, STR("ampere"), STR("amperes"), STR("A"), 1, &error);
    AddToLibWithSIPrefixes(kSIQuantityTemperature, STR("kelvin"), STR("kelvin"), STR("K"), 1, &error);
    AddToLibWithSIPrefixes(kSIQuantityAmount, STR("mole"), STR("moles"), STR("mol"), 1, &error);
    AddToLibWithSIPrefixes(kSIQuantityLuminousIntensity, STR("candela"), STR("candelas"), STR("cd"), 1, &error);

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
    OCMutableArrayRef units = (OCMutableArrayRef)OCDictionaryGetValue(unitsQuantitiesLibrary, kSIQuantityWavenumber);
    OCDictionaryAddValue(unitsQuantitiesLibrary, kSIQuantityInverseLength, units);
//
#pragma mark kSIQuantityLengthRatio, kSIQuantityPlaneAngle
    AddToLibWithSIPrefixes(kSIQuantityLengthRatio, STR("meter per meter"), STR("meters per meter"), STR("m/m"), 1.0, &error);
    AddToLibWithSIPrefixes(kSIQuantityLengthRatio, STR("radian"), STR("radians"), STR("rad"), 1.0, &error);
    AddToLibWithSIPrefixes(kSIQuantityLengthRatio, STR("degree"), STR("degrees"), STR("°"), kSIPi / 180., &error);
    AddToLib(kSIQuantityLengthRatio, STR("pi"), STR("pi"), STR("π"), kSIPi, &error);
    units = (OCMutableArrayRef)OCDictionaryGetValue(unitsQuantitiesLibrary, kSIQuantityLengthRatio);
    OCDictionaryAddValue(unitsQuantitiesLibrary, kSIQuantityPlaneAngle, units);
//
#pragma mark kSIQuantityMass
    AddToLib(kSIQuantityMass, STR("microgram"), STR("micrograms"), STR("mcg"), 1e-9);
    AddToLibWithSIPrefixes(kSIQuantityMass, STR("tonne"), STR("tonnes"), STR("t"), 1e3);
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantityMass, STR("dalton"), STR("daltons"), STR("Da"), kSIAtomicMassConstant);
    AddToLib(kSIQuantityMass, STR("atomic mass unit"), STR("atomic mass units"), STR("u"), kSIAtomicMassConstant);
    AddToLib(kSIQuantityMass, STR("atomic mass constant"), STR("atomic mass constant"), STR("m_u"), kSIAtomicMassConstant);
    AddToLib(kSIQuantityMass, STR("electron mass"), STR("electron mass"), STR("m_e"), kSIElectronMass);
    AddToLib(kSIQuantityMass, STR("proton mass"), STR("proton mass"), STR("m_p"), kSIProtonMass);
    AddToLib(kSIQuantityMass, STR("neutron mass"), STR("neutron mass"), STR("m_n"), kSINeutronMass);
    AddToLib(kSIQuantityMass, STR("alpha particle mass"), STR("alpha particle mass"), STR("m_a"), kSIAlphaParticleMass);
    AddToLib(kSIQuantityMass, STR("muon mass"), STR("muon mass"), STR("m_µ"), kSIMuonMass);
    AddToLib(kSIQuantityMass, STR("ton"), STR("tons"), STR("ton"), 0.45359237 * 2000);
    AddToLib(kSIQuantityMass, STR("hundredweight"), STR("hundredweight"), STR("cwt"), 0.45359237 * 100);
    AddToLib(kSIQuantityMass, STR("pound"), STR("pounds"), STR("lb"), 0.45359237);
    AddToLib(kSIQuantityMass, STR("stone"), STR("stones"), STR("st"), 6.35029318);
    AddToLib(kSIQuantityMass, STR("ounce"), STR("ounces"), STR("oz"), 0.028349523125);
    AddToLib(kSIQuantityMass, STR("grain"), STR("grains"), STR("gr"), 0.45359237 / 7000);
    AddToLib(kSIQuantityMass, STR("dram"), STR("drams"), STR("dr"), 0.45359237 / 256);
    AddToLib(kSIQuantityMass, STR("tonUK"), STR("tonsUK"), STR("tonUK"), 0.45359237 * 2240);
    AddToLib(kSIQuantityMass, STR("hundredweightUK"), STR("hundredweightUK"), STR("cwtUK"), 0.45359237 * 112);
    AddToLib(kSIQuantityMass, STR("planck mass"), STR("planck mass"), STR("m_P"), planckMass);
//
#pragma mark kSIQuantityInverseMass
    AddToLib(kSIQuantityInverseMass, STR("inverse kilogram"), STR("inverse kilograms"), STR("(1/kg)"), 1.);
    AddToLib(kSIQuantityInverseMass, STR("inverse tonne"), STR("inverse tonnes"), STR("(1/t)"), 1. / 1e3);
    AddToLib(kSIQuantityInverseMass, STR("inverse ton"), STR("inverse tons"), STR("(1/ton)"), 1. / (0.45359237 * 2000));
    AddToLib(kSIQuantityInverseMass, STR("inverse stone"), STR("inverse stones"), STR("(1/st)"), 1. / 6.35029318);
    AddToLib(kSIQuantityInverseMass, STR("inverse pound"), STR("inverse pounds"), STR("(1/lb)"), 1. / 0.45359237);
    AddToLib(kSIQuantityInverseMass, STR("inverse ounce"), STR("inverse ounces"), STR("(1/oz)"), 1. / 0.028349523125);
//
#pragma mark kSIQuantityMassRatio
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantityMassRatio, STR("gram per kilogram"), STR("grams per kilogram"), STR("g/kg"), 0.001);
//
#pragma mark kSIQuantityTime
    AddToLib(kSIQuantityTime, STR("minute"), STR("minutes"), STR("min"), kSIMinute);
    AddToLib(kSIQuantityTime, STR("hour"), STR("hours"), STR("h"), kSIHour);
    AddToLib(kSIQuantityTime, STR("day"), STR("days"), STR("d"), kSIDay);
    AddToLib(kSIQuantityTime, STR("week"), STR("weeks"), STR("wk"), kSIWeek);
    AddToLib(kSIQuantityTime, STR("month"), STR("months"), STR("mo"), kSIMonth);
    AddToLib(kSIQuantityTime, STR("year"), STR("years"), STR("yr"), kSIYear);
    AddToLib(kSIQuantityTime, STR("decade"), STR("decades"), STR("dayr"), kSIDecade);
    AddToLib(kSIQuantityTime, STR("century"), STR("centuries"), STR("hyr"), kSICentury);
    AddToLib(kSIQuantityTime, STR("millennium"), STR("millennia"), STR("kyr"), kSIMillennium);
    AddToLib(kSIQuantityTime, STR("atomic unit of time"), STR("atomic units of time"), STR("ℏ/E_h"), hbar / E_h);
    AddToLib(kSIQuantityTime, STR("natural unit of time"), STR("natural units of time"), STR("ℏ/(m_e•c_0^2)"), hbar / (kSIElectronMass * kSISpeedOfLight * kSISpeedOfLight));
    AddToLib(kSIQuantityTime, STR("planck time"), STR("planck time"), STR("t_P"), planckTime);
//
#pragma mark kSIQuantityInverseTime
    AddToLib(kSIQuantityInverseTime, STR("inverse seconds"), STR("inverse seconds"), STR("(1/s)"), 1.);
    AddToLib(kSIQuantityInverseTime, STR("inverse minute"), STR("inverse minutes"), STR("(1/min)"), 1. / 60.);
    AddToLib(kSIQuantityInverseTime, STR("inverse hour"), STR("inverse hours"), STR("(1/h)"), 1. / (60. * 60.));
    AddToLib(kSIQuantityInverseTime, STR("inverse day"), STR("inverse days"), STR("(1/d)"), 1. / (60. * 60 * 24.));
    AddToLib(kSIQuantityInverseTime, STR("inverse week"), STR("inverse weeks"), STR("(1/wk)"), 1. / (60. * 60 * 24. * 7.));
    AddToLib(kSIQuantityInverseTime, STR("inverse month"), STR("inverse months"), STR("(1/month)"), 1. / (365.25 * 86400 / 12.));
    AddToLib(kSIQuantityInverseTime, STR("inverse year"), STR("inverse years"), STR("(1/yr)"), 1. / (365.25 * 86400));
//
#pragma mark kSIQuantityFrequency
    AddToLibWithSIPrefixes(kSIQuantityFrequency, STR("hertz"), STR("hertz"), STR("Hz"),1.0);
//
#pragma mark kSIQuantityRadioactivity
    AddToLibWithSIPrefixes(kSIQuantityRadioactivity, STR("becquerel"), STR("becquerels"), STR("Bq"),1.0);
    AddToLibWithSIPrefixes(kSIQuantityRadioactivity, STR("curie"), STR("curies"), STR("Ci"), 3.7e10);
//
#pragma mark kSIQuantityFrequencyRatio
    AddToLibWithSIPrefixes(kSIQuantityFrequencyRatio, STR("hertz per hertz"), STR("hertz per hertz"), STR("Hz/Hz"));
//
#pragma mark kSIQuantityTimeRatio
    AddToLibWithSIPrefixes(kSIQuantityTimeRatio, STR("second per second"), STR("seconds per second"), STR("s/s"));
//
#pragma mark kSIQuantityInverseTimeSquared
    AddToLib(kSIQuantityInverseTimeSquared, STR("inverse millisecond squared"), STR("inverse milliseconds squared"), STR("(1/ms^2)"), 1000000.);
    AddToLib(kSIQuantityInverseTimeSquared, STR("inverse second squared"), STR("inverse seconds squared"), STR("(1/s^2)"), 1.);
    AddToLib(kSIQuantityInverseTimeSquared, STR("inverse hour inverse second"), STR("inverse hour inverse seconds"), STR("(1/(h•s))"), 1. / 3600.);
    AddToLib(kSIQuantityInverseTimeSquared, STR("inverse hour inverse minute"), STR("inverse hour inverse minutes"), STR("(1/(h•min))"), 1. / 3600. / 60.);
//
#pragma mark kSIQuantityLinearMomentum
    AddToLib(kSIQuantityLinearMomentum, STR("natural unit of momentum"), STR("natural units of momentum"), STR("m_e•c_0"), kSIElectronMass * kSISpeedOfLight);
    AddToLibWithSIPrefixes(kSIQuantityLinearMomentum, STR("gram meter per second"), STR("gram meters per second"), STR("g•m/s"), 0.001);
    AddToLibWithSIPrefixes(kSIQuantityLinearMomentum, STR("newton second"), STR("newton seconds"), STR("N•s"));
    AddToLib(kSIQuantityLinearMomentum, STR("atomic unit of momentum"), STR("atomic units of momentum"), STR("ℏ/a_0"), hbar / a_0);
//
#pragma mark kSIQuantityEnergy
    AddToLib(kSIQuantityEnergy, STR("natural unit of energy"), STR("natural units of energy"), STR("m_e•c_0^2"), kSIElectronMass * kSISpeedOfLight * kSISpeedOfLight);
    AddToLibWithSIPrefixes(kSIQuantityEnergy, STR("joule"), STR("joules"), STR("J"));
    AddToLibWithSIPrefixes(kSIQuantityEnergy, STR("watt hour"), STR("watt hour"), STR("W•h"), 3.6e3);
    AddToLib(kSIQuantityEnergy, STR("rydberg"), STR("rydbergs"), STR("Ry"), Ry);
    AddToLib(kSIQuantityEnergy, STR("alpha particle mass energy"), STR("alpha particle mass energy"), STR("m_a•c_0^2"), kSIAlphaParticleMass * kSISpeedOfLight * kSISpeedOfLight);
    AddToLibWithSIPrefixes(kSIQuantityEnergy, STR("electronvolt"), STR("electronvolts"), STR("eV"), kSIElementaryCharge);
    AddToLibWithSIPrefixes(kSIQuantityEnergy, STR("erg"), STR("ergs"), STR("erg"), 1e-7);
    AddToLibWithSIPrefixes(kSIQuantityEnergy, STR("calorie"), STR("calories"), STR("cal"), 4.1868);
    AddToLib(kSIQuantityEnergy, STR("atomic unit of energy"), STR("atomic unit of energy"), STR("E_h"), E_h);
    AddToLib(kSIQuantityEnergy, STR("british thermal unit"), STR("british thermal units"), STR("Btu"), 1055.05585257348);
//
#pragma mark kSIQuantityTemperature
    AddToLib(kSIQuantityTemperature, STR("planck temperature"), STR("planck temperature"), STR("T_P"), planckTemperature);
    AddToLib(kSIQuantityTemperature, STR("rankine"), STR("rankines"), STR("°R"), 0.555555555555556);
    AddToLib(kSIQuantityTemperature, STR("fahrenheit"), STR("fahrenheit"), STR("°F"), 0.555555555555556);
    AddToLib(kSIQuantityTemperature, STR("celsius"), STR("celsius"), STR("°C"), 1);
//
#pragma mark kSIQuantityCurrent
    AddToLib(kSIQuantityCurrent, STR("atomic unit of current"), STR("atomic unit of current"), STR("q_e•E_h/ℏ"), kSIElementaryCharge * E_h / hbar);
//
#pragma mark kSIQuantityInverseCurrent
    AddToLib(kSIQuantityInverseCurrent, STR("inverse ampere"), STR("inverse amperes"), STR("(1/A)"), 1.);
//
#pragma mark kSIQuantityCurrentRatio
    AddToLibWithSIPrefixes(kSIQuantityCurrentRatio, STR("ampere per ampere"), STR("ampere per ampere"), STR("A/A"));
//
#pragma mark kSIQuantityInverseTemperature
    AddToLib(kSIQuantityInverseTemperature, STR("inverse kelvin"), STR("inverse kelvin"), STR("(1/K)"), 1.);
//
#pragma mark kSIQuantityTemperatureRatio
    AddToLibWithSIPrefixes(kSIQuantityTemperatureRatio, STR("kelvin per kelvin"), STR("kelvin per kelvin"), STR("K/K"));
//
#pragma mark kSIQuantityTemperatureGradient
    AddToLibWithSIPrefixes(kSIQuantityTemperatureGradient, STR("kelvin per meter"), STR("kelvin per meter"), STR("K/m"), 1);
    AddToLib(kSIQuantityTemperatureGradient, STR("celsius per meter"), STR("celsius per meter"), STR("°C/m"), 1);
    AddToLib(kSIQuantityTemperatureGradient, STR("fahrenheit per foot"), STR("fahrenheit per foot"), STR("°F/ft"), 0.555555555555556 / (1609.344 / 5280));
    AddToLib(kSIQuantityTemperatureGradient, STR("rankine per foot"), STR("rankines per foot"), STR("°R/ft"), 0.555555555555556 / (1609.344 / 5280));
//
#pragma mark kSIQuantityInverseAmount
    AddToLib(kSIQuantityInverseAmount, STR("inverse mole"), STR("inverse moles"), STR("(1/mol)"), 1.);
    AddToLib(kSIQuantityInverseAmount, STR("avogadro constant"), STR("avogadro constant"), STR("N_A"), kSIAvogadroConstant);
//
#pragma mark kSIQuantityAmountRatio
    AddToLibWithSIPrefixes(kSIQuantityAmountRatio, STR("mole per mole"), STR("moles per mole"), STR("mol/mol"));
//
#pragma mark kSIQuantityInverseLuminousIntensity
    AddToLib(kSIQuantityInverseLuminousIntensity, STR("inverse candela"), STR("inverse candelas"), STR("(1/cd)"), 1.);
//
#pragma mark kSIQuantityLuminousIntensityRatio
    AddToLibWithSIPrefixes(kSIQuantityLuminousIntensityRatio, STR("candela per candela"), STR("candelas per candela"), STR("cd/cd"));
//
#pragma mark kSIQuantityArea
    AddToLib(kSIQuantityArea, STR("hectare"), STR("hectares"), STR("ha"), 1e4);
    AddToLib(kSIQuantityArea, STR("barn"), STR("barns"), STR("b"), 1.e-28);
    AddToLib(kSIQuantityArea, STR("square meter"), STR("square meters"), STR("m^2"), 1);
    AddToLib(kSIQuantityArea, STR("square centimeter"), STR("square centimeters"), STR("cm^2"), 0.0001);
    AddToLib(kSIQuantityArea, STR("square kilometer"), STR("square kilometers"), STR("km^2"), 1000000);
    AddToLib(kSIQuantityArea, STR("square inch"), STR("square inches"), STR("in^2"), 0.00064516);
    AddToLib(kSIQuantityArea, STR("square foot"), STR("square feet"), STR("ft^2"), 0.09290304);
    AddToLib(kSIQuantityArea, STR("square yard"), STR("square yards"), STR("yd^2"), 0.83612736);
    AddToLib(kSIQuantityArea, STR("square mile"), STR("square miles"), STR("mi^2"), 2589988.110336);
    AddToLib(kSIQuantityArea, STR("square rod"), STR("square rods"), STR("rod^2"), 5.029210 * 5.029210);
    AddToLib(kSIQuantityArea, STR("square chain"), STR("square chains"), STR("ch^2"), 5.029210 * 5.029210 * 16);
    AddToLib(kSIQuantityArea, STR("acre"), STR("acres"), STR("ac"), 4046.8564224);
    AddToLib(kSIQuantityArea, STR("township"), STR("townships"), STR("twp"), 2589988.110336 * 36.);
//
#pragma mark kSIQuantityInverseArea
    AddToLib(kSIQuantityInverseArea, STR("inverse hectare"), STR("inverse hectares"), STR("(1/ha)"), 1e-4);
    AddToLib(kSIQuantityInverseArea, STR("inverse barn"), STR("inverse barns"), STR("(1/b)"), 1.e28);
    AddToLib(kSIQuantityInverseArea, STR("inverse square kilometer"), STR("inverse square kilometer"), STR("(1/km^2)"), 1. / 1000000);
    AddToLib(kSIQuantityInverseArea, STR("inverse square centimeter"), STR("inverse square centimeters"), STR("(1/cm^2)"), 1. / 0.0001);
    AddToLib(kSIQuantityInverseArea, STR("inverse square inch"), STR("inverse square inches"), STR("(1/in^2)"), 1. / 0.00064516);
    AddToLib(kSIQuantityInverseArea, STR("inverse square foot"), STR("inverse square feet"), STR("(1/ft^2)"), 1. / 0.09290304);
    AddToLib(kSIQuantityInverseArea, STR("inverse square yard"), STR("inverse square yards"), STR("(1/yd^2)"), 1. / 0.83612736);
    AddToLib(kSIQuantityInverseArea, STR("inverse acre"), STR("inverse acres"), STR("(1/ac)"), 1. / 4046.8564224);
    AddToLib(kSIQuantityInverseArea, STR("inverse square mile"), STR("inverse square miles"), STR("(1/mi^2)"), 1. / 2589988.110336);
//
#pragma mark kSIQuantityRockPermeability
    AddToLibWithSIPrefixes(kSIQuantityRockPermeability, STR("darcy"), STR("darcys"), STR("Dc"), 9.869233e-13);
//
#pragma mark kSIQuantitySolidAngle
    AddToLib(kSIQuantitySolidAngle, STR("steradian"), STR("steradians"), STR("sr"));
//
#pragma mark kSIQuantityAreaRatio
    AddToLib(kSIQuantityAreaRatio, STR("square meter per square meter"), STR("square meters per square meter"), STR("m^2/m^2"), 1);
//
#pragma mark kSIQuantityVolume
    AddToLibWithSIPrefixes(kSIQuantityVolume, STR("liter"), STR("liters"), STR("L"), 1e-3);
    AddToLib(kSIQuantityVolume, STR("cubic inch"), STR("cubic inches"), STR("in^3"), (1609.344 / 63360) * (1609.344 / 63360) * (1609.344 / 63360));
    AddToLib(kSIQuantityVolume, STR("cubic foot"), STR("cubic feet"), STR("ft^3"), (1609.344 / 5280) * (1609.344 / 5280) * (1609.344 / 5280));
    AddToLib(kSIQuantityVolume, STR("cubic yard"), STR("cubic yards"), STR("yd^3"), (1609.344 / 1760) * (1609.344 / 1760) * (1609.344 / 1760));
    AddToLib(kSIQuantityVolume, STR("acre foot"), STR("acre feet"), STR("ac•ft"), 1609.344 / 5280 * 4046.8564224);
    AddToLib(kSIQuantityVolume, STR("oil barrel"), STR("oil barrels"), STR("bbl"), 0.158987295);
    AddToLib(kSIQuantityVolume, STR("thousand oil barrels"), STR("thousand oil barrels"), STR("Mbbl"), 0.158987295e3);
    AddToLib(kSIQuantityVolume, STR("million oil barrels"), STR("million oil barrels"), STR("MMbbl"), 0.158987295e6);
//
#pragma mark kSIQuantityInverseVolume
    AddToLib(kSIQuantityInverseVolume, STR("inverse liter"), STR("inverse liters"), STR("(1/L)"), 1. / 1e-3);
    AddToLib(kSIQuantityInverseVolume, STR("inverse milliliter"), STR("inverse milliliters"), STR("(1/mL)"), 1. / 1e-6);
    AddToLib(kSIQuantityInverseVolume, STR("inverse cubic inch"), STR("inverse cubic inches"), STR("(1/in^3)"), 1. / ((1609.344 / 63360) * (1609.344 / 63360) * (1609.344 / 63360)));
    AddToLib(kSIQuantityInverseVolume, STR("inverse cubic foot"), STR("inverse cubic feet"), STR("(1/ft^3)"), 1. / ((1609.344 / 5280) * (1609.344 / 5280) * (1609.344 / 5280)));
    AddToLib(kSIQuantityInverseVolume, STR("inverse cubic yard"), STR("inverse cubic yards"), STR("(1/yd^3)"), 1. / ((1609.344 / 1760) * (1609.344 / 1760) * (1609.344 / 1760)));
    AddToLib(kSIQuantityInverseVolume, STR("inverse cubic centimeter"), STR("inverse cubic centimeters"), STR("(1/cm^3)"), 1000000.);
    AddToLib(kSIQuantityInverseVolume, STR("inverse cubic millimeter"), STR("inverse cubic millimeters"), STR("(1/mm^3)"), 1000000000.);
    AddToLib(kSIQuantityInverseVolume, STR("inverse cubic micrometer"), STR("inverse cubic micrometers"), STR("(1/µm^3)"), 1e+18);
    AddToLib(kSIQuantityInverseVolume, STR("inverse cubic ångström"), STR("inverse cubic ångströms"), STR("(1/Å^3)"), 1e+30);
//
#pragma mark kSIQuantityVolumeRatio
    AddToLib(kSIQuantityVolumeRatio, STR("cubic meter per cubic meter"), STR("cubic meters per cubic meter"), STR("m^3/m^3"), 1);
//
#pragma mark kSIQuantitySurfaceAreaToVolumeRatio
    AddToLib(kSIQuantitySurfaceAreaToVolumeRatio, STR("square meter per cubic meter"), STR("square meters per cubic meter"), STR("m^2/m^3"), 1);
    AddToLib(kSIQuantitySurfaceAreaToVolumeRatio, STR("square meter per liter"), STR("square meters per liter"), STR("m^2/L"), 1000);
//
#pragma mark kSIQuantitySpeed
    AddToLibWithSIPrefixes(kSIQuantitySpeed, STR("meter per second"), STR("meters per second"), STR("m/s"));
    AddToLibWithSIPrefixes(kSIQuantitySpeed, STR("meter per minute"), STR("meters per minute"), STR("m/min"), 1. / 60.);
    AddToLibWithSIPrefixes(kSIQuantitySpeed, STR("meter per hour"), STR("meters per hour"), STR("m/h"), 1. / 3600.);
    AddToLib(kSIQuantitySpeed, STR("knot"), STR("knots"), STR("kn"), 0.514444444444444);
    AddToLib(kSIQuantitySpeed, STR("speed of light"), STR("speed of light"), STR("c_0"), kSISpeedOfLight);
    AddToLib(kSIQuantitySpeed, STR("inch per second"), STR("inches per second"), STR("in/s"), 1609.344 / 63360);
    AddToLib(kSIQuantitySpeed, STR("inch per minute"), STR("inches per minute"), STR("in/min"), 1609.344 / 63360 / 60.);
    AddToLib(kSIQuantitySpeed, STR("inch per hour"), STR("inches per hour"), STR("in/h"), 1609.344 / 63360 / 3600.);
    AddToLib(kSIQuantitySpeed, STR("foot per second"), STR("feet per second"), STR("ft/s"), 1609.344 / 5280);
    AddToLib(kSIQuantitySpeed, STR("foot per minute"), STR("feet per minute"), STR("ft/min"), 1609.344 / 5280 / 60.);
    AddToLib(kSIQuantitySpeed, STR("foot per hour"), STR("feet per hour"), STR("ft/h"), 1609.344 / 5280 / 3600.);
    AddToLib(kSIQuantitySpeed, STR("mile per second"), STR("miles per second"), STR("mi/s"), 1609.344);
    AddToLib(kSIQuantitySpeed, STR("mile per minute"), STR("miles per minute"), STR("mi/min"), 1609.344 / 60.);
    AddToLib(kSIQuantitySpeed, STR("mile per hour"), STR("miles per hour"), STR("mi/h"), 1609.344 / 3600.);
    AddToLib(kSIQuantitySpeed, STR("atomic unit of velocity"), STR("atomic units of velocity"), STR("a_0•E_h/ℏ"), a_0 * E_h / hbar);
    units = (OCMutableArrayRef)OCDictionaryGetValue(unitsQuantitiesLibrary, kSIQuantitySpeed);
    OCDictionaryAddValue(unitsQuantitiesLibrary, kSIQuantityVelocity, units);
//
#pragma mark kSIQuantityAngularMomentum
    AddToLibWithSIPrefixes(kSIQuantityAngularMomentum, STR("joule second"), STR("joules second"), STR("J•s"));
    AddToLibWithSIPrefixes(kSIQuantityAngularMomentum, STR("gram square meter per second"), STR("gram square meters per second"), STR("g•m^2/s"), 0.001);
//
#pragma mark kSIQuantityAction
    AddToLib(kSIQuantityAction, STR("planck constant"), STR("planck constant"), STR("h_P"), kSIPlanckConstant);
//
#pragma mark kSIQuantityReducedAction
    AddToLib(kSIQuantityReducedAction, STR("reduced planck constant"), STR("reduced planck constant"), STR("ℏ"), hbar);
//
#pragma mark kSIQuantityCirculation
    AddToLib(kSIQuantityCirculation, STR("quantum of circulation"), STR("quantum of circulation"), STR("h_P/(2•m_e)"), hbar);
//
#pragma mark kSIQuantitySecondRadiationConstant
    AddToLib(kSIQuantitySecondRadiationConstant, STR("second radiation constant"), STR("second radiation constant"), STR("h_P•c_0/k_B"), kSIPlanckConstant * kSISpeedOfLight / kSIBoltmannConstant);
//
#pragma mark kSIQuantityElectricResistance
    AddToLib(kSIQuantityElectricResistance, STR("von klitzing constant"), STR("von klitzing constant"), STR("h_P/(q_e^2)"), kSIPlanckConstant / (kSIElementaryCharge * kSIElementaryCharge));
    AddToLib(kSIQuantityElectricResistance, STR("ohm"), STR("ohms"), STR("Ω"));
    AddToLib(kSIQuantityElectricResistance, STR("inverse conductance quantum"), STR("inverse conductance quantum"), STR("(1/G_0)"), 1 / G_0);
    AddToLib(kSIQuantityElectricResistance, STR("characteristic impedance of vacuum"), STR("characteristic impedance of vacuum"), STR("Z_0"), 4 * kSIPi * 1.e-7 * kSISpeedOfLight);
//
#pragma mark kSIQuantityAcceleration
    AddToLibWithSIPrefixes(kSIQuantityAcceleration, STR("meter per square second"), STR("meters per square second"), STR("m/s^2"));
    AddToLibWithSIPrefixes(kSIQuantityAcceleration, STR("meter per hour per second"), STR("meters per hour per second"), STR("m/(h•s)"), 1. / 3600.);
    AddToLib(kSIQuantityAcceleration, STR("gravity acceleration"), STR("gravity acceleration"), STR("g_0"), kSIGravityAcceleration);
    AddToLib(kSIQuantityAcceleration, STR("mile per square second"), STR("miles per square second"), STR("mi/s^2"), 1609.344);
    AddToLib(kSIQuantityAcceleration, STR("foot per square second"), STR("feet per square second"), STR("ft/s^2"), 1609.344 / 5280);
    AddToLib(kSIQuantityAcceleration, STR("inch per square second"), STR("inches per square second"), STR("in/s^2"), 1609.344 / 63360);
    AddToLib(kSIQuantityAcceleration, STR("mile per square minute"), STR("miles per square minute"), STR("mi/min^2"), 1609.344 / 60. / 60.);
    AddToLib(kSIQuantityAcceleration, STR("foot per square minute"), STR("feet per square minute"), STR("ft/min^2"), 1609.344 / 5280 / 60. / 60.);
    AddToLib(kSIQuantityAcceleration, STR("inch per square minute"), STR("inches per square minute"), STR("in/min^2"), 1609.344 / 63360 / 60. / 60.);
    AddToLib(kSIQuantityAcceleration, STR("mile per hour per second"), STR("miles per hour per second"), STR("mi/(h•s)"), 1609.344 / 60. / 60.);
    AddToLib(kSIQuantityAcceleration, STR("knot per second"), STR("knots per second"), STR("kn/s"), 0.514444444444444);
    AddToLib(kSIQuantityAcceleration, STR("galileo"), STR("galileo"), STR("Gal"), 1e-2);
//
#pragma mark kSIQuantityDensity
    AddToLibWithSIPrefixes(kSIQuantityDensity, STR("gram per cubic meter"), STR("grams per cubic meter"), STR("g/m^3"), 1e-3);
//
#pragma mark kSIQuantityMassFlowRate
    AddToLibWithSIPrefixes(kSIQuantityMassFlowRate, STR("gram per second"), STR("grams per second"), STR("g/s"), 1e-3);
//
#pragma mark kSIQuantityMassFlux
    AddToLibWithSIPrefixes(kSIQuantityMassFlux, STR("gram per square meter per second"), STR("grams per square meter per second"), STR("g/(m^2•s)"), 1e-3);
//
#pragma mark kSIQuantitySurfaceDensity
    AddToLibWithSIPrefixes(kSIQuantitySurfaceDensity, STR("gram per square meter"), STR("grams per square meter"), STR("g/m^2"), 1e-3);
//
#pragma mark kSIQuantityCurrentDensity
    AddToLibWithSIPrefixes(kSIQuantityCurrentDensity, STR("ampere per square meter"), STR("amperes per square meter"), STR("A/m^2"));
//
#pragma mark kSIQuantityAmountConcentration
    AddToLibWithSIPrefixes(kSIQuantityAmountConcentration, STR("mole per cubic meter"), STR("moles per cubic meter"), STR("mol/m^3"));
    AddToLibWithSIPrefixes(kSIQuantityAmountConcentration, STR("mole per liter"), STR("moles per liter"), STR("mol/L"), 1000.);
    AddToLibWithSIPrefixes(kSIQuantityAmountConcentration, STR("mole per milliliter"), STR("moles per milliliter"), STR("mol/mL"), 1000000.);
    AddToLibWithSIPrefixes(kSIQuantityAmountConcentration, STR("mole per microliter"), STR("moles per microliter"), STR("mol/µL"), 1000000000.);
    AddToLibWithSIPrefixes(kSIQuantityAmountConcentration, STR("mole per liter"), STR("moles per liter"), STR("M"), 1000.);
//
#pragma mark kSIQuantityMassConcentration
    AddToLibWithSIPrefixes(kSIQuantityMassConcentration, STR("gram per liter"), STR("grams per liter"), STR("g/L"));
    AddToLibWithSIPrefixes(kSIQuantityMassConcentration, STR("gram per milliliter"), STR("grams per milliliter"), STR("g/mL"), 1e3);
    AddToLibWithSIPrefixes(kSIQuantityMassConcentration, STR("gram per cubic centimeter"), STR("grams per cubic centimeter"), STR("g/cm^3"), 1e3);
    AddToLibWithSIPrefixes(kSIQuantityMassConcentration, STR("gram per microliter"), STR("grams per microliter"), STR("g/µL"), 1e6);
//
#pragma mark kSIQuantityForce
    AddToLibWithSIPrefixes(kSIQuantityForce, STR("newton"), STR("newtons"), STR("N"));
    AddToLib(kSIQuantityForce, STR("atomic unit of force"), STR("atomic units of force"), STR("E_h/a_0"), E_h / a_0);
    AddToLibWithSIPrefixes(kSIQuantityForce, STR("dyne"), STR("dynes"), STR("dyn"), 1e-5);
    AddToLib(kSIQuantityForce, STR("pound force"), STR("pounds force"), STR("lbf"), 4.4482216152605);
    AddToLib(kSIQuantityForce, STR("ounce force"), STR("ounces force"), STR("ozf"), 4.4482216152605 / 16.);
    AddToLibWithSIPrefixes(kSIQuantityForce, STR("gram force"), STR("grams force"), STR("gf"), 0.00980665);
//
#pragma mark kSIQuantityTorque
    AddToLibWithSIPrefixes(kSIQuantityTorque, STR("newton meter per radian"), STR("newton meters per radian"), STR("N•m/rad"));
    AddToLibWithSIPrefixes(kSIQuantityTorque, STR("joule per radian"), STR("joules per radian"), STR("J/rad"));
    AddToLib(kSIQuantityTorque, STR("pound force foot per radian"), STR("pound force feet per radian"), STR("lbf•ft/rad"), 1.3558179483314);
    AddToLib(kSIQuantityTorque, STR("pound force inch per radian"), STR("pound force inches per radian"), STR("lbf•in/rad"), 1.3558179483314 / 12.);
    AddToLibWithSIPrefixes(kSIQuantityTorque, STR("gram force meter per radian"), STR("gram force meters per radian"), STR("gf•m/rad"), 0.00980665);
    AddToLibWithSIPrefixes(kSIQuantityTorque, STR("gram force centimeter per radian"), STR("gram force centimeters per radian"), STR("gf•cm/rad"), 9.80665e-05);
//
#pragma mark kSIQuantityMomentOfInertia
    AddToLib(kSIQuantityMomentOfInertia, STR("meter squared kilogram"), STR("meters squared kilogram"), STR("m^2•kg"), 1);
    AddToLib(kSIQuantityMomentOfInertia, STR("meter squared gram"), STR("meters squared gram"), STR("m^2•g"), 1.e-3);
    AddToLib(kSIQuantityMomentOfInertia, STR("centimeter squared kilogram"), STR("centimeter squared kilogram"), STR("cm^2•kg"), 0.0001);
    AddToLib(kSIQuantityMomentOfInertia, STR("centimeter squared gram"), STR("centimeter squared gram"), STR("cm^2•g"), 1.e-7);
//
#pragma mark kSIQuantityPressure, kSIQuantityStress, kSIQuantityElasticModulus
    AddToLibWithSIPrefixes(kSIQuantityPressure, STR("pascal"), STR("pascals"), STR("Pa"));
    AddToLib(kSIQuantityPressure, STR("pound force per square inch"), STR("pounds force per square inch"), STR("lbf/in^2"), 6894.75729);
    AddToLib(kSIQuantityPressure, STR("pound force per square inch"), STR("pounds force per square inch"), STR("psi"), 6894.75729);
    AddToLib(kSIQuantityPressure, STR("pound force per square foot"), STR("pounds force per square feet"), STR("lbf/ft^2"), 47.880259);
    AddToLibWithSIPrefixes(kSIQuantityPressure, STR("torr"), STR("torrs"), STR("Torr"), 1.01325e5 / 760);
    AddToLibWithSIPrefixes(kSIQuantityPressure, STR("bar"), STR("bars"), STR("bar"), 1e5);
    AddToLib(kSIQuantityPressure, STR("millimeter of Hg"), STR("millimeters of Hg"), STR("mmHg"), 133.322);
    AddToLib(kSIQuantityPressure, STR("atmosphere"), STR("atmospheres"), STR("atm"), 1.01325e5);
    AddToLibWithSIPrefixes(kSIQuantityPressure, STR("newton per square meter"), STR("newtons per square meter"), STR("N/m^2"));
    units = (OCMutableArrayRef)OCDictionaryGetValue(unitsQuantitiesLibrary, kSIQuantityPressure);
    OCDictionaryAddValue(unitsQuantitiesLibrary, kSIQuantityStress, units);
    OCDictionaryAddValue(unitsQuantitiesLibrary, kSIQuantityElasticModulus, units);
//
#pragma mark kSIQuantityCompressibility
    AddToLib(kSIQuantityCompressibility, STR("inverse pascal"), STR("inverse pascals"), STR("1/Pa"), 1);
//
#pragma mark kSIQuantityStressOpticCoefficient
    AddToLib(kSIQuantityStressOpticCoefficient, STR("brewster"), STR("brewsters"), STR("B"), 1.e-12);
//
#pragma mark kSIQuantityPressureGradient
    AddSpecialSIUnit(kSIQuantityPressureGradient, STR("pascal per meter"), STR("pascals per meter"), STR("Pa/m"));
    AddToLibWithSIPrefixes(kSIQuantityPressureGradient, STR("pound force per square inch per foot"), STR("pounds force per square inch per foot"), STR("psi/ft"), 6894.75729 / (1609.344 / 5280));
//
#pragma mark kSIQuantitySpectralRadiantEnergy
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantitySpectralRadiantEnergy, STR("joule per nanometer"), STR("joules per nanometer"), STR("J/nm"), 1.e9);
//
#pragma mark kSIQuantityPower, kSIQuantityRadiantFlux
    AddSpecialSIUnit(kSIQuantityPower, STR("watt"), STR("watts"), STR("W"));
    AddSpecialSIUnit(kSIQuantityPower, STR("Joule per second"), STR("Joules per second"), STR("J/s"));
    AddToLibWithSIPrefixes(kSIQuantityPower, STR("calorie per second"), STR("calories per second"), STR("cal/s"), 4.1868);
    AddToLibWithSIPrefixes(kSIQuantityPower, STR("calorie per minute"), STR("calories per minute"), STR("cal/min"), 4.1868 / 60.);
    AddToLibWithSIPrefixes(kSIQuantityPower, STR("calorie per hour"), STR("calories per hour"), STR("cal/h"), 4.1868 / 3600.);
    AddToLibWithSIPrefixes(kSIQuantityPower, STR("horsepower"), STR("horsepower"), STR("hp"), 745.699872);
    AddToLibWithSIPrefixes(kSIQuantityPower, STR("british thermal unit per hour"), STR("british thermal unit per hour"), STR("Btu/h"), 1055.05585257348 / 3600.);
    AddToLibWithSIPrefixes(kSIQuantityPower, STR("british thermal unit per minute"), STR("british thermal unit per minute"), STR("Btu/min"), 1055.05585257348 / 60);
    AddToLibWithSIPrefixes(kSIQuantityPower, STR("british thermal unit per second"), STR("british thermal unit per second"), STR("Btu/s"), 1055.05585257348);
    AddToLibWithSIPrefixes(kSIQuantityPower, STR("erg per second"), STR("ergs per second"), STR("erg/s"), 1e-7);
    AddToLibWithSIPrefixes(kSIQuantityPower, STR("foot pound force per hour"), STR("feet pound force per hour"), STR("ft•lbf/h"), (1609.344 / 5280) * 4.4482216152605 / 3600.);
    AddToLibWithSIPrefixes(kSIQuantityPower, STR("foot pound force per minute"), STR("feet pound force per minute"), STR("ft•lbf/min"), (1609.344 / 5280) * 4.4482216152605 / 60.);
    AddToLibWithSIPrefixes(kSIQuantityPower, STR("foot pound force per second"), STR("feet pound force per second"), STR("ft•lbf/s"), (1609.344 / 5280) * 4.4482216152605);
    AddToLibWithSIPrefixes(kSIQuantityPower, STR("inch pound force per hour"), STR("inches pound force per hour"), STR("in•lbf/h"), 1.3558179483314 / 12. / 3600.);
    AddToLibWithSIPrefixes(kSIQuantityPower, STR("inch pound force per minute"), STR("inches pound force per minute"), STR("in•lbf/min"), 1.3558179483314 / 12. / 60.);
    AddToLibWithSIPrefixes(kSIQuantityPower, STR("inch pound force per second"), STR("inches pound force per second"), STR("in•lbf/s"), 1.3558179483314 / 12.);
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
    AddToLibWithSIPrefixes(kSIQuantitySpecificPower, STR("horse power per pound"), STR("horse power per pound"), STR("hp/lb"), 1643.986806920936);
    AddToLibWithSIPrefixes(kSIQuantitySpecificPower, STR("horse power per ounce"), STR("horse power per ounce"), STR("hp/oz"), 26303.78891073498);
//
#pragma mark kSIQuantityElectricCharge, kSIQuantityAmountOfElectricity
    AddSpecialSIUnit(kSIQuantityElectricCharge, STR("coulomb"), STR("coulombs"), STR("C"));
    AddToLibWithSIPrefixes(kSIQuantityElectricCharge, STR("elementary charge"), STR("elementary charge"), STR("q_e"), kSIElementaryCharge);
    units = (OCMutableArrayRef)OCDictionaryGetValue(unitsQuantitiesLibrary, kSIQuantityElectricCharge);
    OCDictionaryAddValue(unitsQuantitiesLibrary, kSIQuantityAmountOfElectricity, units);
//
#pragma mark kSIQuantityElectricPotentialDifference, kSIQuantityElectromotiveForce, kSIQuantityVoltage
    AddSpecialSIUnit(kSIQuantityElectricPotentialDifference, STR("volt"), STR("volts"), STR("V"));
    AddToLibWithSIPrefixes(kSIQuantityElectricPotentialDifference, STR("atomic unit of electric potential"), STR("atomic units of electric potential"), STR("E_h/q_e"), E_h / kSIElementaryCharge);
    units = (OCMutableArrayRef)OCDictionaryGetValue(unitsQuantitiesLibrary, kSIQuantityElectricPotentialDifference);
    OCDictionaryAddValue(unitsQuantitiesLibrary, kSIQuantityElectromotiveForce, units);
    OCDictionaryAddValue(unitsQuantitiesLibrary, kSIQuantityVoltage, units);
//
#pragma mark kSIQuantityElectricFieldGradient
    AddSpecialSIUnit(kSIQuantityElectricFieldGradient, STR("volt per square meter"), STR("volts per square meter"), STR("V/m^2"));
    AddToLibWithSIPrefixes(kSIQuantityElectricFieldGradient, STR("atomic unit of electric field gradient"), STR("atomic unit of electric field gradient"), STR("Λ_0"), Λ_0);
    AddToLibWithSIPrefixes(kSIQuantityElectricFieldGradient, STR("atomic unit of electric field gradient"), STR("atomic unit of electric field gradient"), STR("E_h/(q_e•a_0^2)"), Λ_0);
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
    AddToLibWithSIPrefixes(kSIQuantityElectricConductance, STR("conductance quantum"), STR("conductance quantum"), STR("G_0"), G_0);
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
    AddToLibWithSIPrefixes(kSIQuantityMagneticDipoleMoment, STR("nuclear magneton"), STR("nuclear magnetons"), STR("µ_N"), mu_N);
    AddToLibWithSIPrefixes(kSIQuantityMagneticDipoleMoment, STR("bohr magneton"), STR("bohr magnetons"), STR("µ_B"), mu_e);
    AddToLibWithSIPrefixes(kSIQuantityMagneticDipoleMoment, STR("proton magnetic moment"), STR("proton magnetic moment"), STR("µ_p"), kSIProtonMagneticMoment);
    AddToLibWithSIPrefixes(kSIQuantityMagneticDipoleMoment, STR("neutron magnetic moment"), STR("neutron magnetic moment"), STR("µ_n"), kSINeutronMagneticMoment);
    AddToLibWithSIPrefixes(kSIQuantityMagneticDipoleMoment, STR("electron magnetic moment"), STR("electron magnetic moment"), STR("µ_e"), kSIElectronMagneticMoment);
    AddToLibWithSIPrefixes(kSIQuantityMagneticDipoleMoment, STR("muon magnetic moment"), STR("muon magnetic moment"), STR("µ_µ"), kSIMuonMagneticMoment);
    AddToLibWithSIPrefixes(kSIQuantityMagneticDipoleMoment, STR("atomic unit of magnetic dipole moment"), STR("atomic units of magnetic dipole moment"), STR("ℏ•q_e/m_e"), hbar * kSIElementaryCharge / kSIElectronMass);
//
#pragma mark kSIQuantityMagneticDipoleMomentRatio
    AddToLibWithSIPrefixes(kSIQuantityMagneticDipoleMomentRatio, STR("proton g factor"), STR("proton g factor"), STR("g_p"), kSIProtonGFactor);
    AddToLibWithSIPrefixes(kSIQuantityMagneticDipoleMomentRatio, STR("neutron g factor"), STR("neutron g factor"), STR("g_n"), kSINeutronGFactor);
    AddToLibWithSIPrefixes(kSIQuantityMagneticDipoleMomentRatio, STR("electron g factor"), STR("electron g factor"), STR("g_e"), kSIElectronGFactor);
    AddToLibWithSIPrefixes(kSIQuantityMagneticDipoleMomentRatio, STR("muon g factor"), STR("muon g factor"), STR("g_µ"), kSIMuonGFactor);
//
#pragma mark kSIQuantityMagneticFlux
    AddSpecialSIUnit(kSIQuantityMagneticFlux, STR("weber"), STR("webers"), STR("Wb"));
    AddToLibWithSIPrefixes(kSIQuantityMagneticFlux, STR("magnetic flux quantum"), STR("magnetic flux quantum"), STR("Φ_0"), kSIPlanckConstant / (2 * kSIElementaryCharge));
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantityMagneticFlux, STR("maxwell"), STR("maxwells"), STR("Mx"), 1e-8);
//
#pragma mark kSIQuantityMagneticFluxDensity
    AddSpecialSIUnit(kSIQuantityMagneticFluxDensity, STR("tesla"), STR("tesla"), STR("T"));
    AddToLibWithSIPrefixes(kSIQuantityMagneticFluxDensity, STR("atomic unit of magnetic flux density"), STR("atomic units of magnetic flux density"), STR("ℏ/(q_e•a_0^2)"), hbar / (kSIElementaryCharge * a_0 * a_0));
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantityMagneticFluxDensity, STR("gauss"), STR("gauss"), STR("G"), 1e-4);
//
#pragma mark kSIQuantityMagneticFieldGradient
    AddSpecialSIUnit(kSIQuantityMagneticFieldGradient, STR("tesla per meter"), STR("tesla per meter"), STR("T/m"));
    AddToLibWithSIPrefixes(kSIQuantityMagneticFieldGradient, STR("tesla per centimeter"), STR("tesla per centimeter"), STR("T/cm"), 100.);
    AddToLibWithSIPrefixes(kSIQuantityMagneticFieldGradient, STR("gauss per centimeter"), STR("gauss per centimeter"), STR("G/cm"), 0.01);
//
#pragma mark kSIQuantityMolarMagneticSusceptibility
    AddToLibWithSIPrefixes(kSIQuantityMolarMagneticSusceptibility, STR("cubic meter per mole"), STR("cubic meters per mole"), STR("m^3/mol"), 1.);
    AddToLibWithSIPrefixes(kSIQuantityMolarMagneticSusceptibility, STR("cubic centimeter per mole"), STR("cubic centimeters per mole"), STR("cm^3/mol"), 1e-06);
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
    AddToLibWithSIPrefixes(kSIQuantityLuminousFluxDensity, STR("lumen per square foot"), STR("lumens per square foot"), STR("lm/ft^2"), 10.76391041670972);
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
    AddToLibWithSIPrefixes(kSIQuantityRatePerAmountConcentrationPerTime, STR("liter per mole per second"), STR("liter per mole per second"), STR("L/(mol•s)"), 0.001);
//
#pragma mark kSIQuantityRefractiveIndex
    AddSpecialSIUnit(kSIQuantityRefractiveIndex, STR("meter second per meter second"), STR("meter seconds per meter second"), STR("m•s/(m•s)"));
//
#pragma mark kSIQuantityElectricQuadrupoleMoment
    AddToLibWithSIPrefixes(kSIQuantityElectricQuadrupoleMoment, STR("atomic unit of electric quadrupole moment"), STR("atomic units of electric quadrupole moment"), STR("q_e•a_0^2"), kSIElementaryCharge * a_0 * a_0);
//
#pragma mark kSIQuantityMagnetizability
    AddToLibWithSIPrefixes(kSIQuantityMagnetizability, STR("atomic unit of magnetizability"), STR("atomic units of magnetizability"), STR("q_e•a_0^2/m_e"), kSIElementaryCharge * a_0 * a_0 / kSIElectronMass);
//
#pragma mark kSIQuantityPermittivity
    AddToLibWithSIPrefixes(kSIQuantityPermittivity, STR("atomic unit of permittivity"), STR("atomic units of permittivity"), STR("q_e^2/(a_0•E_h)"), kSIElementaryCharge * kSIElementaryCharge / (a_0 * E_h));
    AddSpecialSIUnit(kSIQuantityPermittivity, STR("farad per meter"), STR("farads per meter"), STR("F/m"));
    AddSpecialSIUnit(kSIQuantityPermittivity, STR("coulomb per volt meter"), STR("coulombs per volt meter"), STR("C/(V•m)"));
    AddToLibWithSIPrefixes(kSIQuantityPermittivity, STR("electric constant"), STR("electric constant"), STR("ε_0"), kSIElectricConstant);
//
#pragma mark kSIQuantityElectricPolarizability
    AddToLibWithSIPrefixes(kSIQuantityElectricPolarizability, STR("atomic unit of electric polarizability"), STR("atomic units of electric polarizability"), STR("q_e^2•a_0^2/E_h"), kSIElementaryCharge * kSIElementaryCharge * a_0 * a_0 / (E_h));
//
#pragma mark kSIQuantityFirstHyperPolarizability
    AddToLibWithSIPrefixes(kSIQuantityFirstHyperPolarizability, STR("atomic unit of 1st polarizability"), STR("atomic units of 1st polarizability"), STR("q_e^3•a_0^3/E_h^2"), kSIElementaryCharge * kSIElementaryCharge * kSIElementaryCharge * a_0 * a_0 * a_0 / (E_h * E_h));
//
#pragma mark kSIQuantitySecondHyperPolarizability
    AddToLibWithSIPrefixes(kSIQuantitySecondHyperPolarizability, STR("atomic unit of 2nd polarizability"), STR("atomic units of 2nd polarizability"), STR("q_e^4•a_0^4/E_h^3"), kSIElementaryCharge * kSIElementaryCharge * kSIElementaryCharge * kSIElementaryCharge * a_0 * a_0 * a_0 * a_0 / (E_h * E_h * E_h));
//
#pragma mark kSIQuantitySpecificVolume
    AddToLibWithSIPrefixes(kSIQuantitySpecificVolume, STR("cubic meter per kilogram"), STR("cubic meters per kilogram"), STR("m^3/kg"), 1.);
//
#pragma mark kSIQuantityMassToChargeRatio
    AddToLibWithSIPrefixes(kSIQuantityMassToChargeRatio, STR("thomson"), STR("thomson"), STR("Th"), kSIAtomicMassConstant / kSIElementaryCharge);
//
#pragma mark kSIQuantityChargeToMassRatio
    AddToLibWithSIPrefixes(kSIQuantityChargeToMassRatio, STR("inverse thomson"), STR("inverse thomson"), STR("(1/Th)"), kSIElementaryCharge / kSIAtomicMassConstant);
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
    AddToLibWithSIPrefixes(kSIQuantityDiffusionCoefficient, STR("square meter per second"), STR("square meters per second"), STR("m^2/s"), 1);
    AddToLibWithSIPrefixes(kSIQuantityDiffusionCoefficient, STR("square centimeter per second"), STR("square centimeters per second"), STR("cm^2/s"), 0.0001);
    AddToLibWithSIPrefixes(kSIQuantityDiffusionCoefficient, STR("square millimeter per second"), STR("square millimeters per second"), STR("mm^2/s"), 1e-6);
//
#pragma mark kSIQuantityLuminance
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantityLuminance, STR("stilb"), STR("stilbs"), STR("sb"), 1e4);
    AddSpecialSIUnit(kSIQuantityLuminance, STR("lumen per square meter per steradian"), STR("lumens per square meter per steradian"), STR("lm/(m^2•sr)"));
//
#pragma mark kSIQuantityInverseMagneticFluxDensity
    AddToLibWithSIPrefixes(kSIQuantityInverseMagneticFluxDensity, STR("inverse gauss"), STR("inverse gauss"), STR("(1/G)"), 1.);
//
#pragma mark kSIQuantityMagneticFieldStrength
    AddSpecialSIUnit(kSIQuantityMagneticFieldStrength, STR("ampere per meter"), STR("amperes per meter"), STR("A/m"));
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantityMagneticFieldStrength, STR("œrsted"), STR("œrsteds"), STR("Oe"), 79.577471545947674);
//
#pragma mark kSIQuantityMomentOfForce
    AddSpecialSIUnit(kSIQuantityMomentOfForce, STR("newton meter"), STR("newton meters"), STR("N•m"));
    AddToLibWithSIPrefixes(kSIQuantityMomentOfForce, STR("foot pound force"), STR("feet pound force"), STR("ft•lbf"), 1.3558179483314);
    AddToLibWithSIPrefixes(kSIQuantityMomentOfForce, STR("inch pound force"), STR("inch pound force"), STR("in•lbf"), 1.3558179483314 / 12.);
    AddToLibWithSIPrefixes(kSIQuantityMomentOfForce, STR("inch ounce force"), STR("inch ounce force"), STR("in•ozf"), 1.3558179483314 / 12. / 16.);
    AddToLibWithSIPrefixes(kSIQuantityMomentOfForce, STR("pound force foot"), STR("pound force feet"), STR("lbf•ft"), 1.3558179483314);
    AddToLibWithSIPrefixes(kSIQuantityMomentOfForce, STR("pound force inch"), STR("pound force inches"), STR("lbf•in"), 1.3558179483314 / 12.);
    AddToLibWithSIPrefixes(kSIQuantityMomentOfForce, STR("ounce force inch"), STR("ounce force inches"), STR("ozf•in"), 1.3558179483314 / 12. / 16.);
//
#pragma mark kSIQuantitySurfaceTension
    AddSpecialSIUnit(kSIQuantitySurfaceTension, STR("newton per meter"), STR("newtons per meter"), STR("N/m"));
    AddToLibWithSIPrefixes(kSIQuantitySurfaceTension, STR("dyne per centimeter"), STR("dynes per centimeter"), STR("dyn/cm"), 0.001);
//
#pragma mark kSIQuantitySurfaceEnergy
    AddSpecialSIUnit(kSIQuantitySurfaceEnergy, STR("joule per square meter"), STR("joules per square meter"), STR("J/m^2"));
    AddToLibWithSIPrefixes(kSIQuantitySurfaceEnergy, STR("dyne per square centimeter"), STR("dynes per square centimeter"), STR("dyn/cm^2"), 0.1);
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
    AddToLibWithSIPrefixes(kSIQuantityHeatFluxDensity, STR("watt per square foot"), STR("watts per square foot"), STR("W/ft^2"), 10.76391041670972);
    AddToLibWithSIPrefixes(kSIQuantityHeatFluxDensity, STR("watt per square inch"), STR("watts per square inch"), STR("W/in^2"), 10.76391041670972 / 12.);
    units = (OCMutableArrayRef)OCDictionaryGetValue(unitsQuantitiesLibrary, kSIQuantityHeatFluxDensity);
    OCDictionaryAddValue(unitsQuantitiesLibrary, kSIQuantityIrradiance, units);
//
#pragma mark kSIQuantitySpectralRadiantFluxDensity
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantitySpectralRadiantFluxDensity, STR("watt per square meter per nanometer"), STR("watts per square meter per nanometer"), STR("W/(m^2•nm)"), 1.e9);
//
#pragma mark kSIQuantityEntropy, kSIQuantityHeatCapacity
    AddSpecialSIUnit(kSIQuantityEntropy, STR("joule per kelvin"), STR("joules per kelvin"), STR("J/K"));
    AddToLibWithSIPrefixes(kSIQuantityEntropy, STR("boltzmann constant"), STR("boltzmann constant"), STR("k_B"), kSIBoltmannConstant);
    units = (OCMutableArrayRef)OCDictionaryGetValue(unitsQuantitiesLibrary, kSIQuantityEntropy);
    OCDictionaryAddValue(unitsQuantitiesLibrary, kSIQuantityHeatCapacity, units);
//
#pragma mark kSIQuantitySpecificHeatCapacity, kSIQuantitySpecificEntropy
    AddSpecialSIUnit(kSIQuantitySpecificHeatCapacity, STR("joule per kilogram kelvin"), STR("joules per kilogram kelvin"), STR("J/(kg•K)"));
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantitySpecificHeatCapacity, STR("joule per gram kelvin"), STR("joules per gram kelvin"), STR("J/(g•K)"), 1000.);
    AddToLibWithSIPrefixes(kSIQuantitySpecificHeatCapacity, STR("calorie per gram per kelvin"), STR("calories per gram per kelvin"), STR("cal/(g•K)"), 4186.8);
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
    AddToLibWithSIPrefixes(kSIQuantityThermalConductance, STR("Btu per hour per rankine"), STR("Btus per hour per rankine"), STR("Btu/(h•°R)"), 0.5275279262867396);
    AddToLibWithSIPrefixes(kSIQuantityThermalConductance, STR("calorie per hour per kelvin"), STR("calories per hour per kelvin"), STR("cal/(h•K)"), 1.163e-3);
    AddToLibWithSIPrefixes(kSIQuantityThermalConductance, STR("kilocalorie per hour per kelvin"), STR("kilocalories per hour per kelvin"), STR("kcal/(h•K)"), 1.163);
//
#pragma mark kSIQuantityThermalConductivity
    AddSpecialSIUnit(kSIQuantityThermalConductivity, STR("watt per meter kelvin"), STR("watts per meter kelvin"), STR("W/(m•K)"));
    AddToLibWithSIPrefixes(kSIQuantityThermalConductivity, STR("Btu per hour per foot per rankine"), STR("Btus per hour per foot per rankine"), STR("Btu/(h•ft•°R)"), 1.730734666295077);
    AddToLibWithSIPrefixes(kSIQuantityThermalConductivity, STR("calorie per hour per meter per kelvin"), STR("calories per hour per meter per kelvin"), STR("cal/(h•m•K)"), 1.163e-3);
    AddToLibWithSIPrefixes(kSIQuantityThermalConductivity, STR("kilocalorie per hour per meter per kelvin"), STR("kilocalories per hour per meter per kelvin"), STR("kcal/(h•m•K)"), 1.163);
//
#pragma mark kSIQuantityEnergyDensity
    AddSpecialSIUnit(kSIQuantityEnergyDensity, STR("joule per cubic meter"), STR("joules per cubic meter"), STR("J/m^3"));
    AddNonSIUnitWithPrefixesToLibrary(kSIQuantityEnergyDensity, STR("joule per liter"), STR("joules per liter"), STR("J/L"), 1000.);
//
#pragma mark kSIQuantityElectricDipoleMoment
    AddSpecialSIUnit(kSIQuantityElectricDipoleMoment, STR("coulomb meter"), STR("coulomb meters"), STR("C•m"));
    AddToLibWithSIPrefixes(kSIQuantityElectricDipoleMoment, STR("debye"), STR("debyes"), STR("D"), 3.335640951816991e-30);
    AddToLibWithSIPrefixes(kSIQuantityElectricDipoleMoment, STR("atomic unit of electric dipole moment"), STR("atomic unit of electric dipole moment"), STR("q_e•a_0"), kSIElementaryCharge * a_0);
//
#pragma mark kSIQuantityElectricFieldStrength
    AddSpecialSIUnit(kSIQuantityElectricFieldStrength, STR("volt per meter"), STR("volts per meter"), STR("V/m"));
    AddToLibWithSIPrefixes(kSIQuantityElectricFieldStrength, STR("atomic unit of electric field"), STR("atomic unit of electric field"), STR("E_h/(q_e•a_0)"), E_h / (kSIElementaryCharge * a_0));
    AddSpecialSIUnit(kSIQuantityElectricFieldStrength, STR("volt per centimeter"), STR("volts per centimeter"), STR("V/cm"));
    AddSpecialSIUnit(kSIQuantityElectricFieldStrength, STR("newton per coulomb"), STR("newtons per coulomb"), STR("N/C"));
//
#pragma mark kSIQuantityElectricFlux
    AddSpecialSIUnit(kSIQuantityElectricFlux, STR("volt meter"), STR("volts meter"), STR("V•m"));
//
#pragma mark kSIQuantityElectricChargeDensity
    AddSpecialSIUnit(kSIQuantityElectricChargeDensity, STR("coulomb per cubic meter"), STR("coulombs per cubic meter"), STR("C/m^3"));
    AddToLibWithSIPrefixes(kSIQuantityElectricChargeDensity, STR("charge density"), STR("charge density"), STR("A•h/L"), 3600000);
    AddToLibWithSIPrefixes(kSIQuantityElectricChargeDensity, STR("atomic unit of charge density"), STR("atomic unit of charge density"), STR("q_e/a_0^3"), kSIElementaryCharge / (a_0 * a_0 * a_0));
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
    AddToLibWithSIPrefixes(kSIQuantityPermeability, STR("magnetic constant"), STR("magnetic constant"), STR("µ_0"), 4 * kSIPi * 1.e-7);
//
#pragma mark kSIQuantityMolarEntropy, kSIQuantityMolarHeatCapacity
    AddSpecialSIUnit(kSIQuantityMolarEntropy, STR("joule per mole kelvin"), STR("joules per mole kelvin"), STR("J/(mol•K)"));
    AddToLibWithSIPrefixes(kSIQuantityMolarEntropy, STR("gas constant"), STR("gas constant"), STR("R"), kSIBoltmannConstant * kSIAvogadroConstant);
    units = (OCMutableArrayRef)OCDictionaryGetValue(unitsQuantitiesLibrary, kSIQuantityMolarEntropy);
    OCDictionaryAddValue(unitsQuantitiesLibrary, kSIQuantityMolarHeatCapacity, units);
//
#pragma mark kSIQuantityMolarEnergy
    AddSpecialSIUnit(kSIQuantityMolarEnergy, STR("joule per mole"), STR("joules per mole"), STR("J/mol"));
    AddToLibWithSIPrefixes(kSIQuantityMolarEnergy, STR("calorie per mole"), STR("calories per mole"), STR("cal/mol"), 4.1868);
    AddToLibWithSIPrefixes(kSIQuantityMolarEnergy, STR("kilocalorie per mole"), STR("kilocalories per mole"), STR("kcal/mol"), 4.1868 * 1000.);
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
    AddToLibWithSIPrefixes(kSIQuantityHeatTransferCoefficient, STR("Btu per hour per square foot per rankine"), STR("Btus per hour per square foot per rankine"), STR("Btu/(h•ft^2•°R)"), 5.678263340863113);
    AddToLibWithSIPrefixes(kSIQuantityHeatTransferCoefficient, STR("calorie per hour per square meter per kelvin"), STR("calories per hour per square meter per kelvin"), STR("cal/(h•m^2•K)"), 1.163e-3);
    AddToLibWithSIPrefixes(kSIQuantityHeatTransferCoefficient, STR("kilocalorie per hour per square meter per kelvin"), STR("kilocalories per hour per square meter per kelvin"), STR("kcal/(h•m^2•K)"), 1.163);
//
#pragma mark kSIQuantityChargePerAmount
    AddToLibWithSIPrefixes(kSIQuantityChargePerAmount, STR("faraday constant"), STR("faraday constant"), STR("&F"), kSIElementaryCharge * kSIAvogadroConstant);
    AddToLibWithSIPrefixes(kSIQuantityChargePerAmount, STR("coulomb per mole"), STR("coulombs per mole"), STR("C/mol"), 1.0);
//
#pragma mark kSIQuantityGravitationalConstant
    AddToLibWithSIPrefixes(kSIQuantityGravitationalConstant, STR("gravitational constant"), STR("gravitational constant"), STR("&G"), kSIGravitationalConstant);
//
#pragma mark kSIQuantityVolumePerLength
    AddToLibWithSIPrefixes(kSIQuantityVolumePerLength, STR("liter per 100 kilometers"), STR("liters per 100 kilometers"), STR("L/(100 km)"), 1e-3 / 100000.);
//
#pragma mark kSIQuantityVolumetricFlowRate
    AddToLibWithSIPrefixes(kSIQuantityVolumetricFlowRate, STR("cubic meter per hour"), STR("cubic meters per hour"), STR("m^3/h"), 1. / 3600.);
    AddToLibWithSIPrefixes(kSIQuantityVolumetricFlowRate, STR("cubic meter per minute"), STR("cubic meters per minute"), STR("m^3/min"), 1. / 60.);
    AddToLibWithSIPrefixes(kSIQuantityVolumetricFlowRate, STR("cubic meter per second"), STR("cubic meters per second"), STR("m^3/s"), 1.);
    AddToLibWithSIPrefixes(kSIQuantityVolumetricFlowRate, STR("cubic centimeter per hour"), STR("cubic centimeters per hour"), STR("cm^3/h"), 1e-6 / 3600.);
    AddToLibWithSIPrefixes(kSIQuantityVolumetricFlowRate, STR("cubic centimeter per minute"), STR("cubic centimeters per minute"), STR("cm^3/min"), 1e-6 / 60.);
    AddToLibWithSIPrefixes(kSIQuantityVolumetricFlowRate, STR("cubic centimeter per second"), STR("cubic centimeters per second"), STR("cm^3/s"), 1e-6);
    AddToLibWithSIPrefixes(kSIQuantityVolumetricFlowRate, STR("cubic foot per hour"), STR("cubic feet per hour"), STR("ft^3/h"), (1609.344 / 5280) * (1609.344 / 5280) * (1609.344 / 5280) / 3600.);
    AddToLibWithSIPrefixes(kSIQuantityVolumetricFlowRate, STR("cubic foot per minute"), STR("cubic feet per minute"), STR("ft^3/min"), (1609.344 / 5280) * (1609.344 / 5280) * (1609.344 / 5280) / 60.);
    AddToLibWithSIPrefixes(kSIQuantityVolumetricFlowRate, STR("cubic foot per second"), STR("cubic feet per second"), STR("ft^3/s"), (1609.344 / 5280) * (1609.344 / 5280) * (1609.344 / 5280));
//
#pragma mark kSIQuantityPowerPerAreaPerTemperatureToFourthPower
    AddToLibWithSIPrefixes(kSIQuantityPowerPerAreaPerTemperatureToFourthPower, STR("stefan-boltzmann constant"), STR("stefan-boltzmann constant"), STR("σ"), kSIStefanBoltzmannConstant);
//
#pragma mark kSIQuantityWavelengthDisplacementConstant
    AddToLibWithSIPrefixes(kSIQuantityWavelengthDisplacementConstant, STR("wien wavelength displacement constant"), STR("wien wavelength displacement constant"), STR("b_λ"), kSIWeinDisplacementConstant);
//
#pragma mark kSIQuantityGasPermeance
    AddToLibWithSIPrefixes(kSIQuantityGasPermeance, STR("gas permeance unit"), STR("gas permeance unit"), STR("GPU"), 3.35e-10);
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
