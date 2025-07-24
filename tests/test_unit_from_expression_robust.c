//
//  test_unit_from_expression_robust.c
//  SITypes
//
//  Comprehensive tests for SIUnitFromExpression to catch compatibility issues
//

#include "test_unit_from_expression_robust.h"
#include "SILibrary.h"
#include <stdio.h>
#include <math.h>

// Test helper to verify unit behavior
static bool verify_unit_behavior(OCStringRef expression, double expected_multiplier, OCStringRef expected_symbol, const char* test_name) {
    printf("  Testing '%s':\n", OCStringGetCString(expression));
    
    double multiplier = 1.0;
    OCStringRef error = NULL;
    SIUnitRef unit = SIUnitFromExpression(expression, &multiplier, &error);
    
    if (error) {
        printf("    ERROR: %s\n", OCStringGetCString(error));
        return false;
    }
    
    if (!unit) {
        printf("    ERROR: Returned NULL unit\n");
        return false;
    }
    
    // Check multiplier
    if (fabs(multiplier - expected_multiplier) > 1e-10) {
        printf("    ERROR: Expected multiplier %.10f, got %.10f\n", expected_multiplier, multiplier);
        return false;
    }
    
    // Check symbol
    OCStringRef symbol = SIUnitCopySymbol(unit);
    if (expected_symbol && OCStringCompare(symbol, expected_symbol, 0) != kOCCompareEqualTo) {
        printf("    ERROR: Expected symbol '%s', got '%s'\n", 
               OCStringGetCString(expected_symbol),
               OCStringGetCString(symbol));
        OCRelease(symbol);
        return false;
    }
    
    printf("    SUCCESS: multiplier=%.10f, symbol='%s'\n", 
           multiplier, OCStringGetCString(symbol));
    OCRelease(symbol);
    return true;
}

// Test that multiple calls with same expression return identical results
static bool test_expression_consistency(OCStringRef expression) {
    printf("  Testing consistency for '%s':\n", OCStringGetCString(expression));
    
    double mult1 = 1.0, mult2 = 1.0;
    OCStringRef error1 = NULL, error2 = NULL;
    
    SIUnitRef unit1 = SIUnitFromExpression(expression, &mult1, &error1);
    SIUnitRef unit2 = SIUnitFromExpression(expression, &mult2, &error2);
    
    if (error1 || error2) {
        printf("    ERROR: Got errors on parsing\n");
        return false;
    }
    
    if (!unit1 || !unit2) {
        printf("    ERROR: Got NULL units\n");
        return false;
    }
    
    // Should return same unit object
    if (unit1 != unit2) {
        printf("    ERROR: Different unit objects returned\n");
        return false;
    }
    
    // Should return same multiplier
    if (fabs(mult1 - mult2) > 1e-10) {
        printf("    ERROR: Different multipliers: %.10f vs %.10f\n", mult1, mult2);
        return false;
    }
    
    printf("    SUCCESS: Consistent results\n");
    return true;
}

bool test_unit_from_expression_robust(void) {
    printf("Running comprehensive SIUnitFromExpression tests...\n");
    
    bool allPassed = true;
    
    // Test 1: Basic SI units should always return multiplier 1.0
    printf("\n1. Testing basic SI units:\n");
    allPassed &= verify_unit_behavior(STR("m"), 1.0, STR("m"), "meter");
    allPassed &= verify_unit_behavior(STR("kg"), 1.0, STR("kg"), "kilogram");
    allPassed &= verify_unit_behavior(STR("s"), 1.0, STR("s"), "second");
    allPassed &= verify_unit_behavior(STR("A"), 1.0, STR("A"), "ampere");
    allPassed &= verify_unit_behavior(STR("K"), 1.0, STR("K"), "kelvin");
    allPassed &= verify_unit_behavior(STR("mol"), 1.0, STR("mol"), "mole");
    allPassed &= verify_unit_behavior(STR("cd"), 1.0, STR("cd"), "candela");
    
    // Test 2: Derived SI units should return multiplier 1.0
    printf("\n2. Testing derived SI units:\n");
    allPassed &= verify_unit_behavior(STR("N"), 1.0, STR("N"), "newton");
    allPassed &= verify_unit_behavior(STR("Pa"), 1.0, STR("Pa"), "pascal");
    allPassed &= verify_unit_behavior(STR("J"), 1.0, STR("J"), "joule");
    allPassed &= verify_unit_behavior(STR("W"), 1.0, STR("W"), "watt");
    allPassed &= verify_unit_behavior(STR("Hz"), 1.0, STR("Hz"), "hertz");
    
    // Test 3: Prefixed units should return multiplier 1.0
    printf("\n3. Testing prefixed units:\n");
    allPassed &= verify_unit_behavior(STR("km"), 1.0, STR("km"), "kilometer");
    allPassed &= verify_unit_behavior(STR("mm"), 1.0, STR("mm"), "millimeter");
    allPassed &= verify_unit_behavior(STR("kN"), 1.0, STR("kN"), "kilonewton");
    allPassed &= verify_unit_behavior(STR("mA"), 1.0, STR("mA"), "milliampere");
    
    // Test 4: Compound expressions that should be in library
    printf("\n4. Testing compound expressions:\n");
    allPassed &= verify_unit_behavior(STR("m/s"), 1.0, STR("m/s"), "velocity");
    allPassed &= verify_unit_behavior(STR("m/s^2"), 1.0, STR("m/s^2"), "acceleration");
    allPassed &= verify_unit_behavior(STR("kg*m/s^2"), 1.0, STR("m•kg/s^2"), "force compound");
    allPassed &= verify_unit_behavior(STR("kg*m^2/s^2"), 1.0, STR("m^2•kg/s^2"), "energy compound");
    
    // Test 5: Expression consistency (multiple calls should return same results)
    printf("\n5. Testing expression consistency:\n");
    allPassed &= test_expression_consistency(STR("m"));
    allPassed &= test_expression_consistency(STR("N"));
    allPassed &= test_expression_consistency(STR("kg*m/s^2"));
    allPassed &= test_expression_consistency(STR("m/s"));
    
    // Test 6: Non-SI units that should be in library
    printf("\n6. Testing non-SI units:\n");
    allPassed &= verify_unit_behavior(STR("in"), 1.0, STR("in"), "inch");
    allPassed &= verify_unit_behavior(STR("ft"), 1.0, STR("ft"), "foot");
    allPassed &= verify_unit_behavior(STR("lb"), 1.0, STR("lb"), "pound mass");
    allPassed &= verify_unit_behavior(STR("lbf"), 1.0, STR("lbf"), "pound force");
    
    // Test 7: Expression variants should return same unit
    printf("\n7. Testing expression variants:\n");
    double mult1 = 1.0, mult2 = 1.0;
    OCStringRef error1 = NULL, error2 = NULL;
    
    SIUnitRef unit1 = SIUnitFromExpression(STR("kg*m/s^2"), &mult1, &error1);
    SIUnitRef unit2 = SIUnitFromExpression(STR("m*kg/s^2"), &mult2, &error2);
    
    if (!unit1 || !unit2) {
        printf("    ERROR: Failed to parse variants\n");
        allPassed = false;
    } else {
        // Should be equivalent units (may or may not be same object)
        if (!SIUnitAreEquivalentUnits(unit1, unit2)) {
            printf("    ERROR: Variants are not equivalent\n");
            allPassed = false;
        }
        
        if (fabs(mult1 - mult2) > 1e-10) {
            printf("    ERROR: Different multipliers for variants: %.10f vs %.10f\n", mult1, mult2);
            allPassed = false;
        } else {
            printf("    SUCCESS: Variants are equivalent\n");
        }
    }
    
    // Test 8: Test that library lookup happens before parsing
    printf("\n8. Testing library lookup priority:\n");
    
    // First parse an expression to ensure it's in library
    double tempMult = 1.0;
    OCStringRef tempError = NULL;
    SIUnitRef tempUnit = SIUnitFromExpression(STR("in*lb"), &tempMult, &tempError);
    
    if (tempUnit) {
        // Now parse it again - should get same result without parsing
        double mult3 = 1.0;
        OCStringRef error3 = NULL;
        SIUnitRef unit3 = SIUnitFromExpression(STR("in*lb"), &mult3, &error3);
        
        if (unit3 == tempUnit && fabs(mult3 - 1.0) < 1e-10) {
            printf("    SUCCESS: Library lookup working correctly\n");
        } else {
            printf("    ERROR: Library lookup not working as expected\n");
            allPassed = false;
        }
    }
    
    // Test 9: Verify canonical form doesn't break existing behavior
    printf("\n9. Testing canonical form compatibility:\n");
    
    // These should all resolve to the same canonical form
    const char* equivalent_expressions[] = {
        "lb*ft^2/s^2",
        "ft^2*lb/s^2", 
        "(ft^2*lb)/s^2",
        "ft*ft*lb/s/s",
        "lb*ft*ft/(s*s)"
    };
    
    SIUnitRef first_unit = NULL;
    double first_mult = 1.0;
    OCStringRef first_error = NULL;
    
    for (int i = 0; i < 5; i++) {
        double mult = 1.0;
        OCStringRef error = NULL;
        OCStringRef expr = OCStringCreateWithCString(equivalent_expressions[i]);
        SIUnitRef unit = SIUnitFromExpression(expr, &mult, &error);
        
        if (error || !unit) {
            printf("    ERROR: Failed to parse '%s'\n", equivalent_expressions[i]);
            OCRelease(expr);
            allPassed = false;
            continue;
        }
        
        if (first_unit == NULL) {
            first_unit = unit;
            first_mult = mult;
        } else {
            // Should return same unit object due to canonical form
            if (unit != first_unit) {
                printf("    ERROR: Different units for equivalent expressions\n");
                allPassed = false;
            }
            
            if (fabs(mult - first_mult) > 1e-10) {
                printf("    ERROR: Different multipliers for equivalent expressions\n");
                allPassed = false;
            }
        }
        
        OCRelease(expr);
    }
    
    if (allPassed) {
        printf("    SUCCESS: Canonical form working correctly\n");
    }
    
    printf("\nSIUnitFromExpression robust test %s\n", allPassed ? "PASSED" : "FAILED");
    return allPassed;
}
