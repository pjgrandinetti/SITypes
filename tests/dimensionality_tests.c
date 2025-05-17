#include "../src/SILibrary.h"
#include <assert.h>
#include <stdio.h>

void test_dimensionality_0(void) {
    printf("Running %s...\n", __func__);
    OCStringRef errorString = NULL;

    SIDimensionalityRef dimensionality1 = SIDimensionalityForSymbol(STR("L"), &errorString);

    OCStringRef symbol = SIDimensionalityGetSymbol(dimensionality1);
    assert(OCStringCompare(symbol, STR("L"), 0) == kOCCompareEqualTo);

    SIDimensionalityRef dimensionality2 = SIDimensionalityWithBaseDimensionSymbol(STR("L"), &errorString);
    assert(OCStringCompare(SIDimensionalityGetSymbol(dimensionality1), SIDimensionalityGetSymbol(dimensionality2), 0) == kOCCompareEqualTo);

    assert(SIDimensionalityGetNumeratorExponentAtIndex(dimensionality1, kSILengthIndex) == 1);
    assert(SIDimensionalityGetNumeratorExponentAtIndex(dimensionality1, kSIMassIndex) == 0);
    assert(SIDimensionalityGetNumeratorExponentAtIndex(dimensionality1, kSITimeIndex) == 0);
    assert(SIDimensionalityGetNumeratorExponentAtIndex(dimensionality1, kSICurrentIndex) == 0);
    assert(SIDimensionalityGetNumeratorExponentAtIndex(dimensionality1, kSITemperatureIndex) == 0);
    assert(SIDimensionalityGetNumeratorExponentAtIndex(dimensionality1, kSIAmountIndex) == 0);
    assert(SIDimensionalityGetNumeratorExponentAtIndex(dimensionality1, kSILuminousIntensityIndex) == 0);

    assert(SIDimensionalityGetDenominatorExponentAtIndex(dimensionality1, kSILengthIndex) == 0);
    assert(SIDimensionalityGetDenominatorExponentAtIndex(dimensionality1, kSIMassIndex) == 0);
    assert(SIDimensionalityGetDenominatorExponentAtIndex(dimensionality1, kSITimeIndex) == 0);
    assert(SIDimensionalityGetDenominatorExponentAtIndex(dimensionality1, kSICurrentIndex) == 0);
    assert(SIDimensionalityGetDenominatorExponentAtIndex(dimensionality1, kSITemperatureIndex) == 0);
    assert(SIDimensionalityGetDenominatorExponentAtIndex(dimensionality1, kSIAmountIndex) == 0);
    assert(SIDimensionalityGetDenominatorExponentAtIndex(dimensionality1, kSILuminousIntensityIndex) == 0);

    assert(SIDimensionalityReducedExponentAtIndex(dimensionality1, kSILengthIndex) == 1);
    assert(SIDimensionalityReducedExponentAtIndex(dimensionality1, kSIMassIndex) == 0);
    assert(SIDimensionalityReducedExponentAtIndex(dimensionality1, kSITimeIndex) == 0);
    assert(SIDimensionalityReducedExponentAtIndex(dimensionality1, kSICurrentIndex) == 0);
    assert(SIDimensionalityReducedExponentAtIndex(dimensionality1, kSITemperatureIndex) == 0);
    assert(SIDimensionalityReducedExponentAtIndex(dimensionality1, kSIAmountIndex) == 0);
    assert(SIDimensionalityReducedExponentAtIndex(dimensionality1, kSILuminousIntensityIndex) == 0);
    printf("%s passed\n", __func__);
}

void test_dimensionality_1(void) {
    printf("Running %s...\n", __func__);
    OCStringRef errorString = NULL;

    OCStringRef symbol = STR("L•M^2•T^3•I^4•ϴ^5•N^6•J^7/(L^2•M^3•T^4•I^5•ϴ^6•N^7•J^8)");
    SIDimensionalityRef dimensionality = SIDimensionalityForSymbol(symbol, &errorString);
    assert(dimensionality != NULL);
    assert(SIDimensionalityGetNumeratorExponentAtIndex(dimensionality, kSILengthIndex) == 1);
    assert(SIDimensionalityGetDenominatorExponentAtIndex(dimensionality, kSILengthIndex) == 2);
    assert(SIDimensionalityReducedExponentAtIndex(dimensionality, kSILengthIndex) == -1);
    printf("%s passed\n", __func__);
}

void test_dimensionality_2(void) {
    printf("Running %s...\n", __func__);
    OCStringRef errorString = NULL;

    SIDimensionalityRef force = SIDimensionalityForQuantity(kSIQuantityForce, &errorString);
    SIDimensionalityRef acceleration = SIDimensionalityForQuantity(kSIQuantityAcceleration, &errorString);
    SIDimensionalityRef dimensionality = SIDimensionalityByDividing(force, acceleration);
    SIDimensionalityRef mass = SIDimensionalityForQuantity(kSIQuantityMass, &errorString);

    assert(SIDimensionalityEqual(dimensionality, mass));
    printf("%s passed\n", __func__);
}

void test_dimensionality_3(void) {
    printf("Running %s...\n", __func__);
    OCStringRef error = NULL;
    // Test equality for commutative symbols
    SIDimensionalityRef dim1 = SIDimensionalityForSymbol(STR("L*M"), &error);
    SIDimensionalityRef dim2 = SIDimensionalityForSymbol(STR("M*L"), &error);
    assert(SIDimensionalityEqual(dim1, dim2));
    // Test reduced exponents for mixed powers
    SIDimensionalityRef dim3 = SIDimensionalityForSymbol(STR("L^3*M^-2*T"), &error);
    assert(SIDimensionalityHasReducedExponents(dim3, 3, -2, 1, 0, 0, 0, 0));
    // Test dimensionless
    SIDimensionalityRef dless = SIDimensionalityDimensionless();
    assert(SIDimensionalityIsDimensionless(dless) && !SIDimensionalityIsDerived(dless));
    // Test base dimension index and symbol consistency
    for (int i = kSILengthIndex; i <= kSILuminousIntensityIndex; i++) {
        SIDimensionalityRef baseDim = SIDimensionalityForBaseDimensionIndex((SIBaseDimensionIndex)i);
        assert(SIDimensionalityGetNumeratorExponentAtIndex(baseDim, i) == 1);
        for (int j = kSILengthIndex; j <= kSILuminousIntensityIndex; j++) {
            if (i != j) {
                assert(SIDimensionalityGetNumeratorExponentAtIndex(baseDim, (SIBaseDimensionIndex)j) == 0);
            }
        }
        OCRelease(baseDim);
    }
    // Test WithBaseDimensionSymbol
    SIDimensionalityRef bySym = SIDimensionalityWithBaseDimensionSymbol(STR("L"), &error);
    SIDimensionalityRef byIdx = SIDimensionalityForBaseDimensionIndex(kSILengthIndex);
    assert(SIDimensionalityEqual(bySym, byIdx));
    OCRelease(bySym);
    OCRelease(byIdx);
    OCRelease(dim1);
    OCRelease(dim2);
    OCRelease(dim3);
    OCRelease(dless);
    printf("%s passed\n", __func__);
}
