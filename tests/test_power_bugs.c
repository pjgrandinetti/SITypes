#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "SILibrary.h"
#include "test_power_bugs.h"

// Test that demonstrates known bugs in unit power operations
// This test is EXPECTED TO FAIL until the bugs are fixed
bool test_power_operation_bugs(void) {
    printf("Testing known power operation bugs (EXPECTED TO FAIL)...\n");
    
    bool bug_1_exists = false; // (m^2)^-1 bug
    bool bug_2_exists = false; // 1/m^2 parsing bug
    bool bug_3_exists = false; // m^-2 parsing bug
    
    // Bug 1: (m^2)^-1 should produce (1/m^2) but produces m^2
    printf("  Bug 1: Testing (m^2)^-1...\n");
    OCStringRef error = NULL;
    SIScalarRef area = SIScalarCreateFromExpression(STR("4 m^2"), &error);
    if (area) {
        SIScalarRef inv_area = SIScalarCreateByRaisingToPower(area, -1, &error);
        if (inv_area) {
            SIUnitRef result_unit = SIQuantityGetUnit((SIQuantityRef)inv_area);
            OCStringRef result_symbol = SIUnitCreateSymbol(result_unit);
            printf("    (m^2)^-1 = '%s' (should be '(1/m^2)')\n", OCStringGetCString(result_symbol));
            
            // The bug exists if we get 'm^2' instead of '(1/m^2)'
            bug_1_exists = (strcmp(OCStringGetCString(result_symbol), "m^2") == 0);
            
            OCRelease(result_symbol);
            OCRelease(inv_area);
        }
        OCRelease(area);
    }
    
    // Bug 2: 1/m^2 parsing should produce (1/m^2) but produces m^2  
    printf("  Bug 2: Testing '1/m^2' parsing...\n");
    double multiplier = 1.0;
    OCStringRef parse_error = NULL;
    SIUnitRef parsed_unit = SIUnitFromExpression(STR("1/m^2"), &multiplier, &parse_error);
    if (parsed_unit) {
        OCStringRef parsed_symbol = SIUnitCreateSymbol(parsed_unit);
        printf("    '1/m^2' = '%s' (should be '(1/m^2)')\n", OCStringGetCString(parsed_symbol));
        
        // The bug exists if we get 'm^2' instead of '(1/m^2)'
        bug_2_exists = (strcmp(OCStringGetCString(parsed_symbol), "m^2") == 0);
        
        OCRelease(parsed_symbol);
    }
    if (parse_error) {
        OCRelease(parse_error);
    }
    
    // Bug 3: m^-2 parsing should produce (1/m^2) but produces m^2
    printf("  Bug 3: Testing 'm^-2' parsing...\n");
    multiplier = 1.0;
    parse_error = NULL;
    parsed_unit = SIUnitFromExpression(STR("m^-2"), &multiplier, &parse_error);
    if (parsed_unit) {
        OCStringRef parsed_symbol = SIUnitCreateSymbol(parsed_unit);
        printf("    'm^-2' = '%s' (should be '(1/m^2)')\n", OCStringGetCString(parsed_symbol));
        
        // The bug exists if we get 'm^2' instead of '(1/m^2)'
        bug_3_exists = (strcmp(OCStringGetCString(parsed_symbol), "m^2") == 0);
        
        OCRelease(parsed_symbol);
    }
    if (parse_error) {
        OCRelease(parse_error);
    }
    
    printf("  Bug summary:\n");
    printf("    Bug 1 ((m^2)^-1): %s\n", bug_1_exists ? "EXISTS" : "FIXED");
    printf("    Bug 2 (1/m^2 parsing): %s\n", bug_2_exists ? "EXISTS" : "FIXED");
    printf("    Bug 3 (m^-2 parsing): %s\n", bug_3_exists ? "EXISTS" : "FIXED");
    
    // This test FAILS if bugs exist (to draw attention to the bugs)
    // It PASSES if bugs are fixed 
    bool bugs_exist = bug_1_exists || bug_2_exists || bug_3_exists;
    
    if (bugs_exist) {
        printf("  Status: BUGS DETECTED - test failing to draw attention\n");
        return false; // FAIL because bugs exist and need fixing
    } else {
        printf("  Status: All bugs appear to be fixed!\n");
        return true; // PASS because bugs are fixed
    }
}
