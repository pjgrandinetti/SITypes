//
//  test_duplicate_units.c
//  SITypes
//
//  Test to detect duplicate unit pointers with different keys in unitsLibrary
//
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "../src/SITypes.h"
extern OCMutableDictionaryRef SIUnitGetUnitsDictionaryLib(void);
// Test function to check for duplicate unit pointers in the library
bool test_check_for_duplicate_units(void) {
    printf("=== Testing for duplicate unit pointers in unitsLibrary ===\n");
    // Initialize the units library
    OCMutableDictionaryRef unitsLib = SIUnitGetUnitsDictionaryLib();
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
    // Track duplicate pointers and static instance violations
    bool duplicatesFound = false;
    bool staticInstanceViolations = false;
    int duplicateCount = 0;
    int nonStaticCount = 0;
    // First pass: Check that all units are static instances
    printf("\n=== Checking static instance status ===\n");
    for (uint64_t i = 0; i < count; i++) {
        if (!OCTypeGetStaticInstance(units[i])) {
            staticInstanceViolations = true;
            nonStaticCount++;
            printf("*** NON-STATIC UNIT FOUND ***\n");
            printf("Unit pointer: %p (Key: '%s')\n", units[i], OCStringGetCString(keys[i]));
            // Get the unit's symbol for identification
            OCStringRef symbol = SIUnitCopySymbol(units[i]);
            if (symbol) {
                printf("  Unit symbol: '%s'\n", OCStringGetCString(symbol));
                OCRelease(symbol);
            }
            printf("*** This unit should be a static instance! ***\n\n");
        }
    }
    if (staticInstanceViolations) {
        printf("!!! FOUND %d NON-STATIC UNITS IN LIBRARY !!!\n", nonStaticCount);
        printf("All units in the library should be static instances.\n\n");
    } else {
        printf("✓ All %llu units are static instances.\n\n", count);
    }
    // Second pass: Check for duplicate unit pointers
    printf("=== Checking for duplicate unit pointers ===\n");
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
                    OCStringRef dimSymbol = SIDimensionalityCopySymbol(dim);
                    if (dimSymbol) {
                        printf("  Dimensionality: '%s'\n", OCStringGetCString(dimSymbol));
                        OCRelease(dimSymbol);
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
    // Report final results
    if (duplicatesFound || staticInstanceViolations) {
        printf("\n!!! LIBRARY INTEGRITY ISSUES FOUND !!!\n");
        if (duplicatesFound) {
            printf("- %d duplicate unit pointers\n", duplicateCount);
        }
        if (staticInstanceViolations) {
            printf("- %d non-static units in library\n", nonStaticCount);
        }
        printf("These issues may cause memory management problems.\n");
        return false;
    } else {
        printf("\n✓ Library integrity check passed:\n");
        printf("  - No duplicate unit pointers\n");
        printf("  - All units are static instances\n");
        return true;
    }
}
// Test function to create some expressions that might cause duplicates
bool test_create_potential_duplicates(void) {
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
        NULL};
    for (int i = 0; expressions[i] != NULL; i++) {
        double multiplier = 1.0;
        OCStringRef error = NULL;
        OCStringRef expression = OCStringCreateWithCString(expressions[i]);
        SIUnitRef unit = SIUnitFromExpression(expression, &multiplier, &error);
        if (!unit) {
            printf("  ✗ Failed to create unit from '%s'", expressions[i]);
            if (error) {
                printf(" (Error: %s)", OCStringGetCString(error));
            }
            printf("\n");
            OCRelease(expression);
            return false;
        }
        OCRelease(expression);
    }
    return true;
}
