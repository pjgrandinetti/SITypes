#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SITypes.h"
#include "test_utils.h"  // Include the new test utils header
bool test_scalar_parser_1(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;
    // Part 1: 4.3 eV parsing
    SIScalarRef scalar = SIScalarCreateFromExpression(STR("4.3 eV"), &err);
    if (!scalar) {
        if (err) {
            printf("Error parsing scalar '4.3 eV': %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        printf("%s failed: Failed to create scalar with eV\n", __func__);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }
    SIUnitRef evUnit = SIUnitWithSymbol(STR("eV"));
    SIScalarRef expected1 = SIScalarCreateWithFloatComplex(4.3, evUnit);
    if (!expected1) {
        printf("%s failed: Failed to create expected scalar\n", __func__);
        OCRelease(scalar);
        return false;
    }
    if (SIScalarCompare(scalar, expected1) != kOCCompareEqualTo) {
        printf("%s failed: Scalar value mismatch for 4.3 eV\n", __func__);
        OCRelease(expected1);
        OCRelease(scalar);
        return false;
    }
    OCRelease(expected1);
    OCRelease(scalar);
    // Part 2: mass = force / acceleration
    SIScalarRef acceleration = SIScalarCreateFromExpression(STR("9.8 m/s^2"), &err);
    if (!acceleration) {
        if (err) {
            printf("Error parsing acceleration: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        printf("%s failed: Failed to parse acceleration\n", __func__);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }
    SIScalarRef force = SIScalarCreateFromExpression(STR("500 N"), &err);
    if (!force) {
        if (err) {
            printf("Error parsing force: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        OCRelease(acceleration);
        printf("%s failed: Failed to parse force\n", __func__);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }
    SIScalarRef mass = SIScalarCreateByDividing(force, acceleration, &err);
    if (!mass) {
        if (err) {
            printf("Error computing mass: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        OCRelease(force);
        OCRelease(acceleration);
        printf("%s failed: Failed to compute mass\n", __func__);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }
    SIScalarRef expected2 = SIScalarCreateFromExpression(STR("51.0204081632653 kg"), &err);
    if (!expected2) {
        if (err) {
            printf("Error parsing expected mass: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        OCRelease(mass);
        OCRelease(force);
        OCRelease(acceleration);
        printf("%s failed: Failed to parse expected mass\n", __func__);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }
    if (SIScalarCompareLoose(mass, expected2) != kOCCompareEqualTo) {
        printf("%s failed: Mass value mismatch\n", __func__);
        OCRelease(expected2);
        OCRelease(mass);
        OCRelease(force);
        OCRelease(acceleration);
        return false;
    }
    // Clean up
    OCRelease(expected2);
    OCRelease(mass);
    OCRelease(force);
    OCRelease(acceleration);
    printf("%s passed\n", __func__);
    return true;
}
bool test_scalar_parser_2(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;
    // force
    SIScalarRef force = SIScalarCreateFromExpression(STR("500 N"), &err);
    if (!force) {
        if (err) {
            printf("Error parsing force: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        printf("%s failed: Failed to create force scalar\n", __func__);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }
    // accel
    SIScalarRef accel = SIScalarCreateFromExpression(STR("9.8 m/s^2"), &err);
    if (!accel) {
        if (err) {
            printf("Error parsing acceleration: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        OCRelease(force);
        printf("%s failed: Failed to create acceleration scalar\n", __func__);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }
    // mass = force / accel
    SIScalarRef mass = SIScalarCreateByDividing(force, accel, &err);
    if (!mass) {
        if (err) {
            printf("Error computing mass: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        OCRelease(force);
        OCRelease(accel);
        printf("%s failed: Failed to compute mass by division\n", __func__);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }
    // combined expression
    SIScalarRef mass2 = SIScalarCreateFromExpression(STR("500 N/(9.8 m/s^2)"), &err);
    if (!mass2) {
        if (err) {
            printf("Error parsing combined mass: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        OCRelease(force);
        OCRelease(accel);
        OCRelease(mass);
        printf("%s failed: Failed to parse combined mass expression\n", __func__);
        return false;
    }
    SIScalarReduceUnit((SIMutableScalarRef)mass2);
    if (err) {
        OCRelease(err);
        OCRelease(force);
        OCRelease(accel);
        OCRelease(mass);
        OCRelease(mass2);
        printf("%s failed: Error during unit reduction\n", __func__);
        return false;
    }
    if (SIScalarCompare(mass, mass2) != kOCCompareEqualTo) {
        printf("%s failed: Computed mass does not match parsed expression\n", __func__);
        OCRelease(force);
        OCRelease(accel);
        OCRelease(mass);
        OCRelease(mass2);
        return false;
    }
    // cleanup
    OCRelease(force);
    OCRelease(accel);
    OCRelease(mass);
    OCRelease(mass2);
    printf("%s passed\n", __func__);
    return true;
}
bool test_scalar_parser_3(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;
    // Parse "2^3"
    SIScalarRef res1 = SIScalarCreateFromExpression(STR("2^3"), &err);
    if (!res1) {
        if (err) {
            printf("Error parsing exponent: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        printf("%s failed: Failed to parse exponent expression\n", __func__);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }
    // Parse "8"
    SIScalarRef res2 = SIScalarCreateFromExpression(STR("8"), &err);
    if (!res2) {
        if (err) {
            printf("Error parsing literal: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        OCRelease(res1);
        printf("%s failed: Failed to parse literal expression\n", __func__);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }
    // Compare
    if (SIScalarCompare(res1, res2) != kOCCompareEqualTo) {
        printf("%s failed: Exponent expression does not match expected literal value\n", __func__);
        OCRelease(res1);
        OCRelease(res2);
        return false;
    }
    // Cleanup
    OCRelease(res1);
    OCRelease(res2);
    printf("%s passed\n", __func__);
    return true;
}
bool test_scalar_parser_4(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;
    // Parse expression with implicit multiplication
    SIScalarRef s = SIScalarCreateFromExpression(STR("(2+3)(4+1)"), &err);
    if (!s) {
        if (err) {
            printf("Error parsing implicit multiplication: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        printf("%s failed: Implicit multiplication failed\n", __func__);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }
    // Parse expected value
    SIScalarRef e = SIScalarCreateFromExpression(STR("25"), &err);
    if (!e) {
        if (err) {
            printf("Error parsing expected result: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        OCRelease(s);
        printf("%s failed: Failed to parse expected result\n", __func__);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }
    // Compare results
    if (SIScalarCompare(s, e) != kOCCompareEqualTo) {
        printf("%s failed: Result of expression does not match expected value\n", __func__);
        OCRelease(s);
        OCRelease(e);
        return false;
    }
    // Cleanup and success
    OCRelease(s);
    OCRelease(e);
    printf("%s passed\n", __func__);
    return true;
}
bool test_scalar_parser_5(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;
    // √(9) m
    SIScalarRef r1 = SIScalarCreateFromExpression(STR("√(9) m"), &err);
    if (!r1) {
        if (err) {
            printf("Error parsing root: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        printf("%s failed: Root parsing failed\n", __func__);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }
    // 3 m
    SIScalarRef r2 = SIScalarCreateFromExpression(STR("3 m"), &err);
    if (!r2) {
        if (err) {
            printf("Error parsing comparison scalar for root: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        OCRelease(r1);
        printf("%s failed: Failed to parse comparison scalar for root\n", __func__);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }
    if (SIScalarCompare(r1, r2) != kOCCompareEqualTo) {
        printf("%s failed: Root result does not match expected value\n", __func__);
        OCRelease(r1);
        OCRelease(r2);
        return false;
    }
    OCRelease(r1);
    OCRelease(r2);
    // ∛(8) kg
    SIScalarRef r3 = SIScalarCreateFromExpression(STR("∛(8) kg"), &err);
    if (!r3) {
        if (err) {
            printf("Error parsing cubert: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        printf("%s failed: Cubert parsing failed\n", __func__);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }
    // 2 kg
    SIScalarRef r4 = SIScalarCreateFromExpression(STR("2 kg"), &err);
    if (!r4) {
        if (err) {
            printf("Error parsing comparison scalar for cubert: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        OCRelease(r3);
        printf("%s failed: Failed to parse comparison scalar for cubert\n", __func__);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }
    if (SIScalarCompare(r3, r4) != kOCCompareEqualTo) {
        printf("%s failed: Cubert result does not match expected value\n", __func__);
        OCRelease(r3);
        OCRelease(r4);
        return false;
    }
    OCRelease(r3);
    OCRelease(r4);
    printf("%s passed\n", __func__);
    return true;
}
bool test_scalar_parser_6(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;
    // Multiplication test
    SIScalarRef d1 = SIScalarCreateFromExpression(STR("6×2 kg"), &err);
    if (!d1) {
        if (err) {
            printf("Error parsing multiplication: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        printf("%s failed: Multiplication operator failed\n", __func__);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }
    SIScalarRef d2 = SIScalarCreateFromExpression(STR("12 kg"), &err);
    if (!d2) {
        if (err) {
            printf("Error parsing comparison scalar for multiplication: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        OCRelease(d1);
        printf("%s failed: Failed to parse comparison scalar for multiplication\n", __func__);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }
    // Division test
    SIScalarRef d3 = SIScalarCreateFromExpression(STR("(12 ÷ 4) m"), &err);
    if (!d3) {
        if (err) {
            printf("Error parsing division: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        OCRelease(d1);
        OCRelease(d2);
        printf("%s failed: Division operator failed\n", __func__);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }
    SIScalarRef d4 = SIScalarCreateFromExpression(STR("3 m"), &err);
    if (!d4) {
        if (err) {
            printf("Error parsing comparison scalar for division: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        OCRelease(d1);
        OCRelease(d2);
        OCRelease(d3);
        printf("%s failed: Failed to parse comparison scalar for division\n", __func__);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }
    // Minus sign test
    SIScalarRef d5 = SIScalarCreateFromExpression(STR("−5 m"), &err);
    if (!d5) {
        if (err) {
            printf("Error parsing minus sign: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        OCRelease(d1);
        OCRelease(d2);
        OCRelease(d3);
        OCRelease(d4);
        printf("%s failed: Minus sign parsing failed\n", __func__);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }
    SIScalarRef d6 = SIScalarCreateFromExpression(STR("-5 m"), &err);
    if (!d6) {
        if (err) {
            printf("Error parsing comparison scalar for minus: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        OCRelease(d1);
        OCRelease(d2);
        OCRelease(d3);
        OCRelease(d4);
        OCRelease(d5);
        printf("%s failed: Failed to parse comparison scalar for minus\n", __func__);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }
    // Comparison check
    if (SIScalarCompare(d1, d2) != kOCCompareEqualTo ||
        SIScalarCompare(d3, d4) != kOCCompareEqualTo ||
        SIScalarCompare(d5, d6) != kOCCompareEqualTo) {
        printf("%s failed: One or more scalar comparisons did not match\n", __func__);
        OCRelease(d1);
        OCRelease(d2);
        OCRelease(d3);
        OCRelease(d4);
        OCRelease(d5);
        OCRelease(d6);
        return false;
    }
    // Clean up
    OCRelease(d1);
    OCRelease(d2);
    OCRelease(d3);
    OCRelease(d4);
    OCRelease(d5);
    OCRelease(d6);
    printf("%s passed\n", __func__);
    return true;
}
bool test_scalar_parser_7(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;
    // Parse "5 µm"
    SIScalarRef m1 = SIScalarCreateFromExpression(STR("5 µm"), &err);
    if (!m1) {
        if (err) {
            printf("Error parsing micro prefix: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        printf("%s failed: Micro prefix parsing failed\n", __func__);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }
    // Expected scalar
    SIUnitRef micron = SIUnitWithSymbol(STR("µm"));
    SIScalarRef m2 = SIScalarCreateWithFloatComplex(5.0, micron);
    if (!m2) {
        OCRelease(m1);
        printf("%s failed: Failed to create expected scalar with µm\n", __func__);
        return false;
    }
    // Comparison
    if (SIScalarCompare(m1, m2) != kOCCompareEqualTo) {
        printf("%s failed: Micro-prefixed scalar does not match expected value\n", __func__);
        OCRelease(m1);
        OCRelease(m2);
        return false;
    }
    OCRelease(m1);
    OCRelease(m2);
    printf("%s passed\n", __func__);
    return true;
}
bool test_scalar_parser_8(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;
    // Parse "1 quartertsp"
    SIScalarRef q = SIScalarCreateFromExpression(STR("1 quartertsp"), &err);
    if (!q) {
        if (err) {
            printf("Error parsing unit 'quartertsp': %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        printf("%s failed: quartertsp parsing failed\n", __func__);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }
    // Create expected scalar
    SIUnitRef qUnit = SIUnitWithSymbol(STR("quartertsp"));
    SIScalarRef qExp = SIScalarCreateWithFloatComplex(1.0, qUnit);
    if (!qExp) {
        OCRelease(q);
        printf("%s failed: Failed to create expected scalar for quartertsp\n", __func__);
        return false;
    }
    // Compare
    if (SIScalarCompare(q, qExp) != kOCCompareEqualTo) {
        printf("%s failed: Scalar value does not match expected for quartertsp\n", __func__);
        OCRelease(q);
        OCRelease(qExp);
        return false;
    }
    // Clean up
    OCRelease(q);
    OCRelease(qExp);
    printf("%s passed\n", __func__);
    return true;
}
bool test_scalar_parser_9(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;
    // Try to parse invalid expression
    SIScalarRef bad = SIScalarCreateFromExpression(STR("2+"), &err);
    if (bad != NULL || err == NULL) {
        printf("%s failed: Error detection failed for malformed expression '2+'\n", __func__);
        if (err) OCRelease(err);
        if (bad) OCRelease(bad);
        return false;
    }
    // Clean up
    OCRelease(err);
    printf("%s passed\n", __func__);
    return true;
}
bool test_scalar_parser_10(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;
    // n
    SIScalarRef n = SIScalarCreateFromExpression(STR("0.078 mol"), &err);
    if (!n) {
        if (err) {
            printf("Error parsing n: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        printf("%s failed: Failed to parse n\n", __func__);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }
    // R
    SIScalarRef R = SIScalarCreateFromExpression(STR("R"), &err);
    if (!R) {
        if (err) {
            printf("Error parsing R: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        OCRelease(n);
        printf("%s failed: Failed to parse R\n", __func__);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }
    // T
    SIScalarRef T = SIScalarCreateFromExpression(STR("298.15 K"), &err);
    if (!T) {
        if (err) {
            printf("Error parsing T: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        OCRelease(n);
        OCRelease(R);
        printf("%s failed: Failed to parse T\n", __func__);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }
    // V
    SIScalarRef V = SIScalarCreateFromExpression(STR("42.0 mL"), &err);
    if (!V) {
        if (err) {
            printf("Error parsing V: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        OCRelease(n);
        OCRelease(R);
        OCRelease(T);
        printf("%s failed: Failed to parse V\n", __func__);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }
    // n * R
    SIScalarRef nR = SIScalarCreateByMultiplying(n, R, &err);
    if (!nR) {
        if (err) {
            printf("Error computing n*R: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        OCRelease(n);
        OCRelease(R);
        OCRelease(T);
        OCRelease(V);
        printf("%s failed: Failed to multiply n and R\n", __func__);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }
    // n * R * T
    SIScalarRef nRT = SIScalarCreateByMultiplying(nR, T, &err);
    if (!nRT) {
        if (err) {
            printf("Error computing n*R*T: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        OCRelease(n);
        OCRelease(R);
        OCRelease(T);
        OCRelease(V);
        OCRelease(nR);
        printf("%s failed: Failed to multiply nR and T\n", __func__);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }
    // (nRT) / V
    SIScalarRef p = SIScalarCreateByDividing(nRT, V, &err);
    if (!p) {
        if (err) {
            printf("Error computing n*R*T/V: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        OCRelease(n);
        OCRelease(R);
        OCRelease(T);
        OCRelease(V);
        OCRelease(nR);
        OCRelease(nRT);
        printf("%s failed: Failed to divide nRT by V\n", __func__);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }
    // Expected result
    SIScalarRef expected = SIScalarCreateFromExpression(STR("4603777.340690149 Pa"), &err);
    if (!expected) {
        if (err) {
            printf("Error parsing expected pressure: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        OCRelease(n);
        OCRelease(R);
        OCRelease(T);
        OCRelease(V);
        OCRelease(nR);
        OCRelease(nRT);
        OCRelease(p);
        printf("%s failed: Failed to parse expected p\n", __func__);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }
    // Loose comparison
    OCComparisonResult cmp = SIScalarCompareLoose(p, expected);
    if (cmp != kOCCompareEqualTo) {
        OCStringRef actualStr = SIScalarCreateStringValue(p);
        OCStringRef expectedStr = SIScalarCreateStringValue(expected);
        printf("Mismatch in ideal gas pressure:\n  Computed: %s\n  Expected: %s\n",
               OCStringGetCString(actualStr),
               OCStringGetCString(expectedStr));
        OCRelease(actualStr);
        OCRelease(expectedStr);
        OCRelease(n);
        OCRelease(R);
        OCRelease(T);
        OCRelease(V);
        OCRelease(nR);
        OCRelease(nRT);
        OCRelease(p);
        OCRelease(expected);
        printf("%s failed: Ideal gas pressure mismatch\n", __func__);
        return false;
    }
    // Cleanup
    OCRelease(n);
    OCRelease(R);
    OCRelease(T);
    OCRelease(V);
    OCRelease(nR);
    OCRelease(nRT);
    OCRelease(p);
    OCRelease(expected);
    printf("%s passed\n", __func__);
    return true;
}
bool test_scalar_parser_11(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;
    // Test parser recovery after bad expression
    // This test checks if the parser properly recovers after parsing an invalid expression
    // Step 1: Parse a valid expression first
    SIScalarRef scalar1 = SIScalarCreateFromExpression(STR("42.0 Hz"), &err);
    if (!scalar1) {
        if (err) {
            printf("Error parsing valid expression '42.0 Hz': %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        printf("%s failed: Failed to parse initial valid expression\n", __func__);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }
    OCRelease(scalar1);
    // Step 2: Try to parse an invalid expression (should fail)
    SIScalarRef scalar_bad = SIScalarCreateFromExpression(STR("invalid syntax here"), &err);
    if (scalar_bad) {
        printf("%s failed: Invalid expression unexpectedly succeeded\n", __func__);
        OCRelease(scalar_bad);
        return false;
    }
    // Clean up error from invalid parse
    if (err) {
        OCRelease(err);
        err = NULL;
    }
    // Step 3: Parse another valid expression - this should succeed if parser recovered
    SIScalarRef scalar2 = SIScalarCreateFromExpression(STR("100.0 Hz"), &err);
    if (!scalar2) {
        if (err) {
            printf("Error parsing valid expression after invalid parse '100.0 Hz': %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        printf("%s failed: Parser did not recover after invalid expression\n", __func__);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }
    // Verify the value is correct
    SIUnitRef hzUnit = SIUnitWithSymbol(STR("Hz"));
    SIScalarRef expected = SIScalarCreateWithFloatComplex(100.0, hzUnit);
    if (!expected) {
        printf("%s failed: Failed to create expected scalar\n", __func__);
        OCRelease(scalar2);
        return false;
    }
    if (SIScalarCompare(scalar2, expected) != kOCCompareEqualTo) {
        printf("%s failed: Scalar value mismatch after parser recovery\n", __func__);
        OCRelease(expected);
        OCRelease(scalar2);
        return false;
    }
    // Step 4: Try one more complex sequence to ensure robustness
    // Invalid expression followed by a more complex valid expression
    SIScalarRef scalar_bad2 = SIScalarCreateFromExpression(STR("completely@#$%invalid&*()"), &err);
    if (scalar_bad2) {
        printf("%s failed: Second invalid expression unexpectedly succeeded\n", __func__);
        OCRelease(scalar_bad2);
        OCRelease(expected);
        OCRelease(scalar2);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }
    // Complex valid expression after error
    SIScalarRef scalar3 = SIScalarCreateFromExpression(STR("3.14159 * 2.0 m"), &err);
    if (!scalar3) {
        if (err) {
            printf("Error parsing complex expression after invalid parse: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        printf("%s failed: Parser did not recover for complex expression\n", __func__);
        OCRelease(expected);
        OCRelease(scalar2);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }
    // Cleanup
    OCRelease(expected);
    OCRelease(scalar2);
    OCRelease(scalar3);
    printf("%s passed\n", __func__);
    return true;
}
bool test_scalar_parser_12(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;
    // Test formula weight function fw[CH4]
    SIScalarRef methane_fw = SIScalarCreateFromExpression(STR("fw[CH4]"), &err);
    if (!methane_fw) {
        if (err) {
            printf("Error parsing 'fw[CH4]': %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        printf("%s failed: fw[CH4] function not working\n", __func__);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }
    // Check that we got a reasonable value for methane formula weight (~16.043 g/mol)
    double value = SIScalarDoubleValue(methane_fw);
    if (value < 16.0 || value > 16.1) {
        printf("%s failed: fw[CH4] gave unexpected value %.6f (expected ~16.043)\n", __func__, value);
        OCRelease(methane_fw);
        return false;
    }
    // Check that the units are correct (should be g/mol)
    SIUnitRef unit = SIQuantityGetUnit((SIQuantityRef)methane_fw);
    OCStringRef unit_symbol = SIUnitCopySymbol(unit);
    const char* symbol_str = OCStringGetCString(unit_symbol);
    if (strcmp(symbol_str, "g/mol") != 0) {
        printf("%s failed: fw[CH4] units are '%s' (expected 'g/mol')\n", __func__, symbol_str);
        OCRelease(unit_symbol);
        OCRelease(methane_fw);
        return false;
    }
    OCRelease(unit_symbol);
    // Test formula weight function fw[H2O]
    SIScalarRef water_fw = SIScalarCreateFromExpression(STR("fw[H2O]"), &err);
    if (!water_fw) {
        if (err) {
            printf("Error parsing 'fw[H2O]': %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        printf("%s failed: fw[H2O] function not working\n", __func__);
        OCRelease(methane_fw);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }
    // Check that we got a reasonable value for water formula weight (~18.015 g/mol)
    double water_value = SIScalarDoubleValue(water_fw);
    if (water_value < 18.0 || water_value > 18.1) {
        printf("%s failed: fw[H2O] gave unexpected value %.6f (expected ~18.015)\n", __func__, water_value);
        OCRelease(water_fw);
        OCRelease(methane_fw);
        return false;
    }
    // Test formula weight function fw[CO2]
    SIScalarRef co2_fw = SIScalarCreateFromExpression(STR("fw[CO2]"), &err);
    if (!co2_fw) {
        if (err) {
            printf("Error parsing 'fw[CO2]': %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        printf("%s failed: fw[CO2] function not working\n", __func__);
        OCRelease(water_fw);
        OCRelease(methane_fw);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }
    // Check that we got a reasonable value for CO2 formula weight (~44.01 g/mol)
    double co2_value = SIScalarDoubleValue(co2_fw);
    if (co2_value < 44.0 || co2_value > 44.1) {
        printf("%s failed: fw[CO2] gave unexpected value %.6f (expected ~44.01)\n", __func__, co2_value);
        OCRelease(co2_fw);
        OCRelease(water_fw);
        OCRelease(methane_fw);
        return false;
    }
    // Test calculation using formula weight
    SIScalarRef moles_calc = SIScalarCreateFromExpression(STR("18 g / fw[H2O]"), &err);
    if (!moles_calc) {
        if (err) {
            printf("Error parsing '18 g / fw[H2O]': %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        printf("%s failed: calculation using fw[H2O] not working\n", __func__);
        OCRelease(co2_fw);
        OCRelease(water_fw);
        OCRelease(methane_fw);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }
    // Check that we got approximately 1 mol
    double moles_value = SIScalarDoubleValue(moles_calc);
    if (moles_value < 0.99 || moles_value > 1.01) {
        printf("%s failed: 18 g / fw[H2O] gave unexpected value %.6f (expected ~1.0)\n", __func__, moles_value);
        OCRelease(moles_calc);
        OCRelease(co2_fw);
        OCRelease(water_fw);
        OCRelease(methane_fw);
        return false;
    }
    // Cleanup
    OCRelease(moles_calc);
    OCRelease(co2_fw);
    OCRelease(water_fw);
    OCRelease(methane_fw);
    printf("%s passed\n", __func__);
    return true;
}
bool test_nmr_functions(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;
    // Test 1: Check that spin[H1] works (baseline test)
    printf("  Testing spin[H1]...\n");
    SIScalarRef h1_spin = SIScalarCreateFromExpression(STR("spin[H1]"), &err);
    if (!h1_spin) {
        printf("Error parsing 'spin[H1]': %s\n", err ? OCStringGetCString(err) : "Unknown error");
        if (err) OCRelease(err);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }
    double spin_value = SIScalarDoubleValue(h1_spin);
    printf("    spin[H1] = %f\n", spin_value);
    if (fabs(spin_value - 0.5) > 1e-10) {
        printf("%s failed: Expected spin[H1] = 0.5, got %f\n", __func__, spin_value);
        OCRelease(h1_spin);
        return false;
    }
    OCRelease(h1_spin);
    // Test 2: Check magnetic dipole moment μ_I[H1]
    printf("  Testing μ_I[H1]...\n");
    SIScalarRef h1_moment = SIScalarCreateFromExpression(STR("μ_I[H1]"), &err);
    if (!h1_moment) {
        printf("Error parsing 'μ_I[H1]': %s\n", err ? OCStringGetCString(err) : "Unknown error");
        if (err) OCRelease(err);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }
    double moment_value = SIScalarDoubleValue(h1_moment);
    printf("    μ_I[H1] = %f µ_N\n", moment_value);
    if (fabs(moment_value - 2.79284739) > 1e-6) {
        printf("%s failed: Expected μ_I[H1] ≈ 2.79284739, got %f\n", __func__, moment_value);
        OCRelease(h1_moment);
        return false;
    }
    OCRelease(h1_moment);
    // Test 3: Check gyromagnetic ratio 𝛾_I[H1]
    printf("  Testing 𝛾_I[H1]...\n");
    SIScalarRef h1_gamma = SIScalarCreateFromExpression(STR("𝛾_I[H1]"), &err);
    if (!h1_gamma) {
        printf("Error parsing '𝛾_I[H1]': %s\n", err ? OCStringGetCString(err) : "Unknown error");
        if (err) OCRelease(err);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }
    double gamma_value = SIScalarDoubleValue(h1_gamma);
    printf("    𝛾_I[H1] = %f rad/(s·T)\n", gamma_value);
    if (fabs(gamma_value - 267522191.787411) > 1e6) {
        printf("%s failed: Expected 𝛾_I[H1] ≈ 267522191.787411, got %f\n", __func__, gamma_value);
        OCRelease(h1_gamma);
        return false;
    }
    OCRelease(h1_gamma);
    // Test 4: Check NMR frequency nmr[H1]
    printf("  Testing nmr[H1]...\n");
    SIScalarRef h1_nmr = SIScalarCreateFromExpression(STR("nmr[H1]"), &err);
    if (!h1_nmr) {
        printf("Error parsing 'nmr[H1]': %s\n", err ? OCStringGetCString(err) : "Unknown error");
        if (err) OCRelease(err);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }
    double nmr_value = SIScalarDoubleValue(h1_nmr);
    printf("    nmr[H1] = %f MHz/T\n", nmr_value);
    if (fabs(nmr_value - 42.576) > 1.0) {
        printf("%s failed: Expected nmr[H1] ≈ 42.576 MHz/T, got %f\n", __func__, nmr_value);
        OCRelease(h1_nmr);
        return false;
    }
    OCRelease(h1_nmr);
    printf("%s passed\n", __func__);
    return true;
}
