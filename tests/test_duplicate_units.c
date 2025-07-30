//
//  test_duplicate_units.c
//  SITypes
//
//  Test to detect duplicate unit pointers with different keys in unitsLibrary
//

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../src/SILibrary.h"

extern OCMutableDictionaryRef SIUnitGetUnitsLib(void);

// Test function to check for duplicate unit pointers in the library
bool test_check_for_duplicate_units(void) {
    printf("=== Testing for duplicate unit pointers in unitsLibrary ===\n");
    
    // Initialize the units library
    OCMutableDictionaryRef unitsLib = SIUnitGetUnitsLib();
    if (!unitsLib) {
        printf("ERROR: Could not initialize units library\n");
        return false;
    }
    
    // Get all keys and values from the dictionary
    uint64_t count = OCDictionaryGetCount(unitsLib);
    printf("Total units in library: %llu\n", count);
    
    OCStringRef* keys = malloc(count * sizeof(OCStringRef));
    SIUnitRef* units = malloc(count * sizeof(SIUnitRef));
    
    if (!keys || !units) {
        printf("ERROR: Memory allocation failed\n");
        if (keys) free(keys);
        if (units) free(units);
        return false;
    }
    
    OCDictionaryGetKeysAndValues(unitsLib, (const void**)keys, (const void**)units);
    
    // Track duplicate pointers
    bool duplicatesFound = false;
    int duplicateCount = 0;
    
    // Check for duplicate unit pointers
    for (uint64_t i = 0; i < count; i++) {
        for (uint64_t j = i + 1; j < count; j++) {
            if (units[i] == units[j]) {
                duplicatesFound = true;
                duplicateCount++;
                
                printf("\n*** DUPLICATE FOUND ***\n");
                printf("Unit pointer: %p appears with different keys:\n", units[i]);
                printf("  Key 1: '%s'\n", OCStringGetCString(keys[i]));
                printf("  Key 2: '%s'\n", OCStringGetCString(keys[j]));
                
                // Get the unit's symbol for comparison
                OCStringRef symbol = SIUnitCopySymbol(units[i]);
                if (symbol) {
                    printf("  Unit symbol: '%s'\n", OCStringGetCString(symbol));
                    OCRelease(symbol);
                }
                                
                // Check dimensionality
                SIDimensionalityRef dim = SIUnitGetDimensionality(units[i]);
                if (dim) {
                    OCStringRef dimSymbol = SIDimensionalityGetSymbol(dim);
                    if (dimSymbol) {
                        printf("  Dimensionality: '%s'\n", OCStringGetCString(dimSymbol));
                    }
                }
                
                // Check scaling
                double scale = SIUnitScaleToCoherentSIUnit(units[i]);
                printf("  Scale to coherent SI: %g\n", scale);
                
                printf("*** END DUPLICATE ***\n");
            }
        }
    }
    
    free(keys);
    free(units);
    
    if (duplicatesFound) {
        printf("\n!!! FOUND %d DUPLICATE UNIT POINTERS !!!\n", duplicateCount);
        printf("This explains the bus error during cleanup.\n");
        return false;
    } else {
        printf("\nNo duplicate unit pointers found.\n");
        return true;
    }
}

// Test function to create some expressions that might cause duplicates
bool test_create_potential_duplicates(void) {
    printf("\n=== Testing expressions that might create duplicates ===\n");
    
    // Test equivalent expressions
    const char* expressions[] = {
        "ft*ft*lb/s/s",
        "lb*ft^2/s^2",
        "ft^2*lb/s^2",
        "(ft^2*lb)/s^2",
        "lb*ft*ft/(s*s)",
        "kg*m/s^2",
        "m*kg/s^2",
        "kg*m/(s*s)",
        "m*kg/(s^2)",
        "m*m",
        "m^2",
        NULL
    };
    
    printf("Creating units from equivalent expressions...\n");
    
    for (int i = 0; expressions[i] != NULL; i++) {
        printf("  Testing: '%s'\n", expressions[i]);
        double multiplier = 0.0;
        OCStringRef error = NULL;
        
        OCStringRef expression = OCStringCreateWithCString(expressions[i]);
        SIUnitRef unit = SIUnitFromExpression(expression, &multiplier, &error);
        if (unit) {
            OCStringRef symbol = SIUnitCopySymbol(unit);
            if (symbol) {
                printf("    -> Unit: %p, Symbol: '%s', Multiplier: %g\n", 
                       unit, OCStringGetCString(symbol), multiplier);
                OCRelease(symbol);
            }
        } else {
            printf("    -> FAILED");
            if (error) {
                printf(" (Error: %s)", OCStringGetCString(error));
            }
            printf("\n");
        }
        OCRelease(expression);
    }
    
    return true;
}
