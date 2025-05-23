#include "../src/SILibrary.h"
#include <assert.h>
#include <stdio.h>

void test_dimensionality_0(void) {
    printf("Running %s...\n", __func__);

    OCStringRef err = NULL;

    // Parse base dimension "L"
    SIDimensionalityRef dimensionality1 = SIDimensionalityForSymbol(STR("L"), &err);
    if (!dimensionality1) {
        if (err) {
            printf("Error parsing dimensionality 'L': %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        assert(0 && "Failed to create dimensionality1");
    }
    // clean up any non-NULL err
    if (err) { OCRelease(err); err = NULL; }

    // grab its symbol (this returns an OCStringRef that you don't own)
    OCStringRef symbol1 = SIDimensionalityGetSymbol(dimensionality1);

    // Create by symbol, same thing
    SIDimensionalityRef dimensionality2 = SIDimensionalityWithBaseDimensionSymbol(STR("L"), &err);
    if (!dimensionality2) {
        if (err) {
            printf("Error creating dimensionality2: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        assert(0 && "Failed to create dimensionality2");
    }
    if (err) { OCRelease(err); err = NULL; }

    // compare
    assert(OCStringCompare(symbol1, STR("L"), 0) == kOCCompareEqualTo);
    assert(
        OCStringCompare(
            SIDimensionalityGetSymbol(dimensionality1),
            SIDimensionalityGetSymbol(dimensionality2),
            0
        ) == kOCCompareEqualTo
    );

    // check exponents
    assert(SIDimensionalityGetNumExpAtIndex(dimensionality1, kSILengthIndex) == 1);
    assert(SIDimensionalityGetNumExpAtIndex(dimensionality1, kSIMassIndex) == 0);
    /* ... etc ... */

    printf("%s passed\n", __func__);

    // cleanup the two dimensionality objects
    OCRelease(dimensionality1);
    OCRelease(dimensionality2);
}

void test_dimensionality_1(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;

    // Parse complex symbol
    OCStringRef symbol = STR("L•M^2•T^3•I^4•ϴ^5•N^6•J^7/(L^2•M^3•T^4•I^5•ϴ^6•N^7•J^8)");
    SIDimensionalityRef dimensionality = SIDimensionalityForSymbol(symbol, &err);
    if (!dimensionality) {
        if (err) { printf("Error parsing complex dimensionality: %s\n", OCStringGetCString(err)); OCRelease(err); }
        assert(0 && "Failed to create dimensionality");
    }
    if (err) { OCRelease(err); err = NULL; }
    assert(SIDimensionalityGetNumExpAtIndex(dimensionality, kSILengthIndex) == 1);
    assert(SIDimensionalityGetDenExpAtIndex(dimensionality, kSILengthIndex) == 2);
    assert(SIDimensionalityReducedExponentAtIndex(dimensionality, kSILengthIndex) == -1);
    printf("%s passed\n", __func__);
    OCRelease(dimensionality);
}

void test_dimensionality_2(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;

    // Get force and acceleration dimensionality
    SIDimensionalityRef force = SIDimensionalityForQuantity(kSIQuantityForce, &err);
    if (!force) { if (err) { printf("Error parsing force quantity: %s\n", OCStringGetCString(err)); OCRelease(err); } assert(0 && "Failed to parse force"); }
    OCRelease(err); err = NULL;
    SIDimensionalityRef acceleration = SIDimensionalityForQuantity(kSIQuantityAcceleration, &err);
    if (!acceleration) { if (err) { printf("Error parsing acceleration quantity: %s\n", OCStringGetCString(err)); OCRelease(err); } assert(0 && "Failed to parse acceleration"); }
    OCRelease(err); err = NULL;
    SIDimensionalityRef dimensionality = SIDimensionalityByDividing(force, acceleration);
    SIDimensionalityRef mass = SIDimensionalityForQuantity(kSIQuantityMass, &err);
    if (!mass) { if (err) { printf("Error parsing mass quantity: %s\n", OCStringGetCString(err)); OCRelease(err); } assert(0 && "Failed to parse mass"); }
    OCRelease(err); err = NULL;

    assert(SIDimensionalityEqual(dimensionality, mass));
    printf("%s passed\n", __func__);
    OCRelease(force);
    OCRelease(acceleration);
    OCRelease(dimensionality);
    OCRelease(mass);
}

void test_dimensionality_3(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;
    // Test equality for commutative symbols
    SIDimensionalityRef dim1 = SIDimensionalityForSymbol(STR("L*M"), &err);
    SIDimensionalityRef dim2 = SIDimensionalityForSymbol(STR("M*L"), &err);
    assert(SIDimensionalityEqual(dim1, dim2));
    // Test reduced exponents for mixed powers
    SIDimensionalityRef dim3 = SIDimensionalityForSymbol(STR("L^3*M^-2*T"), &err);
    assert(SIDimensionalityHasReducedExponents(dim3, 3, -2, 1, 0, 0, 0, 0));
    // Test dimensionless
    SIDimensionalityRef dless = SIDimensionalityDimensionless();
    assert(SIDimensionalityIsDimensionless(dless) && !SIDimensionalityIsDerived(dless));
    // Test base dimension index and symbol consistency
    for (int i = kSILengthIndex; i <= kSILuminousIntensityIndex; i++) {
        SIDimensionalityRef baseDim = SIDimensionalityForBaseDimensionIndex((SIBaseDimensionIndex)i);
        assert(SIDimensionalityGetNumExpAtIndex(baseDim, i) == 1);
        for (int j = kSILengthIndex; j <= kSILuminousIntensityIndex; j++) {
            if (i != j) {
                assert(SIDimensionalityGetNumExpAtIndex(baseDim, (SIBaseDimensionIndex)j) == 0);
            }
        }
        OCRelease(baseDim);
    }
    // Test WithBaseDimensionSymbol
    SIDimensionalityRef bySym = SIDimensionalityWithBaseDimensionSymbol(STR("L"), &err);
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

void test_dimensionality_show(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;
    // Get dimensionality for force
    SIDimensionalityRef force = SIDimensionalityForQuantity(kSIQuantityForce, &err);
    if (!force) {
        if (err) { printf("Error obtaining force dimensionality: %s\n", OCStringGetCString(err)); OCRelease(err); }
        assert(0 && "Failed to get force dimensionality");
    }
    if (err) { OCRelease(err); err = NULL; }
    // Compare symbol to documentation: "L•M/T^2"
    OCStringRef sym = SIDimensionalityGetSymbol(force);
    assert(sym && OCStringCompare(sym, STR("L•M/T^2"), 0) == kOCCompareEqualTo);
    printf("%s passed\n", __func__);
    OCRelease(force);
}

void test_dimensionality_symbol_acceleration(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;
    // Acceleration dimensionality via symbol
    SIDimensionalityRef accel = SIDimensionalityForSymbol(STR("L/T^2"), &err);
    if (!accel) {
        if (err) { printf("Error parsing acceleration symbol: %s\n", OCStringGetCString(err)); OCRelease(err); }
        assert(0 && "Failed to parse acceleration symbol");
    }
    if (err) { OCRelease(err); err = NULL; }
    OCStringRef symAccel = SIDimensionalityGetSymbol(accel);
    assert(symAccel && OCStringCompare(symAccel, STR("L/T^2"), 0) == kOCCompareEqualTo);
    printf("%s passed\n", __func__);
    OCRelease(accel);
}

void test_dimensionality_divide_mass(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;
    // mass = force / acceleration
    SIDimensionalityRef force = SIDimensionalityForQuantity(kSIQuantityForce, &err);
    assert(force && !err && "Failed to get force dimensionality");
    SIDimensionalityRef accel = SIDimensionalityForSymbol(STR("L/T^2"), &err);
    assert(accel && !err && "Failed to get acceleration dimensionality");
    SIDimensionalityRef mass = SIDimensionalityByDividing(force, accel);
    OCStringRef symMass = SIDimensionalityGetSymbol(mass);
    assert(symMass && OCStringCompare(symMass, STR("M"), 0) == kOCCompareEqualTo);
    printf("%s passed\n", __func__);
    OCRelease(force);
    OCRelease(accel);
    OCRelease(mass);
}

void test_dimensionality_multiply_work(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;
    // work = force * distance
    SIDimensionalityRef force = SIDimensionalityForQuantity(kSIQuantityForce, &err);
    assert(force && !err && "Failed to get force dimensionality");
    SIDimensionalityRef dist = SIDimensionalityForQuantity(kSIQuantityLength, &err);
    assert(dist && !err && "Failed to get distance dimensionality");
    SIDimensionalityRef work = SIDimensionalityByMultiplying(force, dist, &err);
    assert(work && !err && "Failed to multiply force and distance");
    OCStringRef symWork = SIDimensionalityGetSymbol(work);
    assert(symWork && OCStringCompare(symWork, STR("L^2•M/T^2"), 0) == kOCCompareEqualTo);
    printf("%s passed\n", __func__);
    OCRelease(force);
    OCRelease(dist);
    OCRelease(work);
}

void test_dimensionality_power_area(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;
    // area = distance^2
    SIDimensionalityRef dist = SIDimensionalityForQuantity(kSIQuantityLength, &err);
    assert(dist && !err && "Failed to get distance dimensionality");
    SIDimensionalityRef area = SIDimensionalityByRaisingToAPower(dist, 2, &err);
    assert(area && !err && "Failed to raise dimensionality to power");
    OCStringRef symArea = SIDimensionalityGetSymbol(area);
    assert(symArea && OCStringCompare(symArea, STR("L^2"), 0) == kOCCompareEqualTo);
    printf("%s passed\n", __func__);
    OCRelease(dist);
    OCRelease(area);
}

void test_dimensionality_reduction_behavior(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;
    // Base dimensionalities
    SIDimensionalityRef dist = SIDimensionalityForQuantity(kSIQuantityLength, &err);
    if (!dist) { if (err) { printf("Error obtaining length dimensionality: %s\n", OCStringGetCString(err)); OCRelease(err); } assert(0 && "Failed to get length dimensionality"); }
    OCRelease(err); err = NULL;
    // angle = dist/dist (reduced)
    SIDimensionalityRef angle = SIDimensionalityByDividing(dist, dist);
    OCStringRef symAngle = SIDimensionalityGetSymbol(angle);
    assert(symAngle && OCStringCompare(symAngle, STR("1"), 0) == kOCCompareEqualTo);
    // solidAngle = area/area (reduced), area = dist^2
    SIDimensionalityRef area = SIDimensionalityByRaisingToAPower(dist, 2, &err);
    if (!area) { if (err) { printf("Error raising to power: %s\n", OCStringGetCString(err)); OCRelease(err); } assert(0 && "Failed to compute area dimensionality"); }
    if (err) { OCRelease(err); err = NULL; }
    SIDimensionalityRef solidAngle = SIDimensionalityByDividing(area, area);
    OCStringRef symSolid = SIDimensionalityGetSymbol(solidAngle);
    assert(symSolid && OCStringCompare(symSolid, STR("1"), 0) == kOCCompareEqualTo);
    // non-reduced versions
    SIDimensionalityRef angleDerived = SIDimensionalityByDividingWithoutReducing(dist, dist);
    OCStringRef symAD = SIDimensionalityGetSymbol(angleDerived);
    assert(symAD && OCStringCompare(symAD, STR("L/L"), 0) == kOCCompareEqualTo);
    SIDimensionalityRef solidDerived = SIDimensionalityByDividingWithoutReducing(area, area);
    OCStringRef symSD = SIDimensionalityGetSymbol(solidDerived);
    assert(symSD && OCStringCompare(symSD, STR("L^2/L^2"), 0) == kOCCompareEqualTo);
    // equality checks
    assert(SIDimensionalityEqual(angle, solidAngle));
    assert(!SIDimensionalityEqual(angleDerived, solidDerived));
    assert(SIDimensionalityHasSameReducedDimensionality(angle, solidAngle));
    assert(SIDimensionalityHasSameReducedDimensionality(angleDerived, solidDerived));
    // dimensionless test
    assert(SIDimensionalityIsDimensionless(angleDerived));
    printf("%s passed\n", __func__);
    // cleanup
    OCRelease(dist);
    OCRelease(angle);
    OCRelease(area);
    OCRelease(solidAngle);
    OCRelease(angleDerived);
    OCRelease(solidDerived);
}
