//
//  SIUnit.h
//  SITypes
//
//  Created by philip on 6/7/17.
//  Copyright © 2017 PhySy Ltd. All rights reserved.
//
#ifndef SIUnit_H
#define SIUnit_H
#include "SILibrary.h"
/*!
 * @file SIUnit.h
 * @brief Interface for defining, manipulating, and comparing SI and derived units.
 *
 * The SIUnit API provides a comprehensive, type-safe interface for immutable
 * **physical unit definitions** in the International System of Units (SI), including
 * support for both SI base units and coherent derived units, as well as common non-SI units.
 *
 * ## Concepts and Rationale
 * - **Unit Object Model:** An SIUnit represents a physical unit, including its symbol,
 *   SI prefix, dimensional exponents (for the seven SI base quantities), and scale factor.
 *   Units are opaque and immutable references.
 * - **Dimensional Analysis:** Every SIUnit encodes full dimensionality, allowing
 *   for strict unit checking and compatibility enforcement at runtime.
 * - **Prefixes and Composition:** Supports all standard SI prefixes and algebraic
 *   unit composition (multiplication, division, powers, and roots).
 * - **Canonicalization:** Derived and composite units are always simplified to a
 *   canonical form for robust comparison, formatting, and conversion.
 *
 * ## API Features
 * - **Accessors:** Retrieve unit symbol, name, dimensionality, and other metadata.
 * - **Parsing and Formatting:** Parse units from strings or symbols and generate
 *   human-readable names and formatted symbols.
 * - **Unit Arithmetic:** Compose units by multiplication, division, exponentiation,
 *   or reduction to lowest terms.
 * - **Comparison and Equivalence:** Test for dimensional equivalence, full equality
 *   (including prefix and symbol), and conversion factors.
 * - **Unit Collections:** Query units by dimensionality or physical quantity.
 * - **Constants:** Provides standard symbols and physical constants as macros.
 * - **Library Management:** Manage the global registry of units and library-scoped settings
 *   (e.g., US vs. UK volume units).
 *
 * ## Usage Notes
 * - **Immutability:** All SIUnit objects are immutable and may be safely shared
 *   between threads.
 * - **Memory Management:** Functions with `Create` or `Copy` in the name return
 *   new OCType objects. **Ownership of these objects is transferred to the caller,**
 *   who must call `OCRelease` when the object is no longer needed.
 *   All other functions return non-owned references unless otherwise stated.
 * - **Pointer Safety:** All pointer arguments must be non-NULL unless documented.
 * - **Thread Safety:** All functions are thread-safe when operating on distinct units
 *   or library objects.
 * - **Intended Use:** This interface is designed for scientific, engineering, or
 *   educational software requiring precise, robust, and extensible unit handling.
 *
 * @author Philip Grandinetti
 */
/** @cond INTERNAL */
/** @brief SI Prefix enumeration. */
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
#define kSIMinute 60.
#define kSIHour 3600
#define kSIDay 86400
#define kSIWeek 604800
#define kSIMonth 2629800
#define kSIYear 31557600
#define kSIDecade 315576000
#define kSICentury 3155760000
#define kSIMillennium 31557600000
#define kSIInch 0.0254
#define kSIFoot 0.3048
#define kSIYard 0.9144
#define kSIMile 1609.344
#define kSIAcre 4046.8564224
#define kSIPoundsForce 4.4482216152605
#define kSIBtu 1055.05585257348
#define kSICal 4.1868
#define kSIhp 745.699872
#define kSIPound 0.45359237
#define kSIOunce 0.028349523125
#define kSIPi 3.141592653589793
#define kSIEulersNumber 2.718281828459045
#define kSISpeedOfLight 299792458.
#define kSIElementaryCharge 1.602176634e-19
#define kSIPlanckConstant 6.62607015e-34
#define kSIBoltmannConstant 1.380649e-23
#define kSIAvogadroConstant 6.02214076e23
#define kSIStefanBoltzmannConstant 5.670374419e-8
#define kSIWeinDisplacementConstant 2.897771955e-3
#define kSIElectronMass 9.109383701528e-31
#define kSIProtonMass 1.6726219236951e-27
#define kSINeutronMass 1.6749274980495e-27
#define kSIMuonMass 1.883531627459132e-28
#define kSIAtomicMassConstant 1.6605390666050e-27
#define kSIAlphaParticleMass 6.64465723082e-27
#define kSIGravitationalConstant 6.6743015e-11
#define kSIElectricConstant 8.854187817620389e-12  // Defined as 1/sqrt(c_0^2*µ_0)
// Above updated for 2019 mass definitions
#define kSIElectronMagneticMoment -928.4764620e-26
#define kSIElectronGFactor -2.00231930436182
#define kSIProtonMagneticMoment 1.4106067873e-26
#define kSIProtonGFactor 5.585694702
#define kSINeutronMagneticMoment -0.96623650e-26
#define kSINeutronGFactor -3.82608545
#define kSIMuonMagneticMoment -4.49044826e-26
#define kSIMuonGFactor -2.0023318418
#define kSIGravityAcceleration 9.80665
/** @endcond */
// Type functions
OCTypeID SIUnitGetTypeID(void);
// Constructor functions
SIUnitRef SIUnitCoherentUnitFromDimensionality(SIDimensionalityRef dimensionality);
SIUnitRef SIUnitDimensionlessAndUnderived(void);
// Accessor functions
/** @brief Gets the physical dimensionality for a unit. */
SIDimensionalityRef SIUnitGetDimensionality(SIUnitRef theUnit);
OCStringRef SIUnitGetSymbol(SIUnitRef theUnit);
OCStringRef SIUnitGetName(SIUnitRef theUnit);
OCStringRef SIUnitGetPluralName(SIUnitRef theUnit);
double SIUnitGetScaleToCoherentSI(SIUnitRef theUnit);
double SIUnitScaleToCoherentSIUnit(SIUnitRef theUnit);
OCStringRef SIUnitCopySymbol(SIUnitRef theUnit);
OCStringRef SIUnitCopyName(SIUnitRef theUnit);
OCStringRef SIUnitCopyPluralName(SIUnitRef theUnit);
cJSON *SIUnitCreateJSON(SIUnitRef unit);
bool SIUnitEqual(SIUnitRef theUnit1, SIUnitRef theUnit2);
bool SIUnitAreEquivalentUnits(SIUnitRef theUnit1, SIUnitRef theUnit2);
bool SIUnitIsCoherentUnit(SIUnitRef theUnit);
bool SIUnitIsDimensionless(SIUnitRef theUnit);
// Unit conversion
double SIUnitConversion(SIUnitRef initialUnit, SIUnitRef finalUnit);
// Unit library management
void SIUnitLibrarySetImperialVolumes(bool value);
bool SIUnitLibraryGetImperialVolumes(void);
void SIUnitLibrariesShutdown(void);  // do not call, called by SITypesShutdown()
// Array creation functions
OCArrayRef SIUnitCreateArrayOfUnitsForQuantity(OCStringRef quantity);
OCArrayRef SIUnitCreateArrayOfUnitsForDimensionality(SIDimensionalityRef theDim);
OCArrayRef SIUnitCreateArrayOfUnitsForSameReducedDimensionality(SIDimensionalityRef theDim);
OCArrayRef SIUnitCreateArrayOfConversionUnits(SIUnitRef theUnit);
OCArrayRef SIUnitCreateArrayOfEquivalentUnits(SIUnitRef theUnit);
OCMutableArrayRef SIUnitGetTokenSymbolsLib(void);
SIUnitRef SIUnitWithSymbol(OCStringRef symbol);
SIUnitRef SIUnitFindWithName(OCStringRef input);
SIUnitRef SIUnitFindEquivalentUnitWithShortestSymbol(SIUnitRef theUnit);
// Unit reduction operations
SIUnitRef SIUnitByReducing(SIUnitRef theUnit, double *unit_multiplier);
// Utility functions
OCStringRef SIUnitGuessQuantityName(SIUnitRef theUnit);
// Power operations
SIUnitRef SIUnitByRaisingToPowerWithoutReducing(SIUnitRef input,
                                                int power,
                                                double *unit_multiplier,
                                                OCStringRef *error);
SIUnitRef SIUnitByRaisingToPower(SIUnitRef input,
                                 int power,
                                 double *unit_multiplier,
                                 OCStringRef *error);
// Multiplication operations
SIUnitRef SIUnitByMultiplyingWithoutReducing(SIUnitRef theUnit1,
                                             SIUnitRef theUnit2,
                                             double *unit_multiplier,
                                             OCStringRef *error);
SIUnitRef SIUnitByMultiplying(SIUnitRef theUnit1,
                              SIUnitRef theUnit2,
                              double *unit_multiplier,
                              OCStringRef *error);
// Division operations
SIUnitRef SIUnitByDividingWithoutReducing(SIUnitRef theUnit1,
                                          SIUnitRef theUnit2,
                                          double *unit_multiplier,
                                          OCStringRef *error);
SIUnitRef SIUnitByDividing(SIUnitRef theUnit1,
                           SIUnitRef theUnit2,
                           double *unit_multiplier,
                           OCStringRef *error);
// Nth root operations
SIUnitRef SIUnitByTakingNthRoot(SIUnitRef theUnit,
                                int root,
                                double *unit_multiplier,
                                OCStringRef *error);
SIUnitRef SIUnitFromExpression(OCStringRef expression, double *unit_multiplier, OCStringRef *error);

/*!
 * @brief Temporary normalization function for existing parser compatibility.
 * TODO: Implement proper Unicode normalization
 */
OCMutableStringRef SIUnitCreateNormalizedExpression(OCStringRef expression, bool flag);

/*!
 * @brief Temporary function for expression equivalence checking.
 * TODO: Implement proper expression equivalence checking
 */
bool SIUnitAreExpressionsEquivalent(OCStringRef expr1, OCStringRef expr2);

/*!
 * @brief Creates a cleaned and normalized unit expression by grouping and sorting terms.
 *
 * This function takes a unit expression string and returns a cleaned version where:
 * - Unicode characters are normalized
 * - Identical unit symbols are grouped together (powers combined)
 * - Terms are sorted alphabetically
 * - Expression is formatted in a canonical form
 *
 * Unlike SIUnitCreateCleanedAndReducedExpression, this function does NOT perform
 * algebraic cancellation between numerator and denominator terms.
 *
 * @param expression The input unit expression string to clean
 * @return A cleaned unit expression string, or NULL on error
 *
 * @note The caller is responsible for releasing the returned string with OCRelease()
 *
 * Examples:
 * - "m*kg*m" → "kg•m^2"
 * - "kg*m/s/s" → "kg•m/s^2"  
 * - "m/kg*s" → "m•s/kg"
 */
OCStringRef SIUnitCreateCleanedExpression(OCStringRef expression);

/*!
 * @brief Creates a cleaned, normalized, and algebraically reduced unit expression.
 *
 * This function performs all the operations of SIUnitCreateCleanedExpression plus:
 * - Algebraic cancellation of identical terms between numerator and denominator
 * - Simplification to the most reduced form
 *
 * @param expression The input unit expression string to clean and reduce
 * @return A cleaned and reduced unit expression string, or NULL on error
 *
 * @note The caller is responsible for releasing the returned string with OCRelease()
 *
 * Examples:
 * - "m*kg/m" → "kg"
 * - "m^2*s/m/s" → "m"
 * - "kg*m*s^2/kg/m" → "s^2"
 */
OCStringRef SIUnitCreateCleanedAndReducedExpression(OCStringRef expression);

OCMutableDictionaryRef SIUnitGetUnitsLib(void);

#endif  // SIUnit_H
