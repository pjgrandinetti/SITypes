#include "../src/SILibrary.h" // Updated include path to resolve missing header issue
#include <assert.h>
#include <stdio.h>

void test_scalar_1(void) {
    printf("Running %s...\n", __func__);
    OCStringRef errorString = NULL;

    SIScalarRef scalar = SIScalarCreateWithOCString(STR("4.3 eV"), &errorString);
    if (!scalar) {
        if (errorString) OCRelease(errorString);
        assert(0 && "Failed to create scalar with eV");
    }

    SIUnitRef evUnit = SIUnitForUnderivedSymbol(STR("eV"));

    SIScalarRef temp = SIScalarCreateWithFloatComplex(4.3, evUnit);
    assert(temp != NULL);

    assert(SIScalarCompare(scalar, temp) == kOCCompareEqualTo);
    printf("%s passed\n", __func__);
    OCRelease(temp);
    OCRelease(scalar);
    if (errorString) OCRelease(errorString);
}

void test_scalar_2(void) {
    printf("Running %s...\n", __func__);
    SIScalarRef force = SIScalarCreateWithOCString(STR("500 N"), NULL);
    assert(force != NULL);

    SIScalarRef acceleration = SIScalarCreateWithOCString(STR("9.8 m/s^2"), NULL);
    assert(acceleration != NULL);

    SIScalarRef mass = SIScalarCreateByDividing(force, acceleration, NULL);
    assert(mass != NULL);

    SIScalarRef mass2 = SIScalarCreateWithOCString(STR("500 N * 9.8 m/s^2"), NULL);
    assert(mass2 != NULL);

    assert(SIScalarCompareReduced(mass, mass2));
    printf("%s passed\n", __func__);

    OCRelease(force);
    OCRelease(acceleration);
    OCRelease(mass);
    OCRelease(mass2);
}

void test_scalar_3(void) {
    printf("Running %s...\n", __func__);
    SIScalarRef result = SIScalarCreateWithOCString(STR("2^3"), NULL);
    SIScalarRef expected = SIScalarCreateWithOCString(STR("8"), NULL);
    assert(result != NULL && expected != NULL);
    assert(SIScalarCompare(result, expected) == kOCCompareEqualTo);
    printf("%s passed\n", __func__);
    OCRelease(result);
    OCRelease(expected);
}
