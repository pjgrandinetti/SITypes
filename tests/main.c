//
//  main.c
//  SILibTests
//
//  Created by Philip Grandinetti on 6/13/17.
//  Copyright © 2017 PhySy Ltd. All rights reserved.
//

#define PRINTERROR printf("failure: line %d, %s\n",__LINE__,__FUNCTION__)

#include "SILibrary.h"
#include "dimensionality_tests.c"
#include "unit_tests.c"
#include "scalar_tests.c"

int main(int argc, const char * argv[]) {
    OCLibraryInitialize();

    dimensionalityTest0();
    dimensionalityTest1();
    dimensionalityTest2();

    unitTest0();
    unitTest1();
    unitTest3();

    scalarTest1();
    scalarTest2();
}
