#include "SITypes.h"
#include <stdio.h>

int main() {
    printf("Testing quartertsp unit lookup...\n");
    
    // Check volume system setting
    SIVolumeSystem volumeSystem = SIUnitLibraryGetDefaultVolumeSystem();
    printf("Default volume system: %s\n", volumeSystem == kSIVolumeSystemUS ? "US" : "UK");
    
    // Test if quartertsp unit exists in library
    SIUnitRef unit = SIUnitWithSymbol(STR("quartertsp"));
    if (unit) {
        printf("✓ quartertsp unit found in library\n");
        OCStringRef symbol = SIUnitCopySymbol(unit);
        printf("  Symbol: %s\n", OCStringGetCString(symbol));
        OCRelease(symbol);
    } else {
        printf("✗ quartertsp unit NOT found in library\n");
    }
    
    // Test other similar units
    printf("\nTesting related units:\n");
    
    SIUnitRef tspUnit = SIUnitWithSymbol(STR("tsp"));
    if (tspUnit) {
        printf("✓ tsp found\n");
    } else {
        printf("✗ tsp NOT found\n");
    }
    
    SIUnitRef halftspUnit = SIUnitWithSymbol(STR("halftsp"));
    if (halftspUnit) {
        printf("✓ halftsp found\n");
    } else {
        printf("✗ halftsp NOT found\n");
    }
    
    SIUnitRef quartertspUSUnit = SIUnitWithSymbol(STR("quartertspUS"));
    if (quartertspUSUnit) {
        printf("✓ quartertspUS found\n");
    } else {
        printf("✗ quartertspUS NOT found\n");
    }
    
    SIUnitRef quartertspUKUnit = SIUnitWithSymbol(STR("quartertspUK"));
    if (quartertspUKUnit) {
        printf("✓ quartertspUK found\n");
    } else {
        printf("✗ quartertspUK NOT found\n");
    }
    
    // Test SIUnitCreateCleanedExpression with quartertsp
    printf("\nTesting SIUnitCreateCleanedExpression...\n");
    OCStringRef cleaned = SIUnitCreateCleanedExpression(STR("quartertsp"));
    if (cleaned) {
        printf("✓ SIUnitCreateCleanedExpression returned: %s\n", OCStringGetCString(cleaned));
        OCRelease(cleaned);
    } else {
        printf("✗ SIUnitCreateCleanedExpression returned NULL\n");
    }
    
    return 0;
}
