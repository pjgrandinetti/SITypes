#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "SIDimensionality.h"
#include "SILibrary.h"
#include "SIUnit2.h"

bool test_siunit2_power_basic(void) {
    printf("=== Testing SIUnit2 Power Operations ===\n");
    
    OCStringRef error = NULL;
    bool all_passed = true;
    
    // Test 1: Angular momentum unit (g•m^2/s)^4
    printf("Test 1: (g•m^2/s)^4\n");
    SIUnit2Ref unit1 = SIUnit2WithSymbol(STR("g•m^2/s"));
    if (!unit1) {
        printf("  ✗ Failed to create SIUnit2 from symbol g•m^2/s\n");
        all_passed = false;
    } else {
        printf("  Input unit: %s\n", OCStringGetCString(SIUnit2GetSymbol(unit1)));
        double power_mult = 1.0;
        SIUnit2Ref result = SIUnit2ByRaisingToPowerWithoutReducing(unit1, 4, &power_mult, &error);
        if (!result) {
            printf("  ✗ Failed to raise to 4th power\n");
            if (error) printf("    Error: %s\n", OCStringGetCString(error));
            all_passed = false;
        } else {
            printf("  Result: %s (multiplier: %.15g)\n", OCStringGetCString(SIUnit2GetSymbol(result)), power_mult);
            
            // Verify that the result has the expected symbol
            OCStringRef resultSymbol = SIUnit2GetSymbol(result);
            if (OCStringEqual(resultSymbol, STR("g^4•m^8/s^4"))) {
                printf("  ✓ Result has expected symbol g^4•m^8/s^4\n");
            } else {
                printf("  ✗ Result symbol doesn't match expected\n");
                printf("    Expected: g^4•m^8/s^4\n");
                printf("    Got:      %s\n", OCStringGetCString(resultSymbol));
                all_passed = false;
            }
            OCRelease(result);
        }
    }
    
    // Test 2: Energy unit (J•s)^3 
    printf("\nTest 2: (J•s)^3\n");
    SIUnit2Ref unit2 = SIUnit2WithSymbol(STR("J•s"));
    if (!unit2) {
        printf("  ✗ Failed to create SIUnit2 from symbol J•s\n");
        all_passed = false;
    } else {
        printf("  Input unit: %s\n", OCStringGetCString(SIUnit2GetSymbol(unit2)));
        double power_mult = 1.0;
        SIUnit2Ref result = SIUnit2ByRaisingToPowerWithoutReducing(unit2, 3, &power_mult, &error);
        if (!result) {
            printf("  ✗ Failed to raise to 3rd power\n");
            if (error) printf("    Error: %s\n", OCStringGetCString(error));
            all_passed = false;
        } else {
            printf("  Result: %s (multiplier: %.15g)\n", OCStringGetCString(SIUnit2GetSymbol(result)), power_mult);
            
            // Verify that the result has the expected symbol
            OCStringRef resultSymbol = SIUnit2GetSymbol(result);
            if (OCStringEqual(resultSymbol, STR("J^3•s^3"))) {
                printf("  ✓ Result has expected symbol J^3•s^3\n");
            } else {
                printf("  ✗ Result symbol doesn't match expected\n");
                printf("    Expected: J^3•s^3\n");
                printf("    Got:      %s\n", OCStringGetCString(resultSymbol));
                all_passed = false;
            }
            OCRelease(result);
        }
    }
    
    return all_passed;
}

bool test_siunit2_multiplication(void) {
    printf("=== Testing SIUnit2 Multiplication Operations ===\n");
    
    OCStringRef error = NULL;
    bool all_passed = true;
    
    // Test 1: m * s (create expected result by multiplication)
    printf("Test 1: m * s\n");
    SIUnit2Ref unit_m = SIUnit2WithSymbol(STR("m"));
    SIUnit2Ref unit_s = SIUnit2WithSymbol(STR("s"));
    if (!unit_m || !unit_s) {
        printf("  ✗ Failed to create units m, s\n");
        all_passed = false;
    } else {
        double mult = 1.0;
        SIUnit2Ref result = SIUnit2ByMultiplyingWithoutReducing(unit_m, unit_s, &mult, &error);
        if (!result) {
            printf("  ✗ Multiplication failed\n");
            if (error) printf("    Error: %s\n", OCStringGetCString(error));
            all_passed = false;
        } else {
            printf("  Result: %s (multiplier: %.15g)\n", OCStringGetCString(SIUnit2GetSymbol(result)), mult);
            
            // Verify that the result has the expected symbol
            OCStringRef resultSymbol = SIUnit2GetSymbol(result);
            if (OCStringEqual(resultSymbol, STR("m•s"))) {
                printf("  ✓ Result has expected symbol m•s\n");
            } else {
                printf("  ✗ Result symbol doesn't match expected\n");
                printf("    Expected: m•s\n");
                printf("    Got:      %s\n", OCStringGetCString(resultSymbol));
                all_passed = false;
            }
            OCRelease(result);
        }
    }
    
    // Test 2: N * m
    printf("\nTest 2: N * m\n");
    SIUnit2Ref unit_N = SIUnit2WithSymbol(STR("N"));
    SIUnit2Ref unit_m2 = SIUnit2WithSymbol(STR("m"));
    if (!unit_N || !unit_m2) {
        printf("  ✗ Failed to create units N, m\n");
        all_passed = false;
    } else {
        double mult = 1.0;
        SIUnit2Ref result = SIUnit2ByMultiplying(unit_N, unit_m2, &mult, &error);
        if (!result) {
            printf("  ✗ Multiplication failed\n");
            if (error) printf("    Error: %s\n", OCStringGetCString(error));
            all_passed = false;
        } else {
            printf("  Result: %s (multiplier: %.15g)\n", OCStringGetCString(SIUnit2GetSymbol(result)), mult);
            
            // Verify that the result has the expected symbol (should reduce to J)
            OCStringRef resultSymbol = SIUnit2GetSymbol(result);
            if (OCStringEqual(resultSymbol, STR("J"))) {
                printf("  ✓ Result correctly reduced to J\n");
            } else {
                printf("  ✗ Result doesn't match expected reduced form\n");
                printf("    Expected: J\n");
                printf("    Got:      %s\n", OCStringGetCString(resultSymbol));
                all_passed = false;
            }
            OCRelease(result);
        }
    }
    
    return all_passed;
}

bool test_siunit2_division(void) {
    printf("=== Testing SIUnit2 Division Operations ===\n");
    
    OCStringRef error = NULL;
    bool all_passed = true;
    
    // Test 1: m / s
    printf("Test 1: m / s\n");
    SIUnit2Ref unit_m = SIUnit2WithSymbol(STR("m"));
    SIUnit2Ref unit_s = SIUnit2WithSymbol(STR("s"));
    if (!unit_m || !unit_s) {
        printf("  ✗ Failed to create units m, s\n");
        all_passed = false;
    } else {
        double mult = 1.0;
        SIUnit2Ref result = SIUnit2ByDividingWithoutReducing(unit_m, unit_s, &mult, &error);
        if (!result) {
            printf("  ✗ Division failed\n");
            if (error) printf("    Error: %s\n", OCStringGetCString(error));
            all_passed = false;
        } else {
            printf("  Result: %s (multiplier: %.15g)\n", OCStringGetCString(SIUnit2GetSymbol(result)), mult);
            
            // Verify that the result has the expected symbol
            OCStringRef resultSymbol = SIUnit2GetSymbol(result);
            if (OCStringEqual(resultSymbol, STR("m/s"))) {
                printf("  ✓ Result has expected symbol m/s\n");
            } else {
                printf("  ✗ Result symbol doesn't match expected\n");
                printf("    Expected: m/s\n");
                printf("    Got:      %s\n", OCStringGetCString(resultSymbol));
                all_passed = false;
            }
            OCRelease(result);
        }
    }
    
    // Test 2: J / s
    printf("\nTest 2: J / s\n");
    SIUnit2Ref unit_J = SIUnit2WithSymbol(STR("J"));
    SIUnit2Ref unit_s2 = SIUnit2WithSymbol(STR("s"));
    if (!unit_J || !unit_s2) {
        printf("  ✗ Failed to create units J, s\n");
        all_passed = false;
    } else {
        double mult = 1.0;
        SIUnit2Ref result = SIUnit2ByDividing(unit_J, unit_s2, &mult, &error);
        if (!result) {
            printf("  ✗ Division failed\n");
            if (error) printf("    Error: %s\n", OCStringGetCString(error));
            all_passed = false;
        } else {
            printf("  Result: %s (multiplier: %.15g)\n", OCStringGetCString(SIUnit2GetSymbol(result)), mult);
            
            // Verify that the result has the expected symbol (should reduce to W)
            OCStringRef resultSymbol = SIUnit2GetSymbol(result);
            if (OCStringEqual(resultSymbol, STR("W"))) {
                printf("  ✓ Result correctly reduced to W\n");
            } else {
                printf("  ✗ Result doesn't match expected reduced form\n");
                printf("    Expected: W\n");
                printf("    Got:      %s\n", OCStringGetCString(resultSymbol));
                all_passed = false;
            }
            OCRelease(result);
        }
    }
    
    return all_passed;
}

bool test_siunit2_nth_root(void) {
    printf("=== Testing SIUnit2 Nth Root Operations ===\n");
    
    OCStringRef error = NULL;
    bool all_passed = true;
    
    // Test 1: sqrt(m^2)
    printf("Test 1: sqrt(m^2)\n");
    SIUnit2Ref unit_m2 = SIUnit2WithSymbol(STR("m^2"));
    if (!unit_m2) {
        printf("  ✗ Failed to create unit m^2\n");
        all_passed = false;
    } else {
        double mult = 1.0;
        SIUnit2Ref result = SIUnit2ByTakingNthRoot(unit_m2, 2, &mult, &error);
        if (!result) {
            printf("  ✗ Square root failed\n");
            if (error) printf("    Error: %s\n", OCStringGetCString(error));
            all_passed = false;
        } else {
            printf("  Result: %s (multiplier: %.15g)\n", OCStringGetCString(SIUnit2GetSymbol(result)), mult);
            
            // Verify that the result has the expected symbol
            OCStringRef resultSymbol = SIUnit2GetSymbol(result);
            if (OCStringEqual(resultSymbol, STR("m"))) {
                printf("  ✓ Square root result is correct: m\n");
            } else {
                printf("  ✗ Square root result doesn't match expected\n");
                printf("    Expected: m\n");
                printf("    Got:      %s\n", OCStringGetCString(resultSymbol));
                all_passed = false;
            }
            OCRelease(result);
        }
    }
    
    return all_passed;
}

bool test_siunit2_all_power_operations(void) {
    bool power_passed = test_siunit2_power_basic();
    bool mult_passed = test_siunit2_multiplication();
    bool div_passed = test_siunit2_division();
    bool root_passed = test_siunit2_nth_root();
    
    printf("\n=== Test Summary ===\n");
    printf("Power operations:               %s\n", power_passed ? "✓ PASSED" : "✗ FAILED");
    printf("Multiplication operations:      %s\n", mult_passed ? "✓ PASSED" : "✗ FAILED");
    printf("Division operations:            %s\n", div_passed ? "✓ PASSED" : "✗ FAILED");
    printf("Nth root operations:            %s\n", root_passed ? "✓ PASSED" : "✗ FAILED");
    
    bool overall = power_passed && mult_passed && div_passed && root_passed;
    printf("\nOverall Result: %s\n", overall ? "✓ ALL TESTS PASSED" : "✗ SOME TESTS FAILED");
    
    return overall;
}
