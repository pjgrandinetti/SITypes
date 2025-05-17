#include "../src/SILibrary.h" // Ensure correct path to SI functions and types
#include <assert.h>
#include <stdio.h>

// Integrated test for basic parsing and documentation example
void test_scalar_1(void) {
    printf("Running %s...\n", __func__);
    // Part 1: 4.3 eV parsing
    OCStringRef err = NULL;
    SIScalarRef scalar = SIScalarCreateWithOCString(STR("4.3 eV"), &err);
    if (!scalar) {
        if (err) {
            printf("Error creating scalar '4.3 eV': %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        assert(0 && "Failed to create scalar with eV");
    }
    SIUnitRef evUnit = SIUnitForUnderivedSymbol(STR("eV"));
    SIScalarRef expected1 = SIScalarCreateWithFloatComplex(4.3, evUnit);
    assert(expected1 && "Failed to create expected scalar");
    assert(SIScalarCompare(scalar, expected1) == kOCCompareEqualTo);
    OCRelease(expected1);
    OCRelease(scalar);
    if (err) { OCRelease(err); err = NULL; }
    // Part 2: documentation example (mass = force/acceleration)
    // acceleration
    SIScalarRef acceleration = SIScalarCreateWithOCString(STR("9.8 m/s^2"), &err);
    if (!acceleration) {
        if (err) { printf("Error parsing acceleration: %s\n", OCStringGetCString(err)); OCRelease(err); }
        assert(0 && "Failed to parse acceleration");
    }
    if (err) { OCRelease(err); err = NULL; }

    // force
    SIScalarRef force = SIScalarCreateWithOCString(STR("500 N"), &err);
    if (!force) {
        if (err) { printf("Error parsing force: %s\n", OCStringGetCString(err)); OCRelease(err); }
        assert(0 && "Failed to parse force");
    }
    if (err) { OCRelease(err); err = NULL; }

    // mass = force / acceleration
    SIScalarRef mass = SIScalarCreateByDividing(force, acceleration, &err);
    if (!mass) {
        if (err) { printf("Error computing mass: %s\n", OCStringGetCString(err)); OCRelease(err); }
        assert(0 && "Failed to compute mass");
    }
    if (err) { OCRelease(err); err = NULL; }

    // expected mass
    SIScalarRef expected2 = SIScalarCreateWithOCString(STR("51.0204081632653 kg"), &err);
    if (!expected2) {
        if (err) { printf("Error parsing expected mass: %s\n", OCStringGetCString(err)); OCRelease(err); }
        assert(0 && "Failed to parse expected mass");
    }
    if (err) { OCRelease(err); err = NULL; }

    // Due to round-off, use a looser comparison
    assert(SIScalarCompareLoose(mass, expected2) == kOCCompareEqualTo && "Mass value mismatch");
    OCRelease(acceleration);
    OCRelease(force);
    OCRelease(mass);
    OCRelease(expected2);
    printf("%s passed\n", __func__);
}

void test_scalar_2(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;
    // force
    SIScalarRef force = SIScalarCreateWithOCString(STR("500 N"), &err);
    if (!force) { if (err) { printf("Error parsing force: %s\n", OCStringGetCString(err)); OCRelease(err); } assert(0 && "Failed to create force scalar"); }
    // accel
    SIScalarRef accel = SIScalarCreateWithOCString(STR("9.8 m/s^2"), &err);
    if (!accel) { if (err) { printf("Error parsing acceleration: %s\n", OCStringGetCString(err)); OCRelease(err); } assert(0 && "Failed to create acceleration scalar"); }
    // mass = force/accel
    SIScalarRef mass = SIScalarCreateByDividing(force, accel, &err);
    if (!mass) { if (err) { printf("Error computing mass: %s\n", OCStringGetCString(err)); OCRelease(err); } assert(0 && "Failed to compute mass by division"); }
    // combined expression
    SIScalarRef mass2 = SIScalarCreateWithOCString(STR("500 N*9.8 m/s^2"), &err);
    if (!mass2) { if (err) { printf("Error parsing combined mass: %s\n", OCStringGetCString(err)); OCRelease(err); } assert(0 && "Failed to parse combined mass expression"); }
    assert(SIScalarCompareReduced(mass, mass2));
    printf("%s passed\n", __func__);
    // cleanup
    OCRelease(force); OCRelease(accel); OCRelease(mass); OCRelease(mass2);
}

void test_scalar_3(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;
    SIScalarRef res1 = SIScalarCreateWithOCString(STR("2^3"), &err);
    if (!res1) { if (err) { printf("Error parsing exponent: %s\n", OCStringGetCString(err)); OCRelease(err); } assert(0 && "Failed to parse exponent expression"); }
    SIScalarRef res2 = SIScalarCreateWithOCString(STR("8"), &err);
    if (!res2) { if (err) { printf("Error parsing literal: %s\n", OCStringGetCString(err)); OCRelease(err); } assert(0 && "Failed to parse literal expression"); }
    assert(SIScalarCompare(res1, res2) == kOCCompareEqualTo);
    printf("%s passed\n", __func__);
    OCRelease(res1); OCRelease(res2);
}

void test_scalar_4(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;
    SIScalarRef s = SIScalarCreateWithOCString(STR("(2+3)(4+1)"), &err);
    if (!s) { if (err) { printf("Error parsing implicit multiplication: %s\n", OCStringGetCString(err)); OCRelease(err); } assert(0 && "Implicit multiplication failed"); }
    SIScalarRef e = SIScalarCreateWithOCString(STR("25"), &err);
    if (!e) { if (err) { printf("Error parsing expected: %s\n", OCStringGetCString(err)); OCRelease(err); } assert(0 && "Failed to parse expected result"); }
    assert(SIScalarCompare(s, e) == kOCCompareEqualTo);
    printf("%s passed\n", __func__);
    OCRelease(s); OCRelease(e);
}

void test_scalar_5(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;
    SIScalarRef r1 = SIScalarCreateWithOCString(STR("√(9) m"), &err);
    if (!r1) { if (err) { printf("Error parsing root: %s\n", OCStringGetCString(err)); OCRelease(err); } assert(0 && "Root parsing failed"); }
    SIScalarRef r2 = SIScalarCreateWithOCString(STR("3 m"), &err);
    assert(r2 && "Failed to parse comparison scalar");
    assert(SIScalarCompare(r1, r2) == kOCCompareEqualTo);
    SIScalarRef r3 = SIScalarCreateWithOCString(STR("∛(8) kg"), &err);
    if (!r3) { if (err) { printf("Error parsing cubert: %s\n", OCStringGetCString(err)); OCRelease(err); } assert(0 && "Cubert parsing failed"); }
    SIScalarRef r4 = SIScalarCreateWithOCString(STR("2 kg"), &err);
    assert(r4 && "Failed to parse comparison scalar");
    assert(SIScalarCompare(r3, r4) == kOCCompareEqualTo);
    printf("%s passed\n", __func__);
    OCRelease(r1); OCRelease(r2); OCRelease(r3); OCRelease(r4);
}

void test_scalar_6(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;
    SIScalarRef d1 = SIScalarCreateWithOCString(STR("6×2 kg"), &err);
    if (!d1) { if (err) { printf("Error parsing multiplication: %s\n", OCStringGetCString(err)); OCRelease(err); } assert(0 && "Multiplication operator failed"); }
    SIScalarRef d2 = SIScalarCreateWithOCString(STR("12 kg"), &err);
    assert(d2 && "Failed to parse comparison scalar");
    SIScalarRef d3 = SIScalarCreateWithOCString(STR("12 ÷ 4 m"), &err);
    if (!d3) { if (err) { printf("Error parsing division: %s\n", OCStringGetCString(err)); OCRelease(err); } assert(0 && "Division operator failed"); }
    SIScalarRef d4 = SIScalarCreateWithOCString(STR("3 m"), &err);
    assert(d4 && "Failed to parse comparison scalar");
    SIScalarRef d5 = SIScalarCreateWithOCString(STR("−5 m"), &err);
    if (!d5) { if (err) { printf("Error parsing minus: %s\n", OCStringGetCString(err)); OCRelease(err); } assert(0 && "Minus sign parsing failed"); }
    SIScalarRef d6 = SIScalarCreateWithOCString(STR("-5 m"), &err);
    assert(d6 && "Failed to parse comparison scalar");
    assert(SIScalarCompare(d1, d2) == kOCCompareEqualTo && SIScalarCompare(d3, d4) == kOCCompareEqualTo && SIScalarCompare(d5, d6) == kOCCompareEqualTo);
    printf("%s passed\n", __func__);
    OCRelease(d1); OCRelease(d2); OCRelease(d3); OCRelease(d4); OCRelease(d5); OCRelease(d6);
}

void test_scalar_7(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;
    SIScalarRef m1 = SIScalarCreateWithOCString(STR("5 µm"), &err);
    if (!m1) { if (err) { printf("Error parsing micro prefix: %s\n", OCStringGetCString(err)); OCRelease(err); } assert(0 && "Micro prefix parsing failed"); }
    SIUnitRef micron = SIUnitForUnderivedSymbol(STR("µm"));
    SIScalarRef m2 = SIScalarCreateWithFloatComplex(5.0, micron);
    assert(SIScalarCompare(m1, m2) == kOCCompareEqualTo);
    printf("%s passed\n", __func__);
    OCRelease(m1); OCRelease(m2);
}

void test_scalar_8(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;
    SIScalarRef q = SIScalarCreateWithOCString(STR("1 quartertsp"), &err);
    if (!q) { if (err) { printf("Error parsing unit quartertsp: %s\n", OCStringGetCString(err)); OCRelease(err); } assert(0 && "quartertsp parsing failed"); }
    SIUnitRef qUnit = SIUnitForUnderivedSymbol(STR("quartertsp"));
    SIScalarRef qExp = SIScalarCreateWithFloatComplex(1.0, qUnit);
    assert(SIScalarCompare(q, qExp) == kOCCompareEqualTo);
    printf("%s passed\n", __func__);
    OCRelease(q); OCRelease(qExp);
}

void test_scalar_error(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;
    SIScalarRef bad = SIScalarCreateWithOCString(STR("2+"), &err);
    assert(bad == NULL && err && "Error detection failed");
    printf("%s passed (error detected: %s)\n", __func__, OCStringGetCString(err));
    if (err) OCRelease(err);
}

void test_scalar_ideal_gas(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;
    // Define constants
    SIScalarRef n = SIScalarCreateWithOCString(STR("0.078 mol"), &err);
    if (!n) { if (err) { printf("Error parsing n: %s\n", OCStringGetCString(err)); OCRelease(err); } assert(0 && "Failed to parse n"); }
    if (err) { OCRelease(err); err = NULL; }
    SIScalarRef T = SIScalarCreateWithOCString(STR("298.15 K"), &err);
    if (!T) { if (err) { printf("Error parsing T: %s\n", OCStringGetCString(err)); OCRelease(err); } assert(0 && "Failed to parse T"); }
    if (err) { OCRelease(err); err = NULL; }
    SIScalarRef V = SIScalarCreateWithOCString(STR("42.0 mL"), &err);
    if (!V) { if (err) { printf("Error parsing V: %s\n", OCStringGetCString(err)); OCRelease(err); } assert(0 && "Failed to parse V"); }
    if (err) { OCRelease(err); err = NULL; }
    SIScalarRef R = SIScalarCreateWithOCString(STR("8.314510 J/(K*mol)"), &err);
    if (!R) { if (err) { printf("Error parsing R: %s\n", OCStringGetCString(err)); OCRelease(err); } assert(0 && "Failed to parse R"); }
    if (err) { OCRelease(err); err = NULL; }
    // Compute p = n * R
    SIScalarRef p = SIScalarCreateByMultiplying(n, R, &err);
    if (!p) { if (err) { printf("Error multiplying n*R: %s\n", OCStringGetCString(err)); OCRelease(err); } assert(0 && "Failed to multiply n and R"); }
    if (err) { OCRelease(err); err = NULL; }
    // p = p * T
    SIScalarRef p2 = SIScalarCreateByMultiplying(p, T, &err);
    OCRelease(p);
    p = p2;
    if (!p) { if (err) { printf("Error multiplying p*T: %s\n", OCStringGetCString(err)); OCRelease(err); } assert(0 && "Failed to multiply p and T"); }
    if (err) { OCRelease(err); err = NULL; }
    // p = p / V
    assert(SIScalarDivide((SIMutableScalarRef)p, V, &err) && "Failed to divide by V");
    if (err) { printf("Error dividing by V: %s\n", OCStringGetCString(err)); OCRelease(err); err = NULL; }
    // Expect ~4603803.67339444 Pa
    SIScalarRef expected = SIScalarCreateWithOCString(STR("4603803.67339444 Pa"), &err);
    assert(expected && "Failed to parse expected p");
    // Use loose compare for round-off
    assert(SIScalarCompareLoose(p, expected) == kOCCompareEqualTo && "Ideal gas pressure mismatch");
    // Clean up
    OCRelease(n); OCRelease(T); OCRelease(V); OCRelease(R);
    OCRelease(p); OCRelease(expected);
    printf("%s passed\n", __func__);
}

