//
//  SIUnitExpression.c
//  SITypes
//
//  Created by GitHub Copilot on 7/30/25.
//  Copyright © 2025 PhySy Ltd. All rights reserved.
//

#include "SIUnitExpression.h"
#include "SIUnitExpressionParser.tab.h"
#include <stdlib.h>
#include <string.h>

// External functions from scanner/parser
extern int siueparse(void);
extern void siue_scan_string(const char* str);
extern void siue_delete_buffer(void* buffer);

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
    
    expr->numerator = numerator ? OCArrayCreateCopy(numerator) : NULL;
    expr->denominator = denominator ? OCArrayCreateCopy(denominator) : NULL;
    
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

#pragma mark - Processing Functions

void siueGroupIdenticalTerms(OCMutableArrayRef terms) {
    if (!terms) return;
    
    OCIndex count = OCArrayGetCount(terms);
    if (count <= 1) return;
    
    // Group identical terms by combining their powers
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
                j--; // Adjust index after removal
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
    if (!expr->denominator) return; // Nothing to cancel
    
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
                
                break; // Found match, move to next numerator term
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

#pragma mark - Formatting Functions

OCStringRef siueFormatExpression(const SIUnitExpression* expr, bool reduced) {
    if (!expr) return NULL;
    
    OCMutableStringRef result = OCStringCreateMutable(256);
    
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
        // Empty numerator means "1"
        OCStringAppendCString(result, "1");
    }
    
    // Format denominator
    if (expr->denominator && OCArrayGetCount(expr->denominator) > 0) {
        OCStringAppendCString(result, "/");
        
        OCIndex count = OCArrayGetCount(expr->denominator);
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
    }
    
    return OCStringCreateCopy(result);
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
        // Parse failed
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
    return OCStringCreateMutableCopy(expression);
}

// Temporary implementation to support existing tests
// TODO: Implement proper expression equivalence checking
bool SIUnitAreExpressionsEquivalent(OCStringRef expr1, OCStringRef expr2) {
    if (!expr1 || !expr2) return false;
    
    // For now, just do a simple string comparison
    // This should be replaced with proper expression parsing and comparison
    return OCStringEqual(expr1, expr2);
}

OCStringRef SIUnitCreateCleanedExpression(OCStringRef expression) {
    printf("DEBUG: SIUnitCreateCleanedExpression called\n");
    if (!expression) return NULL;
    
    printf("DEBUG: About to convert bullets to asterisks\n");
    // Step 1: Convert bullet characters to asterisks for parsing
    OCStringRef preprocessed = siueConvertBulletsToAsterisks(expression);
    if (!preprocessed) return NULL;
    
    printf("DEBUG: Preprocessing done, about to parse expression\n");
    // Step 2: Parse the preprocessed expression
    SIUnitExpression* parsed = siueParseExpression(preprocessed);
    OCRelease(preprocessed);
    
    if (!parsed) return NULL;
    
    // Step 3: Process the expression (group and sort)
    OCMutableArrayRef numCopy = parsed->numerator ? OCArrayCreateMutableCopy(parsed->numerator) : OCArrayCreateMutable(10, NULL);
    OCMutableArrayRef denCopy = parsed->denominator ? OCArrayCreateMutableCopy(parsed->denominator) : NULL;
    
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
    
    // Create processed expression
    SIUnitExpression* processed = siueCreateExpression(numCopy, denCopy);
    
    // Clean up
    OCRelease(numCopy);
    if (denCopy) OCRelease(denCopy);
    siueRelease(parsed);
    
    if (!processed) return NULL;
    
    // Step 4: Format the result
    OCStringRef formatted = siueFormatExpression(processed, false);
    siueRelease(processed);
    
    if (!formatted) return NULL;
    
    // Step 5: Convert asterisks back to bullet characters
    OCStringRef result = siueConvertAsterisksToBuilets(formatted);
    OCRelease(formatted);
    
    return result;
}

OCStringRef SIUnitCreateCleanedAndReducedExpression(OCStringRef expression) {
    if (!expression) return NULL;
    
    // Step 1: Convert bullet characters to asterisks for parsing
    OCStringRef preprocessed = siueConvertBulletsToAsterisks(expression);
    if (!preprocessed) return NULL;
    
    // Step 2: Parse the preprocessed expression
    SIUnitExpression* parsed = siueParseExpression(preprocessed);
    OCRelease(preprocessed);
    
    if (!parsed) return NULL;
    
    // Step 3: Process the expression (group, sort, and cancel)
    OCMutableArrayRef numCopy = parsed->numerator ? OCArrayCreateMutableCopy(parsed->numerator) : OCArrayCreateMutable(10, NULL);
    OCMutableArrayRef denCopy = parsed->denominator ? OCArrayCreateMutableCopy(parsed->denominator) : NULL;
    
    // Group identical terms
    siueGroupIdenticalTerms(numCopy);
    if (denCopy) {
        siueGroupIdenticalTerms(denCopy);
    }
    
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
    
    // Clean up
    OCRelease(numCopy);
    if (denCopy) OCRelease(denCopy);
    siueRelease(parsed);
    
    // Step 4: Format the result
    OCStringRef formatted = siueFormatExpression(temp, true);
    siueRelease(temp);
    
    if (!formatted) return NULL;
    
    // Step 5: Convert asterisks back to bullet characters
    OCStringRef result = siueConvertAsterisksToBuilets(formatted);
    OCRelease(formatted);
    
    return result;
}
