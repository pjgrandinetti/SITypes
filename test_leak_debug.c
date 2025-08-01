#include "src/SILibrary.h"
#include <stdio.h>

int main() {
    printf("Testing for memory leaks...\n");

    // Test the functions that showed leaks in AddressSanitizer
    OCStringRef result1 = SIUnitCreateCleanedExpression(STR("m^2"));
    OCRelease(result1);

    OCStringRef result2 = SIUnitCreateCleanedExpression(STR("m^(-1)"));
    OCRelease(result2);

    // Test SIUnitFromExpression
    double multiplier;
    OCStringRef error = NULL;
    SIUnitRef unit = SIUnitFromExpression(STR("m^-2"), &multiplier, &error);
    if (unit) OCRelease(unit);
    if (error) OCRelease(error);

    printf("Test complete.\n");
    return 0;
}
