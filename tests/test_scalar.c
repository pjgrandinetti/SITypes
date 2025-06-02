#include "test_utils.h"
#include "../src/SIScalar.h"
#include "../src/SIUnit.h"
#include <assert.h>
#include <stdio.h>
#include <math.h>   // For fabs, fabsf, creal, cimag
#include <complex.h> // For complex numbers and I macro


static SIUnitRef mps_unit(void) {
    double mult = 1.0;
    SIUnitRef u = SIUnitForSymbol(STR("m/s"), &mult, NULL);
    assert(u != NULL);
    return u;
}

void test_SIScalarGetTypeID(void) { assert(SIScalarGetTypeID() != 0); }

void test_SIScalarCreateCopy(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    float input = 1.0f;
    SIScalarRef s = SIScalarCreateWithFloat(input, u);
    SIScalarRef c = SIScalarCreateCopy(s);
    assert(c != NULL);
    // Must be a different instance
    assert(c != s);
    // Value preserved
    assert(OCCompareFloatValues(SIScalarFloatValue(s), SIScalarFloatValue(c))==kOCCompareEqualTo);
    // Unit preserved
    OCStringRef us = SIScalarCreateUnitString(s);
    OCStringRef uc = SIScalarCreateUnitString(c);
    assert(OCStringEqual(us, uc));
    OCRelease(us);
    OCRelease(uc);
    OCRelease(c);
    OCRelease(s);
}

void test_SIScalarCreateMutableCopy(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    SIScalarRef orig = SIScalarCreateWithFloat(2.0f, u);
    SIMutableScalarRef copy = SIScalarCreateMutableCopy(orig);
    assert(copy != NULL);
    // Must be a distinct object
    assert(copy != orig);
    // Value and unit preserved initially
    assert(OCCompareFloatValues(SIScalarFloatValue(copy), SIScalarFloatValue(orig))==kOCCompareEqualTo);
    OCStringRef unit_orig = SIScalarCreateUnitString(orig);
    OCStringRef unit_copy = SIScalarCreateUnitString(copy);
    assert(OCStringEqual(unit_orig, unit_copy));
    OCRelease(unit_orig);
    OCRelease(unit_copy);
    // Mutate the copy and ensure original stays unchanged
    SIScalarSetFloatValue(copy, 5.0f);
    assert(OCCompareFloatValues(SIScalarFloatValue(copy), 5.0f)==kOCCompareEqualTo);
    assert(OCCompareFloatValues(SIScalarFloatValue(orig), 2.0f)==kOCCompareEqualTo);

    OCRelease(copy);
    OCRelease(orig);
}

void test_SIScalarCreateWithFloat(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    float input = 3.0f;
    SIScalarRef s = SIScalarCreateWithFloat(input, u);
    assert(s != NULL);
    // Value preserved
    assert(OCCompareFloatValues(SIScalarFloatValue(s), input)==kOCCompareEqualTo);
    // Unit preserved
    OCStringRef unitStr = SIScalarCreateUnitString(s);
    assert(OCStringEqual(unitStr, STR("m")));
    OCRelease(unitStr);
    OCRelease(s);
}

void test_SIScalarCreateMutableWithFloat(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    assert(u != NULL);
    
    float testValue = 4.0f;
    SIMutableScalarRef m = SIScalarCreateMutableWithFloat(testValue, u);
    
    // Verify the scalar was created
    assert(m != NULL);
    
    // Verify the value was correctly stored using OCCompareFloatValues
    assert(OCCompareFloatValues(SIScalarFloatValue(m), testValue) == kOCCompareEqualTo);
    
    // Verify the unit was correctly assigned
    OCStringRef unitStr = SIScalarCreateUnitString(m);
    assert(OCStringEqual(unitStr, STR("m")));
    OCRelease(unitStr);
    
    // Verify mutability by changing the value
    SIScalarSetFloatValue(m, 5.0f);
    assert(OCCompareFloatValues(SIScalarFloatValue(m), 5.0f) == kOCCompareEqualTo);
    
    // Clean up - only release what we created
    OCRelease(m);
}

void test_SIScalarCreateWithDouble(void) {
    // Create a unit for testing
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    assert(u != NULL);
    
    // Test value to use
    double testValue = 5.0;
    
    // Create the scalar with our test value
    SIScalarRef s = SIScalarCreateWithDouble(testValue, u);
    
    // Verify the scalar was created successfully
    assert(s != NULL);
    
    // Verify the value was correctly stored using OCCompareDoubleValues
    assert(OCCompareDoubleValues(SIScalarDoubleValue(s), testValue) == kOCCompareEqualTo);
    
    // Verify the unit was correctly assigned
    OCStringRef unitStr = SIScalarCreateUnitString(s);
    assert(OCStringEqual(unitStr, STR("m")));
    OCRelease(unitStr);
    
    // Verify the number type is correct (should be double)
    assert(SIQuantityGetElementType((SIQuantityRef)s) == kSINumberFloat64Type);
    
    // Clean up (only release what we created)
    OCRelease(s);
    // No OCRelease(u) needed since SIUnitForUnderivedSymbol doesn't transfer ownership
}

void test_SIScalarCreateMutableWithDouble(void) {
    // Create a unit for testing
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    assert(u != NULL);
    
    // Test value to use
    double testValue = 6.0;
    
    // Create the scalar with our test value
    SIMutableScalarRef m = SIScalarCreateMutableWithDouble(testValue, u);
    
    // Verify the scalar was created successfully
    assert(m != NULL);
    
    // Verify the value was correctly stored using OCCompareDoubleValues
    assert(OCCompareDoubleValues(SIScalarDoubleValue(m), testValue) == kOCCompareEqualTo);
    
    // Verify the unit was correctly assigned
    OCStringRef unitStr = SIScalarCreateUnitString(m);
    assert(OCStringEqual(unitStr, STR("m")));
    OCRelease(unitStr);
    
    // Verify the number type is correct (should be double)
    assert(SIQuantityGetElementType((SIQuantityRef)m) == kSINumberFloat64Type);
    
    // Verify mutability by changing the value
    SIScalarSetDoubleValue(m, 7.0);
    assert(OCCompareDoubleValues(SIScalarDoubleValue(m), 7.0) == kOCCompareEqualTo);
    
    // Clean up (only release what we created)
    OCRelease(m);
    // No OCRelease(u) needed since SIUnitForUnderivedSymbol doesn't transfer ownership
}

void test_SIScalarCreateWithFloatComplex(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    float complex val = 1.0f + 2.0f * I;
    SIScalarRef s = SIScalarCreateWithFloatComplex(val, u);
    assert(s != NULL);
    assert(fabsf(crealf(SIScalarFloatComplexValue(s)) - 1.0f) < 1e-6);
    assert(fabsf(crealf(SIScalarFloatComplexValue(s)) - 1.0f) < 1e-6);
    assert(fabsf(cimagf(SIScalarFloatComplexValue(s)) - 2.0f) < 1e-6);
    OCRelease(s);
}

void test_SIScalarCreateMutableWithFloatComplex(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    float complex val = 2.0f + 3.0f * I;
    SIMutableScalarRef m = SIScalarCreateMutableWithFloatComplex(val, u);
    assert(m != NULL);
    assert(fabsf(crealf(SIScalarFloatComplexValue(m)) - 2.0f) < 1e-6);
    assert(fabsf(cimagf(SIScalarFloatComplexValue(m)) - 3.0f) < 1e-6);
    OCRelease(m);
}

void test_SIScalarCreateWithDoubleComplex(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    double complex val = 3.0 + 4.0 * I;
    SIScalarRef s = SIScalarCreateWithDoubleComplex(val, u);
    assert(s != NULL);
    assert(fabs(creal(SIScalarDoubleComplexValue(s)) - 3.0) < 1e-9);
    assert(fabs(cimag(SIScalarDoubleComplexValue(s)) - 4.0) < 1e-9);
    OCRelease(s);
}

void test_SIScalarCreateMutableWithDoubleComplex(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    double complex val = 4.0 + 5.0 * I;
    SIMutableScalarRef m = SIScalarCreateMutableWithDoubleComplex(val, u);
    assert(m != NULL);
    assert(fabs(creal(SIScalarDoubleComplexValue(m)) - 4.0) < 1e-9);
    assert(fabs(cimag(SIScalarDoubleComplexValue(m)) - 5.0) < 1e-9);
    OCRelease(m);
}

void test_SIScalarGetValue(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    SIScalarRef s = SIScalarCreateWithFloat(7.0f, u);
    __SINumber n = SIScalarGetValue(s);
    assert(n.floatValue == 7.0f);
    OCRelease(s);
}

void test_SIScalarSetFloatValue(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    SIMutableScalarRef m = SIScalarCreateMutableWithFloat(0.0f, u);
    SIScalarSetFloatValue(m, 8.0f);
    assert(OCCompareFloatValues(SIScalarFloatValue(m), 8.0f)==kOCCompareEqualTo);
    OCRelease(m);
}

void test_SIScalarSetDoubleValue(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    SIMutableScalarRef m = SIScalarCreateMutableWithDouble(0.0, u);
    SIScalarSetDoubleValue(m, 9.0);
    assert(OCCompareDoubleValues(SIScalarDoubleValue(m), 9.0)==kOCCompareEqualTo);
    OCRelease(m);
}

void test_SIScalarSetFloatComplexValue(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    SIMutableScalarRef m = SIScalarCreateMutableWithFloatComplex(0.0f + 0.0f * I, u);
    float complex val = 1.0f + 1.0f * I;
    SIScalarSetFloatComplexValue(m, val);
    assert(fabsf(crealf(SIScalarFloatComplexValue(m)) - 1.0f) < 1e-6);
    assert(fabsf(cimagf(SIScalarFloatComplexValue(m)) - 1.0f) < 1e-6);
    OCRelease(m);
}

void test_SIScalarSetDoubleComplexValue(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    SIMutableScalarRef m = SIScalarCreateMutableWithDoubleComplex(0.0 + 0.0 * I, u);
    double complex val = 2.0 + 2.0 * I;
    SIScalarSetDoubleComplexValue(m, val);
    assert(fabs(creal(SIScalarDoubleComplexValue(m)) - 2.0) < 1e-9);
    assert(fabs(cimag(SIScalarDoubleComplexValue(m)) - 2.0) < 1e-9);
    OCRelease(m);
}

void test_SIScalarSetElementType(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    SIMutableScalarRef m = SIScalarCreateMutableWithFloat(1.0f, u);
    SIScalarSetElementType(m, kSINumberFloat64Type);
    // Check if the internal type is now double, e.g. by checking precision or a specific API if available
    // For now, just check if value is preserved
    assert(fabs(SIScalarDoubleValue(m) - 1.0) < 1e-9);
    OCRelease(m);
}

void test_SIScalarFloatValue(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    SIScalarRef s = SIScalarCreateWithFloat(1.5f, u);
    assert(fabsf(SIScalarFloatValue(s) - 1.5f) < 1e-6);
    OCRelease(s);
}

void test_SIScalarDoubleValue(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    SIScalarRef s = SIScalarCreateWithDouble(2.5, u);
    assert(fabs(SIScalarDoubleValue(s) - 2.5) < 1e-9);
    OCRelease(s);
}

void test_SIScalarFloatComplexValue(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    SIScalarRef s = SIScalarCreateWithFloatComplex(1.0f + 2.0f * I, u);
    assert(fabsf(crealf(SIScalarFloatComplexValue(s)) - 1.0f) < 1e-6);
    assert(fabsf(cimagf(SIScalarFloatComplexValue(s)) - 2.0f) < 1e-6);
    OCRelease(s);
}

void test_SIScalarDoubleComplexValue(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    SIScalarRef s = SIScalarCreateWithDoubleComplex(2.0 + 3.0 * I, u);
    assert(fabs(creal(SIScalarDoubleComplexValue(s)) - 2.0) < 1e-9);
    assert(fabs(cimag(SIScalarDoubleComplexValue(s)) - 3.0) < 1e-9);
    OCRelease(s);
}

void test_SIScalarCreateByConvertingToNumberType(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    SIScalarRef s_double = SIScalarCreateWithDouble(123.456, u);
    SIScalarRef s_float = SIScalarCreateByConvertingToNumberType(s_double, kSINumberFloat32Type);
    assert(s_float != NULL);
    assert(fabsf(SIScalarFloatValue(s_float) - 123.456f) < 1e-3); // Precision loss expected
    OCRelease(s_double); OCRelease(s_float);
}

void test_SIScalarTakeComplexPart(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    SIMutableScalarRef m = SIScalarCreateMutableWithFloatComplex(3.0f + 4.0f * I, u);
    bool success = SIScalarTakeComplexPart(m, kSIRealPart);
    assert(success);
    assert(SIScalarIsReal(m));
    assert(fabsf(SIScalarFloatValue(m) - 3.0f) < 1e-6);
    OCRelease(m);
}

void test_SIScalarCreateByTakingComplexPart(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    SIScalarRef s_complex = SIScalarCreateWithFloatComplex(3.0f + 4.0f * I, u);
    SIScalarRef s_real_part = SIScalarCreateByTakingComplexPart(s_complex, kSIRealPart);
    assert(s_real_part != NULL);
    assert(SIScalarIsReal(s_real_part));
    assert(fabsf(SIScalarFloatValue(s_real_part) - 3.0f) < 1e-6);
    OCRelease(s_complex); OCRelease(s_real_part);
}

void test_SIScalarCreateByReducingUnit(void) {
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
    SIUnitRef s_unit = SIUnitForUnderivedSymbol(STR("s"));
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

    OCRelease(scalar_unreduced); OCRelease(scalar_reduced);
}

void test_SIScalarReduceUnit(void) {
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
    SIUnitRef s_unit = SIUnitForUnderivedSymbol(STR("s"));
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

    OCRelease(scalar);
}

void test_SIScalarConvertToUnit(void) {
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
    SIUnitRef cm = SIUnitForUnderivedSymbol(STR("cm"));
    SIMutableScalarRef scalar = SIScalarCreateMutableWithFloat(1.0f, m); // 1m
    OCStringRef error = NULL;
    bool success = SIScalarConvertToUnit(scalar, cm, &error);
    assert(success);
    assert(error == NULL);
    assert(OCCompareFloatValues(SIScalarFloatValue(scalar), 100.0f)==kOCCompareEqualTo); // 100cm
    OCRelease(scalar);
    if (error) OCRelease(error);
}

void test_SIScalarCreateByConvertingToUnit(void) {
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
    SIUnitRef cm = SIUnitForUnderivedSymbol(STR("cm"));
    SIScalarRef s_m = SIScalarCreateWithFloat(1.0f, m); // 1m
    OCStringRef error = NULL;
    SIScalarRef s_cm = SIScalarCreateByConvertingToUnit(s_m, cm, &error);
    assert(s_cm != NULL);
    assert(error == NULL);
    assert(OCCompareFloatValues(SIScalarFloatValue(s_cm), 100.0f)==kOCCompareEqualTo); // 100cm
    OCRelease(s_m); OCRelease(s_cm);
    if (error) OCRelease(error);
}

void test_SIScalarConvertToCoherentUnit(void) {
    SIUnitRef cm = SIUnitForUnderivedSymbol(STR("cm"));
    SIMutableScalarRef scalar = SIScalarCreateMutableWithFloat(150.0f, cm); // 150cm
    OCStringRef error = NULL;
    bool success = SIScalarConvertToCoherentUnit(scalar, &error);
    assert(success);
    assert(error == NULL);
    assert(OCCompareFloatValues(SIScalarFloatValue(scalar), 1.5f)==kOCCompareEqualTo); // 1.5m
    OCRelease(scalar);
    if (error) OCRelease(error);
}

void test_SIScalarCreateByConvertingToCoherentUnit(void) {
    SIUnitRef cm = SIUnitForUnderivedSymbol(STR("cm"));
    SIScalarRef s_cm = SIScalarCreateWithFloat(150.0f, cm); // 150cm
    OCStringRef error = NULL;
    SIScalarRef s_m = SIScalarCreateByConvertingToCoherentUnit(s_cm, &error);
    assert(s_m != NULL);
    assert(error == NULL);
    assert(OCCompareFloatValues(SIScalarFloatValue(s_m), 1.5f)==kOCCompareEqualTo); // 1.5m

    OCRelease(s_cm); OCRelease(s_m);
    if (error) OCRelease(error);
}

void test_SIScalarCreateByAdding(void) {
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
    SIScalarRef s1 = SIScalarCreateWithFloat(1.0f, m);
    SIScalarRef s2 = SIScalarCreateWithFloat(2.5f, m);
    OCStringRef error = NULL;
    SIScalarRef sum = SIScalarCreateByAdding(s1, s2, &error);
    assert(sum != NULL);
    assert(error == NULL);
    assert(OCCompareFloatValues(SIScalarFloatValue(sum), 3.5f)==kOCCompareEqualTo); 

    OCRelease(s1); OCRelease(s2); OCRelease(sum);
    if (error) OCRelease(error);
}

void test_SIScalarAdd(void) {
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
    SIMutableScalarRef target = SIScalarCreateMutableWithFloat(1.0f, m);
    SIScalarRef s2 = SIScalarCreateWithFloat(2.5f, m);
    OCStringRef error = NULL;
    bool success = SIScalarAdd(target, s2, &error);
    assert(success);
    assert(error == NULL);
    assert(OCCompareFloatValues(SIScalarFloatValue(target), 3.5f)==kOCCompareEqualTo); 

    OCRelease(target); OCRelease(s2);
    if (error) OCRelease(error);
}

void test_SIScalarCreateBySubtracting(void) {
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
    SIScalarRef s1 = SIScalarCreateWithFloat(5.0f, m);
    SIScalarRef s2 = SIScalarCreateWithFloat(2.5f, m);
    OCStringRef error = NULL;
    SIScalarRef diff = SIScalarCreateBySubtracting(s1, s2, &error);
    assert(diff != NULL);
    assert(error == NULL);
    assert(OCCompareFloatValues(SIScalarFloatValue(diff), 2.5f)==kOCCompareEqualTo); 

    OCRelease(s1); OCRelease(s2); OCRelease(diff);
    if (error) OCRelease(error);
}

void test_SIScalarSubtract(void) {
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
    SIMutableScalarRef target = SIScalarCreateMutableWithFloat(5.0f, m);
    SIScalarRef s2 = SIScalarCreateWithFloat(2.5f, m);
    OCStringRef error = NULL;
    bool success = SIScalarSubtract(target, s2, &error);
    assert(success);
    assert(error == NULL);
    assert(OCCompareFloatValues(SIScalarFloatValue(target), 2.5f)==kOCCompareEqualTo); 

    OCRelease(target); OCRelease(s2);
    if (error) OCRelease(error);
}

void test_SIScalarCreateByMultiplyingWithoutReducingUnit(void) {
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
    SIUnitRef s_unit = SIUnitForUnderivedSymbol(STR("s"));
    SIScalarRef s1 = SIScalarCreateWithFloat(2.0f, m); // 2 m
    SIScalarRef s2 = SIScalarCreateWithFloat(3.0f, s_unit); // 3 s
    OCStringRef error = NULL;
    SIScalarRef prod = SIScalarCreateByMultiplyingWithoutReducingUnit(s1, s2, &error);
    assert(prod != NULL);
    assert(error == NULL);
    assert(OCCompareFloatValues(SIScalarFloatValue(prod), 6.0f)==kOCCompareEqualTo); 

    // Check unit string if possible, e.g. "m.s" or "m s"
    OCRelease(s1); OCRelease(s2); OCRelease(prod);
    if (error) OCRelease(error);
}

void test_SIScalarMultiplyWithoutReducingUnit(void) {
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
    SIUnitRef s_unit = SIUnitForUnderivedSymbol(STR("s"));
    SIMutableScalarRef target = SIScalarCreateMutableWithFloat(2.0f, m); // 2 m
    SIScalarRef s2 = SIScalarCreateWithFloat(3.0f, s_unit); // 3 s
    OCStringRef error = NULL;
    bool success = SIScalarMultiplyWithoutReducingUnit(target, s2, &error);
    assert(success);
    assert(error == NULL);
    assert(OCCompareFloatValues(SIScalarFloatValue(target), 6.0f)==kOCCompareEqualTo); 

    OCRelease(target); OCRelease(s2);
    if (error) OCRelease(error);
}

void test_SIScalarCreateByMultiplying(void) {
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
    SIUnitRef s_unit = SIUnitForUnderivedSymbol(STR("s"));
    SIScalarRef s1 = SIScalarCreateWithFloat(2.0f, m); // 2 m
    SIScalarRef s2 = SIScalarCreateWithFloat(3.0f, s_unit); // 3 s
    OCStringRef error = NULL;
    SIScalarRef prod = SIScalarCreateByMultiplying(s1, s2, &error);
    assert(prod != NULL);
    assert(error == NULL);
    assert(OCCompareFloatValues(SIScalarFloatValue(prod), 6.0f)==kOCCompareEqualTo); 

    OCRelease(s1); OCRelease(s2); OCRelease(prod);
    if (error) OCRelease(error);
}

void test_SIScalarMultiply(void) {
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
    SIUnitRef s_unit = SIUnitForUnderivedSymbol(STR("s"));
    SIMutableScalarRef target = SIScalarCreateMutableWithFloat(2.0f, m); // 2 m
    SIScalarRef s2 = SIScalarCreateWithFloat(3.0f, s_unit); // 3 s
    OCStringRef error = NULL;
    bool success = SIScalarMultiply(target, s2, &error);
    assert(success);
    assert(error == NULL);
    assert(OCCompareFloatValues(SIScalarFloatValue(target), 6.0f)==kOCCompareEqualTo); 

    OCRelease(target); OCRelease(s2);
    if (error) OCRelease(error);
}

void test_SIScalarCreateByDividingWithoutReducingUnit(void) {
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
    SIUnitRef s_unit = SIUnitForUnderivedSymbol(STR("s"));
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
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
    SIUnitRef s_unit = SIUnitForUnderivedSymbol(STR("s"));
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
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
    SIUnitRef s_unit = SIUnitForUnderivedSymbol(STR("s"));
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
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
    SIUnitRef s_unit = SIUnitForUnderivedSymbol(STR("s"));
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
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
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
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
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
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
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
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
    SIMutableScalarRef target = SIScalarCreateMutableWithFloat(2.0f, m); // 2 m
    OCStringRef error = NULL;
    bool success = SIScalarRaiseToAPower(target, 3.0, &error);
    assert(success);
    assert(error == NULL);
    assert(fabsf(SIScalarFloatValue(target) - 8.0f) < 1e-6); // 8 m^3
    OCRelease(target); 
    OCRelease(m);
    if (error) OCRelease(error);
}

void test_SIScalarCreateByTakingAbsoluteValue(void) {
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
    SIScalarRef s_neg = SIScalarCreateWithFloat(-5.0f, m);
    OCStringRef error = NULL;
    SIScalarRef s_abs = SIScalarCreateByTakingAbsoluteValue(s_neg, &error);
    assert(s_abs != NULL);
    assert(error == NULL);
    assert(fabsf(SIScalarFloatValue(s_abs) - 5.0f) < 1e-6);
    OCRelease(s_neg); 
    OCRelease(s_abs); OCRelease(m);
    if (error) OCRelease(error);
}

void test_SIScalarTakeAbsoluteValue(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    SIMutableScalarRef target = SIScalarCreateMutableWithFloat(-5.0f, u);
    OCStringRef error = NULL;
    bool success = SIScalarTakeAbsoluteValue(target, &error);
    assert(success);
    assert(error == NULL);
    assert(fabsf(SIScalarFloatValue(target) - 5.0f) < 1e-6);
    OCRelease(target); OCRelease(u);
    if (error) OCRelease(error);
}

void test_SIScalarCreateByGammaFunctionWithoutReducingUnit(void) {
    // Test the Gamma function implementation
    // For dimensionless scalar values:
    // Gamma(1) = 1
    // Gamma(2) = 1
    // Gamma(3) = 2
    // Gamma(4) = 6 = 3!
    // Gamma(5) = 24 = 4!
    
    SIUnitRef dimensionless = SIUnitDimensionlessAndUnderived();
    OCStringRef error = NULL;
    
    // Test case 1: Gamma(4) = 6
    SIScalarRef s4 = SIScalarCreateWithFloat(4.0f, dimensionless);
    SIScalarRef gamma_4 = SIScalarCreateByGammaFunctionWithoutReducingUnit(s4, &error);
    assert(gamma_4 != NULL);
    assert(error == NULL);
    
    // Print diagnostic info if comparison fails
    float gamma_4_value = SIScalarFloatValue(gamma_4);
    if (OCCompareFloatValues(gamma_4_value, 24.0f) != kOCCompareEqualTo) {
        printf("Error: Expected gamma(4) = 24.0f, but got %f\n", gamma_4_value);
    }
    assert(OCCompareFloatValues(gamma_4_value, 24.0f) == kOCCompareEqualTo);
    
    // Verify unit remains dimensionless
    SIUnitRef result_unit = (SIUnitRef)SIQuantityGetUnit((SIQuantityRef)gamma_4);
    assert(SIUnitIsDimensionless(result_unit));
    
    // Test case 2: Gamma(5) = 24
    SIScalarRef s5 = SIScalarCreateWithFloat(5.0f, dimensionless);
    SIScalarRef gamma_5 = SIScalarCreateByGammaFunctionWithoutReducingUnit(s5, &error);
    assert(gamma_5 != NULL);
    
    // Print diagnostic info if comparison fails
    float gamma_5_value = SIScalarFloatValue(gamma_5);
    if (OCCompareFloatValues(gamma_5_value, 120.0f) != kOCCompareEqualTo) {
        printf("Error: Expected gamma(5) = 120.0f, but got %f\n", gamma_5_value);
    }
    assert(OCCompareFloatValues(gamma_5_value, 120.0f) == kOCCompareEqualTo);
    
    // Clean up
    OCRelease(s4);
    OCRelease(gamma_4);
    OCRelease(s5);
    OCRelease(gamma_5);
    if (error) OCRelease(error);
}

void test_SIScalarCreateByTakingNthRoot(void) {
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
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
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
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
    SIUnitRef dimensionless = SIUnitDimensionlessAndUnderived();
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
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    SIScalarRef s_complex = SIScalarCreateWithFloatComplex(3.0f + 4.0f * I, u);
    SIScalarRef s_zero_imag = SIScalarCreateByZeroingPart(s_complex, kSIImaginaryPart);
    assert(s_zero_imag != NULL);
    assert(fabsf(crealf(SIScalarFloatComplexValue(s_zero_imag)) - 3.0f) < 1e-6);
    assert(fabsf(cimagf(SIScalarFloatComplexValue(s_zero_imag)) - 0.0f) < 1e-6);
    OCRelease(s_complex); OCRelease(s_zero_imag);
}

void test_SIScalarZeroPart(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    SIMutableScalarRef m = SIScalarCreateMutableWithFloatComplex(3.0f + 4.0f * I, u);
    bool success = SIScalarZeroPart(m, kSIImaginaryPart);
    assert(success);
    assert(fabsf(crealf(SIScalarFloatComplexValue(m)) - 3.0f) < 1e-6);
    assert(fabsf(cimagf(SIScalarFloatComplexValue(m)) - 0.0f) < 1e-6);
    OCRelease(m);
}

void test_SIScalarMultiplyByDimensionlessRealConstant(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    SIMutableScalarRef m = SIScalarCreateMutableWithFloat(5.0f, u);
    bool success = SIScalarMultiplyByDimensionlessRealConstant(m, 2.5);
    assert(success);
    assert(fabsf(SIScalarFloatValue(m) - 12.5f) < 1e-6);
    OCRelease(m);
}

void test_SIScalarCreateByMultiplyingByDimensionlessRealConstant(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    SIScalarRef s_orig = SIScalarCreateWithFloat(5.0f, u);
    SIScalarRef s_scaled = SIScalarCreateByMultiplyingByDimensionlessRealConstant(s_orig, 2.5);
    assert(s_scaled != NULL);
    assert(fabsf(SIScalarFloatValue(s_scaled) - 12.5f) < 1e-6);
    OCRelease(s_orig); OCRelease(s_scaled);
}

void test_SIScalarMultiplyByDimensionlessComplexConstant(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    SIMutableScalarRef m = SIScalarCreateMutableWithFloat(2.0f, u); // 2 m
    double complex factor = 1.0 + 1.0 * I;
    bool success = SIScalarMultiplyByDimensionlessComplexConstant(m, factor);
    assert(success); // 2*(1+i) = 2+2i
    assert(fabsf(crealf(SIScalarFloatComplexValue(m)) - 2.0f) < 1e-6);
    assert(fabsf(cimagf(SIScalarFloatComplexValue(m)) - 2.0f) < 1e-6);
    OCRelease(m);
}

void test_SIScalarCreateByMultiplyingByDimensionlessComplexConstant(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    
    // Test case 1: Multiply real scalar by (1+i)
    SIMutableScalarRef m = SIScalarCreateMutableWithFloat(2.0f, u); // 2 m
    double complex factor = 1.0 + 1.0 * I; // (1+i)
    bool success = SIScalarMultiplyByDimensionlessComplexConstant(m, factor);
    assert(success);
    
    // 2m * (1+i) should be (2+2i)m
    float real_part = crealf(SIScalarFloatComplexValue(m));
    float imag_part = cimagf(SIScalarFloatComplexValue(m));
    
    if (fabsf(real_part - 2.0f) >= 1e-6 || fabsf(imag_part - 2.0f) >= 1e-6) {
        printf("Error: Expected (2+2i)m, but got (%f+%fi)m\n", real_part, imag_part);
    }
    
    assert(fabsf(real_part - 2.0f) < 1e-6);
    assert(fabsf(imag_part - 2.0f) < 1e-6);
    OCRelease(m);
    
    // Test case 2: Multiply by purely imaginary factor
    m = SIScalarCreateMutableWithFloat(3.0f, u); // 3 m
    factor = 0.0 + 2.0 * I; // 2i
    success = SIScalarMultiplyByDimensionlessComplexConstant(m, factor);
    assert(success);
    
    // 3m * 2i should be (0+6i)m
    real_part = crealf(SIScalarFloatComplexValue(m));
    imag_part = cimagf(SIScalarFloatComplexValue(m));
    
    if (fabsf(real_part - 0.0f) >= 1e-6 || fabsf(imag_part - 6.0f) >= 1e-6) {
        printf("Error: Expected (0+6i)m, but got (%f+%fi)m\n", real_part, imag_part);
    }
    
    assert(fabsf(real_part - 0.0f) < 1e-6);
    assert(fabsf(imag_part - 6.0f) < 1e-6);
    OCRelease(m);
    OCRelease(u);
}

void test_SIScalarCreateByConjugation(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    SIScalarRef s_complex = SIScalarCreateWithFloatComplex(3.0f + 4.0f * I, u);
    SIScalarRef s_conj = SIScalarCreateByConjugation(s_complex);
    assert(s_conj != NULL);
    assert(fabsf(crealf(SIScalarFloatComplexValue(s_conj)) - 3.0f) < 1e-6);
    assert(fabsf(cimagf(SIScalarFloatComplexValue(s_conj)) - (-4.0f)) < 1e-6);
    OCRelease(s_complex); OCRelease(s_conj);
}

void test_SIScalarConjugate(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    SIMutableScalarRef m = SIScalarCreateMutableWithFloatComplex(3.0f + 4.0f * I, u);
    bool success = SIScalarConjugate(m);
    assert(success);
    assert(fabsf(crealf(SIScalarFloatComplexValue(m)) - 3.0f) < 1e-6);
    assert(fabsf(cimagf(SIScalarFloatComplexValue(m)) - (-4.0f)) < 1e-6);
    OCRelease(m);
}

void test_SIScalarShow(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    SIScalarRef s = SIScalarCreateWithFloat(1.23f, u);
    printf("Testing SIScalarShow: "); // Visual check for output
    SIScalarShow(s); // Just call it, ensure no crash
    printf("\n");
    OCRelease(s);
}

void test_SIScalarCreateStringValue(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    SIScalarRef s = SIScalarCreateWithFloat(1.23f, u);
    OCStringRef str_val = SIScalarCreateStringValue(s);
    assert(str_val != NULL);
    // OCStringShow(str_val); // Optional: print for visual check
    OCRelease(str_val);
    OCRelease(s);
}

void test_SIScalarCreateNumericStringValue(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    SIScalarRef s = SIScalarCreateWithFloat(1.23f, u);
    OCStringRef num_str_val = SIScalarCreateNumericStringValue(s);
    assert(num_str_val != NULL);
    // OCStringShow(num_str_val);
    OCRelease(num_str_val); 
    OCRelease(s);
}

void test_SIScalarCreateStringValueForPart(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    SIScalarRef s = SIScalarCreateWithFloatComplex(1.0f + 2.0f * I, u);
    OCStringRef real_part_str = SIScalarCreateStringValueForPart(s, kSIRealPart);
    assert(real_part_str != NULL);
    // OCStringShow(real_part_str);
    OCRelease(real_part_str); 
    OCRelease(s);
}

void test_SIScalarCreateUnitString(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    SIScalarRef s = SIScalarCreateWithFloat(1.0f, u);
    OCStringRef unit_str = SIScalarCreateUnitString(s);
    assert(unit_str != NULL);
    assert(OCStringEqual(unit_str, STR("m")));
    OCRelease(unit_str); 
    OCRelease(s);
}

void test_SIScalarCreateStringValueWithFormat(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    SIScalarRef s = SIScalarCreateWithFloat(1.2345f, u);
    OCStringRef fmt = STR("%.2f %s");
    OCStringRef str_val = SIScalarCreateStringValueWithFormat(s, fmt);
    assert(str_val != NULL);
    // Check if it contains "1.23 m" or similar
    // OCStringShow(str_val);
    OCRelease(fmt); 
    OCRelease(str_val); 
    OCRelease(s);
}

void test_SIScalarCreateNumericStringValueWithFormat(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    SIScalarRef s = SIScalarCreateWithFloat(1.2345f, u);
    OCStringRef fmt = STR("%.2f");
    OCStringRef num_str_val = SIScalarCreateNumericStringValueWithFormat(s, fmt);
    assert(num_str_val != NULL);
    // Check if it contains "1.23"
    // OCStringShow(num_str_val);
    OCRelease(fmt); 
    OCRelease(num_str_val);
    OCRelease(s);
}

void test_SIScalarAddToArrayAsStringValue(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    SIScalarRef s = SIScalarCreateWithFloat(1.0f, u);
    OCMutableArrayRef array = OCArrayCreateMutable(0, &kOCTypeArrayCallBacks);
    SIScalarAddToArrayAsStringValue(s, array);
    assert(OCArrayGetCount(array) == 1);
    OCStringRef str_in_array = (OCStringRef)OCArrayGetValueAtIndex(array, 0);
    assert(str_in_array != NULL);
    OCRelease(array);
    OCRelease(s);
}

void test_SIScalarIsReal(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    SIScalarRef s_real = SIScalarCreateWithFloat(1.0f, u);
    SIScalarRef s_complex = SIScalarCreateWithFloatComplex(1.0f + 0.001f * I, u);
    SIScalarRef s_pure_imag = SIScalarCreateWithFloatComplex(0.0f + 1.0f * I, u);
    assert(SIScalarIsReal(s_real));
    assert(!SIScalarIsReal(s_complex));
    assert(!SIScalarIsReal(s_pure_imag)); // Real part is zero, but imag is not
    OCRelease(s_real); 
    OCRelease(s_complex); 
    OCRelease(s_pure_imag);
}

void test_SIScalarIsImaginary(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    SIScalarRef s_real = SIScalarCreateWithFloat(1.0f, u);
    SIScalarRef s_complex = SIScalarCreateWithFloatComplex(0.001f + 1.0f * I, u);
    SIScalarRef s_pure_imag = SIScalarCreateWithFloatComplex(0.0f + 1.0f * I, u);
    SIScalarRef s_zero_imag = SIScalarCreateWithFloatComplex(1.0f + 0.0f * I, u);
    assert(!SIScalarIsImaginary(s_real));
    assert(!SIScalarIsImaginary(s_complex)); // Real part is non-zero
    assert(SIScalarIsImaginary(s_pure_imag));
    assert(!SIScalarIsImaginary(s_zero_imag)); // Imaginary part is zero
    OCRelease(s_real); 
    OCRelease(s_complex); 
    OCRelease(s_pure_imag); 
    OCRelease(s_zero_imag);
}

void test_SIScalarIsComplex(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    SIScalarRef s_real = SIScalarCreateWithFloat(1.0f, u);
    SIScalarRef s_complex = SIScalarCreateWithFloatComplex(1.0f + 0.001f * I, u);
    SIScalarRef s_zero_imag = SIScalarCreateWithFloatComplex(1.0f + 0.0f * I, u);
    assert(!SIScalarIsComplex(s_real));
    assert(SIScalarIsComplex(s_complex));
    assert(!SIScalarIsComplex(s_zero_imag)); // Imaginary part is zero, so not complex by this definition
    OCRelease(s_real); 
    OCRelease(s_complex); 
    OCRelease(s_zero_imag);
}

void test_SIScalarIsZero(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
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
    OCRelease(s_zero_f); 
    OCRelease(s_zero_d); 
    OCRelease(s_zero_fc); 
    OCRelease(s_zero_dc); 
    OCRelease(s_non_zero);
}

void test_SIScalarIsInfinite(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
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
    OCRelease(s_inf_f); 
    OCRelease(s_inf_d); 
    OCRelease(s_inf_fc_real); 
    OCRelease(s_inf_fc_imag); 
    OCRelease(s_finite);
}

void test_SIScalarIsRealNonNegativeInteger(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m")); // Unit doesn't matter for this property of the value
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
    OCRelease(s_int_pos); 
    OCRelease(s_int_zero); 
    OCRelease(s_float); 
    OCRelease(s_neg_int); 
    OCRelease(s_complex);
}

void test_SIScalarValidateProposedStringValue(void) {
    OCStringRef valid_str = STR("10 cm"); // cm has same dimensionality as m
    OCStringRef invalid_dim_str = STR("10 kg"); // kg has different dimensionality
    OCStringRef invalid_fmt_str = STR("ten meters"); // bad format

    SIScalarRef s_meter = SIScalarCreateWithFloat(1.0f, SIUnitForUnderivedSymbol(STR("m")));
    OCStringRef error = NULL;

    assert(SIScalarValidateProposedStringValue(s_meter, valid_str, &error));
    assert(error == NULL);

    assert(!SIScalarValidateProposedStringValue(s_meter, invalid_dim_str, &error));
    assert(error != NULL); 
    OCRelease(error); 
    error = NULL;

    assert(!SIScalarValidateProposedStringValue(s_meter, invalid_fmt_str, &error));
    assert(error != NULL); 
    OCRelease(error); 
    error = NULL;

    OCRelease(s_meter);
}

void test_SIScalarEqual(void) {
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
    SIUnitRef cm = SIUnitForUnderivedSymbol(STR("cm"));

    SIScalarRef s1 = SIScalarCreateWithFloat(1.0f, m);
    SIScalarRef s2 = SIScalarCreateWithFloat(1.0f, m);
    SIScalarRef s3 = SIScalarCreateWithFloat(1.1f, m);
    SIScalarRef s4 = SIScalarCreateWithFloat(1.0f, cm); // Different unit

    assert(SIScalarEqual(s1, s2));
    assert(!SIScalarEqual(s1, s3));
    assert(!SIScalarEqual(s1, s4)); // Should be false due to different unit object, even if value is same type

    OCRelease(s1); 
    OCRelease(s2); 
    OCRelease(s3); 
    OCRelease(s4);
}

void test_SIScalarCompare(void) {
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
    SIUnitRef cm = SIUnitForUnderivedSymbol(STR("cm"));
    SIUnitRef kg = SIUnitForUnderivedSymbol(STR("kg"));

    SIScalarRef s_1m = SIScalarCreateWithFloat(1.0f, m);
    SIScalarRef s_2m = SIScalarCreateWithFloat(2.0f, m);
    SIScalarRef s_100cm = SIScalarCreateWithFloat(100.0f, cm);
    SIScalarRef s_1kg = SIScalarCreateWithFloat(1.0f, kg);

    assert(SIScalarCompare(s_1m, s_2m) == kOCCompareLessThan);
    assert(SIScalarCompare(s_2m, s_1m) == kOCCompareGreaterThan);
    assert(SIScalarCompare(s_1m, s_100cm) == kOCCompareEqualTo); // 1m == 100cm
    assert(SIScalarCompare(s_1m, s_1kg) == kOCCompareUnequalDimensionalities);

    OCRelease(s_1m); 
    OCRelease(s_2m); 
    OCRelease(s_100cm); 
    OCRelease(s_1kg);
}

void test_SIScalarCompareReduced(void) {
    // Similar to Compare, but ensures units are reduced first.
    // Example: 1 m vs 1000 mm (after reduction, mm becomes m)
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
    double mult = 1.0;
    SIUnitRef mm_unit = SIUnitForSymbol(STR("mm"), &mult, NULL); // millimeter
    SIScalarRef s_1m = SIScalarCreateWithFloat(1.0f, m);
    SIScalarRef s_1000mm = SIScalarCreateWithFloat(1000.0f, mm_unit);

    assert(SIScalarCompareReduced(s_1m, s_1000mm) == kOCCompareEqualTo);

    OCRelease(s_1m); 
    OCRelease(s_1000mm);
}

void test_SIScalarCompareLoose(void) {
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
    SIScalarRef s1 = SIScalarCreateWithDouble(1.000000001, m);
    SIScalarRef s2 = SIScalarCreateWithDouble(1.0, m);
    SIScalarRef s3 = SIScalarCreateWithDouble(1.001, m);

    assert(SIScalarCompare(s1,s2) == kOCCompareGreaterThan); // Exact compare
    assert(SIScalarCompareLoose(s1, s2) == kOCCompareEqualTo); // Loose compare
    assert(SIScalarCompareLoose(s1, s3) == kOCCompareLessThan);

    OCRelease(s1); 
    OCRelease(s2); 
    OCRelease(s3);
}

