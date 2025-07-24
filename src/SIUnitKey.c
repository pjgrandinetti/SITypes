#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
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

// Function prototypes
static void init_component_lists(void);
static void free_component_lists(void);
static void add_component(const char *symbol, int power);
static void consolidate_powers(ComponentList *list);
static int compare_symbols(const void *a, const void *b);
static OCStringRef create_library_key(void);
static void parse_expression_with_ocstring(OCStringRef expr);

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

// Simple string trimming using C string conversion (temporary approach)
static OCStringRef trim_whitespace_and_parens(OCStringRef str) {
    if (!str || OCStringGetLength(str) == 0) return OCStringCreateCopy(str);
    
    // Convert to C string for simpler processing
    const char *cstr = OCStringGetCString(str);
    if (!cstr) return OCStringCreateCopy(str);
    
    size_t len = strlen(cstr);
    if (len == 0) return OCStringCreateWithCString("");
    
    // Find start position (skip whitespace only, be careful with parens)
    size_t start = 0;
    while (start < len && (cstr[start] == ' ' || cstr[start] == '\t')) {
        start++;
    }
    
    // Find end position (skip whitespace only, be careful with parens)
    size_t end = len - 1;
    while (end > start && (cstr[end] == ' ' || cstr[end] == '\t')) {
        end--;
    }
    
    // Only remove outer parentheses if they wrap the entire meaningful content
    // and are not part of power notation (don't have ^ before them)
    if (start < end && cstr[start] == '(' && cstr[end] == ')') {
        // Check if this is NOT a power notation parenthesis
        bool is_power_paren = false;
        if (start > 0 && cstr[start - 1] == '^') {
            is_power_paren = true;
        }
        
        if (!is_power_paren) {
            // Check for balanced parentheses to ensure we're removing outer ones
            int paren_count = 0;
            bool balanced = true;
            for (size_t i = start; i <= end; i++) {
                if (cstr[i] == '(') paren_count++;
                else if (cstr[i] == ')') paren_count--;
                
                if (paren_count < 0 || (paren_count == 0 && i < end)) {
                    balanced = false;
                    break;
                }
            }
            
            if (balanced && paren_count == 0) {
                start++; // Skip opening paren
                end--;   // Skip closing paren
            }
        }
    }
    
    if (start > end) {
        return OCStringCreateWithCString("");
    }
    
    // Create substring
    size_t result_len = end - start + 1;
    char *result = malloc(result_len + 1);
    strncpy(result, cstr + start, result_len);
    result[result_len] = '\0';
    
    OCStringRef trimmed = OCStringCreateWithCString(result);
    free(result);
    return trimmed;
}

// Power extraction using C string approach
static int extract_power_from_token(OCStringRef token, OCStringRef *base_token) {
    const char *token_cstr = OCStringGetCString(token);
    if (!token_cstr) {
        *base_token = OCStringCreateCopy(token);
        return 1;
    }
    
    // Look for caret
    char *caret = strchr(token_cstr, '^');
    if (!caret) {
        *base_token = OCStringCreateCopy(token);
        return 1;
    }
    
    // Extract base token (before caret)
    size_t base_len = caret - token_cstr;
    char *base = malloc(base_len + 1);
    strncpy(base, token_cstr, base_len);
    base[base_len] = '\0';
    *base_token = OCStringCreateWithCString(base);
    free(base);
    
    // Extract power (after caret)
    char *power_str = caret + 1;
    
    // Check for fractional powers - reject them
    if (strchr(power_str, '.')) {
        // Found a decimal point in the power - this is not allowed
        fractional_power_error = true;
        return -9999; // Special error code for fractional powers
    }
    
    // Handle parentheses around power: m^(-1) or m^(2)
    if (*power_str == '(') {
        char *close_paren = strchr(power_str, ')');
        if (close_paren) {
            // Extract content between parentheses
            size_t paren_len = close_paren - power_str - 1;
            char *paren_content = malloc(paren_len + 1);
            strncpy(paren_content, power_str + 1, paren_len);
            paren_content[paren_len] = '\0';
            
            // Check for fractional powers in parentheses
            if (strchr(paren_content, '.')) {
                free(paren_content);
                fractional_power_error = true;
                return -9999; // Special error code for fractional powers
            }
            
            int power = atoi(paren_content);
            free(paren_content);
            return power;
        }
    }
    
    int power = atoi(power_str);
    return power;
}

// Tokenization by splitting on multiplication operators - simpler approach
static OCArrayRef split_by_multiplication(OCStringRef str) {
    OCMutableArrayRef tokens = OCArrayCreateMutable(0, &kOCTypeArrayCallBacks);
    
    if (!str || OCStringGetLength(str) == 0) {
        return tokens;
    }
    
    const char *cstr = OCStringGetCString(str);
    if (!cstr) return tokens;
    
    char *str_copy = strdup(cstr);
    char *token;
    char *save_ptr;
    
    // UTF-8 safe splitting by replacing multiplication operators with a delimiter
    // Replace all multiplication operators with | to avoid UTF-8 corruption
    char *pos = str_copy;
    while (*pos) {
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
    }
    
    // Now split by single ASCII delimiter
    token = strtok_r(str_copy, "|", &save_ptr);
    while (token) {
        OCStringRef token_str = OCStringCreateWithCString(token);
        OCStringRef trimmed = trim_whitespace_and_parens(token_str);
        if (OCStringGetLength(trimmed) > 0) {
            OCArrayInsertValueAtIndex(tokens, OCArrayGetCount(tokens), trimmed);
        }
        OCRelease(trimmed);
        OCRelease(token_str);
        
        token = strtok_r(NULL, "|", &save_ptr);
    }
    
    free(str_copy);
    return tokens;
}

// Simple manual parser for unit expressions - simplified approach
static void parse_expression_manually(const char *expr) {
    if (!expr) return;
    
    char *expr_copy = strdup(expr);
    char *save_ptr;
    
    // Split by '/' to separate numerator and denominators
    char *part = strtok_r(expr_copy, "/", &save_ptr);
    int part_index = 0;
    
    while (part) {
        in_denominator = (part_index > 0); // First part is numerator, rest are denominators
        
        // Convert part to OCString for trimming
        OCStringRef part_str = OCStringCreateWithCString(part);
        OCStringRef trimmed = trim_whitespace_and_parens(part_str);
        
        // Split by multiplication operators
        OCArrayRef tokens = split_by_multiplication(trimmed);
        
        for (OCIndex j = 0; j < OCArrayGetCount(tokens); j++) {
            OCStringRef token = (OCStringRef)OCArrayGetValueAtIndex(tokens, j);
            
            // Extract power and base token
            OCStringRef base_token;
            int power = extract_power_from_token(token, &base_token);
            
            // Check for fractional power error
            if (power == -9999) {
                // Fractional power detected - clean up and return without adding component
                OCRelease(base_token);
                // Set a global error flag or handle the error appropriately
                // For now, we'll skip this component which will result in an incomplete key
                continue;
            }
            
            // Trim the base token
            OCStringRef final_token = trim_whitespace_and_parens(base_token);
            
            if (OCStringGetLength(final_token) > 0) {
                const char *symbol = OCStringGetCString(final_token);
                add_component(symbol, power);
            }
            
            OCRelease(final_token);
            OCRelease(base_token);
        }
        
        OCRelease(tokens);
        OCRelease(trimmed);
        OCRelease(part_str);
        
        part = strtok_r(NULL, "/", &save_ptr);
        part_index++;
    }
    
    free(expr_copy);
}

// Main function to canonicalize a unit expression
OCStringRef SIUnitCreateLibraryKey(OCStringRef expression) {
    if (!expression) return NULL;
    
    // First normalize the expression to handle Unicode variants (μ → µ, etc.)
    OCStringRef normalized_expression = SIUnitCreateNormalizedExpression(expression, true);
    if (!normalized_expression) return NULL;
    
    // Initialize component lists
    init_component_lists();
    
    // Convert normalized OCString to C string for parsing
    const char *expr_cstr = OCStringGetCString(normalized_expression);
    
    // Parse the expression
    parse_expression_manually(expr_cstr);
    
    // Check for fractional power error
    if (fractional_power_error) {
        // Clean up component lists
        free_component_lists();
        // Release the normalized expression
        OCRelease(normalized_expression);
        return NULL; // Return NULL to indicate error
    }
    
    // Create library key from components
    OCStringRef library_key = create_library_key();
    
    // Clean up component lists
    free_component_lists();
    
    // Release the normalized expression
    OCRelease(normalized_expression);
    
    return library_key;
}

// Function to check if two expressions are equivalent using canonicalization
bool SIUnitAreExpressionsEquivalent(OCStringRef expr1, OCStringRef expr2) {
    if (!expr1 || !expr2) return false;
    
    if (OCStringCompare(expr1, expr2, 0) == kOCCompareEqualTo) {
        return true;  // Exact match
    }
    
    OCStringRef canonical1 = SIUnitCreateLibraryKey(expr1);
    OCStringRef canonical2 = SIUnitCreateLibraryKey(expr2);
    
    bool equivalent = (OCStringCompare(canonical1, canonical2, 0) == kOCCompareEqualTo);
    
    OCRelease(canonical1);
    OCRelease(canonical2);
    
    return equivalent;
}
