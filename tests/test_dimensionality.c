#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "SILibrary.h"


bool test_dimensionality_0(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;
    SIDimensionalityRef dimensionality1 = SIDimensionalityParseExpression(STR("L"), &err);
    if (!dimensionality1) {
        if (err) {
            printf("Error parsing dimensionality 'L': %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        assert(0 && "Failed to create dimensionality1");
    }
    if (err) { OCRelease(err); err = NULL; }

    OCStringRef symbol1 = SIDimensionalityGetSymbol(dimensionality1);
    SIDimensionalityRef dimensionality2 = SIDimensionalityWithBaseDimensionSymbol(STR("L"), &err);
    if (!dimensionality2) {
        if (err) {
            printf("Error creating dimensionality2: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        assert(0 && "Failed to create dimensionality2");
    }
    if (err) { OCRelease(err); err = NULL; }

    assert(OCStringCompare(symbol1, STR("L"), 0) == kOCCompareEqualTo);
    assert(
        OCStringCompare(
            SIDimensionalityGetSymbol(dimensionality1),
            SIDimensionalityGetSymbol(dimensionality2),
            0
        ) == kOCCompareEqualTo
    );
    assert(SIDimensionalityGetNumExpAtIndex(dimensionality1, kSILengthIndex) == 1);
    assert(SIDimensionalityGetNumExpAtIndex(dimensionality1, kSIMassIndex) == 0);

    printf("%s passed\n", __func__);
    OCRelease(dimensionality1);
    OCRelease(dimensionality2);
    return true;
}

bool test_dimensionality_1(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;

    OCStringRef symbol = STR("L•M^2•T^3•I^4•ϴ^5•N^6•J^7/(L^2•M^3•T^4•I^5•ϴ^6•N^7•J^8)");
    SIDimensionalityRef dimensionality = SIDimensionalityParseExpression(symbol, &err);
    if (!dimensionality) {
        if (err) {
            printf("Error parsing complex dimensionality: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        return false;
    }
    if (err) { OCRelease(err); }

    if (SIDimensionalityGetNumExpAtIndex(dimensionality, kSILengthIndex) != 1) return false;
    if (SIDimensionalityGetDenExpAtIndex(dimensionality, kSILengthIndex) != 2) return false;
    if (SIDimensionalityReducedExponentAtIndex(dimensionality, kSILengthIndex) != -1) return false;

    printf("%s passed\n", __func__);
    OCRelease(dimensionality);
    return true;
}

bool test_dimensionality_2(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;

    SIDimensionalityRef force = SIDimensionalityForQuantity(kSIQuantityForce, &err);
    if (!force) {
        if (err) {
            printf("Error parsing force quantity: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        return false;
    }
    if (err) { OCRelease(err); err = NULL; }

    SIDimensionalityRef acceleration = SIDimensionalityForQuantity(kSIQuantityAcceleration, &err);
    if (!acceleration) {
        if (err) {
            printf("Error parsing acceleration quantity: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        OCRelease(force);
        return false;
    }
    if (err) { OCRelease(err); err = NULL; }

    SIDimensionalityRef dimensionality = SIDimensionalityByDividing(force, acceleration);

    SIDimensionalityRef mass = SIDimensionalityForQuantity(kSIQuantityMass, &err);
    if (!mass) {
        if (err) {
            printf("Error parsing mass quantity: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        OCRelease(force);
        OCRelease(acceleration);
        OCRelease(dimensionality);
        return false;
    }
    if (err) { OCRelease(err); }

    bool success = SIDimensionalityEqual(dimensionality, mass);
    if (!success) {
        printf("Error: dimensionality of force/acceleration does not equal mass.\n");
    } else {
        printf("%s passed\n", __func__);
    }

    OCRelease(force);
    OCRelease(acceleration);
    OCRelease(dimensionality);
    OCRelease(mass);

    return success;
}

bool test_dimensionality_3(void) {
    printf("Running %s...\n", __func__);
    bool success = true;
    OCStringRef err = NULL;

    SIDimensionalityRef dim1 = NULL, dim2 = NULL, dim3 = NULL, dless = NULL;
    SIDimensionalityRef bySym = NULL, byIdx = NULL;

    dim1 = SIDimensionalityParseExpression(STR("L*M"), &err);
    dim2 = SIDimensionalityParseExpression(STR("M*L"), &err);
    if (!dim1 || !dim2 || !SIDimensionalityEqual(dim1, dim2)) {
        success = false;
        goto cleanup;
    }

    dim3 = SIDimensionalityParseExpression(STR("L^3*M^-2*T"), &err);
    if (!dim3 || !SIDimensionalityHasReducedExponents(dim3, 3, -2, 1, 0, 0, 0, 0)) {
        success = false;
        goto cleanup;
    }

    dless = SIDimensionalityDimensionless();
    if (!SIDimensionalityIsDimensionless(dless) || SIDimensionalityIsDerived(dless)) {
        success = false;
        goto cleanup;
    }

    for (int i = kSILengthIndex; i <= kSILuminousIntensityIndex; i++) {
        SIDimensionalityRef baseDim = SIDimensionalityForBaseDimensionIndex((SIBaseDimensionIndex)i);
        if (!baseDim || SIDimensionalityGetNumExpAtIndex(baseDim, i) != 1) {
            OCRelease(baseDim);
            success = false;
            goto cleanup;
        }
        for (int j = kSILengthIndex; j <= kSILuminousIntensityIndex; j++) {
            if (i != j && SIDimensionalityGetNumExpAtIndex(baseDim, (SIBaseDimensionIndex)j) != 0) {
                OCRelease(baseDim);
                success = false;
                goto cleanup;
            }
        }
        OCRelease(baseDim);
    }

    bySym = SIDimensionalityWithBaseDimensionSymbol(STR("L"), &err);
    byIdx = SIDimensionalityForBaseDimensionIndex(kSILengthIndex);
    if (!bySym || !byIdx || !SIDimensionalityEqual(bySym, byIdx)) {
        success = false;
        goto cleanup;
    }

cleanup:
    if (dim1) OCRelease(dim1);
    if (dim2) OCRelease(dim2);
    if (dim3) OCRelease(dim3);
    if (dless) OCRelease(dless);
    if (bySym) OCRelease(bySym);
    if (byIdx) OCRelease(byIdx);
    if (err) OCRelease(err);

    printf("%s %s\n", __func__, success ? "passed" : "failed ***");
    return success;
}

bool test_dimensionality_show(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;
    bool success = true;

    // Get dimensionality for force
    SIDimensionalityRef force = SIDimensionalityForQuantity(kSIQuantityForce, &err);
    if (!force) {
        if (err) {
            printf("Error obtaining force dimensionality: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        return false;
    }
    if (err) { OCRelease(err); err = NULL; }

    // Compare symbol to expected representation
    OCStringRef sym = SIDimensionalityGetSymbol(force);
    if (!sym || OCStringCompare(sym, STR("L•M/T^2"), 0) != kOCCompareEqualTo) {
        success = false;
    }

    OCRelease(force);

    if (success) printf("%s passed\n", __func__);
    return success;
}

bool test_dimensionality_symbol_acceleration(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;
    bool success = true;

    SIDimensionalityRef accel = SIDimensionalityParseExpression(STR("L/T^2"), &err);
    if (!accel) {
        if (err) {
            printf("Error parsing acceleration symbol: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        return false;
    }
    if (err) { OCRelease(err); err = NULL; }

    OCStringRef symAccel = SIDimensionalityGetSymbol(accel);
    if (!symAccel || OCStringCompare(symAccel, STR("L/T^2"), 0) != kOCCompareEqualTo) {
        success = false;
    }

    OCRelease(accel);
    if (success) printf("%s passed\n", __func__);
    return success;
}

bool test_dimensionality_divide_mass(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;
    bool success = true;

    SIDimensionalityRef force = SIDimensionalityForQuantity(kSIQuantityForce, &err);
    if (!force || err) {
        if (err) {
            printf("Error getting force dimensionality: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        return false;
    }

    SIDimensionalityRef accel = SIDimensionalityParseExpression(STR("L/T^2"), &err);
    if (!accel || err) {
        if (err) {
            printf("Error getting acceleration dimensionality: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        OCRelease(force);
        return false;
    }

    SIDimensionalityRef mass = SIDimensionalityByDividing(force, accel);
    OCStringRef symMass = SIDimensionalityGetSymbol(mass);
    if (!symMass || OCStringCompare(symMass, STR("M"), 0) != kOCCompareEqualTo) {
        success = false;
    }

    OCRelease(force);
    OCRelease(accel);
    OCRelease(mass);

    if (success) printf("%s passed\n", __func__);
    return success;
}

bool test_dimensionality_multiply_work(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;
    bool success = true;

    // work = force * distance
    SIDimensionalityRef force = SIDimensionalityForQuantity(kSIQuantityForce, &err);
    if (!force || err) {
        if (err) {
            printf("Error getting force dimensionality: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        return false;
    }

    SIDimensionalityRef dist = SIDimensionalityForQuantity(kSIQuantityLength, &err);
    if (!dist || err) {
        if (err) {
            printf("Error getting distance dimensionality: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        OCRelease(force);
        return false;
    }

    SIDimensionalityRef work = SIDimensionalityByMultiplying(force, dist, &err);
    if (!work || err) {
        if (err) {
            printf("Error multiplying force and distance: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        OCRelease(force);
        OCRelease(dist);
        return false;
    }

    OCStringRef symWork = SIDimensionalityGetSymbol(work);
    if (!symWork || OCStringCompare(symWork, STR("L^2•M/T^2"), 0) != kOCCompareEqualTo) {
        success = false;
    }

    OCRelease(force);
    OCRelease(dist);
    OCRelease(work);

    if (success) printf("%s passed\n", __func__);
    return success;
}

bool test_dimensionality_power_area(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;
    bool success = true;

    // area = distance^2
    SIDimensionalityRef dist = SIDimensionalityForQuantity(kSIQuantityLength, &err);
    if (!dist || err) {
        if (err) {
            printf("Error getting distance dimensionality: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        return false;
    }

    SIDimensionalityRef area = SIDimensionalityByRaisingToPower(dist, 2, &err);
    if (!area || err) {
        if (err) {
            printf("Error raising dimensionality to power: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        OCRelease(dist);
        return false;
    }

    OCStringRef symArea = SIDimensionalityGetSymbol(area);
    if (!symArea || OCStringCompare(symArea, STR("L^2"), 0) != kOCCompareEqualTo) {
        success = false;
    }

    OCRelease(dist);
    OCRelease(area);

    if (success) printf("%s passed\n", __func__);
    return success;
}

bool test_dimensionality_reduction_behavior(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;
    bool success = true;

    SIDimensionalityRef dist = SIDimensionalityForQuantity(kSIQuantityLength, &err);
    if (!dist || err) {
        if (err) {
            printf("Error obtaining length dimensionality: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        return false;
    }
    OCRelease(err); err = NULL;

    SIDimensionalityRef angle = SIDimensionalityByDividing(dist, dist);
    OCStringRef symAngle = SIDimensionalityGetSymbol(angle);
    if (!symAngle || OCStringCompare(symAngle, STR("1"), 0) != kOCCompareEqualTo) success = false;

    SIDimensionalityRef area = SIDimensionalityByRaisingToPower(dist, 2, &err);
    if (!area || err) {
        if (err) {
            printf("Error raising to power: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        OCRelease(dist);
        OCRelease(angle);
        return false;
    }
    OCRelease(err); err = NULL;

    SIDimensionalityRef solidAngle = SIDimensionalityByDividing(area, area);
    OCStringRef symSolid = SIDimensionalityGetSymbol(solidAngle);
    if (!symSolid || OCStringCompare(symSolid, STR("1"), 0) != kOCCompareEqualTo) success = false;

    SIDimensionalityRef angleDerived = SIDimensionalityByDividingWithoutReducing(dist, dist);
    OCStringRef symAD = SIDimensionalityGetSymbol(angleDerived);
    if (!symAD || OCStringCompare(symAD, STR("L/L"), 0) != kOCCompareEqualTo) success = false;

    SIDimensionalityRef solidDerived = SIDimensionalityByDividingWithoutReducing(area, area);
    OCStringRef symSD = SIDimensionalityGetSymbol(solidDerived);
    if (!symSD || OCStringCompare(symSD, STR("L^2/L^2"), 0) != kOCCompareEqualTo) success = false;

    if (!SIDimensionalityEqual(angle, solidAngle)) success = false;
    if (SIDimensionalityEqual(angleDerived, solidDerived)) success = false;
    if (!SIDimensionalityHasSameReducedDimensionality(angle, solidAngle)) success = false;
    if (!SIDimensionalityHasSameReducedDimensionality(angleDerived, solidDerived)) success = false;
    if (!SIDimensionalityIsDimensionless(angleDerived)) success = false;

    if (success) printf("%s passed\n", __func__);

    OCRelease(dist);
    OCRelease(angle);
    OCRelease(area);
    OCRelease(solidAngle);
    OCRelease(angleDerived);
    OCRelease(solidDerived);

    return success;
}

bool test_dimensionality_deep_copy(void) {
    printf("Running %s...\n", __func__);
    bool success = true;
    OCStringRef err = NULL;

    /* initialize everything to NULL */
    SIDimensionalityRef original = NULL;
    SIDimensionalityRef copy     = NULL;
    SIDimensionalityRef mcopy    = NULL;

    /* parse */
    original = SIDimensionalityParseExpression(STR("L•M^2/T"), &err);
    if (!original || err) {
        fprintf(stderr,
                "  ✗ PARSE failed: %s\n",
                err ? OCStringGetCString(err) : "returned NULL");
        success = false;
        goto cleanup;
    }

    /* Deep-copy (immutable) */
    copy = OCTypeDeepCopy(original);
    if (!copy) {
        fprintf(stderr, "  ✗ OCTypeDeepCopy returned NULL\n");
        success = false;
        goto cleanup;
    }
    if (copy != original) {
        fprintf(stderr,
                "  ✗ OCTypeDeepCopy returned a new instance (copy=%p) but expected the singleton original=%p\n",
                (void*)copy, (void*)original);
        success = false;
        goto cleanup;
    }

    /* Deep-copy (mutable) */
    mcopy = OCTypeDeepCopyMutable(original);
    if (!mcopy) {
        fprintf(stderr, "  ✗ OCTypeDeepCopyMutable returned NULL\n");
        success = false;
        goto cleanup;
    }
    if (mcopy != original) {
        fprintf(stderr,
                "  ✗ OCTypeDeepCopyMutable returned a new instance (mcopy=%p) but expected the singleton original=%p\n",
                (void*)mcopy, (void*)original);
        success = false;
        goto cleanup;
    }

    printf("  ✓ Both DeepCopy and DeepCopyMutable returned the singleton instance\n");

cleanup:
    if (err)      OCRelease(err);
    if (original) OCRelease(original);
    if (copy)     OCRelease(copy);
    if (mcopy)    OCRelease(mcopy);

    printf("%s %s\n\n", __func__, success ? "passed" : "failed");
    return success;
}