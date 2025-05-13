#include "../src/SILibrary.h" // Updated include path to resolve missing header issue
#include <stdio.h>

#ifndef PRINTERROR
#define PRINTERROR printf("failure: line %d, %s\n", __LINE__, __FUNCTION__)
#endif

bool scalarTest1(void) {
    fprintf(stderr, "%s begin...\n", __func__);
    OCStringRef errorString = NULL;

    SIScalarRef scalar = SIScalarCreateWithOCString(STR("4.3 eV"), &errorString);
    if (!scalar) return false;
    SIScalarRef temp = SIScalarCreateWithFloatComplex(4.3, SIUnitForUnderivedSymbol(STR("eV")));
    if (SIScalarCompare(scalar, temp) != kOCCompareEqualTo) PRINTERROR;
    if (scalar) OCRelease(scalar);

    fprintf(stderr, "%s end...without problems\n", __func__);
    return true;
}

bool scalarTest2(void) {
    fprintf(stderr, "%s begin...\n", __func__);

    SIScalarRef force = SIScalarCreateWithOCString(STR("500 N"), NULL);
    SIScalarRef acceleration = SIScalarCreateWithOCString(STR("9.8 m/s^2"), NULL);
    SIScalarRef mass = SIScalarCreateByDividing(force, acceleration, NULL);
    SIScalarRef mass2 = SIScalarCreateWithOCString(STR("500 N * 9.8 m/s^2"), NULL);
    if (!SIScalarCompareReduced(mass, mass2)) PRINTERROR;

    fprintf(stderr, "%s end...without problems\n", __func__);
    return true;
}
