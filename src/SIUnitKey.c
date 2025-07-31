#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "SILibrary.h"
// Structure to hold parsed unit components with their powers
typedef struct {
    char *symbol;
    int power;
} UnitComponent;
// Dynamic array to store components
typedef struct {
    UnitComponent *components;
    int count;
    int capacity;
} ComponentList;
// Static variables for parsing state
static ComponentList *positive_components = NULL;
static ComponentList *negative_components = NULL;
static bool in_denominator = false;
static bool fractional_power_error = false;
// Structure to hold the result of power extraction
typedef struct {
    OCStringRef base_token;  // The base part (e.g., "m" from "m^2")
    int power;               // The power value (e.g., 2 from "m^2")
    bool error;              // True if there was an error (e.g., fractional power)
} PowerExtractionResult;
// Function prototypes
static void init_component_lists(void);
static void free_component_lists(void);
static void add_component(const char *symbol, int power);
static void consolidate_powers(ComponentList *list);
static int compare_symbols(const void *a, const void *b);
static OCStringRef create_library_key(void);
static void parse_expression_with_ocstring(OCStringRef expr);
static void parse_expression_manually(const char *expr);
static OCStringRef expand_parenthetical_power(OCStringRef expression, int power);
static PowerExtractionResult extract_power_from_token(OCStringRef token);
static bool validate_expression_symbols(const char *expr_cstr);

// Helper function to validate that all symbols in an expression are valid units
static bool validate_expression_symbols(const char *expr_cstr) {
    if (!expr_cstr) return false;
    
    // Create a larger buffer to accommodate inserted markers
    size_t original_len = strlen(expr_cstr);
    char *expr_copy = malloc(original_len * 2 + 10);  // Extra space for ~ markers
    strcpy(expr_copy, expr_cstr);
    
    bool has_valid_symbols = true;
    
    // Smarter tokenization: replace operators but preserve context for exponents
    // First pass: identify and mark exponents by looking for ^ followed by numbers
    char *pos = expr_copy;
    while (*pos) {
        if (*pos == '^') {
            *pos = ' ';  // Replace ^ with space
            pos++;
            // Skip whitespace after ^
            while (*pos && isspace(*pos)) pos++;
            // Mark the following number as an exponent (not a unit symbol)
            if (*pos && (isdigit(*pos) || *pos == '-' || *pos == '+')) {
                // Calculate exponent length
                int exp_len = 0;
                char *temp = pos;
                if (*temp == '-' || *temp == '+') temp++;
                while (*temp && (isdigit(*temp) || *temp == '.')) {
                    exp_len++;
                    temp++;
                }
                if (*pos == '-' || *pos == '+') exp_len++;
                
                // Calculate length of remaining string after exponent
                int remaining_len = strlen(pos + exp_len);
                
                // Expand the string to make room for ~ marker
                // Move everything from current position to end of string one position right
                memmove(pos + 1, pos, exp_len + remaining_len + 1);  // +1 for null terminator
                *pos = '~';  // Mark the exponent with ~
                pos += exp_len + 1;  // Skip past the marked exponent
            }
        } else {
            pos++;
        }
    }
    
    // Second pass: replace remaining mathematical operators with spaces
    for (char *p = expr_copy; *p; p++) {
        if (*p == '*' || *p == '/' || *p == '(' || *p == ')') {
            *p = ' ';
        } else if (strncmp(p, "•", 3) == 0 || strncmp(p, "×", 2) == 0 ||
                  strncmp(p, "⋅", 3) == 0 || strncmp(p, "∙", 3) == 0 ||
                  strncmp(p, "·", 2) == 0) {
            *p = ' ';
            // Skip the rest of the multi-byte character
            p++;
            while (*p && (*p & 0x80)) {
                *p = ' ';
                p++;
            }
            p--; // Back up one since the loop will increment
        }
    }
    
    // Tokenize and validate each symbol
    char *token;
    char *saveptr;
    token = strtok_r(expr_copy, " \t", &saveptr);
    while (token != NULL && has_valid_symbols) {
        // Skip empty tokens
        if (strlen(token) == 0) {
            token = strtok_r(NULL, " \t", &saveptr);
            continue;
        }
        
        // Check if this token is marked as an exponent
        bool is_exponent = (token[0] == '~');
        if (is_exponent) {
            // Skip validation for exponents - they're numbers, not unit symbols
            token = strtok_r(NULL, " \t", &saveptr);
            continue;
        }
        
        // For non-exponent tokens, validate against underived library
        OCStringRef symbol_str = OCStringCreateWithCString(token);
        OCMutableArrayRef underivedLib = SIUnitGetTokenSymbolsLib();
        if (!OCArrayContainsValue(underivedLib, symbol_str)) {
            has_valid_symbols = false;
        }
        OCRelease(symbol_str);
        
        token = strtok_r(NULL, " \t", &saveptr);
    }
    
    free(expr_copy);
    return has_valid_symbols;
}
// Initialize component lists
static void init_component_lists(void) {
    positive_components = malloc(sizeof(ComponentList));
    positive_components->capacity = 10;
    positive_components->count = 0;
    positive_components->components = malloc(positive_components->capacity * sizeof(UnitComponent));
    negative_components = malloc(sizeof(ComponentList));
    negative_components->capacity = 10;
    negative_components->count = 0;
    negative_components->components = malloc(negative_components->capacity * sizeof(UnitComponent));
    in_denominator = false;
    fractional_power_error = false;
}
// Free component lists
static void free_component_lists(void) {
    if (positive_components) {
        for (int i = 0; i < positive_components->count; i++) {
            free(positive_components->components[i].symbol);
        }
        free(positive_components->components);
        free(positive_components);
        positive_components = NULL;
    }
    if (negative_components) {
        for (int i = 0; i < negative_components->count; i++) {
            free(negative_components->components[i].symbol);
        }
        free(negative_components->components);
        free(negative_components);
        negative_components = NULL;
    }
}
// Add a component to the appropriate list
static void add_component(const char *symbol, int power) {
    // Adjust power based on position (numerator vs denominator)
    int final_power = in_denominator ? -power : power;
    // Choose the appropriate list
    ComponentList *target_list = (final_power > 0) ? positive_components : negative_components;
    final_power = abs(final_power);  // Store absolute values in lists
    // Check if this symbol already exists in the target list
    for (int i = 0; i < target_list->count; i++) {
        if (strcmp(target_list->components[i].symbol, symbol) == 0) {
            // Combine powers for the same symbol
            target_list->components[i].power += final_power;
            return;
        }
    }
    // Add new component - expand array if needed
    if (target_list->count >= target_list->capacity) {
        target_list->capacity *= 2;
        target_list->components = realloc(target_list->components,
                                          target_list->capacity * sizeof(UnitComponent));
    }
    UnitComponent *comp = &target_list->components[target_list->count++];
    comp->symbol = strdup(symbol);
    comp->power = final_power;
}
// Remove components with zero power
static void consolidate_powers(ComponentList *list) {
    int write_idx = 0;
    for (int read_idx = 0; read_idx < list->count; read_idx++) {
        if (list->components[read_idx].power > 0) {
            if (write_idx != read_idx) {
                list->components[write_idx] = list->components[read_idx];
            }
            write_idx++;
        } else {
            // Free memory for removed component
            free(list->components[read_idx].symbol);
        }
    }
    list->count = write_idx;
}
// Comparison function for sorting components alphabetically
static int compare_symbols(const void *a, const void *b) {
    const UnitComponent *comp_a = (const UnitComponent *)a;
    const UnitComponent *comp_b = (const UnitComponent *)b;
    return strcmp(comp_a->symbol, comp_b->symbol);
}
// Create library key representation
// Create library key representation
static OCStringRef create_library_key(void) {
    // Consolidate and sort both lists
    consolidate_powers(positive_components);
    consolidate_powers(negative_components);
    // Sort both lists alphabetically by symbol
    qsort(positive_components->components, positive_components->count,
          sizeof(UnitComponent), compare_symbols);
    qsort(negative_components->components, negative_components->count,
          sizeof(UnitComponent), compare_symbols);
    OCMutableStringRef result = OCStringCreateMutable(0);
    // Build numerator (positive powers)
    bool first_positive = true;
    for (int i = 0; i < positive_components->count; i++) {
        UnitComponent *comp = &positive_components->components[i];
        if (comp->power == 0) continue;  // Skip zero powers
        if (!first_positive) {
            OCStringAppend(result, STR("•"));
        }
        OCStringRef compString = OCStringCreateWithCString(comp->symbol);
        OCStringAppend(result, compString);
        OCRelease(compString);
        if (comp->power != 1) {
            OCStringAppendFormat(result, STR("^%d"), comp->power);
        }
        first_positive = false;
    }
    // Build denominator (negative powers) if any exist
    if (negative_components->count > 0) {
        // If we have no numerator terms, add "1" as numerator
        if (first_positive) {
            OCStringAppend(result, STR("1"));
        }
        OCStringAppend(result, STR("/"));
        // Use parentheses only if multiple terms in denominator
        bool needs_parens = negative_components->count > 1;
        if (needs_parens) {
            OCStringAppend(result, STR("("));
        }
        bool first_negative = true;
        for (int i = 0; i < negative_components->count; i++) {
            UnitComponent *comp = &negative_components->components[i];
            if (comp->power == 0) continue;  // Skip zero powers
            if (!first_negative) {
                OCStringAppend(result, STR("•"));
            }
            OCStringRef compString = OCStringCreateWithCString(comp->symbol);
            OCStringAppend(result, compString);
            OCRelease(compString);
            if (comp->power != 1) {
                OCStringAppendFormat(result, STR("^%d"), comp->power);
            }
            first_negative = false;
        }
        if (needs_parens) {
            OCStringAppend(result, STR(")"));
        }
    }
    // Handle the case where everything cancels out or empty input
    if (OCStringGetLength(result) == 0) {
        OCStringAppend(result, STR("1"));  // Return "1" for dimensionless/empty
    }
    return result;
}
// Trim whitespace and conditionally remove parentheses using OCTypes APIs
static OCStringRef trim_whitespace_and_parens(OCStringRef str) {
    if (!str || OCStringGetLength(str) == 0) return OCStringCreateCopy(str);
    // Step 1: Create mutable copy and trim whitespace using OCTypes API
    OCMutableStringRef mutable_str = OCStringCreateMutableCopy(str);
    if (!mutable_str) return OCStringCreateCopy(str);
    OCStringTrimWhitespace(mutable_str);
    // Step 2: Check if we should remove outer parentheses
    // Only remove if they wrap the entire content and are not part of power notation
    uint64_t len = OCStringGetLength(mutable_str);
    if (len >= 2 &&
        OCStringGetCharacterAtIndex(mutable_str, 0) == '(' &&
        OCStringGetCharacterAtIndex(mutable_str, len - 1) == ')') {
        // Check if this is NOT a power notation parenthesis by looking for ^ after closing paren
        const char *cstr = OCStringGetCString(mutable_str);
        if (cstr) {
            size_t cstr_len = strlen(cstr);
            bool is_power_paren = false;
            // Look for ^ after the closing parenthesis in the original string
            // We need to check beyond the trimmed length since trimming might have removed trailing spaces
            const char *orig_cstr = OCStringGetCString(str);
            if (orig_cstr) {
                size_t orig_len = strlen(orig_cstr);
                // Find the position of the closing paren in the original string
                for (size_t i = 0; i < orig_len; i++) {
                    if (orig_cstr[i] == ')') {
                        // Check if there's a ^ immediately following this )
                        if (i + 1 < orig_len && orig_cstr[i + 1] == '^') {
                            is_power_paren = true;
                            break;
                        }
                    }
                }
            }
            if (!is_power_paren) {
                // Check for balanced parentheses to ensure we're removing outer ones
                int paren_count = 0;
                bool balanced = true;
                for (size_t i = 0; i < cstr_len; i++) {
                    if (cstr[i] == '(')
                        paren_count++;
                    else if (cstr[i] == ')')
                        paren_count--;
                    if (paren_count < 0 || (paren_count == 0 && i < cstr_len - 1)) {
                        balanced = false;
                        break;
                    }
                }
                if (balanced && paren_count == 0) {
                    // Remove outer parentheses using OCTypes API
                    OCStringDelete(mutable_str, OCRangeMake(len - 1, 1));  // Remove closing paren
                    OCStringDelete(mutable_str, OCRangeMake(0, 1));        // Remove opening paren
                    OCStringTrimWhitespace(mutable_str);                   // Trim any inner whitespace
                }
            }
        }
    }
    // Convert back to immutable string
    OCStringRef result = OCStringCreateCopy(mutable_str);
    OCRelease(mutable_str);
    return result ? result : OCStringCreateWithCString("");
}
// Helper function to expand parenthetical power expressions
// e.g., expand_parenthetical_power("m^2•kg/s", 4) -> "m^8•kg^4/s^4"
static OCStringRef expand_parenthetical_power(OCStringRef expression, int power) {
    if (!expression || power == 1) {
        return OCStringCreateCopy(expression);
    }
    if (power == 0) {
        return OCStringCreateWithCString("1");
    }
    
    // BULLETPROOF: Create completely independent temporary storage
    // Don't rely on global state at all
    ComponentList temp_positive = {0};
    ComponentList temp_negative = {0};
    temp_positive.capacity = 10;
    temp_positive.count = 0;
    temp_positive.components = malloc(temp_positive.capacity * sizeof(UnitComponent));
    temp_negative.capacity = 10;
    temp_negative.count = 0;
    temp_negative.components = malloc(temp_negative.capacity * sizeof(UnitComponent));
    
    // Save current global state (could be NULL)
    ComponentList *saved_positive = positive_components;
    ComponentList *saved_negative = negative_components;
    bool saved_in_denominator = in_denominator;
    bool saved_fractional_power_error = fractional_power_error;
    
    // Use our temporary storage
    positive_components = &temp_positive;
    negative_components = &temp_negative;
    in_denominator = false;
    fractional_power_error = false;
    // Parse the inner expression
    parse_expression_manually(OCStringGetCString(expression));
    // Apply the power to each component
    if (power < 0) {
        // For negative powers, we need to swap positive and negative components
        // and make the power positive
        int abs_power = -power;
        // Apply power and swap lists
        for (int i = 0; i < temp_positive.count; i++) {
            temp_positive.components[i].power *= abs_power;
        }
        for (int i = 0; i < temp_negative.count; i++) {
            temp_negative.components[i].power *= abs_power;
        }
        // Swap the lists (positive becomes negative and vice versa)
        ComponentList temp_swap = temp_positive;
        temp_positive = temp_negative;
        temp_negative = temp_swap;
    } else {
        // For positive powers, just multiply normally
        for (int i = 0; i < temp_positive.count; i++) {
            temp_positive.components[i].power *= power;
        }
        for (int i = 0; i < temp_negative.count; i++) {
            temp_negative.components[i].power *= power;
        }
    }
    // Create the expanded expression from components
    OCStringRef result = create_library_key();
    // Restore original component storage
    positive_components = saved_positive;
    negative_components = saved_negative;
    in_denominator = saved_in_denominator;
    fractional_power_error = saved_fractional_power_error;
    // Clean up temporary storage
    for (int i = 0; i < temp_positive.count; i++) {
        free(temp_positive.components[i].symbol);
    }
    free(temp_positive.components);
    for (int i = 0; i < temp_negative.count; i++) {
        free(temp_negative.components[i].symbol);
    }
    free(temp_negative.components);
    return result;
}
// Power extraction using C string approach
static PowerExtractionResult extract_power_from_token(OCStringRef token) {
    PowerExtractionResult result;
    result.base_token = NULL;
    result.power = 1;
    result.error = false;
    const char *token_cstr = OCStringGetCString(token);
    if (!token_cstr) {
        result.base_token = OCStringCreateCopy(token);
        return result;
    }
    // Look for caret (use strrchr to find the LAST caret for expressions like "(m^2)^-1")
    char *caret = strrchr(token_cstr, '^');
    if (!caret) {
        // No explicit power found
        // Check if this is a parenthetical expression that needs unwrapping
        size_t token_len = strlen(token_cstr);
        if (token_len >= 2 && token_cstr[0] == '(' && token_cstr[token_len - 1] == ')') {
            // This is a parenthetical expression like "(m/s)" with implicit power of 1
            // Extract the inner expression and return it for processing
            size_t inner_len = token_len - 2;
            char *inner_expr = malloc(inner_len + 1);
            strncpy(inner_expr, token_cstr + 1, inner_len);
            inner_expr[inner_len] = '\0';
            
            // Return the inner expression with power 1 for normal processing
            result.base_token = OCStringCreateWithCString(inner_expr);
            result.power = 1;
            result.error = false;
            free(inner_expr);
            return result;
        }
        // Not parenthetical - return as-is
        result.base_token = OCStringCreateCopy(token);
        return result;
    }
    // Extract base token (before caret)
    size_t base_len = caret - token_cstr;
    if (base_len == 0) {
        // Empty base - invalid
        result.base_token = OCStringCreateCopy(token);
        return result;
    }
    char *base = malloc(base_len + 1);
    if (!base) {
        // Allocation failed
        result.base_token = OCStringCreateCopy(token);
        return result;
    }
    strncpy(base, token_cstr, base_len);
    base[base_len] = '\0';
    // Extract power (after caret)
    char *power_str = caret + 1;
    // Parse the power value (could be integer or fractional)
    double power_double;
    bool is_fractional = false;
    // Handle parentheses around power: m^(-1) or m^(2) or m^(0.5)
    char *power_content = NULL;
    if (*power_str == '(') {
        char *close_paren = strchr(power_str, ')');
        if (close_paren) {
            // Extract content between parentheses
            size_t paren_len = close_paren - power_str - 1;
            power_content = malloc(paren_len + 1);
            strncpy(power_content, power_str + 1, paren_len);
            power_content[paren_len] = '\0';
            power_double = atof(power_content);
            // Check for fractional powers: decimal points or division (fractions)
            // BUT NOT negative signs - negative integers are allowed
            is_fractional = (strchr(power_content, '.') != NULL) ||
                            (strchr(power_content, '/') != NULL);
        } else {
            power_double = atof(power_str);
            is_fractional = (strchr(power_str, '.') != NULL);
        }
    } else {
        power_double = atof(power_str);
        // For powers not in parentheses, check for decimal points
        is_fractional = (strchr(power_str, '.') != NULL);
    }
    
    // Check for invalid power values (NaN, infinity)
    if (isnan(power_double) || isinf(power_double)) {
        result.base_token = OCStringCreateCopy(token);
        result.power = -9999;
        result.error = true;
        free(base);
        if (power_content) free(power_content);
        fractional_power_error = true;
        return result;
    }
    
    // Check if base token is a parenthetical expression that needs expansion
    // This handles cases like (m^2•kg/s)^4 -> m^8•kg^4/s^4
    // For fractional powers, we need to check if the mathematical result would be integer
    if (base[0] == '(' && base[base_len - 1] == ')') {
        // For parenthetical expressions with fractional powers, we need special handling
        if (is_fractional) {
            // Extract content inside parentheses
            size_t inner_len = base_len - 2;
            char *inner_expr = malloc(inner_len + 1);
            strncpy(inner_expr, base + 1, inner_len);
            inner_expr[inner_len] = '\0';
            
            // For fractional powers, only allow very simple cases that result in integer powers
            // Check for multiplication operators or complex expressions
            if (strstr(inner_expr, "*") != NULL || strstr(inner_expr, "•") != NULL || 
                strstr(inner_expr, "×") != NULL || strstr(inner_expr, "⋅") != NULL ||
                strstr(inner_expr, "kg") != NULL || strstr(inner_expr, "/") != NULL) {
                // Complex expression with multiple units - reject fractional powers
                result.base_token = OCStringCreateCopy(token);
                result.power = -9999;
                result.error = true;
                free(inner_expr);
                free(base);
                if (power_content) free(power_content);
                fractional_power_error = true;
                return result;
            }
            
            // Check if this is a simple case like (m^2)^0.5 that can be mathematically simplified
            if (strncmp(inner_expr, "m^", 2) == 0) {
                // Extract inner power
                double inner_power = atof(inner_expr + 2);
                double result_power = inner_power * power_double;
                // Check if result is effectively an integer
                double fractional_part = result_power - floor(result_power);
                if (fabs(fractional_part) < 1e-10) {
                    // Result is an integer - create simplified expression
                    int int_result = (int)round(result_power);
                    if (int_result == 1) {
                        result.base_token = OCStringCreateWithCString("m");
                        result.power = 1;
                    } else if (int_result == 0) {
                        result.base_token = OCStringCreateWithCString("1");
                        result.power = 1;
                    } else if (int_result == -1) {
                        // Special case: m^-1 should be formatted as 1/m
                        result.base_token = OCStringCreateWithCString("m");
                        result.power = -1;
                    } else if (int_result < 0) {
                        // Negative powers should be handled by the component system
                        result.base_token = OCStringCreateWithCString("m");
                        result.power = int_result;
                    } else {
                        char result_str[64];
                        snprintf(result_str, sizeof(result_str), "m^%d", int_result);
                        result.base_token = OCStringCreateWithCString(result_str);
                        result.power = 1;
                    }
                    result.error = false;
                    // Clean up
                    free(inner_expr);
                    free(base);
                    if (power_content) free(power_content);
                    return result;
                } else {
                    // Result is fractional - reject
                    result.base_token = OCStringCreateCopy(token);
                    result.power = -9999;
                    result.error = true;
                    free(inner_expr);
                    free(base);
                    if (power_content) free(power_content);
                    fractional_power_error = true;
                    return result;
                }
            }
            // Set base_token before returning error for complex fractional expressions
            result.base_token = OCStringCreateCopy(token);
            result.power = -9999;
            result.error = true;
            free(inner_expr);
            free(base);
            if (power_content) free(power_content);
            fractional_power_error = true;
            return result;
        }
        // Integer power case - use existing logic
        // Extract content inside parentheses
        size_t inner_len = base_len - 2;
        char *inner_expr = malloc(inner_len + 1);
        strncpy(inner_expr, base + 1, inner_len);
        inner_expr[inner_len] = '\0';
        
        // VALIDATION: Recursively validate the inner expression first
        OCStringRef inner_str = OCStringCreateWithCString(inner_expr);
        bool inner_valid = validate_expression_symbols(inner_expr);
        if (!inner_valid) {
            // Inner expression is invalid - reject the whole thing
            result.base_token = OCStringCreateCopy(token);
            result.power = -9999;
            result.error = true;
            OCRelease(inner_str);
            free(inner_expr);
            free(base);
            if (power_content) free(power_content);
            fractional_power_error = true;
            return result;
        }
        OCRelease(inner_str);
        
        // Inner expression is valid - proceed with expansion
        // BULLETPROOF MATHEMATICAL EXPANSION: Properly handle division in parenthetical expressions
        // Instead of naive string repetition, do proper mathematical expansion
        int int_power = (int)round(power_double);
        
        // Parse the inner expression to handle division correctly
        // Split by division first to get numerator and denominator parts
        OCStringRef inner_string = OCStringCreateWithCString(inner_expr);
        OCMutableStringRef expanded_str = OCStringCreateMutable(0);
        
        // Use OCString to split by division first
        OCStringRef slash_str = OCStringCreateWithCString("/");
        OCStringRef bullet_str = OCStringCreateWithCString("•");
        
        // Split by / to separate numerator and denominator
        OCArrayRef div_parts = OCStringCreateArrayBySeparatingStrings(inner_string, slash_str);
        OCIndex div_count = OCArrayGetCount(div_parts);
        
        // For splitting by multiplication, we need to handle both * and • 
        // Since we can't easily replace, we'll split by * first, then by • if needed
        OCStringRef asterisk_str = OCStringCreateWithCString("*");
        
        // Process numerator (first part)
        OCStringRef numerator_part = (OCStringRef)OCArrayGetValueAtIndex(div_parts, 0);
        
        // Split numerator by * first
        OCArrayRef num_terms_asterisk = OCStringCreateArrayBySeparatingStrings(numerator_part, asterisk_str);
        OCIndex num_count = OCArrayGetCount(num_terms_asterisk);
        
        // If there's only one term after splitting by *, try splitting by •
        OCArrayRef num_terms = NULL;
        if (num_count == 1) {
            num_terms = OCStringCreateArrayBySeparatingStrings(numerator_part, bullet_str);
            num_count = OCArrayGetCount(num_terms);
            OCRelease(num_terms_asterisk);
        } else {
            num_terms = num_terms_asterisk;
        }
        
        bool first_term = true;
        
        // Add numerator terms with positive powers
        for (OCIndex i = 0; i < num_count; i++) {
            if (!first_term) {
                OCStringAppend(expanded_str, bullet_str);
            }
            
            OCStringRef term = (OCStringRef)OCArrayGetValueAtIndex(num_terms, i);
            const char *term_cstr = OCStringGetCString(term);
            
            // Parse individual term for existing powers
            char *caret = strchr(term_cstr, '^');
            if (caret) {
                // Term has existing power like "m^2"
                size_t base_len = caret - term_cstr;
                char *base_term = malloc(base_len + 1);
                strncpy(base_term, term_cstr, base_len);
                base_term[base_len] = '\0';
                
                int existing_power = atoi(caret + 1);
                int new_power = existing_power * int_power;
                
                OCStringRef base_str = OCStringCreateWithCString(base_term);
                OCStringAppend(expanded_str, base_str);
                OCStringAppend(expanded_str, OCStringCreateWithCString("^"));
                
                char power_str[20];
                sprintf(power_str, "%d", new_power);
                OCStringAppend(expanded_str, OCStringCreateWithCString(power_str));
                
                OCRelease(base_str);
                free(base_term);
            } else {
                // Simple term like "m"
                OCStringAppend(expanded_str, term);
                if (int_power != 1) {
                    OCStringAppend(expanded_str, OCStringCreateWithCString("^"));
                    char power_str[20];
                    sprintf(power_str, "%d", int_power);
                    OCStringAppend(expanded_str, OCStringCreateWithCString(power_str));
                }
            }
            first_term = false;
        }
        
        // Process denominator parts (if any)
        for (OCIndex d = 1; d < div_count; d++) {
            OCStringRef denom_part = (OCStringRef)OCArrayGetValueAtIndex(div_parts, d);
            
            // Split denominator by * first, then by • if needed
            OCArrayRef den_terms_asterisk = OCStringCreateArrayBySeparatingStrings(denom_part, asterisk_str);
            OCIndex den_count = OCArrayGetCount(den_terms_asterisk);
            
            OCArrayRef den_terms = NULL;
            if (den_count == 1) {
                den_terms = OCStringCreateArrayBySeparatingStrings(denom_part, bullet_str);
                den_count = OCArrayGetCount(den_terms);
                OCRelease(den_terms_asterisk);
            } else {
                den_terms = den_terms_asterisk;
            }
            
            // Add division operator
            if (!first_term) {
                OCStringAppend(expanded_str, OCStringCreateWithCString("/"));
            }
            
            // Add denominator terms with negative powers
            for (OCIndex i = 0; i < den_count; i++) {
                if (i > 0) {
                    OCStringAppend(expanded_str, bullet_str);
                }
                
                OCStringRef term = (OCStringRef)OCArrayGetValueAtIndex(den_terms, i);
                const char *term_cstr = OCStringGetCString(term);
                
                // Parse individual term for existing powers
                char *caret = strchr(term_cstr, '^');
                if (caret) {
                    // Term has existing power like "s^2"
                    size_t base_len = caret - term_cstr;
                    char *base_term = malloc(base_len + 1);
                    strncpy(base_term, term_cstr, base_len);
                    base_term[base_len] = '\0';
                    
                    int existing_power = atoi(caret + 1);
                    int new_power = existing_power * int_power;
                    
                    OCStringRef base_str = OCStringCreateWithCString(base_term);
                    OCStringAppend(expanded_str, base_str);
                    OCStringAppend(expanded_str, OCStringCreateWithCString("^"));
                    
                    char power_str[20];
                    sprintf(power_str, "%d", new_power);
                    OCStringAppend(expanded_str, OCStringCreateWithCString(power_str));
                    
                    OCRelease(base_str);
                    free(base_term);
                } else {
                    // Simple term like "s"
                    OCStringAppend(expanded_str, term);
                    OCStringAppend(expanded_str, OCStringCreateWithCString("^"));
                    char power_str[20];
                    sprintf(power_str, "%d", int_power);
                    OCStringAppend(expanded_str, OCStringCreateWithCString(power_str));
                }
            }
            OCRelease(den_terms);
        }
        
        // Return the mathematically correct expanded expression for re-processing
        result.base_token = OCStringCreateCopy(expanded_str);  
        result.power = -1;                 // Special marker: -1 means "re-parse this"
        result.error = false;
        
        // Clean up OCString objects
        OCRelease(inner_string);
        OCRelease(expanded_str);
        OCRelease(asterisk_str);
        OCRelease(bullet_str);
        OCRelease(slash_str);
        OCRelease(div_parts);
        OCRelease(num_terms);
        
        free(inner_expr);
        free(base);
        if (power_content) free(power_content);
        return result;
    }
    // Non-parenthetical case - reject fractional powers
    if (is_fractional) {
        // Set base_token before returning error
        result.base_token = OCStringCreateCopy(token);
        result.power = -9999;
        result.error = true;
        free(base);
        if (power_content) free(power_content);
        fractional_power_error = true;
        return result;
    }
    // Normal case - return base token and power separately
    result.base_token = OCStringCreateWithCString(base);
    result.power = (int)round(power_double);
    result.error = false;
    free(base);
    if (power_content) free(power_content);
    return result;
}
// Tokenization by splitting on multiplication operators using OCTypes APIs
static OCArrayRef split_by_multiplication(OCStringRef str) {
    OCMutableArrayRef tokens = OCArrayCreateMutable(0, &kOCTypeArrayCallBacks);
    if (!str || OCStringGetLength(str) == 0) {
        return tokens;
    }
    // Create a working copy for parentheses-aware processing
    const char *cstr = OCStringGetCString(str);
    if (!cstr) return tokens;
    char *str_copy = strdup(cstr);
    if (!str_copy) return tokens;
    char *pos = str_copy;
    int paren_depth = 0;
    // Replace multiplication operators with delimiter, respecting parentheses
    while (*pos) {
        if (*pos == '(') {
            paren_depth++;
            pos++;
        } else if (*pos == ')') {
            paren_depth--;
            pos++;
        } else if (paren_depth == 0) {
            // Only replace multiplication operators when not inside parentheses
            if (*pos == '*') {
                *pos = '|';
                pos++;
            } else if (strncmp(pos, "•", 3) == 0) {  // UTF-8 bullet (3 bytes)
                *pos = '|';
                memmove(pos + 1, pos + 3, strlen(pos + 3) + 1);
                pos++;
            } else if (strncmp(pos, "×", 2) == 0) {  // UTF-8 multiplication sign (2 bytes)
                *pos = '|';
                memmove(pos + 1, pos + 2, strlen(pos + 2) + 1);
                pos++;
            } else if (strncmp(pos, "⋅", 3) == 0) {  // UTF-8 dot operator (3 bytes)
                *pos = '|';
                memmove(pos + 1, pos + 3, strlen(pos + 3) + 1);
                pos++;
            } else if (strncmp(pos, "∙", 3) == 0) {  // UTF-8 bullet operator (3 bytes)
                *pos = '|';
                memmove(pos + 1, pos + 3, strlen(pos + 3) + 1);
                pos++;
            } else if (strncmp(pos, "·", 2) == 0) {  // UTF-8 middle dot (2 bytes)
                *pos = '|';
                memmove(pos + 1, pos + 2, strlen(pos + 2) + 1);
                pos++;
            } else {
                pos++;
            }
        } else {
            // Inside parentheses - just move forward without replacing
            pos++;
        }
    }
    // Now use OCTypes string splitting safely
    OCStringRef processed_str = OCStringCreateWithCString(str_copy);
    free(str_copy);
    if (processed_str) {
        // Use OCTypes string splitting function with proper memory management
        OCArrayRef split_result = OCStringCreateArrayBySeparatingStrings(processed_str, STR("|"));
        if (split_result) {
            OCIndex count = OCArrayGetCount(split_result);
            for (OCIndex i = 0; i < count; i++) {
                OCStringRef token = (OCStringRef)OCArrayGetValueAtIndex(split_result, i);
                if (token) {
                    OCStringRef trimmed = trim_whitespace_and_parens(token);
                    if (trimmed && OCStringGetLength(trimmed) > 0) {
                        OCArrayInsertValueAtIndex(tokens, OCArrayGetCount(tokens), trimmed);
                    }
                    if (trimmed) {
                        OCRelease(trimmed);
                    }
                }
            }
            OCRelease(split_result);
        }
        OCRelease(processed_str);
    }
    return tokens;
}
// Simple manual parser for unit expressions - simplified approach
static void parse_expression_manually(const char *expr) {
    if (!expr) return;
    char *expr_copy = strdup(expr);
    // Smart splitting that respects parentheses for division too
    // Find division points that are not inside parentheses
    char *parts[10];  // Support up to 10 parts separated by division
    int part_count = 0;
    char *current_part_start = expr_copy;
    char *pos = expr_copy;
    int paren_depth = 0;
    while (*pos && part_count < 9) {
        if (*pos == '(') {
            paren_depth++;
        } else if (*pos == ')') {
            paren_depth--;
        } else if (*pos == '/' && paren_depth == 0) {
            // Found a division point outside parentheses
            *pos = '\0';  // Terminate current part
            parts[part_count++] = current_part_start;
            current_part_start = pos + 1;
        }
        pos++;
    }
    // Add the last part
    parts[part_count++] = current_part_start;
    // Process each part
    for (int part_index = 0; part_index < part_count; part_index++) {
        in_denominator = (part_index > 0);  // First part is numerator, rest are denominators
        char *part = parts[part_index];
        // Convert part to OCString for trimming
        OCStringRef part_str = OCStringCreateWithCString(part);
        OCStringRef trimmed = trim_whitespace_and_parens(part_str);
        // Split by multiplication operators
        OCArrayRef tokens = split_by_multiplication(trimmed);
        for (OCIndex j = 0; j < OCArrayGetCount(tokens); j++) {
            OCStringRef token = (OCStringRef)OCArrayGetValueAtIndex(tokens, j);
            // Extract power and base token using struct return
            PowerExtractionResult extraction = extract_power_from_token(token);
            // Check for fractional power error
            if (extraction.error || extraction.power == -9999) {
                // Fractional power detected - clean up and return without adding component
                OCRelease(extraction.base_token);
                // Set a global error flag or handle the error appropriately
                // For now, we'll skip this component which will result in an incomplete key
                continue;
            }
            
            // CRITICAL FIX: Handle expanded parenthetical expressions
            if (extraction.power == -1) {
                // This is an expanded parenthetical expression like "m*m" that needs re-parsing
                const char *expanded_expr = OCStringGetCString(extraction.base_token);
                
                // Re-parse the expanded expression recursively through split_by_multiplication
                OCStringRef expanded_str = OCStringCreateWithCString(expanded_expr);
                OCArrayRef expanded_tokens = split_by_multiplication(expanded_str);
                
                // Process each token in the expanded expression
                for (OCIndex k = 0; k < OCArrayGetCount(expanded_tokens); k++) {
                    OCStringRef expanded_token = (OCStringRef)OCArrayGetValueAtIndex(expanded_tokens, k);
                    // Recursively extract power (should be simple tokens now)
                    PowerExtractionResult sub_extraction = extract_power_from_token(expanded_token);
                    if (!sub_extraction.error && sub_extraction.power != -9999 && sub_extraction.power != -1) {
                        OCStringRef final_sub_token = trim_whitespace_and_parens(sub_extraction.base_token);
                        if (OCStringGetLength(final_sub_token) > 0) {
                            const char *symbol = OCStringGetCString(final_sub_token);
                            add_component(symbol, sub_extraction.power);
                        }
                        OCRelease(final_sub_token);
                    }
                    OCRelease(sub_extraction.base_token);
                }
                OCRelease(expanded_tokens);
                OCRelease(expanded_str);
                OCRelease(extraction.base_token);
                continue;
            }
            
            // Normal token processing
            // Trim the base token
            OCStringRef final_token = trim_whitespace_and_parens(extraction.base_token);
            if (OCStringGetLength(final_token) > 0) {
                const char *symbol = OCStringGetCString(final_token);
                add_component(symbol, extraction.power);
            }
            OCRelease(final_token);
            OCRelease(extraction.base_token);
        }
        OCRelease(tokens);
        OCRelease(trimmed);
        OCRelease(part_str);
    }
    free(expr_copy);
}
// Function to clean a unit expression by distributing exponents and grouping identical symbols
// Enhanced with comprehensive symbol validation using the units library
OCStringRef SIUnitCreateCleanedExpression(OCStringRef expression) {
    if (!expression) return NULL;
    
    // Handle empty string - return "1" for dimensionless
    if (OCStringGetLength(expression) == 0) {
        return OCStringCreateWithCString("1");
    }
    
    // Fast path for simple symbols that don't need complex parsing
    // Check if the expression is a simple symbol without operators
    const char *expr_cstr = OCStringGetCString(expression);
    if (expr_cstr) {
        // Check for whitespace-only strings
        bool has_non_whitespace = false;
        for (const char *p = expr_cstr; *p; p++) {
            if (!isspace(*p)) {
                has_non_whitespace = true;
                break;
            }
        }
        if (!has_non_whitespace) {
            return NULL;  // Whitespace-only string
        }
        
        // Check for pure numbers - return "1" for dimensionless
        bool is_pure_number = true;
        bool has_content = false;
        for (const char *p = expr_cstr; *p; p++) {
            if (!isspace(*p)) {
                has_content = true;
                if (!isdigit(*p) && *p != '.' && *p != '-' && *p != '+' && *p != 'e' && *p != 'E') {
                    is_pure_number = false;
                    break;
                }
            }
        }
        if (has_content && is_pure_number) {
            // Special case: "1" should return "1" for dimensionless
            if (strcmp(expr_cstr, "1") == 0) {
                return OCStringCreateWithCString("1");
            }
            return NULL;  // Other pure numbers, not a unit expression
        }
        
        // Enhanced validation: Check for invalid characters early
        for (const char *p = expr_cstr; *p; p++) {
            // Allow alphanumeric characters
            if (isalnum(*p)) continue;
            
            // Allow mathematical operators
            if (*p == '*' || *p == '/' || *p == '^' || *p == '(' || *p == ')' ||
                *p == '+' || *p == '-' || *p == '.') continue;
            
            // Allow Unicode mathematical operators (checking UTF-8 sequences)
            if (strncmp(p, "•", 3) == 0 || strncmp(p, "⋅", 3) == 0 || strncmp(p, "∙", 3) == 0) {
                // Skip the 3-byte UTF-8 character safely
                p += 2; // Move 2 more bytes (loop will increment by 1)
                continue;
            }
            if (strncmp(p, "×", 2) == 0 || strncmp(p, "·", 2) == 0) {
                // Skip the 2-byte UTF-8 character safely
                p += 1; // Move 1 more byte (loop will increment by 1)
                continue;
            }
            
            // Allow Greek letters and special characters commonly used in units
            if (strncmp(p, "μ", 2) == 0 || strncmp(p, "µ", 2) == 0 ||
                strncmp(p, "Ω", 2) == 0 || strncmp(p, "°", 2) == 0 ||
                strncmp(p, "π", 2) == 0 || strncmp(p, "α", 2) == 0) {
                // Skip the 2-byte UTF-8 character safely
                p += 1; // Move 1 more byte (loop will increment by 1)
                continue;
            }
            if (strncmp(p, "ℏ", 3) == 0 || strncmp(p, "∞", 3) == 0) {
                // Skip the 3-byte UTF-8 character safely
                p += 2; // Move 2 more bytes (loop will increment by 1)
                continue;
            }
            
            // Allow whitespace and underscore for subscripts
            if (isspace(*p) || *p == '_') continue;
            
            // Reject any other characters
            return NULL;
        }
        
        // Check for obviously malformed expressions first
        if (strlen(expr_cstr) == 0 || 
            strcmp(expr_cstr, "^") == 0 ||
            strcmp(expr_cstr, "^2") == 0 ||
            strncmp(expr_cstr, "^", 1) == 0 ||
            strstr(expr_cstr, "invalid") != NULL) {
            return NULL;
        }
        
        // Check for malformed power expressions
        if (strstr(expr_cstr, "m^") == expr_cstr && strlen(expr_cstr) == 2) {
            return NULL;  // "m^" without power
        }
        
        // Check for empty parentheses or malformed parentheses
        if (strstr(expr_cstr, "()") != NULL) {
            return NULL;
        }
        
        // Check for unbalanced parentheses
        int paren_count = 0;
        for (const char *p = expr_cstr; *p; p++) {
            if (*p == '(') {
                paren_count++;
            } else if (*p == ')') {
                paren_count--;
                if (paren_count < 0) {
                    return NULL;  // More closing than opening parens
                }
            }
        }
        if (paren_count != 0) {
            return NULL;  // Unbalanced parentheses
        }
        
        // Enhanced validation: Check if all unit symbols are valid using the library
        // This provides comprehensive validation against the actual units library
        bool has_valid_symbols = true;
        
        // Create a working copy for symbol validation
        char *expr_copy = strdup(expr_cstr);
        if (expr_copy) {
            // Smarter tokenization: replace operators but preserve context for exponents
            // First pass: identify and mark exponents by looking for ^ followed by numbers or parentheses
            char *pos = expr_copy;
            size_t original_len = strlen(expr_copy);
            size_t buffer_size = original_len * 3 + 50;  // Much more conservative size estimate
            char *new_expr = malloc(buffer_size);
            if (!new_expr) {
                free(expr_copy);
                return NULL;
            }
            
            char *write_pos = new_expr;
            char *buffer_end = new_expr + buffer_size - 1;  // Leave space for null terminator
            pos = expr_copy;
            
            while (*pos && write_pos < buffer_end) {
                if (*pos == '^') {
                    if (write_pos >= buffer_end) break;
                    *write_pos++ = ' ';  // Replace ^ with space
                    pos++;
                    // Skip whitespace after ^
                    while (*pos && isspace(*pos)) pos++;
                    
                    // Handle parenthetical powers like ^(-1) or ^(2)
                    if (*pos == '(') {
                        // Find the matching closing parenthesis
                        int paren_count = 1;
                        char *start = pos;
                        pos++; // Move past opening paren
                        
                        while (*pos && paren_count > 0) {
                            if (*pos == '(') paren_count++;
                            else if (*pos == ')') paren_count--;
                            pos++;
                        }
                        
                        // Check bounds before writing
                        size_t paren_len = pos - start;
                        if (write_pos + paren_len + 1 >= buffer_end) break;
                        
                        // Mark the entire parenthetical expression with ~
                        *write_pos++ = '~';
                        // Copy the parenthetical content
                        while (start < pos && write_pos < buffer_end) {
                            *write_pos++ = *start++;
                        }
                    } else if (*pos && (isdigit(*pos) || *pos == '-' || *pos == '+')) {
                        // Handle simple powers like ^2 or ^-1
                        char *start = pos;
                        if (*pos == '-' || *pos == '+') pos++;
                        while (*pos && (isdigit(*pos) || *pos == '.')) pos++;
                        
                        // Check bounds before writing
                        size_t exp_len = pos - start;
                        if (write_pos + exp_len + 1 >= buffer_end) break;
                        
                        // Mark the exponent with ~
                        *write_pos++ = '~';
                        // Copy the exponent
                        while (start < pos && write_pos < buffer_end) {
                            *write_pos++ = *start++;
                        }
                    }
                } else {
                    if (write_pos >= buffer_end) break;
                    *write_pos++ = *pos++;
                }
            }
            *write_pos = '\0';
            
            // Replace the old expression with the new one
            free(expr_copy);
            expr_copy = new_expr;
            
            // Second pass: replace remaining mathematical operators with spaces
            for (char *p = expr_copy; *p; p++) {
                if (*p == '*' || *p == '/' || *p == '(' || *p == ')') {
                    *p = ' ';
                } else if (strncmp(p, "•", 3) == 0) {  // UTF-8 bullet (3 bytes)
                    *p = ' ';
                    if (*(p+1)) *(p+1) = ' ';
                    if (*(p+2)) *(p+2) = ' ';
                    p += 2; // Skip the next 2 bytes (loop will increment by 1)
                } else if (strncmp(p, "×", 2) == 0 || strncmp(p, "·", 2) == 0) {  // UTF-8 2-byte chars
                    *p = ' ';
                    if (*(p+1)) *(p+1) = ' ';
                    p += 1; // Skip the next 1 byte (loop will increment by 1)
                } else if (strncmp(p, "⋅", 3) == 0 || strncmp(p, "∙", 3) == 0) {  // UTF-8 3-byte chars
                    *p = ' ';
                    if (*(p+1)) *(p+1) = ' ';
                    if (*(p+2)) *(p+2) = ' ';
                    p += 2; // Skip the next 2 bytes (loop will increment by 1)
                }
                }
            }
            
            // Tokenize and validate each symbol
            char *token;
            char *saveptr;
            token = strtok_r(expr_copy, " \t", &saveptr);
            while (token != NULL && has_valid_symbols) {
                // Skip empty tokens
                if (strlen(token) == 0) {
                    token = strtok_r(NULL, " \t", &saveptr);
                    continue;
                }
                
                // Check if this token is marked as an exponent
                bool is_exponent = (token[0] == '~');
                if (is_exponent) {
                    // Skip validation for exponents - they're numbers, not unit symbols
                    token = strtok_r(NULL, " \t", &saveptr);
                    continue;
                }
                
                // For non-exponent tokens, validate against underived library
                OCStringRef symbol_str = OCStringCreateWithCString(token);
                OCMutableArrayRef underivedLib = SIUnitGetTokenSymbolsLib();
                if (!OCArrayContainsValue(underivedLib, symbol_str)) {
                    has_valid_symbols = false;
                }
                OCRelease(symbol_str);
                
                token = strtok_r(NULL, " \t", &saveptr);
            }
            free(expr_copy);
        }
        
        if (!has_valid_symbols) {
            return NULL;  // Invalid unit symbols found
        }
        
        // Force all expressions through full parsing for consistent sorting
        // (Disabled fast path to ensure proper symbol ordering)
        /*
        bool is_simple = true;
        for (const char *p = expr_cstr; *p; p++) {
            if (*p == '/' || *p == '*' || *p == '^' || *p == '(' || *p == ')' ||
                strncmp(p, "•", 3) == 0 || strncmp(p, "×", 2) == 0 ||
                strncmp(p, "⋅", 3) == 0 || strncmp(p, "∙", 3) == 0 ||
                strncmp(p, "·", 2) == 0) {
                is_simple = false;
                break;
            }
        }
        // For simple symbols, just return a normalized copy
        if (is_simple) {
            return SIUnitCreateNormalizedExpression(expression, true);
        }
        */
    }
    // Complex expression - use full parsing
    // First normalize the expression to handle Unicode variants (μ → µ, etc.)
    OCStringRef normalized_expression = SIUnitCreateNormalizedExpression(expression, true);
    if (!normalized_expression) return NULL;
    // Initialize component lists
    init_component_lists();
    // Convert normalized OCString to C string for parsing
    // Parse the expression
    parse_expression_manually(OCStringGetCString(normalized_expression));
    // Check for fractional power error
    if (fractional_power_error) {
        // Clean up component lists
        free_component_lists();
        // Release the normalized expression
        OCRelease(normalized_expression);
        return NULL;  // Return NULL to indicate error
    }
    
    // CRITICAL: Validate all parsed symbols against the underived library
    // This catches cases like m^2*1 where 1 is parsed as a symbol but invalid as a unit
    OCMutableArrayRef underivedLib = SIUnitGetTokenSymbolsLib();
    
    // Validate positive components
    for (int i = 0; i < positive_components->count; i++) {
        const char *symbol = positive_components->components[i].symbol;
        OCStringRef symbol_str = OCStringCreateWithCString(symbol);
        if (!OCArrayContainsValue(underivedLib, symbol_str)) {
            OCRelease(symbol_str);
            free_component_lists();
            OCRelease(normalized_expression);
            return NULL;  // Invalid symbol found
        }
        OCRelease(symbol_str);
    }
    
    // Validate negative components  
    for (int i = 0; i < negative_components->count; i++) {
        const char *symbol = negative_components->components[i].symbol;
        OCStringRef symbol_str = OCStringCreateWithCString(symbol);
        if (!OCArrayContainsValue(underivedLib, symbol_str)) {
            OCRelease(symbol_str);
            free_component_lists();
            OCRelease(normalized_expression);
            return NULL;  // Invalid symbol found
        }
        OCRelease(symbol_str);
    }
    // Group identical symbols in numerator and denominator (no cancellation between them)
    typedef struct {
        char *symbol;
        int total_power;
    } GroupedComponent;
    
    GroupedComponent grouped_positive[100];
    int positive_count = 0;
    
    // Group positive components
    for (int i = 0; i < positive_components->count; i++) {
        UnitComponent *comp = &positive_components->components[i];
        if (comp->power == 0) continue;
        
        // Find if this symbol already exists in grouped positive
        int found_idx = -1;
        for (int j = 0; j < positive_count; j++) {
            if (strcmp(grouped_positive[j].symbol, comp->symbol) == 0) {
                found_idx = j;
                break;
            }
        }
        
        if (found_idx >= 0) {
            grouped_positive[found_idx].total_power += comp->power;
        } else {
            grouped_positive[positive_count].symbol = strdup(comp->symbol);
            grouped_positive[positive_count].total_power = comp->power;
            positive_count++;
        }
    }
    
    // Group identical symbols in denominator  
    GroupedComponent grouped_negative[100];
    int negative_count = 0;
    
    // Group negative components
    for (int i = 0; i < negative_components->count; i++) {
        UnitComponent *comp = &negative_components->components[i];
        if (comp->power == 0) continue;
        
        // Find if this symbol already exists in grouped negative
        int found_idx = -1;
        for (int j = 0; j < negative_count; j++) {
            if (strcmp(grouped_negative[j].symbol, comp->symbol) == 0) {
                found_idx = j;
                break;
            }
        }
        
        if (found_idx >= 0) {
            grouped_negative[found_idx].total_power += comp->power;
        } else {
            grouped_negative[negative_count].symbol = strdup(comp->symbol);
            grouped_negative[negative_count].total_power = comp->power;
            negative_count++;
        }
    }
    
    // Build the cleaned expression (grouped but not reduced)
    OCMutableStringRef result = OCStringCreateMutable(0);
    
    // Build numerator
    bool first_positive = true;
    for (int i = 0; i < positive_count; i++) {
        if (grouped_positive[i].total_power > 0) {
            if (!first_positive) {
                OCStringAppend(result, STR("•"));
            }
            OCStringRef compString = OCStringCreateWithCString(grouped_positive[i].symbol);
            OCStringAppend(result, compString);
            OCRelease(compString);
            if (grouped_positive[i].total_power != 1) {
                OCStringAppendFormat(result, STR("^%d"), grouped_positive[i].total_power);
            }
            first_positive = false;
        }
    }
    
    // Build denominator if we have negative components
    if (negative_count > 0) {
        bool has_denominator_terms = false;
        for (int i = 0; i < negative_count; i++) {
            if (grouped_negative[i].total_power > 0) {
                has_denominator_terms = true;
                break;
            }
        }
        
        if (has_denominator_terms) {
            if (OCStringGetLength(result) == 0) {
                OCStringAppend(result, STR("1"));
            }
            OCStringAppend(result, STR("/"));
            
            // Count denominator terms to determine if we need parentheses
            int denom_term_count = 0;
            for (int i = 0; i < negative_count; i++) {
                if (grouped_negative[i].total_power > 0) {
                    denom_term_count++;
                }
            }
            
            bool needs_parens = (denom_term_count > 1);
            if (needs_parens) {
                OCStringAppend(result, STR("("));
            }
            
            bool first_negative = true;
            for (int i = 0; i < negative_count; i++) {
                if (grouped_negative[i].total_power > 0) {
                    if (!first_negative) {
                        OCStringAppend(result, STR("•"));
                    }
                    OCStringRef compString = OCStringCreateWithCString(grouped_negative[i].symbol);
                    OCStringAppend(result, compString);
                    OCRelease(compString);
                    if (grouped_negative[i].total_power != 1) {
                        OCStringAppendFormat(result, STR("^%d"), grouped_negative[i].total_power);
                    }
                    first_negative = false;
                }
            }
            
            if (needs_parens) {
                OCStringAppend(result, STR(")"));
            }
        }
    }
    
    // Handle the case where we have no valid terms
    if (OCStringGetLength(result) == 0) {
        OCStringAppend(result, STR("1"));  // Return "1" for dimensionless
    }
    
    // Clean up grouped components
    for (int i = 0; i < positive_count; i++) {
        free(grouped_positive[i].symbol);
    }
    for (int i = 0; i < negative_count; i++) {
        free(grouped_negative[i].symbol);
    }
    
    // Return the cleaned (but not reduced) expression
    OCStringRef final_result = OCStringCreateCopy(result);
    OCRelease(result);
    // Clean up component lists
    free_component_lists();
    // Release the normalized expression
    OCRelease(normalized_expression);
    return final_result;
}
// Function to clean and reduce a unit expression by combining like terms and canceling between numerator/denominator
OCStringRef SIUnitCreateCleanedAndReducedExpression(OCStringRef expression) {
    if (!expression) return NULL;
    
    // First, apply comprehensive validation and cleaning using the existing function
    OCStringRef cleaned_expression = SIUnitCreateCleanedExpression(expression);
    if (!cleaned_expression) {
        return NULL;  // Invalid expression - validation failed
    }
    
    // Special case: if cleaned expression is "1", return it as-is (dimensionless)
    if (OCStringCompare(cleaned_expression, STR("1"), 0) == kOCCompareEqualTo) {
        return cleaned_expression;  // Already reduced
    }
    
    // Initialize component lists for reduction processing
    init_component_lists();
    
    // Parse the cleaned expression (we know it's valid at this point)
    parse_expression_manually(OCStringGetCString(cleaned_expression));
    
    // Now reduce by combining like terms across numerator and denominator (CANCELLATION)
    typedef struct {
        char *symbol;
        int net_power;
    } NetComponent;
    
    // Count unique symbols
    int unique_count = 0;
    NetComponent temp_components[100];  // Temporary array for collecting unique symbols
    
    // Process positive components
    for (int i = 0; i < positive_components->count; i++) {
        UnitComponent *comp = &positive_components->components[i];
        if (comp->power == 0) continue;
        
        // Find if this symbol already exists
        int found_idx = -1;
        for (int j = 0; j < unique_count; j++) {
            if (strcmp(temp_components[j].symbol, comp->symbol) == 0) {
                found_idx = j;
                break;
            }
        }
        
        if (found_idx >= 0) {
            temp_components[found_idx].net_power += comp->power;
        } else {
            temp_components[unique_count].symbol = strdup(comp->symbol);
            temp_components[unique_count].net_power = comp->power;
            unique_count++;
        }
    }
    
    // Process negative components
    for (int i = 0; i < negative_components->count; i++) {
        UnitComponent *comp = &negative_components->components[i];
        if (comp->power == 0) continue;
        
        // Find if this symbol already exists
        int found_idx = -1;
        for (int j = 0; j < unique_count; j++) {
            if (strcmp(temp_components[j].symbol, comp->symbol) == 0) {
                found_idx = j;
                break;
            }
        }
        
        if (found_idx >= 0) {
            temp_components[found_idx].net_power -= comp->power;
        } else {
            temp_components[unique_count].symbol = strdup(comp->symbol);
            temp_components[unique_count].net_power = -comp->power;
            unique_count++;
        }
    }
    
    // Build the reduced expression from net powers
    OCMutableStringRef result = OCStringCreateMutable(0);
    bool has_numerator = false;
    bool has_denominator = false;
    
    // First pass: build numerator (positive net powers)
    bool first_positive = true;
    for (int i = 0; i < unique_count; i++) {
        if (temp_components[i].net_power > 0) {
            if (!first_positive) {
                OCStringAppend(result, STR("•"));
            }
            OCStringRef compString = OCStringCreateWithCString(temp_components[i].symbol);
            OCStringAppend(result, compString);
            OCRelease(compString);
            if (temp_components[i].net_power != 1) {
                OCStringAppendFormat(result, STR("^%d"), temp_components[i].net_power);
            }
            first_positive = false;
            has_numerator = true;
        }
    }
    
    // Second pass: check if we have denominator terms
    for (int i = 0; i < unique_count; i++) {
        if (temp_components[i].net_power < 0) {
            has_denominator = true;
            break;
        }
    }
    
    // Third pass: build denominator (negative net powers)
    if (has_denominator) {
        if (!has_numerator) {
            OCStringAppend(result, STR("1"));
        }
        OCStringAppend(result, STR("/"));
        
        // Count denominator terms to determine if we need parentheses
        int denom_count = 0;
        for (int i = 0; i < unique_count; i++) {
            if (temp_components[i].net_power < 0) {
                denom_count++;
            }
        }
        
        bool needs_parens = (denom_count > 1);
        if (needs_parens) {
            OCStringAppend(result, STR("("));
        }
        
        bool first_negative = true;
        for (int i = 0; i < unique_count; i++) {
            if (temp_components[i].net_power < 0) {
                if (!first_negative) {
                    OCStringAppend(result, STR("•"));
                }
                OCStringRef compString = OCStringCreateWithCString(temp_components[i].symbol);
                OCStringAppend(result, compString);
                OCRelease(compString);
                int abs_power = -temp_components[i].net_power;
                if (abs_power != 1) {
                    OCStringAppendFormat(result, STR("^%d"), abs_power);
                }
                first_negative = false;
            }
        }
        
        if (needs_parens) {
            OCStringAppend(result, STR(")"));
        }
    }
    
    // Handle the case where everything cancels out
    if (OCStringGetLength(result) == 0) {
        OCStringAppend(result, STR("1"));  // Return "1" for dimensionless
    }
    
    // Clean up temporary components
    for (int i = 0; i < unique_count; i++) {
        free(temp_components[i].symbol);
    }
    
    // Clean up component lists
    free_component_lists();
    
    // Release the cleaned expression
    OCRelease(cleaned_expression);
    
    return result;
}
// Function to check if two expressions are equivalent using canonicalization
bool SIUnitAreExpressionsEquivalent(OCStringRef expr1, OCStringRef expr2) {
    if (!expr1 || !expr2) return false;
    if (OCStringCompare(expr1, expr2, 0) == kOCCompareEqualTo) {
        return true;  // Exact match
    }
    OCStringRef canonical1 = SIUnitCreateCleanedExpression(expr1);
    OCStringRef canonical2 = SIUnitCreateCleanedExpression(expr2);
    bool equivalent = (OCStringCompare(canonical1, canonical2, 0) == kOCCompareEqualTo);
    OCRelease(canonical1);
    OCRelease(canonical2);
    return equivalent;
}
OCMutableStringRef SIUnitCreateNormalizedExpression(OCStringRef expression, bool forLibraryLookup) {
    if (!expression) return NULL;
    OCMutableStringRef normalized = OCStringCreateMutableCopy(expression);
    if (!normalized) return NULL;
    // Replace Unicode multiplication and division symbols with ASCII equivalents
    OCStringFindAndReplace2(normalized, STR("×"), STR("*"));
    OCStringFindAndReplace2(normalized, STR("·"), STR("*"));
    OCStringFindAndReplace2(normalized, STR("⋅"), STR("*"));  // Dot operator
    OCStringFindAndReplace2(normalized, STR("∙"), STR("*"));  // Bullet operator
    OCStringFindAndReplace2(normalized, STR("÷"), STR("/"));
    OCStringFindAndReplace2(normalized, STR("∕"), STR("/"));  // Division slash
    OCStringFindAndReplace2(normalized, STR("⁄"), STR("/"));  // Fraction slash
    // Normalize Unicode mu characters to standard micro sign (µ = U+00B5)
    OCStringFindAndReplace2(normalized, STR("μ"), STR("µ"));  // Greek Small Letter Mu (U+03BC)
    OCStringFindAndReplace2(normalized, STR("Μ"), STR("µ"));  // Greek Capital Letter Mu (U+039C)
    OCStringFindAndReplace2(normalized, STR("ɥ"), STR("µ"));  // Latin Small Letter Turned H (U+0265) - sometimes confused
    OCStringFindAndReplace2(normalized, STR("𝜇"), STR("µ"));  // Mathematical Italic Small Mu (U+1D707)
    OCStringFindAndReplace2(normalized, STR("𝝁"), STR("µ"));  // Mathematical Bold Small Mu (U+1D741)
    OCStringFindAndReplace2(normalized, STR("𝝻"), STR("µ"));  // Mathematical Bold Italic Small Mu (U+1D77B)
    // Remove spaces around operators for consistent formatting
    OCStringFindAndReplace2(normalized, STR(" * "), STR("*"));
    OCStringFindAndReplace2(normalized, STR(" / "), STR("/"));
    OCStringFindAndReplace2(normalized, STR(" ^ "), STR("^"));
    // Trim leading/trailing whitespace
    OCStringTrimWhitespace(normalized);
    if (forLibraryLookup) {
        // For library lookup, replace * with • for consistent symbol representation
        OCStringFindAndReplace2(normalized, STR("*"), STR("•"));
    } else {
        // For display, replace • with * for multiplication
        OCStringFindAndReplace2(normalized, STR("•"), STR("*"));
    }
    return normalized;
}
