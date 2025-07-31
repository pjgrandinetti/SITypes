/**
 * @file SIUnitKey.h
 * @brief Unit expression key generation functions for preventing duplicate unit registrations.
 *
 * This module provides functions to generate unique keys for unit expressions, ensuring
 * that equivalent expressions like "lb*ft^2/s^2", "ft^2*lb/s^2", and 
 * "(ft^2*lb)/s^2" all produce the same standardized key.
 *
 * The canonical form uses the following conventions:
 * - Alphabetical ordering of unit symbols (e.g., "ft^2•lb" not "lb•ft^2")
 * - Power consolidation for repeated symbols (e.g., "m*m" → "m^2")
 * - Consistent formatting with "•" for multiplication
 * - Proper parentheses for complex denominators
 * - Multiple division handling (e.g., "/s/s" → "/s^2")
 *
 * @note This prevents duplicate entries in the unit library when equivalent
 *       expressions are proposed for registration.
 */

#ifndef SIUnitKey_h
#define SIUnitKey_h

#include <stdbool.h>

// Forward declarations from SILibrary.h
typedef const struct impl_OCString * OCStringRef;
typedef struct impl_OCString * OCMutableStringRef;

/**
 * @brief Creates a cleaned form of a unit expression by grouping identical symbols.
 * 
 * This function parses a unit expression and groups identical unit symbols in both
 * the numerator and denominator separately, without performing cancellation between
 * numerator and denominator. This is useful for organizing expressions while
 * preserving the original structure.
 *
 * Examples:
 * - "m*m*kg/s/s" → "kg•m^2/s^2"
 * - "ft*lb*ft/s" → "ft^2•lb/s"  
 * - "kg*m*m*kg" → "kg^2•m^2"
 * - "m/s/m" → "m/(m•s)"
 *
 * @param expression The unit expression to clean (e.g., "m*m*kg/s/s")
 * @return A new OCStringRef containing the cleaned form, or NULL if input is invalid.
 *         The caller is responsible for releasing the returned string.
 *
 * @note The cleaned form uses:
 *       - Grouping of identical symbols within numerator and denominator
 *       - "•" character for multiplication  
 *       - "^" for powers (omitted for power of 1)
 *       - No cancellation between numerator and denominator
 *
 * @see SIUnitCreateCleanedAndReducedExpression
 * @ingroup SIUnit
 */
OCStringRef SIUnitCreateCleanedExpression(OCStringRef expression);

/**
 * @brief Reduces a unit expression by combining like terms and canceling between numerator/denominator.
 *
 * This function takes a unit expression and performs full algebraic reduction by
 * combining powers of the same unit symbols and canceling terms between numerator
 * and denominator. This produces the most simplified form possible.
 *
 * Examples:
 * - "in^4/in^2" → "in^2"
 * - "ft^3/ft" → "ft^2" 
 * - "m^6/m^2" → "m^4"
 * - "kg•m^2/m" → "kg•m"
 * - "s^4/(s^2•s)" → "s"
 * - "m*kg/m" → "kg"
 *
 * @param expression The unit expression to reduce
 * @return A new reduced expression, or NULL if the input is invalid
 *
 * @note The caller is responsible for releasing the returned OCStringRef.
 * @see SIUnitCreateCleanedExpression
 * @ingroup SIUnit
 */
OCStringRef SIUnitCreateCleanedAndReducedExpression(OCStringRef expression);

/**
 * @brief Checks if two unit expressions are equivalent.
 *
 * This function determines if two unit expressions represent the same physical
 * unit by comparing their canonical forms. This is useful for detecting when
 * a "new" unit expression is actually equivalent to an existing one.
 *
 * Examples of equivalent expressions:
 * - "lb*ft^2/s^2" ≡ "ft^2*lb/s^2" 
 * - "m*kg" ≡ "kg*m"
 * - "m*m" ≡ "m^2"
 * - "ft*ft*lb/s/s" ≡ "lb*ft^2/s^2"
 *
 * @param expr1 First unit expression to compare
 * @param expr2 Second unit expression to compare  
 * @return true if the expressions are equivalent, false otherwise
 *
 * @note This function first checks for exact string equality for efficiency,
 *       then falls back to library key comparison if needed.
 *
 * @see SIUnitCreateLibraryKey
 * @ingroup SIUnit
 */
bool SIUnitAreExpressionsEquivalent(OCStringRef expr1, OCStringRef expr2);


OCMutableStringRef SIUnitCreateNormalizedExpression(OCStringRef expression, bool forLibraryLookup);

#endif /* SIUnitKey_h */
