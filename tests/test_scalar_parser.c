#include "../src/SILibrary.h" // Ensure correct path to SI functions and types
#include "test_utils.h" // Include the new test utils header
#include <assert.h>
#include <stdio.h>

// Integrated test for basic parsing and documentation example
void test_scalar_parser_1(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;

    // Part 1: 4.3 eV parsing
    SIScalarRef scalar = SIScalarCreateWithOCString(STR("4.3 eV"), &err);

    ASSERT_PARSED(scalar, &err, "scalar '4.3 eV'", "Failed to create scalar with eV");
    SIUnitRef evUnit = SIUnitForUnderivedSymbol(STR("eV"));
    SIScalarRef expected1 = SIScalarCreateWithFloatComplex(4.3, evUnit);

    assert(expected1 && "Failed to create expected scalar");
    // show returned and expected
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
    assert(SIScalarCompareLoose(mass, expected2) == kOCCompareEqualTo && "Mass value mismatch");
    OCRelease(acceleration);
    OCRelease(force);
    OCRelease(mass);
    OCRelease(expected2);
    printf("%s passed\n", __func__);
}

void test_scalar_parser_2(void) {
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
    assert(SIScalarCompare(mass, mass2) == kOCCompareEqualTo);
    printf("%s passed\n", __func__);
    // cleanup
    OCRelease(force); 
    OCRelease(accel); 
    OCRelease(mass); 
    OCRelease(mass2);
}

void test_scalar_parser_3(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;
    SIScalarRef res1 = SIScalarCreateWithOCString(STR("2^3"), &err);
    ASSERT_PARSED(res1, &err, "exponent", "Failed to parse exponent expression");

    SIScalarRef res2 = SIScalarCreateWithOCString(STR("8"), &err);
    ASSERT_PARSED(res2, &err, "literal", "Failed to parse literal expression");

    // show exponent and literal
    assert(SIScalarCompare(res1, res2) == kOCCompareEqualTo);
    OCRelease(res1); 
    OCRelease(res2);
    printf("%s passed\n", __func__);
}

void test_scalar_parser_4(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;
    SIScalarRef s = SIScalarCreateWithOCString(STR("(2+3)(4+1)"), &err);
    ASSERT_PARSED(s, &err, "implicit multiplication", "Implicit multiplication failed");
    SIScalarRef e = SIScalarCreateWithOCString(STR("25"), &err);
    ASSERT_PARSED(e, &err, "expected", "Failed to parse expected result");
    // show s and e
    assert(SIScalarCompare(s, e) == kOCCompareEqualTo);
    printf("%s passed\n", __func__);
    OCRelease(s); 
    OCRelease(e);
}

void test_scalar_parser_5(void) {
    printf("Running %s...\n", __func__);

    OCStringRef err = NULL;
    SIScalarRef r1 = SIScalarCreateWithOCString(STR("√(9) m"), &err);
    ASSERT_PARSED(r1, &err, "root", "Root parsing failed");

    SIScalarRef r2 = SIScalarCreateWithOCString(STR("3 m"), &err);
    ASSERT_PARSED(r2, &err, "comparison scalar for root", "Failed to parse comparison scalar for root");
    assert(SIScalarCompare(r1, r2) == kOCCompareEqualTo);
    OCRelease(r1); 
    OCRelease(r2); 

    SIScalarRef r3 = SIScalarCreateWithOCString(STR("∛(8) kg"), &err);
    ASSERT_PARSED(r3, &err, "cubert", "Cubert parsing failed");
    SIScalarRef r4 = SIScalarCreateWithOCString(STR("2 kg"), &err);
    ASSERT_PARSED(r4, &err, "comparison scalar for cubert", "Failed to parse comparison scalar for cubert");
    assert(SIScalarCompare(r3, r4) == kOCCompareEqualTo);
    OCRelease(r3); 
    OCRelease(r4);
    printf("%s passed\n", __func__);
}

void test_scalar_parser_6(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;
    SIScalarRef d1 = SIScalarCreateWithOCString(STR("6×2 kg"), &err);
    ASSERT_PARSED(d1, &err, "multiplication", "Multiplication operator failed");
    SIScalarRef d2 = SIScalarCreateWithOCString(STR("12 kg"), &err);
    ASSERT_PARSED(d2, &err, "comparison scalar for multiplication", "Failed to parse comparison scalar for multiplication");
    SIScalarRef d3 = SIScalarCreateWithOCString(STR("(12 ÷ 4) m"), &err);
    ASSERT_PARSED(d3, &err, "division", "Division operator failed");
    SIScalarRef d4 = SIScalarCreateWithOCString(STR("3 m"), &err);
    ASSERT_PARSED(d4, &err, "comparison scalar for division", "Failed to parse comparison scalar for division");
    SIScalarRef d5 = SIScalarCreateWithOCString(STR("−5 m"), &err);
    ASSERT_PARSED(d5, &err, "minus", "Minus sign parsing failed");
    SIScalarRef d6 = SIScalarCreateWithOCString(STR("-5 m"), &err);
    ASSERT_PARSED(d6, &err, "comparison scalar for minus", "Failed to parse comparison scalar for minus");
    // show d1, d2, d3, d4, d5, d6
    assert(SIScalarCompare(d1, d2) == kOCCompareEqualTo && SIScalarCompare(d3, d4) == kOCCompareEqualTo && SIScalarCompare(d5, d6) == kOCCompareEqualTo);
    printf("%s passed\n", __func__);
    OCRelease(d1); OCRelease(d2); OCRelease(d3); OCRelease(d4); OCRelease(d5); OCRelease(d6);
}

void test_scalar_parser_7(void) {
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

void test_scalar_parser_8(void) {
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

void test_scalar_parser_9(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;
    SIScalarRef bad = SIScalarCreateWithOCString(STR("2+"), &err);
    assert(bad == NULL && err && "Error detection failed");
    if (err) OCRelease(err);
    printf("%s passed\n", __func__);
}

void test_scalar_parser_10(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;

    SIScalarRef n = SIScalarCreateWithOCString(STR("0.078 mol"), &err);
    ASSERT_PARSED(n, &err, "n", "Failed to parse n");

    SIScalarRef R = SIScalarCreateWithOCString(STR("R"), &err);
    ASSERT_PARSED(R, &err, "R", "Failed to parse R");

    SIScalarRef T = SIScalarCreateWithOCString(STR("298.15 K"), &err);
    ASSERT_PARSED(T, &err, "T", "Failed to parse T");

    SIScalarRef V = SIScalarCreateWithOCString(STR("42.0 mL"), &err);
    ASSERT_PARSED(V, &err, "V", "Failed to parse V");

    SIScalarRef nR = SIScalarCreateByMultiplying(n, R, &err);
    ASSERT_PARSED(nR, &err, "n*R", "Failed to multiply n and R");

    SIScalarRef nRT = SIScalarCreateByMultiplying(nR, T, &err);
    ASSERT_PARSED(nRT, &err, "n*R*T", "Failed to multiply nR and T");

    SIScalarRef p = SIScalarCreateByDividing(nRT, V, &err);
    ASSERT_PARSED(p, &err, "n*R*T/V", "Failed to divide nRT by V");

    SIScalarRef expected = SIScalarCreateWithOCString(STR("4603777.340690149 Pa"), &err);
    ASSERT_PARSED(expected, &err, "expected p", "Failed to parse expected p");

    // Compare and print if mismatch
    OCComparisonResult cmp = SIScalarCompareLoose(p, expected);
    if (cmp != kOCCompareEqualTo) {
        OCStringRef actualStr = SIScalarCreateStringValue(p);
        OCStringRef expectedStr = SIScalarCreateStringValue(expected);
        printf("Mismatch in ideal gas pressure:\n  Computed: %s\n  Expected: %s\n",
               OCStringGetCString(actualStr),
               OCStringGetCString(expectedStr));
        OCRelease(actualStr);
        OCRelease(expectedStr);
    }
    assert(cmp == kOCCompareEqualTo && "Ideal gas pressure mismatch");

    // Cleanup
    OCRelease(T);
    OCRelease(V);
    OCRelease(R);
    OCRelease(p);
    OCRelease(expected);
    OCRelease(n);
    OCRelease(nR);
    OCRelease(nRT);

    printf("%s passed\n", __func__);
}


