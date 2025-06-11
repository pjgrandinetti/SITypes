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
    if (!u) {
        printf("mps_unit failed: SIUnitForSymbol(\"m/s\") returned NULL\n");
    }
    return u;
}


bool test_SIScalarGetTypeID(void) {
    if (SIScalarGetTypeID() == 0) {
        printf("test_SIScalarGetTypeID failed: TypeID is 0\n");
        return false;
    }
    printf("test_SIScalarGetTypeID passed\n");
    return true;
}


bool test_SIScalarCreateCopy(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    if (!u) {
        printf("test_SIScalarCreateCopy failed: SIUnitForUnderivedSymbol returned NULL\n");
        return false;
    }

    float input = 1.0f;
    SIScalarRef s = SIScalarCreateWithFloat(input, u);
    if (!s) {
        printf("test_SIScalarCreateCopy failed: Failed to create original scalar\n");
        return false;
    }

    SIScalarRef c = SIScalarCreateCopy(s);
    if (!c) {
        printf("test_SIScalarCreateCopy failed: Failed to create scalar copy\n");
        OCRelease(s);
        return false;
    }

    if (c == s) {
        printf("test_SIScalarCreateCopy failed: Copy is the same instance as original\n");
        OCRelease(c);
        OCRelease(s);
        return false;
    }

    if (OCCompareFloatValues(SIScalarFloatValue(s), SIScalarFloatValue(c)) != kOCCompareEqualTo) {
        printf("test_SIScalarCreateCopy failed: Float values differ between original and copy\n");
        OCRelease(c);
        OCRelease(s);
        return false;
    }

    OCStringRef us = SIScalarCreateUnitString(s);
    OCStringRef uc = SIScalarCreateUnitString(c);
    if (!OCStringEqual(us, uc)) {
        printf("test_SIScalarCreateCopy failed: Unit strings differ between original and copy\n");
        OCRelease(us);
        OCRelease(uc);
        OCRelease(c);
        OCRelease(s);
        return false;
    }

    OCRelease(us);
    OCRelease(uc);
    OCRelease(c);
    OCRelease(s);

    printf("test_SIScalarCreateCopy passed\n");
    return true;
}

bool test_SIScalarCreateMutableCopy(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    if (!u) {
        printf("test_SIScalarCreateMutableCopy failed: SIUnitForUnderivedSymbol returned NULL\n");
        return false;
    }

    SIScalarRef orig = SIScalarCreateWithFloat(2.0f, u);
    if (!orig) {
        printf("test_SIScalarCreateMutableCopy failed: Failed to create original scalar\n");
        return false;
    }

    SIMutableScalarRef copy = SIScalarCreateMutableCopy(orig);
    if (!copy) {
        printf("test_SIScalarCreateMutableCopy failed: Failed to create mutable copy\n");
        OCRelease(orig);
        return false;
    }

    if (copy == orig) {
        printf("test_SIScalarCreateMutableCopy failed: Mutable copy is the same instance as original\n");
        OCRelease(copy);
        OCRelease(orig);
        return false;
    }

    if (OCCompareFloatValues(SIScalarFloatValue(copy), SIScalarFloatValue(orig)) != kOCCompareEqualTo) {
        printf("test_SIScalarCreateMutableCopy failed: Float values do not match\n");
        OCRelease(copy);
        OCRelease(orig);
        return false;
    }

    OCStringRef unit_orig = SIScalarCreateUnitString(orig);
    OCStringRef unit_copy = SIScalarCreateUnitString(copy);
    if (!OCStringEqual(unit_orig, unit_copy)) {
        printf("test_SIScalarCreateMutableCopy failed: Unit strings do not match\n");
        OCRelease(unit_orig);
        OCRelease(unit_copy);
        OCRelease(copy);
        OCRelease(orig);
        return false;
    }

    OCRelease(unit_orig);
    OCRelease(unit_copy);

    // Mutate copy and check values
    SIScalarSetFloatValue(copy, 5.0f);

    if (OCCompareFloatValues(SIScalarFloatValue(copy), 5.0f) != kOCCompareEqualTo) {
        printf("test_SIScalarCreateMutableCopy failed: Copy mutation failed\n");
        OCRelease(copy);
        OCRelease(orig);
        return false;
    }

    if (OCCompareFloatValues(SIScalarFloatValue(orig), 2.0f) != kOCCompareEqualTo) {
        printf("test_SIScalarCreateMutableCopy failed: Original value changed after mutation\n");
        OCRelease(copy);
        OCRelease(orig);
        return false;
    }

    OCRelease(copy);
    OCRelease(orig);
    printf("test_SIScalarCreateMutableCopy passed\n");
    return true;
}

bool test_SIScalarCreateWithFloat(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    if (!u) {
        printf("test_SIScalarCreateWithFloat failed: SIUnitForUnderivedSymbol returned NULL\n");
        return false;
    }

    float input = 3.0f;
    SIScalarRef s = SIScalarCreateWithFloat(input, u);
    if (!s) {
        printf("test_SIScalarCreateWithFloat failed: Failed to create scalar with float\n");
        return false;
    }

    if (OCCompareFloatValues(SIScalarFloatValue(s), input) != kOCCompareEqualTo) {
        printf("test_SIScalarCreateWithFloat failed: Scalar value mismatch\n");
        OCRelease(s);
        return false;
    }

    OCStringRef unitStr = SIScalarCreateUnitString(s);
    if (!OCStringEqual(unitStr, STR("m"))) {
        printf("test_SIScalarCreateWithFloat failed: Unit string mismatch (expected 'm')\n");
        OCRelease(unitStr);
        OCRelease(s);
        return false;
    }

    OCRelease(unitStr);
    OCRelease(s);
    printf("test_SIScalarCreateWithFloat passed\n");
    return true;
}

bool test_SIScalarCreateMutableWithFloat(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    if (!u) {
        printf("test_SIScalarCreateMutableWithFloat failed: SIUnitForUnderivedSymbol returned NULL\n");
        return false;
    }

    float testValue = 4.0f;
    SIMutableScalarRef m = SIScalarCreateMutableWithFloat(testValue, u);
    if (!m) {
        printf("test_SIScalarCreateMutableWithFloat failed: Failed to create mutable scalar\n");
        return false;
    }

    if (OCCompareFloatValues(SIScalarFloatValue(m), testValue) != kOCCompareEqualTo) {
        printf("test_SIScalarCreateMutableWithFloat failed: Initial value mismatch\n");
        OCRelease(m);
        return false;
    }

    OCStringRef unitStr = SIScalarCreateUnitString(m);
    if (!OCStringEqual(unitStr, STR("m"))) {
        printf("test_SIScalarCreateMutableWithFloat failed: Unit string mismatch (expected 'm')\n");
        OCRelease(unitStr);
        OCRelease(m);
        return false;
    }
    OCRelease(unitStr);

    SIScalarSetFloatValue(m, 5.0f);
    if (OCCompareFloatValues(SIScalarFloatValue(m), 5.0f) != kOCCompareEqualTo) {
        printf("test_SIScalarCreateMutableWithFloat failed: Mutable value change not reflected\n");
        OCRelease(m);
        return false;
    }

    OCRelease(m);
    printf("test_SIScalarCreateMutableWithFloat passed\n");
    return true;
}


bool test_SIScalarCreateWithDouble(void) {
    // Create a unit for testing
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    if (!u) {
        printf("test_SIScalarCreateWithDouble failed: SIUnitForUnderivedSymbol returned NULL\n");
        return false;
    }

    // Test value to use
    double testValue = 5.0;

    // Create the scalar with our test value
    SIScalarRef s = SIScalarCreateWithDouble(testValue, u);
    if (!s) {
        printf("test_SIScalarCreateWithDouble failed: Failed to create scalar with double\n");
        return false;
    }

    // Verify the value was correctly stored
    if (OCCompareDoubleValues(SIScalarDoubleValue(s), testValue) != kOCCompareEqualTo) {
        printf("test_SIScalarCreateWithDouble failed: Scalar value mismatch\n");
        OCRelease(s);
        return false;
    }

    // Verify the unit was correctly assigned
    OCStringRef unitStr = SIScalarCreateUnitString(s);
    if (!OCStringEqual(unitStr, STR("m"))) {
        printf("test_SIScalarCreateWithDouble failed: Unit string mismatch (expected 'm')\n");
        OCRelease(unitStr);
        OCRelease(s);
        return false;
    }
    OCRelease(unitStr);

    // Verify the number type is correct (should be double)
    if (SIQuantityGetElementType((SIQuantityRef)s) != kSINumberFloat64Type) {
        printf("test_SIScalarCreateWithDouble failed: Scalar number type is not double\n");
        OCRelease(s);
        return false;
    }

    OCRelease(s);
    printf("test_SIScalarCreateWithDouble passed\n");
    return true;
}

bool test_SIScalarCreateMutableWithDouble(void) {
    // Create a unit for testing
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    if (!u) {
        printf("test_SIScalarCreateMutableWithDouble failed: SIUnitForUnderivedSymbol returned NULL\n");
        return false;
    }

    // Test value to use
    double testValue = 6.0;

    // Create the scalar with our test value
    SIMutableScalarRef m = SIScalarCreateMutableWithDouble(testValue, u);
    if (!m) {
        printf("test_SIScalarCreateMutableWithDouble failed: Failed to create mutable scalar\n");
        return false;
    }

    // Verify the value was correctly stored
    if (OCCompareDoubleValues(SIScalarDoubleValue(m), testValue) != kOCCompareEqualTo) {
        printf("test_SIScalarCreateMutableWithDouble failed: Initial value mismatch\n");
        OCRelease(m);
        return false;
    }

    // Verify the unit was correctly assigned
    OCStringRef unitStr = SIScalarCreateUnitString(m);
    if (!OCStringEqual(unitStr, STR("m"))) {
        printf("test_SIScalarCreateMutableWithDouble failed: Unit string mismatch (expected 'm')\n");
        OCRelease(unitStr);
        OCRelease(m);
        return false;
    }
    OCRelease(unitStr);

    // Verify the number type is correct
    if (SIQuantityGetElementType((SIQuantityRef)m) != kSINumberFloat64Type) {
        printf("test_SIScalarCreateMutableWithDouble failed: Incorrect number type (expected double)\n");
        OCRelease(m);
        return false;
    }

    // Verify mutability
    SIScalarSetDoubleValue(m, 7.0);
    if (OCCompareDoubleValues(SIScalarDoubleValue(m), 7.0) != kOCCompareEqualTo) {
        printf("test_SIScalarCreateMutableWithDouble failed: Value not updated after mutation\n");
        OCRelease(m);
        return false;
    }

    // Clean up
    OCRelease(m);
    printf("test_SIScalarCreateMutableWithDouble passed\n");
    return true;
}


bool test_SIScalarCreateWithFloatComplex(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    if (!u) {
        printf("test_SIScalarCreateWithFloatComplex failed: SIUnitForUnderivedSymbol returned NULL\n");
        return false;
    }

    float complex val = 1.0f + 2.0f * I;
    SIScalarRef s = SIScalarCreateWithFloatComplex(val, u);
    if (!s) {
        printf("test_SIScalarCreateWithFloatComplex failed: Failed to create scalar with float complex\n");
        return false;
    }

    float complex actual = SIScalarFloatComplexValue(s);
    if (fabsf(crealf(actual) - 1.0f) >= 1e-6) {
        printf("test_SIScalarCreateWithFloatComplex failed: Real part mismatch (expected 1.0f, got %f)\n", crealf(actual));
        OCRelease(s);
        return false;
    }

    if (fabsf(cimagf(actual) - 2.0f) >= 1e-6) {
        printf("test_SIScalarCreateWithFloatComplex failed: Imaginary part mismatch (expected 2.0f, got %f)\n", cimagf(actual));
        OCRelease(s);
        return false;
    }

    OCRelease(s);
    printf("test_SIScalarCreateWithFloatComplex passed\n");
    return true;
}

bool test_SIScalarCreateMutableWithFloatComplex(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    if (!u) {
        printf("test_SIScalarCreateMutableWithFloatComplex failed: SIUnitForUnderivedSymbol returned NULL\n");
        return false;
    }

    float complex val = 2.0f + 3.0f * I;
    SIMutableScalarRef m = SIScalarCreateMutableWithFloatComplex(val, u);
    if (!m) {
        printf("test_SIScalarCreateMutableWithFloatComplex failed: Failed to create mutable scalar\n");
        return false;
    }

    float complex actual = SIScalarFloatComplexValue(m);
    if (fabsf(crealf(actual) - 2.0f) >= 1e-6) {
        printf("test_SIScalarCreateMutableWithFloatComplex failed: Real part mismatch (expected 2.0f, got %f)\n", crealf(actual));
        OCRelease(m);
        return false;
    }

    if (fabsf(cimagf(actual) - 3.0f) >= 1e-6) {
        printf("test_SIScalarCreateMutableWithFloatComplex failed: Imaginary part mismatch (expected 3.0f, got %f)\n", cimagf(actual));
        OCRelease(m);
        return false;
    }

    OCRelease(m);
    printf("test_SIScalarCreateMutableWithFloatComplex passed\n");
    return true;
}


bool test_SIScalarCreateWithDoubleComplex(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    if (!u) {
        printf("test_SIScalarCreateWithDoubleComplex failed: SIUnitForUnderivedSymbol returned NULL\n");
        return false;
    }

    double complex val = 3.0 + 4.0 * I;
    SIScalarRef s = SIScalarCreateWithDoubleComplex(val, u);
    if (!s) {
        printf("test_SIScalarCreateWithDoubleComplex failed: Failed to create scalar with double complex\n");
        return false;
    }

    double complex actual = SIScalarDoubleComplexValue(s);
    if (fabs(creal(actual) - 3.0) >= 1e-9) {
        printf("test_SIScalarCreateWithDoubleComplex failed: Real part mismatch (expected 3.0, got %.12f)\n", creal(actual));
        OCRelease(s);
        return false;
    }

    if (fabs(cimag(actual) - 4.0) >= 1e-9) {
        printf("test_SIScalarCreateWithDoubleComplex failed: Imaginary part mismatch (expected 4.0, got %.12f)\n", cimag(actual));
        OCRelease(s);
        return false;
    }

    OCRelease(s);
    printf("test_SIScalarCreateWithDoubleComplex passed\n");
    return true;
}

bool test_SIScalarCreateMutableWithDoubleComplex(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    if (!u) {
        printf("test_SIScalarCreateMutableWithDoubleComplex failed: SIUnitForUnderivedSymbol returned NULL\n");
        return false;
    }

    double complex val = 4.0 + 5.0 * I;
    SIMutableScalarRef m = SIScalarCreateMutableWithDoubleComplex(val, u);
    if (!m) {
        printf("test_SIScalarCreateMutableWithDoubleComplex failed: Failed to create mutable scalar\n");
        return false;
    }

    double complex actual = SIScalarDoubleComplexValue(m);
    if (fabs(creal(actual) - 4.0) >= 1e-9) {
        printf("test_SIScalarCreateMutableWithDoubleComplex failed: Real part mismatch (expected 4.0, got %.12f)\n", creal(actual));
        OCRelease(m);
        return false;
    }

    if (fabs(cimag(actual) - 5.0) >= 1e-9) {
        printf("test_SIScalarCreateMutableWithDoubleComplex failed: Imaginary part mismatch (expected 5.0, got %.12f)\n", cimag(actual));
        OCRelease(m);
        return false;
    }

    OCRelease(m);
    printf("test_SIScalarCreateMutableWithDoubleComplex passed\n");
    return true;
}


bool test_SIScalarGetValue(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    if (!u) {
        printf("test_SIScalarGetValue failed: SIUnitForUnderivedSymbol returned NULL\n");
        return false;
    }

    SIScalarRef s = SIScalarCreateWithFloat(7.0f, u);
    if (!s) {
        printf("test_SIScalarGetValue failed: Failed to create scalar\n");
        return false;
    }

    impl_SINumber n = SIScalarGetValue(s);
    if (n.floatValue != 7.0f) {
        printf("test_SIScalarGetValue failed: Expected value 7.0f, got %.6f\n", n.floatValue);
        OCRelease(s);
        return false;
    }

    OCRelease(s);
    printf("test_SIScalarGetValue passed\n");
    return true;
}


bool test_SIScalarSetFloatValue(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    if (!u) {
        printf("test_SIScalarSetFloatValue failed: SIUnitForUnderivedSymbol returned NULL\n");
        return false;
    }

    SIMutableScalarRef m = SIScalarCreateMutableWithFloat(0.0f, u);
    if (!m) {
        printf("test_SIScalarSetFloatValue failed: Failed to create mutable scalar\n");
        return false;
    }

    SIScalarSetFloatValue(m, 8.0f);

    if (OCCompareFloatValues(SIScalarFloatValue(m), 8.0f) != kOCCompareEqualTo) {
        printf("test_SIScalarSetFloatValue failed: Scalar value was not updated to 8.0f\n");
        OCRelease(m);
        return false;
    }

    OCRelease(m);
    printf("test_SIScalarSetFloatValue passed\n");
    return true;
}

bool test_SIScalarSetDoubleValue(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    if (!u) {
        printf("test_SIScalarSetDoubleValue failed: SIUnitForUnderivedSymbol returned NULL\n");
        return false;
    }

    SIMutableScalarRef m = SIScalarCreateMutableWithDouble(0.0, u);
    if (!m) {
        printf("test_SIScalarSetDoubleValue failed: Failed to create mutable scalar\n");
        return false;
    }

    SIScalarSetDoubleValue(m, 9.0);

    if (OCCompareDoubleValues(SIScalarDoubleValue(m), 9.0) != kOCCompareEqualTo) {
        printf("test_SIScalarSetDoubleValue failed: Scalar value was not updated to 9.0\n");
        OCRelease(m);
        return false;
    }

    OCRelease(m);
    printf("test_SIScalarSetDoubleValue passed\n");
    return true;
}

bool test_SIScalarSetFloatComplexValue(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    if (!u) {
        printf("test_SIScalarSetFloatComplexValue failed: SIUnitForUnderivedSymbol returned NULL\n");
        return false;
    }

    SIMutableScalarRef m = SIScalarCreateMutableWithFloatComplex(0.0f + 0.0f * I, u);
    if (!m) {
        printf("test_SIScalarSetFloatComplexValue failed: Failed to create mutable scalar\n");
        return false;
    }

    float complex val = 1.0f + 1.0f * I;
    SIScalarSetFloatComplexValue(m, val);

    float complex actual = SIScalarFloatComplexValue(m);
    if (fabsf(crealf(actual) - 1.0f) >= 1e-6) {
        printf("test_SIScalarSetFloatComplexValue failed: Real part mismatch (expected 1.0f, got %f)\n", crealf(actual));
        OCRelease(m);
        return false;
    }

    if (fabsf(cimagf(actual) - 1.0f) >= 1e-6) {
        printf("test_SIScalarSetFloatComplexValue failed: Imaginary part mismatch (expected 1.0f, got %f)\n", cimagf(actual));
        OCRelease(m);
        return false;
    }

    OCRelease(m);
    printf("test_SIScalarSetFloatComplexValue passed\n");
    return true;
}

bool test_SIScalarSetDoubleComplexValue(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    if (!u) {
        printf("test_SIScalarSetDoubleComplexValue failed: SIUnitForUnderivedSymbol returned NULL\n");
        return false;
    }

    SIMutableScalarRef m = SIScalarCreateMutableWithDoubleComplex(0.0 + 0.0 * I, u);
    if (!m) {
        printf("test_SIScalarSetDoubleComplexValue failed: Failed to create mutable scalar\n");
        return false;
    }

    double complex val = 2.0 + 2.0 * I;
    SIScalarSetDoubleComplexValue(m, val);

    double complex actual = SIScalarDoubleComplexValue(m);
    if (fabs(creal(actual) - 2.0) >= 1e-9) {
        printf("test_SIScalarSetDoubleComplexValue failed: Real part mismatch (expected 2.0, got %.12f)\n", creal(actual));
        OCRelease(m);
        return false;
    }

    if (fabs(cimag(actual) - 2.0) >= 1e-9) {
        printf("test_SIScalarSetDoubleComplexValue failed: Imaginary part mismatch (expected 2.0, got %.12f)\n", cimag(actual));
        OCRelease(m);
        return false;
    }

    OCRelease(m);
    printf("test_SIScalarSetDoubleComplexValue passed\n");
    return true;
}

bool test_SIScalarSetElementType(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    if (!u) {
        printf("test_SIScalarSetElementType failed: SIUnitForUnderivedSymbol returned NULL\n");
        return false;
    }

    SIMutableScalarRef m = SIScalarCreateMutableWithFloat(1.0f, u);
    if (!m) {
        printf("test_SIScalarSetElementType failed: Failed to create mutable scalar\n");
        return false;
    }

    SIScalarSetElementType(m, kSINumberFloat64Type);

    // Verify value was preserved after type change
    if (fabs(SIScalarDoubleValue(m) - 1.0) >= 1e-9) {
        printf("test_SIScalarSetElementType failed: Value not preserved after changing type (expected 1.0, got %.12f)\n",
               SIScalarDoubleValue(m));
        OCRelease(m);
        return false;
    }

    OCRelease(m);
    printf("test_SIScalarSetElementType passed\n");
    return true;
}

bool test_SIScalarFloatValue(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    if (!u) {
        printf("test_SIScalarFloatValue failed: SIUnitForUnderivedSymbol returned NULL\n");
        return false;
    }

    SIScalarRef s = SIScalarCreateWithFloat(1.5f, u);
    if (!s) {
        printf("test_SIScalarFloatValue failed: Failed to create scalar\n");
        return false;
    }

    float val = SIScalarFloatValue(s);
    if (fabsf(val - 1.5f) >= 1e-6) {
        printf("test_SIScalarFloatValue failed: Expected 1.5f, got %.6f\n", val);
        OCRelease(s);
        return false;
    }

    OCRelease(s);
    printf("test_SIScalarFloatValue passed\n");
    return true;
}


bool test_SIScalarDoubleValue(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    if (!u) {
        printf("test_SIScalarDoubleValue failed: SIUnitForUnderivedSymbol returned NULL\n");
        return false;
    }

    SIScalarRef s = SIScalarCreateWithDouble(2.5, u);
    if (!s) {
        printf("test_SIScalarDoubleValue failed: Failed to create scalar\n");
        return false;
    }

    double val = SIScalarDoubleValue(s);
    if (fabs(val - 2.5) >= 1e-9) {
        printf("test_SIScalarDoubleValue failed: Expected 2.5, got %.12f\n", val);
        OCRelease(s);
        return false;
    }

    OCRelease(s);
    printf("test_SIScalarDoubleValue passed\n");
    return true;
}

bool test_SIScalarFloatComplexValue(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    if (!u) {
        printf("test_SIScalarFloatComplexValue failed: SIUnitForUnderivedSymbol returned NULL\n");
        return false;
    }

    SIScalarRef s = SIScalarCreateWithFloatComplex(1.0f + 2.0f * I, u);
    if (!s) {
        printf("test_SIScalarFloatComplexValue failed: Failed to create scalar\n");
        return false;
    }

    float complex val = SIScalarFloatComplexValue(s);
    if (fabsf(crealf(val) - 1.0f) >= 1e-6) {
        printf("test_SIScalarFloatComplexValue failed: Real part mismatch (expected 1.0f, got %.6f)\n", crealf(val));
        OCRelease(s);
        return false;
    }

    if (fabsf(cimagf(val) - 2.0f) >= 1e-6) {
        printf("test_SIScalarFloatComplexValue failed: Imaginary part mismatch (expected 2.0f, got %.6f)\n", cimagf(val));
        OCRelease(s);
        return false;
    }

    OCRelease(s);
    printf("test_SIScalarFloatComplexValue passed\n");
    return true;
}

bool test_SIScalarDoubleComplexValue(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    if (!u) {
        printf("test_SIScalarDoubleComplexValue failed: SIUnitForUnderivedSymbol returned NULL\n");
        return false;
    }

    SIScalarRef s = SIScalarCreateWithDoubleComplex(2.0 + 3.0 * I, u);
    if (!s) {
        printf("test_SIScalarDoubleComplexValue failed: Failed to create scalar\n");
        return false;
    }

    double complex val = SIScalarDoubleComplexValue(s);
    if (fabs(creal(val) - 2.0) >= 1e-9) {
        printf("test_SIScalarDoubleComplexValue failed: Real part mismatch (expected 2.0, got %.12f)\n", creal(val));
        OCRelease(s);
        return false;
    }

    if (fabs(cimag(val) - 3.0) >= 1e-9) {
        printf("test_SIScalarDoubleComplexValue failed: Imaginary part mismatch (expected 3.0, got %.12f)\n", cimag(val));
        OCRelease(s);
        return false;
    }

    OCRelease(s);
    printf("test_SIScalarDoubleComplexValue passed\n");
    return true;
}


bool test_SIScalarCreateByConvertingToNumberType(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    if (!u) {
        printf("test_SIScalarCreateByConvertingToNumberType failed: SIUnitForUnderivedSymbol returned NULL\n");
        return false;
    }

    SIScalarRef s_double = SIScalarCreateWithDouble(123.456, u);
    if (!s_double) {
        printf("test_SIScalarCreateByConvertingToNumberType failed: Failed to create double scalar\n");
        return false;
    }

    SIScalarRef s_float = SIScalarCreateByConvertingToNumberType(s_double, kSINumberFloat32Type);
    if (!s_float) {
        printf("test_SIScalarCreateByConvertingToNumberType failed: Conversion to float scalar returned NULL\n");
        OCRelease(s_double);
        return false;
    }

    float float_val = SIScalarFloatValue(s_float);
    if (fabsf(float_val - 123.456f) >= 1e-3f) {  // Allowable precision loss
        printf("test_SIScalarCreateByConvertingToNumberType failed: Float value mismatch (expected ~123.456, got %.6f)\n", float_val);
        OCRelease(s_double);
        OCRelease(s_float);
        return false;
    }

    OCRelease(s_double);
    OCRelease(s_float);
    printf("test_SIScalarCreateByConvertingToNumberType passed\n");
    return true;
}

bool test_SIScalarTakeComplexPart(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    if (!u) {
        printf("test_SIScalarTakeComplexPart failed: SIUnitForUnderivedSymbol returned NULL\n");
        return false;
    }

    SIMutableScalarRef m = SIScalarCreateMutableWithFloatComplex(3.0f + 4.0f * I, u);
    if (!m) {
        printf("test_SIScalarTakeComplexPart failed: Failed to create mutable scalar\n");
        return false;
    }

    bool success = SIScalarTakeComplexPart(m, kSIRealPart);
    if (!success) {
        printf("test_SIScalarTakeComplexPart failed: SIScalarTakeComplexPart returned false\n");
        OCRelease(m);
        return false;
    }

    if (!SIScalarIsReal(m)) {
        printf("test_SIScalarTakeComplexPart failed: Scalar is not real after taking real part\n");
        OCRelease(m);
        return false;
    }

    float val = SIScalarFloatValue(m);
    if (fabsf(val - 3.0f) >= 1e-6f) {
        printf("test_SIScalarTakeComplexPart failed: Real part value mismatch (expected 3.0, got %.6f)\n", val);
        OCRelease(m);
        return false;
    }

    OCRelease(m);
    printf("test_SIScalarTakeComplexPart passed\n");
    return true;
}


bool test_SIScalarCreateByTakingComplexPart(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    if (!u) {
        printf("test_SIScalarCreateByTakingComplexPart failed: SIUnitForUnderivedSymbol returned NULL\n");
        return false;
    }

    SIScalarRef s_complex = SIScalarCreateWithFloatComplex(3.0f + 4.0f * I, u);
    if (!s_complex) {
        printf("test_SIScalarCreateByTakingComplexPart failed: Failed to create complex scalar\n");
        return false;
    }

    SIScalarRef s_real_part = SIScalarCreateByTakingComplexPart(s_complex, kSIRealPart);
    if (!s_real_part) {
        printf("test_SIScalarCreateByTakingComplexPart failed: Failed to extract real part\n");
        OCRelease(s_complex);
        return false;
    }

    if (!SIScalarIsReal(s_real_part)) {
        printf("test_SIScalarCreateByTakingComplexPart failed: Resulting scalar is not real\n");
        OCRelease(s_complex);
        OCRelease(s_real_part);
        return false;
    }

    float real_val = SIScalarFloatValue(s_real_part);
    if (fabsf(real_val - 3.0f) >= 1e-6f) {
        printf("test_SIScalarCreateByTakingComplexPart failed: Real part value mismatch (expected 3.0, got %.6f)\n", real_val);
        OCRelease(s_complex);
        OCRelease(s_real_part);
        return false;
    }

    OCRelease(s_complex);
    OCRelease(s_real_part);
    printf("test_SIScalarCreateByTakingComplexPart passed\n");
    return true;
}


bool test_SIScalarCreateByReducingUnit(void) {
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
    SIUnitRef s_unit = SIUnitForUnderivedSymbol(STR("s"));
    if (!m || !s_unit) {
        printf("test_SIScalarCreateByReducingUnit failed: Failed to retrieve base units\n");
        return false;
    }

    double mult1 = 1.0;
    SIUnitRef ms = SIUnitByMultiplying(m, s_unit, &mult1, NULL);
    if (!ms) {
        printf("test_SIScalarCreateByReducingUnit failed: Failed to multiply units\n");
        return false;
    }

    double mult2 = 1.0;
    SIUnitRef m_final = SIUnitByDividing(ms, s_unit, &mult2);
    if (!m_final) {
        printf("test_SIScalarCreateByReducingUnit failed: Failed to divide units\n");
        return false;
    }

    SIScalarRef scalar_unreduced = SIScalarCreateWithFloat(1.0f, m_final);
    if (!scalar_unreduced) {
        printf("test_SIScalarCreateByReducingUnit failed: Failed to create unreduced scalar\n");
        return false;
    }

    SIScalarRef scalar_reduced = SIScalarCreateByReducingUnit(scalar_unreduced);
    if (!scalar_reduced) {
        printf("test_SIScalarCreateByReducingUnit failed: Failed to reduce unit\n");
        OCRelease(scalar_unreduced);
        return false;
    }

    OCStringRef unit_str = SIScalarCreateUnitString(scalar_reduced);
    if (!OCStringEqual(unit_str, STR("m"))) {
        printf("test_SIScalarCreateByReducingUnit failed: Reduced unit is not 'm' (got: %s)\n",
               OCStringGetCString(unit_str));
        OCRelease(unit_str);
        OCRelease(scalar_unreduced);
        OCRelease(scalar_reduced);
        return false;
    }

    OCRelease(unit_str);
    OCRelease(scalar_unreduced);
    OCRelease(scalar_reduced);

    printf("test_SIScalarCreateByReducingUnit passed\n");
    return true;
}


bool test_SIScalarReduceUnit(void) {
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
    SIUnitRef s_unit = SIUnitForUnderivedSymbol(STR("s"));
    if (!m || !s_unit) {
        printf("test_SIScalarReduceUnit failed: Failed to retrieve base units\n");
        return false;
    }

    double mult1 = 1.0;
    SIUnitRef ms = SIUnitByMultiplying(m, s_unit, &mult1, NULL);
    if (!ms) {
        printf("test_SIScalarReduceUnit failed: Failed to multiply units\n");
        return false;
    }

    double mult2 = 1.0;
    SIUnitRef m_final_unit = SIUnitByDividing(ms, s_unit, &mult2);
    if (!m_final_unit) {
        printf("test_SIScalarReduceUnit failed: Failed to divide units\n");
        return false;
    }

    SIMutableScalarRef scalar = SIScalarCreateMutableWithFloat(1.0f, m_final_unit);
    if (!scalar) {
        printf("test_SIScalarReduceUnit failed: Failed to create scalar\n");
        return false;
    }

    bool success = SIScalarReduceUnit(scalar);
    if (!success) {
        printf("test_SIScalarReduceUnit failed: SIScalarReduceUnit returned false\n");
        OCRelease(scalar);
        return false;
    }

    OCStringRef unit_str = SIScalarCreateUnitString(scalar);
    if (!OCStringEqual(unit_str, STR("m"))) {
        printf("test_SIScalarReduceUnit failed: Reduced unit is not 'm' (got: %s)\n",
               OCStringGetCString(unit_str));
        OCRelease(unit_str);
        OCRelease(scalar);
        return false;
    }

    OCRelease(unit_str);
    OCRelease(scalar);
    printf("test_SIScalarReduceUnit passed\n");
    return true;
}


bool test_SIScalarConvertToUnit(void) {
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
    SIUnitRef cm = SIUnitForUnderivedSymbol(STR("cm"));
    if (!m || !cm) {
        printf("test_SIScalarConvertToUnit failed: Failed to retrieve 'm' or 'cm' units\n");
        return false;
    }

    SIMutableScalarRef scalar = SIScalarCreateMutableWithFloat(1.0f, m); // 1m
    if (!scalar) {
        printf("test_SIScalarConvertToUnit failed: Failed to create scalar\n");
        return false;
    }

    OCStringRef error = NULL;
    bool success = SIScalarConvertToUnit(scalar, cm, &error);
    if (!success) {
        if (error) {
            printf("test_SIScalarConvertToUnit failed: Conversion error: %s\n", OCStringGetCString(error));
            OCRelease(error);
        } else {
            printf("test_SIScalarConvertToUnit failed: Conversion returned false with no error message\n");
        }
        OCRelease(scalar);
        return false;
    }

    if (error != NULL) {
        printf("test_SIScalarConvertToUnit failed: Unexpected non-NULL error string after successful conversion\n");
        OCRelease(error);
        OCRelease(scalar);
        return false;
    }

    if (OCCompareFloatValues(SIScalarFloatValue(scalar), 100.0f) != kOCCompareEqualTo) {
        printf("test_SIScalarConvertToUnit failed: Expected 100.0f cm, got %.6f\n", SIScalarFloatValue(scalar));
        OCRelease(scalar);
        return false;
    }

    OCRelease(scalar);
    printf("test_SIScalarConvertToUnit passed\n");
    return true;
}

bool test_SIScalarCreateByConvertingToUnit(void) {
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
    SIUnitRef cm = SIUnitForUnderivedSymbol(STR("cm"));
    if (!m || !cm) {
        printf("test_SIScalarCreateByConvertingToUnit failed: Failed to retrieve 'm' or 'cm' units\n");
        return false;
    }

    SIScalarRef s_m = SIScalarCreateWithFloat(1.0f, m); // 1m
    if (!s_m) {
        printf("test_SIScalarCreateByConvertingToUnit failed: Failed to create scalar in meters\n");
        return false;
    }

    OCStringRef error = NULL;
    SIScalarRef s_cm = SIScalarCreateByConvertingToUnit(s_m, cm, &error);
    if (!s_cm) {
        if (error) {
            printf("test_SIScalarCreateByConvertingToUnit failed: Conversion error: %s\n", OCStringGetCString(error));
            OCRelease(error);
        } else {
            printf("test_SIScalarCreateByConvertingToUnit failed: Conversion returned NULL without error\n");
        }
        OCRelease(s_m);
        return false;
    }

    if (error != NULL) {
        printf("test_SIScalarCreateByConvertingToUnit failed: Unexpected non-NULL error string after successful conversion\n");
        OCRelease(error);
        OCRelease(s_m);
        OCRelease(s_cm);
        return false;
    }

    if (OCCompareFloatValues(SIScalarFloatValue(s_cm), 100.0f) != kOCCompareEqualTo) {
        printf("test_SIScalarCreateByConvertingToUnit failed: Expected 100.0f cm, got %.6f\n", SIScalarFloatValue(s_cm));
        OCRelease(s_m);
        OCRelease(s_cm);
        return false;
    }

    OCRelease(s_m);
    OCRelease(s_cm);
    printf("test_SIScalarCreateByConvertingToUnit passed\n");
    return true;
}

bool test_SIScalarConvertToCoherentUnit(void) {
    SIUnitRef cm = SIUnitForUnderivedSymbol(STR("cm"));
    if (!cm) {
        printf("test_SIScalarConvertToCoherentUnit failed: Failed to retrieve 'cm' unit\n");
        return false;
    }

    SIMutableScalarRef scalar = SIScalarCreateMutableWithFloat(150.0f, cm); // 150cm
    if (!scalar) {
        printf("test_SIScalarConvertToCoherentUnit failed: Failed to create scalar\n");
        return false;
    }

    OCStringRef error = NULL;
    bool success = SIScalarConvertToCoherentUnit(scalar, &error);
    if (!success) {
        if (error) {
            printf("test_SIScalarConvertToCoherentUnit failed: Conversion error: %s\n", OCStringGetCString(error));
            OCRelease(error);
        } else {
            printf("test_SIScalarConvertToCoherentUnit failed: Conversion returned false without error message\n");
        }
        OCRelease(scalar);
        return false;
    }

    if (error != NULL) {
        printf("test_SIScalarConvertToCoherentUnit failed: Unexpected non-NULL error string after successful conversion\n");
        OCRelease(error);
        OCRelease(scalar);
        return false;
    }

    float val = SIScalarFloatValue(scalar);
    if (OCCompareFloatValues(val, 1.5f) != kOCCompareEqualTo) {
        printf("test_SIScalarConvertToCoherentUnit failed: Expected 1.5f (m), got %.6f\n", val);
        OCRelease(scalar);
        return false;
    }

    OCRelease(scalar);
    printf("test_SIScalarConvertToCoherentUnit passed\n");
    return true;
}

bool test_SIScalarCreateByConvertingToCoherentUnit(void) {
    SIUnitRef cm = SIUnitForUnderivedSymbol(STR("cm"));
    if (!cm) {
        printf("test_SIScalarCreateByConvertingToCoherentUnit failed: Failed to retrieve 'cm' unit\n");
        return false;
    }

    SIScalarRef s_cm = SIScalarCreateWithFloat(150.0f, cm); // 150cm
    if (!s_cm) {
        printf("test_SIScalarCreateByConvertingToCoherentUnit failed: Failed to create scalar in cm\n");
        return false;
    }

    OCStringRef error = NULL;
    SIScalarRef s_m = SIScalarCreateByConvertingToCoherentUnit(s_cm, &error);
    if (!s_m) {
        if (error) {
            printf("test_SIScalarCreateByConvertingToCoherentUnit failed: Conversion error: %s\n", OCStringGetCString(error));
            OCRelease(error);
        } else {
            printf("test_SIScalarCreateByConvertingToCoherentUnit failed: Conversion returned NULL with no error\n");
        }
        OCRelease(s_cm);
        return false;
    }

    if (error != NULL) {
        printf("test_SIScalarCreateByConvertingToCoherentUnit failed: Unexpected non-NULL error string after successful conversion\n");
        OCRelease(error);
        OCRelease(s_cm);
        OCRelease(s_m);
        return false;
    }

    if (OCCompareFloatValues(SIScalarFloatValue(s_m), 1.5f) != kOCCompareEqualTo) {
        printf("test_SIScalarCreateByConvertingToCoherentUnit failed: Expected 1.5f (m), got %.6f\n", SIScalarFloatValue(s_m));
        OCRelease(s_cm);
        OCRelease(s_m);
        return false;
    }

    OCRelease(s_cm);
    OCRelease(s_m);
    printf("test_SIScalarCreateByConvertingToCoherentUnit passed\n");
    return true;
}


bool test_SIScalarCreateByAdding(void) {
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
    if (!m) {
        printf("test_SIScalarCreateByAdding failed: Failed to retrieve unit 'm'\n");
        return false;
    }

    SIScalarRef s1 = SIScalarCreateWithFloat(1.0f, m);
    SIScalarRef s2 = SIScalarCreateWithFloat(2.5f, m);
    if (!s1 || !s2) {
        printf("test_SIScalarCreateByAdding failed: Failed to create input scalars\n");
        if (s1) OCRelease(s1);
        if (s2) OCRelease(s2);
        return false;
    }

    OCStringRef error = NULL;
    SIScalarRef sum = SIScalarCreateByAdding(s1, s2, &error);
    if (!sum) {
        if (error) {
            printf("test_SIScalarCreateByAdding failed: Addition error: %s\n", OCStringGetCString(error));
            OCRelease(error);
        } else {
            printf("test_SIScalarCreateByAdding failed: Addition returned NULL with no error\n");
        }
        OCRelease(s1);
        OCRelease(s2);
        return false;
    }

    if (error != NULL) {
        printf("test_SIScalarCreateByAdding failed: Unexpected error string after successful addition\n");
        OCRelease(error);
        OCRelease(s1);
        OCRelease(s2);
        OCRelease(sum);
        return false;
    }

    float result = SIScalarFloatValue(sum);
    if (OCCompareFloatValues(result, 3.5f) != kOCCompareEqualTo) {
        printf("test_SIScalarCreateByAdding failed: Expected 3.5f, got %.6f\n", result);
        OCRelease(s1);
        OCRelease(s2);
        OCRelease(sum);
        return false;
    }

    OCRelease(s1);
    OCRelease(s2);
    OCRelease(sum);
    printf("test_SIScalarCreateByAdding passed\n");
    return true;
}

bool test_SIScalarAdd(void) {
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
    if (!m) {
        printf("test_SIScalarAdd failed: Failed to retrieve unit 'm'\n");
        return false;
    }

    SIMutableScalarRef target = SIScalarCreateMutableWithFloat(1.0f, m);
    SIScalarRef s2 = SIScalarCreateWithFloat(2.5f, m);
    if (!target || !s2) {
        printf("test_SIScalarAdd failed: Failed to create scalar operands\n");
        if (target) OCRelease(target);
        if (s2) OCRelease(s2);
        return false;
    }

    OCStringRef error = NULL;
    bool success = SIScalarAdd(target, s2, &error);
    if (!success) {
        if (error) {
            printf("test_SIScalarAdd failed: Addition error: %s\n", OCStringGetCString(error));
            OCRelease(error);
        } else {
            printf("test_SIScalarAdd failed: Addition returned false with no error message\n");
        }
        OCRelease(target);
        OCRelease(s2);
        return false;
    }

    if (error != NULL) {
        printf("test_SIScalarAdd failed: Unexpected error string after successful addition\n");
        OCRelease(error);
        OCRelease(target);
        OCRelease(s2);
        return false;
    }

    float result = SIScalarFloatValue(target);
    if (OCCompareFloatValues(result, 3.5f) != kOCCompareEqualTo) {
        printf("test_SIScalarAdd failed: Expected 3.5f, got %.6f\n", result);
        OCRelease(target);
        OCRelease(s2);
        return false;
    }

    OCRelease(target);
    OCRelease(s2);
    printf("test_SIScalarAdd passed\n");
    return true;
}

bool test_SIScalarCreateBySubtracting(void) {
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
    if (!m) {
        printf("test_SIScalarCreateBySubtracting failed: Failed to retrieve unit 'm'\n");
        return false;
    }

    SIScalarRef s1 = SIScalarCreateWithFloat(5.0f, m);
    SIScalarRef s2 = SIScalarCreateWithFloat(2.5f, m);
    if (!s1 || !s2) {
        printf("test_SIScalarCreateBySubtracting failed: Failed to create input scalars\n");
        if (s1) OCRelease(s1);
        if (s2) OCRelease(s2);
        return false;
    }

    OCStringRef error = NULL;
    SIScalarRef diff = SIScalarCreateBySubtracting(s1, s2, &error);
    if (!diff) {
        if (error) {
            printf("test_SIScalarCreateBySubtracting failed: Subtraction error: %s\n", OCStringGetCString(error));
            OCRelease(error);
        } else {
            printf("test_SIScalarCreateBySubtracting failed: Subtraction returned NULL with no error\n");
        }
        OCRelease(s1);
        OCRelease(s2);
        return false;
    }

    if (error != NULL) {
        printf("test_SIScalarCreateBySubtracting failed: Unexpected error string after successful subtraction\n");
        OCRelease(error);
        OCRelease(s1);
        OCRelease(s2);
        OCRelease(diff);
        return false;
    }

    float result = SIScalarFloatValue(diff);
    if (OCCompareFloatValues(result, 2.5f) != kOCCompareEqualTo) {
        printf("test_SIScalarCreateBySubtracting failed: Expected 2.5f, got %.6f\n", result);
        OCRelease(s1);
        OCRelease(s2);
        OCRelease(diff);
        return false;
    }

    OCRelease(s1);
    OCRelease(s2);
    OCRelease(diff);
    printf("test_SIScalarCreateBySubtracting passed\n");
    return true;
}

bool test_SIScalarSubtract(void) {
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
    if (!m) {
        printf("test_SIScalarSubtract failed: Failed to retrieve unit 'm'\n");
        return false;
    }

    SIMutableScalarRef target = SIScalarCreateMutableWithFloat(5.0f, m);
    SIScalarRef s2 = SIScalarCreateWithFloat(2.5f, m);
    if (!target || !s2) {
        printf("test_SIScalarSubtract failed: Failed to create scalar operands\n");
        if (target) OCRelease(target);
        if (s2) OCRelease(s2);
        return false;
    }

    OCStringRef error = NULL;
    bool success = SIScalarSubtract(target, s2, &error);
    if (!success) {
        if (error) {
            printf("test_SIScalarSubtract failed: Subtraction error: %s\n", OCStringGetCString(error));
            OCRelease(error);
        } else {
            printf("test_SIScalarSubtract failed: Subtraction returned false without error message\n");
        }
        OCRelease(target);
        OCRelease(s2);
        return false;
    }

    if (error != NULL) {
        printf("test_SIScalarSubtract failed: Unexpected error string after successful subtraction\n");
        OCRelease(error);
        OCRelease(target);
        OCRelease(s2);
        return false;
    }

    float result = SIScalarFloatValue(target);
    if (OCCompareFloatValues(result, 2.5f) != kOCCompareEqualTo) {
        printf("test_SIScalarSubtract failed: Expected 2.5f, got %.6f\n", result);
        OCRelease(target);
        OCRelease(s2);
        return false;
    }

    OCRelease(target);
    OCRelease(s2);
    printf("test_SIScalarSubtract passed\n");
    return true;
}


bool test_SIScalarCreateByMultiplyingWithoutReducingUnit(void) {
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
    SIUnitRef s_unit = SIUnitForUnderivedSymbol(STR("s"));
    if (!m || !s_unit) {
        printf("test_SIScalarCreateByMultiplyingWithoutReducingUnit failed: Failed to retrieve base units\n");
        return false;
    }

    SIScalarRef s1 = SIScalarCreateWithFloat(2.0f, m); // 2 m
    SIScalarRef s2 = SIScalarCreateWithFloat(3.0f, s_unit); // 3 s
    if (!s1 || !s2) {
        printf("test_SIScalarCreateByMultiplyingWithoutReducingUnit failed: Failed to create input scalars\n");
        if (s1) OCRelease(s1);
        if (s2) OCRelease(s2);
        return false;
    }

    OCStringRef error = NULL;
    SIScalarRef prod = SIScalarCreateByMultiplyingWithoutReducingUnit(s1, s2, &error);
    if (!prod) {
        if (error) {
            printf("test_SIScalarCreateByMultiplyingWithoutReducingUnit failed: Multiplication error: %s\n", OCStringGetCString(error));
            OCRelease(error);
        } else {
            printf("test_SIScalarCreateByMultiplyingWithoutReducingUnit failed: Multiplication returned NULL with no error\n");
        }
        OCRelease(s1);
        OCRelease(s2);
        return false;
    }

    if (error != NULL) {
        printf("test_SIScalarCreateByMultiplyingWithoutReducingUnit failed: Unexpected error string after successful multiplication\n");
        OCRelease(error);
        OCRelease(s1);
        OCRelease(s2);
        OCRelease(prod);
        return false;
    }

    float result = SIScalarFloatValue(prod);
    if (OCCompareFloatValues(result, 6.0f) != kOCCompareEqualTo) {
        printf("test_SIScalarCreateByMultiplyingWithoutReducingUnit failed: Expected 6.0f, got %.6f\n", result);
        OCRelease(s1);
        OCRelease(s2);
        OCRelease(prod);
        return false;
    }

    // Optional: could check unit string if required

    OCRelease(s1);
    OCRelease(s2);
    OCRelease(prod);
    printf("test_SIScalarCreateByMultiplyingWithoutReducingUnit passed\n");
    return true;
}


bool test_SIScalarMultiplyWithoutReducingUnit(void) {
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
    SIUnitRef s_unit = SIUnitForUnderivedSymbol(STR("s"));
    if (!m || !s_unit) {
        printf("test_SIScalarMultiplyWithoutReducingUnit failed: Failed to retrieve base units\n");
        return false;
    }

    SIMutableScalarRef target = SIScalarCreateMutableWithFloat(2.0f, m); // 2 m
    SIScalarRef s2 = SIScalarCreateWithFloat(3.0f, s_unit); // 3 s
    if (!target || !s2) {
        printf("test_SIScalarMultiplyWithoutReducingUnit failed: Failed to create scalar operands\n");
        if (target) OCRelease(target);
        if (s2) OCRelease(s2);
        return false;
    }

    OCStringRef error = NULL;
    bool success = SIScalarMultiplyWithoutReducingUnit(target, s2, &error);
    if (!success) {
        if (error) {
            printf("test_SIScalarMultiplyWithoutReducingUnit failed: Multiplication error: %s\n", OCStringGetCString(error));
            OCRelease(error);
        } else {
            printf("test_SIScalarMultiplyWithoutReducingUnit failed: Multiplication returned false without error\n");
        }
        OCRelease(target);
        OCRelease(s2);
        return false;
    }

    if (error != NULL) {
        printf("test_SIScalarMultiplyWithoutReducingUnit failed: Unexpected non-NULL error string after success\n");
        OCRelease(error);
        OCRelease(target);
        OCRelease(s2);
        return false;
    }

    float result = SIScalarFloatValue(target);
    if (OCCompareFloatValues(result, 6.0f) != kOCCompareEqualTo) {
        printf("test_SIScalarMultiplyWithoutReducingUnit failed: Expected 6.0f, got %.6f\n", result);
        OCRelease(target);
        OCRelease(s2);
        return false;
    }

    OCRelease(target);
    OCRelease(s2);
    printf("test_SIScalarMultiplyWithoutReducingUnit passed\n");
    return true;
}

bool test_SIScalarCreateByMultiplying(void) {
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
    SIUnitRef s_unit = SIUnitForUnderivedSymbol(STR("s"));
    if (!m || !s_unit) {
        printf("test_SIScalarCreateByMultiplying failed: Failed to retrieve base units\n");
        return false;
    }

    SIScalarRef s1 = SIScalarCreateWithFloat(2.0f, m); // 2 m
    SIScalarRef s2 = SIScalarCreateWithFloat(3.0f, s_unit); // 3 s
    if (!s1 || !s2) {
        printf("test_SIScalarCreateByMultiplying failed: Failed to create scalar operands\n");
        if (s1) OCRelease(s1);
        if (s2) OCRelease(s2);
        return false;
    }

    OCStringRef error = NULL;
    SIScalarRef prod = SIScalarCreateByMultiplying(s1, s2, &error);
    if (!prod) {
        if (error) {
            printf("test_SIScalarCreateByMultiplying failed: Multiplication error: %s\n", OCStringGetCString(error));
            OCRelease(error);
        } else {
            printf("test_SIScalarCreateByMultiplying failed: Multiplication returned NULL with no error\n");
        }
        OCRelease(s1);
        OCRelease(s2);
        return false;
    }

    if (error != NULL) {
        printf("test_SIScalarCreateByMultiplying failed: Unexpected non-NULL error after successful multiplication\n");
        OCRelease(error);
        OCRelease(s1);
        OCRelease(s2);
        OCRelease(prod);
        return false;
    }

    float result = SIScalarFloatValue(prod);
    if (OCCompareFloatValues(result, 6.0f) != kOCCompareEqualTo) {
        printf("test_SIScalarCreateByMultiplying failed: Expected 6.0f, got %.6f\n", result);
        OCRelease(s1);
        OCRelease(s2);
        OCRelease(prod);
        return false;
    }

    OCRelease(s1);
    OCRelease(s2);
    OCRelease(prod);
    printf("test_SIScalarCreateByMultiplying passed\n");
    return true;
}

bool test_SIScalarMultiply(void) {
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
    SIUnitRef s_unit = SIUnitForUnderivedSymbol(STR("s"));
    if (!m || !s_unit) {
        printf("test_SIScalarMultiply failed: Failed to retrieve base units\n");
        return false;
    }

    SIMutableScalarRef target = SIScalarCreateMutableWithFloat(2.0f, m); // 2 m
    SIScalarRef s2 = SIScalarCreateWithFloat(3.0f, s_unit); // 3 s
    if (!target || !s2) {
        printf("test_SIScalarMultiply failed: Failed to create scalar operands\n");
        if (target) OCRelease(target);
        if (s2) OCRelease(s2);
        return false;
    }

    OCStringRef error = NULL;
    bool success = SIScalarMultiply(target, s2, &error);
    if (!success) {
        if (error) {
            printf("test_SIScalarMultiply failed: Multiplication error: %s\n", OCStringGetCString(error));
            OCRelease(error);
        } else {
            printf("test_SIScalarMultiply failed: Multiplication returned false with no error\n");
        }
        OCRelease(target);
        OCRelease(s2);
        return false;
    }

    if (error != NULL) {
        printf("test_SIScalarMultiply failed: Unexpected error string after successful multiplication\n");
        OCRelease(error);
        OCRelease(target);
        OCRelease(s2);
        return false;
    }

    float result = SIScalarFloatValue(target);
    if (OCCompareFloatValues(result, 6.0f) != kOCCompareEqualTo) {
        printf("test_SIScalarMultiply failed: Expected 6.0f, got %.6f\n", result);
        OCRelease(target);
        OCRelease(s2);
        return false;
    }

    OCRelease(target);
    OCRelease(s2);
    printf("test_SIScalarMultiply passed\n");
    return true;
}


bool test_SIScalarCreateByDividingWithoutReducingUnit(void) {
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
    SIUnitRef s_unit = SIUnitForUnderivedSymbol(STR("s"));
    if (!m || !s_unit) {
        printf("test_SIScalarCreateByDividingWithoutReducingUnit failed: Failed to retrieve base units\n");
        return false;
    }

    SIScalarRef s1 = SIScalarCreateWithFloat(6.0f, m); // 6 m
    SIScalarRef s2 = SIScalarCreateWithFloat(2.0f, s_unit); // 2 s
    if (!s1 || !s2) {
        printf("test_SIScalarCreateByDividingWithoutReducingUnit failed: Failed to create input scalars\n");
        if (s1) OCRelease(s1);
        if (s2) OCRelease(s2);
        return false;
    }

    OCStringRef error = NULL;
    SIScalarRef quot = SIScalarCreateByDividingWithoutReducingUnit(s1, s2, &error);
    if (!quot) {
        if (error) {
            printf("test_SIScalarCreateByDividingWithoutReducingUnit failed: Division error: %s\n", OCStringGetCString(error));
            OCRelease(error);
        } else {
            printf("test_SIScalarCreateByDividingWithoutReducingUnit failed: Division returned NULL with no error\n");
        }
        OCRelease(s1);
        OCRelease(s2);
        return false;
    }

    if (error != NULL) {
        printf("test_SIScalarCreateByDividingWithoutReducingUnit failed: Unexpected error after successful division\n");
        OCRelease(error);
        OCRelease(s1);
        OCRelease(s2);
        OCRelease(quot);
        return false;
    }

    float result = SIScalarFloatValue(quot);
    if (fabsf(result - 3.0f) >= 1e-6f) {
        printf("test_SIScalarCreateByDividingWithoutReducingUnit failed: Expected 3.0f, got %.6f\n", result);
        OCRelease(s1);
        OCRelease(s2);
        OCRelease(quot);
        return false;
    }

    OCRelease(s1);
    OCRelease(s2);
    OCRelease(quot);
    OCRelease(m);
    OCRelease(s_unit);
    printf("test_SIScalarCreateByDividingWithoutReducingUnit passed\n");
    return true;
}


bool test_SIScalarDivideWithoutReducingUnit(void) {
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
    SIUnitRef s_unit = SIUnitForUnderivedSymbol(STR("s"));
    if (!m || !s_unit) {
        printf("test_SIScalarDivideWithoutReducingUnit failed: Failed to retrieve base units\n");
        return false;
    }

    SIMutableScalarRef target = SIScalarCreateMutableWithFloat(6.0f, m); // 6 m
    SIScalarRef s2 = SIScalarCreateWithFloat(2.0f, s_unit); // 2 s
    if (!target || !s2) {
        printf("test_SIScalarDivideWithoutReducingUnit failed: Failed to create scalar operands\n");
        if (target) OCRelease(target);
        if (s2) OCRelease(s2);
        return false;
    }

    OCStringRef error = NULL;
    bool success = SIScalarDivideWithoutReducingUnit(target, s2, &error);
    if (!success) {
        if (error) {
            printf("test_SIScalarDivideWithoutReducingUnit failed: Division error: %s\n", OCStringGetCString(error));
            OCRelease(error);
        } else {
            printf("test_SIScalarDivideWithoutReducingUnit failed: Division returned false without error\n");
        }
        OCRelease(target);
        OCRelease(s2);
        return false;
    }

    if (error != NULL) {
        printf("test_SIScalarDivideWithoutReducingUnit failed: Unexpected error string after successful division\n");
        OCRelease(error);
        OCRelease(target);
        OCRelease(s2);
        return false;
    }

    float result = SIScalarFloatValue(target);
    if (fabsf(result - 3.0f) >= 1e-6f) {
        printf("test_SIScalarDivideWithoutReducingUnit failed: Expected 3.0f, got %.6f\n", result);
        OCRelease(target);
        OCRelease(s2);
        return false;
    }

    OCRelease(target);
    OCRelease(s2);
    OCRelease(m);
    OCRelease(s_unit);
    printf("test_SIScalarDivideWithoutReducingUnit passed\n");
    return true;
}

bool test_SIScalarCreateByDividing(void) {
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
    SIUnitRef s_unit = SIUnitForUnderivedSymbol(STR("s"));
    if (!m || !s_unit) {
        printf("test_SIScalarCreateByDividing failed: Failed to retrieve base units\n");
        return false;
    }

    SIScalarRef s1 = SIScalarCreateWithFloat(6.0f, m); // 6 m
    SIScalarRef s2 = SIScalarCreateWithFloat(2.0f, s_unit); // 2 s
    if (!s1 || !s2) {
        printf("test_SIScalarCreateByDividing failed: Failed to create scalar operands\n");
        if (s1) OCRelease(s1);
        if (s2) OCRelease(s2);
        return false;
    }

    OCStringRef error = NULL;
    SIScalarRef quot = SIScalarCreateByDividing(s1, s2, &error);
    if (!quot) {
        if (error) {
            printf("test_SIScalarCreateByDividing failed: Division error: %s\n", OCStringGetCString(error));
            OCRelease(error);
        } else {
            printf("test_SIScalarCreateByDividing failed: Division returned NULL without error message\n");
        }
        OCRelease(s1);
        OCRelease(s2);
        return false;
    }

    if (error != NULL) {
        printf("test_SIScalarCreateByDividing failed: Unexpected non-NULL error after successful division\n");
        OCRelease(error);
        OCRelease(s1);
        OCRelease(s2);
        OCRelease(quot);
        return false;
    }

    float result = SIScalarFloatValue(quot);
    if (fabsf(result - 3.0f) >= 1e-6f) {
        printf("test_SIScalarCreateByDividing failed: Expected 3.0f, got %.6f\n", result);
        OCRelease(s1);
        OCRelease(s2);
        OCRelease(quot);
        return false;
    }

    OCRelease(s1);
    OCRelease(s2);
    OCRelease(quot);
    OCRelease(m);
    OCRelease(s_unit);
    printf("test_SIScalarCreateByDividing passed\n");
    return true;
}

bool test_SIScalarDivide(void) {
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
    SIUnitRef s_unit = SIUnitForUnderivedSymbol(STR("s"));
    if (!m || !s_unit) {
        printf("test_SIScalarDivide failed: Failed to retrieve base units\n");
        return false;
    }

    SIMutableScalarRef target = SIScalarCreateMutableWithFloat(6.0f, m); // 6 m
    SIScalarRef s2 = SIScalarCreateWithFloat(2.0f, s_unit); // 2 s
    if (!target || !s2) {
        printf("test_SIScalarDivide failed: Failed to create scalar operands\n");
        if (target) OCRelease(target);
        if (s2) OCRelease(s2);
        return false;
    }

    OCStringRef error = NULL;
    bool success = SIScalarDivide(target, s2, &error);
    if (!success) {
        if (error) {
            printf("test_SIScalarDivide failed: Division error: %s\n", OCStringGetCString(error));
            OCRelease(error);
        } else {
            printf("test_SIScalarDivide failed: Division returned false without error message\n");
        }
        OCRelease(target);
        OCRelease(s2);
        return false;
    }

    if (error != NULL) {
        printf("test_SIScalarDivide failed: Unexpected non-NULL error string after successful division\n");
        OCRelease(error);
        OCRelease(target);
        OCRelease(s2);
        return false;
    }

    float result = SIScalarFloatValue(target);
    if (fabsf(result - 3.0f) >= 1e-6f) {
        printf("test_SIScalarDivide failed: Expected 3.0f, got %.6f\n", result);
        OCRelease(target);
        OCRelease(s2);
        return false;
    }

    OCRelease(target);
    OCRelease(s2);
    OCRelease(m);
    OCRelease(s_unit);
    printf("test_SIScalarDivide passed\n");
    return true;
}

bool test_SIScalarCreateByRaisingToPowerWithoutReducingUnit(void) {
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
    if (!m) {
        printf("test_SIScalarCreateByRaisingToPowerWithoutReducingUnit failed: Failed to retrieve unit 'm'\n");
        return false;
    }

    SIScalarRef s = SIScalarCreateWithFloat(2.0f, m); // 2 m
    if (!s) {
        printf("test_SIScalarCreateByRaisingToPowerWithoutReducingUnit failed: Failed to create base scalar\n");
        OCRelease(m);
        return false;
    }

    OCStringRef error = NULL;
    SIScalarRef pow_s = SIScalarCreateByRaisingToPowerWithoutReducingUnit(s, 3.0, &error);
    if (!pow_s) {
        if (error) {
            printf("test_SIScalarCreateByRaisingToPowerWithoutReducingUnit failed: Power operation error: %s\n", OCStringGetCString(error));
            OCRelease(error);
        } else {
            printf("test_SIScalarCreateByRaisingToPowerWithoutReducingUnit failed: Operation returned NULL without error\n");
        }
        OCRelease(s);
        OCRelease(m);
        return false;
    }

    if (error != NULL) {
        printf("test_SIScalarCreateByRaisingToPowerWithoutReducingUnit failed: Unexpected error string after success\n");
        OCRelease(error);
        OCRelease(s);
        OCRelease(pow_s);
        OCRelease(m);
        return false;
    }

    float result = SIScalarFloatValue(pow_s);
    if (fabsf(result - 8.0f) >= 1e-6f) {
        printf("test_SIScalarCreateByRaisingToPowerWithoutReducingUnit failed: Expected 8.0f, got %.6f\n", result);
        OCRelease(s);
        OCRelease(pow_s);
        OCRelease(m);
        return false;
    }

    OCRelease(s);
    OCRelease(pow_s);
    OCRelease(m);
    printf("test_SIScalarCreateByRaisingToPowerWithoutReducingUnit passed\n");
    return true;
}

bool test_SIScalarRaiseToAPowerWithoutReducingUnit(void) {
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
    if (!m) {
        printf("test_SIScalarRaiseToAPowerWithoutReducingUnit failed: Failed to retrieve unit 'm'\n");
        return false;
    }

    SIMutableScalarRef target = SIScalarCreateMutableWithFloat(2.0f, m); // 2 m
    if (!target) {
        printf("test_SIScalarRaiseToAPowerWithoutReducingUnit failed: Failed to create mutable scalar\n");
        OCRelease(m);
        return false;
    }

    OCStringRef error = NULL;
    bool success = SIScalarRaiseToAPowerWithoutReducingUnit(target, 3.0, &error);
    if (!success) {
        if (error) {
            printf("test_SIScalarRaiseToAPowerWithoutReducingUnit failed: Power operation error: %s\n", OCStringGetCString(error));
            OCRelease(error);
        } else {
            printf("test_SIScalarRaiseToAPowerWithoutReducingUnit failed: Operation returned false with no error\n");
        }
        OCRelease(target);
        OCRelease(m);
        return false;
    }

    if (error != NULL) {
        printf("test_SIScalarRaiseToAPowerWithoutReducingUnit failed: Unexpected non-NULL error string after success\n");
        OCRelease(error);
        OCRelease(target);
        OCRelease(m);
        return false;
    }

    float result = SIScalarFloatValue(target);
    if (fabsf(result - 8.0f) >= 1e-6f) {
        printf("test_SIScalarRaiseToAPowerWithoutReducingUnit failed: Expected 8.0f, got %.6f\n", result);
        OCRelease(target);
        OCRelease(m);
        return false;
    }

    OCRelease(target);
    OCRelease(m);
    printf("test_SIScalarRaiseToAPowerWithoutReducingUnit passed\n");
    return true;
}


bool test_SIScalarCreateByRaisingToPower(void) {
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
    if (!m) {
        printf("test_SIScalarCreateByRaisingToPower failed: Failed to retrieve unit 'm'\n");
        return false;
    }

    SIScalarRef s = SIScalarCreateWithFloat(2.0f, m); // 2 m
    if (!s) {
        printf("test_SIScalarCreateByRaisingToPower failed: Failed to create base scalar\n");
        OCRelease(m);
        return false;
    }

    OCStringRef error = NULL;
    SIScalarRef pow_s = SIScalarCreateByRaisingToPower(s, 3.0, &error);
    if (!pow_s) {
        if (error) {
            printf("test_SIScalarCreateByRaisingToPower failed: Power operation error: %s\n", OCStringGetCString(error));
            OCRelease(error);
        } else {
            printf("test_SIScalarCreateByRaisingToPower failed: Operation returned NULL without error message\n");
        }
        OCRelease(s);
        OCRelease(m);
        return false;
    }

    if (error != NULL) {
        printf("test_SIScalarCreateByRaisingToPower failed: Unexpected error string after successful operation\n");
        OCRelease(error);
        OCRelease(s);
        OCRelease(pow_s);
        OCRelease(m);
        return false;
    }

    float result = SIScalarFloatValue(pow_s);
    if (fabsf(result - 8.0f) >= 1e-6f) {
        printf("test_SIScalarCreateByRaisingToPower failed: Expected 8.0f, got %.6f\n", result);
        OCRelease(s);
        OCRelease(pow_s);
        OCRelease(m);
        return false;
    }

    OCRelease(s);
    OCRelease(pow_s);
    OCRelease(m);
    printf("test_SIScalarCreateByRaisingToPower passed\n");
    return true;
}

bool test_SIScalarRaiseToAPower(void) {
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
    if (!m) {
        printf("test_SIScalarRaiseToAPower failed: Failed to retrieve unit 'm'\n");
        return false;
    }

    SIMutableScalarRef target = SIScalarCreateMutableWithFloat(2.0f, m); // 2 m
    if (!target) {
        printf("test_SIScalarRaiseToAPower failed: Failed to create mutable scalar\n");
        OCRelease(m);
        return false;
    }

    OCStringRef error = NULL;
    bool success = SIScalarRaiseToAPower(target, 3.0, &error);
    if (!success) {
        if (error) {
            printf("test_SIScalarRaiseToAPower failed: Power operation error: %s\n", OCStringGetCString(error));
            OCRelease(error);
        } else {
            printf("test_SIScalarRaiseToAPower failed: Operation returned false with no error message\n");
        }
        OCRelease(target);
        OCRelease(m);
        return false;
    }

    if (error != NULL) {
        printf("test_SIScalarRaiseToAPower failed: Unexpected error string after success\n");
        OCRelease(error);
        OCRelease(target);
        OCRelease(m);
        return false;
    }

    float result = SIScalarFloatValue(target);
    if (fabsf(result - 8.0f) >= 1e-6f) {
        printf("test_SIScalarRaiseToAPower failed: Expected 8.0f, got %.6f\n", result);
        OCRelease(target);
        OCRelease(m);
        return false;
    }

    OCRelease(target);
    OCRelease(m);
    printf("test_SIScalarRaiseToAPower passed\n");
    return true;
}


bool test_SIScalarCreateByTakingAbsoluteValue(void) {
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
    if (!m) {
        printf("test_SIScalarCreateByTakingAbsoluteValue failed: Failed to retrieve unit 'm'\n");
        return false;
    }

    SIScalarRef s_neg = SIScalarCreateWithFloat(-5.0f, m);
    if (!s_neg) {
        printf("test_SIScalarCreateByTakingAbsoluteValue failed: Failed to create negative scalar\n");
        OCRelease(m);
        return false;
    }

    OCStringRef error = NULL;
    SIScalarRef s_abs = SIScalarCreateByTakingAbsoluteValue(s_neg, &error);
    if (!s_abs) {
        if (error) {
            printf("test_SIScalarCreateByTakingAbsoluteValue failed: Absolute value error: %s\n", OCStringGetCString(error));
            OCRelease(error);
        } else {
            printf("test_SIScalarCreateByTakingAbsoluteValue failed: Operation returned NULL without error\n");
        }
        OCRelease(s_neg);
        OCRelease(m);
        return false;
    }

    if (error != NULL) {
        printf("test_SIScalarCreateByTakingAbsoluteValue failed: Unexpected error string after successful operation\n");
        OCRelease(error);
        OCRelease(s_neg);
        OCRelease(s_abs);
        OCRelease(m);
        return false;
    }

    float result = SIScalarFloatValue(s_abs);
    if (fabsf(result - 5.0f) >= 1e-6f) {
        printf("test_SIScalarCreateByTakingAbsoluteValue failed: Expected 5.0f, got %.6f\n", result);
        OCRelease(s_neg);
        OCRelease(s_abs);
        OCRelease(m);
        return false;
    }

    OCRelease(s_neg);
    OCRelease(s_abs);
    OCRelease(m);
    printf("test_SIScalarCreateByTakingAbsoluteValue passed\n");
    return true;
}


bool test_SIScalarTakeAbsoluteValue(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    if (!u) {
        printf("test_SIScalarTakeAbsoluteValue failed: Failed to retrieve unit 'm'\n");
        return false;
    }

    SIMutableScalarRef target = SIScalarCreateMutableWithFloat(-5.0f, u);
    if (!target) {
        printf("test_SIScalarTakeAbsoluteValue failed: Failed to create mutable scalar\n");
        OCRelease(u);
        return false;
    }

    OCStringRef error = NULL;
    bool success = SIScalarTakeAbsoluteValue(target, &error);
    if (!success) {
        if (error) {
            printf("test_SIScalarTakeAbsoluteValue failed: Absolute value error: %s\n", OCStringGetCString(error));
            OCRelease(error);
        } else {
            printf("test_SIScalarTakeAbsoluteValue failed: Operation returned false without error\n");
        }
        OCRelease(target);
        OCRelease(u);
        return false;
    }

    if (error != NULL) {
        printf("test_SIScalarTakeAbsoluteValue failed: Unexpected error string after successful operation\n");
        OCRelease(error);
        OCRelease(target);
        OCRelease(u);
        return false;
    }

    float result = SIScalarFloatValue(target);
    if (fabsf(result - 5.0f) >= 1e-6f) {
        printf("test_SIScalarTakeAbsoluteValue failed: Expected 5.0f, got %.6f\n", result);
        OCRelease(target);
        OCRelease(u);
        return false;
    }

    OCRelease(target);
    OCRelease(u);
    printf("test_SIScalarTakeAbsoluteValue passed\n");
    return true;
}



bool test_SIScalarCreateByGammaFunctionWithoutReducingUnit(void) {
    SIUnitRef dimensionless = SIUnitDimensionlessAndUnderived();
    if (!dimensionless) {
        printf("test_SIScalarCreateByGammaFunctionWithoutReducingUnit failed: Failed to retrieve dimensionless unit\n");
        return false;
    }

    OCStringRef error = NULL;

    // Test case 1: Gamma(4) = 24.0 assuming Γ(n) = n!
    SIScalarRef s4 = SIScalarCreateWithFloat(4.0f, dimensionless);
    if (!s4) {
        printf("test_SIScalarCreateByGammaFunctionWithoutReducingUnit failed: Failed to create scalar 4.0\n");
        return false;
    }

    SIScalarRef gamma_4 = SIScalarCreateByGammaFunctionWithoutReducingUnit(s4, &error);
    if (!gamma_4 || error) {
        printf("test_SIScalarCreateByGammaFunctionWithoutReducingUnit failed: Gamma(4.0) returned error or NULL\n");
        if (error) {
            printf("Error: %s\n", OCStringGetCString(error));
            OCRelease(error);
        }
        OCRelease(s4);
        if (gamma_4) OCRelease(gamma_4);
        return false;
    }

    float gamma_4_value = SIScalarFloatValue(gamma_4);
    if (OCCompareFloatValues(gamma_4_value, 24.0f) != kOCCompareEqualTo) {
        printf("test_SIScalarCreateByGammaFunctionWithoutReducingUnit failed: Expected Gamma(4) = 24.0, got %.6f\n", gamma_4_value);
        OCRelease(s4);
        OCRelease(gamma_4);
        return false;
    }

    if (!SIUnitIsDimensionless(SIQuantityGetUnit((SIQuantityRef)gamma_4))) {
        printf("test_SIScalarCreateByGammaFunctionWithoutReducingUnit failed: Result unit is not dimensionless\n");
        OCRelease(s4);
        OCRelease(gamma_4);
        return false;
    }

    // Test case 2: Gamma(5) = 120.0
    SIScalarRef s5 = SIScalarCreateWithFloat(5.0f, dimensionless);
    if (!s5) {
        printf("test_SIScalarCreateByGammaFunctionWithoutReducingUnit failed: Failed to create scalar 5.0\n");
        OCRelease(s4);
        OCRelease(gamma_4);
        return false;
    }

    SIScalarRef gamma_5 = SIScalarCreateByGammaFunctionWithoutReducingUnit(s5, &error);
    if (!gamma_5 || error) {
        printf("test_SIScalarCreateByGammaFunctionWithoutReducingUnit failed: Gamma(5.0) returned error or NULL\n");
        if (error) {
            printf("Error: %s\n", OCStringGetCString(error));
            OCRelease(error);
        }
        OCRelease(s4);
        OCRelease(gamma_4);
        OCRelease(s5);
        if (gamma_5) OCRelease(gamma_5);
        return false;
    }

    float gamma_5_value = SIScalarFloatValue(gamma_5);
    if (OCCompareFloatValues(gamma_5_value, 120.0f) != kOCCompareEqualTo) {
        printf("test_SIScalarCreateByGammaFunctionWithoutReducingUnit failed: Expected Gamma(5) = 120.0, got %.6f\n", gamma_5_value);
        OCRelease(s4);
        OCRelease(gamma_4);
        OCRelease(s5);
        OCRelease(gamma_5);
        return false;
    }

    OCRelease(s4);
    OCRelease(gamma_4);
    OCRelease(s5);
    OCRelease(gamma_5);

    printf("test_SIScalarCreateByGammaFunctionWithoutReducingUnit passed\n");
    return true;
}

bool test_SIScalarCreateByTakingNthRoot(void) {
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
    if (!m) {
        printf("test_SIScalarCreateByTakingNthRoot failed: Failed to retrieve unit 'm'\n");
        return false;
    }

    double mult = 1.0;
    SIUnitRef m_cubed_unit = SIUnitByRaisingToPower(m, 3, &mult, NULL);
    if (!m_cubed_unit) {
        printf("test_SIScalarCreateByTakingNthRoot failed: Failed to create m^3 unit\n");
        OCRelease(m);
        return false;
    }

    SIScalarRef s = SIScalarCreateWithFloat(8.0f, m_cubed_unit); // 8 m^3
    if (!s) {
        printf("test_SIScalarCreateByTakingNthRoot failed: Failed to create scalar with m^3\n");
        OCRelease(m);
        OCRelease(m_cubed_unit);
        return false;
    }

    OCStringRef error = NULL;
    SIScalarRef root_s = SIScalarCreateByTakingNthRoot(s, 3, &error);
    if (!root_s) {
        if (error) {
            printf("test_SIScalarCreateByTakingNthRoot failed: Root operation error: %s\n", OCStringGetCString(error));
            OCRelease(error);
        } else {
            printf("test_SIScalarCreateByTakingNthRoot failed: Root operation returned NULL without error\n");
        }
        OCRelease(s);
        OCRelease(m);
        OCRelease(m_cubed_unit);
        return false;
    }

    if (error != NULL) {
        printf("test_SIScalarCreateByTakingNthRoot failed: Unexpected error after successful operation\n");
        OCRelease(error);
        OCRelease(s);
        OCRelease(root_s);
        OCRelease(m);
        OCRelease(m_cubed_unit);
        return false;
    }

    float value = SIScalarFloatValue(root_s);
    if (fabsf(value - 2.0f) >= 1e-6f) {
        printf("test_SIScalarCreateByTakingNthRoot failed: Expected 2.0f, got %.6f\n", value);
        OCRelease(s);
        OCRelease(root_s);
        OCRelease(m);
        OCRelease(m_cubed_unit);
        return false;
    }

    OCStringRef unit_str = SIScalarCreateUnitString(root_s);
    if (!OCStringEqual(unit_str, STR("m"))) {
        printf("test_SIScalarCreateByTakingNthRoot failed: Expected unit 'm', got '%s'\n", OCStringGetCString(unit_str));
        OCRelease(unit_str);
        OCRelease(s);
        OCRelease(root_s);
        OCRelease(m);
        OCRelease(m_cubed_unit);
        return false;
    }

    OCRelease(unit_str);
    OCRelease(s);
    OCRelease(root_s);
    OCRelease(m);
    OCRelease(m_cubed_unit);
    printf("test_SIScalarCreateByTakingNthRoot passed\n");
    return true;
}

bool test_SIScalarTakeNthRoot(void) {
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
    if (!m) {
        printf("test_SIScalarTakeNthRoot failed: Failed to retrieve unit 'm'\n");
        return false;
    }

    double mult = 1.0;
    SIUnitRef m_cubed_unit = SIUnitByRaisingToPower(m, 3, &mult, NULL);
    if (!m_cubed_unit) {
        printf("test_SIScalarTakeNthRoot failed: Failed to create m^3 unit\n");
        OCRelease(m);
        return false;
    }

    SIMutableScalarRef target = SIScalarCreateMutableWithFloat(8.0f, m_cubed_unit); // 8 m^3
    if (!target) {
        printf("test_SIScalarTakeNthRoot failed: Failed to create scalar\n");
        OCRelease(m);
        OCRelease(m_cubed_unit);
        return false;
    }

    OCStringRef error = NULL;
    bool success = SIScalarTakeNthRoot(target, 3, &error);
    if (!success) {
        if (error) {
            printf("test_SIScalarTakeNthRoot failed: Root operation error: %s\n", OCStringGetCString(error));
            OCRelease(error);
        } else {
            printf("test_SIScalarTakeNthRoot failed: Operation returned false with no error message\n");
        }
        OCRelease(target);
        OCRelease(m);
        OCRelease(m_cubed_unit);
        return false;
    }

    if (error != NULL) {
        printf("test_SIScalarTakeNthRoot failed: Unexpected error string after successful operation\n");
        OCRelease(error);
        OCRelease(target);
        OCRelease(m);
        OCRelease(m_cubed_unit);
        return false;
    }

    float result = SIScalarFloatValue(target);
    if (fabsf(result - 2.0f) >= 1e-6f) {
        printf("test_SIScalarTakeNthRoot failed: Expected 2.0f, got %.6f\n", result);
        OCRelease(target);
        OCRelease(m);
        OCRelease(m_cubed_unit);
        return false;
    }

    OCStringRef unit_str = SIScalarCreateUnitString(target);
    if (!OCStringEqual(unit_str, STR("m"))) {
        printf("test_SIScalarTakeNthRoot failed: Expected unit 'm', got '%s'\n", OCStringGetCString(unit_str));
        OCRelease(unit_str);
        OCRelease(target);
        OCRelease(m);
        OCRelease(m_cubed_unit);
        return false;
    }

    OCRelease(unit_str);
    OCRelease(target);
    OCRelease(m);
    OCRelease(m_cubed_unit);
    printf("test_SIScalarTakeNthRoot passed\n");
    return true;
}


bool test_SIScalarTakeLog10(void) {
    // log10 is typically for dimensionless quantities
    SIUnitRef dimensionless = SIUnitDimensionlessAndUnderived();
    if (!dimensionless) {
        printf("test_SIScalarTakeLog10 failed: Failed to retrieve dimensionless unit\n");
        return false;
    }

    SIMutableScalarRef target = SIScalarCreateMutableWithFloat(100.0f, dimensionless);
    if (!target) {
        printf("test_SIScalarTakeLog10 failed: Failed to create scalar\n");
        OCRelease(dimensionless);
        return false;
    }

    OCStringRef error = NULL;
    bool success = SIScalarTakeLog10(target, &error);
    if (!success) {
        if (error) {
            printf("test_SIScalarTakeLog10 failed: Log10 error: %s\n", OCStringGetCString(error));
            OCRelease(error);
        } else {
            printf("test_SIScalarTakeLog10 failed: Operation returned false with no error message\n");
        }
        OCRelease(target);
        OCRelease(dimensionless);
        return false;
    }

    if (error != NULL) {
        printf("test_SIScalarTakeLog10 failed: Unexpected error string after success\n");
        OCRelease(error);
        OCRelease(target);
        OCRelease(dimensionless);
        return false;
    }

    float value = SIScalarFloatValue(target);
    if (fabsf(value - 2.0f) >= 1e-6f) {
        printf("test_SIScalarTakeLog10 failed: Expected 2.0f, got %.6f\n", value);
        OCRelease(target);
        OCRelease(dimensionless);
        return false;
    }

    SIUnitRef final_unit = (SIUnitRef)SIQuantityGetUnit((SIQuantityRef)target);
    if (!SIUnitIsDimensionless(final_unit)) {
        printf("test_SIScalarTakeLog10 failed: Resulting unit is not dimensionless\n");
        OCRelease(target);
        OCRelease(dimensionless);
        return false;
    }

    OCRelease(target);
    OCRelease(dimensionless);
    printf("test_SIScalarTakeLog10 passed\n");
    return true;
}

bool test_SIScalarCreateByZeroingPart(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    if (!u) {
        printf("test_SIScalarCreateByZeroingPart failed: Failed to retrieve unit 'm'\n");
        return false;
    }

    SIScalarRef s_complex = SIScalarCreateWithFloatComplex(3.0f + 4.0f * I, u);
    if (!s_complex) {
        printf("test_SIScalarCreateByZeroingPart failed: Failed to create complex scalar\n");
        return false;
    }

    SIScalarRef s_zero_imag = SIScalarCreateByZeroingPart(s_complex, kSIImaginaryPart);
    if (!s_zero_imag) {
        printf("test_SIScalarCreateByZeroingPart failed: Failed to zero imaginary part\n");
        OCRelease(s_complex);
        return false;
    }

    float complex val = SIScalarFloatComplexValue(s_zero_imag);
    if (fabsf(crealf(val) - 3.0f) >= 1e-6f) {
        printf("test_SIScalarCreateByZeroingPart failed: Real part mismatch (expected 3.0, got %.6f)\n", crealf(val));
        OCRelease(s_complex);
        OCRelease(s_zero_imag);
        return false;
    }

    if (fabsf(cimagf(val)) >= 1e-6f) {
        printf("test_SIScalarCreateByZeroingPart failed: Imaginary part not zeroed (got %.6f)\n", cimagf(val));
        OCRelease(s_complex);
        OCRelease(s_zero_imag);
        return false;
    }

    OCRelease(s_complex);
    OCRelease(s_zero_imag);
    printf("test_SIScalarCreateByZeroingPart passed\n");
    return true;
}

bool test_SIScalarZeroPart(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    if (!u) {
        printf("test_SIScalarZeroPart failed: Failed to retrieve unit 'm'\n");
        return false;
    }

    SIMutableScalarRef m = SIScalarCreateMutableWithFloatComplex(3.0f + 4.0f * I, u);
    if (!m) {
        printf("test_SIScalarZeroPart failed: Failed to create mutable complex scalar\n");
        return false;
    }

    bool success = SIScalarZeroPart(m, kSIImaginaryPart);
    if (!success) {
        printf("test_SIScalarZeroPart failed: Operation returned false\n");
        OCRelease(m);
        return false;
    }

    float complex val = SIScalarFloatComplexValue(m);
    if (fabsf(crealf(val) - 3.0f) >= 1e-6f) {
        printf("test_SIScalarZeroPart failed: Real part mismatch (expected 3.0, got %.6f)\n", crealf(val));
        OCRelease(m);
        return false;
    }

    if (fabsf(cimagf(val)) >= 1e-6f) {
        printf("test_SIScalarZeroPart failed: Imaginary part not zeroed (got %.6f)\n", cimagf(val));
        OCRelease(m);
        return false;
    }

    OCRelease(m);
    printf("test_SIScalarZeroPart passed\n");
    return true;
}

bool test_SIScalarMultiplyByDimensionlessRealConstant(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    if (!u) {
        printf("test_SIScalarMultiplyByDimensionlessRealConstant failed: Failed to retrieve unit 'm'\n");
        return false;
    }

    SIMutableScalarRef m = SIScalarCreateMutableWithFloat(5.0f, u);
    if (!m) {
        printf("test_SIScalarMultiplyByDimensionlessRealConstant failed: Failed to create mutable scalar\n");
        return false;
    }

    bool success = SIScalarMultiplyByDimensionlessRealConstant(m, 2.5);
    if (!success) {
        printf("test_SIScalarMultiplyByDimensionlessRealConstant failed: Operation returned false\n");
        OCRelease(m);
        return false;
    }

    float result = SIScalarFloatValue(m);
    if (fabsf(result - 12.5f) >= 1e-6f) {
        printf("test_SIScalarMultiplyByDimensionlessRealConstant failed: Expected 12.5f, got %.6f\n", result);
        OCRelease(m);
        return false;
    }

    OCRelease(m);
    printf("test_SIScalarMultiplyByDimensionlessRealConstant passed\n");
    return true;
}

bool test_SIScalarCreateByMultiplyingByDimensionlessRealConstant(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    if (!u) {
        printf("test_SIScalarCreateByMultiplyingByDimensionlessRealConstant failed: Failed to retrieve unit 'm'\n");
        return false;
    }

    SIScalarRef s_orig = SIScalarCreateWithFloat(5.0f, u);
    if (!s_orig) {
        printf("test_SIScalarCreateByMultiplyingByDimensionlessRealConstant failed: Failed to create original scalar\n");
        return false;
    }

    SIScalarRef s_scaled = SIScalarCreateByMultiplyingByDimensionlessRealConstant(s_orig, 2.5);
    if (!s_scaled) {
        printf("test_SIScalarCreateByMultiplyingByDimensionlessRealConstant failed: Operation returned NULL\n");
        OCRelease(s_orig);
        return false;
    }

    float result = SIScalarFloatValue(s_scaled);
    if (fabsf(result - 12.5f) >= 1e-6f) {
        printf("test_SIScalarCreateByMultiplyingByDimensionlessRealConstant failed: Expected 12.5f, got %.6f\n", result);
        OCRelease(s_orig);
        OCRelease(s_scaled);
        return false;
    }

    OCRelease(s_orig);
    OCRelease(s_scaled);
    printf("test_SIScalarCreateByMultiplyingByDimensionlessRealConstant passed\n");
    return true;
}

bool test_SIScalarMultiplyByDimensionlessComplexConstant(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    if (!u) {
        printf("test_SIScalarMultiplyByDimensionlessComplexConstant failed: Failed to retrieve unit 'm'\n");
        return false;
    }

    SIMutableScalarRef m = SIScalarCreateMutableWithFloat(2.0f, u); // 2 m
    if (!m) {
        printf("test_SIScalarMultiplyByDimensionlessComplexConstant failed: Failed to create mutable scalar\n");
        return false;
    }

    double complex factor = 1.0 + 1.0 * I;
    bool success = SIScalarMultiplyByDimensionlessComplexConstant(m, factor);
    if (!success) {
        printf("test_SIScalarMultiplyByDimensionlessComplexConstant failed: Operation returned false\n");
        OCRelease(m);
        return false;
    }

    float complex result = SIScalarFloatComplexValue(m);
    if (fabsf(crealf(result) - 2.0f) >= 1e-6f) {
        printf("test_SIScalarMultiplyByDimensionlessComplexConstant failed: Real part mismatch (expected 2.0, got %.6f)\n", crealf(result));
        OCRelease(m);
        return false;
    }

    if (fabsf(cimagf(result) - 2.0f) >= 1e-6f) {
        printf("test_SIScalarMultiplyByDimensionlessComplexConstant failed: Imaginary part mismatch (expected 2.0, got %.6f)\n", cimagf(result));
        OCRelease(m);
        return false;
    }

    OCRelease(m);
    printf("test_SIScalarMultiplyByDimensionlessComplexConstant passed\n");
    return true;
}


bool test_SIScalarCreateByMultiplyingByDimensionlessComplexConstant(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    if (!u) {
        printf("test_SIScalarCreateByMultiplyingByDimensionlessComplexConstant failed: Failed to retrieve unit 'm'\n");
        return false;
    }

    // Test case 1: Multiply real scalar by (1+i)
    SIMutableScalarRef m = SIScalarCreateMutableWithFloat(2.0f, u); // 2 m
    if (!m) {
        printf("test_SIScalarCreateByMultiplyingByDimensionlessComplexConstant failed: Failed to create first scalar\n");
        OCRelease(u);
        return false;
    }

    double complex factor = 1.0 + 1.0 * I; // (1+i)
    bool success = SIScalarMultiplyByDimensionlessComplexConstant(m, factor);
    if (!success) {
        printf("test_SIScalarCreateByMultiplyingByDimensionlessComplexConstant failed: Multiplication by (1+i) failed\n");
        OCRelease(m);
        OCRelease(u);
        return false;
    }

    float real_part = crealf(SIScalarFloatComplexValue(m));
    float imag_part = cimagf(SIScalarFloatComplexValue(m));

    if (fabsf(real_part - 2.0f) >= 1e-6f || fabsf(imag_part - 2.0f) >= 1e-6f) {
        printf("test_SIScalarCreateByMultiplyingByDimensionlessComplexConstant failed: Expected (2+2i), got (%.6f+%.6fi)\n", real_part, imag_part);
        OCRelease(m);
        OCRelease(u);
        return false;
    }
    OCRelease(m);

    // Test case 2: Multiply by purely imaginary factor
    m = SIScalarCreateMutableWithFloat(3.0f, u); // 3 m
    if (!m) {
        printf("test_SIScalarCreateByMultiplyingByDimensionlessComplexConstant failed: Failed to create second scalar\n");
        OCRelease(u);
        return false;
    }

    factor = 0.0 + 2.0 * I; // 2i
    success = SIScalarMultiplyByDimensionlessComplexConstant(m, factor);
    if (!success) {
        printf("test_SIScalarCreateByMultiplyingByDimensionlessComplexConstant failed: Multiplication by 2i failed\n");
        OCRelease(m);
        OCRelease(u);
        return false;
    }

    real_part = crealf(SIScalarFloatComplexValue(m));
    imag_part = cimagf(SIScalarFloatComplexValue(m));

    if (fabsf(real_part) >= 1e-6f || fabsf(imag_part - 6.0f) >= 1e-6f) {
        printf("test_SIScalarCreateByMultiplyingByDimensionlessComplexConstant failed: Expected (0+6i), got (%.6f+%.6fi)\n", real_part, imag_part);
        OCRelease(m);
        OCRelease(u);
        return false;
    }

    OCRelease(m);
    OCRelease(u);
    printf("test_SIScalarCreateByMultiplyingByDimensionlessComplexConstant passed\n");
    return true;
}


bool test_SIScalarCreateByConjugation(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    if (!u) {
        printf("test_SIScalarCreateByConjugation failed: Failed to retrieve unit 'm'\n");
        return false;
    }

    SIScalarRef s_complex = SIScalarCreateWithFloatComplex(3.0f + 4.0f * I, u);
    if (!s_complex) {
        printf("test_SIScalarCreateByConjugation failed: Failed to create complex scalar\n");
        return false;
    }

    SIScalarRef s_conj = SIScalarCreateByConjugation(s_complex);
    if (!s_conj) {
        printf("test_SIScalarCreateByConjugation failed: Conjugation returned NULL\n");
        OCRelease(s_complex);
        return false;
    }

    float complex val = SIScalarFloatComplexValue(s_conj);
    if (fabsf(crealf(val) - 3.0f) >= 1e-6f) {
        printf("test_SIScalarCreateByConjugation failed: Real part mismatch (expected 3.0, got %.6f)\n", crealf(val));
        OCRelease(s_complex);
        OCRelease(s_conj);
        return false;
    }

    if (fabsf(cimagf(val) + 4.0f) >= 1e-6f) {
        printf("test_SIScalarCreateByConjugation failed: Imaginary part mismatch (expected -4.0, got %.6f)\n", cimagf(val));
        OCRelease(s_complex);
        OCRelease(s_conj);
        return false;
    }

    OCRelease(s_complex);
    OCRelease(s_conj);
    printf("test_SIScalarCreateByConjugation passed\n");
    return true;
}

bool test_SIScalarConjugate(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    if (!u) {
        printf("test_SIScalarConjugate failed: Failed to retrieve unit 'm'\n");
        return false;
    }

    SIMutableScalarRef m = SIScalarCreateMutableWithFloatComplex(3.0f + 4.0f * I, u);
    if (!m) {
        printf("test_SIScalarConjugate failed: Failed to create mutable complex scalar\n");
        return false;
    }

    bool success = SIScalarConjugate(m);
    if (!success) {
        printf("test_SIScalarConjugate failed: Conjugation operation returned false\n");
        OCRelease(m);
        return false;
    }

    float complex val = SIScalarFloatComplexValue(m);
    if (fabsf(crealf(val) - 3.0f) >= 1e-6f) {
        printf("test_SIScalarConjugate failed: Real part mismatch (expected 3.0, got %.6f)\n", crealf(val));
        OCRelease(m);
        return false;
    }

    if (fabsf(cimagf(val) + 4.0f) >= 1e-6f) {
        printf("test_SIScalarConjugate failed: Imaginary part mismatch (expected -4.0, got %.6f)\n", cimagf(val));
        OCRelease(m);
        return false;
    }

    OCRelease(m);
    printf("test_SIScalarConjugate passed\n");
    return true;
}

bool test_SIScalarShow(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    if (!u) {
        printf("test_SIScalarShow failed: Failed to retrieve unit 'm'\n");
        return false;
    }

    SIScalarRef s = SIScalarCreateWithFloat(1.23f, u);
    if (!s) {
        printf("test_SIScalarShow failed: Failed to create scalar\n");
        return false;
    }

    printf("Testing SIScalarShow (output below): ");
    SIScalarShow(s);  // Visual check for output; ensure no crash
    printf("\n");

    OCRelease(s);
    printf("test_SIScalarShow passed\n");
    return true;
}

bool test_SIScalarCreateStringValue(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    if (!u) {
        printf("test_SIScalarCreateStringValue failed: Failed to retrieve unit 'm'\n");
        return false;
    }

    SIScalarRef s = SIScalarCreateWithFloat(1.23f, u);
    if (!s) {
        printf("test_SIScalarCreateStringValue failed: Failed to create scalar\n");
        return false;
    }

    OCStringRef str_val = SIScalarCreateStringValue(s);
    if (!str_val) {
        printf("test_SIScalarCreateStringValue failed: Failed to create string value from scalar\n");
        OCRelease(s);
        return false;
    }

    // Optional: Print the string value for visual confirmation
    // OCStringShow(str_val);

    OCRelease(str_val);
    OCRelease(s);
    printf("test_SIScalarCreateStringValue passed\n");
    return true;
}

bool test_SIScalarCreateNumericStringValue(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    if (!u) {
        printf("test_SIScalarCreateNumericStringValue failed: Failed to retrieve unit 'm'\n");
        return false;
    }

    SIScalarRef s = SIScalarCreateWithFloat(1.23f, u);
    if (!s) {
        printf("test_SIScalarCreateNumericStringValue failed: Failed to create scalar\n");
        return false;
    }

    OCStringRef num_str_val = SIScalarCreateNumericStringValue(s);
    if (!num_str_val) {
        printf("test_SIScalarCreateNumericStringValue failed: Failed to create numeric string value from scalar\n");
        OCRelease(s);
        return false;
    }

    // Optional: Uncomment to visually verify output
    // OCStringShow(num_str_val);

    OCRelease(num_str_val);
    OCRelease(s);
    printf("test_SIScalarCreateNumericStringValue passed\n");
    return true;
}


bool test_SIScalarCreateStringValueForPart(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    if (!u) {
        printf("test_SIScalarCreateStringValueForPart failed: Failed to retrieve unit 'm'\n");
        return false;
    }

    SIScalarRef s = SIScalarCreateWithFloatComplex(1.0f + 2.0f * I, u);
    if (!s) {
        printf("test_SIScalarCreateStringValueForPart failed: Failed to create complex scalar\n");
        return false;
    }

    OCStringRef real_part_str = SIScalarCreateStringValueForPart(s, kSIRealPart);
    if (!real_part_str) {
        printf("test_SIScalarCreateStringValueForPart failed: Failed to create string for real part\n");
        OCRelease(s);
        return false;
    }

    // Optional: Uncomment to view output
    // OCStringShow(real_part_str);

    OCRelease(real_part_str);
    OCRelease(s);
    printf("test_SIScalarCreateStringValueForPart passed\n");
    return true;
}

bool test_SIScalarCreateUnitString(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    if (!u) {
        printf("test_SIScalarCreateUnitString failed: Failed to retrieve unit 'm'\n");
        return false;
    }

    SIScalarRef s = SIScalarCreateWithFloat(1.0f, u);
    if (!s) {
        printf("test_SIScalarCreateUnitString failed: Failed to create scalar\n");
        return false;
    }

    OCStringRef unit_str = SIScalarCreateUnitString(s);
    if (!unit_str) {
        printf("test_SIScalarCreateUnitString failed: Failed to create unit string\n");
        OCRelease(s);
        return false;
    }

    if (!OCStringEqual(unit_str, STR("m"))) {
        printf("test_SIScalarCreateUnitString failed: Unit string mismatch (expected 'm')\n");
        OCRelease(unit_str);
        OCRelease(s);
        return false;
    }

    OCRelease(unit_str);
    OCRelease(s);
    printf("test_SIScalarCreateUnitString passed\n");
    return true;
}


bool test_SIScalarCreateStringValueWithFormat(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    if (!u) {
        printf("test_SIScalarCreateStringValueWithFormat failed: Failed to retrieve unit 'm'\n");
        return false;
    }

    SIScalarRef s = SIScalarCreateWithFloat(1.2345f, u);
    if (!s) {
        printf("test_SIScalarCreateStringValueWithFormat failed: Failed to create scalar\n");
        return false;
    }

    OCStringRef fmt = STR("%.2f %s");
    OCStringRef str_val = SIScalarCreateStringValueWithFormat(s, fmt);
    if (!str_val) {
        printf("test_SIScalarCreateStringValueWithFormat failed: Failed to create formatted string\n");
        OCRelease(fmt);
        OCRelease(s);
        return false;
    }

    // Optional: Uncomment to visually verify output
    // OCStringShow(str_val);

    OCRelease(fmt);
    OCRelease(str_val);
    OCRelease(s);
    printf("test_SIScalarCreateStringValueWithFormat passed\n");
    return true;
}

bool test_SIScalarCreateNumericStringValueWithFormat(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    if (!u) {
        printf("test_SIScalarCreateNumericStringValueWithFormat failed: Failed to retrieve unit 'm'\n");
        return false;
    }

    SIScalarRef s = SIScalarCreateWithFloat(1.2345f, u);
    if (!s) {
        printf("test_SIScalarCreateNumericStringValueWithFormat failed: Failed to create scalar\n");
        return false;
    }

    OCStringRef fmt = STR("%.2f");
    OCStringRef num_str_val = SIScalarCreateNumericStringValueWithFormat(s, fmt);
    if (!num_str_val) {
        printf("test_SIScalarCreateNumericStringValueWithFormat failed: Failed to create formatted numeric string\n");
        OCRelease(fmt);
        OCRelease(s);
        return false;
    }

    // Optional: Uncomment to visually inspect the result
    // OCStringShow(num_str_val);

    OCRelease(fmt);
    OCRelease(num_str_val);
    OCRelease(s);
    printf("test_SIScalarCreateNumericStringValueWithFormat passed\n");
    return true;
}

bool test_SIScalarAddToArrayAsStringValue(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    if (!u) {
        printf("test_SIScalarAddToArrayAsStringValue failed: Failed to retrieve unit 'm'\n");
        return false;
    }

    SIScalarRef s = SIScalarCreateWithFloat(1.0f, u);
    if (!s) {
        printf("test_SIScalarAddToArrayAsStringValue failed: Failed to create scalar\n");
        return false;
    }

    OCMutableArrayRef array = OCArrayCreateMutable(0, &kOCTypeArrayCallBacks);
    if (!array) {
        printf("test_SIScalarAddToArrayAsStringValue failed: Failed to create array\n");
        OCRelease(s);
        return false;
    }

    SIScalarAddToArrayAsStringValue(s, array);
    if (OCArrayGetCount(array) != 1) {
        printf("test_SIScalarAddToArrayAsStringValue failed: Expected array count 1, got %lu\n", (unsigned long)OCArrayGetCount(array));
        OCRelease(array);
        OCRelease(s);
        return false;
    }

    OCStringRef str_in_array = (OCStringRef)OCArrayGetValueAtIndex(array, 0);
    if (!str_in_array) {
        printf("test_SIScalarAddToArrayAsStringValue failed: String in array is NULL\n");
        OCRelease(array);
        OCRelease(s);
        return false;
    }

    OCRelease(array);
    OCRelease(s);
    printf("test_SIScalarAddToArrayAsStringValue passed\n");
    return true;
}

#include <complex.h>

bool test_SIScalarIsReal(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    if (!u) {
        printf("test_SIScalarIsReal failed: Failed to retrieve unit 'm'\n");
        return false;
    }

    SIScalarRef s_real = SIScalarCreateWithFloat(1.0f, u);
    SIScalarRef s_complex = SIScalarCreateWithFloatComplex(1.0f + 0.001f * I, u);
    SIScalarRef s_pure_imag = SIScalarCreateWithFloatComplex(0.0f + 1.0f * I, u);

    if (!s_real || !s_complex || !s_pure_imag) {
        printf("test_SIScalarIsReal failed: Failed to create one or more scalars\n");
        if (s_real) OCRelease(s_real);
        if (s_complex) OCRelease(s_complex);
        if (s_pure_imag) OCRelease(s_pure_imag);
        return false;
    }

    if (!SIScalarIsReal(s_real)) {
        printf("test_SIScalarIsReal failed: Real scalar incorrectly reported as non-real\n");
        OCRelease(s_real);
        OCRelease(s_complex);
        OCRelease(s_pure_imag);
        return false;
    }

    if (SIScalarIsReal(s_complex)) {
        printf("test_SIScalarIsReal failed: Complex scalar with non-zero imaginary part incorrectly reported as real\n");
        OCRelease(s_real);
        OCRelease(s_complex);
        OCRelease(s_pure_imag);
        return false;
    }

    if (SIScalarIsReal(s_pure_imag)) {
        printf("test_SIScalarIsReal failed: Pure imaginary scalar incorrectly reported as real\n");
        OCRelease(s_real);
        OCRelease(s_complex);
        OCRelease(s_pure_imag);
        return false;
    }

    OCRelease(s_real);
    OCRelease(s_complex);
    OCRelease(s_pure_imag);
    printf("test_SIScalarIsReal passed\n");
    return true;
}

bool test_SIScalarIsImaginary(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    if (!u) {
        printf("test_SIScalarIsImaginary failed: Failed to retrieve unit 'm'\n");
        return false;
    }

    SIScalarRef s_real = SIScalarCreateWithFloat(1.0f, u);
    SIScalarRef s_complex = SIScalarCreateWithFloatComplex(0.001f + 1.0f * I, u);
    SIScalarRef s_pure_imag = SIScalarCreateWithFloatComplex(0.0f + 1.0f * I, u);
    SIScalarRef s_zero_imag = SIScalarCreateWithFloatComplex(1.0f + 0.0f * I, u);

    if (!s_real || !s_complex || !s_pure_imag || !s_zero_imag) {
        printf("test_SIScalarIsImaginary failed: Failed to create one or more scalars\n");
        if (s_real) OCRelease(s_real);
        if (s_complex) OCRelease(s_complex);
        if (s_pure_imag) OCRelease(s_pure_imag);
        if (s_zero_imag) OCRelease(s_zero_imag);
        return false;
    }

    if (SIScalarIsImaginary(s_real)) {
        printf("test_SIScalarIsImaginary failed: Real scalar incorrectly reported as imaginary\n");
        goto fail;
    }

    if (SIScalarIsImaginary(s_complex)) {
        printf("test_SIScalarIsImaginary failed: Complex scalar with non-zero real part incorrectly reported as imaginary\n");
        goto fail;
    }

    if (!SIScalarIsImaginary(s_pure_imag)) {
        printf("test_SIScalarIsImaginary failed: Pure imaginary scalar not reported as imaginary\n");
        goto fail;
    }

    if (SIScalarIsImaginary(s_zero_imag)) {
        printf("test_SIScalarIsImaginary failed: Scalar with zero imaginary part incorrectly reported as imaginary\n");
        goto fail;
    }

    OCRelease(s_real);
    OCRelease(s_complex);
    OCRelease(s_pure_imag);
    OCRelease(s_zero_imag);
    printf("test_SIScalarIsImaginary passed\n");
    return true;

fail:
    OCRelease(s_real);
    OCRelease(s_complex);
    OCRelease(s_pure_imag);
    OCRelease(s_zero_imag);
    return false;
}

bool test_SIScalarIsComplex(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    if (!u) {
        printf("test_SIScalarIsComplex failed: Failed to retrieve unit 'm'\n");
        return false;
    }

    SIScalarRef s_real = SIScalarCreateWithFloat(1.0f, u);
    SIScalarRef s_complex = SIScalarCreateWithFloatComplex(1.0f + 0.001f * I, u);
    SIScalarRef s_zero_imag = SIScalarCreateWithFloatComplex(1.0f + 0.0f * I, u);

    if (!s_real || !s_complex || !s_zero_imag) {
        printf("test_SIScalarIsComplex failed: Failed to create one or more scalars\n");
        if (s_real) OCRelease(s_real);
        if (s_complex) OCRelease(s_complex);
        if (s_zero_imag) OCRelease(s_zero_imag);
        return false;
    }

    if (SIScalarIsComplex(s_real)) {
        printf("test_SIScalarIsComplex failed: Real scalar incorrectly reported as complex\n");
        goto fail;
    }

    if (!SIScalarIsComplex(s_complex)) {
        printf("test_SIScalarIsComplex failed: Complex scalar not reported as complex\n");
        goto fail;
    }

    if (SIScalarIsComplex(s_zero_imag)) {
        printf("test_SIScalarIsComplex failed: Scalar with zero imaginary part incorrectly reported as complex\n");
        goto fail;
    }

    OCRelease(s_real);
    OCRelease(s_complex);
    OCRelease(s_zero_imag);
    printf("test_SIScalarIsComplex passed\n");
    return true;

fail:
    OCRelease(s_real);
    OCRelease(s_complex);
    OCRelease(s_zero_imag);
    return false;
}


bool test_SIScalarIsZero(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    if (!u) {
        printf("test_SIScalarIsZero failed: Failed to retrieve unit 'm'\n");
        return false;
    }

    SIScalarRef s_zero_f = SIScalarCreateWithFloat(0.0f, u);
    SIScalarRef s_zero_d = SIScalarCreateWithDouble(0.0, u);
    SIScalarRef s_zero_fc = SIScalarCreateWithFloatComplex(0.0f + 0.0f * I, u);
    SIScalarRef s_zero_dc = SIScalarCreateWithDoubleComplex(0.0 + 0.0 * I, u);
    SIScalarRef s_non_zero = SIScalarCreateWithFloat(0.001f, u);

    if (!s_zero_f || !s_zero_d || !s_zero_fc || !s_zero_dc || !s_non_zero) {
        printf("test_SIScalarIsZero failed: Failed to create one or more scalars\n");
        OCRelease(s_zero_f); OCRelease(s_zero_d);
        OCRelease(s_zero_fc); OCRelease(s_zero_dc);
        OCRelease(s_non_zero);
        return false;
    }

    if (!SIScalarIsZero(s_zero_f)) {
        printf("test_SIScalarIsZero failed: Float zero scalar not recognized as zero\n");
        goto fail;
    }

    if (!SIScalarIsZero(s_zero_d)) {
        printf("test_SIScalarIsZero failed: Double zero scalar not recognized as zero\n");
        goto fail;
    }

    if (!SIScalarIsZero(s_zero_fc)) {
        printf("test_SIScalarIsZero failed: Float complex zero scalar not recognized as zero\n");
        goto fail;
    }

    if (!SIScalarIsZero(s_zero_dc)) {
        printf("test_SIScalarIsZero failed: Double complex zero scalar not recognized as zero\n");
        goto fail;
    }

    if (SIScalarIsZero(s_non_zero)) {
        printf("test_SIScalarIsZero failed: Non-zero scalar incorrectly reported as zero\n");
        goto fail;
    }

    OCRelease(s_zero_f); OCRelease(s_zero_d);
    OCRelease(s_zero_fc); OCRelease(s_zero_dc);
    OCRelease(s_non_zero);
    printf("test_SIScalarIsZero passed\n");
    return true;

fail:
    OCRelease(s_zero_f); OCRelease(s_zero_d);
    OCRelease(s_zero_fc); OCRelease(s_zero_dc);
    OCRelease(s_non_zero);
    return false;
}

bool test_SIScalarIsInfinite(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m"));
    if (!u) {
        printf("test_SIScalarIsInfinite failed: Failed to retrieve unit 'm'\n");
        return false;
    }

    SIScalarRef s_inf_f = SIScalarCreateWithFloat(INFINITY, u);
    SIScalarRef s_inf_d = SIScalarCreateWithDouble(INFINITY, u);
    SIScalarRef s_inf_fc_real = SIScalarCreateWithFloatComplex(INFINITY + 0.0f * I, u);
    SIScalarRef s_inf_fc_imag = SIScalarCreateWithFloatComplex(0.0f + INFINITY * I, u);
    SIScalarRef s_finite = SIScalarCreateWithFloat(1.0f, u);

    if (!s_inf_f || !s_inf_d || !s_inf_fc_real || !s_inf_fc_imag || !s_finite) {
        printf("test_SIScalarIsInfinite failed: Failed to create one or more scalars\n");
        OCRelease(s_inf_f); OCRelease(s_inf_d);
        OCRelease(s_inf_fc_real); OCRelease(s_inf_fc_imag);
        OCRelease(s_finite);
        return false;
    }

    if (!SIScalarIsInfinite(s_inf_f)) {
        printf("test_SIScalarIsInfinite failed: Float infinity not recognized\n");
        goto fail;
    }

    if (!SIScalarIsInfinite(s_inf_d)) {
        printf("test_SIScalarIsInfinite failed: Double infinity not recognized\n");
        goto fail;
    }

    if (!SIScalarIsInfinite(s_inf_fc_real)) {
        printf("test_SIScalarIsInfinite failed: Float complex (real infinite) not recognized\n");
        goto fail;
    }

    if (!SIScalarIsInfinite(s_inf_fc_imag)) {
        printf("test_SIScalarIsInfinite failed: Float complex (imag infinite) not recognized\n");
        goto fail;
    }

    if (SIScalarIsInfinite(s_finite)) {
        printf("test_SIScalarIsInfinite failed: Finite value incorrectly reported as infinite\n");
        goto fail;
    }

    OCRelease(s_inf_f); OCRelease(s_inf_d);
    OCRelease(s_inf_fc_real); OCRelease(s_inf_fc_imag);
    OCRelease(s_finite);
    printf("test_SIScalarIsInfinite passed\n");
    return true;

fail:
    OCRelease(s_inf_f); OCRelease(s_inf_d);
    OCRelease(s_inf_fc_real); OCRelease(s_inf_fc_imag);
    OCRelease(s_finite);
    return false;
}

bool test_SIScalarIsRealNonNegativeInteger(void) {
    SIUnitRef u = SIUnitForUnderivedSymbol(STR("m")); // Unit doesn't affect value check
    if (!u) {
        printf("test_SIScalarIsRealNonNegativeInteger failed: Failed to retrieve unit 'm'\n");
        return false;
    }

    SIScalarRef s_int_pos = SIScalarCreateWithFloat(5.0f, u);
    SIScalarRef s_int_zero = SIScalarCreateWithFloat(0.0f, u);
    SIScalarRef s_float = SIScalarCreateWithFloat(5.1f, u);
    SIScalarRef s_neg_int = SIScalarCreateWithFloat(-5.0f, u);
    SIScalarRef s_complex = SIScalarCreateWithFloatComplex(5.0f + 1.0f * I, u);

    if (!s_int_pos || !s_int_zero || !s_float || !s_neg_int || !s_complex) {
        printf("test_SIScalarIsRealNonNegativeInteger failed: Failed to create one or more scalars\n");
        OCRelease(s_int_pos); OCRelease(s_int_zero);
        OCRelease(s_float); OCRelease(s_neg_int); OCRelease(s_complex);
        return false;
    }

    if (!SIScalarIsRealNonNegativeInteger(s_int_pos)) {
        printf("test_SIScalarIsRealNonNegativeInteger failed: 5.0 should be valid\n");
        goto fail;
    }

    if (!SIScalarIsRealNonNegativeInteger(s_int_zero)) {
        printf("test_SIScalarIsRealNonNegativeInteger failed: 0.0 should be valid\n");
        goto fail;
    }

    if (SIScalarIsRealNonNegativeInteger(s_float)) {
        printf("test_SIScalarIsRealNonNegativeInteger failed: 5.1 should not be valid\n");
        goto fail;
    }

    if (SIScalarIsRealNonNegativeInteger(s_neg_int)) {
        printf("test_SIScalarIsRealNonNegativeInteger failed: -5.0 should not be valid\n");
        goto fail;
    }

    if (SIScalarIsRealNonNegativeInteger(s_complex)) {
        printf("test_SIScalarIsRealNonNegativeInteger failed: Complex value should not be valid\n");
        goto fail;
    }

    OCRelease(s_int_pos); OCRelease(s_int_zero); OCRelease(s_float);
    OCRelease(s_neg_int); OCRelease(s_complex);
    printf("test_SIScalarIsRealNonNegativeInteger passed\n");
    return true;

fail:
    OCRelease(s_int_pos); OCRelease(s_int_zero); OCRelease(s_float);
    OCRelease(s_neg_int); OCRelease(s_complex);
    return false;
}


bool test_SIScalarValidateProposedStringValue(void) {
    OCStringRef valid_str = STR("10 cm");         // Same dimensionality as "m"
    OCStringRef invalid_dim_str = STR("10 kg");   // Different dimensionality
    OCStringRef invalid_fmt_str = STR("ten meters"); // Malformed input

    SIScalarRef s_meter = SIScalarCreateWithFloat(1.0f, SIUnitForUnderivedSymbol(STR("m")));
    if (!s_meter) {
        printf("test_SIScalarValidateProposedStringValue failed: Failed to create reference scalar\n");
        return false;
    }

    OCStringRef error = NULL;

    // Valid input
    if (!SIScalarValidateProposedStringValue(s_meter, valid_str, &error)) {
        printf("test_SIScalarValidateProposedStringValue failed: Valid input '10 cm' was rejected\n");
        if (error) {
            printf("Error: %s\n", OCStringGetCString(error));
            OCRelease(error);
        }
        OCRelease(s_meter);
        return false;
    }

    if (error != NULL) {
        printf("test_SIScalarValidateProposedStringValue failed: Unexpected error for valid input\n");
        OCRelease(error);
        OCRelease(s_meter);
        return false;
    }

    // Invalid dimensionality
    if (SIScalarValidateProposedStringValue(s_meter, invalid_dim_str, &error)) {
        printf("test_SIScalarValidateProposedStringValue failed: Input '10 kg' should have failed (wrong dimensionality)\n");
        OCRelease(s_meter);
        return false;
    }

    if (error == NULL) {
        printf("test_SIScalarValidateProposedStringValue failed: Missing error for dimensionality mismatch\n");
        OCRelease(s_meter);
        return false;
    }
    OCRelease(error);
    error = NULL;

    // Invalid format
    if (SIScalarValidateProposedStringValue(s_meter, invalid_fmt_str, &error)) {
        printf("test_SIScalarValidateProposedStringValue failed: Input 'ten meters' should have failed (invalid format)\n");
        OCRelease(s_meter);
        return false;
    }

    if (error == NULL) {
        printf("test_SIScalarValidateProposedStringValue failed: Missing error for invalid format\n");
        OCRelease(s_meter);
        return false;
    }

    OCRelease(error);
    OCRelease(s_meter);
    printf("test_SIScalarValidateProposedStringValue passed\n");
    return true;
}

bool test_SIScalarEqual(void) {
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
    SIUnitRef cm = SIUnitForUnderivedSymbol(STR("cm"));
    if (!m || !cm) {
        printf("test_SIScalarEqual failed: Failed to retrieve unit(s)\n");
        return false;
    }

    SIScalarRef s1 = SIScalarCreateWithFloat(1.0f, m);
    SIScalarRef s2 = SIScalarCreateWithFloat(1.0f, m);
    SIScalarRef s3 = SIScalarCreateWithFloat(1.1f, m);
    SIScalarRef s4 = SIScalarCreateWithFloat(1.0f, cm); // Different unit

    if (!s1 || !s2 || !s3 || !s4) {
        printf("test_SIScalarEqual failed: Failed to create one or more scalars\n");
        OCRelease(s1); OCRelease(s2); OCRelease(s3); OCRelease(s4);
        return false;
    }

    if (!SIScalarEqual(s1, s2)) {
        printf("test_SIScalarEqual failed: s1 and s2 should be equal (same value and unit)\n");
        goto fail;
    }

    if (SIScalarEqual(s1, s3)) {
        printf("test_SIScalarEqual failed: s1 and s3 should not be equal (different values)\n");
        goto fail;
    }

    if (SIScalarEqual(s1, s4)) {
        printf("test_SIScalarEqual failed: s1 and s4 should not be equal (different units)\n");
        goto fail;
    }

    OCRelease(s1); OCRelease(s2); OCRelease(s3); OCRelease(s4);
    printf("test_SIScalarEqual passed\n");
    return true;

fail:
    OCRelease(s1); OCRelease(s2); OCRelease(s3); OCRelease(s4);
    return false;
}


bool test_SIScalarCompare(void) {
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
    SIUnitRef cm = SIUnitForUnderivedSymbol(STR("cm"));
    SIUnitRef kg = SIUnitForUnderivedSymbol(STR("kg"));

    if (!m || !cm || !kg) {
        printf("test_SIScalarCompare failed: Failed to retrieve one or more base units\n");
        return false;
    }

    SIScalarRef s_1m = SIScalarCreateWithFloat(1.0f, m);
    SIScalarRef s_2m = SIScalarCreateWithFloat(2.0f, m);
    SIScalarRef s_100cm = SIScalarCreateWithFloat(100.0f, cm);
    SIScalarRef s_1kg = SIScalarCreateWithFloat(1.0f, kg);

    if (!s_1m || !s_2m || !s_100cm || !s_1kg) {
        printf("test_SIScalarCompare failed: Failed to create one or more scalars\n");
        OCRelease(s_1m); OCRelease(s_2m); OCRelease(s_100cm); OCRelease(s_1kg);
        return false;
    }

    if (SIScalarCompare(s_1m, s_2m) != kOCCompareLessThan) {
        printf("test_SIScalarCompare failed: 1m vs 2m should return kOCCompareLessThan\n");
        goto fail;
    }

    if (SIScalarCompare(s_2m, s_1m) != kOCCompareGreaterThan) {
        printf("test_SIScalarCompare failed: 2m vs 1m should return kOCCompareGreaterThan\n");
        goto fail;
    }

    if (SIScalarCompare(s_1m, s_100cm) != kOCCompareEqualTo) {
        printf("test_SIScalarCompare failed: 1m vs 100cm should return kOCCompareEqualTo\n");
        goto fail;
    }

    if (SIScalarCompare(s_1m, s_1kg) != kOCCompareUnequalDimensionalities) {
        printf("test_SIScalarCompare failed: 1m vs 1kg should return kOCCompareUnequalDimensionalities\n");
        goto fail;
    }

    OCRelease(s_1m); OCRelease(s_2m); OCRelease(s_100cm); OCRelease(s_1kg);
    printf("test_SIScalarCompare passed\n");
    return true;

fail:
    OCRelease(s_1m); OCRelease(s_2m); OCRelease(s_100cm); OCRelease(s_1kg);
    return false;
}


bool test_SIScalarCompareReduced(void) {
    // Similar to Compare, but ensures units are reduced first.
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
    double mult = 1.0;
    SIUnitRef mm_unit = SIUnitForSymbol(STR("mm"), &mult, NULL); // millimeter

    if (!m || !mm_unit) {
        printf("test_SIScalarCompareReduced failed: Failed to retrieve unit(s)\n");
        return false;
    }

    SIScalarRef s_1m = SIScalarCreateWithFloat(1.0f, m);
    SIScalarRef s_1000mm = SIScalarCreateWithFloat(1000.0f, mm_unit);

    if (!s_1m || !s_1000mm) {
        printf("test_SIScalarCompareReduced failed: Failed to create scalar(s)\n");
        OCRelease(s_1m);
        OCRelease(s_1000mm);
        return false;
    }

    if (SIScalarCompareReduced(s_1m, s_1000mm) != kOCCompareEqualTo) {
        printf("test_SIScalarCompareReduced failed: Expected 1m == 1000mm after reduction\n");
        OCRelease(s_1m);
        OCRelease(s_1000mm);
        return false;
    }

    OCRelease(s_1m);
    OCRelease(s_1000mm);
    printf("test_SIScalarCompareReduced passed\n");
    return true;
}


bool test_SIScalarCompareLoose(void) {
    SIUnitRef m = SIUnitForUnderivedSymbol(STR("m"));
    if (!m) {
        printf("test_SIScalarCompareLoose failed: Failed to retrieve unit 'm'\n");
        return false;
    }

    SIScalarRef s1 = SIScalarCreateWithDouble(1.000000001, m);
    SIScalarRef s2 = SIScalarCreateWithDouble(1.0, m);
    SIScalarRef s3 = SIScalarCreateWithDouble(1.001, m);

    if (!s1 || !s2 || !s3) {
        printf("test_SIScalarCompareLoose failed: Failed to create scalar(s)\n");
        OCRelease(s1); OCRelease(s2); OCRelease(s3);
        return false;
    }

    if (SIScalarCompare(s1, s2) != kOCCompareGreaterThan) {
        printf("test_SIScalarCompareLoose failed: Exact compare s1 > s2 expected\n");
        goto fail;
    }

    if (SIScalarCompareLoose(s1, s2) != kOCCompareEqualTo) {
        printf("test_SIScalarCompareLoose failed: Loose compare s1 ≈ s2 expected\n");
        goto fail;
    }

    if (SIScalarCompareLoose(s1, s3) != kOCCompareLessThan) {
        printf("test_SIScalarCompareLoose failed: Loose compare s1 < s3 expected\n");
        goto fail;
    }

    OCRelease(s1); OCRelease(s2); OCRelease(s3);
    printf("test_SIScalarCompareLoose passed\n");
    return true;

fail:
    OCRelease(s1); OCRelease(s2); OCRelease(s3);
    return false;
}



// -----------------------------------------------------------------------------
// Test SIScalarBestConversionForQuantity: 0.005 s → 5 ms
bool test_SIScalarBestConversionForQuantity(void) {
    SIScalarRef s = SIScalarCreateWithDouble(0.005, SIUnitForUnderivedSymbol(STR("s")));
    OCStringRef err = NULL;

    bool ok = SIScalarBestConversionForQuantity((SIMutableScalarRef)s,
                                                STR("time"),
                                               &err);
    if (!ok || err) {
        printf("test_SIScalarBestConversionForQuantity failed: unexpected error: %s\n",
               err ? OCStringGetCString(err) : "(null)");
        return false;
    }

    /* 1) Compare numeric part with a tolerance */
    double got = SIScalarDoubleValue(s);
    if (OCCompareDoubleValuesLoose(got, 5.0) != kOCCompareEqualTo) {
        printf("test_SIScalarBestConversionForQuantity failed: expected 5.0, got %f\n", got);
        return false;
    }

    /* 2) Check that the unit is “ms” */
    OCStringRef unitSym = SIUnitCopySymbol(SIQuantityGetUnit((SIQuantityRef)s));
    if (OCStringCompare(unitSym, STR("ms"), 0) != kOCCompareEqualTo) {
        printf("test_SIScalarBestConversionForQuantity failed: expected unit 'ms', got '%s'\n",
               OCStringGetCString(unitSym));
        OCRelease(unitSym);
        return false;
    }
    OCRelease(unitSym);
    OCRelease(s);

    return true;
}


// -----------------------------------------------------------------------------
// Test SIScalarBestConversionForQuantity: large value → hours
bool test_SIScalarBestConversionForQuantity_large(void) {
    // 36 000 s → 10 h
    SIScalarRef s = SIScalarCreateWithDouble(36000.0, SIUnitForUnderivedSymbol(STR("s")));
    OCStringRef err = NULL;

    bool ok = SIScalarBestConversionForQuantity((SIMutableScalarRef)s,
                                                STR("time"),
                                               &err);
    if (!ok || err) {
        printf("test_SIScalarBestConversionForQuantity_large failed: unexpected error: %s\n",
               err ? OCStringGetCString(err) : "(null)");
        return false;
    }

    double gotVal = SIScalarDoubleValue(s);
    if (OCCompareDoubleValuesLoose(gotVal, 10.0) != kOCCompareEqualTo) {
        printf("test_SIScalarBestConversionForQuantity_large failed: expected 10.0, got %f\n", gotVal);
        OCRelease(s);
        return false;
    }

    OCStringRef unitSym = SIUnitCopySymbol(SIQuantityGetUnit((SIQuantityRef)s));
    if (OCStringCompare(unitSym, STR("h"), 0) != kOCCompareEqualTo) {
        printf("test_SIScalarBestConversionForQuantity_large failed: expected unit 'h', got '%s'\n",
               OCStringGetCString(unitSym));
        OCRelease(unitSym);
        OCRelease(s);
        return false;
    }

    OCRelease(unitSym);
    OCRelease(s);
    return true;
}

// -----------------------------------------------------------------------------
// Test SIScalarBestConversionForQuantity: very small value → microseconds
bool test_SIScalarBestConversionForQuantity_tiny(void) {
    // 1e-6 s → 1 µs
    SIScalarRef s = SIScalarCreateWithDouble(0.000001, SIUnitForUnderivedSymbol(STR("s")));
    OCStringRef err = NULL;

    bool ok = SIScalarBestConversionForQuantity((SIMutableScalarRef)s,
                                                STR("time"),
                                                &err);
    if (!ok || err) {
        printf("test_SIScalarBestConversionForQuantity_tiny failed: unexpected error: %s\n",
               err ? OCStringGetCString(err) : "(null)");
        return false;
    }

    double gotVal = SIScalarDoubleValue(s);
    if (OCCompareDoubleValuesLoose(gotVal, 1.0) != kOCCompareEqualTo) {
        printf("test_SIScalarBestConversionForQuantity_tiny failed: expected 1.0, got %f\n", gotVal);
        OCRelease(s);
        return false;
    }

    OCStringRef unitSym = SIUnitCopySymbol(SIQuantityGetUnit((SIQuantityRef)s));
    // expect the micro-sign U+00B5, not Greek-mu
    if (OCStringCompare(unitSym, STR("µs"), 0) != kOCCompareEqualTo) {
        printf("test_SIScalarBestConversionForQuantity_tiny failed: expected unit 'µs', got '%s'\n",
               OCStringGetCString(unitSym));
        OCRelease(unitSym);
        OCRelease(s);
        return false;
    }

    OCRelease(unitSym);
    OCRelease(s);
    return true;
}

// -----------------------------------------------------------------------------
// Test SIScalarBestConversionForQuantity: no change when in “good” range
bool test_SIScalarBestConversionForQuantity_noop(void) {
    // 2 s stays as seconds
    SIScalarRef s = SIScalarCreateWithDouble(2.0, SIUnitForUnderivedSymbol(STR("s")));
    OCStringRef err = NULL;

    bool ok = SIScalarBestConversionForQuantity((SIMutableScalarRef)s,
                                                STR("time"),
                                               &err);
    if (!ok || err) {
        printf("test_SIScalarBestConversionForQuantity_noop failed: unexpected error: %s\n",
               err ? OCStringGetCString(err) : "(null)");
        return false;
    }

    double gotVal = SIScalarDoubleValue(s);
    if (OCCompareDoubleValuesLoose(gotVal, 2.0) != kOCCompareEqualTo) {
        printf("test_SIScalarBestConversionForQuantity_noop failed: expected 2.0, got %f\n", gotVal);
        OCRelease(s);
        return false;
    }

    OCStringRef unitSym = SIUnitCopySymbol(SIQuantityGetUnit((SIQuantityRef)s));
    if (OCStringCompare(unitSym, STR("s"), 0) != kOCCompareEqualTo) {
        printf("test_SIScalarBestConversionForQuantity_noop failed: expected unit 's', got '%s'\n",
               OCStringGetCString(unitSym));
        OCRelease(unitSym);
        OCRelease(s);
        return false;
    }

    OCRelease(unitSym);
    OCRelease(s);
    return true;
}

// -----------------------------------------------------------------------------
// Test SIScalarBestConversionForQuantity: zero → no error, no change
bool test_SIScalarBestConversionForQuantity_zero(void) {
    SIScalarRef s = SIScalarCreateWithDouble(0.0, SIUnitForUnderivedSymbol(STR("s")));
    OCStringRef err = STR("pre-existing");
    bool ok = SIScalarBestConversionForQuantity((SIMutableScalarRef)s,
                                                STR("time"),
                                               &err);
    if (!ok || err) {
        printf("test_SIScalarBestConversionForQuantity_zero failed: unexpected err state\n");
        OCRelease(s);
        return false;
    }
    // value stays 0, unit stays s
    if (SIScalarDoubleValue(s) != 0.0) {
        printf("test_SIScalarBestConversionForQuantity_zero failed: value changed\n");
        OCRelease(s);
        return false;
    }
    OCStringRef unitSym = SIUnitCopySymbol(SIQuantityGetUnit((SIQuantityRef)s));
    bool unitOk = (OCStringCompare(unitSym, STR("s"), 0) == kOCCompareEqualTo);
    OCRelease(unitSym);
    OCRelease(s);
    if (!unitOk) {
        printf("test_SIScalarBestConversionForQuantity_zero failed: unit changed\n");
        return false;
    }
    return true;
}

// -----------------------------------------------------------------------------
// Test SIScalarBestConversionForQuantity: negative duration → preserves sign
bool test_SIScalarBestConversionForQuantity_negative(void) {
    // –0.002 s → –2 ms
    SIScalarRef s = SIScalarCreateWithDouble(-0.002, SIUnitForUnderivedSymbol(STR("s")));
    OCStringRef err = NULL;
    bool ok = SIScalarBestConversionForQuantity((SIMutableScalarRef)s,
                                                STR("time"),
                                               &err);
    if (!ok || err) {
        printf("test_SIScalarBestConversionForQuantity_negative failed: unexpected error\n");
        OCRelease(s);
        return false;
    }

    double gotVal = SIScalarDoubleValue(s);
    if (OCCompareDoubleValuesLoose(gotVal, -2.0) != kOCCompareEqualTo) {
        printf("test_SIScalarBestConversionForQuantity_negative failed: expected -2.0, got %f\n", gotVal);
        OCRelease(s);
        return false;
    }

    OCStringRef unitSym = SIUnitCopySymbol(SIQuantityGetUnit((SIQuantityRef)s));
    if (OCStringCompare(unitSym, STR("ms"), 0) != kOCCompareEqualTo) {
        printf("test_SIScalarBestConversionForQuantity_negative failed: expected unit 'ms', got '%s'\n",
               OCStringGetCString(unitSym));
        OCRelease(unitSym);
        OCRelease(s);
        return false;
    }

    OCRelease(unitSym);
    OCRelease(s);
    return true;
}
