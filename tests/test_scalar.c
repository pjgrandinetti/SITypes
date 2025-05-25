#include "test_utils.h"
#include "../src/SIScalar.h"
#include "../src/SIUnit.h"
#include <assert.h>
#include <stdio.h>
#include <math.h>   // For fabs, fabsf, creal, cimag
#include <complex.h> // For complex numbers and I macro


// Helper unit creation functions
static SIUnitRef meter_unit(void) {
    double mult = 1.0;
    SIUnitRef u = SIUnitForSymbol(STR("m"), &mult, NULL);
    assert(u != NULL);
    return u;
}
static SIUnitRef second_unit(void) {
    double mult = 1.0;
    SIUnitRef u = SIUnitForSymbol(STR("s"), &mult, NULL);
    assert(u != NULL);
    return u;
}
static SIUnitRef kilogram_unit(void) {
    double mult = 1.0;
    SIUnitRef u = SIUnitForSymbol(STR("kg"), &mult, NULL);
    assert(u != NULL);
    return u;
}
static SIUnitRef cm_unit(void) {
    double mult = 1.0;
    SIUnitRef u = SIUnitForSymbol(STR("cm"), &mult, NULL);
    assert(u != NULL);
    return u;
}
static SIUnitRef km_unit(void) {
    double mult = 1.0;
    SIUnitRef u = SIUnitForSymbol(STR("km"), &mult, NULL);
    assert(u != NULL);
    return u;
}
static SIUnitRef mps_unit(void) {
    double mult = 1.0;
    SIUnitRef u = SIUnitForSymbol(STR("m/s"), &mult, NULL);
    assert(u != NULL);
    return u;
}
static SIUnitRef dimensionless_unit(void) {
    return SIUnitDimensionlessAndUnderived();
}

void test_SIScalarGetTypeID(void) { assert(SIScalarGetTypeID() != 0); }

void test_SIScalarCreateCopy(void) {
    SIUnitRef u = meter_unit();
    float input = 1.0f;
    SIScalarRef s = SIScalarCreateWithFloat(input, u);
    SIScalarRef c = SIScalarCreateCopy(s);
    assert(c != NULL);
    // Must be a different instance
    assert(c != s);
    // Value preserved
    assert(fabsf(SIScalarFloatValue(s) - SIScalarFloatValue(c)) < 1e-6);
    // Unit preserved
    OCStringRef us = SIScalarCreateUnitString(s);
    OCStringRef uc = SIScalarCreateUnitString(c);
    assert(OCStringEqual(us, uc));
    OCRelease(us);
    OCRelease(uc);
    OCRelease(c);
    OCRelease(s);
    OCRelease(u);
}

void test_SIScalarCreateMutableCopy(void) {
    SIUnitRef u = meter_unit();
    SIScalarRef orig = SIScalarCreateWithFloat(2.0f, u);
    SIMutableScalarRef copy = SIScalarCreateMutableCopy(orig);
    assert(copy != NULL);
    // Must be a distinct object
    assert(copy != orig);
    // Value and unit preserved initially
    assert(fabsf(SIScalarFloatValue(copy) - SIScalarFloatValue(orig)) < 1e-6);
    OCStringRef unit_orig = SIScalarCreateUnitString(orig);
    OCStringRef unit_copy = SIScalarCreateUnitString(copy);
    assert(OCStringEqual(unit_orig, unit_copy));
    OCRelease(unit_orig);
    OCRelease(unit_copy);
    // Mutate the copy and ensure original stays unchanged
    SIScalarSetFloatValue(copy, 5.0f);
    assert(fabsf(SIScalarFloatValue(copy) - 5.0f) < 1e-6);
    assert(fabsf(SIScalarFloatValue(orig) - 2.0f) < 1e-6);
    OCRelease(copy);
    OCRelease(orig);
    OCRelease(u);
}

void test_SIScalarCreateWithFloat(void) {
    SIUnitRef u = meter_unit();
    SIScalarRef s = SIScalarCreateWithFloat(3.0f, u);
    assert(s != NULL);
    OCRelease(s);
    OCRelease(u);
}

void test_SIScalarCreateMutableWithFloat(void) {
    SIUnitRef u = meter_unit();
    SIMutableScalarRef m = SIScalarCreateMutableWithFloat(4.0f, u);
    assert(m != NULL);
    assert(fabsf(SIScalarFloatValue(m) - 4.0f) < 1e-6);
    OCRelease(m); OCRelease(u);
}

void test_SIScalarCreateWithDouble(void) {
    SIUnitRef u = meter_unit();
    SIScalarRef s = SIScalarCreateWithDouble(5.0, u);
    assert(s != NULL);
    assert(fabs(SIScalarDoubleValue(s) - 5.0) < 1e-9);
    OCRelease(s); OCRelease(u);
}

void test_SIScalarCreateMutableWithDouble(void) {
    SIUnitRef u = meter_unit();
    SIMutableScalarRef m = SIScalarCreateMutableWithDouble(6.0, u);
    assert(m != NULL);
    assert(fabs(SIScalarDoubleValue(m) - 6.0) < 1e-9);
    OCRelease(m); OCRelease(u);
}

void test_SIScalarCreateWithFloatComplex(void) {
    SIUnitRef u = meter_unit();
    float complex val = 1.0f + 2.0f * I;
    SIScalarRef s = SIScalarCreateWithFloatComplex(val, u);
    assert(s != NULL);
    assert(fabsf(crealf(SIScalarFloatComplexValue(s)) - 1.0f) < 1e-6);
    assert(fabsf(cimagf(SIScalarFloatComplexValue(s)) - 2.0f) < 1e-6);
    OCRelease(s); OCRelease(u);
}

void test_SIScalarCreateMutableWithFloatComplex(void) {
    SIUnitRef u = meter_unit();
    float complex val = 2.0f + 3.0f * I;
    SIMutableScalarRef m = SIScalarCreateMutableWithFloatComplex(val, u);
    assert(m != NULL);
    assert(fabsf(crealf(SIScalarFloatComplexValue(m)) - 2.0f) < 1e-6);
    assert(fabsf(cimagf(SIScalarFloatComplexValue(m)) - 3.0f) < 1e-6);
    OCRelease(m); OCRelease(u);
}

void test_SIScalarCreateWithDoubleComplex(void) {
    SIUnitRef u = meter_unit();
    double complex val = 3.0 + 4.0 * I;
    SIScalarRef s = SIScalarCreateWithDoubleComplex(val, u);
    assert(s != NULL);
    assert(fabs(creal(SIScalarDoubleComplexValue(s)) - 3.0) < 1e-9);
    assert(fabs(cimag(SIScalarDoubleComplexValue(s)) - 4.0) < 1e-9);
    OCRelease(s); OCRelease(u);
}

void test_SIScalarCreateMutableWithDoubleComplex(void) {
    SIUnitRef u = meter_unit();
    double complex val = 4.0 + 5.0 * I;
    SIMutableScalarRef m = SIScalarCreateMutableWithDoubleComplex(val, u);
    assert(m != NULL);
    assert(fabs(creal(SIScalarDoubleComplexValue(m)) - 4.0) < 1e-9);
    assert(fabs(cimag(SIScalarDoubleComplexValue(m)) - 5.0) < 1e-9);
    OCRelease(m); OCRelease(u);
}

void test_SIScalarCreateWithStringContainingSingleUnitFromLibrary(void) {
    OCStringRef unitStr = STR("kg");
    SIScalarRef s = SIScalarCreateWithStringContainingSingleUnitFromLibrary(unitStr);
    assert(s != NULL);
    assert(fabs(SIScalarDoubleValue(s) - 1.0) < 1e-9); // Should be 1.0 of that unit
    // Further check unit if possible, e.g. by comparing to SIUnitForSymbol("kg")
    OCRelease(s); OCRelease(unitStr);
}

void test_SIScalarGetValue(void) {
    SIUnitRef u = meter_unit();
    SIScalarRef s = SIScalarCreateWithFloat(7.0f, u);
    __SINumber n = SIScalarGetValue(s);
    assert(n.floatValue == 7.0f); // Assuming it was created with float
    OCRelease(s); OCRelease(u);
}

void test_SIScalarSetFloatValue(void) {
    SIUnitRef u = meter_unit();
    SIMutableScalarRef m = SIScalarCreateMutableWithFloat(0.0f, u);
    SIScalarSetFloatValue(m, 8.0f);
    assert(fabsf(SIScalarFloatValue(m) - 8.0f) < 1e-6);
    OCRelease(m); OCRelease(u);
}

void test_SIScalarSetDoubleValue(void) {
    SIUnitRef u = meter_unit();
    SIMutableScalarRef m = SIScalarCreateMutableWithDouble(0.0, u);
    SIScalarSetDoubleValue(m, 9.0);
    assert(fabs(SIScalarDoubleValue(m) - 9.0) < 1e-9);
    OCRelease(m); OCRelease(u);
}

void test_SIScalarSetFloatComplexValue(void) {
    SIUnitRef u = meter_unit();
    SIMutableScalarRef m = SIScalarCreateMutableWithFloatComplex(0.0f + 0.0f * I, u);
    float complex val = 1.0f + 1.0f * I;
    SIScalarSetFloatComplexValue(m, val);
    assert(fabsf(crealf(SIScalarFloatComplexValue(m)) - 1.0f) < 1e-6);
    assert(fabsf(cimagf(SIScalarFloatComplexValue(m)) - 1.0f) < 1e-6);
    OCRelease(m); OCRelease(u);
}

void test_SIScalarSetDoubleComplexValue(void) {
    SIUnitRef u = meter_unit();
    SIMutableScalarRef m = SIScalarCreateMutableWithDoubleComplex(0.0 + 0.0 * I, u);
    double complex val = 2.0 + 2.0 * I;
    SIScalarSetDoubleComplexValue(m, val);
    assert(fabs(creal(SIScalarDoubleComplexValue(m)) - 2.0) < 1e-9);
    assert(fabs(cimag(SIScalarDoubleComplexValue(m)) - 2.0) < 1e-9);
    OCRelease(m); OCRelease(u);
}

void test_SIScalarSetElementType(void) {
    SIUnitRef u = meter_unit();
    SIMutableScalarRef m = SIScalarCreateMutableWithFloat(1.0f, u);
    SIScalarSetElementType(m, kSINumberFloat64Type);
    // Check if the internal type is now double, e.g. by checking precision or a specific API if available
    // For now, just check if value is preserved
    assert(fabs(SIScalarDoubleValue(m) - 1.0) < 1e-9);
    OCRelease(m); OCRelease(u);
}

void test_SIScalarFloatValue(void) {
    SIUnitRef u = meter_unit();
    SIScalarRef s = SIScalarCreateWithFloat(1.5f, u);
    assert(fabsf(SIScalarFloatValue(s) - 1.5f) < 1e-6);
    OCRelease(s); OCRelease(u);
}

void test_SIScalarDoubleValue(void) {
    SIUnitRef u = meter_unit();
    SIScalarRef s = SIScalarCreateWithDouble(2.5, u);
    assert(fabs(SIScalarDoubleValue(s) - 2.5) < 1e-9);
    OCRelease(s); OCRelease(u);
}

void test_SIScalarFloatComplexValue(void) {
    SIUnitRef u = meter_unit();
    SIScalarRef s = SIScalarCreateWithFloatComplex(1.0f + 2.0f * I, u);
    assert(fabsf(crealf(SIScalarFloatComplexValue(s)) - 1.0f) < 1e-6);
    assert(fabsf(cimagf(SIScalarFloatComplexValue(s)) - 2.0f) < 1e-6);
    OCRelease(s); OCRelease(u);
}

void test_SIScalarDoubleComplexValue(void) {
    SIUnitRef u = meter_unit();
    SIScalarRef s = SIScalarCreateWithDoubleComplex(2.0 + 3.0 * I, u);
    assert(fabs(creal(SIScalarDoubleComplexValue(s)) - 2.0) < 1e-9);
    assert(fabs(cimag(SIScalarDoubleComplexValue(s)) - 3.0) < 1e-9);
    OCRelease(s); OCRelease(u);
}

void test_SIScalarCreateByConvertingToNumberType(void) {
    SIUnitRef u = meter_unit();
    SIScalarRef s_double = SIScalarCreateWithDouble(123.456, u);
    SIScalarRef s_float = SIScalarCreateByConvertingToNumberType(s_double, kSINumberFloat32Type);
    assert(s_float != NULL);
    assert(fabsf(SIScalarFloatValue(s_float) - 123.456f) < 1e-3); // Precision loss expected
    OCRelease(s_double); OCRelease(s_float); OCRelease(u);
}

void test_SIScalarTakeComplexPart(void) {
    SIUnitRef u = meter_unit();
    SIMutableScalarRef m = SIScalarCreateMutableWithFloatComplex(3.0f + 4.0f * I, u);
    bool success = SIScalarTakeComplexPart(m, kSIRealPart);
    assert(success);
    assert(SIScalarIsReal(m));
    assert(fabsf(SIScalarFloatValue(m) - 3.0f) < 1e-6);
    OCRelease(m); OCRelease(u);
}

void test_SIScalarCreateByTakingComplexPart(void) {
    SIUnitRef u = meter_unit();
    SIScalarRef s_complex = SIScalarCreateWithFloatComplex(3.0f + 4.0f * I, u);
    SIScalarRef s_real_part = SIScalarCreateByTakingComplexPart(s_complex, kSIRealPart);
    assert(s_real_part != NULL);
    assert(SIScalarIsReal(s_real_part));
    assert(fabsf(SIScalarFloatValue(s_real_part) - 3.0f) < 1e-6);
    OCRelease(s_complex); OCRelease(s_real_part); OCRelease(u);
}

void test_SIScalarCreateByReducingUnit(void) {
    SIUnitRef m = meter_unit();
    SIUnitRef s_unit = second_unit();
    double mult1 = 1.0;
    SIUnitRef ms = SIUnitByMultiplying(m, s_unit, &mult1, NULL);
    double mult2 = 1.0;
    SIUnitRef m_final = SIUnitByDividing(ms, s_unit, &mult2);

    SIScalarRef scalar_unreduced = SIScalarCreateWithFloat(1.0f, m_final);
    SIScalarRef scalar_reduced = SIScalarCreateByReducingUnit(scalar_unreduced);
    assert(scalar_reduced != NULL);
    OCStringRef unit_str = SIScalarCreateUnitString(scalar_reduced);
    assert(OCStringEqual(unit_str, STR("m")));
    OCRelease(unit_str);

    OCRelease(m); OCRelease(s_unit); OCRelease(ms); OCRelease(m_final);
    OCRelease(scalar_unreduced); OCRelease(scalar_reduced);
}

void test_SIScalarReduceUnit(void) {
    SIUnitRef m = meter_unit();
    SIUnitRef s_unit = second_unit();
    double mult1 = 1.0;
    SIUnitRef ms = SIUnitByMultiplying(m, s_unit, &mult1, NULL);
    double mult2 = 1.0;
    SIUnitRef m_final_unit = SIUnitByDividing(ms, s_unit, &mult2);

    SIMutableScalarRef scalar = SIScalarCreateMutableWithFloat(1.0f, m_final_unit);
    bool success = SIScalarReduceUnit(scalar);
    assert(success);
    OCStringRef unit_str = SIScalarCreateUnitString(scalar);
    assert(OCStringEqual(unit_str, STR("m")));
    OCRelease(unit_str);

    OCRelease(m); OCRelease(s_unit); OCRelease(ms); OCRelease(m_final_unit);
    OCRelease(scalar);
}

void test_SIScalarConvertToUnit(void) {
    SIUnitRef m = meter_unit();
    SIUnitRef cm = cm_unit();
    SIMutableScalarRef scalar = SIScalarCreateMutableWithFloat(1.0f, m); // 1m
    OCStringRef error = NULL;
    bool success = SIScalarConvertToUnit(scalar, cm, &error);
    assert(success);
    assert(error == NULL);
    assert(fabsf(SIScalarFloatValue(scalar) - 100.0f) < 1e-3); // 100cm
    OCRelease(scalar); OCRelease(m); OCRelease(cm);
    if (error) OCRelease(error);
}

void test_SIScalarCreateByConvertingToUnit(void) {
    SIUnitRef m = meter_unit();
    SIUnitRef cm = cm_unit();
    SIScalarRef s_m = SIScalarCreateWithFloat(1.0f, m); // 1m
    OCStringRef error = NULL;
    SIScalarRef s_cm = SIScalarCreateByConvertingToUnit(s_m, cm, &error);
    assert(s_cm != NULL);
    assert(error == NULL);
    assert(fabsf(SIScalarFloatValue(s_cm) - 100.0f) < 1e-3); // 100cm
    OCRelease(s_m); OCRelease(s_cm); OCRelease(m); OCRelease(cm);
    if (error) OCRelease(error);
}

void test_SIScalarConvertToCoherentUnit(void) {
    SIUnitRef cm = cm_unit();
    SIMutableScalarRef scalar = SIScalarCreateMutableWithFloat(150.0f, cm); // 150cm
    OCStringRef error = NULL;
    bool success = SIScalarConvertToCoherentUnit(scalar, &error);
    assert(success);
    assert(error == NULL);
    assert(fabsf(SIScalarFloatValue(scalar) - 1.5f) < 1e-3); // 1.5m
    OCRelease(scalar); OCRelease(cm);
    if (error) OCRelease(error);
}

void test_SIScalarCreateByConvertingToCoherentUnit(void) {
    SIUnitRef cm = cm_unit();
    SIScalarRef s_cm = SIScalarCreateWithFloat(150.0f, cm); // 150cm
    OCStringRef error = NULL;
    SIScalarRef s_m = SIScalarCreateByConvertingToCoherentUnit(s_cm, &error);
    assert(s_m != NULL);
    assert(error == NULL);
    assert(fabsf(SIScalarFloatValue(s_m) - 1.5f) < 1e-3); // 1.5m
    OCRelease(s_cm); OCRelease(s_m); OCRelease(cm);
    if (error) OCRelease(error);
}

void test_SIScalarCreateByAdding(void) {
    SIUnitRef m = meter_unit();
    SIScalarRef s1 = SIScalarCreateWithFloat(1.0f, m);
    SIScalarRef s2 = SIScalarCreateWithFloat(2.5f, m);
    OCStringRef error = NULL;
    SIScalarRef sum = SIScalarCreateByAdding(s1, s2, &error);
    assert(sum != NULL);
    assert(error == NULL);
    assert(fabsf(SIScalarFloatValue(sum) - 3.5f) < 1e-6);
    OCRelease(s1); OCRelease(s2); OCRelease(sum); OCRelease(m);
    if (error) OCRelease(error);
}

void test_SIScalarAdd(void) {
    SIUnitRef m = meter_unit();
    SIMutableScalarRef target = SIScalarCreateMutableWithFloat(1.0f, m);
    SIScalarRef s2 = SIScalarCreateWithFloat(2.5f, m);
    OCStringRef error = NULL;
    bool success = SIScalarAdd(target, s2, &error);
    assert(success);
    assert(error == NULL);
    assert(fabsf(SIScalarFloatValue(target) - 3.5f) < 1e-6);
    OCRelease(target); OCRelease(s2); OCRelease(m);
    if (error) OCRelease(error);
}

void test_SIScalarCreateBySubtracting(void) {
    SIUnitRef m = meter_unit();
    SIScalarRef s1 = SIScalarCreateWithFloat(5.0f, m);
    SIScalarRef s2 = SIScalarCreateWithFloat(2.5f, m);
    OCStringRef error = NULL;
    SIScalarRef diff = SIScalarCreateBySubtracting(s1, s2, &error);
    assert(diff != NULL);
    assert(error == NULL);
    assert(fabsf(SIScalarFloatValue(diff) - 2.5f) < 1e-6);
    OCRelease(s1); OCRelease(s2); OCRelease(diff); OCRelease(m);
    if (error) OCRelease(error);
}

void test_SIScalarSubtract(void) {
    SIUnitRef m = meter_unit();
    SIMutableScalarRef target = SIScalarCreateMutableWithFloat(5.0f, m);
    SIScalarRef s2 = SIScalarCreateWithFloat(2.5f, m);
    OCStringRef error = NULL;
    bool success = SIScalarSubtract(target, s2, &error);
    assert(success);
    assert(error == NULL);
    assert(fabsf(SIScalarFloatValue(target) - 2.5f) < 1e-6);
    OCRelease(target); OCRelease(s2); OCRelease(m);
    if (error) OCRelease(error);
}

void test_SIScalarCreateByMultiplyingWithoutReducingUnit(void) {
    SIUnitRef m = meter_unit();
    SIUnitRef s_unit = second_unit();
    SIScalarRef s1 = SIScalarCreateWithFloat(2.0f, m); // 2 m
    SIScalarRef s2 = SIScalarCreateWithFloat(3.0f, s_unit); // 3 s
    OCStringRef error = NULL;
    SIScalarRef prod = SIScalarCreateByMultiplyingWithoutReducingUnit(s1, s2, &error);
    assert(prod != NULL);
    assert(error == NULL);
    assert(fabsf(SIScalarFloatValue(prod) - 6.0f) < 1e-6); // 6 m*s
    // Check unit string if possible, e.g. "m.s" or "m s"
    OCRelease(s1); OCRelease(s2); OCRelease(prod); OCRelease(m); OCRelease(s_unit);
    if (error) OCRelease(error);
}

void test_SIScalarMultiplyWithoutReducingUnit(void) {
    SIUnitRef m = meter_unit();
    SIUnitRef s_unit = second_unit();
    SIMutableScalarRef target = SIScalarCreateMutableWithFloat(2.0f, m); // 2 m
    SIScalarRef s2 = SIScalarCreateWithFloat(3.0f, s_unit); // 3 s
    OCStringRef error = NULL;
    bool success = SIScalarMultiplyWithoutReducingUnit(target, s2, &error);
    assert(success);
    assert(error == NULL);
    assert(fabsf(SIScalarFloatValue(target) - 6.0f) < 1e-6); // 6 m*s
    OCRelease(target); OCRelease(s2); OCRelease(m); OCRelease(s_unit);
    if (error) OCRelease(error);
}

void test_SIScalarCreateByMultiplying(void) {
    SIUnitRef m = meter_unit();
    SIUnitRef s_unit = second_unit();
    SIScalarRef s1 = SIScalarCreateWithFloat(2.0f, m); // 2 m
    SIScalarRef s2 = SIScalarCreateWithFloat(3.0f, s_unit); // 3 s
    OCStringRef error = NULL;
    SIScalarRef prod = SIScalarCreateByMultiplying(s1, s2, &error);
    assert(prod != NULL);
    assert(error == NULL);
    assert(fabsf(SIScalarFloatValue(prod) - 6.0f) < 1e-6); // 6 m*s
    OCRelease(s1); OCRelease(s2); OCRelease(prod); OCRelease(m); OCRelease(s_unit);
    if (error) OCRelease(error);
}

void test_SIScalarMultiply(void) {
    SIUnitRef m = meter_unit();
    SIUnitRef s_unit = second_unit();
    SIMutableScalarRef target = SIScalarCreateMutableWithFloat(2.0f, m); // 2 m
    SIScalarRef s2 = SIScalarCreateWithFloat(3.0f, s_unit); // 3 s
    OCStringRef error = NULL;
    bool success = SIScalarMultiply(target, s2, &error);
    assert(success);
    assert(error == NULL);
    assert(fabsf(SIScalarFloatValue(target) - 6.0f) < 1e-6); // 6 m*s
    OCRelease(target); OCRelease(s2); OCRelease(m); OCRelease(s_unit);
    if (error) OCRelease(error);
}

void test_SIScalarCreateByDividingWithoutReducingUnit(void) {
    SIUnitRef m = meter_unit();
    SIUnitRef s_unit = second_unit();
    SIScalarRef s1 = SIScalarCreateWithFloat(6.0f, m); // 6 m
    SIScalarRef s2 = SIScalarCreateWithFloat(2.0f, s_unit); // 2 s
    OCStringRef error = NULL;
    SIScalarRef quot = SIScalarCreateByDividingWithoutReducingUnit(s1, s2, &error);
    assert(quot != NULL);
    assert(error == NULL);
    assert(fabsf(SIScalarFloatValue(quot) - 3.0f) < 1e-6); // 3 m/s
    OCRelease(s1); OCRelease(s2); OCRelease(quot); OCRelease(m); OCRelease(s_unit);
    if (error) OCRelease(error);
}

void test_SIScalarDivideWithoutReducingUnit(void) {
    SIUnitRef m = meter_unit();
    SIUnitRef s_unit = second_unit();
    SIMutableScalarRef target = SIScalarCreateMutableWithFloat(6.0f, m); // 6 m
    SIScalarRef s2 = SIScalarCreateWithFloat(2.0f, s_unit); // 2 s
    OCStringRef error = NULL;
    bool success = SIScalarDivideWithoutReducingUnit(target, s2, &error);
    assert(success);
    assert(error == NULL);
    assert(fabsf(SIScalarFloatValue(target) - 3.0f) < 1e-6); // 3 m/s
    OCRelease(target); OCRelease(s2); OCRelease(m); OCRelease(s_unit);
    if (error) OCRelease(error);
}

void test_SIScalarCreateByDividing(void) {
    SIUnitRef m = meter_unit();
    SIUnitRef s_unit = second_unit();
    SIScalarRef s1 = SIScalarCreateWithFloat(6.0f, m); // 6 m
    SIScalarRef s2 = SIScalarCreateWithFloat(2.0f, s_unit); // 2 s
    OCStringRef error = NULL;
    SIScalarRef quot = SIScalarCreateByDividing(s1, s2, &error);
    assert(quot != NULL);
    assert(error == NULL);
    assert(fabsf(SIScalarFloatValue(quot) - 3.0f) < 1e-6); // 3 m/s
    OCRelease(s1); OCRelease(s2); OCRelease(quot); OCRelease(m); OCRelease(s_unit);
    if (error) OCRelease(error);
}

void test_SIScalarDivide(void) {
    SIUnitRef m = meter_unit();
    SIUnitRef s_unit = second_unit();
    SIMutableScalarRef target = SIScalarCreateMutableWithFloat(6.0f, m); // 6 m
    SIScalarRef s2 = SIScalarCreateWithFloat(2.0f, s_unit); // 2 s
    OCStringRef error = NULL;
    bool success = SIScalarDivide(target, s2, &error);
    assert(success);
    assert(error == NULL);
    assert(fabsf(SIScalarFloatValue(target) - 3.0f) < 1e-6); // 3 m/s
    OCRelease(target); OCRelease(s2); OCRelease(m); OCRelease(s_unit);
    if (error) OCRelease(error);
}

void test_SIScalarCreateByRaisingToAPowerWithoutReducingUnit(void) {
    SIUnitRef m = meter_unit();
    SIScalarRef s = SIScalarCreateWithFloat(2.0f, m); // 2 m
    OCStringRef error = NULL;
    SIScalarRef pow_s = SIScalarCreateByRaisingToAPowerWithoutReducingUnit(s, 3.0, &error);
    assert(pow_s != NULL);
    assert(error == NULL);
    assert(fabsf(SIScalarFloatValue(pow_s) - 8.0f) < 1e-6); // 8 m^3
    OCRelease(s); OCRelease(pow_s); OCRelease(m);
    if (error) OCRelease(error);
}

void test_SIScalarRaiseToAPowerWithoutReducingUnit(void) {
    SIUnitRef m = meter_unit();
    SIMutableScalarRef target = SIScalarCreateMutableWithFloat(2.0f, m); // 2 m
    OCStringRef error = NULL;
    bool success = SIScalarRaiseToAPowerWithoutReducingUnit(target, 3.0, &error);
    assert(success);
    assert(error == NULL);
    assert(fabsf(SIScalarFloatValue(target) - 8.0f) < 1e-6); // 8 m^3
    OCRelease(target); OCRelease(m);
    if (error) OCRelease(error);
}

void test_SIScalarCreateByRaisingToAPower(void) {
    SIUnitRef m = meter_unit();
    SIScalarRef s = SIScalarCreateWithFloat(2.0f, m); // 2 m
    OCStringRef error = NULL;
    SIScalarRef pow_s = SIScalarCreateByRaisingToAPower(s, 3.0, &error);
    assert(pow_s != NULL);
    assert(error == NULL);
    assert(fabsf(SIScalarFloatValue(pow_s) - 8.0f) < 1e-6); // 8 m^3
    OCRelease(s); OCRelease(pow_s); OCRelease(m);
    if (error) OCRelease(error);
}

void test_SIScalarRaiseToAPower(void) {
    SIUnitRef m = meter_unit();
    SIMutableScalarRef target = SIScalarCreateMutableWithFloat(2.0f, m); // 2 m
    OCStringRef error = NULL;
    bool success = SIScalarRaiseToAPower(target, 3.0, &error);
    assert(success);
    assert(error == NULL);
    assert(fabsf(SIScalarFloatValue(target) - 8.0f) < 1e-6); // 8 m^3
    OCRelease(target); OCRelease(m);
    if (error) OCRelease(error);
}

void test_SIScalarCreateByTakingAbsoluteValue(void) {
    SIUnitRef m = meter_unit();
    SIScalarRef s_neg = SIScalarCreateWithFloat(-5.0f, m);
    OCStringRef error = NULL;
    SIScalarRef s_abs = SIScalarCreateByTakingAbsoluteValue(s_neg, &error);
    assert(s_abs != NULL);
    assert(error == NULL);
    assert(fabsf(SIScalarFloatValue(s_abs) - 5.0f) < 1e-6);
    OCRelease(s_neg); OCRelease(s_abs); OCRelease(m);
    if (error) OCRelease(error);
}

void test_SIScalarTakeAbsoluteValue(void) {
    SIUnitRef m = meter_unit();
    SIMutableScalarRef target = SIScalarCreateMutableWithFloat(-5.0f, m);
    OCStringRef error = NULL;
    bool success = SIScalarTakeAbsoluteValue(target, &error);
    assert(success);
    assert(error == NULL);
    assert(fabsf(SIScalarFloatValue(target) - 5.0f) < 1e-6);
    OCRelease(target); OCRelease(m);
    if (error) OCRelease(error);
}

void test_SIScalarCreateByGammaFunctionWithoutReducingUnit(void) {
    // Gamma function tests are tricky due to unit changes and values.
    // For a dimensionless scalar with value 4, Gamma(4) = 3! = 6.
    SIUnitRef dimensionless = dimensionless_unit();
    SIScalarRef s = SIScalarCreateWithFloat(4.0f, dimensionless);
    OCStringRef error = NULL;
    SIScalarRef gamma_s = SIScalarCreateByGammaFunctionWithoutReducingUnit(s, &error);
    assert(gamma_s != NULL);
    assert(error == NULL);
    assert(fabsf(SIScalarFloatValue(gamma_s) - 6.0f) < 1e-5); // Gamma(4) = 6
    // Unit should remain dimensionless or be handled according to rules.
    OCRelease(s); OCRelease(gamma_s); OCRelease(dimensionless);
    if (error) OCRelease(error);
}

void test_SIScalarCreateByTakingNthRoot(void) {
    SIUnitRef m = meter_unit();
    double mult = 1.0;
    SIUnitRef m_cubed_unit = SIUnitByRaisingToPower(m, 3, &mult, NULL);

    SIScalarRef s = SIScalarCreateWithFloat(8.0f, m_cubed_unit); // 8 m^3
    OCStringRef error = NULL;
    SIScalarRef root_s = SIScalarCreateByTakingNthRoot(s, 3, &error); // Cube root
    assert(root_s != NULL);
    assert(error == NULL);
    assert(fabsf(SIScalarFloatValue(root_s) - 2.0f) < 1e-6); // 2 m
    // Check unit is 'm'
    OCStringRef unit_str = SIScalarCreateUnitString(root_s);
    assert(OCStringEqual(unit_str, STR("m")));
    OCRelease(unit_str);

    OCRelease(s); OCRelease(root_s); OCRelease(m); OCRelease(m_cubed_unit);
    if (error) OCRelease(error);
}

void test_SIScalarTakeNthRoot(void) {
    SIUnitRef m = meter_unit();
    double mult = 1.0;
    SIUnitRef m_cubed_unit = SIUnitByRaisingToPower(m, 3, &mult, NULL);

    SIMutableScalarRef target = SIScalarCreateMutableWithFloat(8.0f, m_cubed_unit); // 8 m^3
    OCStringRef error = NULL;
    bool success = SIScalarTakeNthRoot(target, 3, &error); // Cube root
    assert(success);
    assert(error == NULL);
    assert(fabsf(SIScalarFloatValue(target) - 2.0f) < 1e-6); // 2 m
    OCStringRef unit_str = SIScalarCreateUnitString(target);
    assert(OCStringEqual(unit_str, STR("m")));
    OCRelease(unit_str);

    OCRelease(target); OCRelease(m); OCRelease(m_cubed_unit);
    if (error) OCRelease(error);
}

void test_SIScalarTakeLog10(void) {
    // log10 is typically for dimensionless quantities or requires careful unit handling.
    // Assuming it makes the scalar dimensionless.
    SIUnitRef dimensionless = dimensionless_unit();
    SIMutableScalarRef target = SIScalarCreateMutableWithFloat(100.0f, dimensionless);
    OCStringRef error = NULL;
    bool success = SIScalarTakeLog10(target, &error);
    assert(success);
    assert(error == NULL);
    assert(fabsf(SIScalarFloatValue(target) - 2.0f) < 1e-6); // log10(100) = 2
    // Assert unit is dimensionless
    SIUnitRef final_unit = (SIUnitRef)SIQuantityGetUnit((SIQuantityRef)target);
    assert(SIUnitIsDimensionless(final_unit));

    OCRelease(target); OCRelease(dimensionless);
    if (error) OCRelease(error);
}

void test_SIScalarCreateByZeroingPart(void) {
    SIUnitRef u = meter_unit();
    SIScalarRef s_complex = SIScalarCreateWithFloatComplex(3.0f + 4.0f * I, u);
    SIScalarRef s_zero_imag = SIScalarCreateByZeroingPart(s_complex, kSIImaginaryPart);
    assert(s_zero_imag != NULL);
    assert(fabsf(crealf(SIScalarFloatComplexValue(s_zero_imag)) - 3.0f) < 1e-6);
    assert(fabsf(cimagf(SIScalarFloatComplexValue(s_zero_imag)) - 0.0f) < 1e-6);
    OCRelease(s_complex); OCRelease(s_zero_imag); OCRelease(u);
}

void test_SIScalarZeroPart(void) {
    SIUnitRef u = meter_unit();
    SIMutableScalarRef m = SIScalarCreateMutableWithFloatComplex(3.0f + 4.0f * I, u);
    bool success = SIScalarZeroPart(m, kSIImaginaryPart);
    assert(success);
    assert(fabsf(crealf(SIScalarFloatComplexValue(m)) - 3.0f) < 1e-6);
    assert(fabsf(cimagf(SIScalarFloatComplexValue(m)) - 0.0f) < 1e-6);
    OCRelease(m); OCRelease(u);
}

void test_SIScalarMultiplyByDimensionlessRealConstant(void) {
    SIUnitRef u = meter_unit();
    SIMutableScalarRef m = SIScalarCreateMutableWithFloat(5.0f, u);
    bool success = SIScalarMultiplyByDimensionlessRealConstant(m, 2.5);
    assert(success);
    assert(fabsf(SIScalarFloatValue(m) - 12.5f) < 1e-6);
    OCRelease(m); OCRelease(u);
}

void test_SIScalarCreateByMultiplyingByDimensionlessRealConstant(void) {
    SIUnitRef u = meter_unit();
    SIScalarRef s_orig = SIScalarCreateWithFloat(5.0f, u);
    SIScalarRef s_scaled = SIScalarCreateByMultiplyingByDimensionlessRealConstant(s_orig, 2.5);
    assert(s_scaled != NULL);
    assert(fabsf(SIScalarFloatValue(s_scaled) - 12.5f) < 1e-6);
    OCRelease(s_orig); OCRelease(s_scaled); OCRelease(u);
}

void test_SIScalarCreateByMultiplyingByDimensionlessComplexConstant(void) {
    SIUnitRef u = meter_unit();
    SIScalarRef s_orig = SIScalarCreateWithFloat(2.0f, u); // 2 m
    double complex factor = 1.0 + 1.0 * I;
    SIScalarRef s_scaled = SIScalarCreateByMultiplyingByDimensionlessComplexConstant(s_orig, factor);
    assert(s_scaled != NULL); // 2*(1+i) = 2+2i
    assert(fabsf(crealf(SIScalarFloatComplexValue(s_scaled)) - 2.0f) < 1e-6);
    assert(fabsf(cimagf(SIScalarFloatComplexValue(s_scaled)) - 2.0f) < 1e-6);
    OCRelease(s_orig); OCRelease(s_scaled); OCRelease(u);
}

void test_SIScalarMultiplyByDimensionlessComplexConstant(void) {
    SIUnitRef u = meter_unit();
    SIMutableScalarRef m = SIScalarCreateMutableWithFloat(2.0f, u); // 2 m
    double complex factor = 1.0 + 1.0 * I;
    bool success = SIScalarMultiplyByDimensionlessComplexConstant(m, factor);
    assert(success); // 2*(1+i) = 2+2i
    assert(fabsf(crealf(SIScalarFloatComplexValue(m)) - 2.0f) < 1e-6);
    assert(fabsf(cimagf(SIScalarFloatComplexValue(m)) - 2.0f) < 1e-6);
    OCRelease(m); OCRelease(u);
}

void test_SIScalarCreateByConjugation(void) {
    SIUnitRef u = meter_unit();
    SIScalarRef s_complex = SIScalarCreateWithFloatComplex(3.0f + 4.0f * I, u);
    SIScalarRef s_conj = SIScalarCreateByConjugation(s_complex);
    assert(s_conj != NULL);
    assert(fabsf(crealf(SIScalarFloatComplexValue(s_conj)) - 3.0f) < 1e-6);
    assert(fabsf(cimagf(SIScalarFloatComplexValue(s_conj)) - (-4.0f)) < 1e-6);
    OCRelease(s_complex); OCRelease(s_conj); OCRelease(u);
}

void test_SIScalarConjugate(void) {
    SIUnitRef u = meter_unit();
    SIMutableScalarRef m = SIScalarCreateMutableWithFloatComplex(3.0f + 4.0f * I, u);
    bool success = SIScalarConjugate(m);
    assert(success);
    assert(fabsf(crealf(SIScalarFloatComplexValue(m)) - 3.0f) < 1e-6);
    assert(fabsf(cimagf(SIScalarFloatComplexValue(m)) - (-4.0f)) < 1e-6);
    OCRelease(m); OCRelease(u);
}

void test_SIScalarShow(void) {
    SIUnitRef u = meter_unit();
    SIScalarRef s = SIScalarCreateWithFloat(1.23f, u);
    printf("Testing SIScalarShow: "); // Visual check for output
    SIScalarShow(s); // Just call it, ensure no crash
    printf("\n");
    OCRelease(s); OCRelease(u);
}

void test_SIScalarCreateStringValue(void) {
    SIUnitRef u = meter_unit();
    SIScalarRef s = SIScalarCreateWithFloat(1.23f, u);
    OCStringRef str_val = SIScalarCreateStringValue(s);
    assert(str_val != NULL);
    // OCStringShow(str_val); // Optional: print for visual check
    OCRelease(str_val); OCRelease(s); OCRelease(u);
}

void test_SIScalarCreateNumericStringValue(void) {
    SIUnitRef u = meter_unit();
    SIScalarRef s = SIScalarCreateWithFloat(1.23f, u);
    OCStringRef num_str_val = SIScalarCreateNumericStringValue(s);
    assert(num_str_val != NULL);
    // OCStringShow(num_str_val);
    OCRelease(num_str_val); OCRelease(s); OCRelease(u);
}

void test_SIScalarCreateStringValueForPart(void) {
    SIUnitRef u = meter_unit();
    SIScalarRef s = SIScalarCreateWithFloatComplex(1.0f + 2.0f * I, u);
    OCStringRef real_part_str = SIScalarCreateStringValueForPart(s, kSIRealPart);
    assert(real_part_str != NULL);
    // OCStringShow(real_part_str);
    OCRelease(real_part_str); OCRelease(s); OCRelease(u);
}

void test_SIScalarCreateUnitString(void) {
    SIUnitRef u = meter_unit();
    SIScalarRef s = SIScalarCreateWithFloat(1.0f, u);
    OCStringRef unit_str = SIScalarCreateUnitString(s);
    assert(unit_str != NULL);
    assert(OCStringEqual(unit_str, STR("m")));
    OCRelease(unit_str); OCRelease(s); OCRelease(u);
}

void test_SIScalarCreateStringValueWithFormat(void) {
    SIUnitRef u = meter_unit();
    SIScalarRef s = SIScalarCreateWithFloat(1.2345f, u);
    OCStringRef fmt = STR("%.2f %s");
    OCStringRef str_val = SIScalarCreateStringValueWithFormat(s, fmt);
    assert(str_val != NULL);
    // Check if it contains "1.23 m" or similar
    // OCStringShow(str_val);
    OCRelease(fmt); OCRelease(str_val); OCRelease(s); OCRelease(u);
}

void test_SIScalarCreateNumericStringValueWithFormat(void) {
    SIUnitRef u = meter_unit();
    SIScalarRef s = SIScalarCreateWithFloat(1.2345f, u);
    OCStringRef fmt = STR("%.2f");
    OCStringRef num_str_val = SIScalarCreateNumericStringValueWithFormat(s, fmt);
    assert(num_str_val != NULL);
    // Check if it contains "1.23"
    // OCStringShow(num_str_val);
    OCRelease(fmt); OCRelease(num_str_val); OCRelease(s); OCRelease(u);
}

void test_SIScalarAddToArrayAsStringValue(void) {
    SIUnitRef u = meter_unit();
    SIScalarRef s = SIScalarCreateWithFloat(1.0f, u);
    OCMutableArrayRef array = OCArrayCreateMutable(0, &kOCTypeArrayCallBacks);
    SIScalarAddToArrayAsStringValue(s, array);
    assert(OCArrayGetCount(array) == 1);
    OCStringRef str_in_array = (OCStringRef)OCArrayGetValueAtIndex(array, 0);
    assert(str_in_array != NULL);
    OCRelease(array);
    OCRelease(s);
    OCRelease(u);
}

void test_SIScalarIsReal(void) {
    SIUnitRef u = meter_unit();
    SIScalarRef s_real = SIScalarCreateWithFloat(1.0f, u);
    SIScalarRef s_complex = SIScalarCreateWithFloatComplex(1.0f + 0.001f * I, u);
    SIScalarRef s_pure_imag = SIScalarCreateWithFloatComplex(0.0f + 1.0f * I, u);
    assert(SIScalarIsReal(s_real));
    assert(!SIScalarIsReal(s_complex));
    assert(!SIScalarIsReal(s_pure_imag)); // Real part is zero, but imag is not
    OCRelease(s_real); OCRelease(s_complex); OCRelease(s_pure_imag); OCRelease(u);
}

void test_SIScalarIsImaginary(void) {
    SIUnitRef u = meter_unit();
    SIScalarRef s_real = SIScalarCreateWithFloat(1.0f, u);
    SIScalarRef s_complex = SIScalarCreateWithFloatComplex(0.001f + 1.0f * I, u);
    SIScalarRef s_pure_imag = SIScalarCreateWithFloatComplex(0.0f + 1.0f * I, u);
    SIScalarRef s_zero_imag = SIScalarCreateWithFloatComplex(1.0f + 0.0f * I, u);
    assert(!SIScalarIsImaginary(s_real));
    assert(!SIScalarIsImaginary(s_complex)); // Real part is non-zero
    assert(SIScalarIsImaginary(s_pure_imag));
    assert(!SIScalarIsImaginary(s_zero_imag)); // Imaginary part is zero
    OCRelease(s_real); OCRelease(s_complex); OCRelease(s_pure_imag); OCRelease(s_zero_imag); OCRelease(u);
}

void test_SIScalarIsComplex(void) {
    SIUnitRef u = meter_unit();
    SIScalarRef s_real = SIScalarCreateWithFloat(1.0f, u);
    SIScalarRef s_complex = SIScalarCreateWithFloatComplex(1.0f + 0.001f * I, u);
    SIScalarRef s_zero_imag = SIScalarCreateWithFloatComplex(1.0f + 0.0f * I, u);
    assert(!SIScalarIsComplex(s_real));
    assert(SIScalarIsComplex(s_complex));
    assert(!SIScalarIsComplex(s_zero_imag)); // Imaginary part is zero, so not complex by this definition
    OCRelease(s_real); OCRelease(s_complex); OCRelease(s_zero_imag); OCRelease(u);
}

void test_SIScalarIsZero(void) {
    SIUnitRef u = meter_unit();
    SIScalarRef s_zero_f = SIScalarCreateWithFloat(0.0f, u);
    SIScalarRef s_zero_d = SIScalarCreateWithDouble(0.0, u);
    SIScalarRef s_zero_fc = SIScalarCreateWithFloatComplex(0.0f + 0.0f * I, u);
    SIScalarRef s_zero_dc = SIScalarCreateWithDoubleComplex(0.0 + 0.0 * I, u);
    SIScalarRef s_non_zero = SIScalarCreateWithFloat(0.001f, u);
    assert(SIScalarIsZero(s_zero_f));
    assert(SIScalarIsZero(s_zero_d));
    assert(SIScalarIsZero(s_zero_fc));
    assert(SIScalarIsZero(s_zero_dc));
    assert(!SIScalarIsZero(s_non_zero));
    OCRelease(s_zero_f); OCRelease(s_zero_d); OCRelease(s_zero_fc); OCRelease(s_zero_dc); OCRelease(s_non_zero); OCRelease(u);
}

void test_SIScalarIsInfinite(void) {
    SIUnitRef u = meter_unit();
    SIScalarRef s_inf_f = SIScalarCreateWithFloat(INFINITY, u);
    SIScalarRef s_inf_d = SIScalarCreateWithDouble(INFINITY, u);
    SIScalarRef s_inf_fc_real = SIScalarCreateWithFloatComplex(INFINITY + 0.0f * I, u);
    SIScalarRef s_inf_fc_imag = SIScalarCreateWithFloatComplex(0.0f + INFINITY * I, u);
    SIScalarRef s_finite = SIScalarCreateWithFloat(1.0f, u);
    assert(SIScalarIsInfinite(s_inf_f));
    assert(SIScalarIsInfinite(s_inf_d));
    assert(SIScalarIsInfinite(s_inf_fc_real));
    assert(SIScalarIsInfinite(s_inf_fc_imag));
    assert(!SIScalarIsInfinite(s_finite));
    OCRelease(s_inf_f); OCRelease(s_inf_d); OCRelease(s_inf_fc_real); OCRelease(s_inf_fc_imag); OCRelease(s_finite); OCRelease(u);
}

void test_SIScalarIsRealNonNegativeInteger(void) {
    SIUnitRef u = meter_unit(); // Unit doesn't matter for this property of the value
    SIScalarRef s_int_pos = SIScalarCreateWithFloat(5.0f, u);
    SIScalarRef s_int_zero = SIScalarCreateWithFloat(0.0f, u);
    SIScalarRef s_float = SIScalarCreateWithFloat(5.1f, u);
    SIScalarRef s_neg_int = SIScalarCreateWithFloat(-5.0f, u);
    SIScalarRef s_complex = SIScalarCreateWithFloatComplex(5.0f + 1.0f * I, u);
    assert(SIScalarIsRealNonNegativeInteger(s_int_pos));
    assert(SIScalarIsRealNonNegativeInteger(s_int_zero));
    assert(!SIScalarIsRealNonNegativeInteger(s_float));
    assert(!SIScalarIsRealNonNegativeInteger(s_neg_int));
    assert(!SIScalarIsRealNonNegativeInteger(s_complex));
    OCRelease(s_int_pos); OCRelease(s_int_zero); OCRelease(s_float); OCRelease(s_neg_int); OCRelease(s_complex); OCRelease(u);
}

void test_SIScalarValidateProposedStringValue(void) {
    SIUnitRef m = meter_unit();
    SIScalarRef s_meter = SIScalarCreateWithFloat(1.0f, m);
    OCStringRef valid_str = STR("10 cm"); // cm has same dimensionality as m
    OCStringRef invalid_dim_str = STR("10 kg"); // kg has different dimensionality
    OCStringRef invalid_fmt_str = STR("ten meters"); // bad format
    OCStringRef error = NULL;

    assert(SIScalarValidateProposedStringValue(s_meter, valid_str, &error));
    assert(error == NULL);

    assert(!SIScalarValidateProposedStringValue(s_meter, invalid_dim_str, &error));
    assert(error != NULL); OCRelease(error); error = NULL;

    assert(!SIScalarValidateProposedStringValue(s_meter, invalid_fmt_str, &error));
    assert(error != NULL); OCRelease(error); error = NULL;

    OCRelease(s_meter); OCRelease(m);
    OCRelease(valid_str); OCRelease(invalid_dim_str); OCRelease(invalid_fmt_str);
}

void test_SIScalarEqual(void) {
    SIUnitRef m = meter_unit();
    SIScalarRef s1 = SIScalarCreateWithFloat(1.0f, m);
    SIScalarRef s2 = SIScalarCreateWithFloat(1.0f, m);
    SIScalarRef s3 = SIScalarCreateWithFloat(1.1f, m);
    SIUnitRef cm = cm_unit();
    SIScalarRef s4 = SIScalarCreateWithFloat(1.0f, cm); // Different unit

    assert(SIScalarEqual(s1, s2));
    assert(!SIScalarEqual(s1, s3));
    assert(!SIScalarEqual(s1, s4)); // Should be false due to different unit object, even if value is same type

    OCRelease(s1); OCRelease(s2); OCRelease(s3); OCRelease(s4);
    OCRelease(m); OCRelease(cm);
}

void test_SIScalarCompare(void) {
    SIUnitRef m = meter_unit();
    SIUnitRef cm = cm_unit();
    SIScalarRef s_1m = SIScalarCreateWithFloat(1.0f, m);
    SIScalarRef s_2m = SIScalarCreateWithFloat(2.0f, m);
    SIScalarRef s_100cm = SIScalarCreateWithFloat(100.0f, cm);
    SIUnitRef kg = kilogram_unit();
    SIScalarRef s_1kg = SIScalarCreateWithFloat(1.0f, kg);

    assert(SIScalarCompare(s_1m, s_2m) == kOCCompareLessThan);
    assert(SIScalarCompare(s_2m, s_1m) == kOCCompareGreaterThan);
    assert(SIScalarCompare(s_1m, s_100cm) == kOCCompareEqualTo); // 1m == 100cm
    assert(SIScalarCompare(s_1m, s_1kg) == kOCCompareUnequalDimensionalities);

    OCRelease(s_1m); OCRelease(s_2m); OCRelease(s_100cm); OCRelease(s_1kg);
    OCRelease(m); OCRelease(cm); OCRelease(kg);
}

void test_SIScalarCompareReduced(void) {
    // Similar to Compare, but ensures units are reduced first.
    // Example: 1 m vs 1000 mm (after reduction, mm becomes m)
    SIUnitRef m = meter_unit();
    double mult = 1.0;
    SIUnitRef mm_unit = SIUnitForSymbol(STR("mm"), &mult, NULL); // millimeter
    SIScalarRef s_1m = SIScalarCreateWithFloat(1.0f, m);
    SIScalarRef s_1000mm = SIScalarCreateWithFloat(1000.0f, mm_unit);

    assert(SIScalarCompareReduced(s_1m, s_1000mm) == kOCCompareEqualTo);

    OCRelease(s_1m); OCRelease(s_1000mm); OCRelease(m); OCRelease(mm_unit);
}

void test_SIScalarCompareLoose(void) {
    SIUnitRef m = meter_unit();
    SIScalarRef s1 = SIScalarCreateWithDouble(1.000000001, m);
    SIScalarRef s2 = SIScalarCreateWithDouble(1.0, m);
    SIScalarRef s3 = SIScalarCreateWithDouble(1.001, m);

    assert(SIScalarCompare(s1,s2) == kOCCompareGreaterThan); // Exact compare
    assert(SIScalarCompareLoose(s1, s2) == kOCCompareEqualTo); // Loose compare
    assert(SIScalarCompareLoose(s1, s3) == kOCCompareLessThan);

    OCRelease(s1); OCRelease(s2); OCRelease(s3); OCRelease(m);
}

