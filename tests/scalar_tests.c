#include "../src/SILibrary.h" // Updated include path to resolve missing header issue
#include <stdio.h>

#ifndef PRINTERROR
#define PRINTERROR printf("failure: line %d, %s\n", __LINE__, __FUNCTION__)
#endif

bool scalarTest1(void) {
    fprintf(stderr, "%s begin…\n", __func__);
    OCStringRef errorString = NULL;

    // Debug instrumentation
    SIScalarRef scalar = SIScalarCreateWithOCString(STR("4.3 eV"), &errorString);

    if (!scalar) {
        if (errorString) OCRelease(errorString);
        return false;
    }

    // Check unit creation
    SIUnitRef evUnit = SIUnitForUnderivedSymbol(STR("eV"));

    // Test float complex creation
    SIScalarRef temp = SIScalarCreateWithFloatComplex(
        4.3,
        evUnit
    );
    if (!temp) {
        OCRelease(scalar);
        if (errorString) OCRelease(errorString);
        return false;
    }

    if (SIScalarCompare(scalar, temp) != kOCCompareEqualTo) {
        PRINTERROR;
    }

    OCRelease(temp);
    OCRelease(scalar);
    if (errorString) OCRelease(errorString);

    fprintf(stderr, "%s end…without problems\n", __func__);
    return true;
}

bool scalarTest2(void) {
    fprintf(stderr, "%s begin...\n", __func__);

    SIScalarRef force = SIScalarCreateWithOCString(STR("500 N"), NULL);
    if (!force) {
        PRINTERROR;
        return false;
    }
    SIScalarRef acceleration = SIScalarCreateWithOCString(STR("9.8 m/s^2"), NULL);
    if (!acceleration) {
        OCRelease(force);
        PRINTERROR;
        return false;
    }
    SIScalarRef mass = SIScalarCreateByDividing(force, acceleration, NULL);
    if (!mass) {
        OCRelease(force);
        OCRelease(acceleration);
        PRINTERROR;
        return false;
    }
    SIScalarRef mass2 = SIScalarCreateWithOCString(STR("500 N * 9.8 m/s^2"), NULL);
    if (!mass2) {
        OCRelease(force);
        OCRelease(acceleration);
        OCRelease(mass);
        PRINTERROR;
        return false;
    }
    if (!SIScalarCompareReduced(mass, mass2)) PRINTERROR;

    fprintf(stderr, "%s end...without problems\n", __func__);
    return true;
}

bool scalarTest3(void) {
    fprintf(stderr, "%s begin...\n", __func__);
    SIScalarRef result = SIScalarCreateWithOCString(STR("2^3"), NULL);
    SIScalarRef expected = SIScalarCreateWithOCString(STR("8"), NULL);
    if (!result || !expected) PRINTERROR;
    if (SIScalarCompare(result, expected) != kOCCompareEqualTo) PRINTERROR;
    OCRelease(result);
    OCRelease(expected);
    fprintf(stderr, "%s end...without problems\n", __func__);
    return true;
}
