//
//  SIUnitExpression.c
//  SITypes
//
//  Unit expression processing functions for cleaning and reducing expressions
//  involving ~951 valid token unit symbols with multiplication, division, and integer powers.
//
//  Key functions:
//  - SIUnitCreateCleanedExpression: Groups and sorts without cancellation
//  - SIUnitCreateCleanedAndReducedExpression: Full algebraic reduction with cancellation
//
//  Created by GitHub Copilot on 7/30/25.
//  Copyright © 2025 PhySy Ltd. All rights reserved.
//
#include "SIUnitExpression.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "SIUnitExpressionParser.tab.h"
// External lex/yacc parser functions (siue prefix for namespace isolation)
extern int siueparse(void);
typedef struct yy_buffer_state* YY_BUFFER_STATE;
extern YY_BUFFER_STATE siue_scan_string(const char* str);
extern void siue_delete_buffer(YY_BUFFER_STATE buffer);
// Forward declarations for power operations and expression processing
OCArrayRef siueApplyPowerToTermList(OCArrayRef term_list, int power);
SIUnitExpression* siueApplyPowerToExpression(SIUnitExpression* expression, int power);
SIUnitExpression* siueApplyFractionalPowerToExpression(SIUnitExpression* expression, double power);
// Global error state for parser communication
OCStringRef siueError = NULL;
#pragma mark - Term Management
/**
 * Creates a new term with unit symbol and power.
 * Used as building block for expression parsing (e.g., "m" with power 2 for "m^2").
 */
SIUnitTerm* siueCreateTerm(OCStringRef symbol, int power) {
    if (!symbol) return NULL;
    SIUnitTerm* term = malloc(sizeof(SIUnitTerm));
    if (!term) return NULL;
    term->symbol = OCStringCreateCopy(symbol);
    term->power = power;
    return term;
}
/**
 * Creates deep copy of a term for array manipulation.
 */
SIUnitTerm* siueCopyTerm(const SIUnitTerm* term) {
    if (!term) return NULL;
    return siueCreateTerm(term->symbol, term->power);
}
/**
 * Releases term memory and symbol reference.
 */
void siueReleaseTerm(SIUnitTerm* term) {
    if (!term) return;
    if (term->symbol) {
        OCRelease(term->symbol);
    }
    free(term);
}
#pragma mark - Expression Management
/**
 * Creates expression with numerator and denominator term arrays.
 * Performs deep copy of all terms to ensure memory safety.
 */
SIUnitExpression* siueCreateExpression(OCArrayRef numerator, OCArrayRef denominator) {
    if (!numerator) return NULL;
    SIUnitExpression* expr = malloc(sizeof(SIUnitExpression));
    if (!expr) return NULL;
    // Deep copy numerator terms
    OCIndex count = OCArrayGetCount(numerator);
    OCMutableArrayRef numCopy = OCArrayCreateMutable(count, NULL);
    if (!numCopy) {
        free(expr);
        return NULL;
    }
    for (OCIndex i = 0; i < count; i++) {
        SIUnitTerm* term = (SIUnitTerm*)OCArrayGetValueAtIndex(numerator, i);
        SIUnitTerm* termCopy = siueCopyTerm(term);
        if (termCopy) {
            if (!OCArrayAppendValue(numCopy, termCopy)) {
                // Append failed - clean up termCopy and abort
                siueReleaseTerm(termCopy);
                // Clean up any terms already in numCopy
                siueReleaseTermArray(numCopy);
                OCRelease(numCopy);
                free(expr);
                return NULL;
            }
        }
    }
    expr->numerator = numCopy;
    // Deep copy denominator terms if present
    if (denominator) {
        count = OCArrayGetCount(denominator);
        OCMutableArrayRef denCopy = OCArrayCreateMutable(count, NULL);
        if (!denCopy) {
            // Clean up numerator array and expression
            siueReleaseTermArray(numCopy);
            OCRelease(numCopy);
            free(expr);
            return NULL;
        }
        for (OCIndex i = 0; i < count; i++) {
            SIUnitTerm* term = (SIUnitTerm*)OCArrayGetValueAtIndex(denominator, i);
            SIUnitTerm* termCopy = siueCopyTerm(term);
            if (termCopy) {
                if (!OCArrayAppendValue(denCopy, termCopy)) {
                    // Append failed - clean up termCopy and abort
                    siueReleaseTerm(termCopy);
                    // Clean up denominator terms already added
                    siueReleaseTermArray(denCopy);
                    OCRelease(denCopy);
                    // Clean up numerator array
                    siueReleaseTermArray(numCopy);
                    OCRelease(numCopy);
                    free(expr);
                    return NULL;
                }
            }
        }
        expr->denominator = denCopy;
    } else {
        expr->denominator = NULL;
    }
    return expr;
}
/**
 * Creates deep copy of entire expression.
 */
SIUnitExpression* siueCopyExpression(const SIUnitExpression* expr) {
    if (!expr) return NULL;
    return siueCreateExpression(expr->numerator, expr->denominator);
}
/**
 * Releases expression and all contained terms.
 * Ensures complete cleanup to prevent memory leaks.
 */
void siueRelease(SIUnitExpression* expr) {
    if (!expr) return;
    // Release all numerator terms
    if (expr->numerator) {
        OCIndex count = OCArrayGetCount(expr->numerator);
        for (OCIndex i = 0; i < count; i++) {
            SIUnitTerm* term = (SIUnitTerm*)OCArrayGetValueAtIndex(expr->numerator, i);
            siueReleaseTerm(term);
        }
        OCRelease(expr->numerator);
    }
    // Release all denominator terms
    if (expr->denominator) {
        OCIndex count = OCArrayGetCount(expr->denominator);
        for (OCIndex i = 0; i < count; i++) {
            SIUnitTerm* term = (SIUnitTerm*)OCArrayGetValueAtIndex(expr->denominator, i);
            siueReleaseTerm(term);
        }
        OCRelease(expr->denominator);
    }
    free(expr);
}
/**
 * Utility function to release array of terms.
 */
void siueReleaseTermArray(OCArrayRef term_array) {
    if (!term_array) return;
    OCIndex count = OCArrayGetCount(term_array);
    for (OCIndex i = 0; i < count; i++) {
        SIUnitTerm* term = (SIUnitTerm*)OCArrayGetValueAtIndex(term_array, i);
        siueReleaseTerm(term);
    }
    OCRelease(term_array);
}
#pragma mark - Parser Helper Functions
/**
 * Applies integer power to all terms in array.
 * Used for parenthetical expressions like (m*kg)^2.
 */
OCArrayRef siueApplyPowerToTermList(OCArrayRef term_list, int power) {
    if (!term_list || power == 0) return term_list;
    OCIndex count = OCArrayGetCount(term_list);
    for (OCIndex i = 0; i < count; i++) {
        SIUnitTerm* term = (SIUnitTerm*)OCArrayGetValueAtIndex(term_list, i);
        if (term) {
            term->power *= power;
        }
    }
    return term_list;
}
/**
 * Applies integer power to entire expression.
 * Handles negative powers by swapping numerator/denominator: (a/b)^(-n) = (b/a)^n.
 */
SIUnitExpression* siueApplyPowerToExpression(SIUnitExpression* expression, int power) {
    if (!expression || power == 0) return expression;
    if (power < 0) {
        // Swap numerator and denominator for negative powers
        OCArrayRef oldNumerator = expression->numerator;
        OCArrayRef oldDenominator = expression->denominator;
        expression->numerator = oldDenominator;
        expression->denominator = oldNumerator;
        power = -power;  // Apply positive power after swap
    }
    // Apply power to both numerator and denominator
    if (expression->numerator) {
        siueApplyPowerToTermList(expression->numerator, power);
    }
    if (expression->denominator) {
        siueApplyPowerToTermList(expression->denominator, power);
    }
    return expression;
}
/**
 * Validates and applies fractional power to expression.
 * Rejects powers that would result in non-integer exponents (per spec: no fractional powers allowed).
 * Returns NULL if any resulting power would be fractional.
 */
SIUnitExpression* siueApplyFractionalPowerToExpression(SIUnitExpression* expression, double power) {
    if (!expression) return NULL;
    // Validate that fractional power would yield integer results for all terms
    if (expression->numerator) {
        OCIndex count = OCArrayGetCount(expression->numerator);
        for (OCIndex i = 0; i < count; i++) {
            SIUnitTerm* term = (SIUnitTerm*)OCArrayGetValueAtIndex(expression->numerator, i);
            if (term) {
                double resultPower = term->power * power;
                if (fabs(resultPower - round(resultPower)) > 1e-10) {
                    return NULL;  // Would result in fractional power - forbidden
                }
            }
        }
    }
    // Check denominator terms
    if (expression->denominator) {
        OCIndex count = OCArrayGetCount(expression->denominator);
        for (OCIndex i = 0; i < count; i++) {
            SIUnitTerm* term = (SIUnitTerm*)OCArrayGetValueAtIndex(expression->denominator, i);
            if (term) {
                double resultPower = term->power * power;
                if (fabs(resultPower - round(resultPower)) > 1e-10) {
                    return NULL;  // Would result in fractional power - forbidden
                }
            }
        }
    }
    // All powers would be integers - apply the fractional power
    if (expression->numerator) {
        OCIndex count = OCArrayGetCount(expression->numerator);
        for (OCIndex i = 0; i < count; i++) {
            SIUnitTerm* term = (SIUnitTerm*)OCArrayGetValueAtIndex(expression->numerator, i);
            if (term) {
                term->power = (int)round(term->power * power);
            }
        }
    }
    if (expression->denominator) {
        OCIndex count = OCArrayGetCount(expression->denominator);
        for (OCIndex i = 0; i < count; i++) {
            SIUnitTerm* term = (SIUnitTerm*)OCArrayGetValueAtIndex(expression->denominator, i);
            if (term) {
                term->power = (int)round(term->power * power);
            }
        }
    }
    return expression;
}
#pragma mark - Processing Functions
/**
 * Groups identical symbols by combining their powers.
 * Implements power consolidation: m*m*kg → kg•m^2.
 * Removes terms with zero power after consolidation.
 */
void siueGroupIdenticalTerms(OCMutableArrayRef terms) {
    if (!terms) return;
    OCIndex count = OCArrayGetCount(terms);
    // Combine powers for identical symbols
    if (count > 1) {
        for (OCIndex i = 0; i < count; i++) {
            SIUnitTerm* term1 = (SIUnitTerm*)OCArrayGetValueAtIndex(terms, i);
            if (!term1) continue;
            for (OCIndex j = i + 1; j < count; j++) {
                SIUnitTerm* term2 = (SIUnitTerm*)OCArrayGetValueAtIndex(terms, j);
                if (!term2) continue;
                if (OCStringEqual(term1->symbol, term2->symbol)) {
                    term1->power += term2->power;  // Combine powers
                    siueReleaseTerm(term2);
                    OCArrayRemoveValueAtIndex(terms, j);
                    count--;
                    j--;  // Adjust index after removal
                }
            }
        }
    }
    // Remove terms with zero power
    for (OCIndex i = count - 1; i >= 0; i--) {
        SIUnitTerm* term = (SIUnitTerm*)OCArrayGetValueAtIndex(terms, i);
        if (term && term->power == 0) {
            siueReleaseTerm(term);
            OCArrayRemoveValueAtIndex(terms, i);
        }
    }
}
// Alphabetical comparison function for term sorting
static int termCompare(const void* a, const void* b) {
    const SIUnitTerm* term1 = *(const SIUnitTerm**)a;
    const SIUnitTerm* term2 = *(const SIUnitTerm**)b;
    if (!term1 || !term1->symbol) return 1;
    if (!term2 || !term2->symbol) return -1;
    return OCStringCompare(term1->symbol, term2->symbol, 0);
}
/**
 * Sorts terms alphabetically by symbol.
 * Implements alphabetical ordering requirement: m*kg → kg•m.
 */
void siueSortTermsAlphabetically(OCMutableArrayRef terms) {
    if (!terms) return;
    OCIndex count = OCArrayGetCount(terms);
    if (count <= 1) return;
    // Create array of pointers for qsort
    SIUnitTerm** termArray = malloc(count * sizeof(SIUnitTerm*));
    if (!termArray) return;
    for (OCIndex i = 0; i < count; i++) {
        termArray[i] = (SIUnitTerm*)OCArrayGetValueAtIndex(terms, i);
    }
    // Sort alphabetically
    qsort(termArray, count, sizeof(SIUnitTerm*), termCompare);
    // Replace original array content with sorted terms
    while (OCArrayGetCount(terms) > 0) {
        OCArrayRemoveValueAtIndex(terms, 0);
    }
    for (OCIndex i = 0; i < count; i++) {
        OCArrayAppendValue(terms, termArray[i]);
    }
    free(termArray);
}
/**
 * Cancels identical terms between numerator and denominator.
 * Implements algebraic reduction: kg*m/kg → m, m^3/m^2 → m.
 * Used only by SIUnitCreateCleanedAndReducedExpression.
 */
void siueCancelTerms(SIUnitExpression* expr) {
    if (!expr || !expr->numerator || !expr->denominator) return;
    OCMutableArrayRef num = OCArrayCreateMutableCopy(expr->numerator);
    OCMutableArrayRef den = OCArrayCreateMutableCopy(expr->denominator);
    OCIndex numCount = OCArrayGetCount(num);
    OCIndex denCount = OCArrayGetCount(den);
    // Cancel matching symbols by subtracting powers
    for (OCIndex i = numCount - 1; i >= 0; i--) {
        SIUnitTerm* numTerm = (SIUnitTerm*)OCArrayGetValueAtIndex(num, i);
        if (!numTerm) continue;
        for (OCIndex j = denCount - 1; j >= 0; j--) {
            SIUnitTerm* denTerm = (SIUnitTerm*)OCArrayGetValueAtIndex(den, j);
            if (!denTerm) continue;
            if (OCStringEqual(numTerm->symbol, denTerm->symbol)) {
                // Subtract minimum power from both terms
                int minPower = (numTerm->power < denTerm->power) ? numTerm->power : denTerm->power;
                numTerm->power -= minPower;
                denTerm->power -= minPower;
                // Remove terms with zero power
                if (numTerm->power == 0) {
                    siueReleaseTerm(numTerm);
                    OCArrayRemoveValueAtIndex(num, i);
                    numCount--;
                }
                if (denTerm->power == 0) {
                    siueReleaseTerm(denTerm);
                    OCArrayRemoveValueAtIndex(den, j);
                    denCount--;
                }
                break;  // Found match, move to next numerator term
            }
        }
    }
    // Update expression with cancelled arrays
    if (expr->numerator) OCRelease(expr->numerator);
    if (expr->denominator) OCRelease(expr->denominator);
    expr->numerator = num;
    expr->denominator = (OCArrayGetCount(den) > 0) ? den : NULL;
    if (!expr->denominator && den) {
        OCRelease(den);
    }
}
/**
 * Moves terms with negative powers from numerator to denominator and returns the denominator.
 * Converts m^-2 → 1/m^2 for proper formatting.
 *
 * @param numerator The numerator array to process (modified in-place)
 * @param denominator The existing denominator array (may be NULL)
 * @return The denominator array with negative power terms added. If denominator was NULL,
 *         returns a new array that caller owns. If denominator was non-NULL, returns it
 *         with retain count increased (caller must OCRelease).
 */
OCMutableArrayRef siueCreateDenominatorWithNegativePowers(OCMutableArrayRef numerator, OCArrayRef denominator) {
    if (!numerator) return denominator ? (OCMutableArrayRef)OCRetain(denominator) : NULL;
    OCMutableArrayRef workingDenominator;
    if (denominator) {
        workingDenominator = (OCMutableArrayRef)OCRetain(denominator);  // Increase retain count
    } else {
        workingDenominator = OCArrayCreateMutable(10, NULL);  // Create new array
    }
    if (!workingDenominator) return NULL;
    // Move negative power terms to denominator
    OCIndex count = OCArrayGetCount(numerator);
    for (OCIndex i = count - 1; i >= 0; i--) {
        SIUnitTerm* term = (SIUnitTerm*)OCArrayGetValueAtIndex(numerator, i);
        if (term && term->power < 0) {
            // Create positive power term for denominator
            SIUnitTerm* newTerm = siueCreateTerm(term->symbol, -term->power);
            if (newTerm) {
                if (OCArrayAppendValue(workingDenominator, newTerm)) {
                    // Successfully added to denominator, now remove from numerator
                    siueReleaseTerm(term);
                    OCArrayRemoveValueAtIndex(numerator, i);
                } else {
                    // Failed to add to denominator, clean up newTerm
                    siueReleaseTerm(newTerm);
                }
            }
        }
    }
    return workingDenominator;
}
#pragma mark - Formatting Functions
/**
 * Formats expression as output string with proper notation.
 * Uses • for multiplication, / for division, ^ for powers (omitted when power=1).
 * Handles special cases: empty numerator (1/m), dimensionless (space character).
 */
OCStringRef siueCreateFormattedExpression(const SIUnitExpression* expr, bool reduced) {
    if (!expr) return NULL;
    OCMutableStringRef result = OCStringCreateMutable(256);
    // Check for special formatting cases
    bool hasEmptyNumerator = (!expr->numerator || OCArrayGetCount(expr->numerator) == 0);
    bool hasNonEmptyDenominator = (expr->denominator && OCArrayGetCount(expr->denominator) > 0);
    bool needsOuterParens = hasEmptyNumerator && hasNonEmptyDenominator;
    if (needsOuterParens) {
        OCStringAppendCString(result, "(");
    }
    // Format numerator terms with • separator
    if (expr->numerator && OCArrayGetCount(expr->numerator) > 0) {
        OCIndex count = OCArrayGetCount(expr->numerator);
        for (OCIndex i = 0; i < count; i++) {
            SIUnitTerm* term = (SIUnitTerm*)OCArrayGetValueAtIndex(expr->numerator, i);
            if (!term) continue;
            if (i > 0) {
                OCStringAppendCString(result, "•");
            }
            OCStringAppend(result, term->symbol);
            if (term->power != 1) {
                char powerStr[32];
                snprintf(powerStr, sizeof(powerStr), "^%d", term->power);
                OCStringAppendCString(result, powerStr);
            }
        }
    } else {
        // Empty numerator - output "1" for expressions like 1/m, space for dimensionless
        if (expr->denominator && OCArrayGetCount(expr->denominator) > 0) {
            OCStringAppendCString(result, "1");
        } else {
            OCStringAppendCString(result, " ");  // Dimensionless
        }
    }
    // Format denominator terms
    if (expr->denominator && OCArrayGetCount(expr->denominator) > 0) {
        OCStringAppendCString(result, "/");
        OCIndex count = OCArrayGetCount(expr->denominator);
        // Add parentheses for multiple terms in denominator
        bool needParens = count > 1;
        if (needParens) {
            OCStringAppendCString(result, "(");
        }
        for (OCIndex i = 0; i < count; i++) {
            SIUnitTerm* term = (SIUnitTerm*)OCArrayGetValueAtIndex(expr->denominator, i);
            if (!term) continue;
            if (i > 0) {
                OCStringAppendCString(result, "•");
            }
            OCStringAppend(result, term->symbol);
            if (term->power != 1) {
                char powerStr[32];
                snprintf(powerStr, sizeof(powerStr), "^%d", term->power);
                OCStringAppendCString(result, powerStr);
            }
        }
        if (needParens) {
            OCStringAppendCString(result, ")");
        }
    }
    if (needsOuterParens) {
        OCStringAppendCString(result, ")");
    }
    return result;  // Return the mutable string directly - it's already an OCStringRef
}
/**
 * Converts internal "1" result to space character for dimensionless output.
 * Per spec: dimensionless results should be space character, not "1".
 */
static OCStringRef siueCreateByConvertingDimensionlessOutput(OCStringRef formatted) {
    if (!formatted) return NULL;
    if (OCStringEqual(formatted, STR("1"))) {
        return OCStringCreateCopy(STR(" "));  // Dimensionless → space
    }
    return OCStringCreateCopy(formatted);  // Always return a copy to maintain "Create" semantics
}
#pragma mark - Global Variables for Parser
// Parser state management for lex/yacc communication
static SIUnitExpression* g_parsed_expression = NULL;
SIUnitExpression* siueGetParsedExpression(void) {
    return g_parsed_expression;
}
void siueClearParsedExpression(void) {
    if (g_parsed_expression) {
        siueRelease(g_parsed_expression);
        g_parsed_expression = NULL;
    }
}
void siueSetParsedExpression(SIUnitExpression* expr) {
    siueClearParsedExpression();
    g_parsed_expression = expr;
}
#pragma mark - Validation Functions
/**
 * Validates symbol against ~951 allowed token unit symbols.
 * Only symbols from SIUnitGetTokenSymbolsLib() are valid per spec.
 */
bool siueValidateSymbol(OCStringRef symbol) {
    if (!symbol) return false;
    OCArrayRef tokenSymbols = SIUnitGetTokenSymbolsLib();
    if (!tokenSymbols) return false;
    OCIndex count = OCArrayGetCount(tokenSymbols);
    for (OCIndex i = 0; i < count; i++) {
        OCStringRef validSymbol = (OCStringRef)OCArrayGetValueAtIndex(tokenSymbols, i);
        if (OCStringEqual(symbol, validSymbol)) {
            return true;
        }
    }
    return false;
}
// Parses normalized expressions using lex/yacc parser with siue prefix
// Returns parsed SIUnitExpression or NULL on failure/invalid symbols
SIUnitExpression* siueCreateParsedExpression(OCStringRef normalized_expr) {
    if (!normalized_expr) return NULL;
    // Clear previous state
    if (siueError) {
        OCRelease(siueError);
        siueError = NULL;
    }
    siueClearParsedExpression();
    // Convert to C string for lex/yacc
    const char* exprStr = OCStringGetCString(normalized_expr);
    if (!exprStr) return NULL;
    // Parse using lex/yacc with siue prefix
    YY_BUFFER_STATE buffer = siue_scan_string(exprStr);
    int parseResult = siueparse();
    siue_delete_buffer(buffer);
    if (parseResult != 0 || siueError) {
        siueClearParsedExpression();
        return NULL;
    }
    // Transfer ownership from global parser state
    SIUnitExpression* result = siueGetParsedExpression();
    if (result) {
        g_parsed_expression = NULL;  // Clear without freeing
        return result;
    }
    return NULL;
}
#pragma mark - Unicode Conversion Helpers
// Converts bullet characters (•) to asterisks (*) for internal parser compatibility
static OCStringRef siueCreateByConvertingBulletsToAsterisks(OCStringRef expression) {
    if (!expression) return NULL;
    OCMutableStringRef mutable = OCStringCreateMutableCopy(expression);
    if (!mutable) return NULL;
    OCStringFindAndReplace2(mutable, STR("•"), STR("*"));
    return mutable;
}
// Converts asterisks (*) back to bullet characters (•) for output formatting
static OCStringRef siueCreateByConvertingAsterisksToBullets(OCStringRef expression) {
    if (!expression) return NULL;
    OCMutableStringRef mutable = OCStringCreateMutableCopy(expression);
    if (!mutable) return NULL;
    OCStringFindAndReplace2(mutable, STR("*"), STR("•"));
    return mutable;  // Return mutable string directly - it's already an OCStringRef
}
#pragma mark - Main Public Functions
// Unicode normalization: converts operators to standard forms (×→*, ÷→/, μ→µ, superscripts→^n)
// Handles empty strings, spaces→"1", cleans whitespace around operators
// TODO: Integrate with existing SIUnitCreateNormalizedExpression when available
OCMutableStringRef SIUnitCreateNormalizedExpression(OCStringRef expression, bool something) {
    if (!expression) return NULL;
    // Handle empty and dimensionless cases
    if (OCStringGetLength(expression) == 0) {
        return OCStringCreateMutableCopy(STR("1"));
    }
    if (OCStringGetLength(expression) == 1 && OCStringEqual(expression, STR(" "))) {
        return OCStringCreateMutableCopy(STR("1"));  // Space → "1" for parser
    }
    OCMutableStringRef mutString = OCStringCreateMutableCopy(expression);
    // Trim leading and trailing whitespace
    OCStringTrimWhitespace(mutString);
    // Unicode operator normalizations per spec
    OCStringFindAndReplace(mutString, STR("×"), STR("*"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("÷"), STR("/"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("μ"), STR("µ"), OCRangeMake(0, OCStringGetLength(mutString)), 0);  // Greek mu → micro
    // Additional multiplication operators → *
    OCStringFindAndReplace(mutString, STR("•"), STR("*"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("⋅"), STR("*"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("∙"), STR("*"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("·"), STR("*"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    // Additional division operators → /
    OCStringFindAndReplace(mutString, STR("∕"), STR("/"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("⁄"), STR("/"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    // Superscript normalizations (negative powers first)
    OCStringFindAndReplace(mutString, STR("⁻⁰"), STR("^-0"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("⁻¹"), STR("^-1"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("⁻²"), STR("^-2"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("⁻³"), STR("^-3"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("⁻⁴"), STR("^-4"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("⁻⁵"), STR("^-5"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("⁻⁶"), STR("^-6"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("⁻⁷"), STR("^-7"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("⁻⁸"), STR("^-8"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("⁻⁹"), STR("^-9"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    // Positive superscripts
    OCStringFindAndReplace(mutString, STR("⁰"), STR("^0"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("¹"), STR("^1"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("²"), STR("^2"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("³"), STR("^3"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("⁴"), STR("^4"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("⁵"), STR("^5"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("⁶"), STR("^6"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("⁷"), STR("^7"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("⁸"), STR("^8"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("⁹"), STR("^9"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    // Clean up whitespace around operators
    OCStringFindAndReplace(mutString, STR(" * "), STR("*"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR(" / "), STR("/"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR(" ^ "), STR("^"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    return mutString;
}
// Checks if two unit expressions are equivalent after cleaning and normalization
bool SIUnitAreExpressionsEquivalent(OCStringRef expr1, OCStringRef expr2) {
    if (!expr1 || !expr2) return false;
    // Handle identical strings quickly
    if (OCStringEqual(expr1, expr2)) return true;
    // Get cleaned expressions for both inputs
    OCStringRef cleaned1 = SIUnitCreateCleanedExpression(expr1);
    OCStringRef cleaned2 = SIUnitCreateCleanedExpression(expr2);
    // If either cleaning failed, they can't be equivalent
    if (!cleaned1 || !cleaned2) {
        if (cleaned1) OCRelease(cleaned1);
        if (cleaned2) OCRelease(cleaned2);
        return false;
    }
    // Compare the cleaned expressions
    bool equivalent = OCStringEqual(cleaned1, cleaned2);
    OCRelease(cleaned1);
    OCRelease(cleaned2);
    return equivalent;
}
// Creates cleaned expression: normalizes, groups identical terms, sorts alphabetically
// Groups and sorts without power cancellation - returns clean formatted expression
OCStringRef SIUnitCreateCleanedExpression(OCStringRef expression) {
    if (!expression) return NULL;
    // Handle empty string case early
    if (OCStringGetLength(expression) == 0) return STR(" ");
    // Step 1: Normalize Unicode characters first
    OCMutableStringRef normalized = SIUnitCreateNormalizedExpression(expression, false);
    if (!normalized) return NULL;
    // Step 2: Convert bullet characters to asterisks for parsing
    OCStringRef preprocessed = siueCreateByConvertingBulletsToAsterisks(normalized);
    OCRelease(normalized);
    if (!preprocessed) return NULL;
    // Step 3: Parse the preprocessed expression
    SIUnitExpression* parsed = siueCreateParsedExpression(preprocessed);
    OCRelease(preprocessed);
    if (!parsed) return NULL;
    // Step 3: Process the expression (group and sort)
    // Work directly with the parsed expression arrays
    if (parsed->numerator) {
        OCMutableArrayRef mutableNum = OCArrayCreateMutableCopy(parsed->numerator);
        siueGroupIdenticalTerms(mutableNum);
        siueSortTermsAlphabetically(mutableNum);
        OCMutableArrayRef newDenominator = siueCreateDenominatorWithNegativePowers(mutableNum, parsed->denominator);
        if (parsed->numerator) OCRelease(parsed->numerator);
        if (parsed->denominator) OCRelease(parsed->denominator);
        parsed->numerator = mutableNum;
        parsed->denominator = newDenominator;
    }
    if (parsed->denominator) {
        OCMutableArrayRef mutableDen = OCArrayCreateMutableCopy(parsed->denominator);
        siueGroupIdenticalTerms(mutableDen);
        siueSortTermsAlphabetically(mutableDen);
        if (parsed->denominator) OCRelease(parsed->denominator);
        parsed->denominator = mutableDen;
    }
    // Step 4: Format the result
    OCStringRef formatted = siueCreateFormattedExpression(parsed, false);
    siueRelease(parsed);
    if (!formatted) return NULL;
    // Step 5: Convert asterisks back to bullet characters
    OCStringRef bullets = siueCreateByConvertingAsterisksToBullets(formatted);
    OCRelease(formatted);
    if (!bullets) return NULL;
    // Step 6: Convert "1" to space character for dimensionless output
    OCStringRef result = siueCreateByConvertingDimensionlessOutput(bullets);
    OCRelease(bullets);
    return result;
}
// Creates cleaned and reduced expression: full algebraic reduction with power cancellation
// Groups, sorts, cancels matching powers between numerator/denominator, handles dimensionless output
OCStringRef SIUnitCreateCleanedAndReducedExpression(OCStringRef expression) {
    if (!expression) return NULL;
    // Step 1: Normalize Unicode characters first
    OCMutableStringRef normalized = SIUnitCreateNormalizedExpression(expression, false);
    if (!normalized) return NULL;
    // Step 2: Convert bullet characters to asterisks for parsing
    OCStringRef preprocessed = siueCreateByConvertingBulletsToAsterisks(normalized);
    OCRelease(normalized);
    if (!preprocessed) return NULL;
    // Step 2: Parse the preprocessed expression
    SIUnitExpression* parsed = siueCreateParsedExpression(preprocessed);
    OCRelease(preprocessed);
    if (!parsed) return NULL;
    // Step 3: Process the expression (group, sort, and cancel)
    // Work directly with the parsed expression arrays
    if (parsed->numerator) {
        OCMutableArrayRef mutableNum = OCArrayCreateMutableCopy(parsed->numerator);
        siueGroupIdenticalTerms(mutableNum);
        OCMutableArrayRef newDenominator = siueCreateDenominatorWithNegativePowers(mutableNum, parsed->denominator);
        if (parsed->numerator) OCRelease(parsed->numerator);
        if (parsed->denominator) OCRelease(parsed->denominator);
        parsed->numerator = mutableNum;
        parsed->denominator = newDenominator;
    }
    if (parsed->denominator) {
        OCMutableArrayRef mutableDen = OCArrayCreateMutableCopy(parsed->denominator);
        siueGroupIdenticalTerms(mutableDen);
        if (parsed->denominator) OCRelease(parsed->denominator);
        parsed->denominator = mutableDen;
    }
    // Cancel terms between numerator and denominator
    siueCancelTerms(parsed);
    // Sort terms alphabetically after cancellation
    if (parsed->numerator) {
        OCMutableArrayRef sortableNum = OCArrayCreateMutableCopy(parsed->numerator);
        siueSortTermsAlphabetically(sortableNum);
        OCRelease(parsed->numerator);
        parsed->numerator = sortableNum;
    }
    if (parsed->denominator) {
        OCMutableArrayRef sortableDen = OCArrayCreateMutableCopy(parsed->denominator);
        siueSortTermsAlphabetically(sortableDen);
        OCRelease(parsed->denominator);
        parsed->denominator = sortableDen;
    }
    // Step 4: Format the result
    OCStringRef formatted = siueCreateFormattedExpression(parsed, true);
    siueRelease(parsed);
    if (!formatted) return NULL;
    // Step 5: Convert asterisks back to bullet characters
    OCStringRef bullets = siueCreateByConvertingAsterisksToBullets(formatted);
    OCRelease(formatted);
    if (!bullets) return NULL;
    // Step 6: Convert "1" to space character for dimensionless output
    OCStringRef result = siueCreateByConvertingDimensionlessOutput(bullets);
    OCRelease(bullets);
    return result;
}
int SIUnitCountTokenSymbols(OCStringRef cleanedExpression) {
    if (!cleanedExpression) return 0;
    // Handle empty or dimensionless expressions
    if (OCStringGetLength(cleanedExpression) == 0) return 0;
    if (OCStringEqual(cleanedExpression, STR(" ")) || OCStringEqual(cleanedExpression, STR("1"))) return 0;
    // Create a mutable set to track unique symbols
    OCMutableSetRef uniqueSymbols = OCSetCreateMutable(20);
    if (!uniqueSymbols) return 0;
    // Create a working copy to manipulate
    OCMutableStringRef workingCopy = OCStringCreateMutableCopy(cleanedExpression);
    if (!workingCopy) {
        OCRelease(uniqueSymbols);
        return 0;
    }
    // Replace operators with spaces to simplify parsing
    OCStringFindAndReplace2(workingCopy, STR("•"), STR(" "));
    OCStringFindAndReplace2(workingCopy, STR("/"), STR(" "));
    OCStringFindAndReplace2(workingCopy, STR("("), STR(" "));
    OCStringFindAndReplace2(workingCopy, STR(")"), STR(" "));
    // Remove power notations (^n)
    OCRange powerRange;
    do {
        powerRange = OCStringFind(workingCopy, STR("^"), 0);
        if (powerRange.location != kOCNotFound) {
            // Find the end of the power value
            OCIndex powerStart = powerRange.location;
            OCIndex powerEnd = powerStart + 1;  // Start after the '^'
            // Skip optional minus sign
            if (powerEnd < OCStringGetLength(workingCopy)) {
                uint32_t ch = OCStringGetCharacterAtIndex(workingCopy, powerEnd);
                if (ch == '-') powerEnd++;
            }
            // Skip digits
            while (powerEnd < OCStringGetLength(workingCopy)) {
                uint32_t ch = OCStringGetCharacterAtIndex(workingCopy, powerEnd);
                if (ch >= '0' && ch <= '9') {
                    powerEnd++;
                } else {
                    break;
                }
            }
            // Replace the power notation with a space
            OCStringRef powerSubstring = OCStringCreateWithSubstring(workingCopy, OCRangeMake(powerStart, powerEnd - powerStart));
            OCStringFindAndReplace(workingCopy,
                                   powerSubstring,
                                   STR(" "),
                                   OCRangeMake(0, OCStringGetLength(workingCopy)),
                                   0);
            OCRelease(powerSubstring);  // Release the substring to prevent leak
        }
    } while (powerRange.location != kOCNotFound);
    // Now split by whitespace and collect unique symbols
    OCIndex length = OCStringGetLength(workingCopy);
    OCIndex start = 0;
    for (OCIndex i = 0; i <= length; i++) {
        bool isSpace = (i == length) || (OCStringGetCharacterAtIndex(workingCopy, i) == ' ') ||
                       (OCStringGetCharacterAtIndex(workingCopy, i) == '\t') ||
                       (OCStringGetCharacterAtIndex(workingCopy, i) == '\n');
        if (isSpace) {
            if (i > start) {
                // Extract the symbol
                OCStringRef symbol = OCStringCreateWithSubstring(workingCopy, OCRangeMake(start, i - start));
                if (symbol && OCStringGetLength(symbol) > 0) {
                    // Check if it's not just whitespace
                    bool hasContent = false;
                    for (OCIndex j = 0; j < OCStringGetLength(symbol); j++) {
                        uint32_t ch = OCStringGetCharacterAtIndex(symbol, j);
                        if (ch != ' ' && ch != '\t' && ch != '\n') {
                            hasContent = true;
                            break;
                        }
                    }
                    if (hasContent) {
                        OCSetAddValue(uniqueSymbols, (OCTypeRef)symbol);
                    }
                    OCRelease(symbol);
                }
            }
            start = i + 1;
        }
    }
    // Get count and cleanup
    int count = (int)OCSetGetCount(uniqueSymbols);
    OCRelease(uniqueSymbols);
    OCRelease(workingCopy);
    return count;
}
