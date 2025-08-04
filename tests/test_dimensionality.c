#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SITypes.h"
bool test_dimensionality_0(void) {
    printf("Running %s...\n", __func__);
    bool success = true;
    OCStringRef err = NULL;
    SIDimensionalityRef dimensionality1 = SIDimensionalityFromExpression(STR("L"), &err);
    if (!dimensionality1) {
        if (err) {
            printf("Error parsing dimensionality 'L': %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        printf("  ✗ Failed to create dimensionality1\n");
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }
    OCStringRef dimSym1 = SIDimensionalityCopySymbol(dimensionality1);
    SIDimensionalityRef dimensionality2 = SIDimensionalityWithBaseDimensionSymbol(STR("L"), &err);
    if (!dimensionality2) {
        if (err) {
            printf("Error creating dimensionality2: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        printf("  ✗ Failed to create dimensionality2\n");
        OCRelease(dimSym1);
        OCRelease(dimensionality1);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }
    if (OCStringCompare(dimSym1, STR("L"), 0) != kOCCompareEqualTo) {
        printf("  ✗ Symbol comparison failed: expected 'L', got '%s'\n", OCStringGetCString(dimSym1));
        success = false;
    }
    OCStringRef dimSym2 = SIDimensionalityCopySymbol(dimensionality2);
    if (OCStringCompare(dimSym1, dimSym2, 0) != kOCCompareEqualTo) {
        printf("  ✗ Symbol comparison failed: '%s' != '%s'\n", OCStringGetCString(dimSym1), OCStringGetCString(dimSym2));
        success = false;
    }
    if (SIDimensionalityGetNumExpAtIndex(dimensionality1, kSILengthIndex) != 1) {
        printf("  ✗ Length exponent check failed: expected 1, got %d\n", SIDimensionalityGetNumExpAtIndex(dimensionality1, kSILengthIndex));
        success = false;
    }
    if (SIDimensionalityGetNumExpAtIndex(dimensionality1, kSIMassIndex) != 0) {
        printf("  ✗ Mass exponent check failed: expected 0, got %d\n", SIDimensionalityGetNumExpAtIndex(dimensionality1, kSIMassIndex));
        success = false;
    }
    printf("%s %s\n", __func__, success ? "passed" : "failed");
    OCRelease(dimSym1);
    OCRelease(dimSym2);
    OCRelease(dimensionality1);
    OCRelease(dimensionality2);
    return success;
}
bool test_dimensionality_1(void) {
    printf("Running %s...\n", __func__);
    bool success = true;
    OCStringRef err = NULL;
    OCStringRef symbol = STR("L•M^2•T^3•I^4•ϴ^5•N^6•J^7/(L^2•M^3•T^4•I^5•ϴ^6•N^7•J^8)");
    SIDimensionalityRef dimensionality = SIDimensionalityFromExpression(symbol, &err);
    if (!dimensionality) {
        if (err) {
            printf("Error parsing complex dimensionality: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        return false;
    }
    if (err) { OCRelease(err); }
    if (SIDimensionalityGetNumExpAtIndex(dimensionality, kSILengthIndex) != 1) {
        printf("  ✗ Length numerator exponent check failed: expected 1, got %d\n", SIDimensionalityGetNumExpAtIndex(dimensionality, kSILengthIndex));
        success = false;
    }
    if (SIDimensionalityGetDenExpAtIndex(dimensionality, kSILengthIndex) != 2) {
        printf("  ✗ Length denominator exponent check failed: expected 2, got %d\n", SIDimensionalityGetDenExpAtIndex(dimensionality, kSILengthIndex));
        success = false;
    }
    if (SIDimensionalityReducedExponentAtIndex(dimensionality, kSILengthIndex) != -1) {
        printf("  ✗ Length reduced exponent check failed: expected -1, got %d\n", SIDimensionalityReducedExponentAtIndex(dimensionality, kSILengthIndex));
        success = false;
    }
    printf("%s %s\n", __func__, success ? "passed" : "failed");
    OCRelease(dimensionality);
    return success;
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
    if (err) {
        OCRelease(err);
        err = NULL;
    }
    SIDimensionalityRef acceleration = SIDimensionalityForQuantity(kSIQuantityAcceleration, &err);
    if (!acceleration) {
        if (err) {
            printf("Error parsing acceleration quantity: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        OCRelease(force);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }
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
    dim1 = SIDimensionalityFromExpression(STR("L*M"), &err);
    dim2 = SIDimensionalityFromExpression(STR("M*L"), &err);
    if (!dim1 || !dim2 || !SIDimensionalityEqual(dim1, dim2)) {
        success = false;
        goto cleanup;
    }
    dim3 = SIDimensionalityFromExpression(STR("M^-2*L^3*T"), &err);
    if (!dim3 || !SIDimensionalityHasReducedExponents(dim3, -2, 3, 1, 0, 0, 0, 0)) {
        success = false;
        goto cleanup;
    }
    dless = SIDimensionalityDimensionless();
    if (!SIDimensionalityIsDimensionless(dless) || SIDimensionalityIsDerived(dless)) {
        success = false;
        goto cleanup;
    }
    for (int i = kSIMassIndex; i <= kSILuminousIntensityIndex; i++) {
        SIDimensionalityRef baseDim = SIDimensionalityForBaseDimensionIndex((SIBaseDimensionIndex)i);
        if (!baseDim || SIDimensionalityGetNumExpAtIndex(baseDim, i) != 1) {
            OCRelease(baseDim);
            success = false;
            goto cleanup;
        }
        for (int j = kSIMassIndex; j <= kSILuminousIntensityIndex; j++) {
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
    if (err) {
        OCRelease(err);
        err = NULL;
    }
    // Compare symbol to expected representation
    OCStringRef sym = SIDimensionalityCopySymbol(force);
    if (!sym || OCStringCompare(sym, STR("M•L/T^2"), 0) != kOCCompareEqualTo) {
        printf("  ✗ Force symbol check failed: expected 'M•L/T^2', got '%s'\n", sym ? OCStringGetCString(sym) : "NULL");
        success = false;
    }
    OCRelease(sym);
    OCRelease(force);
    printf("%s %s\n", __func__, success ? "passed" : "failed");
    return success;
}
bool test_dimensionality_symbol_acceleration(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;
    bool success = true;
    SIDimensionalityRef accel = SIDimensionalityFromExpression(STR("L/T^2"), &err);
    if (!accel) {
        if (err) {
            printf("Error parsing acceleration symbol: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }
    OCStringRef symAccel = SIDimensionalityCopySymbol(accel);
    if (!symAccel || OCStringCompare(symAccel, STR("L/T^2"), 0) != kOCCompareEqualTo) {
        printf("  ✗ Acceleration symbol check failed: expected 'L/T^2', got '%s'\n", symAccel ? OCStringGetCString(symAccel) : "NULL");
        success = false;
    }
    OCRelease(symAccel);
    OCRelease(accel);
    printf("%s %s\n", __func__, success ? "passed" : "failed");
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
    SIDimensionalityRef accel = SIDimensionalityFromExpression(STR("L/T^2"), &err);
    if (!accel || err) {
        if (err) {
            printf("Error getting acceleration dimensionality: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        OCRelease(force);
        return false;
    }
    SIDimensionalityRef mass = SIDimensionalityByDividing(force, accel);
    OCStringRef symMass = SIDimensionalityCopySymbol(mass);
    if (!symMass || OCStringCompare(symMass, STR("M"), 0) != kOCCompareEqualTo) {
        printf("  ✗ Mass symbol check failed: expected 'M', got '%s'\n", symMass ? OCStringGetCString(symMass) : "NULL");
        success = false;
    }
    OCRelease(symMass);
    OCRelease(force);
    OCRelease(accel);
    OCRelease(mass);
    printf("%s %s\n", __func__, success ? "passed" : "failed");
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
    OCStringRef symWork = SIDimensionalityCopySymbol(work);
    if (!symWork || OCStringCompare(symWork, STR("M•L^2/T^2"), 0) != kOCCompareEqualTo) {
        printf("  ✗ Work symbol check failed: expected 'M•L^2/T^2', got '%s'\n", symWork ? OCStringGetCString(symWork) : "NULL");
        success = false;
    }
    OCRelease(symWork);
    OCRelease(force);
    OCRelease(dist);
    OCRelease(work);
    printf("%s %s\n", __func__, success ? "passed" : "failed");
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
    OCStringRef symArea = SIDimensionalityCopySymbol(area);
    if (!symArea || OCStringCompare(symArea, STR("L^2"), 0) != kOCCompareEqualTo) {
        printf("  ✗ Area symbol check failed: expected 'L^2', got '%s'\n", symArea ? OCStringGetCString(symArea) : "NULL");
        success = false;
    }
    OCRelease(symArea);
    OCRelease(dist);
    OCRelease(area);
    printf("%s %s\n", __func__, success ? "passed" : "failed");
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
    OCRelease(err);
    err = NULL;
    SIDimensionalityRef angle = SIDimensionalityByDividing(dist, dist);
    OCStringRef symAngle = SIDimensionalityCopySymbol(angle);
    if (!symAngle || OCStringCompare(symAngle, STR("1"), 0) != kOCCompareEqualTo) {
        printf("  ✗ Angle symbol check failed: expected '1', got '%s'\n", symAngle ? OCStringGetCString(symAngle) : "NULL");
        success = false;
    }
    OCRelease(symAngle);
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
    OCRelease(err);
    err = NULL;
    SIDimensionalityRef solidAngle = SIDimensionalityByDividing(area, area);
    OCStringRef symSolid = SIDimensionalityCopySymbol(solidAngle);
    if (!symSolid || OCStringCompare(symSolid, STR("1"), 0) != kOCCompareEqualTo) {
        printf("  ✗ Solid angle symbol check failed: expected '1', got '%s'\n", symSolid ? OCStringGetCString(symSolid) : "NULL");
        success = false;
    }
    OCRelease(symSolid);
    SIDimensionalityRef angleDerived = SIDimensionalityByDividingWithoutReducing(dist, dist);
    OCStringRef symAD = SIDimensionalityCopySymbol(angleDerived);
    if (!symAD || OCStringCompare(symAD, STR("L/L"), 0) != kOCCompareEqualTo) {
        printf("  ✗ Angle derived symbol check failed: expected 'L/L', got '%s'\n", symAD ? OCStringGetCString(symAD) : "NULL");
        success = false;
    }
    OCRelease(symAD);
    SIDimensionalityRef solidDerived = SIDimensionalityByDividingWithoutReducing(area, area);
    OCStringRef symSD = SIDimensionalityCopySymbol(solidDerived);
    if (!symSD || OCStringCompare(symSD, STR("L^2/L^2"), 0) != kOCCompareEqualTo) {
        printf("  ✗ Solid derived symbol check failed: expected 'L^2/L^2', got '%s'\n", symSD ? OCStringGetCString(symSD) : "NULL");
        success = false;
    }
    OCRelease(symSD);
    if (!SIDimensionalityEqual(angle, solidAngle)) {
        printf("  ✗ Angle and solid angle should be equal\n");
        success = false;
    }
    if (SIDimensionalityEqual(angleDerived, solidDerived)) {
        printf("  ✗ Angle derived and solid derived should NOT be equal\n");
        success = false;
    }
    if (!SIDimensionalityHasSameReducedDimensionality(angle, solidAngle)) {
        printf("  ✗ Angle and solid angle should have same reduced dimensionality\n");
        success = false;
    }
    if (!SIDimensionalityHasSameReducedDimensionality(angleDerived, solidDerived)) {
        printf("  ✗ Angle derived and solid derived should have same reduced dimensionality\n");
        success = false;
    }
    if (!SIDimensionalityIsDimensionless(angleDerived)) {
        printf("  ✗ Angle derived should be dimensionless\n");
        success = false;
    }
    printf("%s %s\n", __func__, success ? "passed" : "failed");
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
    SIDimensionalityRef copy = NULL;
    SIDimensionalityRef mcopy = NULL;
    /* parse */
    original = SIDimensionalityFromExpression(STR("L•M^2/T"), &err);
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
    // Success case - no verbose output needed
cleanup:
    if (err) OCRelease(err);
    if (original) OCRelease(original);
    if (copy) OCRelease(copy);
    if (mcopy) OCRelease(mcopy);
    printf("%s %s\n\n", __func__, success ? "passed" : "failed");
    return success;
}
bool test_dimensionality_parser_strictness(void) {
    printf("Running %s...\n", __func__);
    bool success = true;
    OCStringRef err = NULL;
    // These expressions should be REJECTED by the parser
    // Addition and subtraction are not valid in dimensional analysis
    const char* forbidden_expressions[] = {
        "L+T",      // Length + Time is physically meaningless
        "M-L",      // Mass - Length is physically meaningless
        "L + T",    // With spaces
        "M - L",    // With spaces
        "L+M*T",    // Mixed addition
        "L*T+M",    // Addition at end
        "L/T+M",    // Adding velocity and mass (meaningless)
        "L^2+T^2",  // Adding area and time-squared (meaningless)
        "M+L+T",    // Multiple additions
        "L-T+M",    // Mixed subtraction and addition
        NULL};
    for (int i = 0; forbidden_expressions[i] != NULL; i++) {
        const char* expr = forbidden_expressions[i];
        err = NULL;
        OCStringRef expr_str = OCStringCreateWithCString(expr);
        SIDimensionalityRef result = SIDimensionalityFromExpression(expr_str, &err);
        if (result != NULL) {
            // Parser incorrectly accepted the expression - this is a failure
            OCStringRef symbol = SIDimensionalityCopySymbol(result);
            const char* symbol_str = symbol ? OCStringGetCString(symbol) : "NULL";
            printf("  ✗ Expression '%s' was incorrectly accepted -> '%s'\n",
                   expr, symbol_str);
            printf("    This violates dimensional analysis principles!\n");
            OCRelease(result);
            OCRelease(symbol);
            success = false;
        } else if (err != NULL) {
            // Parser correctly rejected the expression - expected behavior, no output needed
            OCRelease(err);
        } else {
            // Parser returned NULL without error - unexpected
            printf("  ✗ Unexpected: '%s' returned NULL without error\n", expr);
            success = false;
        }
        OCRelease(expr_str);
    }
    if (!success) {
        printf("  ✗ CRITICAL FAILURE: Parser accepts invalid dimensional operations\n");
        printf("    Addition and subtraction MUST be rejected in dimensional analysis!\n");
    }
    printf("%s %s\n", __func__, success ? "passed" : "failed");
    return success;
}
