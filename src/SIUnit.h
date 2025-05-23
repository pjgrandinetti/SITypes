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
 * @brief Returns the unique type identifier for SIUnit objects.
 *
 * Every SIUnit instance is associated with a globally unique OCTypeID. 
 * This identifier can be used for runtime type checking, assertions,
 * or bridging with other OC-based frameworks.
 *
 * @thread_safety Safe to call from any thread.
 *
 * @return The OCTypeID corresponding to the SIUnit “class.”
 *
 * @see OCGetTypeID, OCTypeID
 */
OCTypeID SIUnitGetTypeID(void);

/**
 * @brief Retrieves the physical dimensionality associated with a given SI unit.
 *
 * Each SIUnit encapsulates a scale factor and a dimensionality that describes
 * its underlying physical dimensions (e.g., length, mass, time, etc.). Use this
 * function to obtain the unit’s dimensionality for compatibility checks or
 * further dimensional analysis.
 *
 * @param theUnit
 *   A valid SIUnitRef representing the unit whose dimensionality is requested.
 *   Must not be NULL.
 *
 * @return
 *   An immutable SIDimensionalityRef describing the unit’s dimensions.
 *   This reference is owned by the library and must not be retained, released,
 *   or modified by the caller.
 *
 * @thread_safety
 *   Safe to call from any thread.
 *
 * @see SIDimensionalityGetSymbol, SIDimensionalityHasSameReducedDimensionality
 */
SIDimensionalityRef SIUnitGetDimensionality(SIUnitRef theUnit);

/**
 * @brief Retrieves the SI prefix applied to a unit’s numerator for a given base dimension.
 *
 * In SI units, each base dimension (length, mass, time, electric current,
 * temperature, amount of substance, luminous intensity) may carry a decimal
 * prefix (e.g., kilo-, milli-) that scales its magnitude by a power of ten.
 * This function returns the prefix exponent used in the unit’s numerator
 * term for the specified base dimension index.
 *
 * @param theUnit
 *   A valid SIUnitRef whose numerator prefix you wish to query. Must not be NULL.
 * @param index
 *   One of the SIBaseDimensionIndex constants identifying which base dimension’s
 *   numerator prefix to retrieve:
 *     - kSILengthIndex  
 *     - kSIMassIndex  
 *     - kSITimeIndex  
 *     - kSICurrentIndex  
 *     - kSITemperatureIndex  
 *     - kSIAmountIndex  
 *     - kSILuminousIntensityIndex  
 *
 * @return
 *   A SIPrefix value indicating the power-of-ten exponent applied to the numerator
 *   of the specified base dimension (e.g., SIPrefixKilo == +3, SIPrefixMilli == -3).
 *
 * @details
 *   The returned prefix represents how many powers of ten the raw base unit is
 *   multiplied by in the numerator. For example, for the unit “km” (kilometre),
 *   calling this function with index = kSILengthIndex will return SIPrefixKilo.
 *
 * @thread_safety
 *   Safe to call from any thread provided `theUnit` remains valid.
 *
 * @see SIUnitGetDenominatorPrefixAtIndex, SIPrefix
 */
SIPrefix SIUnitGetNumeratorPrefixAtIndex(SIUnitRef theUnit, uint8_t index);

/**
 * @brief Retrieves the SI prefix applied to a unit’s denominator for a given base dimension.
 *
 * SI units can include decimal prefixes (e.g., milli-, micro-) in the denominator
 * for certain base dimensions. This function returns the prefix exponent used
 * in the unit’s denominator term for the specified base dimension index.
 *
 * @param theUnit
 *   A valid SIUnitRef whose denominator prefix you wish to query. Must not be NULL.
 * @param index
 *   One of the SIBaseDimensionIndex constants identifying which base dimension’s
 *   denominator prefix to retrieve:
 *     - kSILengthIndex  
 *     - kSIMassIndex  
 *     - kSITimeIndex  
 *     - kSICurrentIndex  
 *     - kSITemperatureIndex  
 *     - kSIAmountIndex  
 *     - kSILuminousIntensityIndex  
 *
 * @return
 *   A SIPrefix value indicating the power-of-ten exponent applied to the denominator
 *   of the specified base dimension (e.g., SIPrefixMilli == -3, SIPrefixKilo == +3).
 *
 * @details
 *   The returned prefix represents how many powers of ten the base unit is divided
 *   by in the denominator. For example, for the unit “m/s²”, calling this function
 *   with index = kSITimeIndex will return SIPrefixNone for the numerator and
 *   SIPrefixNone for the denominator of length, but the exponent of the squared second
 *   would be handled in the dimensionality rather than via prefix.
 *
 * @thread_safety
 *   Safe to call from any thread provided `theUnit` remains valid.
 *
 * @see SIUnitGetNumeratorPrefixAtIndex, SIPrefix
 */
SIPrefix SIUnitGetDenominatorPrefixAtIndex(SIUnitRef theUnit, uint8_t index);

/**
 * @brief Creates and returns a copy of the base (singular) name for a unit.
 *
 * The “root name” is the canonical, language-neutral singular name of the unit,
 * before any SI prefixes or pluralization are applied.
 *
 * @param theUnit
 *   A valid SIUnitRef whose base name you wish to retrieve. Must not be NULL.
 *
 * @return
 *   An OCStringRef containing the unit’s singular root name (e.g., "meter"),
 *   or NULL if no base name is defined. The caller owns the returned string
 *   and must release it with OCRelease().
 *
 * @thread_safety
 *   Safe to call from any thread provided `theUnit` remains valid.
 *
 * @see SIUnitCopyRootPluralName, SIUnitCopyRootSymbol
 */
OCStringRef SIUnitCopyRootName(SIUnitRef theUnit);

/**
 * @brief Creates and returns a copy of the plural base name for a unit.
 *
 * The “root plural name” is the canonical, language-neutral plural form of the
 * unit’s base name, before any SI prefixes are applied.
 *
 * @param theUnit
 *   A valid SIUnitRef whose plural base name you wish to retrieve. Must not be NULL.
 *
 * @return
 *   An OCStringRef containing the unit’s plural root name (e.g., "meters"),
 *   or NULL if no plural base name is defined. The caller owns the returned string
 *   and must release it with OCRelease().
 *
 * @thread_safety
 *   Safe to call from any thread provided `theUnit` remains valid.
 *
 * @see SIUnitCopyRootName, SIUnitCopyRootSymbol
 */
OCStringRef SIUnitCopyRootPluralName(SIUnitRef theUnit);

/**
 * @brief Creates and returns a copy of the base symbol for a unit.
 *
 * The “root symbol” is the canonical abbreviation or symbol for the unit,
 * before any SI prefixes or exponentiation are applied.
 *
 * @param theUnit
 *   A valid SIUnitRef whose base symbol you wish to retrieve. Must not be NULL.
 *
 * @return
 *   An OCStringRef containing the unit’s symbol (e.g., "m"), or NULL if no
 *   base symbol is defined. The caller owns the returned string and must
 *   release it with OCRelease().
 *
 * @thread_safety
 *   Safe to call from any thread provided `theUnit` remains valid.
 *
 * @see SIUnitCopyRootName, SIUnitCopyRootPluralName
 */
OCStringRef SIUnitCopyRootSymbol(SIUnitRef theUnit);

/**
 * @brief Determines if the unit’s base symbol may be modified by an SI prefix.
 *
 * Some units prohibit standard SI prefixes (e.g., “kg” forbids “mkg” or “gkg”).
 *
 * @param theUnit
 *   A valid SIUnitRef to test. Must not be NULL.
 *
 * @return
 *   true if the unit’s root symbol accepts standard SI prefixes;
 *   false otherwise.
 *
 * @thread_safety
 *   Safe to call concurrently on distinct SIUnitRef objects.
 *
 * @see SIUnitCopyRootSymbolPrefix, SIUnitGetIsSpecialSISymbol
 */
bool SIUnitAllowsSIPrefix(SIUnitRef theUnit);

/**
 * @brief Returns the default SI prefix exponent applied to the unit’s base symbol.
 *
 * Each unit may carry an implicit SI prefix (e.g., “kilogram” has a prefix exponent of 3
 * for kilo). This function retrieves that exponent.
 *
 * @param theUnit
 *   A valid SIUnitRef whose symbol prefix you wish to query. Must not be NULL.
 *
 * @return
 *   A SIPrefix value indicating the power-of-ten exponent for the unit’s inherent prefix
 *   (e.g., 3 for “kilo”), or 0 if no prefix is present.
 *
 * @thread_safety
 *   Safe to call concurrently on distinct SIUnitRef objects.
 *
 * @see SIUnitAllowsSIPrefix, SIUnitGetIsSpecialSISymbol
 */
SIPrefix SIUnitCopyRootSymbolPrefix(SIUnitRef theUnit);

/**
 * @brief Checks if the unit’s root symbol is one of the special-case SI symbols.
 *
 * Certain SI units use non-standard or historic symbols (e.g., “ℓ” for liter). This function
 * identifies those exceptions.
 *
 * @param theUnit
 *   A valid SIUnitRef to test. Must not be NULL.
 *
 * @return
 *   true if the unit’s root symbol is marked as a special SI symbol;
 *   false for ordinary symbols.
 *
 * @thread_safety
 *   Safe to call concurrently on distinct SIUnitRef objects.
 *
 * @see SIUnitAllowsSIPrefix, SIUnitCopyRootSymbolPrefix
 */
bool SIUnitGetIsSpecialSISymbol(SIUnitRef theUnit);

#pragma mark Tests

/**
 * @brief Checks whether a unit is one of the seven SI coherent base units.
 *
 * The coherent base units are the fundamental SI units without prefixes:
 * meter, kilogram, second, ampere, kelvin, mole, and candela.
 *
 * @param theUnit
 *   A valid SIUnitRef to test. Must not be NULL.
 *
 * @return
 *   true if theUnit is a coherent SI base unit; false otherwise.
 *
 * @thread_safety
 *   Safe to call concurrently on distinct SIUnitRef objects.
 *
 * @see SIUnitIsSIBaseRootUnit, SIUnitIsSIBaseUnit
 */
bool SIUnitIsCoherentSIBaseUnit(SIUnitRef theUnit);

/**
 * @brief Determines if a unit is one of the seven base root units (no prefix).
 *
 * Base root units refer to the unprefixed form of the SI base units:
 * m, g (gram), s, A, K, mol, cd.
 *
 * @param theUnit
 *   A valid SIUnitRef to test. Must not be NULL.
 *
 * @return
 *   true if theUnit matches one of the unprefixed base root units; false otherwise.
 *
 * @thread_safety
 *   Safe to call concurrently on distinct SIUnitRef objects.
 *
 * @see SIUnitIsCoherentSIBaseUnit, SIUnitIsSIBaseUnit
 */
bool SIUnitIsSIBaseRootUnit(SIUnitRef theUnit);

/**
 * @brief Determines if a unit is any of the seven SI base units, including prefixed forms.
 *
 * This includes both coherent base units (no prefix) and their standard SI-prefixed variants
 * (e.g., mm, kA, cK, etc.).
 *
 * @param theUnit
 *   A valid SIUnitRef to test. Must not be NULL.
 *
 * @return
 *   true if theUnit is an SI base unit or its prefix variant; false otherwise.
 *
 * @thread_safety
 *   Safe to call concurrently on distinct SIUnitRef objects.
 *
 * @see SIUnitIsSIBaseRootUnit, SIUnitIsCoherentSIBaseUnit
 */
bool SIUnitIsSIBaseUnit(SIUnitRef theUnit);

/**
 * @brief Checks whether a unit is an SI coherent derived unit.
 *
 * Coherent derived units are combinations of base units in which the
 * derived unit is expressed without additional numeric factors (e.g., N, Pa, J).
 *
 * @param theUnit
 *   A valid SIUnitRef to test. Must not be NULL.
 *
 * @return
 *   true if theUnit is a coherent derived unit; false otherwise.
 *
 * @thread_safety
 *   Safe to call concurrently on distinct SIUnitRef objects.
 *
 * @see SIUnitAreEquivalentUnits, SIUnitIsSIBaseUnit
 */
bool SIUnitIsCoherentDerivedUnit(SIUnitRef theUnit);

/**
 * @brief Determines if a unit represents the dimensionless, underived quantity “one”.
 *
 * This is the unit for pure numbers (no physical dimension, no derivation).
 *
 * @param theUnit
 *   A valid SIUnitRef to test. Must not be NULL.
 *
 * @return
 *   true if theUnit is the dimensionless underived unit; false otherwise.
 *
 * @thread_safety
 *   Safe to call concurrently on distinct SIUnitRef objects.
 */
bool SIUnitIsDimensionlessAndUnderived(SIUnitRef theUnit);

/**
 * @brief Determines if a unit is dimensionless (may be derived by cancelling dimensions).
 *
 * Returns true for both the pure “one” unit and dimensionless units obtained
 * by cancelling base dimensions (e.g., rad, sr).
 *
 * @param theUnit
 *   A valid SIUnitRef to test. Must not be NULL.
 *
 * @return
 *   true if theUnit has no net physical dimension; false otherwise.
 *
 * @thread_safety
 *   Safe to call concurrently on distinct SIUnitRef objects.
 *
 * @see SIUnitIsDimensionlessAndUnderived
 */
bool SIUnitIsDimensionless(SIUnitRef theUnit);

/**
 * @brief Tests whether two units are directly interchangeable without scaling.
 *
 * Units are considered equivalent if they represent the same physical quantity
 * and share identical scale factors. For example:
 * - ‘N’ and ‘kg·m/s^2’ are equivalent (same dimension & scale).
 * - ‘m’ and ‘cm’ are not equivalent (same dimension but different scale).
 *
 * @param theUnit1
 *   The first unit to compare. Must not be NULL.
 * @param theUnit2
 *   The second unit to compare. Must not be NULL.
 *
 * @return
 *   true if both units have identical dimensions and scaling; false otherwise.
 *
 * @details
 * Use this when you need to know if two units can replace each other without
 * modifying any numerical value. To compare only dimensionality (ignoring scale),
 * see SIDimensionalityHasSameReducedDimensionality().
 *
 * @thread_safety
 *   Safe to call concurrently on distinct SIUnitRef objects.
 *
 * @see SIUnitEqual, SIDimensionalityEqual, SIDimensionalityHasSameReducedDimensionality
 */
bool SIUnitAreEquivalentUnits(SIUnitRef theUnit1, SIUnitRef theUnit2);

/**
 * @brief Compares two units for full equivalence (dimension, scale, and prefix).
 *
 * Returns true if both units have the same underlying symbol, prefix, and
 * dimensionality representation.
 *
 * @param theUnit1
 *   The first unit to compare. Must not be NULL.
 * @param theUnit2
 *   The second unit to compare. Must not be NULL.
 *
 * @return
 *   true if theUnit1 and theUnit2 are identical in every aspect; false otherwise.
 *
 * @thread_safety
 *   Safe to call concurrently on distinct SIUnitRef objects.
 *
 * @see SIUnitAreEquivalentUnits
 */
bool SIUnitEqual(SIUnitRef theUnit1, SIUnitRef theUnit2);


#pragma mark Operations

/**
 * @brief   Retrieves the special SI unit that represents a pure, dimensionless quantity.
 *
 * @return  A non-owned SIUnitRef for the dimensionless, underived unit (e.g., “1”).
 *          Do not release this reference.
 */
SIUnitRef SIUnitDimensionlessAndUnderived(void);

/**
 * @brief   Builds a list of all units that are strictly equivalent to a given unit.
 *
 * @param   theUnit
 *          A reference to the unit whose equivalents are to be found. Must not be NULL.
 *
 * @return  An OCArrayRef you own, containing non-owned SIUnitRef values for every
 *          unit exactly equivalent in dimensionality and scale to @p theUnit.
 *          You must release the returned array when finished. Returns NULL if none found.
 */
OCArrayRef SIUnitCreateArrayOfEquivalentUnits(SIUnitRef theUnit);

/**
 * @brief   Finds, among all units equivalent to a given unit, the one with the shortest symbol.
 *
 * @param   theUnit
 *          A reference to the unit whose equivalents are to be considered. Must not be NULL.
 *
 * @return  A non-owned SIUnitRef for the equivalent unit with the shortest symbol.
 *          Returns the input unit itself if it has the shortest symbol. Do not release this reference.
 */
SIUnitRef SIUnitFindEquivalentUnitWithShortestSymbol(SIUnitRef theUnit);

/**
 * @brief   Lists all units applicable to a particular physical quantity.
 *
 * @param   quantity
 *          An OCStringRef naming the quantity (e.g., kSIQuantityPressure). Must not be NULL.
 *
 * @return  An OCArrayRef you own containing non-owned SIUnitRef values for each unit
 *          defined for @p quantity. You must release the returned array when finished.
 *          Returns NULL if the quantity is unrecognized or has no associated units.
 */
OCArrayRef SIUnitCreateArrayOfUnitsForQuantity(OCStringRef quantity);
/**
 * @brief   Gathers all units matching a specific physical dimensionality.
 *
 * @param   theDimensionality
 *          A non-owned SIDimensionalityRef describing the target dimensionality. Must not be NULL.
 *
 * @return  An OCArrayRef you own, containing non-owned SIUnitRef values for each unit
 *          whose dimensionality exactly equals @p theDimensionality. You must release
 *          the returned array when finished. Returns NULL if no units match.
 */
OCArrayRef SIUnitCreateArrayOfUnitsForDimensionality(SIDimensionalityRef theDimensionality);

/**
 * @brief   Gathers all units with the same reduced dimensionality as the given dimensionality.
 *
 * @param   theDimensionality
 *          A non-owned SIDimensionalityRef describing the target reduced dimensionality. Must not be NULL.
 *
 * @return  An OCArrayRef you own, containing non-owned SIUnitRef values for each unit
 *          whose net (reduced) exponents match @p theDimensionality. You must release
 *          the returned array when finished. Returns NULL if no units match.
 */
OCArrayRef SIUnitCreateArrayOfUnitsForSameReducedDimensionality(SIDimensionalityRef theDimensionality);

/**
 * @brief   Enumerates all units sharing the same dimensionality as a given unit.
 *
 * @param   theUnit
 *          A non-owned SIUnitRef whose dimensionality is used for matching. Must not be NULL.
 *
 * @return  An OCArrayRef you own, containing non-owned SIUnitRef values for each unit
 *          dimensionally equivalent to @p theUnit. You must release the returned array
 *          when finished. Returns NULL if no matching units are found.
 */
OCArrayRef SIUnitCreateArrayOfConversionUnits(SIUnitRef theUnit);

/**
 * @brief   Retrieves the numeric scale factor needed to convert a non-SI root unit into
 *          the corresponding coherent derived SI unit of identical dimensionality.
 *
 * @param   theUnit
 *          A non-owned SIUnitRef representing a non-SI root unit (e.g., "in", "lb"). Must not be NULL.
 *
 * @return  A double precision factor F such that:
 *             value_in_SI = value_in_non_SI × F
 *          Use this factor to convert the numerical value of @p theUnit into its
 *          equivalent in the coherent SI unit.
 */
double SIUnitGetScaleNonSIToCoherentSI(SIUnitRef theUnit);

/**
 * @brief   Builds the human‐readable name of a unit by combining its root name and prefix.
 *
 * @param   theUnit
 *          A non‐owned SIUnitRef whose name is to be constructed. Must not be NULL.
 *
 * @return  An OCStringRef you own, containing the unit’s full name (e.g., “kilogram”, “millisecond”).
 *          You must release the returned string when finished.
 */
OCStringRef SIUnitCreateName(SIUnitRef theUnit);

/**
 * @brief   Builds the plural human‐readable name of a unit by combining its plural root name and prefix.
 *
 * @param   theUnit
 *          A non‐owned SIUnitRef whose plural name is to be constructed. Must not be NULL.
 *
 * @return  An OCStringRef you own, containing the unit’s plural name (e.g., “kilograms”, “milliseconds”).
 *          You must release the returned string when finished.
 */
OCStringRef SIUnitCreatePluralName(SIUnitRef theUnit);

/**
 * @brief   Retrieves the symbol of a unit, including any prefix (e.g., “kg”, “ms”).
 *
 * @param   theUnit
 *          A non‐owned SIUnitRef whose symbol is to be retrieved. Must not be NULL.
 *
 * @return  An OCStringRef you own, containing the unit’s symbol. You must release
 *          the returned string when finished.
 */
OCStringRef SIUnitCopySymbol(SIUnitRef theUnit);

/**
 * @brief   Computes the multiplier needed to convert a value in this unit into the equivalent
 *          numerical value for the coherent SI unit of the same dimensionality.
 *
 * @param   theUnit
 *          A non‐owned SIUnitRef for which to compute the scale factor. Must not be NULL.
 *
 * @return  A double scale factor F such that:
 *             value_in_SI = value_in_unit × F
 *          Use this to adjust numeric values when converting to the coherent SI unit.
 */
double SIUnitScaleToCoherentSIUnit(SIUnitRef theUnit);

/**
 * @brief   Builds the derived‐unit symbol by combining numerator and denominator parts
 *          (e.g., “m·kg/s²” as “m·kg/s²”).
 *
 * @param   theUnit
 *          A non‐owned SIUnitRef whose derived symbol is to be constructed. Must not be NULL.
 *
 * @return  An OCStringRef you own, containing the unit’s full derived symbol.
 *          You must release the returned string when finished.
 */
OCStringRef SIUnitCreateDerivedSymbol(SIUnitRef theUnit);

/**
 * @brief   Finds a coherent SI unit whose dimensionality exactly matches the given dimensionality.
 *
 * @param   theDimensionality
 *          A non‐owned SIDimensionalityRef describing the required dimensionality. Must not be NULL.
 *
 * @return  A non‐owned SIUnitRef for the matching coherent SI unit (e.g., “m” for length,
 *          “kg·m/s²” for force). Returns NULL if no such unit exists. The caller does not own
 *          the returned reference.
 */
SIUnitRef SIUnitFindCoherentSIUnitWithDimensionality(SIDimensionalityRef theDimensionality);

/**
 * @brief   Finds the coherent SI unit equivalent to an arbitrary unit, adjusting the multiplier
 *          to preserve the numeric value.
 *
 * @param   input
 *          A non‐owned SIUnitRef representing the original unit. Must not be NULL.
 * @param   unit_multiplier
 *          A pointer to a double initialized with the current multiplier (e.g., from parsing).
 *          On return, this value is scaled by the factor needed so that:
 *             original_value × (*unit_multiplier) remains consistent under the new unit.
 *
 * @return  A non‐owned SIUnitRef for the coherent SI unit matching @p input’s dimensionality,
 *          or NULL on error. The caller does not own the returned reference.
 */
SIUnitRef SIUnitFindCoherentSIUnit(SIUnitRef input, double *unit_multiplier);


/**
 * @brief   Computes the Nth‐root of a unit by dividing its dimensional exponents by an integer.
 *
 * @param   input
 *          A non‐owned SIUnitRef representing the original unit. Must not be NULL.
 * @param   root
 *          The integer root to apply (e.g., 2 for square root).
 * @param   unit_multiplier
 *          Pointer to a double initialized to the existing multiplier for this unit.
 *          On return, this value is multiplied by the factor needed so that
 *          the numeric value remains consistent under the new unit.
 * @param   error
 *          Pointer to an OCStringRef; if non‐NULL on entry and an error occurs,
 *          *error will be set to a newly created OCString describing the problem
 *          (e.g., "exponents not divisible"), which the caller must release.
 *
 * @return  A non‐owned SIUnitRef for the Nth‐root unit, or NULL on failure.
 *          The caller does not own the returned reference.
 *
 * @details Each base‐unit exponent must be exactly divisible by @p root. If not,
 *          the function fails, sets @p *error, and returns NULL.
 */
SIUnitRef SIUnitByTakingNthRoot(SIUnitRef input,
                                uint8_t root,
                                double *unit_multiplier,
                                OCStringRef *error);

/**
 * @brief   Reduces a unit’s exponents to lowest integer form (e.g., simplifies m²/s² → (m/s)²).
 *
 * @param   theUnit
 *          A non‐owned SIUnitRef representing the original unit. Must not be NULL.
 * @param   unit_multiplier
 *          Pointer to a double initialized to the existing multiplier for this unit.
 *          On return, this value is multiplied by the factor needed so that
 *          the numeric value remains consistent under the reduced unit.
 *
 * @return  A non‐owned SIUnitRef for the reduced unit.
 *          The caller does not own the returned reference.
 */
SIUnitRef SIUnitByReducing(SIUnitRef theUnit,
                           double *unit_multiplier);

/**
 * @brief   Multiplies two units and then reduces the result to lowest integer exponents.
 *
 * @param   theUnit1
 *          A non‐owned SIUnitRef for the first unit. Must not be NULL.
 * @param   theUnit2
 *          A non‐owned SIUnitRef for the second unit. Must not be NULL.
 * @param   unit_multiplier
 *          Pointer to a double initialized to the existing multiplier for the first unit.
 *          On return, this value is multiplied by the factor needed so that
 *          the numeric value remains consistent under the new unit.
 * @param   error
 *          Pointer to an OCStringRef; if non‐NULL on entry and an error occurs,
 *          *error will be set to a newly created OCString describing the problem,
 *          which the caller must release.
 *
 * @return  A non‐owned SIUnitRef for the multiplied unit, reduced to lowest exponents,
 *          or NULL on failure. The caller does not own the returned reference.
 */
SIUnitRef SIUnitByMultiplying(SIUnitRef      theUnit1,
                              SIUnitRef      theUnit2,
                              double        *unit_multiplier,
                              OCStringRef   *error);

/**
 * @brief   Multiplies two units without reducing exponents.
 *
 * @param   theUnit1
 *          A non‐owned SIUnitRef for the first unit. Must not be NULL.
 * @param   theUnit2
 *          A non‐owned SIUnitRef for the second unit. Must not be NULL.
 * @param   unit_multiplier
 *          Pointer to a double initialized to the existing multiplier for the first unit.
 *          On return, this value is multiplied by the factor needed so that
 *          the numeric value remains consistent under the new unit.
 * @param   error
 *          Pointer to an OCStringRef; if non‐NULL on entry and an error occurs,
 *          *error will be set to a newly created OCString describing the problem,
 *          which the caller must release.
 *
 * @return  A non‐owned SIUnitRef for the multiplied unit without exponent reduction,
 *          or NULL on failure. The caller does not own the returned reference.
 */
SIUnitRef SIUnitByMultiplyingWithoutReducing(SIUnitRef      theUnit1,
                                             SIUnitRef      theUnit2,
                                             double        *unit_multiplier,
                                             OCStringRef   *error);

/**
 * @brief   Divides one unit by another and reduces the result to lowest integer exponents.
 *
 * @param   theUnit1
 *          A non‐owned SIUnitRef representing the numerator unit. Must not be NULL.
 * @param   theUnit2
 *          A non‐owned SIUnitRef representing the denominator unit. Must not be NULL.
 * @param   unit_multiplier
 *          Pointer to a double initialized to the multiplier for @p theUnit1.
 *          On return, this value is multiplied by the factor required so that
 *          the numeric value remains consistent under the new unit.
 *
 * @return  A non‐owned SIUnitRef for the reduced quotient unit, or NULL if an error occurs.
 *          The caller does not own the returned reference.
 *
 * @details Computes the quotient of two units (theUnit1 ÷ theUnit2), then simplifies
 *          all numerator and denominator exponents to their lowest common integer form.
 */
SIUnitRef SIUnitByDividing(SIUnitRef theUnit1,
                           SIUnitRef theUnit2,
                           double   *unit_multiplier);

/**
 * @brief   Divides one unit by another without reducing exponents.
 *
 * @param   theUnit1
 *          A non‐owned SIUnitRef representing the numerator unit. Must not be NULL.
 * @param   theUnit2
 *          A non‐owned SIUnitRef representing the denominator unit. Must not be NULL.
 * @param   unit_multiplier
 *          Pointer to a double initialized to the multiplier for @p theUnit1.
 *          On return, this value is multiplied by the factor required so that
 *          the numeric value remains consistent under the new unit.
 *
 * @return  A non‐owned SIUnitRef for the raw quotient unit, or NULL if an error occurs.
 *          The caller does not own the returned reference.
 *
 * @details Computes the quotient of two units (theUnit1 ÷ theUnit2) but leaves
 *          the exponent arrays unmodified (no reduction).
 */
SIUnitRef SIUnitByDividingWithoutReducing(SIUnitRef theUnit1,
                                          SIUnitRef theUnit2,
                                          double   *unit_multiplier);

/**
 * @brief   Raises a unit to a given power and then reduces its exponents.
 *
 * @param   input
 *          A non‐owned SIUnitRef representing the original unit. Must not be NULL.
 * @param   power
 *          The real‐valued exponent to raise the unit by.
 * @param   unit_multiplier
 *          Pointer to a double initialized to the multiplier for @p input.
 *          On return, this value is multiplied by the factor required so that
 *          the numeric value remains consistent under the new unit.
 * @param   error
 *          Pointer to an OCStringRef; if non‐NULL on entry and an error occurs,
 *          *error will be set to a newly created OCString describing the problem
 *          (e.g., non‐integer result), which the caller must release.
 *
 * @return  A non‐owned SIUnitRef for the powered, reduced unit, or NULL on failure.
 *          The caller does not own the returned reference.
 *
 * @details Supports integer and rational powers. If the power is not exactly
 *          representable by integer exponents (after reduction), the function
 *          fails and sets @p *error.
 */
SIUnitRef SIUnitByRaisingToAPower(SIUnitRef    input,
                                  double       power,
                                  double      *unit_multiplier,
                                  OCStringRef *error);

/**
 * @brief   Raises a unit to a given power without reducing its exponents.
 *
 * @param   input
 *          A non‐owned SIUnitRef representing the original unit. Must not be NULL.
 * @param   power
 *          The real‐valued exponent to raise the unit by.
 * @param   unit_multiplier
 *          Pointer to a double initialized to the multiplier for @p input.
 *          On return, this value is multiplied by the factor required so that
 *          the numeric value remains consistent under the new unit.
 * @param   error
 *          Pointer to an OCStringRef; if non‐NULL on entry and an error occurs,
 *          *error will be set to a newly created OCString describing the problem,
 *          which the caller must release.
 *
 * @return  A non‐owned SIUnitRef for the powered unit without exponent reduction,
 *          or NULL on failure. The caller does not own the returned reference.
 *
 * @details Performs exponentiation of the underlying dimensional exponents by @p power
 *          but does not simplify the resulting exponent arrays.
 */
SIUnitRef SIUnitByRaisingToAPowerWithoutReducing(SIUnitRef    input,
                                                 double       power,
                                                 double      *unit_multiplier,
                                                 OCStringRef *error);

/**
 * @brief   Look up a unit by its canonical name.
 *
 * @param   input
 *          An OCStringRef containing the unit name to search for.
 *
 * @return  A non-owned SIUnitRef matching the given name, or NULL if not found.
 *          The caller must not release the returned reference.
 */
SIUnitRef SIUnitFindWithName(OCStringRef input);

/**
 * @brief   Retrieve a unit by its underived (base) symbol.
 *
 * @param   symbol
 *          An OCStringRef containing the base unit symbol (e.g., "m", "kg").
 *
 * @return  A non-owned SIUnitRef corresponding to the symbol, or NULL if unknown.
 *          The caller must not release the returned reference.
 */
SIUnitRef SIUnitForUnderivedSymbol(OCStringRef symbol);

/**
 * @brief   Parse a (possibly derived) symbol and return the corresponding unit.
 *
 * @param   symbol
 *          An OCStringRef containing the derived unit symbol (e.g., "N·m", "m/s^2").
 * @param   unit_multiplier
 *          Pointer to a double initialized by the caller. On return, it is multiplied
 *          by any scale factor implied by the symbol (e.g., prefixes or compound factors).
 * @param   error
 *          Pointer to an OCStringRef. On failure, *error will be set to a newly created
 *          OCString describing the parse error; the caller is responsible for releasing it.
 *
 * @return  A non-owned SIUnitRef for the parsed symbol, or NULL if invalid.
 *          The caller must not release the returned reference.
 */
SIUnitRef SIUnitForSymbol(OCStringRef symbol,
                          double     *unit_multiplier,
                          OCStringRef *error);

/**
 * @brief   Build an array of all units defined for a given quantity.
 *
 * @param   quantity
 *          An OCStringRef naming the physical quantity (e.g., "length", "time").
 *
 * @return  A newly created OCArray of SIUnitRef values (caller must release).
 *          Returns NULL if no units are defined for that quantity.
 *          The caller owns the array but not its elements.
 */
OCArrayRef SIUnitCreateArrayForQuantity(OCStringRef quantity);

/**
 * @brief   Construct a dictionary mapping symbols to units for a given dimensionality.
 *
 * @param   theDimensionality
 *          A non-owned SIDimensionalityRef representing the target dimensionality.
 *
 * @return  A newly created OCDictionary with OCStringRef keys and SIUnitRef values
 *          (caller must release), or NULL if none exist.
 *          The caller owns the dictionary but not its keys or values.
 */
OCDictionaryRef SIUnitCreateDictionaryOfUnitsWithDimensionality(SIDimensionalityRef theDimensionality);

/**
 * @brief   Construct a dictionary mapping symbols to units having the same reduced dimensionality.
 *
 * @param   theDimensionality
 *          A non-owned SIDimensionalityRef representing the target reduced dimensionality.
 *
 * @return  A newly created OCDictionary with OCStringRef keys and SIUnitRef values
 *          (caller must release), or NULL if none exist.
 *          The caller owns the dictionary but not its keys or values.
 */
OCDictionaryRef SIUnitCreateDictionaryOfUnitsWithSameReducedDimensionality(SIDimensionalityRef theDimensionality);

/**
 * @brief   Compute the multiplicative conversion factor between two compatible units.
 *
 * @param   initialUnit
 *          A non-owned SIUnitRef representing the source unit.
 * @param   finalUnit
 *          A non-owned SIUnitRef representing the destination unit.
 *
 * @return  A double factor such that:
 *          numeric_value_in_final = numeric_value_in_initial * returned_factor.
 *
 * @note    The two units must share the same dimensionality. This function does not
 *          handle prefix multipliers from SIUnitForParsedSymbol – combine those
 *          separately if needed.
 */
double SIUnitConversion(SIUnitRef initialUnit,
                        SIUnitRef finalUnit);

#pragma mark Strings and Archiving

/**
 * @brief   Print a concise, human-readable description of the unit to stdout.
 *
 * @param   theUnit
 *          A non-owned SIUnitRef whose summary will be displayed,
 *          including its symbol and scale.
 */
void SIUnitShow(SIUnitRef theUnit);

/**
 * @brief   Print a detailed, multi-line report of the unit to stdout.
 *
 * @param   theUnit
 *          A non-owned SIUnitRef whose full metadata will be displayed,
 *          including its name, symbol, dimensionality exponents, and conversion factors.
 */
void SIUnitShowFull(SIUnitRef theUnit);

#pragma mark Library

/**
 * @brief   Query whether the library uses U.S. (galUS, qtUS, etc.) or U.K. (galUK, qtUK, etc.) volume units.
 *
 * @return  true if U.S. volume units (galUS, qtUS, ptUS, fl ozUS, tbspUS, tspUS) are in use;
 *          false if U.K./Imperial units (galUK, qtUK, ptUK, fl ozUK, tbspUK, tspUK) are in use.
 */
bool SIUnitsLibraryImperialVolumes(void);

/**
 * @brief   Configure the library to use U.S. or U.K./Imperial volume symbols.
 *
 * @param   value
 *          true  ⇒ append “US” suffix to gallon, quart, pint, fluid ounce, tablespoon, teaspoon;
 *          false ⇒ append “UK” suffix for Imperial usage.
 */
void SIUnitsLibrarySetImperialVolumes(bool value);

/**
 * @brief   Retrieve a mutable copy of the current units library.
 *
 * @return  A newly created OCMutableDictionaryRef containing all registered SIUnit objects
 *          keyed by their symbols. Caller must release the returned dictionary.
 */
OCMutableDictionaryRef SIUnitGetLibrary(void);

/**
 * @brief   Replace the global units library with a new dictionary.
 *
 * @param   newUnitsLibrary
 *          A retained OCMutableDictionaryRef of SIUnit objects keyed by symbol.
 *          The previous library, if any, will be released.
 */
void SIUnitSetLibrary(OCMutableDictionaryRef newUnitsLibrary);

/**
 * @brief   Print a one-line summary of each unit in the global library to stdout.
 */
void SIUnitLibraryShow(void);

/**
 * @brief   Print a detailed, multi-line description of each unit in the global library to stdout.
 */
void SIUnitLibraryShowFull(void);

/**
 * @brief   List all units sorted by the length of their printed names.
 *
 * @return  An OCArrayRef of SIUnitRef elements, sorted ascending by name length.
 *          Caller must release the returned array. The units themselves are not owned.
 */
OCArrayRef SIUnitGetUnitsSortedByNameLength(void);

/**
 * @brief   List every base (root) unit, ordered alphabetically by root name.
 *
 * @return  An OCArrayRef of SIUnitRef root units. Caller must release the returned array.
 */
OCArrayRef SIUnitCreateArrayOfRootUnits(void);

/**
 * @brief   List the root units applicable to a given physical quantity.
 *
 * @param   quantity
 *          The quantity symbol (e.g. "length", "mass").
 * @param   error
 *          On failure, a pointer to an OCStringRef describing the error.
 *
 * @return  An OCArrayRef of SIUnitRef root units for that quantity.
 *          Caller must release the returned array. Returns NULL on error.
 */
OCArrayRef SIUnitCreateArrayOfRootUnitsForQuantity(OCStringRef quantity, OCStringRef *error);

/**
 * @brief   Tear down all static and global unit dictionaries.
 *
 * @details Frees internal memory used by the units subsystem. Should be called
 *          during application shutdown if static cleanup did not run.
 */
void cleanupUnitsLibraries(void);

#endif /* SIUnit_h */
