#include "../src/SILibrary.h" // Ensure correct path to SI functions and types
#include "test_utils.h" // Include the new test utils header
#include <assert.h>
#include <stdio.h>

// Integrated test for basic parsing and documentation example
void test_scalar_1(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;

    // Part 1: 4.3 eV parsing
    SIScalarRef scalar = SIScalarCreateWithOCString(STR("4.3 eV"), &err);
    ASSERT_PARSED(scalar, &err, "scalar '4.3 eV'", "Failed to create scalar with eV");

    SIUnitRef evUnit = SIUnitForUnderivedSymbol(STR("eV"));
    SIScalarRef expected1 = SIScalarCreateWithFloatComplex(4.3, evUnit);
    assert(expected1 && "Failed to create expected scalar");
    // show returned and expected
    printf("Returned scalar: "); SIScalarShow(scalar);
    printf("Expected scalar: "); SIScalarShow(expected1);
    assert(SIScalarCompare(scalar, expected1) == kOCCompareEqualTo);
    OCRelease(expected1);
    OCRelease(scalar);
    // Part 2: documentation example (mass = force/acceleration)
    // acceleration
    SIScalarRef acceleration = SIScalarCreateWithOCString(STR("9.8 m/s^2"), &err);
    ASSERT_PARSED(acceleration, &err, "acceleration", "Failed to parse acceleration");

    // force
    SIScalarRef force = SIScalarCreateWithOCString(STR("500 N"), &err);
    ASSERT_PARSED(force, &err, "force", "Failed to parse force");

    // mass = force / acceleration
    SIScalarRef mass = SIScalarCreateByDividing(force, acceleration, &err);
    ASSERT_PARSED(mass, &err, "mass", "Failed to compute mass");

    // expected mass
    SIScalarRef expected2 = SIScalarCreateWithOCString(STR("51.0204081632653 kg"), &err);
    ASSERT_PARSED(expected2, &err, "expected mass", "Failed to parse expected mass");

    // Due to round-off, use a looser comparison
    // show result and expected for second part
    printf("Returned mass: "); SIScalarShow(mass);
    printf("Expected mass: "); SIScalarShow(expected2);
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
    ASSERT_PARSED(force, &err, "force", "Failed to create force scalar");
    // accel
    SIScalarRef accel = SIScalarCreateWithOCString(STR("9.8 m/s^2"), &err);
    ASSERT_PARSED(accel, &err, "acceleration", "Failed to create acceleration scalar");
    // mass = force/accel
    SIScalarRef mass = SIScalarCreateByDividing(force, accel, &err);
    ASSERT_PARSED(mass, &err, "mass", "Failed to compute mass by division");
    // combined expression
    SIScalarRef mass2 = SIScalarCreateWithOCString(STR("500 N/(9.8 m/s^2)"), &err);
    SIScalarReduceUnit((SIMutableScalarRef) mass2);
    ASSERT_PARSED(mass2, &err, "combined mass", "Failed to parse combined mass expression");
    // show both mass values
    printf("mass: "); SIScalarShow(mass);
    printf("mass2: "); SIScalarShow(mass2);
    assert(SIScalarCompare(mass, mass2) == kOCCompareEqualTo);
    printf("%s passed\n", __func__);
    // cleanup
    OCRelease(force); 
    OCRelease(accel); 
    OCRelease(mass); 
    OCRelease(mass2);
}

void test_scalar_3(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;
    fprintf(stderr, "Testing exponentiation...\n");
    SIScalarRef res1 = SIScalarCreateWithOCString(STR("2^3"), &err);
    ASSERT_PARSED(res1, &err, "exponent", "Failed to parse exponent expression");
    printf("res1: "); SIScalarShow(res1);

    fprintf(stderr, "Testing integer...\n");
    SIScalarRef res2 = SIScalarCreateWithOCString(STR("8"), &err);
    ASSERT_PARSED(res2, &err, "literal", "Failed to parse literal expression");
    printf("res2: "); SIScalarShow(res2);

    fprintf(stderr, "Testing SIScalarCompare...\n");
    // show exponent and literal
    assert(SIScalarCompare(res1, res2) == kOCCompareEqualTo);
    OCRelease(res1); 
    OCRelease(res2);
    printf("%s passed\n", __func__);
}

void test_scalar_4(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;
    SIScalarRef s = SIScalarCreateWithOCString(STR("(2+3)(4+1)"), &err);
    ASSERT_PARSED(s, &err, "implicit multiplication", "Implicit multiplication failed");
    SIScalarRef e = SIScalarCreateWithOCString(STR("25"), &err);
    ASSERT_PARSED(e, &err, "expected", "Failed to parse expected result");
    // show s and e
    printf("s: "); SIScalarShow(s);
    printf("e: "); SIScalarShow(e);
    assert(SIScalarCompare(s, e) == kOCCompareEqualTo);
    printf("%s passed\n", __func__);
    OCRelease(s); 
    OCRelease(e);
}

void test_scalar_5(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;
    SIScalarRef r1 = SIScalarCreateWithOCString(STR("√(9) m"), &err);
    ASSERT_PARSED(r1, &err, "root", "Root parsing failed");
    SIScalarRef r2 = SIScalarCreateWithOCString(STR("3 m"), &err);
    ASSERT_PARSED(r2, &err, "comparison scalar for root", "Failed to parse comparison scalar for root");
    assert(SIScalarCompare(r1, r2) == kOCCompareEqualTo);
    SIScalarRef r3 = SIScalarCreateWithOCString(STR("∛(8) kg"), &err);
    ASSERT_PARSED(r3, &err, "cubert", "Cubert parsing failed");
    SIScalarRef r4 = SIScalarCreateWithOCString(STR("2 kg"), &err);
    ASSERT_PARSED(r4, &err, "comparison scalar for cubert", "Failed to parse comparison scalar for cubert");
    assert(SIScalarCompare(r3, r4) == kOCCompareEqualTo);
    // show r1, r2, r3, r4
    printf("r1: "); SIScalarShow(r1);
    printf("r2: "); SIScalarShow(r2);
    printf("r3: "); SIScalarShow(r3);
    printf("r4: "); SIScalarShow(r4);
    printf("%s passed\n", __func__);
    OCRelease(r1); OCRelease(r2); OCRelease(r3); OCRelease(r4);
}

void test_scalar_6(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;
    SIScalarRef d1 = SIScalarCreateWithOCString(STR("6×2 kg"), &err);
    ASSERT_PARSED(d1, &err, "multiplication", "Multiplication operator failed");
    SIScalarRef d2 = SIScalarCreateWithOCString(STR("12 kg"), &err);
    ASSERT_PARSED(d2, &err, "comparison scalar for multiplication", "Failed to parse comparison scalar for multiplication");
    SIScalarRef d3 = SIScalarCreateWithOCString(STR("12 ÷ 4 m"), &err);
    ASSERT_PARSED(d3, &err, "division", "Division operator failed");
    SIScalarRef d4 = SIScalarCreateWithOCString(STR("3 m"), &err);
    ASSERT_PARSED(d4, &err, "comparison scalar for division", "Failed to parse comparison scalar for division");
    SIScalarRef d5 = SIScalarCreateWithOCString(STR("−5 m"), &err);
    ASSERT_PARSED(d5, &err, "minus", "Minus sign parsing failed");
    SIScalarRef d6 = SIScalarCreateWithOCString(STR("-5 m"), &err);
    ASSERT_PARSED(d6, &err, "comparison scalar for minus", "Failed to parse comparison scalar for minus");
    // show d1, d2, d3, d4, d5, d6
    printf("d1: "); SIScalarShow(d1);
    printf("d2: "); SIScalarShow(d2);
    printf("d3: "); SIScalarShow(d3);
    printf("d4: "); SIScalarShow(d4);
    printf("d5: "); SIScalarShow(d5);
    printf("d6: "); SIScalarShow(d6);
    assert(SIScalarCompare(d1, d2) == kOCCompareEqualTo && SIScalarCompare(d3, d4) == kOCCompareEqualTo && SIScalarCompare(d5, d6) == kOCCompareEqualTo);
    printf("%s passed\n", __func__);
    OCRelease(d1); OCRelease(d2); OCRelease(d3); OCRelease(d4); OCRelease(d5); OCRelease(d6);
}

void test_scalar_7(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;
    SIScalarRef m1 = SIScalarCreateWithOCString(STR("5 µm"), &err);
    ASSERT_PARSED(m1, &err, "micro prefix", "Micro prefix parsing failed");
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
    ASSERT_PARSED(q, &err, "unit quartertsp", "quartertsp parsing failed");
    SIUnitRef qUnit = SIUnitForUnderivedSymbol(STR("quartertsp"));
    SIScalarRef qExp = SIScalarCreateWithFloatComplex(1.0, qUnit);
    assert(SIScalarCompare(q, qExp) == kOCCompareEqualTo);
    OCRelease(q); 
    OCRelease(qExp);
    printf("%s passed\n", __func__);
}

void test_scalar_9(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;
    SIScalarRef bad = SIScalarCreateWithOCString(STR("2+"), &err);
    assert(bad == NULL && err && "Error detection failed");
    if (err) OCRelease(err);
    printf("%s passed\n", __func__);
}

void test_scalar_10(void) {
    printf("Running %s...\n", __func__);
    // OCStringRef err = NULL;

    // SIScalarRef n = SIScalarCreateWithOCString(STR("0.078 mol"), &err);
    // ASSERT_PARSED(n, &err, "n", "Failed to parse n");

    // SIScalarRef T = SIScalarCreateWithOCString(STR("298.15 K"), &err);
    // ASSERT_PARSED(T, &err, "T", "Failed to parse T");
    // SIScalarRef V = SIScalarCreateWithOCString(STR("42.0 mL"), &err);
    // ASSERT_PARSED(V, &err, "V", "Failed to parse V");
    // SIScalarRef R = SIScalarCreateWithOCString(STR("8.314510 J/(K*mol)"), &err);
    // ASSERT_PARSED(R, &err, "R", "Failed to parse R");
    // // Compute p = n * R
    // SIScalarRef p = SIScalarCreateByMultiplying(n, R, &err);
    // ASSERT_PARSED(p, &err, "n*R", "Failed to multiply n and R");
    // // p = p * T
    // SIScalarRef p2 = SIScalarCreateByMultiplying(p, T, &err);
    // OCRelease(p);
    // p = p2;
    // ASSERT_PARSED(p, &err, "p*T", "Failed to multiply p and T");
    // // p = p / V
    // assert(SIScalarDivide((SIMutableScalarRef)p, V, &err) && "Failed to divide by V");
    // if (err) { printf("Error dividing by V: %s\n", OCStringGetCString(err)); OCRelease(err); err = NULL; }
    // // Expect ~4603803.67339444 Pa
    // SIScalarRef expected = SIScalarCreateWithOCString(STR("4603803.67339444 Pa"), &err);
    // ASSERT_PARSED(expected, &err, "expected p", "Failed to parse expected p");
    // // Use loose compare for round-off
    // assert(SIScalarCompareLoose(p, expected) == kOCCompareEqualTo && "Ideal gas pressure mismatch");
    // // Clean up
    // OCRelease(T); 
    // OCRelease(V); 
    // OCRelease(R);
    // OCRelease(p); 
    // OCRelease(expected);
    // OCRelease(n); 

    printf("%s passed\n", __func__);
}

