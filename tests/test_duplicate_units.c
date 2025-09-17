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
    // Initialize the units library
    OCMutableDictionaryRef unitsLib = SIUnitGetUnitsDictionaryLib();
    if (!unitsLib) {
        printf("ERROR: Could not initialize units library\n");
        return false;
    }
    // Get all keys and values from the dictionary
    uint64_t count = OCDictionaryGetCount(unitsLib);
    OCStringRef *keys = malloc(count * sizeof(OCStringRef));
    SIUnitRef *units = malloc(count * sizeof(SIUnitRef));
    if (!keys || !units) {
        printf("ERROR: Memory allocation failed\n");
        if (keys) free(keys);
        if (units) free(units);
        return false;
    }
    OCDictionaryGetKeysAndValues(unitsLib, (const void **)keys, (const void **)units);
    // Track duplicate pointers and static instance violations
    bool duplicatesFound = false;
    bool staticInstanceViolations = false;
    int duplicateCount = 0;
    int nonStaticCount = 0;
    // First pass: Check that all units are static instances
    for (uint64_t i = 0; i < count; i++) {
        if (!OCTypeGetStaticInstance(units[i])) {
            staticInstanceViolations = true;
            nonStaticCount++;
        }
    }
    // Second pass: Check for duplicate unit pointers
    for (uint64_t i = 0; i < count; i++) {
        for (uint64_t j = i + 1; j < count; j++) {
            if (units[i] == units[j]) {
                duplicatesFound = true;
                duplicateCount++;
            }
        }
    }
    free(keys);
    free(units);
    // Report final results
    if (duplicatesFound || staticInstanceViolations) {
        return false;
    } else {
        return true;
    }
}
// Test function to create some expressions that might cause duplicates
bool test_create_potential_duplicates(void) {
    // Test equivalent expressions
    const char *expressions[] = {
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
