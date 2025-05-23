//
//  SIUnit.h
//  SITypes
//
//  Created by philip on 6/7/17.
//  Copyright © 2017 PhySy Ltd. All rights reserved.
//

#ifndef SIUnit_h
#define SIUnit_h

#include "SILibrary.h"

/**
 * @file SIUnit.h
 * @brief Declares the SIUnit interface for representing and manipulating physical units.
 *
 * SIUnit provides an interface for SI and derived units, supporting unit construction, comparison, and conversion.
 * The API is CoreFoundation-style, with explicit memory management and reference counting.
 *
 * SIUnit objects are immutable by default; use SIMutableUnitRef for mutable variants.
 *
 * This interface supports querying the symbol, dimensionality, and conversion factors of a unit, as well as unit arithmetic and comparison.
 *
 * @author Philip Grandinetti
 * @copyright PhySy Ltd.
 */

/**
 * @brief SIUnit represents the unit of a physical quantity.
 * @details An important characteristic of physical quantities is that any given physical quantity can be derived from other physical quantities through physical laws. 
 * For example, the physical quantity of speed is calculated as a ratio of distance traveled to time elapsed. The volume of a box is calculated as the product of three quantities of length: i.e., height, width, and depth of the box. 
 * Any physical quantity can always be related back through physical laws to a smaller set of reference physical quantities. 
 * In fact, as the laws of physics become unified it has been argued that this smaller set can be reduced to simply the Planck length and the speed of light. 
 * At the level of theory employed by most scientists and engineers, however, there is a practical agreement that seven physical quantities should serve as fundamental reference quantities from which all other physical quantities can be derived. 
 * These reference quantities are:
 * - length
 * - mass
 * - time
 * - electric current
 * - thermodynamic temperature (the absolute measure of temperature)
 * - amount of substance
 * - luminous intensity
 */

/**
 * @enum SIPrefix
 * @brief SI Prefix enumeration.
 * @var kSIPrefixYocto SI Prefix yocto
 * @var kSIPrefixZepto SI Prefix zepto
 * @var kSIPrefixAtto SI Prefix atto
 * @var kSIPrefixFemto SI Prefix femto
 * @var kSIPrefixPico SI Prefix pico
 * @var kSIPrefixNano SI Prefix nano
 * @var kSIPrefixMicro SI Prefix micro
 * @var kSIPrefixMilli SI Prefix milli
 * @var kSIPrefixCenti SI Prefix centi
 * @var kSIPrefixDeci SI Prefix deci
 * @var kSIPrefixNone no SI Prefix
 * @var kSIPrefixDeca SI Prefix deca
 * @var kSIPrefixHecto SI Prefix hecto
 * @var kSIPrefixKilo SI Prefix kilo
 * @var kSIPrefixMega SI Prefix mega
 * @var kSIPrefixGiga SI Prefix giga
 * @var kSIPrefixTera SI Prefix tera
 * @var kSIPrefixPeta SI Prefix peta
 * @var kSIPrefixExa SI Prefix exa
 * @var kSIPrefixZetta SI Prefix zetta
 * @var kSIPrefixYotta SI Prefix yotta
 */
typedef enum {
    kSIPrefixYocto = -24,
    kSIPrefixZepto = -21,
    kSIPrefixAtto = -18,
    kSIPrefixFemto = -15,
    kSIPrefixPico = -12,
    kSIPrefixNano = -9,
    kSIPrefixMicro = -6,
    kSIPrefixMilli = -3,
    kSIPrefixCenti = -2,
    kSIPrefixDeci = -1,
    kSIPrefixNone = 0,
    kSIPrefixDeca = 1,
    kSIPrefixHecto = 2,
    kSIPrefixKilo = 3,
    kSIPrefixMega = 6,
    kSIPrefixGiga = 9,
    kSIPrefixTera = 12,
    kSIPrefixPeta = 15,
    kSIPrefixExa = 18,
    kSIPrefixZetta = 21,
    kSIPrefixYotta = 24
} SIPrefix;

/**
 * @brief Constants for SI units and physical constants.
 */
#define kSIUnitMeter             STR("meter")
#define kSIUnitMeters            STR("meters")
#define kSIUnitGram              STR("gram")
#define kSIUnitGrams             STR("grams")
#define kSIUnitSecond            STR("second")
#define kSIUnitSeconds           STR("seconds")
#define kSIUnitAmpere            STR("ampere")
#define kSIUnitAmperes           STR("amperes")
#define kSIUnitKelvin            STR("kelvin")
#define kSIUnitMole              STR("mole")
#define kSIUnitMoles             STR("moles")
#define kSIUnitCandela           STR("candela")
#define kSIUnitCandelas          STR("candelas")

#define kSIMinute       60.
#define kSIHour         3600            //  60.*60.
#define kSIDay          86400           //  60.*60*24.
#define kSIWeek         604800          //  60.*60*24.*7.
#define kSIMonth        2629800         //  365.25*86400/12.
#define kSIYear         31557600        //  365.25*86400
#define kSIDecade       315576000       //  365.25*86400*10
#define kSICentury      3155760000      //  365.25*86400*100
#define kSIMillennium   31557600000     //  365.25*86400*1000

#define kSISpeedOfLight             299792458
#define kSIElectricConstant         8.854187817620413e-12

#define kSIElementaryCharge         1.6021766208e-19
#define kSIPlanckConstant           6.626070040e-34
#define kSIPi                       3.141592653589793
#define kSIEulersNumber             2.718281828459045
#define kSIBoltmannConstant         1.38064852e-23
#define kSIAvogadroConstant         6.022140857e23
#define kSIStefanBoltzmannConstant  5.670367e-8
#define kSIWeinDisplacementConstant 2.8977729e-3

#define kSIElectronMass             9.10938356e-31
#define kSIElectronMagneticMoment   -928.4764620e-26
#define kSIElectronGFactor          -2.00231930436182

#define kSIProtonMass               1.672621898e-27
#define kSIProtonMagneticMoment     1.4106067873e-26
#define kSIProtonGFactor            5.585694702

#define kSINeutronMass              1.674927471e-27
#define kSINeutronMagneticMoment    -0.96623650e-26
#define kSINeutronGFactor           -3.82608545

#define kSIMuonMass                 1.883531594e-28
#define kSIMuonMagneticMoment       -4.49044826e-26
#define kSIMuonGFactor              -2.0023318418

#define kSIAtomicMassConstant       1.660539040e-27
#define kSIAlphaParticleMass        6.644657230e-27

#define kSIGravitaionalConstant     6.67408e-11
#define kSIGravityAcceleration      9.80665

/**
 * @typedef SIUnitRef
 * @brief Type of a reference to immutable SIUnit.
 */
typedef const struct __SIUnit * SIUnitRef;

#pragma mark Accessors

/**
 * @brief Gets the type ID of SIUnit.
 * @return The type ID.
 */
OCTypeID SIUnitGetTypeID(void);

/**
 * @brief Gets unit's dimensionality.
 * @param theUnit The unit.
 * @return The dimensionality of a unit.
 */
SIDimensionalityRef SIUnitGetDimensionality(SIUnitRef theUnit);

/**
 * @brief Gets unit's numerator root unit prefix for the dimension at index.
 * @param theUnit The unit.
 * @param index The index of the dimension.
 * @return The integer exponent associated with an SI prefix.
 * @details Root unit for length, mass, time, current, temperature, amount, and luminous intensity are assigned to index constants kSILengthIndex, kSIMassIndex, kSITimeIndex, kSICurrentIndex, kSITemperatureIndex, kSIAmountIndex, kSILuminousIntensityIndex, respectively.
 */
SIPrefix SIUnitGetNumeratorPrefixAtIndex(SIUnitRef theUnit, const uint8_t index);

/**
 * @brief Gets unit's denominator root unit prefix for the dimension at index.
 * @param theUnit The unit.
 * @param index The index of the dimension.
 * @return The integer exponent associated with an SI prefix.
 * @details Root unit for length, mass, time, current, temperature, amount, and luminous intensity are assigned to index constants kSILengthIndex, kSIMassIndex, kSITimeIndex, kSICurrentIndex, kSITemperatureIndex, kSIAmountIndex, kSILuminousIntensityIndex, respectively.
 */
SIPrefix SIUnitGetDenominatorPrefixAtIndex(SIUnitRef theUnit, const uint8_t index);

/**
 * @brief Gets the root name of unit.
 * @param theUnit The unit.
 * @return String containing the base name, or NULL.
 */
OCStringRef SIUnitCopyRootName(SIUnitRef theUnit);

/**
 * @brief Gets the plural root name of unit.
 * @param theUnit The unit.
 * @return String containing the plural base name, or NULL.
 */
OCStringRef SIUnitCopyRootPluralName(SIUnitRef theUnit);

/**
 * @brief Gets the root symbol of unit.
 * @param theUnit The unit.
 * @return String containing the base symbol, or NULL.
 */
OCStringRef SIUnitCopyRootSymbol(SIUnitRef theUnit);

/**
 * @brief Returns whether an SI prefix can be used with root symbol.
 * @param theUnit The unit.
 * @return True or false.
 */
bool SIUnitAllowsSIPrefix(SIUnitRef theUnit);

/**
 * @brief Gets the symbol prefix of unit.
 * @param theUnit The unit.
 * @return The integer exponent associated with SI prefix.
 */
SIPrefix SIUnitCopyRootSymbolPrefix(SIUnitRef theUnit);

/**
 * @brief Returns whether root symbol is a special SI unit symbol.
 * @param theUnit The unit.
 * @return True or false.
 */
bool SIUnitGetIsSpecialSISymbol(SIUnitRef theUnit);

#pragma mark Tests

/**
 * @brief Determines if unit is one of the seven coherent base units.
 * @param theUnit The unit.
 * @return True or false.
 */
bool SIUnitIsCoherentSIBaseUnit(SIUnitRef theUnit);

/**
 * @brief Determines if unit is one of the seven base root units.
 * @param theUnit The unit.
 * @return True or false.
 */
bool SIUnitIsSIBaseRootUnit(SIUnitRef theUnit);

/**
 * @brief Determines if unit is one of the prefixed seven base units.
 * @param theUnit The unit.
 * @return True or false.
 */
bool SIUnitIsSIBaseUnit(SIUnitRef theUnit);

/**
 * @brief Determines if unit is one of the SI coherent derived units.
 * @param theUnit The unit.
 * @return True or false.
 */
bool SIUnitIsCoherentDerivedUnit(SIUnitRef theUnit);

/**
 * @brief Determines if unit is the dimensionless and underived unit.
 * @param theUnit The unit.
 * @return True or false.
 */
bool SIUnitIsDimensionlessAndUnderived(SIUnitRef theUnit);

/**
 * @brief Determines if unit is the dimensionless unit.
 * @param theUnit The unit.
 * @return True or false.
 */
bool SIUnitIsDimensionless(SIUnitRef theUnit);

/**
 * @brief Determines if units are equivalent.
 * @param theUnit1 The first unit.
 * @param theUnit2 The second unit.
 * @return True or false.
 * @details Determines if units are equivalent - meaning they can be substituted for each other without modifying
 * the quantity's numerical value. Units with the same dimensionality but different prefixes (e.g., 'N' and 'kN',
 * or 'm' and 'cm') are NOT equivalent because the numerical value would need to be scaled when converting between them.
 * 
 * Equivalent units include:
 * - Coherent derived units and their base expressions (e.g., 'N' and 'kg*m/s^2')
 * - Units with the same dimensionality and same scale (e.g., 'in/s' and 'm/s' if properly converted)
 * - Units with identical reduction behavior
 * 
 * Non-equivalent units (even with same dimensionality):
 * - Units with different prefixes ('m' vs 'cm', 'N' vs 'kN')
 * - Units that require scaling for conversion ('Pa' vs 'lbf/in^2' - they have the same dimensionality
 *   but are not directly substitutable without a multiplier)
 * 
 * Use SIDimensionalityEqual() to check if units have the same dimensionality regardless of equivalence.
 * Use SIDimensionalityHasSameReducedDimensionality() for comparing units that might have different
 * representations of the same dimensionality.
 */
bool SIUnitAreEquivalentUnits(SIUnitRef theUnit1, SIUnitRef theUnit2);

/**
 * @brief Determines if the two units are equal.
 * @param theUnit1 The first unit.
 * @param theUnit2 The second unit.
 * @return True or false.
 */
bool SIUnitEqual(SIUnitRef theUnit1, SIUnitRef theUnit2);

#pragma mark Operations

/**
 * @brief Returns the dimensionless and underived unit.
 * @return The unit.
 */
SIUnitRef SIUnitDimensionlessAndUnderived(void);

/**
 * @brief Creates an array of units that are equivalent to a unit.
 * @param theUnit The equivalent unit.
 * @return An OCArray containing the results.
 */
OCArrayRef SIUnitCreateArrayOfEquivalentUnits(SIUnitRef theUnit);

/**
 * @brief Finds the unit with the shortest symbol.
 * @param theUnit The equivalent unit.
 * @return The unit with the shortest symbol.
 */
SIUnitRef SIUnitFindEquivalentUnitWithShortestSymbol(SIUnitRef theUnit);

/**
 * @brief Creates an array of units for a quantity.
 * @param quantity The quantity.
 * @return An OCArray containing the results.
 */
OCArrayRef SIUnitCreateArrayOfUnitsForQuantity(OCStringRef quantity);

/**
 * @brief Creates an array of units for a dimensionality.
 * @param theDimensionality The dimensionality.
 * @return An OCArray containing the results.
 */
OCArrayRef SIUnitCreateArrayOfUnitsForDimensionality(SIDimensionalityRef theDimensionality);

/**
 * @brief Creates an array of units having same reduced dimensionality.
 * @param theDimensionality The dimensionality.
 * @return An OCArray containing the results.
 */
OCArrayRef SIUnitCreateArrayOfUnitsForSameReducedDimensionality(SIDimensionalityRef theDimensionality);

/**
 * @brief Creates an array of units that have same dimensionality as unit.
 * @param theUnit The unit.
 * @return An OCArray containing the results.
 */
OCArrayRef SIUnitCreateArrayOfConversionUnits(SIUnitRef theUnit);

/**
 * @brief Gets the scaling from the Non-SI root unit to coherent derived SI unit with the same dimensionality.
 * @param theUnit The unit.
 * @return Scaling factor.
 */
double SIUnitGetScaleNonSIToCoherentSI(SIUnitRef theUnit);

/**
 * @brief Creates a OCStringRef with the name of unit from its root_name and prefix.
 * @param theUnit The unit.
 * @return A OCStringRef with the name.
 */
OCStringRef SIUnitCreateName(SIUnitRef theUnit);

/**
 * @brief Creates a OCStringRef with the plural name of unit from its plural root name and prefix.
 * @param theUnit The unit.
 * @return A OCStringRef with the plural name.
 */
OCStringRef SIUnitCreatePluralName(SIUnitRef theUnit);

/**
 * @brief Gets the OCStringRef with the Symbol of unit from its root symbol and prefix.
 * @param theUnit The unit.
 * @return A OCStringRef with the symbol.
 */
OCStringRef SIUnitCopySymbol(SIUnitRef theUnit);

/**
 * @brief Calculates the scaling factor needed to transform the numerical value of quantity with this unit into the appropriate numerical value for the coherent si unit of the same dimensionality.
 * @param theUnit The unit.
 * @return Numerical value scaling factor.
 */
double SIUnitScaleToCoherentSIUnit(SIUnitRef theUnit);

/**
 * @brief Creates a OCStringRef with the unit's derived symbol.
 * @param theUnit The unit.
 * @return A OCStringRef with the derived unit symbol.
 */
OCStringRef SIUnitCreateDerivedSymbol(SIUnitRef theUnit);

/**
 * @brief Returns a coherent SI unit with given dimensionality.
 * @param theDimensionality The dimensionality.
 * @return The unit.
 */
SIUnitRef SIUnitFindCoherentSIUnitWithDimensionality(SIDimensionalityRef theDimensionality);

/**
 * @brief Returns a coherent SI unit with the dimensionality of input unit.
 * @param input The input unit.
 * @param unit_multiplier Pointer to a double float variable for the unit_multiplier. This unit_multiplier will be scaled to make the new quantity's numerical value consistent with the new unit. Thus, the initial value for the unit_multiplier should be non-zero.
 * @return The new unit.
 */
SIUnitRef SIUnitFindCoherentSIUnit(SIUnitRef input, double *unit_multiplier);

/**
 * @brief Returns the unit by dividing the numerator and denominator exponents by an integer.
 * @param input The unit.
 * @param root The integer root.
 * @param unit_multiplier Pointer to a double float variable for the unit_multiplier. This unit_multiplier will be scaled to make the new quantity's numerical value consistent with the new unit. Thus, the initial value for the unit_multiplier should be non-zero.
 * @param error An OCString describing the error.
 * @return The nth root unit.
 * @details The numerator and denominator exponents in a valid unit can only take on integer values. If this function cannot return a valid unit then it will return NULL.
 */
SIUnitRef SIUnitByTakingNthRoot(SIUnitRef input, uint8_t root, double *unit_multiplier, OCStringRef *error);

/**
 * @brief Returns the unit by reducing the numerator and denominator exponents to their lowest integer values.
 * @param theUnit The unit.
 * @param unit_multiplier Pointer to a double float variable for the unit_multiplier. This unit_multiplier will be scaled to make the new quantity's numerical value consistent with the new unit. Thus, the initial value for the unit_multiplier should be non-zero.
 * @return The reduced unit.
 */
SIUnitRef SIUnitByReducing(SIUnitRef theUnit, double *unit_multiplier);

/**
 * @brief Returns the unit obtained by multiplying two units and reducing the dimensionality numerator and denominator exponents to their lowest integer values.
 * @param theUnit1 The first unit.
 * @param theUnit2 The second unit.
 * @param unit_multiplier Pointer to a double float variable for the unit_multiplier. This unit_multiplier will be scaled to make the new quantity's numerical value consistent with the new unit. Thus, the initial value for the unit_multiplier should be non-zero.
 * @param error An OCString describing the error.
 * @return The new unit.
 */
SIUnitRef SIUnitByMultiplying(SIUnitRef theUnit1, SIUnitRef theUnit2, double *unit_multiplier, OCStringRef *error);

/**
 * @brief Returns the unit obtained by multiplying two units.
 * @param theUnit1 The first unit.
 * @param theUnit2 The second unit.
 * @param unit_multiplier Pointer to a double float variable for the unit_multiplier. This unit_multiplier will be scaled to make the new quantity's numerical value consistent with the new unit. Thus, the initial value for the unit_multiplier should be non-zero.
 * @param error An OCString describing the error.
 * @return The new unit.
 */
SIUnitRef SIUnitByMultiplyingWithoutReducing(SIUnitRef theUnit1, SIUnitRef theUnit2, double *unit_multiplier, OCStringRef *error);

/**
 * @brief Returns the unit obtained by dividing two units and reducing the dimensionality numerator and denominator exponents to their lowest integer values.
 * @param theUnit1 The first unit.
 * @param theUnit2 The second unit.
 * @param unit_multiplier Pointer to a double float variable for the unit_multiplier. This unit_multiplier will be scaled to make the new quantity's numerical value consistent with the new unit. Thus, the initial value for the unit_multiplier should be non-zero.
 * @return The new unit.
 */
SIUnitRef SIUnitByDividing(SIUnitRef theUnit1, SIUnitRef theUnit2, double *unit_multiplier);

/**
 * @brief Returns the unit obtained by dividing two units.
 * @param theUnit1 The first unit.
 * @param theUnit2 The second unit.
 * @param unit_multiplier Pointer to a double float variable for the unit_multiplier. This unit_multiplier will be scaled to make the new quantity's numerical value consistent with the new unit. Thus, the initial value for the unit_multiplier should be non-zero.
 * @return The new unit.
 */
SIUnitRef SIUnitByDividingWithoutReducing(SIUnitRef theUnit1, SIUnitRef theUnit2, double *unit_multiplier);

/**
 * @brief Returns the unit obtained by raising a unit to a power and reducing the dimensionality numerator and denominator exponents to their lowest integer values.
 * @param input The unit.
 * @param power The power to raise the unit to.
 * @param unit_multiplier Pointer to a double float variable for the unit_multiplier. This unit_multiplier will be scaled to make the new quantity's numerical value consistent with the new unit. Thus, the initial value for the unit_multiplier should be non-zero.
 * @param error An OCString describing the error.
 * @return The new unit.
 */
SIUnitRef SIUnitByRaisingToAPower(SIUnitRef input, double power, double *unit_multiplier, OCStringRef *error);

/**
 * @brief Returns the unit obtained by raising a unit to a power.
 * @param input The unit.
 * @param power The power to raise the unit to.
 * @param unit_multiplier Pointer to a double float variable for the unit_multiplier. This unit_multiplier will be scaled to make the new quantity's numerical value consistent with the new unit. Thus, the initial value for the unit_multiplier should be non-zero.
 * @param error An OCString describing the error.
 * @return The new unit.
 */
SIUnitRef SIUnitByRaisingToAPowerWithoutReducing(SIUnitRef input, double power, double *unit_multiplier, OCStringRef *error);

/**
 * @brief Returns the unit with a specific name, if known.
 * @param input The name.
 * @return The unit or NULL if unit with name is not found.
 */
SIUnitRef SIUnitFindWithName(OCStringRef input);

/**
 * @brief Returns the unit with an underived symbol, if known.
 * @param symbol The symbol.
 * @return The unit or NULL if unit with symbol is not found.
 */
SIUnitRef SIUnitForUnderivedSymbol(OCStringRef symbol);

/**
 * @brief Returns the unit with symbol, if valid.
 * @param symbol The derived symbol.
 * @param unit_multiplier Pointer to a double float variable for the unit_multiplier. This unit_multiplier will be scaled to make the new quantity's numerical value consistent with the new unit. Thus, the initial value for the unit_multiplier should be non-zero.
 * @param error An OCString describing the error.
 * @return The unit or NULL if unit with derived symbol is not valid.
 */
SIUnitRef SIUnitForSymbol(OCStringRef symbol, double *unit_multiplier, OCStringRef *error);

/**
 * @brief Create an array of units for quantity, if valid.
 * @param quantity The quantity.
 * @return An array of units or NULL if no units for quantity are found.
 */
OCArrayRef SIUnitCreateArrayForQuantity(OCStringRef quantity);

/**
 * @brief Returns a OCDictionary with units with a given dimensionality.
 * @param theDimensionality The dimensionality.
 * @return A dictionary with unit symbols as keys, and unit objects as values.
 */
OCDictionaryRef SIUnitCreateDictionaryOfUnitsWithDimensionality(SIDimensionalityRef theDimensionality);

/**
 * @brief Returns a OCDictionary with units with the same reduced dimensionality.
 * @param theDimensionality The dimensionality.
 * @return A dictionary with unit symbols as keys, and unit objects as values.
 */
OCDictionaryRef SIUnitCreateDictionaryOfUnitsWithSameReducedDimensionality(SIDimensionalityRef theDimensionality);

/**
 * @brief Calculates the conversion factor between units of the same dimensionality.
 * @param initialUnit The initial unit.
 * @param finalUnit The final unit.
 * @return The conversion factor to convert from initialUnit to finalUnit.
 * @details When working with multipliers from SIUnitForParsedSymbol, you often need to combine them with
 * this conversion factor. For example, if you parse a non-SI unit (like "lb") and want to convert it to 
 * a standard SI unit (like "kg"):
 * 
 * ```
 * double multiplier = 1.0;  // Initialize to 1.0
 * SIUnitRef unit_lb = SIUnitForParsedSymbol(STR("lb"), &multiplier, &err);
 * SIUnitRef unit_kg = SIUnitForUnderivedSymbol(STR("kg"));
 * 
 * // Calculate the full conversion from lb to kg
 * double conversion = SIUnitConversion(unit_lb, unit_kg);
 * double total_conversion = multiplier * conversion;  // Total lb to kg conversion factor
 * ```
 * 
 * The total_conversion will be 0.45359237 (the standard lb to kg conversion factor).
 * 
 * @note This function requires that units have the same dimensionality or are dimensionally compatible. 
 * It will not work for units with different underlying dimensions.
 */
double SIUnitConversion(SIUnitRef initialUnit, SIUnitRef finalUnit);

#pragma mark Strings and Archiving

/**
 * @brief Shows a short descriptor of the unit.
 * @param theUnit The unit.
 */
void SIUnitShow(SIUnitRef theUnit);

/**
 * @brief Shows a long descriptor of the unit.
 * @param theUnit The unit.
 */
void SIUnitShowFull(SIUnitRef theUnit);

#pragma mark Library

/**
 * @brief Returns a boolean indicating if library uses imperial volumes for gallon, quart, pint, fluid ounce, tablespoon, and teaspoon.
 * @return The boolean result.
 * @details If true, the US unit symbols are appended with 'US', i.e. galUS, qtUS, ptUS, etc... If false, the imperial symbols are appended with 'UK', i.e. galUK, qtUK, ptUK, etc...
 */
bool SIUnitsLibraryImperialVolumes(void);

/**
 * @brief Sets the library to use imperial volumes for gallon, quart, pint, fluid ounce, tablespoon, and teaspoon.
 * @param value Boolean value indicating whether to use imperial volumes.
 * @details If true, the US unit symbols are appended with 'US', i.e. galUS, qtUS, ptUS, etc... If false, the imperial symbols are appended with 'UK', i.e. galUK, qtUK, ptUK, etc...
 */
void SIUnitsLibrarySetImperialVolumes(bool value);

/**
 * @brief Gets a copy of the library of units.
 * @return A OCMutableDictionary containing the units.
 */
OCMutableDictionaryRef SIUnitGetLibrary(void);

/**
 * @brief Sets the library of units.
 * @param newUnitsLibrary The new units library.
 */
void SIUnitSetLibrary(OCMutableDictionaryRef newUnitsLibrary);

/**
 * @brief Shows a description of every unit in the library.
 */
void SIUnitLibraryShow(void);

/**
 * @brief Shows a full description of every unit in the library.
 */
void SIUnitLibraryShowFull(void);

/**
 * @brief Gets units sorted by name length.
 * @return An OCArray containing the units sorted by name length.
 */
OCArrayRef SIUnitGetUnitsSortedByNameLength(void);

/**
 * @brief Creates an array of root units in alphabetical order according to root unit name.
 * @return An OCArray containing the root units.
 */
OCArrayRef SIUnitCreateArrayOfRootUnits(void);

/**
 * @brief Creates an array of root units for a quantity.
 * @param quantity The quantity.
 * @param error An OCString describing the error.
 * @return An OCArray containing the root units.
 */
OCArrayRef SIUnitCreateArrayOfRootUnitsForQuantity(OCStringRef quantity, OCStringRef *error);

// Expose cleanup function for external shutdown
void cleanupUnitsLibraries(void);

#endif /* SIUnit_h */
