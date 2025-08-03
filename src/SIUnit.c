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
#include "SITypes.h"
/**
 * @file SIUnit.c
 * @brief Implementation of the SIUnit type and related functionality.
 *
 * This file contains the definition of the SIUnit type, along with functions
 * for creating, manipulating, and destroying SIUnit instances.
 *
 * @section SI Unit Prefixes
 *
 * The SIUnit system uses a set of prefixes in front of the root unit symbol to
 * indicate a power of ten multiplier, e.g., k (kilo) for 10^3, m (milli) for 10^-3,
 * etc.  Only SI Units, the SI Special Units, and a few Non-SI units are allowed to
 * have SI prefixes.  These are detailed below.
 *
 * @section Units
 *
 * The SIUnit system recognizes several categories of units:
 *
 * @subsection Coherent SI Base Units
 * A set of seven base units, given by the symbols: m, kg, s, A, K, mol, cd
 *
 * @subsection Coherent Derived SI Units
 * Units derived from the Coherent SI Base Units, such as m/s, kg·m/s², etc.
 * These units are coherent because they are expressed entirely in terms of the
 * Coherent SI Base Units, i.e., multiplied or divided. For each dimensionality,
 * there is only one coherent derived unit, formed from the Coherent SI Base Units,
 * such as kg•m/s, etc.  Every SIUnit has a scale_to_coherent_si value, which is the
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
 * @section SIUnit Implementation Approach
 *
 * In defining the SIUnit type, we use the following approach:
 *
 * Each SIUnit has a dimensionality, which is described by a SIDimensionalityRef.
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
// SIUnit Opaque Type
struct impl_SIUnit {
    OCBase base;
    // SIUnit Type attributes
    SIDimensionalityRef dimensionality;  // required: dimensionality of the unit
    double scale_to_coherent_si;         // required: Scale factor to convert from this unit to its coherent SI unit
    OCStringRef symbol;                  // required: Symbol of the unit, e.g., "m", "kg", "s", etc.
    OCStringRef name;                    // optional: name of the unit, e.g., "meter", "gram", "second", etc.
    OCStringRef plural_name;             // optional: Plural name of the unit, e.g., "meters", "grams", "seconds", etc.
    // Boolean flags packed into a single byte
    struct {
        unsigned int isSIUnit : 1;        // Unit belongs to the SI system
        unsigned int isCGSUnit : 1;       // Unit belongs to the CGS system
        unsigned int isImperialUnit : 1;  // Unit belongs to the Imperial/British system
        unsigned int isAtomicUnit : 1;    // Unit belongs to the Atomic units system
        unsigned int isPlanckUnit : 1;    // Unit belongs to the Planck units system
        unsigned int reserved6 : 1;       // Reserved for future use
        unsigned int reserved7 : 1;       // Reserved for future use
        unsigned int isConstant : 1;      // Unit represents a physical constant
    } flags;
};
static struct impl_SIUnit *SIUnitAllocate();
static OCTypeID kSIUnitID = kOCNotATypeID;
OCTypeID SIUnitGetTypeID(void) {
    if (kSIUnitID == kOCNotATypeID)
        kSIUnitID = OCRegisterType("SIUnit");
    return kSIUnitID;
}
bool impl_SIUnitEqual(const void *theType1, const void *theType2) {
    if (!theType1 || !theType2) return false;
    if (theType1 == theType2) return true;
    if (OCGetTypeID(theType1) != OCGetTypeID(theType2)) return false;
    SIUnitRef u1 = (SIUnitRef)theType1;
    SIUnitRef u2 = (SIUnitRef)theType2;
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
void impl_SIUnitFinalize(const void *theType) {
    if (NULL == theType) return;
    SIUnitRef theUnit = (SIUnitRef)theType;
    if (theUnit->dimensionality) OCRelease(theUnit->dimensionality);
    if (theUnit->symbol) OCRelease(theUnit->symbol);
    if (theUnit->name) OCRelease(theUnit->name);
    if (theUnit->plural_name) OCRelease(theUnit->plural_name);
}
static OCStringRef impl_SIUnitCopyFormattingDescription(OCTypeRef theType) {
    if (!theType) return NULL;
    SIUnitRef theUnit = (SIUnitRef)theType;
    if (theUnit->symbol) return OCStringCreateCopy(theUnit->symbol);
    return OCStringCreateWithCString("<SIUnit>");
}
static void *impl_SIUnitDeepCopy(const void *obj) {
    if (!obj) return NULL;
    const SIUnitRef src = (SIUnitRef)obj;
    struct impl_SIUnit *copy = SIUnitAllocate();
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
static void *impl_SIUnitDeepCopyMutable(const void *obj) {
    // SIUnit is immutable; just return a standard deep copy
    return impl_SIUnitDeepCopy(obj);
}
static cJSON *impl_SIUnitCopyJSON(const void *obj) {
    if (!obj) return cJSON_CreateNull();
    cJSON *json = cJSON_CreateObject();
    const SIUnitRef unit = (SIUnitRef)obj;
    cJSON_AddItemToObject(json, "dimensionality", SIDimensionalityCreateJSON(unit->dimensionality));
    cJSON_AddNumberToObject(json, "scale_to_coherent_si", unit->scale_to_coherent_si);
    if (unit->name)
        cJSON_AddStringToObject(json, "name", OCStringGetCString(unit->name));
    else
        cJSON_AddNullToObject(json, "name");
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
static struct impl_SIUnit *SIUnitAllocate() {
    return OCTypeAlloc(struct impl_SIUnit,
                       SIUnitGetTypeID(),
                       impl_SIUnitFinalize,
                       impl_SIUnitEqual,
                       impl_SIUnitCopyFormattingDescription,
                       impl_SIUnitCopyJSON,
                       impl_SIUnitDeepCopy,
                       impl_SIUnitDeepCopyMutable);
}
static SIUnitRef SIUnitCreate(SIDimensionalityRef dimensionality,
                              OCStringRef name,
                              OCStringRef plural_name,
                              OCStringRef symbol,
                              double scale_to_coherent_si) {
    struct impl_SIUnit *theUnit = SIUnitAllocate();
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
    // Initialize all flags to false/0
    memset(&theUnit->flags, 0, sizeof(theUnit->flags));
    return (SIUnitRef)theUnit;
}
// Accessor functions for SIUnit
SIDimensionalityRef SIUnitGetDimensionality(SIUnitRef theUnit) {
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, NULL);
    return theUnit->dimensionality;
}
OCStringRef SIUnitCopySymbol(SIUnitRef theUnit) {
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, NULL);
    return OCStringCreateCopy(theUnit->symbol);
}
OCStringRef SIUnitCopyName(SIUnitRef theUnit) {
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, NULL);
    return OCStringCreateCopy(theUnit->name);
}
OCStringRef SIUnitCopyPluralName(SIUnitRef theUnit) {
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, NULL);
    return OCStringCreateCopy(theUnit->plural_name);
}
double SIUnitGetScaleToCoherentSI(SIUnitRef theUnit) {
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, 0);
    return theUnit->scale_to_coherent_si;
}
// Boolean property getters
bool SIUnitIsSIUnit(SIUnitRef theUnit) {
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, false);
    return theUnit->flags.isSIUnit;
}
bool SIUnitIsCGSUnit(SIUnitRef theUnit) {
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, false);
    return theUnit->flags.isCGSUnit;
}
bool SIUnitIsImperialUnit(SIUnitRef theUnit) {
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, false);
    return theUnit->flags.isImperialUnit;
}
bool SIUnitIsAtomicUnit(SIUnitRef theUnit) {
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, false);
    return theUnit->flags.isAtomicUnit;
}
bool SIUnitIsPlanckUnit(SIUnitRef theUnit) {
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, false);
    return theUnit->flags.isPlanckUnit;
}
bool SIUnitIsConstant(SIUnitRef theUnit) {
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, false);
    return theUnit->flags.isConstant;
}
// Boolean property setters (for internal use)
void SIUnitSetIsSIUnit(SIUnitRef theUnit, bool value) {
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, );
    ((struct impl_SIUnit *)theUnit)->flags.isSIUnit = value ? 1 : 0;
}
void SIUnitSetIsCGSUnit(SIUnitRef theUnit, bool value) {
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, );
    ((struct impl_SIUnit *)theUnit)->flags.isCGSUnit = value ? 1 : 0;
}
void SIUnitSetIsImperialUnit(SIUnitRef theUnit, bool value) {
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, );
    ((struct impl_SIUnit *)theUnit)->flags.isImperialUnit = value ? 1 : 0;
}
void SIUnitSetIsAtomicUnit(SIUnitRef theUnit, bool value) {
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, );
    ((struct impl_SIUnit *)theUnit)->flags.isAtomicUnit = value ? 1 : 0;
}
void SIUnitSetIsPlanckUnit(SIUnitRef theUnit, bool value) {
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, );
    ((struct impl_SIUnit *)theUnit)->flags.isPlanckUnit = value ? 1 : 0;
}
void SIUnitSetIsConstant(SIUnitRef theUnit, bool value) {
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, );
    ((struct impl_SIUnit *)theUnit)->flags.isConstant = value ? 1 : 0;
}
// Elimate this function after SIUnit refactor
double SIUnitScaleToCoherentSIUnit(SIUnitRef theUnit) {
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, 0);
    return theUnit->scale_to_coherent_si;
}
bool SIUnitEqual(SIUnitRef theUnit1, SIUnitRef theUnit2) {
    return impl_SIUnitEqual(theUnit1, theUnit2);
}
double SIUnitConversion(SIUnitRef initialUnit, SIUnitRef finalUnit) {
    IF_NO_OBJECT_EXISTS_RETURN(initialUnit, 0);
    IF_NO_OBJECT_EXISTS_RETURN(finalUnit, 0);
    if (SIDimensionalityHasSameReducedDimensionality(initialUnit->dimensionality, finalUnit->dimensionality))
        return initialUnit->scale_to_coherent_si / finalUnit->scale_to_coherent_si;
    return 0;
}
bool SIUnitAreEquivalentUnits(SIUnitRef theUnit1, SIUnitRef theUnit2) {
    IF_NO_OBJECT_EXISTS_RETURN(theUnit1, false);
    IF_NO_OBJECT_EXISTS_RETURN(theUnit2, false);
    if (theUnit1 == theUnit2) return true;
    if (!SIDimensionalityEqual(theUnit1->dimensionality, theUnit2->dimensionality)) return false;
    if (OCCompareDoubleValues(theUnit1->scale_to_coherent_si, theUnit2->scale_to_coherent_si) != kOCCompareEqualTo)
        return false;
    return true;
}
// Common helper function for creating and simplifying symbols
static OCStringRef SIUnitCreateSimplifiedSymbol(OCStringRef raw_symbol, bool reduce, OCStringRef *error) {
    OCStringRef simplified_symbol;
    if (reduce) {
        simplified_symbol = SIUnitCreateCleanedAndReducedExpression(raw_symbol);
    } else {
        simplified_symbol = SIUnitCreateCleanedExpression(raw_symbol);
    }
    if (!simplified_symbol) {
        simplified_symbol = raw_symbol;  // Fallback to raw symbol
        OCRetain(simplified_symbol);     // Balance the retain for cleanup
    }
    return simplified_symbol;
}
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
    // 4) optional name / plural name
    if (unit->name)
        cJSON_AddStringToObject(json,
                                "name",
                                OCStringGetCString(unit->name));
    else
        cJSON_AddNullToObject(json, "name");
    if (unit->plural_name)
        cJSON_AddStringToObject(json,
                                "plural_name",
                                OCStringGetCString(unit->plural_name));
    else
        cJSON_AddNullToObject(json, "plural_name");
    if (unit->symbol)
        cJSON_AddStringToObject(json,
                                "symbol",
                                OCStringGetCString(unit->symbol));
    else
        cJSON_AddNullToObject(json, "symbol");
    return json;
}
//
static OCMutableArrayRef unitsArrayLibrary = NULL;
static OCMutableDictionaryRef unitsDictionaryLibrary = NULL;
static OCMutableDictionaryRef unitsQuantitiesLibrary = NULL;
static OCMutableDictionaryRef unitsDimensionalitiesLibrary = NULL;
static OCMutableArrayRef tokenSymbolLibrary = NULL;
static bool imperialVolumes = false;
// Function prototypes
static bool SIUnitCreateLibraries(void);
static bool SIUnitAddUSPlainVolumeUnits(OCStringRef *error);
static bool SIUnitAddUKPlainVolumeUnits(OCStringRef *error);
static bool SIUnitAddUKLabeledVolumeUnits(OCStringRef *error);
static bool SIUnitLibraryAddUSLabeledVolumeUnits(OCStringRef *error);
// Library accessor functions
OCMutableDictionaryRef SIUnitGetUnitsDictionaryLib(void) {
    if (NULL == unitsDictionaryLibrary) SIUnitCreateLibraries();
    return unitsDictionaryLibrary;
}
static OCMutableDictionaryRef SIUnitGetQuantitiesLib(void) {
    if (NULL == unitsQuantitiesLibrary) SIUnitCreateLibraries();
    return unitsQuantitiesLibrary;
}
static OCMutableDictionaryRef SIUnitGetDimensionalitiesLib(void) {
    if (NULL == unitsDimensionalitiesLibrary) SIUnitCreateLibraries();
    return unitsDimensionalitiesLibrary;
}
static OCMutableArrayRef SIUnitGetUnitsArrayLib(void) {
    if (NULL == unitsArrayLibrary) SIUnitCreateLibraries();
    return unitsArrayLibrary;
}
// Helper function to check if a symbol is underived (contains no operators)
static bool SIUnitSymbolIsUnderived(OCStringRef symbol) {
    if (!symbol) return false;
    const char *cstr = OCStringGetCString(symbol);
    if (!cstr) return false;
    // Check for mathematical operators that indicate derived units
    for (const char *p = cstr; *p; p++) {
        if (*p == '^' || *p == '*' || *p == '/' || *p == '(' || *p == ')') {
            return false;
        }
    }
    return true;
}
static void AddToUnitsDictionaryLibrary(SIUnitRef unit) {
    if (!SIUnitSymbolIsUnderived(unit->symbol)) {
        OCStringRef key = SIUnitCreateCleanedExpression(unit->symbol);
        OCDictionaryAddValue(unitsDictionaryLibrary, key, unit);
        OCRelease(key);
    } else
        OCDictionaryAddValue(unitsDictionaryLibrary, unit->symbol, unit);
}
// Helper function to register a unit in all the appropriate libraries
// After calling this function you must add the unit into
// the unitsDictionaryLibrary dictionary using AddToUnitsDictionaryLibrary
static SIUnitRef RegisterUnitInLibraries(SIUnitRef theUnit,
    OCStringRef quantity,
    SIDimensionalityRef dimensionality) {
    // First check if unit is already registered.
    if (OCArrayContainsValue(unitsArrayLibrary, theUnit)) return theUnit;
    OCTypeSetStaticInstance(theUnit, true);
    OCArrayAppendValue(unitsArrayLibrary, theUnit);
    // If unit symbol is underived, i.e., one of the token unit symbols, add to tokenSymbolLibrary
    if (SIUnitSymbolIsUnderived(theUnit->symbol)) {
        if (!OCArrayContainsValue(tokenSymbolLibrary, theUnit->symbol)) {
            OCStringRef symbol_copy = OCStringCreateCopy(theUnit->symbol);
            OCArrayAppendValue(tokenSymbolLibrary, symbol_copy);
            OCRelease(symbol_copy);
        }
    }
    // Append unit to mutable array value associated with dimensionality key inside dimensionality library dictionary
    OCStringRef dimensionalitySymbol = SIDimensionalityCopySymbol(dimensionality);
    {
        OCMutableDictionaryRef unitsDimensionalitiesLib = SIUnitGetDimensionalitiesLib();
        OCMutableArrayRef units = (OCMutableArrayRef)OCDictionaryGetValue(unitsDimensionalitiesLib, dimensionalitySymbol);
        if (units) OCArrayAppendValue(units, theUnit);
        else {
            units = OCArrayCreateMutable(0, &kOCTypeArrayCallBacks);
            OCArrayAppendValue(units, theUnit);
            OCDictionaryAddValue(unitsDimensionalitiesLib, dimensionalitySymbol, units);
            OCRelease(units);
        }
    }
    // Append unit to mutable array value associated with quantity key inside quantity library dictionary
    {
        OCMutableDictionaryRef unitsQuantitiesLib = SIUnitGetQuantitiesLib();
        OCMutableArrayRef units = (OCMutableArrayRef)OCDictionaryGetValue(unitsQuantitiesLib, quantity);
        if (units)
            OCArrayAppendValue(units, theUnit);
        else {
            units = OCArrayCreateMutable(0, &kOCTypeArrayCallBacks);
            OCArrayAppendValue(units, theUnit);
            if(quantity) OCDictionaryAddValue(unitsQuantitiesLib, quantity, units);
            else OCDictionaryAddValue(unitsQuantitiesLib, dimensionalitySymbol, units);
            OCRelease(units);
        }
    }
    OCRelease(dimensionalitySymbol);
    return theUnit;
}
OCMutableArrayRef SIUnitGetTokenSymbolsLib(void) {
    if (NULL == tokenSymbolLibrary) SIUnitCreateLibraries();
    return tokenSymbolLibrary;
}
static SIUnitRef AddToLib(
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
    SIUnitRef theUnit = SIUnitCreate(dimensionality, name, plural_name, symbol, scale_to_coherent_si);
    if (NULL == theUnit) {
        if (error && *error == NULL) {
            *error = STR("ERROR: Could not create SIUnit");
        }
        return NULL;
    }
    // Check and Register the unit in all appropriate libraries
    return RegisterUnitInLibraries(theUnit, quantity, dimensionality);
}
SIUnitRef AddNonExistingToLib(
    OCStringRef quantity,
    OCStringRef name,
    OCStringRef plural_name,
    OCStringRef symbol,
    double scale_to_coherent_si,
    OCStringRef *error) {
    SIUnitRef unit = AddToLib(quantity, name, plural_name, symbol, scale_to_coherent_si, error);
    OCDictionaryAddValue(SIUnitGetUnitsDictionaryLib(), unit->symbol, unit);
    return unit;
}
static SIUnitRef AddSIToLib(
    OCStringRef quantity,
    OCStringRef name,
    OCStringRef plural_name,
    OCStringRef symbol,
    double scale_to_coherent_si,
    OCStringRef *error) {
    SIUnitRef unit = AddToLib(quantity, name, plural_name, symbol, scale_to_coherent_si, error);
    SIUnitSetIsSIUnit(unit, true);
    return unit;
}
static SIUnitRef AddCGSToLib(
    OCStringRef quantity,
    OCStringRef name,
    OCStringRef plural_name,
    OCStringRef symbol,
    double scale_to_coherent_si,
    OCStringRef *error) {
    SIUnitRef unit = AddToLib(quantity, name, plural_name, symbol, scale_to_coherent_si, error);
    SIUnitSetIsCGSUnit(unit, true);
    return unit;
}
static SIUnitRef AddAtomicUnitToLib(
    OCStringRef quantity,
    OCStringRef name,
    OCStringRef plural_name,
    OCStringRef symbol,
    double scale_to_coherent_si,
    OCStringRef *error) {
    SIUnitRef unit = AddToLib(quantity, name, plural_name, symbol, scale_to_coherent_si, error);
    SIUnitSetIsAtomicUnit(unit, true);
    return unit;
}
static SIUnitRef AddConstantToLib(
    OCStringRef quantity,
    OCStringRef name,
    OCStringRef plural_name,
    OCStringRef symbol,
    double scale_to_coherent_si,
    OCStringRef *error) {
    SIUnitRef unit = AddToLib(quantity, name, plural_name, symbol, scale_to_coherent_si, error);
    SIUnitSetIsConstant(unit, true);
    return unit;
}
static SIUnitRef AddPlanckUnitToLib(
    OCStringRef quantity,
    OCStringRef name,
    OCStringRef plural_name,
    OCStringRef symbol,
    double scale_to_coherent_si,
    OCStringRef *error) {
    SIUnitRef unit = AddToLib(quantity, name, plural_name, symbol, scale_to_coherent_si, error);
    SIUnitSetIsPlanckUnit(unit, true);
    return unit;
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
static bool AddToLibPrefixedWithUnitSystem(
    OCStringRef quantity,
    OCStringRef name,
    OCStringRef plural_name,
    OCStringRef symbol,
    double scale_to_coherent_si,
    void (*unitSystemSetter)(SIUnitRef, bool),
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
        // Create immutable copies to avoid issues with mutable string reuse
        OCStringRef finalPrefixedName = OCStringCreateCopy(prefixedName);
        OCStringRef finalPrefixedPluralName = OCStringCreateCopy(prefixedPluralName);
        OCStringRef finalPrefixedSymbol = OCStringCreateCopy(prefixedSymbol);
        SIUnitRef theUnit = AddToLib(quantity, finalPrefixedName, finalPrefixedPluralName, finalPrefixedSymbol, scale_to_coherent_si * prefixValueForSIPrefix(prefix), error);
        // Set the unit system flag
        if (unitSystemSetter) unitSystemSetter(theUnit, true);
        // Release the immutable copies
        OCRelease(finalPrefixedName);
        OCRelease(finalPrefixedPluralName);
        OCRelease(finalPrefixedSymbol);
        if (!theUnit) {
            OCRelease(prefixedName);
            OCRelease(prefixedPluralName);
            OCRelease(prefixedSymbol);
            return false;
        }
    }
    return true;
}
static bool AddToLibPrefixed(
    OCStringRef quantity,
    OCStringRef name,
    OCStringRef plural_name,
    OCStringRef symbol,
    double scale_to_coherent_si,
    OCStringRef *error) {
    return AddToLibPrefixedWithUnitSystem(quantity, name, plural_name, symbol,
                                          scale_to_coherent_si, NULL, error);
}
static bool AddSIToLibPrefixed(
    OCStringRef quantity,
    OCStringRef name,
    OCStringRef plural_name,
    OCStringRef symbol,
    double scale_to_coherent_si,
    OCStringRef *error) {
    return AddToLibPrefixedWithUnitSystem(quantity, name, plural_name, symbol,
                                          scale_to_coherent_si, SIUnitSetIsSIUnit, error);
}
static bool AddCGSToLibPrefixed(
    OCStringRef quantity,
    OCStringRef name,
    OCStringRef plural_name,
    OCStringRef symbol,
    double scale_to_coherent_si,
    OCStringRef *error) {
    return AddToLibPrefixedWithUnitSystem(quantity, name, plural_name, symbol,
                                          scale_to_coherent_si, SIUnitSetIsCGSUnit, error);
}
static OCComparisonResult unitNameLengthSort(const void *val1, const void *val2, void *context) {
    SIUnitRef unit1 = (SIUnitRef)val1;
    SIUnitRef unit2 = (SIUnitRef)val2;
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
static bool SIUnitCreateLibraries(void) {
    setlocale(LC_ALL, "");
    const struct lconv *const currentlocale = localeconv();
    unitsArrayLibrary = OCArrayCreateMutable(0, &kOCTypeArrayCallBacks);
    unitsDictionaryLibrary = OCDictionaryCreateMutable(0);
    unitsQuantitiesLibrary = OCDictionaryCreateMutable(0);
    unitsDimensionalitiesLibrary = OCDictionaryCreateMutable(0);
    tokenSymbolLibrary = OCArrayCreateMutable(0, &kOCTypeArrayCallBacks);
    IF_NO_OBJECT_EXISTS_RETURN(unitsDictionaryLibrary, false);
    IF_NO_OBJECT_EXISTS_RETURN(unitsQuantitiesLibrary, false);
    IF_NO_OBJECT_EXISTS_RETURN(unitsDimensionalitiesLibrary, false);
    IF_NO_OBJECT_EXISTS_RETURN(tokenSymbolLibrary, false);
    // Initialize all units by including the definitions
#include "SIUnitDefinitions.h"
    SIUnitAddUSPlainVolumeUnits(&error);    // Define US Volume units as default volume
    SIUnitAddUKLabeledVolumeUnits(&error);  // Define UK Volume units
    if (currentlocale->currency_symbol) {
        if (strcmp(currentlocale->currency_symbol, "£") == 0) {
            SIUnitLibrarySetDefaultVolumeSystem(kSIVolumeSystemUK);
        }
    }
    OCArraySortValues(unitsArrayLibrary, OCRangeMake(0, OCArrayGetCount(unitsArrayLibrary)), unitNameLengthSort, NULL);
    // Add unit to units library dictionary
    for (OCIndex index = 0; index < OCArrayGetCount(unitsArrayLibrary); index++) {
        SIUnitRef unit = OCArrayGetValueAtIndex(unitsArrayLibrary, index);
        AddToUnitsDictionaryLibrary(unit);
    }
    return true;
}
// Add a cleanup function for static dictionaries and array
void SIUnitLibrariesShutdown(void) {
    if (!unitsDictionaryLibrary) return;
    // All SIUnits inside these Arrays should be static instances.
    if (unitsQuantitiesLibrary) {
        OCRelease(unitsQuantitiesLibrary);
        unitsQuantitiesLibrary = NULL;
    }
    if (unitsDimensionalitiesLibrary) {
        OCRelease(unitsDimensionalitiesLibrary);
        unitsDimensionalitiesLibrary = NULL;
    }
    if (tokenSymbolLibrary) {
        OCRelease(tokenSymbolLibrary);
        tokenSymbolLibrary = NULL;
    }
    if (unitsDictionaryLibrary) {
        OCRelease(unitsDictionaryLibrary);
        unitsDictionaryLibrary = NULL;
    }
    if (unitsArrayLibrary) {
        for (OCIndex index = 0; index < OCArrayGetCount(unitsArrayLibrary); index++) {
            SIUnitRef unit = (SIUnitRef)OCArrayGetValueAtIndex(unitsArrayLibrary, index);
            OCTypeSetStaticInstance(unit, false);
        }
        OCRelease(unitsArrayLibrary);
        unitsArrayLibrary = NULL;
    }
}
SIUnitRef SIUnitWithSymbol(OCStringRef symbol) {
    if (NULL == symbol) {
        return NULL;
    }
    if (NULL == unitsDictionaryLibrary) SIUnitCreateLibraries();
    IF_NO_OBJECT_EXISTS_RETURN(unitsDictionaryLibrary, NULL);
    OCStringRef key = SIUnitCreateCleanedExpression(symbol);
    SIUnitRef unit = OCDictionaryGetValue(unitsDictionaryLibrary, key);
    OCRelease(key);
    return unit;
}
static bool SIUnitLibraryRemoveUnitWithSymbol(OCStringRef symbol) {
    if (NULL == unitsDictionaryLibrary) SIUnitCreateLibraries();
    OCStringRef key = SIUnitCreateCleanedExpression(symbol);
    if (OCDictionaryContainsKey(unitsDictionaryLibrary, key)) {
        SIUnitRef unit = (SIUnitRef)OCDictionaryGetValue(unitsDictionaryLibrary, key);
        OCDictionaryRemoveValue(unitsDictionaryLibrary, key);
        OCIndex index = OCArrayGetFirstIndexOfValue(unitsArrayLibrary, unit);
        OCTypeSetStaticInstance(unit, false);
        OCArrayRemoveValueAtIndex(unitsArrayLibrary, index);
        return true;
    }
    return false;
}
static bool SIUnitLibraryRemovePlainVolumeUnits(void) {
    bool success = true;
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("gal"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("qt"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("pt"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("cup"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("gi"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("floz"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("tbsp"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("tsp"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("halftsp"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("quartertsp"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("(1/gal)"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("(1/qt)"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("(1/pt)"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("(1/cup)"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("(1/gi)"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("(1/floz)"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("(1/tbsp)"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("(1/tsp)"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("mi/gal"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("gal/h"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("gal/min"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("gal/s"));
    return success;
}
static bool SIUnitLibraryRemoveUSLabeledVolumeUnits(void) {
    bool success = true;
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("galUS"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("qtUS"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("ptUS"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("cupUS"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("giUS"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("flozUS"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("tbspUS"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("tspUS"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("halftspUS"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("quartertspUS"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("(1/galUS)"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("(1/qtUS)"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("(1/ptUS)"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("(1/cupUS)"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("(1/giUS)"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("(1/flozUS)"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("(1/tbspUS)"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("(1/tspUS)"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("mi/galUS"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("galUS/h"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("galUS/min"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("galUS/s"));
    return success;
}
static bool SIUnitLibraryRemoveUKLabeledVolumeUnits(void) {
    bool success = true;
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("galUK"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("qtUK"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("ptUK"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("cupUK"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("giUK"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("flozUK"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("tbspUK"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("tspUK"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("halftspUK"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("quartertspUK"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("(1/galUK)"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("(1/qtUK)"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("(1/ptUK)"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("(1/cupUK)"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("(1/giUK)"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("(1/flozUK)"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("(1/tbspUK)"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("(1/tspUK)"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("mi/galUK"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("galUK/h"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("galUK/min"));
    success *= SIUnitLibraryRemoveUnitWithSymbol(STR("galUK/s"));
    return success;
}
static bool SIUnitLibraryAddUSLabeledVolumeUnits(OCStringRef *error) {
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
    AddToLib(kSIQuantityLengthPerVolume, STR("mile per US gallon"), STR("miles per US gallon"), STR("mi/galUS"), kSIMile / 0.003785411784, error);
    AddToLib(kSIQuantityVolumetricFlowRate, STR("US gallon per hour"), STR("US gallons per hour"), STR("galUS/h"), 0.003785411784 / 3600., error);
    AddToLib(kSIQuantityVolumetricFlowRate, STR("US gallon per minute"), STR("US gallons per minute"), STR("galUS/min"), 0.003785411784 / 60., error);
    AddToLib(kSIQuantityVolumetricFlowRate, STR("US gallon per second"), STR("US gallons per second"), STR("galUS/s"), 0.003785411784, error);
    // Check if any error occurred
    if (error && *error) return false;
    return true;
}
static bool SIUnitAddUSPlainVolumeUnits(OCStringRef *error) {
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
    AddToLib(kSIQuantityLengthPerVolume, STR("mile per gallon"), STR("miles per gallon"), STR("mi/gal"), kSIMile / 0.003785411784, error);
    AddToLib(kSIQuantityVolumetricFlowRate, STR("gallon per hour"), STR("gallons per hour"), STR("gal/h"), 0.003785411784 / 3600., error);
    AddToLib(kSIQuantityVolumetricFlowRate, STR("gallon per minute"), STR("gallons per minute"), STR("gal/min"), 0.003785411784 / 60., error);
    AddToLib(kSIQuantityVolumetricFlowRate, STR("gallon per second"), STR("gallons per second"), STR("gal/s"), 0.003785411784, error);
    // Check if any error occurred
    if (error && *error) return false;
    return true;
}
static bool SIUnitAddUKPlainVolumeUnits(OCStringRef *error) {
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
    AddToLib(kSIQuantityLengthPerVolume, STR("mile per gallon"), STR("miles per gallon"), STR("mi/gal"), kSIMile / 0.00454609, error);
    AddToLib(kSIQuantityVolumetricFlowRate, STR("gallon per hour"), STR("gallons per hour"), STR("gal/h"), 0.00454609 / 3600., error);
    AddToLib(kSIQuantityVolumetricFlowRate, STR("gallon per minute"), STR("gallons per minute"), STR("gal/min"), 0.00454609 / 60., error);
    AddToLib(kSIQuantityVolumetricFlowRate, STR("gallon per second"), STR("gallons per second"), STR("gal/s"), 0.00454609, error);
    // Check if any error occurred
    if (error && *error) return false;
    return true;
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
    AddToLib(kSIQuantityLengthPerVolume, STR("mile per imperial gallon"), STR("miles per imperial gallon"), STR("mi/galUK"), kSIMile / 0.00454609, error);
    AddToLib(kSIQuantityVolumetricFlowRate, STR("imperial gallon per hour"), STR("imperial gallons per hour"), STR("galUK/h"), 0.00454609 / 3600., error);
    AddToLib(kSIQuantityVolumetricFlowRate, STR("imperial gallon per minute"), STR("imperial gallons per minute"), STR("galUK/min"), 0.00454609 / 60., error);
    AddToLib(kSIQuantityVolumetricFlowRate, STR("imperial gallon per second"), STR("imperial gallons per second"), STR("galUK/s"), 0.00454609, error);
    // Check if any error occurred
    if (error && *error) return false;
    return true;
}
void SIUnitLibrarySetDefaultVolumeSystem(SIVolumeSystem system) {
    bool useUKAsDefault = (system == kSIVolumeSystemUK);
    if (imperialVolumes == useUKAsDefault) return;
    OCStringRef error = NULL;
    SIUnitLibraryRemovePlainVolumeUnits();
    if (useUKAsDefault) {                              // UK volumes get plain symbols (gal, qt, tsp, etc.)
        SIUnitLibraryRemoveUKLabeledVolumeUnits();     // Remove UK labeled volumes
        SIUnitLibraryAddUSLabeledVolumeUnits(&error);  // Add US labeled volumes (galUS, qtUS, etc.)
        SIUnitAddUKPlainVolumeUnits(&error);           // Add UK plain volumes (gal, qt, etc.)
    } else {                                           // US volumes get plain symbols (gal, qt, tsp, etc.)
        SIUnitLibraryRemoveUSLabeledVolumeUnits();     // Remove US labeled volumes
        SIUnitAddUSPlainVolumeUnits(&error);           // Add US plain volumes (gal, qt, etc.)
        SIUnitAddUKLabeledVolumeUnits(&error);         // Add UK labeled volumes (galUK, qtUK, etc.)
    }
    imperialVolumes = useUKAsDefault;
}
SIVolumeSystem SIUnitLibraryGetDefaultVolumeSystem(void) {
    return imperialVolumes ? kSIVolumeSystemUK : kSIVolumeSystemUS;
}
SIUnitRef SIUnitFindWithName(OCStringRef input) {
    if (NULL == unitsDictionaryLibrary) SIUnitCreateLibraries();
    IF_NO_OBJECT_EXISTS_RETURN(unitsDictionaryLibrary, NULL);
    int64_t count = OCDictionaryGetCount(unitsDictionaryLibrary);
    OCStringRef keys[count];
    SIUnitRef units[count];
    OCDictionaryGetKeysAndValues(unitsDictionaryLibrary, (const void **)keys, (const void **)units);
    for (int64_t index = 0; index < count; index++) {
        OCStringRef name = units[index]->name;
        if (name)
            if (OCStringCompare(name, input, 0) == kOCCompareEqualTo) {
                SIUnitRef theUnit = units[index];
                return theUnit;
            }
        OCStringRef plural_name = units[index]->plural_name;
        if (plural_name)
            if (OCStringCompare(plural_name, input, 0) == kOCCompareEqualTo) {
                SIUnitRef theUnit = units[index];
                return theUnit;
            }
    }
    return NULL;
}
static SIUnitRef SIUnitWithParameters(SIDimensionalityRef dimensionality,
                                      OCStringRef name,
                                      OCStringRef plural_name,
                                      OCStringRef symbol,
                                      double scale_to_coherent_si) {
    // Create a temporary unit to get its symbol, then check if equivalent exists
    SIUnitRef tempUnit = SIUnitCreate(dimensionality, name, plural_name, symbol, scale_to_coherent_si);
    if (NULL == tempUnit) return NULL;
    // Check if another unit with this symbol already exists
    OCStringRef key = SIUnitCreateCleanedExpression(tempUnit->symbol);
    OCDictionaryRef unitsLib = SIUnitGetUnitsDictionaryLib();
    if (OCDictionaryContainsKey(unitsLib, key)) {
        SIUnitRef existingUnit = OCDictionaryGetValue(unitsLib, key);
        OCRelease(tempUnit);  // Discard the temporary unit
        OCRelease(key);
        return existingUnit;
    }
    // No existing unit found, so add this fresh unit to library
    RegisterUnitInLibraries(tempUnit, NULL, dimensionality);
    AddToUnitsDictionaryLibrary(tempUnit);
    OCRelease(tempUnit);  // will be no-op since should be static instance now
    OCRelease(key);
    return tempUnit;
}
SIUnitRef SIUnitCoherentUnitFromDimensionality(SIDimensionalityRef dimensionality) {
    OCMutableStringRef symbol = (OCMutableStringRef)SIDimensionalityCopySymbol(dimensionality);
    // Create Coherent Unit symbol by simply replacing base dimensionality symbols with base SI symbols
    OCStringFindAndReplace2(symbol, STR("L"), STR("m"));
    OCStringFindAndReplace2(symbol, STR("M"), STR("kg"));
    OCStringFindAndReplace2(symbol, STR("T"), STR("s"));
    OCStringFindAndReplace2(symbol, STR("I"), STR("A"));
    OCStringFindAndReplace2(symbol, STR("N"), STR("mol"));
    OCStringFindAndReplace2(symbol, STR("J"), STR("cd"));
    OCStringRef key = SIUnitCreateCleanedExpression(symbol);
    // See if unit is already in the unitsDictionaryLibrary
    OCDictionaryRef unitsLib = SIUnitGetUnitsDictionaryLib();
    if (OCDictionaryContainsKey(unitsLib, key)) {
        SIUnitRef existingUnit = OCDictionaryGetValue(unitsLib, key);
        OCRelease(key);
        return existingUnit;
    }
    OCRelease(key);

    // Unit not in library so create new unit
    SIUnitRef tempUnit = SIUnitCreate(dimensionality, NULL, NULL, symbol, 1.0);
    SIUnitSetIsSIUnit(tempUnit, true);
    // No existing unit found, so add this fresh unit to library
    RegisterUnitInLibraries(tempUnit, NULL, dimensionality);
    AddToUnitsDictionaryLibrary(tempUnit);
    OCRelease(tempUnit);  // will be no-op since should be static instance now
    return tempUnit;
}
bool SIUnitIsCoherentUnit(SIUnitRef theUnit) {
    if (SIUnitCoherentUnitFromDimensionality(theUnit->dimensionality) == theUnit) return true;
    return false;
}
bool SIUnitIsDimensionless(SIUnitRef theUnit) {
    if (!SIDimensionalityIsDimensionless(theUnit->dimensionality))
        return false;
    return true;
}
OCArrayRef SIUnitCreateArrayOfUnitsForQuantity(OCStringRef quantity) {
    IF_NO_OBJECT_EXISTS_RETURN(quantity, NULL);
    if (NULL == unitsDictionaryLibrary) SIUnitCreateLibraries();
    if (OCDictionaryContainsKey(unitsQuantitiesLibrary, quantity)) {
        OCMutableArrayRef array = (OCMutableArrayRef)OCDictionaryGetValue(unitsQuantitiesLibrary, quantity);
        return OCArrayCreateCopy(array);
    }
    return NULL;
}
OCArrayRef SIUnitCreateArrayOfUnitsForDimensionality(SIDimensionalityRef theDim) {
    IF_NO_OBJECT_EXISTS_RETURN(theDim, NULL);
    if (NULL == unitsDictionaryLibrary) SIUnitCreateLibraries();
    OCStringRef symbol = SIDimensionalityCopySymbol(theDim);
    if (OCDictionaryContainsKey(unitsDimensionalitiesLibrary, symbol)) {
        OCMutableArrayRef array = (OCMutableArrayRef)OCDictionaryGetValue(unitsDimensionalitiesLibrary, symbol);
        OCRelease(symbol);
        return OCArrayCreateCopy(array);
    }
    OCRelease(symbol);
    return NULL;
}
OCArrayRef SIUnitCreateArrayOfUnitsForSameReducedDimensionality(SIDimensionalityRef theDim) {
    IF_NO_OBJECT_EXISTS_RETURN(theDim, NULL);
    if (NULL == unitsDictionaryLibrary) SIUnitCreateLibraries();
    OCArrayRef dimensionalities = SIDimensionalityCreateArrayWithSameReducedDimensionality(theDim);
    OCMutableArrayRef result = OCArrayCreateMutable(0, &kOCTypeArrayCallBacks);
    for (uint64_t index = 0; index < OCArrayGetCount(dimensionalities); index++) {
        SIDimensionalityRef dimensionality = OCArrayGetValueAtIndex(dimensionalities, index);
        OCStringRef symbol = SIDimensionalityCopySymbol(dimensionality);
        if (OCDictionaryContainsKey(unitsDimensionalitiesLibrary, symbol)) {
            OCMutableArrayRef array = (OCMutableArrayRef)OCDictionaryGetValue(unitsDimensionalitiesLibrary, symbol);
            OCArrayAppendArray(result, array, OCRangeMake(0, OCArrayGetCount(array)));
        }
        OCRelease(symbol);
    }
    OCRelease(dimensionalities);
    return result;
}
SIUnitRef SIUnitDimensionlessAndUnderived(void) {
    return OCDictionaryGetValue(SIUnitGetUnitsDictionaryLib(), STR(" "));
}
// Equivalent units can be substituted for each other without changing the associated numerical value
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
static OCComparisonResult unit2Sort(const void *val1, const void *val2, void *context) {
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
OCArrayRef SIUnitCreateArrayOfConversionUnits(SIUnitRef theUnit) {
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, NULL);
    OCArrayRef result = SIUnitCreateArrayOfUnitsForSameReducedDimensionality(theUnit->dimensionality);
    OCMutableArrayRef sorted = OCArrayCreateMutableCopy(result);
    OCArraySortValues(sorted, OCRangeMake(0, OCArrayGetCount(result)), unit2Sort, NULL);
    OCRelease(result);
    return sorted;
}
SIUnitRef SIUnitFindEquivalentUnitWithShortestSymbol(SIUnitRef theUnit) {
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, NULL);
    if (theUnit == SIUnitDimensionlessAndUnderived())
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
    int64_t length = OCStringGetLength(symbol);
    OCRelease(symbol);
    for (uint64_t i = 0, cnt = OCArrayGetCount(candidates); i < cnt; i++) {
        SIUnitRef candidate = OCArrayGetValueAtIndex(candidates, i);
        OCStringRef candidateSymbol = SIUnitCopySymbol(candidate);
        if (OCStringGetLength(candidateSymbol) < length) {
            best = candidate;
            length = OCStringGetLength(candidateSymbol);
        }
        OCRelease(candidateSymbol);
    }
    OCRelease(candidates);
    return best;
}
//
// Helper function to check if a scale is approximately a power of 10 (SI unit)
static bool is_si_scale(double scale) {
    if (scale <= 0.0) return false;
    // Check for common SI prefixes by looking at powers of 1000
    double log_val = log10(scale);
    double rounded = round(log_val / 3.0) * 3.0;  // Round to nearest multiple of 3
    // Check if it's close to a multiple of 3 (1000-based prefixes)
    if (fabs(log_val - rounded) < 0.01) return true;
    // Also check for simple powers of 10
    double simple_rounded = round(log_val);
    if (fabs(log_val - simple_rounded) < 0.01) return true;
    return false;
}
//
// Common helper function for finding best matching unit with SI preference
static SIUnitRef SIUnitFindBestMatchingUnit(SIDimensionalityRef dimensionality, double target_scale) {
    OCArrayRef candidates = SIUnitCreateArrayOfUnitsForDimensionality(dimensionality);
    if (!candidates || OCArrayGetCount(candidates) == 0) {
        if (candidates) OCRelease(candidates);
        return NULL;
    }
    SIUnitRef best_match = NULL;
    int best_token_count = INT_MAX;
    double best_scale_diff = 1e100;
    for (OCIndex i = 0; i < OCArrayGetCount(candidates); i++) {
        SIUnitRef candidate = (SIUnitRef)OCArrayGetValueAtIndex(candidates, i);
        int symbol_token_count = SIUnitCountTokenSymbols(candidate->symbol);
        double scale_diff = fabs(candidate->scale_to_coherent_si - target_scale);
        bool is_si = SIUnitIsSIUnit(candidate);
        // Only consider SI units as candidates
        if (!is_si) {
            continue;
        }
        // Determine if this candidate is better than the current best
        bool is_better = false;
        if (best_match == NULL) {
            // First SI candidate is automatically the best so far
            is_better = true;
        } else {
            // Priority 1: Prefer smaller scale difference (closest to target scale)
            if (scale_diff < best_scale_diff) {
                is_better = true;
            } else if (fabs(scale_diff - best_scale_diff) < 1e-15) {
                // Priority 2: Among units with same scale difference, prefer fewer token symbols
                if (symbol_token_count < best_token_count) {
                    is_better = true;
                }
            }
        }
        if (is_better) {
            best_match = candidate;
            best_token_count = symbol_token_count;
            best_scale_diff = scale_diff;
        }
    }
    OCRelease(candidates);
    return best_match;
}
SIUnitRef SIUnitByReducing(SIUnitRef theUnit, double *unit_multiplier) {
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, NULL);
    SIDimensionalityRef dimensionality = SIDimensionalityByReducing(theUnit->dimensionality);
    if (SIDimensionalityEqual(dimensionality, theUnit->dimensionality))
        return SIUnitFindEquivalentUnitWithShortestSymbol(theUnit);
    SIUnitRef best_match = SIUnitFindBestMatchingUnit(dimensionality, theUnit->scale_to_coherent_si);
    if (best_match) {
        if (unit_multiplier) {
            *unit_multiplier *= theUnit->scale_to_coherent_si / best_match->scale_to_coherent_si;
        }
        return best_match;
    }
    // Reduce the symbol
    OCStringRef error = NULL;
    OCStringRef simplified_symbol = SIUnitCreateSimplifiedSymbol(theUnit->symbol, true, &error);
    // Create new unit with the reduced symbol and scale_to_coherent_si
    SIUnitRef result = SIUnitWithParameters(dimensionality, NULL, NULL, simplified_symbol, theUnit->scale_to_coherent_si);
    OCRelease(simplified_symbol);
    return result;
}
// Power operation
static SIUnitRef SIUnitByRaisingToPowerInternal(SIUnitRef input,
                                                int power,
                                                double *unit_multiplier,
                                                bool reduce,
                                                OCStringRef *error) {
    if (error && *error) return NULL;
    IF_NO_OBJECT_EXISTS_RETURN(input, NULL);
    if (power == 0) return SIUnitDimensionlessAndUnderived();
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
    SIUnitRef best_match = SIUnitFindBestMatchingUnit(dimensionality, scale);
    if (best_match) {
        if (unit_multiplier) {
            *unit_multiplier *= scale / best_match->scale_to_coherent_si;
        }
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
    OCStringRef simplified_symbol = SIUnitCreateSimplifiedSymbol(new_symbol, reduce, error);
    // Create new unit with the simplified symbol and calculated scale
    SIUnitRef result = SIUnitWithParameters(dimensionality, NULL, NULL, simplified_symbol, scale);
    OCRelease(new_symbol);
    OCRelease(simplified_symbol);
    return result;
}
// Multiplication operation
static SIUnitRef SIUnitByMultiplyingInternal(SIUnitRef theUnit1,
                                             SIUnitRef theUnit2,
                                             double *unit_multiplier,
                                             bool reduce,
                                             OCStringRef *error) {
    if (error && *error) return NULL;
    IF_NO_OBJECT_EXISTS_RETURN(theUnit1, NULL);
    IF_NO_OBJECT_EXISTS_RETURN(theUnit2, NULL);
    // multiplying by dimensionless does nothing
    SIUnitRef dimless = SIUnitDimensionlessAndUnderived();
    if (theUnit1 == dimless) return theUnit2;
    if (theUnit2 == dimless) return theUnit1;
    if (SIUnitAreEquivalentUnits(theUnit1, theUnit2)) {
        return SIUnitByRaisingToPowerInternal(theUnit1, 2, unit_multiplier, reduce, error);
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
    // First approach: Look for existing units with matching dimensionality
    SIUnitRef best_match = SIUnitFindBestMatchingUnit(dimensionality, scale);
    if (best_match) {
        if (unit_multiplier) {
            *unit_multiplier *= scale / best_match->scale_to_coherent_si;
        }
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
    OCStringRef simplified_symbol = SIUnitCreateSimplifiedSymbol(new_symbol, reduce, error);
    // Create new unit with the simplified symbol and calculated scale
    SIUnitRef result = SIUnitWithParameters(dimensionality, NULL, NULL, simplified_symbol, scale);
    OCRelease(new_symbol);
    OCRelease(simplified_symbol);
    return result;
}
// Public API functions - now more intuitive with 'reduce' parameter
SIUnitRef SIUnitByRaisingToPowerWithoutReducing(SIUnitRef input,
                                                int power,
                                                double *unit_multiplier,
                                                OCStringRef *error) {
    return SIUnitByRaisingToPowerInternal(input, power, unit_multiplier, false, error);  // reduce = false
}
SIUnitRef SIUnitByRaisingToPower(SIUnitRef input,
                                 int power,
                                 double *unit_multiplier,
                                 OCStringRef *error) {
    return SIUnitByRaisingToPowerInternal(input, power, unit_multiplier, true, error);  // reduce = true
}
SIUnitRef SIUnitByMultiplyingWithoutReducing(SIUnitRef theUnit1,
                                             SIUnitRef theUnit2,
                                             double *unit_multiplier,
                                             OCStringRef *error) {
    return SIUnitByMultiplyingInternal(theUnit1, theUnit2, unit_multiplier, false, error);  // reduce = false
}
SIUnitRef SIUnitByMultiplying(SIUnitRef theUnit1,
                              SIUnitRef theUnit2,
                              double *unit_multiplier,
                              OCStringRef *error) {
    return SIUnitByMultiplyingInternal(theUnit1, theUnit2, unit_multiplier, true, error);  // reduce = true
}
// Division operation
static SIUnitRef SIUnitByDividingInternal(SIUnitRef theUnit1,
                                          SIUnitRef theUnit2,
                                          double *unit_multiplier,
                                          bool reduce,
                                          OCStringRef *error) {
    if (error && *error) return NULL;
    IF_NO_OBJECT_EXISTS_RETURN(theUnit1, NULL);
    IF_NO_OBJECT_EXISTS_RETURN(theUnit2, NULL);
    // dividing by dimensionless does nothing
    SIUnitRef dimless = SIUnitDimensionlessAndUnderived();
    if (theUnit2 == dimless) return theUnit1;
    if (SIUnitAreEquivalentUnits(theUnit1, theUnit2)) {
        return SIUnitDimensionlessAndUnderived();
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
    // First approach: Look for existing units with matching dimensionality
    SIUnitRef best_match = SIUnitFindBestMatchingUnit(dimensionality, scale);
    if (best_match) {
        if (unit_multiplier) {
            *unit_multiplier *= scale / best_match->scale_to_coherent_si;
        }
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
    OCStringRef simplified_symbol = SIUnitCreateSimplifiedSymbol(new_symbol, reduce, error);
    // Create new unit with the simplified symbol and calculated scale
    SIUnitRef result = SIUnitWithParameters(dimensionality, NULL, NULL, simplified_symbol, scale);
    OCRelease(new_symbol);
    OCRelease(simplified_symbol);
    return result;
}
// Public API functions
SIUnitRef SIUnitByDividingWithoutReducing(SIUnitRef theUnit1,
                                          SIUnitRef theUnit2,
                                          double *unit_multiplier,
                                          OCStringRef *error) {
    return SIUnitByDividingInternal(theUnit1, theUnit2, unit_multiplier, false, error);  // reduce = false
}
SIUnitRef SIUnitByDividing(SIUnitRef theUnit1,
                           SIUnitRef theUnit2,
                           double *unit_multiplier,
                           OCStringRef *error) {
    return SIUnitByDividingInternal(theUnit1, theUnit2, unit_multiplier, true, error);  // reduce = true
}
// Nth root operation
SIUnitRef SIUnitByTakingNthRoot(SIUnitRef theUnit,
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
    SIUnitRef best_match = SIUnitFindBestMatchingUnit(dimensionality, scale);
    if (best_match) {
        if (unit_multiplier) {
            *unit_multiplier *= scale / best_match->scale_to_coherent_si;
        }
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
    OCStringRef simplified_symbol = SIUnitCreateSimplifiedSymbol(new_symbol, false, error);
    // Create new unit with the simplified symbol and calculated scale
    SIUnitRef result = SIUnitWithParameters(dimensionality,
                                            NULL,
                                            NULL,
                                            simplified_symbol,
                                            scale);
    OCRelease(new_symbol);
    OCRelease(simplified_symbol);
    return result;
}
OCStringRef SIUnitCreateQuantityNameGuess(SIUnitRef theUnit) {
    OCStringRef quantityName = NULL;
    OCArrayRef quantityNames = SIDimensionalityCreateArrayOfQuantityNames(theUnit->dimensionality);
    if (quantityNames) {
        if (OCArrayGetCount(quantityNames) > 0) {
            quantityName = OCArrayGetValueAtIndex(quantityNames, 0);  // Borrowed
        }
        OCRelease(quantityNames);
        return OCStringCreateCopy(quantityName);
    } else {
        return SIDimensionalityCopySymbol(theUnit->dimensionality);
    }
}
SIUnitRef SIUnitFromExpression(OCStringRef expression, double *unit_multiplier, OCStringRef *error) {
    if (error && *error) return NULL;
    OCMutableDictionaryRef unitsLib = SIUnitGetUnitsDictionaryLib();
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
    IF_NO_OBJECT_EXISTS_RETURN(unitsDictionaryLibrary, NULL);
    if (OCStringCompare(expression, STR(" "), 0) == kOCCompareEqualTo) {
        if (unit_multiplier) *unit_multiplier = 1.0;
        return SIUnitDimensionlessAndUnderived();
    }
    // Try library lookup first
    OCStringRef key = SIUnitCreateCleanedExpression(expression);
    if (NULL == key) {
        *error = OCStringCreateWithFormat(STR("Invalid unit expression: %@"), expression);
        return NULL;
    }
    SIUnitRef unit = OCDictionaryGetValue(unitsDictionaryLibrary, key);
    if (unit) {
        if (unit_multiplier) *unit_multiplier = 1.0;
        OCRelease(key);
        return unit;
    }
    // Parse the expression if not found in library
    double multiplier = 1.0;  // Default multiplier
    unit = SIUnitFromExpressionInternal(expression, &multiplier, error);
    if (NULL == unit) {
        *error = OCStringCreateWithFormat(STR("Invalid unit expression: %@"), expression);
        return NULL;
    }
    if (multiplier != 1.0) {
        SIDimensionalityRef dimensionality = SIUnitGetDimensionality(unit);
        OCStringRef dimensionalitySymbol = SIDimensionalityCopySymbol(dimensionality);
        unit = AddNonExistingToLib(dimensionalitySymbol, NULL, NULL, key, multiplier, error);
        OCRelease(dimensionalitySymbol);
        if (unit_multiplier) *unit_multiplier = 1.0;
        OCRelease(key);
        return unit;
    }
    if (unit_multiplier) *unit_multiplier = multiplier;
    OCRelease(key);
    // error returned from SIUnitFromExpressionInternal
    return unit;
}
