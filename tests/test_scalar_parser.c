#include "../src/SILibrary.h" // Ensure correct path to SI functions and types
#include "test_utils.h" // Include the new test utils header
#include <assert.h>
#include <stdio.h>

bool test_scalar_parser_1(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;

    // Part 1: 4.3 eV parsing
    SIScalarRef scalar = SIScalarCreateWithOCString(STR("4.3 eV"), &err);
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

    SIUnitRef evUnit = SIUnitForUnderivedSymbol(STR("eV"));
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
    SIScalarRef acceleration = SIScalarCreateWithOCString(STR("9.8 m/s^2"), &err);
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

    SIScalarRef force = SIScalarCreateWithOCString(STR("500 N"), &err);
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

    SIScalarRef expected2 = SIScalarCreateWithOCString(STR("51.0204081632653 kg"), &err);
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
    SIScalarRef force = SIScalarCreateWithOCString(STR("500 N"), &err);
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
    SIScalarRef accel = SIScalarCreateWithOCString(STR("9.8 m/s^2"), &err);
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
    SIScalarRef mass2 = SIScalarCreateWithOCString(STR("500 N/(9.8 m/s^2)"), &err);
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
    SIScalarReduceUnit((SIMutableScalarRef) mass2);
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
    SIScalarRef res1 = SIScalarCreateWithOCString(STR("2^3"), &err);
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
    SIScalarRef res2 = SIScalarCreateWithOCString(STR("8"), &err);
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
    SIScalarRef s = SIScalarCreateWithOCString(STR("(2+3)(4+1)"), &err);
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
    SIScalarRef e = SIScalarCreateWithOCString(STR("25"), &err);
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
    SIScalarRef r1 = SIScalarCreateWithOCString(STR("√(9) m"), &err);
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
    SIScalarRef r2 = SIScalarCreateWithOCString(STR("3 m"), &err);
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
    SIScalarRef r3 = SIScalarCreateWithOCString(STR("∛(8) kg"), &err);
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
    SIScalarRef r4 = SIScalarCreateWithOCString(STR("2 kg"), &err);
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
    SIScalarRef d1 = SIScalarCreateWithOCString(STR("6×2 kg"), &err);
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

    SIScalarRef d2 = SIScalarCreateWithOCString(STR("12 kg"), &err);
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
    SIScalarRef d3 = SIScalarCreateWithOCString(STR("(12 ÷ 4) m"), &err);
    if (!d3) {
        if (err) {
            printf("Error parsing division: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        OCRelease(d1); OCRelease(d2);
        printf("%s failed: Division operator failed\n", __func__);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }

    SIScalarRef d4 = SIScalarCreateWithOCString(STR("3 m"), &err);
    if (!d4) {
        if (err) {
            printf("Error parsing comparison scalar for division: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        OCRelease(d1); OCRelease(d2); OCRelease(d3);
        printf("%s failed: Failed to parse comparison scalar for division\n", __func__);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }

    // Minus sign test
    SIScalarRef d5 = SIScalarCreateWithOCString(STR("−5 m"), &err);
    if (!d5) {
        if (err) {
            printf("Error parsing minus sign: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        OCRelease(d1); OCRelease(d2); OCRelease(d3); OCRelease(d4);
        printf("%s failed: Minus sign parsing failed\n", __func__);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }

    SIScalarRef d6 = SIScalarCreateWithOCString(STR("-5 m"), &err);
    if (!d6) {
        if (err) {
            printf("Error parsing comparison scalar for minus: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        OCRelease(d1); OCRelease(d2); OCRelease(d3); OCRelease(d4); OCRelease(d5);
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
        OCRelease(d1); OCRelease(d2); OCRelease(d3); OCRelease(d4); OCRelease(d5); OCRelease(d6);
        return false;
    }

    // Clean up
    OCRelease(d1); OCRelease(d2); OCRelease(d3); OCRelease(d4); OCRelease(d5); OCRelease(d6);
    printf("%s passed\n", __func__);
    return true;
}

bool test_scalar_parser_7(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;

    // Parse "5 µm"
    SIScalarRef m1 = SIScalarCreateWithOCString(STR("5 µm"), &err);
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
    SIUnitRef micron = SIUnitForUnderivedSymbol(STR("µm"));
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
    SIScalarRef q = SIScalarCreateWithOCString(STR("1 quartertsp"), &err);
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
    SIUnitRef qUnit = SIUnitForUnderivedSymbol(STR("quartertsp"));
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
    SIScalarRef bad = SIScalarCreateWithOCString(STR("2+"), &err);
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
    SIScalarRef n = SIScalarCreateWithOCString(STR("0.078 mol"), &err);
    if (!n) {
        if (err) {
            printf("Error parsing n: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        printf("%s failed: Failed to parse n\n", __func__);
        return false;
    }
    if (err) { OCRelease(err); err = NULL; }

    // R
    SIScalarRef R = SIScalarCreateWithOCString(STR("R"), &err);
    if (!R) {
        if (err) {
            printf("Error parsing R: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        OCRelease(n);
        printf("%s failed: Failed to parse R\n", __func__);
        return false;
    }
    if (err) { OCRelease(err); err = NULL; }

    // T
    SIScalarRef T = SIScalarCreateWithOCString(STR("298.15 K"), &err);
    if (!T) {
        if (err) {
            printf("Error parsing T: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        OCRelease(n); OCRelease(R);
        printf("%s failed: Failed to parse T\n", __func__);
        return false;
    }
    if (err) { OCRelease(err); err = NULL; }

    // V
    SIScalarRef V = SIScalarCreateWithOCString(STR("42.0 mL"), &err);
    if (!V) {
        if (err) {
            printf("Error parsing V: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        OCRelease(n); OCRelease(R); OCRelease(T);
        printf("%s failed: Failed to parse V\n", __func__);
        return false;
    }
    if (err) { OCRelease(err); err = NULL; }

    // n * R
    SIScalarRef nR = SIScalarCreateByMultiplying(n, R, &err);
    if (!nR) {
        if (err) {
            printf("Error computing n*R: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        OCRelease(n); OCRelease(R); OCRelease(T); OCRelease(V);
        printf("%s failed: Failed to multiply n and R\n", __func__);
        return false;
    }
    if (err) { OCRelease(err); err = NULL; }

    // n * R * T
    SIScalarRef nRT = SIScalarCreateByMultiplying(nR, T, &err);
    if (!nRT) {
        if (err) {
            printf("Error computing n*R*T: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        OCRelease(n); OCRelease(R); OCRelease(T); OCRelease(V); OCRelease(nR);
        printf("%s failed: Failed to multiply nR and T\n", __func__);
        return false;
    }
    if (err) { OCRelease(err); err = NULL; }

    // (nRT) / V
    SIScalarRef p = SIScalarCreateByDividing(nRT, V, &err);
    if (!p) {
        if (err) {
            printf("Error computing n*R*T/V: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        OCRelease(n); OCRelease(R); OCRelease(T); OCRelease(V); OCRelease(nR); OCRelease(nRT);
        printf("%s failed: Failed to divide nRT by V\n", __func__);
        return false;
    }
    if (err) { OCRelease(err); err = NULL; }

    // Expected result
    SIScalarRef expected = SIScalarCreateWithOCString(STR("4603777.340690149 Pa"), &err);
    if (!expected) {
        if (err) {
            printf("Error parsing expected pressure: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        OCRelease(n); OCRelease(R); OCRelease(T); OCRelease(V); OCRelease(nR); OCRelease(nRT); OCRelease(p);
        printf("%s failed: Failed to parse expected p\n", __func__);
        return false;
    }
    if (err) { OCRelease(err); err = NULL; }

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

        OCRelease(n); OCRelease(R); OCRelease(T); OCRelease(V);
        OCRelease(nR); OCRelease(nRT); OCRelease(p); OCRelease(expected);
        printf("%s failed: Ideal gas pressure mismatch\n", __func__);
        return false;
    }

    // Cleanup
    OCRelease(n); OCRelease(R); OCRelease(T); OCRelease(V);
    OCRelease(nR); OCRelease(nRT); OCRelease(p); OCRelease(expected);
    printf("%s passed\n", __func__);
    return true;
}
