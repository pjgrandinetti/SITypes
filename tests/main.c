//
//  main.c
//  SILibTests
//
//  Created by Philip Grandinetti on 6/13/17.
//  Copyright © 2017 PhySy Ltd. All rights reserved.
//

// Using <assert.h> in tests; main just invokes test functions

#include <stdio.h>
#include "../src/SILibrary.h" // Updated include path to resolve missing header issue
#include "octypes_tests.c"
#include "dimensionality_tests.c"
#include "unit_tests.c"
#include "scalar_tests.c"

int main(int argc, const char * argv[]) {

    printf("=== OCTypes Tests ===\n");
    octypesTest1();
    octypesTest2();
    octypesTest3();
    octypesTest4();
    octypesTest5();
    octypesTest6();

    printf("\n=== Dimensionality Tests ===\n");
    test_dimensionality_0();
    test_dimensionality_1();
    test_dimensionality_2();
    test_dimensionality_3();

    printf("\n=== SIUnit Tests ===\n");
    test_unit_0();
    test_unit_1();
    test_unit_3();
    test_unit_4();
    test_unit_5();
    test_unit_6();

    printf("\n=== SIScalar Tests ===\n");
    test_scalar_1();
    test_scalar_2();
    test_scalar_3();

    // Print summary message if all tests pass
    printf("\nAll tests passed\n");
    return 0;

}
