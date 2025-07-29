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
            OCRelease(result);
        }
    }
    
    // Test 3: Linear momentum unit (g•m/s)^2
    printf("\nTest 3: (g•m/s)^2\n");
    SIUnit2Ref unit3 = SIUnit2WithSymbol(STR("g•m/s"));
    if (!unit3) {
        printf("  ✗ Failed to create SIUnit2 from symbol g•m/s\n");
        all_passed = false;
    } else {
        printf("  Input unit: %s\n", OCStringGetCString(SIUnit2GetSymbol(unit3)));
        double power_mult = 1.0;
        SIUnit2Ref result = SIUnit2ByRaisingToPowerWithoutReducing(unit3, 2, &power_mult, &error);
        if (!result) {
            printf("  ✗ Failed to raise to 2nd power\n");
            if (error) printf("    Error: %s\n", OCStringGetCString(error));
            all_passed = false;
        } else {
            printf("  Result: %s (multiplier: %.15g)\n", OCStringGetCString(SIUnit2GetSymbol(result)), power_mult);
            OCRelease(result);
        }
    }
    
    // Test 4: Simple area unit (m^2)^3
    printf("\nTest 4: (m^2)^3\n");
    SIUnit2Ref unit4 = SIUnit2WithSymbol(STR("m^2"));
    if (!unit4) {
        printf("  ✗ Failed to create SIUnit2 from symbol m^2\n");
        all_passed = false;
    } else {
        printf("  Input unit: %s\n", OCStringGetCString(SIUnit2GetSymbol(unit4)));
        double power_mult = 1.0;
        SIUnit2Ref result = SIUnit2ByRaisingToPowerWithoutReducing(unit4, 3, &power_mult, &error);
        if (!result) {
            printf("  ✗ Failed to raise to 3rd power\n");
            if (error) printf("    Error: %s\n", OCStringGetCString(error));
            all_passed = false;
        } else {
            printf("  Result: %s (multiplier: %.15g)\n", OCStringGetCString(SIUnit2GetSymbol(result)), power_mult);
            OCRelease(result);
        }
    }
    
    // Test 5: Velocity unit (m/s)^4
    printf("\nTest 5: (m/s)^4\n");
    SIUnit2Ref unit5 = SIUnit2WithSymbol(STR("m/s"));
    if (!unit5) {
        printf("  ✗ Failed to create SIUnit2 from symbol m/s\n");
        all_passed = false;
    } else {
        printf("  Input unit: %s\n", OCStringGetCString(SIUnit2GetSymbol(unit5)));
        double power_mult = 1.0;
        SIUnit2Ref result = SIUnit2ByRaisingToPowerWithoutReducing(unit5, 4, &power_mult, &error);
        if (!result) {
            printf("  ✗ Failed to raise to 4th power\n");
            if (error) printf("    Error: %s\n", OCStringGetCString(error));
            all_passed = false;
        } else {
            printf("  Result: %s (multiplier: %.15g)\n", OCStringGetCString(SIUnit2GetSymbol(result)), power_mult);
            OCRelease(result);
        }
    }
    
    // Test 6: Prefixed unit (mg•m/s)^2
    printf("\nTest 6: (mg•m/s)^2\n");
    SIUnit2Ref unit6 = SIUnit2WithSymbol(STR("mg•m/s"));
    if (!unit6) {
        printf("  ✗ Failed to create SIUnit2 from symbol mg•m/s\n");
        all_passed = false;
    } else {
        printf("  Input unit: %s\n", OCStringGetCString(SIUnit2GetSymbol(unit6)));
        double power_mult = 1.0;
        SIUnit2Ref result = SIUnit2ByRaisingToPowerWithoutReducing(unit6, 2, &power_mult, &error);
        if (!result) {
            printf("  ✗ Failed to raise to 2nd power\n");
            if (error) printf("    Error: %s\n", OCStringGetCString(error));
            all_passed = false;
        } else {
            printf("  Result: %s (multiplier: %.15g)\n", OCStringGetCString(SIUnit2GetSymbol(result)), power_mult);
            OCRelease(result);
        }
    }
    
    // Test 7: Energy-time unit (kJ•s)^3
    printf("\nTest 7: (kJ•s)^3\n");
    SIUnit2Ref unit7 = SIUnit2WithSymbol(STR("kJ•s"));
    if (!unit7) {
        printf("  ✗ Failed to create SIUnit2 from symbol kJ•s\n");
        all_passed = false;
    } else {
        printf("  Input unit: %s\n", OCStringGetCString(SIUnit2GetSymbol(unit7)));
        double power_mult = 1.0;
        SIUnit2Ref result = SIUnit2ByRaisingToPowerWithoutReducing(unit7, 3, &power_mult, &error);
        if (!result) {
            printf("  ✗ Failed to raise to 3rd power\n");
            if (error) printf("    Error: %s\n", OCStringGetCString(error));
            all_passed = false;
        } else {
            printf("  Result: %s (multiplier: %.15g)\n", OCStringGetCString(SIUnit2GetSymbol(result)), power_mult);
            OCRelease(result);
        }
    }
    
    // Test 8: Force-length unit (mN•m)^2
    printf("\nTest 8: (mN•m)^2\n");
    SIUnit2Ref unit8 = SIUnit2WithSymbol(STR("mN•m"));
    if (!unit8) {
        printf("  ✗ Failed to create SIUnit2 from symbol mN•m\n");
        all_passed = false;
    } else {
        printf("  Input unit: %s\n", OCStringGetCString(SIUnit2GetSymbol(unit8)));
        double power_mult = 1.0;
        SIUnit2Ref result = SIUnit2ByRaisingToPowerWithoutReducing(unit8, 2, &power_mult, &error);
        if (!result) {
            printf("  ✗ Failed to raise to 2nd power\n");
            if (error) printf("    Error: %s\n", OCStringGetCString(error));
            all_passed = false;
        } else {
            printf("  Result: %s (multiplier: %.15g)\n", OCStringGetCString(SIUnit2GetSymbol(result)), power_mult);
            OCRelease(result);
        }
    }
    
    if (all_passed) {
        printf("\n  ✓ All parenthetical expansion tests completed successfully!\n");
    } else {
        printf("\n  ✗ Some tests failed\n");
    }
    
    return all_passed;
}

// Main test function
bool test_siunit2_all_power_operations(void) {
    bool test1 = test_siunit2_power_basic();
    
    printf("\n=== Test Summary ===\n");
    printf("Multiple unit power tests: %s\n", test1 ? "✓ PASSED" : "✗ FAILED");
    
    return test1;
}
