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

/**
 * @brief Creates a canonical (normalized) form of a unit expression.
 * 
 * This function parses a unit expression and returns a standardized representation
 * that is consistent regardless of the input ordering or formatting. Equivalent
 * expressions will always produce the same canonical form.
 *
 * Examples:
 * - "lb*ft^2/s^2" → "ft^2•lb/s^2"
 * - "ft^2*lb/s^2" → "ft^2•lb/s^2"  
 * - "(ft^2*lb)/s^2" → "ft^2•lb/s^2"
 * - "m*kg" → "kg•m"
 * - "kg*m" → "kg•m"
 * - "m*m" → "m^2"
 *
 * @param expression The unit expression to canonicalize (e.g., "lb*ft^2/s^2")
 * @return A new OCStringRef containing the canonical form, or NULL if input is NULL.
 *         The caller is responsible for releasing the returned string.
 *
 * @note The canonical form uses:
 *       - Alphabetical ordering of symbols
 *       - "•" character for multiplication  
 *       - "^" for powers (omitted for power of 1)
 *       - Parentheses around multi-term denominators
 *
 * @see SIUnitAreExpressionsEquivalent
 * @ingroup SIUnit
 */
OCStringRef SIUnitCreateLibraryKey(OCStringRef expression);

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

#endif /* SIUnitKey_h */
