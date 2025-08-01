//
//  SIUnitExpression.c
//  SITypes
//
//  Created by GitHub Copilot on 7/30/25.
//  Copyright © 2025 PhySy Ltd. All rights reserved.
//
#include "SIUnitExpression.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "SIUnitExpressionParser.tab.h"
// External functions from scanner/parser
extern int siueparse(void);
extern void siue_scan_string(const char* str);
extern void siue_delete_buffer(void* buffer);
// Forward declarations
OCArrayRef siueApplyPowerToTermList(OCArrayRef term_list, int power);
SIUnitExpression* siueApplyPowerToExpression(SIUnitExpression* expression, int power);
SIUnitExpression* siueApplyFractionalPowerToExpression(SIUnitExpression* expression, double power);
// Global error string
OCStringRef siueError = NULL;
#pragma mark - Term Management
SIUnitTerm* siueCreateTerm(OCStringRef symbol, int power) {
    if (!symbol) return NULL;
    SIUnitTerm* term = malloc(sizeof(SIUnitTerm));
    if (!term) return NULL;
    term->symbol = OCStringCreateCopy(symbol);
    term->power = power;
    return term;
}
SIUnitTerm* siueCopyTerm(const SIUnitTerm* term) {
    if (!term) return NULL;
    return siueCreateTerm(term->symbol, term->power);
}
void siueReleaseTerm(SIUnitTerm* term) {
    if (!term) return;
    if (term->symbol) {
        OCRelease(term->symbol);
    }
    free(term);
}
#pragma mark - Expression Management
SIUnitExpression* siueCreateExpression(OCArrayRef numerator, OCArrayRef denominator) {
    if (!numerator) return NULL;
    SIUnitExpression* expr = malloc(sizeof(SIUnitExpression));
    if (!expr) return NULL;
    // Create deep copies of the arrays with deep copying of terms
    if (numerator) {
        OCIndex count = OCArrayGetCount(numerator);
        OCMutableArrayRef numCopy = OCArrayCreateMutable(count, NULL);
        for (OCIndex i = 0; i < count; i++) {
            SIUnitTerm* term = (SIUnitTerm*)OCArrayGetValueAtIndex(numerator, i);
            SIUnitTerm* termCopy = siueCopyTerm(term);
            if (termCopy) {
                OCArrayAppendValue(numCopy, termCopy);
            }
        }
        expr->numerator = numCopy;
    } else {
        expr->numerator = NULL;
    }
    if (denominator) {
        OCIndex count = OCArrayGetCount(denominator);
        OCMutableArrayRef denCopy = OCArrayCreateMutable(count, NULL);
        for (OCIndex i = 0; i < count; i++) {
            SIUnitTerm* term = (SIUnitTerm*)OCArrayGetValueAtIndex(denominator, i);
            SIUnitTerm* termCopy = siueCopyTerm(term);
            if (termCopy) {
                OCArrayAppendValue(denCopy, termCopy);
            }
        }
        expr->denominator = denCopy;
    } else {
        expr->denominator = NULL;
    }
    return expr;
}
SIUnitExpression* siueCopyExpression(const SIUnitExpression* expr) {
    if (!expr) return NULL;
    return siueCreateExpression(expr->numerator, expr->denominator);
}
void siueRelease(SIUnitExpression* expr) {
    if (!expr) return;
    if (expr->numerator) {
        // Release all terms in numerator
        OCIndex count = OCArrayGetCount(expr->numerator);
        for (OCIndex i = 0; i < count; i++) {
            SIUnitTerm* term = (SIUnitTerm*)OCArrayGetValueAtIndex(expr->numerator, i);
            siueReleaseTerm(term);
        }
        OCRelease(expr->numerator);
    }
    if (expr->denominator) {
        // Release all terms in denominator
        OCIndex count = OCArrayGetCount(expr->denominator);
        for (OCIndex i = 0; i < count; i++) {
            SIUnitTerm* term = (SIUnitTerm*)OCArrayGetValueAtIndex(expr->denominator, i);
            siueReleaseTerm(term);
        }
        OCRelease(expr->denominator);
    }
    free(expr);
}
#pragma mark - Parser Helper Functions
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
SIUnitExpression* siueApplyPowerToExpression(SIUnitExpression* expression, int power) {
    if (!expression || power == 0) return expression;
    if (power < 0) {
        // For negative powers, swap numerator and denominator: (a/b)^(-n) = (b/a)^n
        OCArrayRef oldNumerator = expression->numerator;
        OCArrayRef oldDenominator = expression->denominator;
        // Swap them
        expression->numerator = oldDenominator;
        expression->denominator = oldNumerator;
        // Apply the positive power
        power = -power;
    }
    // Apply power to numerator
    if (expression->numerator) {
        siueApplyPowerToTermList(expression->numerator, power);
    }
    // Apply power to denominator
    if (expression->denominator) {
        siueApplyPowerToTermList(expression->denominator, power);
    }
    return expression;
}
SIUnitExpression* siueApplyFractionalPowerToExpression(SIUnitExpression* expression, double power) {
    if (!expression) return NULL;
    // Check if applying this fractional power would result in integer powers for all terms
    // First check numerator
    if (expression->numerator) {
        OCIndex count = OCArrayGetCount(expression->numerator);
        for (OCIndex i = 0; i < count; i++) {
            SIUnitTerm* term = (SIUnitTerm*)OCArrayGetValueAtIndex(expression->numerator, i);
            if (term) {
                double resultPower = term->power * power;
                // Check if the result is an integer (within floating point precision)
                if (fabs(resultPower - round(resultPower)) > 1e-10) {
                    // This would result in a fractional power - reject
                    return NULL;
                }
            }
        }
    }
    // Check denominator
    if (expression->denominator) {
        OCIndex count = OCArrayGetCount(expression->denominator);
        for (OCIndex i = 0; i < count; i++) {
            SIUnitTerm* term = (SIUnitTerm*)OCArrayGetValueAtIndex(expression->denominator, i);
            if (term) {
                double resultPower = term->power * power;
                // Check if the result is an integer (within floating point precision)
                if (fabs(resultPower - round(resultPower)) > 1e-10) {
                    // This would result in a fractional power - reject
                    return NULL;
                }
            }
        }
    }
    // All resulting powers would be integers, so apply the fractional power
    // Apply power to numerator
    if (expression->numerator) {
        OCIndex count = OCArrayGetCount(expression->numerator);
        for (OCIndex i = 0; i < count; i++) {
            SIUnitTerm* term = (SIUnitTerm*)OCArrayGetValueAtIndex(expression->numerator, i);
            if (term) {
                term->power = (int)round(term->power * power);
            }
        }
    }
    // Apply power to denominator
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
void siueGroupIdenticalTerms(OCMutableArrayRef terms) {
    if (!terms) return;
    OCIndex count = OCArrayGetCount(terms);
    // Group identical terms by combining their powers (only if we have multiple terms)
    if (count > 1) {
        for (OCIndex i = 0; i < count; i++) {
            SIUnitTerm* term1 = (SIUnitTerm*)OCArrayGetValueAtIndex(terms, i);
            if (!term1) continue;
            for (OCIndex j = i + 1; j < count; j++) {
                SIUnitTerm* term2 = (SIUnitTerm*)OCArrayGetValueAtIndex(terms, j);
                if (!term2) continue;
                if (OCStringEqual(term1->symbol, term2->symbol)) {
                    // Combine powers
                    term1->power += term2->power;
                    // Remove the second term
                    siueReleaseTerm(term2);
                    OCArrayRemoveValueAtIndex(terms, j);
                    count--;
                    j--;  // Adjust index after removal
                }
            }
        }
    }
    // Remove terms with zero power (always do this, even for single terms)
    for (OCIndex i = count - 1; i >= 0; i--) {
        SIUnitTerm* term = (SIUnitTerm*)OCArrayGetValueAtIndex(terms, i);
        if (term && term->power == 0) {
            siueReleaseTerm(term);
            OCArrayRemoveValueAtIndex(terms, i);
        }
    }
}
// Comparison function for alphabetical sorting
static int termCompare(const void* a, const void* b) {
    const SIUnitTerm* term1 = *(const SIUnitTerm**)a;
    const SIUnitTerm* term2 = *(const SIUnitTerm**)b;
    if (!term1 || !term1->symbol) return 1;
    if (!term2 || !term2->symbol) return -1;
    return OCStringCompare(term1->symbol, term2->symbol, 0);
}
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
    // Sort the array
    qsort(termArray, count, sizeof(SIUnitTerm*), termCompare);
    // Create a new array with sorted terms
    OCMutableArrayRef sortedTerms = OCArrayCreateMutable(count, NULL);
    for (OCIndex i = 0; i < count; i++) {
        OCArrayAppendValue(sortedTerms, termArray[i]);
    }
    // Replace the original array content
    // Remove all existing values
    while (OCArrayGetCount(terms) > 0) {
        OCArrayRemoveValueAtIndex(terms, 0);
    }
    // Add sorted values
    for (OCIndex i = 0; i < count; i++) {
        OCArrayAppendValue(terms, termArray[i]);
    }
    OCRelease(sortedTerms);
    free(termArray);
}
void siueCancelTerms(SIUnitExpression* expr) {
    if (!expr || !expr->numerator) return;
    if (!expr->denominator) return;  // Nothing to cancel
    OCMutableArrayRef num = OCArrayCreateMutableCopy(expr->numerator);
    OCMutableArrayRef den = OCArrayCreateMutableCopy(expr->denominator);
    OCIndex numCount = OCArrayGetCount(num);
    OCIndex denCount = OCArrayGetCount(den);
    // For each term in numerator, look for matching term in denominator
    for (OCIndex i = numCount - 1; i >= 0; i--) {
        SIUnitTerm* numTerm = (SIUnitTerm*)OCArrayGetValueAtIndex(num, i);
        if (!numTerm) continue;
        for (OCIndex j = denCount - 1; j >= 0; j--) {
            SIUnitTerm* denTerm = (SIUnitTerm*)OCArrayGetValueAtIndex(den, j);
            if (!denTerm) continue;
            if (OCStringEqual(numTerm->symbol, denTerm->symbol)) {
                // Cancel common terms
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
    // Replace the original arrays
    if (expr->numerator) OCRelease(expr->numerator);
    if (expr->denominator) OCRelease(expr->denominator);
    expr->numerator = num;
    expr->denominator = (OCArrayGetCount(den) > 0) ? den : NULL;
    if (!expr->denominator && den) {
        OCRelease(den);
    }
}
void siueMoveNegativePowersToDenominator(OCMutableArrayRef numerator, OCMutableArrayRef* denominator) {
    if (!numerator) return;
    // Create denominator if it doesn't exist
    if (!*denominator) {
        *denominator = OCArrayCreateMutable(10, NULL);
    }
    // Find terms with negative powers in numerator
    OCIndex count = OCArrayGetCount(numerator);
    for (OCIndex i = count - 1; i >= 0; i--) {
        SIUnitTerm* term = (SIUnitTerm*)OCArrayGetValueAtIndex(numerator, i);
        if (term && term->power < 0) {
            // Create a copy with positive power for denominator
            SIUnitTerm* newTerm = siueCreateTerm(term->symbol, -term->power);
            if (newTerm) {
                OCArrayAppendValue(*denominator, newTerm);
            }
            // Remove from numerator
            OCArrayRemoveValueAtIndex(numerator, i);
        }
    }
}
#pragma mark - Formatting Functions
OCStringRef siueFormatExpression(const SIUnitExpression* expr, bool reduced) {
    if (!expr) return NULL;
    OCMutableStringRef result = OCStringCreateMutable(256);
    // Check if we have an empty numerator with a non-empty denominator
    bool hasEmptyNumerator = (!expr->numerator || OCArrayGetCount(expr->numerator) == 0);
    bool hasNonEmptyDenominator = (expr->denominator && OCArrayGetCount(expr->denominator) > 0);
    bool needsOuterParens = hasEmptyNumerator && hasNonEmptyDenominator;
    // If we need outer parentheses, add opening parenthesis
    if (needsOuterParens) {
        OCStringAppendCString(result, "(");
    }
    // Format numerator
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
        // Empty numerator - check if this is truly dimensionless or has denominator
        if (expr->denominator && OCArrayGetCount(expr->denominator) > 0) {
            // Has denominator, so this is like 1/m - output "1"
            OCStringAppendCString(result, "1");
        } else {
            // No denominator either, this is dimensionless - output space character
            OCStringAppendCString(result, " ");
        }
    }
    // Format denominator
    if (expr->denominator && OCArrayGetCount(expr->denominator) > 0) {
        OCStringAppendCString(result, "/");
        OCIndex count = OCArrayGetCount(expr->denominator);
        // Add parentheses if there are multiple terms in denominator
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
    // If we need outer parentheses, add closing parenthesis
    if (needsOuterParens) {
        OCStringAppendCString(result, ")");
    }
    return OCStringCreateCopy(result);
}
// Helper function to convert internal "1" back to space for dimensionless output
static OCStringRef siueConvertDimensionlessOutput(OCStringRef formatted) {
    if (!formatted) return NULL;
    // Check if the result is exactly "1" (dimensionless)
    if (OCStringEqual(formatted, STR("1"))) {
        return OCStringCreateCopy(STR(" "));
    }
    // For any other result, return as-is
    return OCStringCreateCopy(formatted);
}
#pragma mark - Global Variables for Parser
// Global variables for parser state management
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
bool siueValidateSymbol(OCStringRef symbol) {
    if (!symbol) return false;
    // Get the list of valid token symbols
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
SIUnitExpression* siueParseExpression(OCStringRef normalized_expr) {
    if (!normalized_expr) return NULL;
    // Clear any previous error
    if (siueError) {
        OCRelease(siueError);
        siueError = NULL;
    }
    // Clear any previous parsed expression
    siueClearParsedExpression();
    // Convert to C string for parser
    const char* exprStr = OCStringGetCString(normalized_expr);
    if (!exprStr) return NULL;
    // Parse the expression
    siue_scan_string(exprStr);
    int parseResult = siueparse();
    if (parseResult != 0 || siueError) {
        // Parse failed - clean up state
        siueClearParsedExpression();
        return NULL;
    }
    // Get the parsed expression
    SIUnitExpression* result = siueGetParsedExpression();
    return result ? siueCopyExpression(result) : NULL;
}
#pragma mark - Unicode Conversion Helpers
// Convert bullet characters to asterisks for parsing
static OCStringRef siueConvertBulletsToAsterisks(OCStringRef expression) {
    if (!expression) return NULL;
    OCMutableStringRef mutable = OCStringCreateMutableCopy(expression);
    if (!mutable) return NULL;
    // Replace bullet character (•) with asterisk (*)
    OCStringFindAndReplace2(mutable, STR("•"), STR("*"));
    return mutable;
}
// Convert asterisks back to bullet characters for output
static OCStringRef siueConvertAsterisksToBuilets(OCStringRef expression) {
    if (!expression) return NULL;
    OCMutableStringRef mutable = OCStringCreateMutableCopy(expression);
    if (!mutable) return NULL;
    // Replace asterisk (*) with UTF-8 bullet character (•)
    OCStringFindAndReplace2(mutable, STR("*"), STR("•"));
    return mutable;
}
#pragma mark - Main Public Functions
// Temporary implementation to support existing SIUnitParser.y
// TODO: Implement proper Unicode normalization
OCMutableStringRef SIUnitCreateNormalizedExpression(OCStringRef expression, bool something) {
    if (!expression) return NULL;
    // Check if this is an empty string - treat as dimensionless
    if (OCStringGetLength(expression) == 0) {
        return OCStringCreateMutableCopy(STR("1"));
    }
    OCMutableStringRef mutString = OCStringCreateMutableCopy(expression);
    // Check if this is just a single space character (dimensionless unit)
    if (OCStringGetLength(expression) == 1 && OCStringEqual(expression, STR(" "))) {
        // Convert space character to "1" for internal parser processing
        OCRelease(mutString);
        return OCStringCreateMutableCopy(STR("1"));
    }
    // Perform Unicode-aware replacements similar to SIScalarParserHelpers.c
    OCStringFindAndReplace(mutString, STR("×"), STR("*"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("÷"), STR("/"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("−"), STR("-"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("μ"), STR("µ"), OCRangeMake(0, OCStringGetLength(mutString)), 0);  // Greek letter mu to micro sign
    OCStringFindAndReplace(mutString, STR("º"), STR("°"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    // Additional Unicode multiplication operators → •
    OCStringFindAndReplace(mutString, STR("⋅"), STR("*"), OCRangeMake(0, OCStringGetLength(mutString)), 0);  // DOT OPERATOR
    OCStringFindAndReplace(mutString, STR("∙"), STR("*"), OCRangeMake(0, OCStringGetLength(mutString)), 0);  // BULLET OPERATOR
    OCStringFindAndReplace(mutString, STR("·"), STR("*"), OCRangeMake(0, OCStringGetLength(mutString)), 0);  // MIDDLE DOT
    // Additional Unicode division operators → /
    OCStringFindAndReplace(mutString, STR("∕"), STR("/"), OCRangeMake(0, OCStringGetLength(mutString)), 0);  // DIVISION SLASH
    OCStringFindAndReplace(mutString, STR("⁄"), STR("/"), OCRangeMake(0, OCStringGetLength(mutString)), 0);  // FRACTION SLASH
    // Convert superscript numbers to ^n format (handle negative superscripts first)
    OCStringFindAndReplace(mutString, STR("⁻⁰"), STR("^-0"), OCRangeMake(0, OCStringGetLength(mutString)), 0);  // SUPERSCRIPT MINUS ZERO
    OCStringFindAndReplace(mutString, STR("⁻¹"), STR("^-1"), OCRangeMake(0, OCStringGetLength(mutString)), 0);  // SUPERSCRIPT MINUS ONE
    OCStringFindAndReplace(mutString, STR("⁻²"), STR("^-2"), OCRangeMake(0, OCStringGetLength(mutString)), 0);  // SUPERSCRIPT MINUS TWO
    OCStringFindAndReplace(mutString, STR("⁻³"), STR("^-3"), OCRangeMake(0, OCStringGetLength(mutString)), 0);  // SUPERSCRIPT MINUS THREE
    OCStringFindAndReplace(mutString, STR("⁻⁴"), STR("^-4"), OCRangeMake(0, OCStringGetLength(mutString)), 0);  // SUPERSCRIPT MINUS FOUR
    OCStringFindAndReplace(mutString, STR("⁻⁵"), STR("^-5"), OCRangeMake(0, OCStringGetLength(mutString)), 0);  // SUPERSCRIPT MINUS FIVE
    OCStringFindAndReplace(mutString, STR("⁻⁶"), STR("^-6"), OCRangeMake(0, OCStringGetLength(mutString)), 0);  // SUPERSCRIPT MINUS SIX
    OCStringFindAndReplace(mutString, STR("⁻⁷"), STR("^-7"), OCRangeMake(0, OCStringGetLength(mutString)), 0);  // SUPERSCRIPT MINUS SEVEN
    OCStringFindAndReplace(mutString, STR("⁻⁸"), STR("^-8"), OCRangeMake(0, OCStringGetLength(mutString)), 0);  // SUPERSCRIPT MINUS EIGHT
    OCStringFindAndReplace(mutString, STR("⁻⁹"), STR("^-9"), OCRangeMake(0, OCStringGetLength(mutString)), 0);  // SUPERSCRIPT MINUS NINE
    // Then handle positive superscripts
    OCStringFindAndReplace(mutString, STR("⁰"), STR("^0"), OCRangeMake(0, OCStringGetLength(mutString)), 0);  // SUPERSCRIPT ZERO
    OCStringFindAndReplace(mutString, STR("¹"), STR("^1"), OCRangeMake(0, OCStringGetLength(mutString)), 0);  // SUPERSCRIPT ONE
    OCStringFindAndReplace(mutString, STR("²"), STR("^2"), OCRangeMake(0, OCStringGetLength(mutString)), 0);  // SUPERSCRIPT TWO
    OCStringFindAndReplace(mutString, STR("³"), STR("^3"), OCRangeMake(0, OCStringGetLength(mutString)), 0);  // SUPERSCRIPT THREE
    OCStringFindAndReplace(mutString, STR("⁴"), STR("^4"), OCRangeMake(0, OCStringGetLength(mutString)), 0);  // SUPERSCRIPT FOUR
    OCStringFindAndReplace(mutString, STR("⁵"), STR("^5"), OCRangeMake(0, OCStringGetLength(mutString)), 0);  // SUPERSCRIPT FIVE
    OCStringFindAndReplace(mutString, STR("⁶"), STR("^6"), OCRangeMake(0, OCStringGetLength(mutString)), 0);  // SUPERSCRIPT SIX
    OCStringFindAndReplace(mutString, STR("⁷"), STR("^7"), OCRangeMake(0, OCStringGetLength(mutString)), 0);  // SUPERSCRIPT SEVEN
    OCStringFindAndReplace(mutString, STR("⁸"), STR("^8"), OCRangeMake(0, OCStringGetLength(mutString)), 0);  // SUPERSCRIPT EIGHT
    OCStringFindAndReplace(mutString, STR("⁹"), STR("^9"), OCRangeMake(0, OCStringGetLength(mutString)), 0);  // SUPERSCRIPT NINE
    // Remove spaces around operators, but preserve standalone space
    OCStringFindAndReplace(mutString, STR(" * "), STR("*"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR(" / "), STR("/"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR(" ^ "), STR("^"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR(" + "), STR("+"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR(" - "), STR("-"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    return mutString;
}
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
OCStringRef SIUnitCreateCleanedExpression(OCStringRef expression) {
    if (!expression) return NULL;
    // Step 1: Normalize Unicode characters first
    OCMutableStringRef normalized = SIUnitCreateNormalizedExpression(expression, false);
    if (!normalized) return NULL;
    // Step 2: Convert bullet characters to asterisks for parsing
    OCStringRef preprocessed = siueConvertBulletsToAsterisks(normalized);
    OCRelease(normalized);
    if (!preprocessed) return NULL;
    // Step 3: Parse the preprocessed expression
    SIUnitExpression* parsed = siueParseExpression(preprocessed);
    OCRelease(preprocessed);
    if (!parsed) return NULL;
    // Step 3: Process the expression (group and sort)
    // Create deep copies of the arrays to avoid double-free issues
    OCMutableArrayRef numCopy = OCArrayCreateMutable(10, NULL);
    if (parsed->numerator) {
        OCIndex count = OCArrayGetCount(parsed->numerator);
        for (OCIndex i = 0; i < count; i++) {
            SIUnitTerm* term = (SIUnitTerm*)OCArrayGetValueAtIndex(parsed->numerator, i);
            SIUnitTerm* termCopy = siueCopyTerm(term);
            if (termCopy) {
                OCArrayAppendValue(numCopy, termCopy);
            }
        }
    }
    OCMutableArrayRef denCopy = NULL;
    if (parsed->denominator) {
        OCIndex count = OCArrayGetCount(parsed->denominator);
        denCopy = OCArrayCreateMutable(count, NULL);
        for (OCIndex i = 0; i < count; i++) {
            SIUnitTerm* term = (SIUnitTerm*)OCArrayGetValueAtIndex(parsed->denominator, i);
            SIUnitTerm* termCopy = siueCopyTerm(term);
            if (termCopy) {
                OCArrayAppendValue(denCopy, termCopy);
            }
        }
    }
    // Group identical terms
    siueGroupIdenticalTerms(numCopy);
    if (denCopy) {
        siueGroupIdenticalTerms(denCopy);
    }
    // Sort terms alphabetically
    siueSortTermsAlphabetically(numCopy);
    if (denCopy) {
        siueSortTermsAlphabetically(denCopy);
    }
    // Move negative powers from numerator to denominator
    siueMoveNegativePowersToDenominator(numCopy, &denCopy);
    // Sort again after moving negative powers to ensure denominator is alphabetical
    if (denCopy) {
        siueSortTermsAlphabetically(denCopy);
    }
    // Create processed expression (no cancellation for "Cleaned" function)
    SIUnitExpression* processed = siueCreateExpression(numCopy, denCopy);
    siueRelease(parsed);
    if (!processed) {
        return NULL;
    }
    // Step 4: Format the result
    OCStringRef formatted = siueFormatExpression(processed, false);
    siueRelease(processed);
    // Clean up the deep-copied arrays after we're done with processed
    if (numCopy) {
        OCIndex count = OCArrayGetCount(numCopy);
        for (OCIndex i = 0; i < count; i++) {
            SIUnitTerm* term = (SIUnitTerm*)OCArrayGetValueAtIndex(numCopy, i);
            siueReleaseTerm(term);
        }
        OCRelease(numCopy);
    }
    if (denCopy) {
        OCIndex count = OCArrayGetCount(denCopy);
        for (OCIndex i = 0; i < count; i++) {
            SIUnitTerm* term = (SIUnitTerm*)OCArrayGetValueAtIndex(denCopy, i);
            siueReleaseTerm(term);
        }
        OCRelease(denCopy);
    }
    if (!formatted) return NULL;
    // Step 5: Convert asterisks back to bullet characters
    OCStringRef bullets = siueConvertAsterisksToBuilets(formatted);
    OCRelease(formatted);
    if (!bullets) return NULL;
    // Step 6: Convert "1" to space character for dimensionless output
    OCStringRef result = siueConvertDimensionlessOutput(bullets);
    OCRelease(bullets);
    return result;
}
OCStringRef SIUnitCreateCleanedAndReducedExpression(OCStringRef expression) {
    if (!expression) return NULL;
    // Step 1: Normalize Unicode characters first
    OCMutableStringRef normalized = SIUnitCreateNormalizedExpression(expression, false);
    if (!normalized) return NULL;
    // Step 2: Convert bullet characters to asterisks for parsing
    OCStringRef preprocessed = siueConvertBulletsToAsterisks(normalized);
    OCRelease(normalized);
    if (!preprocessed) return NULL;
    // Step 2: Parse the preprocessed expression
    SIUnitExpression* parsed = siueParseExpression(preprocessed);
    OCRelease(preprocessed);
    if (!parsed) return NULL;
    // Step 3: Process the expression (group, sort, and cancel)
    // Create deep copies of the arrays to avoid double-free issues
    OCMutableArrayRef numCopy = OCArrayCreateMutable(10, NULL);
    if (parsed->numerator) {
        OCIndex count = OCArrayGetCount(parsed->numerator);
        for (OCIndex i = 0; i < count; i++) {
            SIUnitTerm* term = (SIUnitTerm*)OCArrayGetValueAtIndex(parsed->numerator, i);
            SIUnitTerm* termCopy = siueCopyTerm(term);
            if (termCopy) {
                OCArrayAppendValue(numCopy, termCopy);
            }
        }
    }
    OCMutableArrayRef denCopy = NULL;
    if (parsed->denominator) {
        OCIndex count = OCArrayGetCount(parsed->denominator);
        denCopy = OCArrayCreateMutable(count, NULL);
        for (OCIndex i = 0; i < count; i++) {
            SIUnitTerm* term = (SIUnitTerm*)OCArrayGetValueAtIndex(parsed->denominator, i);
            SIUnitTerm* termCopy = siueCopyTerm(term);
            if (termCopy) {
                OCArrayAppendValue(denCopy, termCopy);
            }
        }
    }
    // Group identical terms
    siueGroupIdenticalTerms(numCopy);
    if (denCopy) {
        siueGroupIdenticalTerms(denCopy);
    }
    // Move negative powers from numerator to denominator
    siueMoveNegativePowersToDenominator(numCopy, &denCopy);
    // Create temporary expression for cancellation
    SIUnitExpression* temp = siueCreateExpression(numCopy, denCopy);
    // Cancel terms between numerator and denominator
    siueCancelTerms(temp);
    // Sort terms alphabetically after cancellation
    if (temp->numerator) {
        OCMutableArrayRef sortableNum = OCArrayCreateMutableCopy(temp->numerator);
        siueSortTermsAlphabetically(sortableNum);
        OCRelease(temp->numerator);
        temp->numerator = sortableNum;
    }
    if (temp->denominator) {
        OCMutableArrayRef sortableDen = OCArrayCreateMutableCopy(temp->denominator);
        siueSortTermsAlphabetically(sortableDen);
        OCRelease(temp->denominator);
        temp->denominator = sortableDen;
    }
    // Clean up the deep-copied arrays
    // Release all terms in numCopy
    if (numCopy) {
        OCIndex count = OCArrayGetCount(numCopy);
        for (OCIndex i = 0; i < count; i++) {
            SIUnitTerm* term = (SIUnitTerm*)OCArrayGetValueAtIndex(numCopy, i);
            siueReleaseTerm(term);
        }
        OCRelease(numCopy);
    }
    // Release all terms in denCopy
    if (denCopy) {
        OCIndex count = OCArrayGetCount(denCopy);
        for (OCIndex i = 0; i < count; i++) {
            SIUnitTerm* term = (SIUnitTerm*)OCArrayGetValueAtIndex(denCopy, i);
            siueReleaseTerm(term);
        }
        OCRelease(denCopy);
    }
    siueRelease(parsed);
    // Step 4: Format the result
    OCStringRef formatted = siueFormatExpression(temp, true);
    siueRelease(temp);
    if (!formatted) return NULL;
    // Step 5: Convert asterisks back to bullet characters
    OCStringRef bullets = siueConvertAsterisksToBuilets(formatted);
    OCRelease(formatted);
    if (!bullets) return NULL;
    // Step 6: Convert "1" to space character for dimensionless output
    OCStringRef result = siueConvertDimensionlessOutput(bullets);
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
            OCStringFindAndReplace(workingCopy,
                                   OCStringCreateWithSubstring(workingCopy, OCRangeMake(powerStart, powerEnd - powerStart)),
                                   STR(" "),
                                   OCRangeMake(0, OCStringGetLength(workingCopy)),
                                   0);
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
