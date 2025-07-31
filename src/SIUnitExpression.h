//
//  SIUnitExpression.h
//  SITypes
//
//  Created by GitHub Copilot on 7/30/25.
//  Copyright © 2025 PhySy Ltd. All rights reserved.
//
#ifndef SIUnitExpression_h
#define SIUnitExpression_h
#include "SILibrary.h"
/*!
 * @file SIUnitExpression.h
 * @brief Internal data structures and functions for unit expression parsing and processing.
 *
 * This header defines the internal data structures and helper functions used
 * by the SIUnitCreateCleanedExpression and SIUnitCreateCleanedAndReducedExpression
 * functions. These structures represent parsed unit expressions in a form that
 * can be easily manipulated for grouping, sorting, and cancellation operations.
 */
#pragma mark - Data Structures
/*!
 * @struct SIUnitTerm
 * @brief Represents a single unit symbol with its power.
 *
 * A term consists of a unit symbol (e.g., "m", "kg") and an integer power.
 * This is the basic building block for representing unit expressions.
 */
typedef struct SIUnitTerm {
    OCStringRef symbol; /*!< Unit symbol (e.g., "m", "kg") */
    int power;          /*!< Power of the symbol (can be negative) */
} SIUnitTerm;
/*!
 * @struct SIUnitExpression
 * @brief Represents a complete unit expression with numerator and denominator.
 *
 * An expression consists of two arrays of terms: one for the numerator
 * and one for the denominator. This allows for easy manipulation of
 * expressions like "kg•m^2/s^2".
 */
typedef struct SIUnitExpression {
    OCArrayRef numerator;   /*!< Array of SIUnitTerm for numerator */
    OCArrayRef denominator; /*!< Array of SIUnitTerm for denominator */
} SIUnitExpression;
#pragma mark - Term Management
/*!
 * @brief Creates a new unit term with the specified symbol and power.
 *
 * @param symbol The unit symbol (ownership transferred to the term)
 * @param power The power of the symbol
 * @return A new SIUnitTerm structure, or NULL on failure
 *
 * @note The caller is responsible for releasing the returned term with siueReleaseTerm()
 */
SIUnitTerm* siueCreateTerm(OCStringRef symbol, int power);
/*!
 * @brief Creates a copy of an existing term.
 *
 * @param term The term to copy
 * @return A new SIUnitTerm structure, or NULL on failure
 *
 * @note The caller is responsible for releasing the returned term with siueReleaseTerm()
 */
SIUnitTerm* siueCopyTerm(const SIUnitTerm* term);
/*!
 * @brief Releases a unit term and its associated memory.
 *
 * @param term The term to release (can be NULL)
 */
void siueReleaseTerm(SIUnitTerm* term);
#pragma mark - Expression Management
/*!
 * @brief Creates a new unit expression with the specified numerator and denominator.
 *
 * @param numerator Array of SIUnitTerm for the numerator (ownership transferred)
 * @param denominator Array of SIUnitTerm for the denominator (ownership transferred, can be NULL)
 * @return A new SIUnitExpression structure, or NULL on failure
 *
 * @note The caller is responsible for releasing the returned expression with siueRelease()
 */
SIUnitExpression* siueCreateExpression(OCArrayRef numerator, OCArrayRef denominator);
/*!
 * @brief Creates a copy of an existing expression.
 *
 * @param expr The expression to copy
 * @return A new SIUnitExpression structure, or NULL on failure
 *
 * @note The caller is responsible for releasing the returned expression with siueRelease()
 */
SIUnitExpression* siueCopyExpression(const SIUnitExpression* expr);
/*!
 * @brief Releases a unit expression and its associated memory.
 *
 * @param expr The expression to release (can be NULL)
 */
void siueRelease(SIUnitExpression* expr);
#pragma mark - Parser Helper Functions
/*!
 * @brief Applies a power to all terms in a term list.
 *
 * This function is used to handle parenthetical expressions with exponents,
 * such as "(m*kg)^2" which becomes "m^2*kg^2".
 *
 * @param term_list The list of terms to modify
 * @param power The power to apply to each term
 * @return The modified term list
 */
OCArrayRef siueApplyPowerToTermList(OCArrayRef term_list, int power);
/*!
 * @brief Applies a power to a full unit expression (numerator and denominator).
 *
 * @param expression The expression to modify
 * @param power The power to apply to each term
 * @return The modified expression
 */
SIUnitExpression* siueApplyPowerToExpression(SIUnitExpression* expression, int power);
/*!
 * @brief Gets the parsed expression from the parser.
 *
 * @return The currently parsed expression, or NULL if none
 */
SIUnitExpression* siueGetParsedExpression(void);
/*!
 * @brief Sets the parsed expression (used by parser).
 *
 * @param expr The expression to set as the current parsed expression
 */
void siueSetParsedExpression(SIUnitExpression* expr);
/*!
 * @brief Clears the parsed expression and releases its memory.
 */
void siueClearParsedExpression(void);
#pragma mark - Processing Functions
/*!
 * @brief Groups identical terms in a term list by combining their powers.
 *
 * This function combines terms with the same symbol by adding their powers.
 * For example, "m*m^2" becomes "m^3".
 *
 * @param terms Mutable array of SIUnitTerm to process
 */
void siueGroupIdenticalTerms(OCMutableArrayRef terms);
/*!
 * @brief Sorts terms alphabetically by symbol.
 *
 * @param terms Mutable array of SIUnitTerm to sort
 */
void siueSortTermsAlphabetically(OCMutableArrayRef terms);
/*!
 * @brief Cancels terms between numerator and denominator.
 *
 * This function performs algebraic reduction by subtracting powers of
 * identical symbols between numerator and denominator.
 *
 * @param expr The expression to reduce
 */
void siueCancelTerms(SIUnitExpression* expr);
/*!
 * @brief Formats a unit expression as a string.
 *
 * @param expr The expression to format
 * @param reduced Whether this is for the reduced version (affects formatting)
 * @return A formatted string representation, or NULL on failure
 *
 * @note The caller is responsible for releasing the returned string
 */
OCStringRef siueFormatExpression(const SIUnitExpression* expr, bool reduced);
#pragma mark - Validation Functions
/*!
 * @brief Validates that a symbol is in the allowed token symbols array.
 *
 * @param symbol The symbol to validate
 * @return true if the symbol is valid, false otherwise
 */
bool siueValidateSymbol(OCStringRef symbol);
/*!
 * @brief Parses a normalized expression string into a SIUnitExpression.
 *
 * @param normalized_expr The normalized expression string to parse
 * @return A parsed expression, or NULL on failure
 *
 * @note The caller is responsible for releasing the returned expression
 */
SIUnitExpression* siueParseExpression(OCStringRef normalized_expr);
#pragma mark - External Variables
/*!
 * @brief Global error string for parser errors.
 */
extern OCStringRef siueError;
#endif /* SIUnitExpression_h */
