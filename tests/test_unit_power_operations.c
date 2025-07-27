#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "SILibrary.h"
#include "test_unit_power_operations.h"

bool test_unit_negative_power_operations(void) {
    printf("Running test_unit_negative_power_operations...\n");
    
    // Test: Unit raised to negative power should produce inverse
    OCStringRef error = NULL;
    double multiplier = 1.0;
    SIUnitRef m_squared = SIUnitFromExpression(STR("m^2"), &multiplier, &error);
    if (!m_squared) {
        printf("  ✗ Failed to create m^2 unit\n");
        if (error) {
            printf("    Error: %s\n", OCStringGetCString(error));
            OCRelease(error);
        }
        return false;
    }
    
    double power_multiplier = 1.0;
    OCStringRef power_error = NULL;
    SIUnitRef result = SIUnitByRaisingToPowerWithoutReducing(m_squared, -1, &power_multiplier, &power_error);
    if (!result) {
        printf("  ✗ Failed to raise m^2 to power -1\n");
        if (power_error) {
            printf("    Error: %s\n", OCStringGetCString(power_error));
            OCRelease(power_error);
        }
        OCRelease(m_squared);
        return false;
    }
    
    OCStringRef symbol = SIUnitCreateSymbol(result);
    if (!symbol) {
        printf("  ✗ Failed to create symbol for (m^2)^-1\n");
        OCRelease(result);
        OCRelease(m_squared);
        return false;
    }
    
    const char* symbol_cstr = OCStringGetCString(symbol);
    bool test_passed = (strcmp(symbol_cstr, "(1/m^2)") == 0);
    
    if (!test_passed) {
        printf("  ✗ (m^2)^-1 = '%s', expected '(1/m^2)'\n", symbol_cstr);
    }
    
    OCRelease(symbol);
    OCRelease(result);
    OCRelease(m_squared);
    if (error) OCRelease(error);
    if (power_error) OCRelease(power_error);
    
    if (test_passed) {
        printf("test_unit_negative_power_operations passed\n");
    }
    return test_passed;
}

bool test_unit_inverse_expression_parsing(void) {
    printf("Running test_unit_inverse_expression_parsing...\n");
    
    // Test: parsing '1/m^2' should produce inverse square meter
    double multiplier = 1.0;
    OCStringRef error = NULL;
    SIUnitRef parsed_inverse = SIUnitFromExpression(STR("1/m^2"), &multiplier, &error);
    if (!parsed_inverse) {
        printf("  ✗ Failed to parse '1/m^2'\n");
        if (error) {
            printf("    Error: %s\n", OCStringGetCString(error));
            OCRelease(error);
        }
        return false;
    }
    
    OCStringRef symbol = SIUnitCreateSymbol(parsed_inverse);
    if (!symbol) {
        printf("  ✗ Failed to create symbol for parsed 1/m^2\n");
        OCRelease(parsed_inverse);
        return false;
    }
    
    const char* symbol_cstr = OCStringGetCString(symbol);
    bool test_passed = (strcmp(symbol_cstr, "(1/m^2)") == 0);
    
    if (!test_passed) {
        printf("  ✗ '1/m^2' parsed as '%s', expected '(1/m^2)'\n", symbol_cstr);
    }
    
    OCRelease(symbol);
    OCRelease(parsed_inverse);
    if (error) OCRelease(error);
    
    if (test_passed) {
        printf("test_unit_inverse_expression_parsing passed\n");
    }
    return test_passed;
}

bool test_unit_negative_exponent_parsing(void) {
    printf("Running test_unit_negative_exponent_parsing...\n");
    
    // Test: parsing 'm^-2' should produce inverse square meter  
    double multiplier = 1.0;
    OCStringRef error = NULL;
    SIUnitRef parsed_negative = SIUnitFromExpression(STR("m^-2"), &multiplier, &error);
    if (!parsed_negative) {
        printf("  ✗ Failed to parse 'm^-2'\n");
        if (error) {
            printf("    Error: %s\n", OCStringGetCString(error));
            OCRelease(error);
        }
        return false;
    }
    
    OCStringRef symbol = SIUnitCreateSymbol(parsed_negative);
    if (!symbol) {
        printf("  ✗ Failed to create symbol for parsed m^-2\n");
        OCRelease(parsed_negative);
        return false;
    }
    
    const char* symbol_cstr = OCStringGetCString(symbol);
    bool test_passed = (strcmp(symbol_cstr, "(1/m^2)") == 0);
    
    if (!test_passed) {
        printf("  ✗ 'm^-2' parsed as '%s', expected '(1/m^2)'\n", symbol_cstr);
    }
    
    OCRelease(symbol);
    OCRelease(parsed_negative);
    if (error) OCRelease(error);
    
    if (test_passed) {
        printf("test_unit_negative_exponent_parsing passed\n");
    }
    return test_passed;
}
