#include <stdio.h>
#include <stdbool.h>
#include "tests/test_siunit2_power.h"

int main() {
    printf("=== SIUnit2 Power Test Only ===\n");
    bool result = test_siunit2_all_power_operations();
    printf("\n=== Final Result ===\n");
    printf("Test %s\n", result ? "PASSED" : "FAILED");
    return result ? 0 : 1;
}
