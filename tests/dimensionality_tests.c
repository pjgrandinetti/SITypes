#include "SILibrary.h"
#include <stdio.h>

#define PRINTERROR printf("failure: line %d, %s\n", __LINE__, __FUNCTION__)

bool dimensionalityTest0(void) {

    OCStringRef errorString = NULL;
    fprintf(stderr, "%s begin...\n", __func__);

    SIDimensionalityRef dimensionality1 = SIDimensionalityForSymbol(STR("L"), &errorString);

    OCStringRef symbol = SIDimensionalityGetSymbol(dimensionality1);
    if (OCStringCompare(symbol, STR("L"), 0) != kOCCompareEqualTo) PRINTERROR;

    SIDimensionalityRef dimensionality2 = SIDimensionalityWithBaseDimensionSymbol(STR("L"), &errorString);
    if (OCStringCompare(SIDimensionalityGetSymbol(dimensionality1), SIDimensionalityGetSymbol(dimensionality2), 0) != kOCCompareEqualTo) PRINTERROR;

    if (SIDimensionalityGetNumeratorExponentAtIndex(dimensionality1, kSILengthIndex) != 1) PRINTERROR;
    if (SIDimensionalityGetNumeratorExponentAtIndex(dimensionality1, kSIMassIndex) != 0) PRINTERROR;
    if (SIDimensionalityGetNumeratorExponentAtIndex(dimensionality1, kSITimeIndex) != 0) PRINTERROR;
    if (SIDimensionalityGetNumeratorExponentAtIndex(dimensionality1, kSICurrentIndex) != 0) PRINTERROR;
    if (SIDimensionalityGetNumeratorExponentAtIndex(dimensionality1, kSITemperatureIndex) != 0) PRINTERROR;
    if (SIDimensionalityGetNumeratorExponentAtIndex(dimensionality1, kSIAmountIndex) != 0) PRINTERROR;
    if (SIDimensionalityGetNumeratorExponentAtIndex(dimensionality1, kSILuminousIntensityIndex) != 0) PRINTERROR;

    if (SIDimensionalityGetDenominatorExponentAtIndex(dimensionality1, kSILengthIndex) != 0) PRINTERROR;
    if (SIDimensionalityGetDenominatorExponentAtIndex(dimensionality1, kSIMassIndex) != 0) PRINTERROR;
    if (SIDimensionalityGetDenominatorExponentAtIndex(dimensionality1, kSITimeIndex) != 0) PRINTERROR;
    if (SIDimensionalityGetDenominatorExponentAtIndex(dimensionality1, kSICurrentIndex) != 0) PRINTERROR;
    if (SIDimensionalityGetDenominatorExponentAtIndex(dimensionality1, kSITemperatureIndex) != 0) PRINTERROR;
    if (SIDimensionalityGetDenominatorExponentAtIndex(dimensionality1, kSIAmountIndex) != 0) PRINTERROR;
    if (SIDimensionalityGetDenominatorExponentAtIndex(dimensionality1, kSILuminousIntensityIndex) != 0) PRINTERROR;

    if (SIDimensionalityReducedExponentAtIndex(dimensionality1, kSILengthIndex) != 1) PRINTERROR;
    if (SIDimensionalityReducedExponentAtIndex(dimensionality1, kSIMassIndex) != 0) PRINTERROR;
    if (SIDimensionalityReducedExponentAtIndex(dimensionality1, kSITimeIndex) != 0) PRINTERROR;
    if (SIDimensionalityReducedExponentAtIndex(dimensionality1, kSICurrentIndex) != 0) PRINTERROR;
    if (SIDimensionalityReducedExponentAtIndex(dimensionality1, kSITemperatureIndex) != 0) PRINTERROR;
    if (SIDimensionalityReducedExponentAtIndex(dimensionality1, kSIAmountIndex) != 0) PRINTERROR;
    if (SIDimensionalityReducedExponentAtIndex(dimensionality1, kSILuminousIntensityIndex) != 0) PRINTERROR;

    fprintf(stderr, "%s end...without problems\n", __func__);
    return true;
}

bool dimensionalityTest1(void) {
    OCStringRef errorString = NULL;
    fprintf(stderr, "%s begin...\n", __func__);

    OCStringRef symbol = STR("L•M^2•T^3•I^4•ϴ^5•N^6•J^7/(L^2•M^3•T^4•I^5•ϴ^6•N^7•J^8)");
    SIDimensionalityRef dimensionality = SIDimensionalityForSymbol(symbol, &errorString);
    if (dimensionality == NULL) PRINTERROR;
    if (SIDimensionalityGetNumeratorExponentAtIndex(dimensionality, kSILengthIndex) != 1) PRINTERROR;
    if (SIDimensionalityGetDenominatorExponentAtIndex(dimensionality, kSILengthIndex) != 2) PRINTERROR;
    if (SIDimensionalityReducedExponentAtIndex(dimensionality, kSILengthIndex) != -1) PRINTERROR;

    fprintf(stderr, "%s end...without problems\n", __func__);
    return true;
}

bool dimensionalityTest2(void) {
    fprintf(stderr, "%s begin...\n", __func__);
    OCStringRef errorString = NULL;

    SIDimensionalityRef force = SIDimensionalityForQuantity(kSIQuantityForce, &errorString);
    SIDimensionalityRef acceleration = SIDimensionalityForQuantity(kSIQuantityAcceleration, &errorString);
    SIDimensionalityRef dimensionality = SIDimensionalityByDividing(force, acceleration);
    SIDimensionalityRef mass = SIDimensionalityForQuantity(kSIQuantityMass, &errorString);

    if (!SIDimensionalityEqual(dimensionality, mass)) PRINTERROR;

    fprintf(stderr, "%s end...without problems\n", __func__);
    return true;
}
