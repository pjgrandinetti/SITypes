//
//  SIDimensionality.h
//  SITypes
//
//  Created by philip on 6/7/17.
//  Copyright © 2017 PhySy Ltd. All rights reserved.
//

#ifndef SIDIMENSIONALITY_H
#define SIDIMENSIONALITY_H

#include "SILibrary.h"

/**
 * @file SIDimensionality.h
 * @brief Declares the SIDimensionality interface for representing and manipulating physical dimensionalities.
 *
 * The seven SI base dimensions are supported: length, mass, time, electric current, temperature,
 * amount of substance, and luminous intensity.
 *
 * @author Philip Grandinetti
 */



#ifdef __cplusplus
extern "C" {
#endif

/** @cond INTERNAL */

/**
 * @typedef SIDimensionalityRef
 * @brief This is the type of a reference to immutable SIDimensionality.
 * @details A NULL reference represents a dimensionless and underived dimensionality.
 */
typedef const struct __SIDimensionality * SIDimensionalityRef;

/**
 * @enum SIBaseDimensionIndex
 * @brief Index constants for the seven SI base dimensions.
 * @var kSILengthIndex Index for length dimension.
 * @var kSIMassIndex Index for mass dimension.
 * @var kSITimeIndex Index for time dimension.
 * @var kSICurrentIndex Index for current dimension.
 * @var kSITemperatureIndex Index for temperature dimension.
 * @var kSIAmountIndex Index for amount dimension.
 * @var kSILuminousIntensityIndex Index for luminous intensity dimension.
 */
typedef enum {
    kSILengthIndex = 0,
    kSIMassIndex = 1,
    kSITimeIndex = 2,
    kSICurrentIndex = 3,
    kSITemperatureIndex = 4,
    kSIAmountIndex = 5,
    kSILuminousIntensityIndex = 6,
} SIBaseDimensionIndex;
/** @endcond */

/**
 * @brief Parse an SI dimensionality expression into a cached, immutable object.
 *
 * This function recognizes the seven SI base dimensions:
 * - Length           (L)
 * - Mass             (M)
 * - Time             (T)
 * - Electric current (I)
 * - Temperature      (Θ or, when only ASCII is available, ‘@’)
 * - Amount of substance (N)
 * - Luminous intensity  (J)
 *
 * The grammar supports:
 *   • Integer exponents (e.g. `L^2`, `M^-1`)  
 *   • Multiplication via `*`  
 *   • Division via `/`  
 *
 * Examples:
 * @code
 *   "L^1*M^2/T^3"
 *   "M/L^3"
 *   "@^1"        // same as Θ^1
 *   "T^-2"       // time squared in denominator
 * @endcode
 *
 * @param[in]  symbol  An OCStringRef containing the dimensionality expression.
 *                      Must not be NULL.
 * @param[out] error   If non-NULL and parsing fails, set to an OCStringRef
 *                      describing the error (caller must release it).
 *
 * @return
 *   On success, a cached, immutable SIDimensionalityRef (do not retain or release).
 *   On failure, NULL (and *error is set if the error pointer was provided).
 *
 * @note
 *   • Numerator or denominator terms with zero exponent may be omitted.  
 *   • Temperature symbol may be the Unicode ‘Θ’ or ASCII ‘@’.  
 */
SIDimensionalityRef
SIDimensionalityForSymbol(OCStringRef symbol, OCStringRef *error);

/** @cond INTERNAL */

extern OCMutableDictionaryRef dimLibrary;
extern OCMutableDictionaryRef dimQuantitiesLibrary;

#define IF_UNEQUAL_DIMENSIONALITIES(DIM1,DIM2,RESULT) if(!SIDimensionalityEqual(DIM1,DIM2)) { \
fprintf(stderr, "%s : Unequal dimensionalities:  ",__FUNCTION__); \
OCStringShow(SIDimensionalityGetSymbol(DIM1)); \
fprintf(stderr, " and "); \
OCStringShow(SIDimensionalityGetSymbol(DIM2)); \
fprintf(stderr, "\n"); \
return RESULT;}

#define IF_INCOMPATIBLE_DIMENSIONALITIES(DIM1,DIM2,RESULT) if(!SIDimensionalityHasSameReducedDimensionality(DIM1,DIM2)) { \
fprintf(stderr, "%s : Incompatible dimensionalities:  ",__FUNCTION__); \
OCStringShow(SIDimensionalityGetSymbol(DIM1)); \
fprintf(stderr, " and "); \
OCStringShow(SIDimensionalityGetSymbol(DIM2)); \
fprintf(stderr, "\n"); \
return RESULT;}
/** @endcond */

/**
 * @brief SIDimensionality represents the dimensionality of a physical quantity.
 * @details Seven physical quantities serve as fundamental reference quantities from which all other physical quantities can be derived. These reference quantities are:
 * - Length
 * - Mass
 * - Time
 * - Electric current
 * - Thermodynamic temperature (the absolute measure of temperature)
 * - Amount of substance
 * - Luminous intensity
 *
 * @copyright PhySy
 */

#pragma mark Accessors
/**
 * @brief Accessor functions for SIDimensionality.
 */

/**
 * @brief Obtain the runtime type identifier for SIDimensionality objects.
 *
 * Every OC “class” has a unique OCTypeID that can be used for
 * runtime checks, type assertions, or bridging with CoreFoundation
 * and other OC-based frameworks.
 *
 * This function is thread-safe and returns a constant value
 * for the lifetime of the process.
 *
 * @return The unique OCTypeID corresponding to SIDimensionality.
 *
 * @see OCTypeID, OCGetTypeID
 */
OCTypeID
SIDimensionalityGetTypeID(void);

/**
 * @brief Retrieve the canonical symbol representation of a dimensionality.
 *
 * This function returns an immutable OCString that encodes the
 * dimensionality in the standard form, for example:
 * 
 *     L^1 * M^0 * T^-2  →  "L/T^2"
 * 
 * Factors with a zero exponent are omitted entirely, negative
 * exponents appear in the denominator, and the temperature symbol
 * “ϴ” may be represented as “@” if desired.  Multiplication signs
 * (“*”) and exponent markers (“^”) follow the conventions used by
 * SIDimensionalityForSymbol.
 *
 * @param theDim
 *   A valid SIDimensionalityRef whose symbol you wish to obtain.
 *   Must not be NULL.
 *
 * @return
 *   An OCStringRef holding the dimensionality symbol.  This string
 *   is owned by the library, is guaranteed immutable, and must not
 *   be retained, released, or modified by the caller.
 *
 * @thread_safety
 *   Safe to call from any thread.
 *
 * @see SIDimensionalityForSymbol, OCStringRef
 */
OCStringRef
SIDimensionalityGetSymbol(SIDimensionalityRef theDim);

/** @cond INTERNAL */

/**
 * @brief Retrieve the numerator of the rational exponent for a given SI base dimension.
 *
 * Each SIDimensionality object represents the exponent of each of the seven
 * SI base dimensions (length, mass, time, electric current, temperature,
 * amount of substance, luminous intensity) as a rational number (numerator/denominator).
 * This function returns the numerator component (p) of that fraction for the
 * base dimension specified by @p index.
 *
 * @param theDim
 *   A valid SIDimensionalityRef. Must not be NULL.
 * @param index
 *   An SIBaseDimensionIndex constant identifying which base dimension to query:
 *     - kSILengthIndex
 *     - kSIMassIndex
 *     - kSITimeIndex
 *     - kSICurrentIndex
 *     - kSITemperatureIndex
 *     - kSIAmountIndex
 *     - kSILuminousIntensityIndex
 *
 * @return
 *   The unsigned integer numerator of the rational exponent for the specified
 *   base dimension.
 *
 * @see SIDimensionalityGetDenExpAtIndex to retrieve the corresponding denominator.
 * @thread_safety
 *   Safe to call concurrently on distinct SIDimensionalityRef objects.
 */
uint8_t
SIDimensionalityGetNumExpAtIndex(SIDimensionalityRef      theDim,
                                 SIBaseDimensionIndex     index);

/**
 * @brief Retrieve the denominator of the rational exponent for a given SI base dimension.
 *
 * Each SIDimensionality object represents the exponent of each of the seven
 * SI base dimensions (length, mass, time, electric current, temperature,
 * amount of substance, luminous intensity) as a rational number (numerator/denominator).
 * This function returns the denominator component (q) of that fraction for the
 * base dimension specified by @p index.
 *
 * @param theDim
 *   A valid SIDimensionalityRef. Must not be NULL.
 * @param index
 *   An SIBaseDimensionIndex constant identifying which base dimension to query:
 *     - kSILengthIndex
 *     - kSIMassIndex
 *     - kSITimeIndex
 *     - kSICurrentIndex
 *     - kSITemperatureIndex
 *     - kSIAmountIndex
 *     - kSILuminousIntensityIndex
 *
 * @return
 *   The unsigned integer denominator of the rational exponent for the specified
 *   base dimension.
 *
 * @see SIDimensionalityGetNumExpAtIndex to retrieve the corresponding numerator.
 * @thread_safety
 *   Safe to call concurrently on distinct SIDimensionalityRef objects.
 */
uint8_t
SIDimensionalityGetDenExpAtIndex(SIDimensionalityRef      theDim,
                                 SIBaseDimensionIndex     index);

/**
 * @brief   Retrieve the simplified exponent for a given SI base dimension.
 *
 * Each SIDimensionality object stores the exponent of each base dimension as
 * a rational number (numerator/denominator).  This function returns the
 * “reduced” exponent—i.e. the numerator minus the denominator—as a single
 * signed integer for the specified base dimension.
 *
 * @param   theDim    A valid SIDimensionalityRef. Must not be NULL.
 * @param   index     One of the SIBaseDimensionIndex constants identifying
 *                    which base dimension to query:
 *                      - kSILengthIndex
 *                      - kSIMassIndex
 *                      - kSITimeIndex
 *                      - kSICurrentIndex
 *                      - kSITemperatureIndex
 *                      - kSIAmountIndex
 *                      - kSILuminousIntensityIndex
 *
 * @return  The signed integer “reduced” exponent (numerator – denominator)
 *          for the specified base dimension.
 *
 * @see     SIDimensionalityGetNumExpAtIndex() for the numerator component.
 * @see     SIDimensionalityGetDenExpAtIndex() for the denominator component.
 * @thread_safety
 *          Safe to call concurrently on distinct SIDimensionalityRef objects.
 */
int8_t
SIDimensionalityReducedExponentAtIndex(SIDimensionalityRef   theDim,
                                       SIBaseDimensionIndex  index);
/** @endcond */

#pragma mark Tests
/**
 * @brief Test functions for SIDimensionality.
 */

/**
 * @brief   Compare two dimensionality objects for exact representational equality.
 *
 * This function returns true only if both SIDimensionalityRef instances have
 * identical numerator and denominator exponents for each SI base dimension,
 * preserving any non-reduced form. For example, although L/T and L^2/T^2 reduce
 * to the same net exponent, they will be considered unequal here.
 *
 * @param   theDim1   The first dimensionality object. Must not be NULL.
 * @param   theDim2   The second dimensionality object. Must not be NULL.
 *
 * @return  true if every numerator and denominator exponent in theDim1 matches
 *          exactly the corresponding exponent in theDim2; false otherwise.
 *
 * @see     SIDimensionalityHasSameReducedDimensionality() to test equivalence
 *          based on reduced (numerator – denominator) exponents.
 * @thread_safety
 *          Safe to call concurrently on different SIDimensionalityRef objects.
 */
bool
SIDimensionalityEqual(SIDimensionalityRef theDim1,
                      SIDimensionalityRef theDim2);

/**
 * @brief   Check whether the given dimensionality represents a dimensionless quantity.
 *
 * A dimensionality is considered dimensionless when all SI base dimension exponents
 * reduce to zero (i.e., numerator and denominator exponents cancel out). This is
 * true for pure numeric values or unitless ratios.
 *
 * @param   theDim
 *          A valid SIDimensionalityRef to test. Must not be NULL.
 *
 * @return  true if every base dimension’s reduced exponent is zero (dimensionless);
 *          false otherwise.
 *
 * @see     SIDimensionalityReducedExponentAtIndex()
 * @thread_safety
 *          Safe to call concurrently on distinct SIDimensionalityRef objects.
 */
bool
SIDimensionalityIsDimensionless(SIDimensionalityRef theDim);

/**
 * @brief Determines whether a dimensionality represents an SI “derived” dimension.
 *
 * In SI, a derived dimensionality is any combination of base dimensional symbols
 * beyond a single simple base. It excludes:
 *   - Dimensionless quantities (no symbols; all exponents zero), and
 *   - Simple base dimensionalities (exactly one of L, M, T, I, Θ, N, or J to ±1).
 *
 * Examples:
 *   - L·T⁻¹    (velocity)       → derived
 *   - L²        (area)           → derived
 *   - M         (mass)           → not derived
 *   - —         (dimensionless)  → not derived
 *
 * @param theDim  The dimensionality to test.
 * @return        true if theDim combines two or more base symbols or raises
 *                one base symbol to an exponent other than ±1; false if it is
 *                dimensionless or exactly one base symbol to ±1.
 */
bool SIDimensionalityIsDerived(SIDimensionalityRef theDim);

/**
 * @brief Checks whether a dimensionality is strictly dimensionless (no base symbols)
 *        and not the result of cancelling matching symbols.
 *
 * A “dimensionless, not derived” dimensionality meets both of these:
 *   - All base dimension exponents (L, M, T, I, Θ, N, J) are zero.
 *   - It is not formed by cancelling identical symbols (e.g., L·T⁻¹·T/L).
 *
 * Use this to identify pure numeric or counting quantities.
 *
 * @param theDim  The dimensionality to test.
 * @return        true if theDim has no base symbols and was not produced by symbol cancellation; 
 *                false otherwise.
 */
bool SIDimensionalityIsDimensionlessAndNotDerived(SIDimensionalityRef theDim);

/**
 * @brief Checks if a dimensionality is dimensionless but arises from cancelling base symbols.
 *
 * A “dimensionless, derived” dimensionality meets both criteria:
 *   - All reduced exponents for L, M, T, I, Θ, N, and J are zero (net dimensionless).
 *   - Its internal representation contains non-zero numerator or denominator exponents
 *     (i.e. it was formed by combining and cancelling base symbols, not a pure count).
 *
 * Examples:
 *   - L·T⁻¹·T·L⁻¹ ⇒ dimensionless (derived by cancellation)
 *   - 1 ⇒ dimensionless (not derived)
 *
 * @param theDim  The dimensionality to evaluate.
 * @return        true if theDim is net dimensionless but built from cancelling base symbols;  
 *                false otherwise.
 */
bool SIDimensionalityIsDimensionlessAndDerived(SIDimensionalityRef theDim);

/**
 * @brief Checks whether a dimensionality represents exactly one of the seven base dimensions.
 *
 * A “base dimensionality” is one of the fundamental SI dimensions without any exponentiation or combination:
 *   - Length           (L)
 *   - Mass             (M)
 *   - Time             (T)
 *   - Electric current (I)
 *   - Temperature      (Θ — or “@” in ASCII)
 *   - Amount of substance (N)
 *   - Luminous intensity (J)
 *
 * @param theDim  A reference to the dimensionality to test.
 * @return        true if theDim matches exactly one base symbol (exponent = 1 for that symbol, 0 for all others);
 *                false if it is dimensionless, derived, composite, raised to any power ≠ 1, or not one of the seven.
 *
 * @details
 * Internally, each base dimension is indexed by one of the constants:
 *   kSILengthIndex, kSIMassIndex, kSITimeIndex,
 *   kSICurrentIndex, kSITemperatureIndex,
 *   kSIAmountIndex, kSILuminousIntensityIndex.
 *
 * This function returns true only when exactly one of the numerator exponents at those indices is 1
 * (and all denominator exponents are zero), indicating a pure base dimension.
 *
 * Examples:
 *   - L       ⇒ true
 *   - M⁻¹     ⇒ false (negative exponent)
 *   - L·T     ⇒ false (composite)
 *   - T²      ⇒ false (power ≠ 1)
 *   - 1       ⇒ false (dimensionless)
 */
bool SIDimensionalityIsBaseDimensionality(SIDimensionalityRef theDim);

/**
 * @brief Tests whether two dimensionalities represent the same physical dimensions.
 *
 * Compares the net (reduced) exponent of each of the seven SI base dimensions:
 *   Length (L), Mass (M), Time (T), Electric current (I),
 *   Temperature (Θ or “@”), Amount of substance (N),
 *   Luminous intensity (J).
 *
 * @param theDim1  The first dimensionality reference.
 * @param theDim2  The second dimensionality reference.
 * @return         true if, for every base dimension index, the reduced exponent
 *                 (numerator minus denominator) is identical in both arguments;
 *                 false otherwise.
 *
 * @details
 * Two dimensionalities are “reduced equivalent” if they yield the same overall power for each
 * base symbol, regardless of how factors are arranged in numerators or denominators.
 *
 * Examples:
 *   - m/s      and m·s⁻¹     ⇒ true (both reduce to L¹·T⁻¹)
 *   - Pa       (N·m⁻²)      and lbf/in²    ⇒ true (both reduce to M¹·L⁻¹·T⁻²)
 *   - L/T      and L²/T²     ⇒ false (exponents [1, –1] vs [2, –2])
 *   - dimensionless (1)     and count (1)  ⇒ true (all exponents = 0)
 *
 * Use this function to verify dimensional compatibility (e.g., unit conversion checks)
 * where only the physical dimensions matter, not their internal representation.
 */
bool SIDimensionalityHasSameReducedDimensionality(SIDimensionalityRef theDim1,
                                                 SIDimensionalityRef theDim2);

/** @cond INTERNAL */
/**
 * @brief Verifies that a dimensionality has the specified reduced exponents for each base symbol.
 *
 * Compares the net exponent (numerator minus denominator) of each of the seven SI base
 * dimensions—Length (L), Mass (M), Time (T), Electric current (I), Temperature (Θ or “@”),
 * Amount of substance (N), and Luminous intensity (J)—against the values provided.
 *
 * @param theDim                             The dimensionality to test.
 * @param length_exponent                    Desired reduced exponent for Length (L).
 * @param mass_exponent                      Desired reduced exponent for Mass (M).
 * @param time_exponent                      Desired reduced exponent for Time (T).
 * @param current_exponent                   Desired reduced exponent for Electric current (I).
 * @param temperature_exponent               Desired reduced exponent for Temperature (Θ, “@”).
 * @param amount_exponent                    Desired reduced exponent for Amount of substance (N).
 * @param luminous_intensity_exponent        Desired reduced exponent for Luminous intensity (J).
 * @return                                    true if every base dimension’s reduced exponent in
 *                                            `theDim` equals the corresponding parameter; 
 *                                            false otherwise.
 *
 * @details
 * This function is useful for asserting that a dimensionality matches an exact combination
 * of powers. A reduced exponent is calculated as:
 *   reduced = numerator_exponent − denominator_exponent.
 *
 * Example:
 *   // Check for velocity dimensions (L¹·T⁻¹):
 *   bool isVelocity = SIDimensionalityHasReducedExponents(
 *       dim, 1, 0, -1, 0, 0, 0, 0);
 */
bool SIDimensionalityHasReducedExponents(SIDimensionalityRef theDim,
                                         int8_t length_exponent,
                                         int8_t mass_exponent,
                                         int8_t time_exponent,
                                         int8_t current_exponent,
                                         int8_t temperature_exponent,
                                         int8_t amount_exponent,
                                         int8_t luminous_intensity_exponent);

/** @endcond */


#pragma mark Operations
/**
 * @brief Operations for SIDimensionality.
 */

/**
 * @brief Retrieves the canonical “dimensionless” dimensionality (all exponents = 0).
 *
 * Returns a shared, immutable `SIDimensionalityRef` in which every base-dimension exponent
 * (Length, Mass, Time, Current, Temperature, Amount, Luminous Intensity) is zero.
 * This represents a truly dimensionless quantity (e.g., pure numbers, ratios).
 *
 * @return An unowned, immutable `SIDimensionalityRef` for the dimensionless dimensionality.
 *         This reference is managed internally—do not retain or release it.
 *
 * @note
 * Dimensionless dimensionalities are commonly used for pure numeric values or
 * counting quantities not derived from physical base dimensions.
 */
SIDimensionalityRef SIDimensionalityDimensionless(void);

/** @cond INTERNAL */

/**
 * @brief Retrieves the dimensionality corresponding to one of the seven base dimensions.
 *
 * Given an `SIBaseDimensionIndex`—one of:
 *   - `kSILengthIndex`  
 *   - `kSIMassIndex`  
 *   - `kSITimeIndex`  
 *   - `kSICurrentIndex`  
 *   - `kSITemperatureIndex`  
 *   - `kSIAmountIndex`  
 *   - `kSILuminousIntensityIndex`  
 * returns the `SIDimensionalityRef` representing that base dimension raised to the first power.
 *
 * @param index  An `SIBaseDimensionIndex` value identifying which base dimension to return.
 * @return       An unowned, immutable `SIDimensionalityRef` for the specified base dimension.
 *               This reference is managed internally—do not retain, release, or modify it.
 *
 * @note
 * Base dimensionalities serve as the fundamental building blocks for all derived dimensional quantities.
 *
 * @example
 * @code
 *   // Obtain the "length" dimensionality:
 *   SIDimensionalityRef lengthDim = SIDimensionalityForBaseDimensionIndex(kSILengthIndex);
 * @endcode
 */
SIDimensionalityRef SIDimensionalityForBaseDimensionIndex(SIBaseDimensionIndex index);


/**
 * @brief Creates a base-dimensionality object from its single‐character symbol.
 *
 * @param symbol   A one‐character `OCStringRef` representing a base dimension:
 *                   - `'L'` for length  
 *                   - `'M'` for mass  
 *                   - `'T'` for time  
 *                   - `'I'` for electric current  
 *                   - `'ϴ'` or `'@'` for thermodynamic temperature  
 *                   - `'N'` for amount of substance  
 *                   - `'J'` for luminous intensity  
 * @param error    If non‐NULL and a lookup failure occurs, on return `*error` will
 *                 point to a newly created `OCStringRef` describing the error. The
 *                 caller owns and must release this error string. On success, `*error`
 *                 is left unchanged.
 *
 * @return         An unowned, immutable `SIDimensionalityRef` corresponding to the
 *                 given base dimension symbol, or `NULL` if the symbol is invalid.
 *                 The returned reference is managed internally—do not retain or release it.
 *
 * @details
 * This function maps a single‐character dimension symbol to the corresponding
 * base-dimensionality object. Because the Unicode character 'ϴ' is not part of ASCII,
 * the ASCII character `'@'` may be used interchangeably to represent temperature.
 *
 * @note           This function only accepts *base* dimension symbols and will **not**
 *                 parse any derived-dimensionality expressions
 *                 (e.g. `L/T`, `M^2T^-1`, etc.).
 *
 * @example
 * @code
 *   OCStringRef err = NULL;
 *   SIDimensionalityRef dim = SIDimensionalityWithBaseDimensionSymbol(
 *       OCStringCreateWithCString("M"), &err);
 *
 *   if (!dim) {
 *       // Handle invalid symbol
 *       fprintf(stderr, "Dimension lookup failed: %s\n", OCStringGetCStringPtr(err));
 *       OCRelease(err);
 *   }
 * @endcode
 */
SIDimensionalityRef
SIDimensionalityWithBaseDimensionSymbol(OCStringRef symbol, OCStringRef *error);
/** @endcond */

/**
 * @brief Parses a dimensionality expression and returns the corresponding dimensionality object.
 *
 * @param expression
 *   An immutable `OCStringRef` containing a dimensionality symbol or expression. Valid symbols:
 *     - Base dimensions:  
 *         - `L` = length  
 *         - `M` = mass  
 *         - `T` = time  
 *         - `I` = electric current  
 *         - `ϴ` or `@` = thermodynamic temperature  
 *         - `N` = amount of substance  
 *         - `J` = luminous intensity  
 *     - Compound expressions of the form:
 *         `L^l * M^m * T^t * I^i * ϴ^q * N^n * J^j`
 *       or with a denominator:
 *         `(L^l * M^m * … * J^j) / (L^l' * M^m' * … * J^j')`
 *       where each lowercase exponent is an (optionally signed) integer and factors may be omitted.
 *
 * @param error
 *   If non-NULL and parsing fails, on return `*error` will point to a newly allocated
 *   `OCStringRef` describing the failure. The caller is responsible for releasing it.
 *   On success, `*error` is left unchanged.
 *
 * @return
 *   An unowned, immutable `SIDimensionalityRef` representing the parsed dimensionality,
 *   or `NULL` if parsing failed. The returned object is managed internally—do not retain
 *   or release it.
 *
 * @details
 *   - The parser recognizes any valid combination of base‐dimension symbols
 *     multiplied, divided, and raised to arbitrary signed integer powers.
 *   - Because the Unicode character “ϴ” (theta) is not ASCII, the parser accepts
 *     the ASCII substitute `@` for temperature.
 *
 * @note
 *   This function parses *derived* dimensionality expressions as well as single‐symbol
 *   base dimensions. For purely base‐symbol lookup without exponents or operators,
 *   use `SIDimensionalityWithBaseDimensionSymbol()`.
 *
 * @example
 * @code
 *   OCStringRef err = NULL;
 *   SIDimensionalityRef dim = SIDimensionalityForSymbol(
 *       OCStringCreateWithCString("M^1*T^-2"), &err);
 *   if (!dim) {
 *       fprintf(stderr, "Failed to parse dimensionality: %s\n",
 *               OCStringGetCStringPtr(err));
 *       OCRelease(err);
 *   }
 * @endcode
 */
SIDimensionalityRef
SIDimensionalityForSymbol(OCStringRef expression, OCStringRef *error);

/**
 * @brief Maps a predefined quantity name to its dimensionality.
 *
 * @param quantity
 *   An immutable `OCStringRef` whose value must exactly match one of the
 *   quantity constants defined in this API, for example:
 *     - kSIQuantityLength
 *     - kSIQuantityMass
 *     - kSIQuantityTime
 *     - kSIQuantityForce
 *     - kSIQuantityPressure
 *     - … and all other `kSIQuantity*` macros in this header.
 *
 * @param error
 *   If non-NULL and the lookup fails (unknown quantity string), on return
 *   `*error` will point to a newly allocated `OCStringRef` containing
 *   a descriptive error message. The caller is responsible for releasing
 *   this string. On success, `*error` is left unchanged.
 *
 * @return
 *   An unowned, immutable `SIDimensionalityRef` corresponding to the
 *   dimensionality of the given quantity (e.g. mass → M, force → M·L·T⁻²).
 *   Returns `NULL` if `quantity` is not recognized. The returned reference
 *   is managed internally—do not retain or release it.
 *
 * @details
 *   Use this function when you need to convert a high-level quantity
 *   identifier into its underlying dimensionality for unit compatibility
 *   checks or for assembling compound unit expressions.
 *
 * @example
 *   OCStringRef err = NULL;
 *   SIDimensionalityRef dim = SIDimensionalityForQuantity(
 *       kSIQuantityPressure, &err);
 *   if (!dim) {
 *       fprintf(stderr, "Unknown quantity: %s\n",
 *               OCStringGetCStringPtr(err));
 *       OCRelease(err);
 *   }
 */
SIDimensionalityRef
SIDimensionalityForQuantity(OCStringRef quantity, OCStringRef *error);

/**
 * @brief Produces a new dimensionality with all exponents reduced to lowest terms.
 *
 * @param theDimensionality
 *   An immutable `SIDimensionalityRef` whose numerator and denominator exponents
 *   may share a common factor.
 *
 * @return
 *   An unowned `SIDimensionalityRef` whose exponents have been divided by their
 *   greatest common divisor. The returned object is managed internally; do not
 *   retain or release it.
 *
 * @details
 *   For example, given a dimensionality representing
 *     L²·M⁴ / (T²·I²),
 *   this function computes the greatest common divisor of all exponents (2)
 *   and returns a dimensionality equivalent to
 *     L·M² / (T·I).
 *
 *   If the input is already in lowest terms, the same internal instance may be
 *   returned. This operation does not modify the original `SIDimensionality`.
 *
 * @see SIDimensionalityHasSameReducedDimensionality()
 */
SIDimensionalityRef
SIDimensionalityByReducing(SIDimensionalityRef theDimensionality);

/**
 * @brief Computes the integer nth‐root of a dimensionality by dividing each exponent by a given root.
 *
 * @param theDim
 *   An immutable `SIDimensionalityRef` whose numerator and denominator exponents
 *   will be divided by `root`.
 * @param root
 *   The integer by which each exponent must be exactly divisible.
 * @param error
 *   On failure, receives an owned `OCStringRef` describing why the operation
 *   could not be performed (e.g. an exponent not evenly divisible by `root`).
 *
 * @return
 *   A new `SIDimensionalityRef` in which each exponent of `theDim` has been
 *   divided by `root` and then reduced to lowest terms. If any exponent in
 *   `theDim` is not an integer multiple of `root`, returns `NULL` and
 *   populates `*error`. The returned object is managed internally; do not
 *   retain or release it.
 *
 * @details
 *   Both numerator and denominator exponents of the input must be exactly
 *   divisible by `root`, since only integer exponents are valid. After
 *   division, the resulting dimensionality is passed through the GCD‐based
 *   reduction process (see `SIDimensionalityByReducing`) to ensure lowest terms.
 *
 * @see SIDimensionalityByReducing()
 */
SIDimensionalityRef
SIDimensionalityByTakingNthRoot(SIDimensionalityRef theDim,
                               uint8_t root,
                               OCStringRef *error);


/**
 * @brief Multiplies two dimensionalities and returns the result in lowest terms.
 *
 * @param theDim1
 *   The first immutable `SIDimensionalityRef` operand.
 * @param theDim2
 *   The second immutable `SIDimensionalityRef` operand.
 * @param error
 *   On failure (e.g. internal overflow or invalid state), receives an owned
 *   `OCStringRef` describing the error.
 *
 * @return
 *   A new `SIDimensionalityRef` whose exponents are the sum of `theDim1` and
 *   `theDim2` exponents, reduced to lowest integer values. Returns `NULL` and
 *   populates `*error` if the operation cannot be completed. The returned
 *   object is managed internally; do not retain or release it.
 *
 * @details
 *   Each base dimension’s numerator exponents are added together, as are the
 *   denominator exponents. The combined exponents are then normalized by
 *   dividing by their greatest common divisor (see `SIDimensionalityByReducing`)
 *   to produce the simplest representation.
 *
 * @see SIDimensionalityByReducing()
 */
SIDimensionalityRef
SIDimensionalityByMultiplying(SIDimensionalityRef theDim1,
                              SIDimensionalityRef theDim2,
                              OCStringRef *error);
/**
 * @brief Multiplies two dimensionalities without performing any exponent reduction.
 *
 * @param theDim1
 *   The first immutable `SIDimensionalityRef` operand.
 * @param theDim2
 *   The second immutable `SIDimensionalityRef` operand.
 * @param error
 *   On failure, receives an owned `OCStringRef` describing the error.  Pass
 *   NULL if you do not need detailed error information.
 *
 * @return
 *   A new `SIDimensionalityRef` whose numerator and denominator exponents are
 *   the raw sum and sum‐of‐denominators of `theDim1` and `theDim2`.  Returns
 *   `NULL` on error. The returned object is managed internally; do not
 *   retain or release it.
 *
 * @details
 *   Unlike `SIDimensionalityByMultiplying`, this variant does not normalize
 *   the resulting exponents by their greatest common divisor.  Use it when
 *   you need to preserve the exact intermediate exponent values.
 *
 * @see SIDimensionalityByMultiplying
 */
SIDimensionalityRef
SIDimensionalityByMultiplyingWithoutReducing(SIDimensionalityRef theDim1,
                                            SIDimensionalityRef theDim2,
                                            OCStringRef *error);


/**
 * @brief Divides one dimensionality by another, then reduces exponents to lowest terms.
 *
 * @param theDim1
 *   The dividend immutable `SIDimensionalityRef`.
 * @param theDim2
 *   The divisor immutable `SIDimensionalityRef`.
 *
 * @return
 *   A new `SIDimensionalityRef` with each base‐dimension exponent equal to
 *   (exponent₁ − exponent₂) and then normalized by their greatest common
 *   divisor.  The returned object is managed internally; do not retain or
 *   release it.
 *
 * @details
 *   If you only need the raw subtraction of exponents without reduction,
 *   use `SIDimensionalityByDividingWithoutReducing`.
 *
 * @see SIDimensionalityByDividingWithoutReducing
 */
SIDimensionalityRef
SIDimensionalityByDividing(SIDimensionalityRef theDim1,
                          SIDimensionalityRef theDim2);


/**
 * @brief Divides one dimensionality by another without reducing exponents.
 *
 * @param theDim1
 *   The dividend immutable `SIDimensionalityRef`.
 * @param theDim2
 *   The divisor immutable `SIDimensionalityRef`.
 *
 * @return
 *   A new `SIDimensionalityRef` with each base‐dimension exponent equal to
 *   (exponent₁ − exponent₂) in raw form.  The returned object is managed
 *   internally; do not retain or release it.
 *
 * @details
 *   Use this variant when you need to preserve the exact difference of
 *   numerator and denominator exponents without normalizing to lowest terms.
 *
 * @see SIDimensionalityByDividing
 */
SIDimensionalityRef
SIDimensionalityByDividingWithoutReducing(SIDimensionalityRef theDim1,
                                          SIDimensionalityRef theDim2);


/**
 * @brief Raises a dimensionality to a real power and then reduces exponents.
 *
 * @param theDim
 *   The base immutable `SIDimensionalityRef`.
 * @param power
 *   The real exponent to apply to each base dimension.
 * @param error
 *   On failure (e.g. non‐integer exponent on non‐dimensionless base), receives
 *   an owned `OCStringRef` describing the error.
 *
 * @return
 *   A new `SIDimensionalityRef` whose exponents are each multiplied by
 *   `power` and then normalized to lowest integer ratios.  Returns `NULL` on
 *   error. The returned object is managed internally; do not retain or
 *   release it.
 *
 * @details
 *   Only integer results make sense for dimension exponents.  This
 *   function multiplies each exponent by `power`, verifies integer validity,
 *   then reduces by GCD (see `SIDimensionalityByReducing`).
 *
 * @see SIDimensionalityByRaisingToAPowerWithoutReducing
 * @see SIDimensionalityByReducing
 */
SIDimensionalityRef
SIDimensionalityByRaisingToAPower(SIDimensionalityRef theDim,
                                  double power,
                                  OCStringRef *error);


/**
 * @brief Raises a dimensionality to a real power without reducing exponents.
 *
 * @param theDim
 *   The base immutable `SIDimensionalityRef`.
 * @param power
 *   The real exponent to apply to each base dimension.
 * @param error
 *   On failure (e.g. non‐integer exponent on non‐dimensionless base), receives
 *   an owned `OCStringRef` describing the error.
 *
 * @return
 *   A new `SIDimensionalityRef` whose exponents are each multiplied by
 *   `power` in raw form.  Returns `NULL` on error. The returned object is
 *   managed internally; do not retain or release it.
 *
 * @details
 *   Use this variant to preserve the exact real‐scaled exponents before any
 *   normalization step.
 *
 * @see SIDimensionalityByRaisingToAPower
 */
SIDimensionalityRef
SIDimensionalityByRaisingToAPowerWithoutReducing(SIDimensionalityRef theDim,
                                                 double power,
                                                 OCStringRef *error);

/**
 * @brief Retrieves all quantity names whose dimensionality exactly matches theDim.
 *
 * @param theDim
 *   An immutable `SIDimensionalityRef` whose matching quantities to list.
 * @return
 *   An `OCArrayRef` of `OCStringRef` objects, each naming a physical quantity
 *   defined to have exactly the same numerator/denominator exponents as `theDim`.
 *   The returned array and its string elements are managed internally; do not
 *   retain or release them.
 *
 * @details
 *   For example, passing the dimensionality symbol “L·T⁻²” (length × time⁻²) might return
 *   `[STR("acceleration"), STR("gravitational acceleration")]`.
 */
OCArrayRef
SIDimensionalityCreateArrayOfQuantities(SIDimensionalityRef theDim);


/**
 * @brief Retrieves all quantity names that share the same reduced dimensionality as theDim.
 *
 * @param theDim
 *   An immutable `SIDimensionalityRef` whose reduced dimensionality to match.
 * @return
 *   An `OCArrayRef` of `OCStringRef` objects, each naming a physical quantity
 *   whose net exponents (numerator minus denominator) match those of `theDim`.
 *   The returned array and its string elements are managed internally; do not
 *   retain or release them.
 *
 * @details
 *   This function ignores how exponents are split between numerator and denominator
 *   and only compares net exponents.  For instance, both “L·T⁻¹” and “L/T” share net exponents
 *   [L:1, T:–1], so they match the dimensionality “L·T⁻¹”.
 */
OCArrayRef
SIDimensionalityCreateArrayOfQuantitiesWithSameReducedDimensionality(SIDimensionalityRef theDim);


/**
 * @brief Retrieves all built-in dimensionalities that share the same reduced dimensionality as theDim.
 *
 * @param theDim
 *   An immutable `SIDimensionalityRef` whose reduced dimensionality to match.
 * @return
 *   An `OCArrayRef` of `SIDimensionalityRef` objects, ordered from those with the
 *   largest raw exponents (numerator or denominator) down to the fully reduced form.
 *   The returned array and its elements are managed internally; do not
 *   retain or release them.
 *
 * @details
 *   Useful for seeing alternate representations that collapse to the same
 *   reduced dimensionality.  E.g., “L²/T²”, “L·T⁻²”, and “L²·T⁻²” all reduce to “L·T⁻²”.
 */
OCArrayRef
SIDimensionalityCreateArrayWithSameReducedDimensionality(SIDimensionalityRef theDim);


#pragma mark — String Output

/**
 * @brief Prints a concise, human-readable representation of theDim to stdout.
 *
 * @param theDim
 *   An immutable `SIDimensionalityRef` to display.
 *
 * @details
 *   Outputs only nonzero exponents in numerator/denominator form, using the
 *   base symbols L, M, T, I, ϴ, N, J.  For example:
 *     L·T⁻²
 */
void
SIDimensionalityShow(SIDimensionalityRef theDim);


/**
 * @brief Prints a detailed, fully-annotated description of theDim to stdout.
 *
 * @param theDim
 *   An immutable `SIDimensionalityRef` to display.
 *
 * @details
 *   Includes both numerator and denominator exponent lists, reduced-form summary,
 *   and classification (base, derived, dimensionless, etc.), all in terms of
 *   the dimensionality symbols L, M, T, I, ϴ, N, and J.
 */
void
SIDimensionalityShowFull(SIDimensionalityRef theDim);


/**
 * @brief Dimensionality constants for various physical quantities.
 */
#define kSIQuantityDimensionless                        STR("dimensionless")
#define kSIQuantityLength                               STR("length")
#define kSIQuantityInverseLength                        STR("inverse length")
#define kSIQuantityWavenumber                           STR("wavenumber")
#define kSIQuantityLengthRatio                          STR("length ratio")
#define kSIQuantityPlaneAngle                           STR("plane angle")
#define kSIQuantityMass                                 STR("mass")
#define kSIQuantityInverseMass                          STR("inverse mass")
#define kSIQuantityMassRatio                            STR("mass ratio")
#define kSIQuantityTime                                 STR("time")
#define kSIQuantityInverseTime                          STR("inverse time")
#define kSIQuantityFrequency                            STR("frequency")
#define kSIQuantityRadioactivity                        STR("radioactivity")
#define kSIQuantityTimeRatio                            STR("time ratio")
#define kSIQuantityFrequencyRatio                       STR("frequency ratio")
#define kSIQuantityInverseTimeSquared                   STR("inverse time squared")
#define kSIQuantityCurrent                              STR("current")
#define kSIQuantityInverseCurrent                       STR("inverse current")
#define kSIQuantityCurrentRatio                         STR("current ratio")
#define kSIQuantityTemperature                          STR("temperature")
#define kSIQuantityInverseTemperature                   STR("inverse temperature")
#define kSIQuantityTemperatureRatio                     STR("temperature ratio")
#define kSIQuantityTemperatureGradient                  STR("temperature gradient")
#define kSIQuantityAmount                               STR("amount")
#define kSIQuantityInverseAmount                        STR("inverse amount")
#define kSIQuantityAmountRatio                          STR("amount ratio")
#define kSIQuantityLuminousIntensity                    STR("luminous intensity")
#define kSIQuantityInverseLuminousIntensity             STR("inverse luminous intensity")
#define kSIQuantityLuminousIntensityRatio               STR("luminous intensity ratio")
#define kSIQuantityArea                                 STR("area")
#define kSIQuantityInverseArea                          STR("inverse area")
#define kSIQuantityAreaRatio                            STR("area ratio")
#define kSIQuantitySolidAngle                           STR("solid angle")
#define kSIQuantityVolume                               STR("volume")
#define kSIQuantityInverseVolume                        STR("inverse volume")
#define kSIQuantityVolumeRatio                          STR("volume ratio")
#define kSIQuantitySpeed                                STR("speed")
#define kSIQuantityVelocity                             STR("velocity")
#define kSIQuantityLinearMomentum                       STR("linear momentum")
#define kSIQuantityAngularMomentum                      STR("angular momentum")
#define kSIQuantityMomentOfInertia                      STR("moment of inertia")
#define kSIQuantityAcceleration                         STR("acceleration")
#define kSIQuantityMassFlowRate                         STR("mass flow rate")
#define kSIQuantityMassFlux                             STR("mass flux")
#define kSIQuantityDensity                              STR("density")
#define kSIQuantitySpecificGravity                      STR("specific gravity")
#define kSIQuantitySpecificSurfaceArea                  STR("specific surface area")
#define kSIQuantitySurfaceAreaToVolumeRatio             STR("surface area to volume ratio")
#define kSIQuantitySurfaceDensity                       STR("surface density")
#define kSIQuantitySpecificVolume                       STR("specific volume")
#define kSIQuantityCurrentDensity                       STR("current density")
#define kSIQuantityMagneticFieldStrength                STR("magnetic field strength")
#define kSIQuantityLuminance                            STR("luminance")
#define kSIQuantityRefractiveIndex                      STR("refractive index")
#define kSIQuantityFluidity                             STR("fluidity")
#define kSIQuantityMomentOfForce                        STR("moment of force")
#define kSIQuantitySurfaceTension                       STR("surface tension")
#define kSIQuantitySurfaceEnergy                        STR("surface energy")
#define kSIQuantityAngularSpeed                         STR("angular speed")
#define kSIQuantityAngularVelocity                      STR("angular velocity")
#define kSIQuantityAngularAcceleration                  STR("angular acceleration")
#define kSIQuantityHeatFluxDensity                      STR("heat flux density")
#define kSIQuantityIrradiance                           STR("irradiance")
#define kSIQuantitySpectralRadiantFluxDensity           STR("spectral radiant flux density")
#define kSIQuantityHeatCapacity                         STR("heat capacity")
#define kSIQuantityEntropy                              STR("entropy")
#define kSIQuantitySpecificHeatCapacity                 STR("specific heat capacity")
#define kSIQuantitySpecificEntropy                      STR("specific entropy")
#define kSIQuantitySpecificEnergy                       STR("specific energy")
#define kSIQuantityThermalConductance                   STR("thermal conductance")
#define kSIQuantityThermalConductivity                  STR("thermal conductivity")
#define kSIQuantityEnergyDensity                        STR("energy density")
#define kSIQuantityElectricFieldStrength                STR("electric field strength")
#define kSIQuantityElectricFieldGradient                STR("electric field gradient")
#define kSIQuantityElectricChargeDensity                STR("electric charge density")
#define kSIQuantitySurfaceChargeDensity                 STR("surface charge density")
#define kSIQuantityElectricFlux                         STR("electric flux")
#define kSIQuantityElectricFluxDensity                  STR("electric flux density")
#define kSIQuantityElectricDisplacement                 STR("electric displacement")
#define kSIQuantityPermittivity                         STR("permittivity")
#define kSIQuantityPermeability                         STR("permeability")
#define kSIQuantityMolarEnergy                          STR("molar energy")
#define kSIQuantityMolarEntropy                         STR("molar entropy")
#define kSIQuantityMolarHeatCapacity                    STR("molar heat capacity")
#define kSIQuantityMolarMass                            STR("molar mass")
#define kSIQuantityMolality                             STR("molality")
#define kSIQuantityDiffusionFlux                        STR("diffusion flux")
#define kSIQuantityMassToChargeRatio                    STR("mass to charge ratio")
#define kSIQuantityChargeToMassRatio                    STR("charge to mass ratio")
#define kSIQuantityRadiationExposure                    STR("radiation exposure")
#define kSIQuantityAbsorbedDoseRate                     STR("absorbed dose rate")
#define kSIQuantityRadiantIntensity                     STR("radiant intensity")
#define kSIQuantitySpectralRadiantIntensity             STR("spectral radiant intensity")
#define kSIQuantityRadiance                             STR("radiance")
#define kSIQuantitySpectralRadiance                     STR("spectral radiance")
#define kSIQuantityPorosity                             STR("porosity")
#define kSIQuantityAngularFrequency                     STR("angular frequency")
#define kSIQuantityForce                                STR("force")
#define kSIQuantityTorque                               STR("torque")
#define kSIQuantityPressure                             STR("pressure")
#define kSIQuantityStress                               STR("stress")
#define kSIQuantityElasticModulus                       STR("elastic modulus")
#define kSIQuantityCompressibility                      STR("compressibility")
#define kSIQuantityStressOpticCoefficient               STR("stress-optic coefficient")
#define kSIQuantityPressureGradient                     STR("pressure gradient")
#define kSIQuantityEnergy                               STR("energy")
#define kSIQuantitySpectralRadiantEnergy                STR("spectral radiant energy")
#define kSIQuantityPower                                STR("power")
#define kSIQuantitySpectralPower                        STR("spectral power")
#define kSIQuantityVolumePowerDensity                   STR("volume power density")
#define kSIQuantitySpecificPower                        STR("specific power")
#define kSIQuantityRadiantFlux                          STR("radiant flux")
#define kSIQuantityElectricCharge                       STR("electric charge")
#define kSIQuantityAmountOfElectricity                  STR("amount of electricity")
#define kSIQuantityElectricPotentialDifference          STR("electric potential difference")
#define kSIQuantityElectromotiveForce                   STR("electromotive force")
#define kSIQuantityElectricPolarizability               STR("electric polarizability")
#define kSIQuantityElectricDipoleMoment                 STR("electric dipole moment")
#define kSIQuantityVoltage                              STR("voltage")
#define kSIQuantityCapacitance                          STR("capacitance")
#define kSIQuantityElectricResistance                   STR("electric resistance")
#define kSIQuantityElectricResistancePerLength          STR("electric resistance per length")
#define kSIQuantityElectricResistivity                  STR("electric resistivity")
#define kSIQuantityElectricConductance                  STR("electric conductance")
#define kSIQuantityElectricConductivity                 STR("electric conductivity")
#define kSIQuantityElectricalMobility                   STR("electrical mobility")
#define kSIQuantityMolarConductivity                    STR("molar conductivity")
#define kSIQuantityMagneticDipoleMoment                 STR("magnetic dipole moment")
#define kSIQuantityMagneticDipoleMomentRatio            STR("magnetic dipole moment ratio")
#define kSIQuantityMagneticFlux                         STR("magnetic flux")
#define kSIQuantityMagneticFluxDensity                  STR("magnetic flux density")
#define kSIQuantityMolarMagneticSusceptibility          STR("molar magnetic susceptibility")
#define kSIQuantityInverseMagneticFluxDensity           STR("inverse magnetic flux density")
#define kSIQuantityMagneticFieldGradient                STR("magnetic field gradient")
#define kSIQuantityInductance                           STR("inductance")
#define kSIQuantityLuminousFlux                         STR("luminous flux")
#define kSIQuantityLuminousFluxDensity                  STR("luminous flux density")
#define kSIQuantityLuminousEnergy                       STR("luminous energy")
#define kSIQuantityIlluminance                          STR("illuminance")
#define kSIQuantityAbsorbedDose                         STR("absorbed dose")
#define kSIQuantityDoseEquivalent                       STR("dose equivalent")
#define kSIQuantityCatalyticActivity                    STR("catalytic activity")
#define kSIQuantityCatalyticActivityConcentration       STR("catalytic activity concentration")
#define kSIQuantityCatalyticActivityContent             STR("catalytic activity content")
#define kSIQuantityAction                               STR("action")
#define kSIQuantityReducedAction                        STR("reduced action")
#define kSIQuantityKinematicViscosity                   STR("kinematic viscosity")
#define kSIQuantityDiffusionCoefficient                 STR("diffusion coefficient")
#define kSIQuantityCirculation                          STR("circulation")
#define kSIQuantityDynamicViscosity                     STR("dynamic viscosity")
#define kSIQuantityAmountConcentration                  STR("amount concentration")
#define kSIQuantityMassConcentration                    STR("mass concentration")
#define kSIQuantityChargePerAmount                      STR("charge per amount")
#define kSIQuantityGravitationalConstant                STR("gravitational constant")
#define kSIQuantityLengthPerVolume                      STR("distance per volume")
#define kSIQuantityVolumePerLength                      STR("volume per length")
#define kSIQuantityVolumetricFlowRate                   STR("volumetric flow rate")
#define kSIQuantityFrequencyPerMagneticFluxDensity      STR("frequency per magnetic flux density")
#define kSIQuantityPowerPerLuminousFlux                 STR("power per luminous flux")
#define kSIQuantityLuminousEfficacy                     STR("luminous efficacy")
#define kSIQuantityRockPermeability                     STR("rock permeability")
#define kSIQuantityGyromagneticRatio                    STR("gyromagnetic ratio")
#define kSIQuantityHeatTransferCoefficient              STR("heat transfer coefficient")
#define kSIQuantityGasPermeance                         STR("gas permeance")
#define kSIQuantityPowerPerAreaPerTemperatureToFourthPower        STR("stefan-boltzmann constant")
#define kSIQuantityFirstHyperPolarizability             STR("first hyperpolarizability")
#define kSIQuantitySecondHyperPolarizability            STR("second hyperpolarizability")
#define kSIQuantityElectricQuadrupoleMoment             STR("electric quadrupole moment")
#define kSIQuantityMagnetizability                      STR("magnetizability")
#define kSIQuantitySecondRadiationConstant              STR("second radiation constant")
#define kSIQuantityWavelengthDisplacementConstant       STR("wavelength displacement constant")
#define kSIQuantityFineStructureConstant                STR("fine structure constant")
#define kSIQuantityRatePerAmountConcentrationPerTime    STR("inverse amount concentration inverse time")



#ifdef __cplusplus
}
#endif

#endif /* SIDIMENSIONALITY_H */