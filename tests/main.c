//
//  main.c
//  SILibTests
//
//  Created by Philip Grandinetti on 6/13/17.
//  Copyright © 2017 PhySy Ltd. All rights reserved.
//

#ifndef PRINTERROR
#define PRINTERROR printf("failure: line %d, %s\n",__LINE__,__FUNCTION__)
#endif

#include "../src/SILibrary.h" // Updated include path to resolve missing header issue
#include "octypes_tests.c"
#include "dimensionality_tests.c"
#include "unit_tests.c"
#include "scalar_tests.c"

int main(int argc, const char * argv[]) {

    octypesTest1();
    octypesTest2();
    octypesTest3();
    octypesTest4();
    octypesTest5();
    octypesTest6();

    dimensionalityTest0();
    dimensionalityTest1();
    dimensionalityTest2();

    unitTest0();
    unitTest1();
    unitTest3();

    scalarTest1();
    scalarTest2();

}
