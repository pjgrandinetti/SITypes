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

/*!
 @header SIUnit
 @copyright PhySy Ltd
 @discussion
 SIUnit represents the unit of a physical quantity.  An important characteristic of physical
 quantities is that any given physical quantity can be derived from other physical quantities
 through physical laws.  For example, the physical quantity of speed is calculated as a ratio
 of distance traveled to time elapsed.   The volume of a box is calculated as the product of
 three quantities of length: i.e., height, width, and depth of the box.  Any physical quantity
 can always be related back through physical laws to a smaller set of reference physical
 quantities.   In fact, as the laws of physics become unified it has been argued that this
 smaller set can be reduced to simply the Planck length and the speed of light.    At the level
 of theory employed by most scientists and engineers, however, there is a practical agreement
 that seven physical quantities should serve as fundamental reference quantities from which
 all other physical quantities can be derived.  These reference quantities are
 (1) length,
 (2) mass,
 (3) time,
 (4) electric current,
 (5) thermodynamic temperature (the absolute measure of temperature)
 (6) amount of substance,
 (7) luminous intensity.
 
 @unsorted
 @copyright PhySy
 */


/*!
 @enum SIPrefix
 @constant kSIPrefixYocto SI Prefix yocto
 @constant kSIPrefixZepto SI Prefix zepto
 @constant kSIPrefixAtto SI Prefix atto
 @constant kSIPrefixFemto SI Prefix femto
 @constant kSIPrefixPico SI Prefix pico
 @constant kSIPrefixNano SI Prefix nano
 @constant kSIPrefixMicro SI Prefix micro
 @constant kSIPrefixMilli SI Prefix milli
 @constant kSIPrefixCenti SI Prefix centi
 @constant kSIPrefixDeci SI Prefix deci
 @constant kSIPrefixNone no SI Prefix
 @constant kSIPrefixDeca SI Prefix deca
 @constant kSIPrefixHecto SI Prefix hecto
 @constant kSIPrefixKilo SI Prefix kilo
 @constant kSIPrefixMega SI Prefix mega
 @constant kSIPrefixGiga SI Prefix giga
 @constant kSIPrefixTera SI Prefix tera
 @constant kSIPrefixPeta SI Prefix peta
 @constant kSIPrefixExa SI Prefix exa
 @constant kSIPrefixZetta SI Prefix zetta
 @constant kSIPrefixYotta SI Prefix yotta
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

/*!
 @typedef SIUnitRef
 This is the type of a reference to immutable SIUnit.
 */
typedef const struct __SIUnit * SIUnitRef;


#pragma mark Accessors

/*!
 @functiongroup Accessors
 */

OCTypeID SIUnitGetTypeID(void);

/*!
 @function SIUnitGetDimensionality
 @abstract Gets unit's dimensionality.
 @param theUnit The unit.
 @result the dimensionality of a unit
 */
SIDimensionalityRef SIUnitGetDimensionality(SIUnitRef theUnit);

/*
 @function SIUnitGetNumeratorPrefixAtIndex
 @abstract Gets unit's numerator root unit prefix for the dimension at index.
 @param theUnit The unit.
 @result the integer exponent associated with an SI prefix
 @discussion root unit for length, mass, time, current, temperature, amount, and luminous intensity are
 assigned to index constants kPSLengthIndex, kPSMassIndex, kPSTimeIndex, kPSCurrentIndex,  kPSTemperatureIndex,
 kPSAmountIndex, kPSLuminousIntensityIndex, respectively.
 */
SIPrefix SIUnitGetNumeratorPrefixAtIndex(SIUnitRef theUnit, const uint8_t index);

/*
 @function SIUnitGetDenominatorPrefixAtIndex
 @abstract Gets unit's denominator root unit prefix for the dimension at index.
 @param theUnit The unit.
 @result the integer exponent associated with an SI prefix
 @discussion root unit for length, mass, time, current, temperature, amount, and luminous intensity are
 assigned to index constants kPSLengthIndex, kPSMassIndex, kPSTimeIndex, kPSCurrentIndex,  kPSTemperatureIndex,
 kPSAmountIndex, kPSLuminousIntensityIndex, respectively.
 */
SIPrefix SIUnitGetDenominatorPrefixAtIndex(SIUnitRef theUnit, const uint8_t index);

/*!
 @function SIUnitCopyRootName
 @abstract Gets the root name of unit.
 @param theUnit The unit.
 @result string containing the base name, or NULL.
 */
OCStringRef SIUnitCopyRootName(SIUnitRef theUnit);

/*!
 @function SIUnitCopyRootPluralName
 @abstract Gets the plural root name of unit.
 @param theUnit The unit.
 @result string containing the plural base name, or NULL.
 */
OCStringRef SIUnitCopyRootPluralName(SIUnitRef theUnit);

/*!
 @function SIUnitCopyRootSymbol
 @abstract Gets the root symbol of unit.
 @param theUnit The unit.
 @result string containing the base symbol, or NULL
 */
OCStringRef SIUnitCopyRootSymbol(SIUnitRef theUnit);


/*!
 @function SIUnitCopyRootSymbolPrefix
 @abstract returns whether an SI prefix can be used with root symbol.
 @param theUnit The unit.
 @result true or false.
 */
bool SIUnitAllowsSIPrefix(SIUnitRef theUnit);

/*!
 @function SIUnitCopyRootSymbolPrefix
 @abstract Gets the symbol prefix of unit.
 @param theUnit The unit.
 @result the integer exponent associated with SI prefix
 */
SIPrefix SIUnitCopyRootSymbolPrefix(SIUnitRef theUnit);

/*!
 @function SIUnitGetIsSpecialSISymbol
 @abstract returns whether root symbol is a special SI unit symbol.
 @param theUnit The unit.
 @result true or false .
 */
bool SIUnitGetIsSpecialSISymbol(SIUnitRef theUnit);


#pragma mark Tests
/*!
 @functiongroup Tests
 */

/*!
 @function SIUnitIsCoherentSIBaseUnit
 @abstract Determines if unit is one of the seven coherent base units.
 @param theUnit The unit.
 @result true or false.
 */
bool SIUnitIsCoherentSIBaseUnit(SIUnitRef theUnit);

/*!
 @function SIUnitIsSIBaseRootUnit
 @abstract Determines if unit is one of the seven base root units.
 @param theUnit The unit.
 @result true or false.
 */
bool SIUnitIsSIBaseRootUnit(SIUnitRef theUnit);

/*!
 @function SIUnitIsSIBaseUnit
 @abstract Determines if unit is one of the prefixed seven base units.
 @param theUnit The unit.
 @result true or false.
 */
bool SIUnitIsSIBaseUnit(SIUnitRef theUnit);

/*!
 @function SIUnitIsCoherentDerivedUnit
 @abstract Determines if unit is one of the SI coherent derived units.
 @param theUnit The unit.
 @result true or false.
 */
bool SIUnitIsCoherentDerivedUnit(SIUnitRef theUnit);

/*!
 @function SIUnitIsDimensionlessAndUnderived
 @abstract Determines if unit is the dimensionless and underived unit.
 @param theUnit The unit.
 @result true or false.
 */
bool SIUnitIsDimensionlessAndUnderived(SIUnitRef theUnit);

/*!
 @function SIUnitIsDimensionless
 @abstract Determines if unit is the dimensionless unit.
 @param theUnit The unit.
 @result true or false.
 */
bool SIUnitIsDimensionless(SIUnitRef theUnit);

/*!
 @function SIUnitAreEquivalentUnits
 @abstract Determines if units are equivalent.
 @param theUnit1 The unit.
 @param theUnit2 The unit.
 @result true or false.
 @discussion Determines if units are equivalent.  If true,
 these two units can be substituted for each other without modifying
 the quantity's numerical value.
 */
bool SIUnitAreEquivalentUnits(SIUnitRef theUnit1, SIUnitRef theUnit2);

/*!
 @function SIUnitEqual
 @abstract Determines if the two units are equal.
 @param theUnit1 The first unit.
 @param theUnit2 The second unit.
 @result true or false.
 */
bool SIUnitEqual(SIUnitRef theUnit1,SIUnitRef theUnit2);

#pragma mark Operations
/*!
 @functiongroup Operations
 */

/*!
 @function SIUnitDimensionlessAndUnderived
 @abstract returns the dimensionless and underived unit.
 @result The unit.
 */
SIUnitRef SIUnitDimensionlessAndUnderived(void);

/*!
 @function SIUnitCreateArrayOfEquivalentUnits
 @abstract Creates an array of units that are equivalent to a unit
 @param theUnit The equivalent unit.
 @result a OCArray containing the results.
 */
OCArrayRef SIUnitCreateArrayOfEquivalentUnits(SIUnitRef theUnit);

/*!
 @function SIUnitFindEquivalentUnitWithShortestSymbol
 @abstract Finds the unit with the shortest symbol
 @param theUnit The equivalent unit.
 @result the unit with the shortest symbol.
 */
SIUnitRef SIUnitFindEquivalentUnitWithShortestSymbol(SIUnitRef theUnit);

/*!
 @function SIUnitCreateArrayOfUnitsForQuantity
 @abstract Creates an array of units for a quantity
 @param quantity The quantity.
 @result a OCArray containing the results.
 */
OCArrayRef SIUnitCreateArrayOfUnitsForQuantity(OCStringRef quantity);

/*!
 @function SIUnitCreateArrayOfUnitsForDimensionality
 @abstract Creates an array of units for a dimensionality
 @param theDimensionality The dimensionality.
 @result a OCArray containing the results.
 */
OCArrayRef SIUnitCreateArrayOfUnitsForDimensionality(SIDimensionalityRef theDimensionality);

/*!
 @function SIUnitCreateArrayOfUnitsForSameReducedDimensionality
 @abstract Creates an array of units having same reduced dimensionality
 @param theDimensionality The dimensionality.
 @result a OCArray containing the results.
 */
OCArrayRef SIUnitCreateArrayOfUnitsForSameReducedDimensionality(SIDimensionalityRef theDimensionality);

/*!
 @function SIUnitCreateArrayOfConversionUnits
 @abstract Creates an array of units that have same dimensionality as unit
 @param theUnit The unit.
 @result a OCArray containing the results.
 */
OCArrayRef SIUnitCreateArrayOfConversionUnits(SIUnitRef theUnit);

/*!
 @function SIUnitGetScaleNonSIToCoherentSI
 @abstract Gets the scaling from the Non-SI root unit
 to coherent derived SI unit with the same dimensionality.
 @param theUnit The unit.
 @result scaling factor.
 */
double SIUnitGetScaleNonSIToCoherentSI(SIUnitRef theUnit);

/*!
 @function SIUnitCreateName
 @abstract Creates a OCStringRef with the name of unit
 from its root_name and prefix.
 @param theUnit The unit.
 @result a OCStringRef with the name.
 */
OCStringRef SIUnitCreateName(SIUnitRef theUnit);

/*!
 @function SIUnitCreatePluralName
 @abstract Creates a OCStringRef with the plural name of unit
 from its plural root name and prefix.
 @param theUnit The unit.
 @result a OCStringRef with the plural name.
 */
OCStringRef SIUnitCreatePluralName(SIUnitRef theUnit);

/*!
 @function SIUnitCopySymbol
 @abstract Gets the OCStringRef with the Symbol of unit
 from its root symbol and prefix.
 @param theUnit The unit.
 @result a OCStringRef with the symbol.
 */
OCStringRef SIUnitCopySymbol(SIUnitRef theUnit);

/*!
 @function SIUnitScaleToCoherentSIUnit
 @abstract Calculates the conversion scaling factor.
 @param theUnit The unit.
 @result numerical value scaling factor.
 @discussion Calculates the scaling factor needed to transform the
 numerical value of quantity with this unit into the appropriate
 numerical value for the coherent si unit of the same dimensionality.
 */
double SIUnitScaleToCoherentSIUnit(SIUnitRef theUnit);

/*!
 @function SIUnitCreateDerivedSymbol
 @abstract Creates a OCStringRef with the unit's derived symbol
 @param theUnit The unit.
 @result a OCStringRef with the derived unit symbol.
 */
OCStringRef SIUnitCreateDerivedSymbol(SIUnitRef theUnit);

/*!
 @function SIUnitFindCoherentSIUnitWithDimensionality
 @abstract Returns a coherent SI unit with given dimensionality
 @param theDimensionality The dimensionality.
 @result the unit.
 */
SIUnitRef SIUnitFindCoherentSIUnitWithDimensionality(SIDimensionalityRef theDimensionality);

/*!
 @function SIUnitFindCoherentSIUnit
 @abstract Returns a coherent SI unit with the dimensionality of input unit
 @param input The input unit.
 @param unit_multiplier pointer to a double float variable for the unit_multiplier.
 This unit_multiplier will be scaled to make the new quantity's numerical value consistent with the new unit.
 Thus, the initial value for the unit_multiplier should be non-zero.
 @result the new unit.
 */
SIUnitRef SIUnitFindCoherentSIUnit(SIUnitRef input, double *unit_multiplier);

/*!
 @function SIUnitByTakingNthRoot
 @abstract Returns the unit by dividing the numerator and denominator exponents by an integer.
 @param input The unit.
 @param root the integer root.
 @param unit_multiplier pointer to a double float variable for the unit_multiplier.
 This unit_multiplier will be scaled to make the new quantity's numerical value consistent with the new unit.
 Thus, the initial value for the unit_multiplier should be non-zero.
 @param error an OCString describing the error.
 @result the nth root unit
 @discussion The numerator and denominator exponents in a valid unit can only take on integer values.
 If this function cannot return a valid unit then it will return NULL */
SIUnitRef SIUnitByTakingNthRoot(SIUnitRef input, uint8_t root, double *unit_multiplier, OCStringRef *error);

/*!
 @function SIUnitByReducing
 @abstract Returns the unit by reducing the numerator and denominator exponents to their lowest integer values.
 @param theUnit The unit.
 @param unit_multiplier pointer to a double float variable for the unit_multiplier.
 This unit_multiplier will be scaled to make the new quantity's numerical value consistent with the new unit.
 Thus, the initial value for the unit_multiplier should be non-zero.
 @result the reduced unit.
 */
SIUnitRef SIUnitByReducing(SIUnitRef theUnit, double *unit_multiplier);

/*!
 @function SIUnitByMultiplying
 @abstract Returns the unit obtained by multiplying two units and reducing the dimensionality numerator and denominator exponents to their lowest integer values.
 @param theUnit1 The first unit.
 @param theUnit2 The second unit.
 @param unit_multiplier pointer to a double float variable for the unit_multiplier.
 This unit_multiplier will be scaled to make the new quantity's numerical value consistent with the new unit.
 Thus, the initial value for the unit_multiplier should be non-zero.
 @result the new unit.
 */
SIUnitRef SIUnitByMultiplying(SIUnitRef theUnit1, SIUnitRef theUnit2, double *unit_multiplier, OCStringRef *error);

/*!
 @function SIUnitByMultiplyingWithoutReducing
 @abstract Returns the unit obtained by multiplying two units.
 @param theUnit1 The first unit.
 @param theUnit2 The second unit.
 @param unit_multiplier pointer to a double float variable for the unit_multiplier.
 This unit_multiplier will be scaled to make the new quantity's numerical value consistent with the new unit.
 Thus, the initial value for the unit_multiplier should be non-zero.
 @result the new unit.
 */
SIUnitRef SIUnitByMultiplyingWithoutReducing(SIUnitRef theUnit1, SIUnitRef theUnit2, double *unit_multiplier, OCStringRef *error);

/*!
 @function SIUnitByDividing
 @abstract Returns the unit obtained by dividing two units and reducing the dimensionality numerator and denominator exponents to their lowest integer values.
 @param theUnit1 The first unit.
 @param theUnit2 The second unit.
 @param unit_multiplier pointer to a double float variable for the unit_multiplier.
 This unit_multiplier will be scaled to make the new quantity's numerical value consistent with the new unit.
 Thus, the initial value for the unit_multiplier should be non-zero.
 @result the new unit.
 */
SIUnitRef SIUnitByDividing(SIUnitRef theUnit1, SIUnitRef theUnit2, double *unit_multiplier);

/*!
 @function SIUnitByDividingWithoutReducing
 @abstract Returns the unit obtained by dividing two units.
 @param theUnit1 The first unit.
 @param theUnit2 The second unit.
 @param unit_multiplier pointer to a double float variable for the unit_multiplier.
 This unit_multiplier will be scaled to make the new quantity's numerical value consistent with the new unit.
 Thus, the initial value for the unit_multiplier should be non-zero.
 @result the new unit.
 */
SIUnitRef SIUnitByDividingWithoutReducing(SIUnitRef theUnit1, SIUnitRef theUnit2, double *unit_multiplier);

/*!
 @function SIUnitByRaisingToAPower
 @abstract Returns the unit obtained by raising a unit to a power and reducing the dimensionality numerator and denominator exponents to their lowest integer values.
 @param input The unit.
 @param unit_multiplier pointer to a double float variable for the unit_multiplier.
 This unit_multiplier will be scaled to make the new quantity's numerical value consistent with the new unit.
 Thus, the initial value for the unit_multiplier should be non-zero.
 @result the new unit.
 */
SIUnitRef SIUnitByRaisingToAPower(SIUnitRef input, double power,double *unit_multiplier, OCStringRef *error);

/*!
 @function SIUnitByRaisingToAPowerWithoutReducing
 @abstract Returns the unit obtained by raising a unit to a power.
 @param input The unit.
 @param unit_multiplier pointer to a double float variable for the unit_multiplier.
 This unit_multiplier will be scaled to make the new quantity's numerical value consistent with the new unit.
 Thus, the initial value for the unit_multiplier should be non-zero.
 @param error a OCStringRef.
 @result the new unit.
 */
SIUnitRef SIUnitByRaisingToAPowerWithoutReducing(SIUnitRef input, double power, double *unit_multiplier, OCStringRef *error);

/*!
 @function SIUnitFindWithName
 @abstract Returns the unit with a specific name, if known.
 @param input The name.
 @result the unit or NULL if unit with name is not found.
 */
SIUnitRef SIUnitFindWithName(OCStringRef input);

/*
 @function SIUnitForUnderivedSymbol
 @abstract Returns the unit with an underived symbol, if known.
 @param input The symbol.
 @result the unit or NULL if unit with symbol is not found.
 */
SIUnitRef SIUnitForUnderivedSymbol(OCStringRef symbol);

/*!
 @function SIUnitForSymbol
 @abstract Returns the unit with symbol, if valid.
 @param symbol The derived symbol.
 @param unit_multiplier pointer to a double float variable for the unit_multiplier.
 This unit_multiplier will be scaled to make the new quantity's numerical value consistent with the new unit.
 Thus, the initial value for the unit_multiplier should be non-zero.
 @param error a OCStringRef.
 @result the unit or NULL if unit with derived symbol is not valid.
 */
SIUnitRef SIUnitForSymbol(OCStringRef symbol, double *unit_multiplier, OCStringRef *error);

/*!
 @function SIUnitCreateArrayForQuantity
 @abstract Create an array of units for quantity, if valid.
 @param quantity The quantity.
 @result an array of units or NULL if no units for quantity are found.
 */
OCArrayRef SIUnitCreateArrayForQuantity(OCStringRef quantity);

/*!
 @function SIUnitCreateDictionaryOfUnitsWithDimensionality
 @abstract Returns a OCDictionary with units with a given dimensionality.
 @param theDimensionality The dimensionality.
 @result a dictionary with unit symbols as keys, and unit objects as values.
 */
OCDictionaryRef SIUnitCreateDictionaryOfUnitsWithDimensionality(SIDimensionalityRef theDimensionality);

/*!
 @function SIUnitCreateDictionaryOfUnitsWithSameReducedDimensionality
 @abstract Returns a OCDictionary with units with the same reduced dimensionality.
 @param theDimensionality The dimensionality.
 @result a dictionary with unit symbols as keys, and unit objects as values.
 */
OCDictionaryRef SIUnitCreateDictionaryOfUnitsWithSameReducedDimensionality(SIDimensionalityRef theDimensionality);

/*!
 @function SIUnitConversion
 @abstract Calculates the conversion factor between units of the same dimensionality.
 @param initialUnit The initial unit.
 @param finalUnit The final value.
 @result the conversion factor.
 */
double SIUnitConversion(SIUnitRef initialUnit, SIUnitRef finalUnit);

#pragma mark Strings and Archiving
/*!
 @functiongroup Strings and Archiving
 */

/*!
 @function SIUnitShow
 @abstract Shows a short descriptor of the unit
 @param theUnit The unit.
 */
void SIUnitShow(SIUnitRef theUnit);

/*!
 @function SIUnitShowFull
 @abstract Shows a long descriptor of the unit
 @param theUnit The unit.
 */
void SIUnitShowFull(SIUnitRef theUnit);

#pragma mark Library
/*!
 @function SIUnitsLibraryImperialVolumes
 @abstract Returns a boolean indicating if library uses imperial volumes for gallon, quart, pint, fluid ounce, tablespoon, and teaspoon
 If true, the US unit symbols are appended with 'US', i.e. galUS, qtUS, ptUS, etc...
 if false, the imperial symbols are appended with 'UK', i.e. galUK, qtUK, ptUK, etc...
 @result the boolean result
 */
bool SIUnitsLibraryImperialVolumes(void);

/*!
 @function SIUnitsLibrarySetImperialVolumes
 @abstract Sets the library to use imperial volumes for gallon, quart, pint, fluid ounce, tablespoon, and teaspoon
 If true, the US unit symbols are appended with 'US', i.e. galUS, qtUS, ptUS, etc...
 if false, the imperial symbols are appended with 'UK', i.e. galUK, qtUK, ptUK, etc...
 */
void SIUnitsLibrarySetImperialVolumes(bool value);

/*!
 @functiongroup Library
 */

/*
 @function SIUnitGetLibrary
 @abstract Gets a copy of the library of units
 @result a OCMutableDictionary containing the units.
 */
OCMutableDictionaryRef SIUnitGetLibrary(void);

void SIUnitSetLibrary(OCMutableDictionaryRef newUnitsLibrary);

/*
 @function SIUnitLibraryShow
 @abstract Shows a description every unit in the library
 */
void SIUnitLibraryShow(void);

/*
 @function SIUnitLibraryShowFull
 @abstract Shows a full description every unit in the library
 */
void SIUnitLibraryShowFull(void);


OCArrayRef SIUnitGetUnitsSortedByNameLength(void);

/*
 @function SIUnitCreateArrayOfRootUnits
 @abstract Creates an array of root units in alphabetical order according to root unit name.
 */
OCArrayRef SIUnitCreateArrayOfRootUnits(void);
OCArrayRef SIUnitCreateArrayOfRootUnitsForQuantity(OCStringRef quantity, OCStringRef *error);



#endif /* SIUnit_h */
