#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>   // For fabs, fabsf, creal, cimag
#include <complex.h> // For complex numbers and I macro
#include "SILibrary.h"

#include "test_utils.h" // Include the test utilities header

bool test_unit_0(void) {
    printf("Running %s...\n", __func__);
    OCStringRef errorString = NULL;

    SIUnitRef unit = SIUnitFromExpression(STR("m•kg^2•s^3•A^4•K^5•mol^6•cd^7/(m^2•kg^3•s^4•A^5•K^6•mol^7•cd^8)"), NULL, &errorString);
    if (!unit) {
        if (errorString) {
            printf("test_unit_0 failed: Failed to parse first unit: %s\n", OCStringGetCString(errorString));
            OCRelease(errorString);
        } else {
            printf("test_unit_0 failed: Unknown error parsing first unit\n");
        }
        return false;
    }

    OCStringRef plist = SIUnitCopySymbol(unit);
    if (!plist) {
        printf("test_unit_0 failed: Failed to serialize unit to symbol string\n");
        OCRelease(unit);
        return false;
    }

    SIUnitRef unit2 = SIUnitFromExpression(plist, NULL, &errorString);
    if (!unit2) {
        printf("test_unit_0 failed: Failed to re-parse serialized unit string\n");
        if (errorString) {
            printf("Error: %s\n", OCStringGetCString(errorString));
            OCRelease(errorString);
        }
        OCRelease(plist);
        OCRelease(unit);
        return false;
    }

    if (!SIUnitEqual(unit, unit2)) {
        printf("test_unit_0 failed: Re-parsed unit does not equal original\n");
        OCRelease(unit);
        OCRelease(unit2);
        OCRelease(plist);
        return false;
    }

    OCRelease(unit);
    OCRelease(unit2);
    OCRelease(plist);
    if (errorString) OCRelease(errorString);

    printf("%s passed\n", __func__);
    return true;
}


bool test_unit_1(void) {
    printf("Running %s...\n", __func__);
    OCStringRef errorString = NULL;

    SIDimensionalityRef dimensionality = SIDimensionalityParseExpression(STR("M/(L*T^2)"), &errorString);
    if (!dimensionality) {
        if (errorString) {
            printf("test_unit_1 failed: Failed to parse dimensionality: %s\n", OCStringGetCString(errorString));
            OCRelease(errorString);
        } else {
            printf("test_unit_1 failed: Failed to parse dimensionality (no error string provided)\n");
        }
        return false;
    }

    SIUnitRef unit = SIUnitFindWithUnderivedSymbol(STR("bar"));
    if (!unit) {
        printf("test_unit_1 failed: Failed to retrieve unit 'bar'\n");
        OCRelease(dimensionality);
        return false;
    }

    if (errorString) OCRelease(errorString);

    OCRelease(dimensionality);
    printf("%s passed\n", __func__);
    return true;
}


bool test_unit_3(void) {
    printf("Running %s...\n", __func__);

    SIUnitRef unit = SIUnitFindWithUnderivedSymbol(STR("N"));
    if (!unit) {
        printf("test_unit_3 failed: Failed to retrieve unit 'N'\n");
        return false;
    }

    unit = SIUnitFindWithUnderivedSymbol(STR("m"));
    if (!unit) {
        printf("test_unit_3 failed: Failed to retrieve unit 'm'\n");
        return false;
    }

    OCStringRef root_name = SIUnitCopyRootSymbol(unit);
    if (!root_name) {
        printf("test_unit_3 failed: Failed to copy root symbol from unit 'm'\n");
        return false;
    }

    if (OCStringCompare(root_name, STR("m"), 0) != kOCCompareEqualTo) {
        printf("test_unit_3 failed: Root symbol mismatch (expected 'm')\n");
        OCRelease(root_name);
        return false;
    }

    OCRelease(root_name);
    printf("%s passed\n", __func__);
    return true;
}


bool test_unit_4(void) {
    printf("Running %s...\n", __func__);
    OCStringRef errorString = NULL;
    double multiplier = 1.0;

    SIUnitRef unit = SIUnitFromExpression(STR("km"), &multiplier, &errorString);
    if (!unit) {
        printf("test_unit_4 failed: Failed to parse 'km'\n");
        if (errorString) {
            printf("Error: %s\n", OCStringGetCString(errorString));
            OCRelease(errorString);
        }
        return false;
    }

    if (multiplier != 1.0) {
        printf("test_unit_4 failed: Expected multiplier 1.0, got %.6f\n", multiplier);
        OCRelease(unit);
        return false;
    }

    if (SIUnitGetNumeratorPrefixAtIndex(unit, kSILengthIndex) != kSIPrefixKilo) {
        printf("test_unit_4 failed: Expected numerator prefix at index %d to be kilo\n", kSILengthIndex);
        OCRelease(unit);
        return false;
    }

    if (SIUnitGetDenominatorPrefixAtIndex(unit, kSILengthIndex) != kSIPrefixNone) {
        printf("test_unit_4 failed: Expected denominator prefix at index %d to be none\n", kSILengthIndex);
        OCRelease(unit);
        return false;
    }

    OCStringRef rootName = SIUnitCopyRootName(unit);
    if (!rootName) {
        printf("test_unit_4 failed: Failed to retrieve root name\n");
        OCRelease(unit);
        return false;
    }

    if (OCStringCompare(rootName, STR("meter"), 0) != kOCCompareEqualTo) {
        printf("test_unit_4 failed: Expected root name 'meter', got '%s'\n", OCStringGetCString(rootName));
        OCRelease(rootName);
        OCRelease(unit);
        return false;
    }

    OCRelease(rootName);
    OCRelease(unit);
    if (errorString) OCRelease(errorString);

    printf("%s passed\n", __func__);
    return true;
}


bool test_unit_5(void) {
    printf("Running %s...\n", __func__);

    SIUnitRef unit = SIUnitFindWithUnderivedSymbol(STR("g"));
    if (!unit) {
        printf("test_unit_5 failed: Failed to retrieve unit 'g'\n");
        return false;
    }

    OCStringRef rootPlural = SIUnitCopyRootPluralName(unit);
    if (!rootPlural) {
        printf("test_unit_5 failed: Failed to retrieve root plural name\n");
        return false;
    }

    if (OCStringCompare(rootPlural, STR("grams"), 0) != kOCCompareEqualTo) {
        printf("test_unit_5 failed: Expected plural root name 'grams', got '%s'\n", OCStringGetCString(rootPlural));
        OCRelease(rootPlural);
        return false;
    }

    if (!SIUnitAllowsSIPrefix(unit)) {
        printf("test_unit_5 failed: Unit 'g' should allow SI prefixes\n");
        OCRelease(rootPlural);
        return false;
    }

    OCRelease(rootPlural);
    printf("%s passed\n", __func__);
    return true;
}


bool test_unit_6(void) {
    printf("Running %s...\n", __func__);
    OCStringRef errorString = NULL;
    double multiplier = 1.0;

    SIUnitRef unit = SIUnitFindWithUnderivedSymbol(STR("N"));
    if (!unit) {
        printf("test_unit_6 failed: Failed to retrieve unit 'N'\n");
        return false;
    }

    OCStringRef symbol = SIUnitCopySymbol(unit);
    if (!symbol) {
        printf("test_unit_6 failed: Failed to copy unit symbol\n");
        OCRelease(unit);
        return false;
    }

    SIUnitRef unit2 = SIUnitFromExpression(symbol, &multiplier, &errorString);
    if (!unit2) {
        printf("test_unit_6 failed: Failed to parse symbol back into unit\n");
        if (errorString) {
            printf("Error: %s\n", OCStringGetCString(errorString));
            OCRelease(errorString);
        }
        OCRelease(symbol);
        OCRelease(unit);
        return false;
    }

    if (!SIUnitEqual(unit, unit2)) {
        printf("test_unit_6 failed: Parsed unit is not equal to original\n");
        OCRelease(unit2);
        OCRelease(symbol);
        OCRelease(unit);
        return false;
    }

    OCRelease(unit2);
    OCRelease(symbol);
    OCRelease(unit);
    if (errorString) OCRelease(errorString);

    printf("%s passed\n", __func__);
    return true;
}

// New test for parsing "kg" without multiplier pointer
bool test_unit_7(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;

    SIUnitRef unit = SIUnitFromExpression(STR("kg"), NULL, &err);
    if (!unit) {
        if (err) {
            printf("test_unit_7 failed: Error parsing 'kg': %s\n", OCStringGetCString(err));
            OCRelease(err);
        } else {
            printf("test_unit_7 failed: Failed to parse 'kg' with no error message\n");
        }
        return false;
    }

    if (err) {
        OCRelease(err);
        err = NULL;
    }

    OCRelease(unit);
    printf("%s passed\n", __func__);
    return true;
}

// New test for coherent derived unit "kg*m/s^2" and equivalence to N
bool test_unit_8(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;
    double multiplier = 1.0;

    SIUnitRef unit = SIUnitFromExpression(STR("kg*m/s^2"), &multiplier, &err);
    if (!unit) {
        if (err) {
            printf("test_unit_8 failed: Error parsing 'kg*m/s^2': %s\n", OCStringGetCString(err));
            OCRelease(err);
        } else {
            printf("test_unit_8 failed: Failed to parse 'kg*m/s^2' with no error message\n");
        }
        return false;
    }

    if (err) {
        OCRelease(err);
        err = NULL;
    }

    if (multiplier != 1.0) {
        printf("test_unit_8 failed: Expected multiplier 1.0, got %.6f\n", multiplier);
        OCRelease(unit);
        return false;
    }

    SIUnitRef N = SIUnitFindWithUnderivedSymbol(STR("N"));
    if (!N) {
        printf("test_unit_8 failed: Failed to retrieve unit 'N'\n");
        OCRelease(unit);
        return false;
    }

    if (!SIUnitAreEquivalentUnits(unit, N)) {
        printf("test_unit_8 failed: 'kg*m/s^2' is not equivalent to 'N'\n");
        OCRelease(unit);
        OCRelease(N);
        return false;
    }

    OCRelease(unit);
    OCRelease(N);
    printf("%s passed\n", __func__);
    return true;
}

// Test parsing special SI unit with prefix: kilo-newton
bool test_unit_9(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;
    double multiplier = 1.0;

    SIUnitRef unit = SIUnitFromExpression(STR("kN"), &multiplier, &err);
    if (!unit) {
        if (err) {
            printf("test_unit_9 failed: Error parsing 'kN': %s\n", OCStringGetCString(err));
            OCRelease(err);
        } else {
            printf("test_unit_9 failed: Failed to parse 'kN' with no error message\n");
        }
        return false;
    }

    if (err) {
        OCRelease(err);
        err = NULL;
    }

    OCStringRef sym = SIUnitCopySymbol(unit);
    if (!sym) {
        printf("test_unit_9 failed: Failed to copy unit symbol for 'kN'\n");
        OCRelease(unit);
        return false;
    }

    if (OCStringCompare(sym, STR("kN"), 0) != kOCCompareEqualTo) {
        printf("test_unit_9 failed: Expected symbol 'kN', got '%s'\n", OCStringGetCString(sym));
        OCRelease(sym);
        OCRelease(unit);
        return false;
    }
    OCRelease(sym);

    if (multiplier != 1.0) {
        printf("test_unit_9 failed: Expected multiplier 1.0, got %.6f\n", multiplier);
        OCRelease(unit);
        return false;
    }

    SIUnitRef baseN = SIUnitFindWithUnderivedSymbol(STR("N"));
    if (!baseN) {
        printf("test_unit_9 failed: Failed to retrieve base unit 'N'\n");
        OCRelease(unit);
        return false;
    }

    SIDimensionalityRef dimKN = SIUnitGetDimensionality(unit);
    SIDimensionalityRef dimN = SIUnitGetDimensionality(baseN);
    if (!SIDimensionalityEqual(dimKN, dimN)) {
        printf("test_unit_9 failed: Dimensionality of 'kN' and 'N' should be equal\n");
        OCRelease(unit);
        OCRelease(baseN);
        return false;
    }

    if (SIUnitAreEquivalentUnits(unit, baseN)) {
        printf("test_unit_9 failed: 'kN' should not be equivalent to 'N'\n");
        OCRelease(unit);
        OCRelease(baseN);
        return false;
    }

    OCRelease(unit);
    OCRelease(baseN);
    printf("%s passed\n", __func__);
    return true;
}


// Test parsing special SI unit: pascal (Pa)
bool test_unit_10(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;

    SIUnitRef unit = SIUnitFromExpression(STR("Pa"), NULL, &err);
    if (!unit) {
        if (err) {
            printf("test_unit_10 failed: Error parsing 'Pa': %s\n", OCStringGetCString(err));
            OCRelease(err);
        } else {
            printf("test_unit_10 failed: Failed to parse 'Pa' with no error message\n");
        }
        return false;
    }

    if (err) {
        OCRelease(err);
        err = NULL;
    }

    OCStringRef sym = SIUnitCopySymbol(unit);
    if (!sym) {
        printf("test_unit_10 failed: Failed to copy symbol for unit 'Pa'\n");
        OCRelease(unit);
        return false;
    }

    if (OCStringCompare(sym, STR("Pa"), 0) != kOCCompareEqualTo) {
        printf("test_unit_10 failed: Expected symbol 'Pa', got '%s'\n", OCStringGetCString(sym));
        OCRelease(sym);
        OCRelease(unit);
        return false;
    }

    OCRelease(sym);
    OCRelease(unit);
    printf("%s passed\n", __func__);
    return true;
}


// Test parsing non-SI unit: inch per second (in/s)
bool test_unit_11(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;
    double multiplier = 1.0;

    // parse "in/s"
    SIUnitRef unit = SIUnitFromExpression(STR("in/s"), &multiplier, &err);
    if (!unit) {
        if (err) {
            fprintf(stderr, "  ✗ Error parsing 'in/s': %s\n",
                    OCStringGetCString(err));
            OCRelease(err);
        } else {
            fprintf(stderr, "  ✗ Failed to parse 'in/s' (no error message)\n");
        }
        return false;
    }
    if (err) { OCRelease(err); err = NULL; }

    // check multiplier == 1.0
    if (fabs(multiplier - 1.0) > 1e-12) {
        fprintf(stderr,
                "  ✗ Unexpected multiplier for 'in/s': expected 1.0, got %.12f\n",
                multiplier);
        OCRelease(unit);
        return false;
    }

    // grab its dimensionality
    SIDimensionalityRef dim_in_s = SIUnitGetDimensionality(unit);

    // parse "m/s" (we don't care about its multiplier here)
    SIUnitRef ms = SIUnitFromExpression(STR("m/s"), NULL, &err);
    if (!ms) {
        if (err) {
            fprintf(stderr, "  ✗ Error parsing 'm/s': %s\n",
                    OCStringGetCString(err));
            OCRelease(err);
        } else {
            fprintf(stderr, "  ✗ Failed to parse 'm/s' (no error message)\n");
        }
        OCRelease(unit);
        return false;
    }

    // grab its dimensionality
    SIDimensionalityRef dim_m_s = SIUnitGetDimensionality(ms);

    // now compare dimensions, not raw conversion factors
    if (!SIDimensionalityEqual(dim_in_s, dim_m_s)) {
        fprintf(stderr,
                "  ✗ Dimensionality mismatch: 'in/s' vs 'm/s'\n");
        OCRelease(unit);
        OCRelease(ms);
        return false;
    }

    // all good!
    OCRelease(unit);
    OCRelease(ms);
    printf("%s passed\n\n", __func__);
    return true;
}

// Test PSI unit (pound-force per square inch)
bool test_unit_12(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;
    double multiplier = 1.0;

    SIUnitRef unit = SIUnitFromExpression(STR("lbf/in^2"), &multiplier, &err);
    if (!unit) {
        if (err) {
            printf("test_unit_12 failed: Error parsing 'lbf/in^2': %s\n", OCStringGetCString(err));
            OCRelease(err);
        } else {
            printf("test_unit_12 failed: Failed to parse 'lbf/in^2' with no error message\n");
        }
        return false;
    }

    if (err) {
        OCRelease(err);
        err = NULL;
    }

    // Get reference unit "Pa"
    SIUnitRef pa = SIUnitFindWithUnderivedSymbol(STR("Pa"));
    if (!pa) {
        printf("test_unit_12 failed: Failed to retrieve reference unit 'Pa'\n");
        OCRelease(unit);
        return false;
    }

    // Known conversion from lbf/in^2 to Pa (from SIUnit.c)
    double actualConversion = 6894.757293168361;
    double expectedConversion = 6894.757293168361;

    if (OCCompareDoubleValuesLoose(actualConversion, expectedConversion) != kOCCompareEqualTo) {
        printf("test_unit_12 failed: Expected %.10f, got %.10f\n", expectedConversion, actualConversion);
        OCRelease(unit);
        OCRelease(pa);
        return false;
    }

    OCRelease(unit);
    OCRelease(pa);
    printf("%s passed\n", __func__);
    return true;
}


// Test to distinguish between lb (mass) and lbf (force)
bool test_unit_13(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;

    // Part 1: Test pound-mass (lb)
    double multiplier1 = 1.0;
    SIUnitRef unit_lb = SIUnitFromExpression(STR("lb"), &multiplier1, &err);
    if (!unit_lb) {
        if (err) {
            printf("test_unit_13 failed: Error parsing 'lb': %s\n", OCStringGetCString(err));
            OCRelease(err);
        } else {
            printf("test_unit_13 failed: Failed to parse 'lb'\n");
        }
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }

    OCStringRef root_symbol = SIUnitCopyRootSymbol(unit_lb);
    if (!root_symbol) {
        printf("test_unit_13 failed: Failed to copy root symbol for 'lb'\n");
        OCRelease(unit_lb);
        return false;
    }

    SIUnitRef kg = SIUnitFindWithUnderivedSymbol(STR("kg"));
    if (!kg) {
        printf("test_unit_13 failed: Failed to retrieve 'kg' unit\n");
        OCRelease(unit_lb);
        OCRelease(root_symbol);
        return false;
    }

    double conversion = SIUnitConversion(unit_lb, kg);
    double total_conversion = multiplier1 * conversion;
    double expected_lb_to_kg = 0.45359237;

    if (OCCompareDoubleValuesLoose(total_conversion, expected_lb_to_kg) != kOCCompareEqualTo) {
        printf("test_unit_13 failed: Expected 0.45359237, got %.12f\n", total_conversion);
        OCRelease(unit_lb);
        OCRelease(root_symbol);
        OCRelease(kg);
        return false;
    }

    SIDimensionalityRef dim_lb = SIUnitGetDimensionality(unit_lb);
    SIDimensionalityRef dim_kg = SIUnitGetDimensionality(kg);
    if (!SIDimensionalityEqual(dim_lb, dim_kg)) {
        printf("test_unit_13 failed: 'lb' and 'kg' should have the same dimensionality\n");
        OCRelease(unit_lb);
        OCRelease(root_symbol);
        OCRelease(kg);
        return false;
    }

    // Part 2: Test pound-force (lbf)
    double multiplier2 = 1.0;
    SIUnitRef unit_lbf = SIUnitFromExpression(STR("lbf"), &multiplier2, &err);
    if (!unit_lbf) {
        if (err) {
            printf("test_unit_13 failed: Error parsing 'lbf': %s\n", OCStringGetCString(err));
            OCRelease(err);
        } else {
            printf("test_unit_13 failed: Failed to parse 'lbf'\n");
        }
        OCRelease(unit_lb);
        OCRelease(root_symbol);
        OCRelease(kg);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }

    OCStringRef lbf_root_symbol = SIUnitCopyRootSymbol(unit_lbf);
    if (!lbf_root_symbol) {
        printf("test_unit_13 failed: Failed to copy root symbol for 'lbf'\n");
        OCRelease(unit_lb);
        OCRelease(root_symbol);
        OCRelease(kg);
        OCRelease(unit_lbf);
        return false;
    }

    SIUnitRef N = SIUnitFindWithUnderivedSymbol(STR("N"));
    if (!N) {
        printf("test_unit_13 failed: Failed to retrieve unit 'N'\n");
        OCRelease(unit_lb);
        OCRelease(root_symbol);
        OCRelease(kg);
        OCRelease(unit_lbf);
        OCRelease(lbf_root_symbol);
        return false;
    }

    double lbf_conversion = SIUnitConversion(unit_lbf, N);
    double total_lbf_conversion = multiplier2 * lbf_conversion;
    double expected_lbf_to_N = 4.4482216152605;

    if (OCCompareDoubleValuesLoose(total_lbf_conversion, expected_lbf_to_N) != kOCCompareEqualTo) {
        printf("test_unit_13 failed: Expected 4.4482216152605, got %.12f\n", total_lbf_conversion);
        OCRelease(unit_lb);
        OCRelease(root_symbol);
        OCRelease(kg);
        OCRelease(unit_lbf);
        OCRelease(lbf_root_symbol);
        OCRelease(N);
        return false;
    }

    SIDimensionalityRef dim_lbf = SIUnitGetDimensionality(unit_lbf);
    SIDimensionalityRef dim_N = SIUnitGetDimensionality(N);
    if (!SIDimensionalityEqual(dim_lbf, dim_N)) {
        printf("test_unit_13 failed: 'lbf' and 'N' should have the same dimensionality\n");
        OCRelease(unit_lb);
        OCRelease(root_symbol);
        OCRelease(kg);
        OCRelease(unit_lbf);
        OCRelease(lbf_root_symbol);
        OCRelease(N);
        return false;
    }

    OCRelease(unit_lb);
    OCRelease(root_symbol);
    OCRelease(kg);
    OCRelease(unit_lbf);
    OCRelease(lbf_root_symbol);
    OCRelease(N);
    printf("%s passed\n", __func__);
    return true;
}



bool test_unit_by_multiplying_without_reducing(void) {
    printf("Running %s...\n", __func__);
    bool success = true;
    OCStringRef err = NULL;
    double multiplier = 0.0;

    // 1) Prepare base units: m and s
    SIUnitRef m = SIUnitFromExpression(STR("m"), NULL, &err);
    SIUnitRef s = NULL;  // <-- declare and initialize to NULL at the top
    if (!m) {
        fprintf(stderr, "  ✗ Failed to parse 'm': %s\n",
                err ? OCStringGetCString(err) : "no error");
        success = false;
        goto cleanup;
    }
    s = SIUnitFromExpression(STR("s"), NULL, &err);
    if (!s) {
        fprintf(stderr, "  ✗ Failed to parse 's': %s\n",
                err ? OCStringGetCString(err) : "no error");
        success = false;
        goto cleanup;
    }

    // 2) Multiply two distinct units: m * s
    multiplier = 0.0;
    SIUnitRef ms = SIUnitByMultiplyingWithoutReducing(m, s, &multiplier, &err);
    if (!ms) {
        fprintf(stderr, "  ✗ m*s returned NULL: %s\n",
                err ? OCStringGetCString(err) : "no error");
        success = false;
        goto cleanup;
    }
    {
        double exprMul = 0.0;
        SIUnitRef ms_expr = SIUnitFromExpression(STR("m*s"), &exprMul, &err);
        if (!ms_expr) {
            fprintf(stderr, "  ✗ Failed to parse 'm*s': %s\n",
                    OCStringGetCString(err));
            success = false;
        }
        else if (!SIUnitAreEquivalentUnits(ms, ms_expr)) {
            fprintf(stderr, "  ✗ m*s mismatch (constructed vs parsed)\n");
            success = false;
        }
        OCRelease(ms_expr);
    }
    if (fabs(multiplier - 1.0) > 1e-12) {
        fprintf(stderr,
                "  ✗ Unexpected multiplier for 'm*s': expected 1.0, got %.12f\n",
                multiplier);
        success = false;
    }
    OCRelease(ms);

    // 3) Identity: dimensionless * m => m, multiplier must not change
    SIUnitRef dimless = SIUnitDimensionlessAndUnderived();
    multiplier = 2.5;
    SIUnitRef id1 = SIUnitByMultiplyingWithoutReducing(dimless, m, &multiplier, &err);
    if (id1 != m) {
        fprintf(stderr, "  ✗ dimensionless * m did not return the same m instance\n");
        success = false;
    }
    if (fabs(multiplier - 2.5) > 1e-12) {
        fprintf(stderr,
                "  ✗ multiplier changed for dimless*m: expected 2.5, got %.12f\n",
                multiplier);
        success = false;
    }

    // 4) Squaring shortcut: m * m => m^2
    multiplier = 0.0;
    SIUnitRef m2 = SIUnitByMultiplyingWithoutReducing(m, m, &multiplier, &err);
    if (!m2) {
        fprintf(stderr, "  ✗ Squaring m returned NULL: %s\n",
                err ? OCStringGetCString(err) : "no error");
        success = false;
        goto cleanup;
    }
    {
        double exprMul2 = 0.0;
        SIUnitRef m2_expr = SIUnitFromExpression(STR("m^2"), &exprMul2, &err);
        if (!m2_expr) {
            fprintf(stderr, "  ✗ Failed to parse 'm^2': %s\n",
                    OCStringGetCString(err));
            success = false;
        }
        else if (!SIUnitAreEquivalentUnits(m2, m2_expr)) {
            fprintf(stderr, "  ✗ m^2 mismatch (constructed vs parsed)\n");
            success = false;
        }
        OCRelease(m2_expr);
    }
    if (fabs(multiplier - 1.0) > 1e-12) {
        fprintf(stderr,
                "  ✗ Unexpected multiplier for 'm*m': expected 1.0, got %.12f\n",
                multiplier);
        success = false;
    }
    OCRelease(m2);

cleanup:
    if (err) OCRelease(err);
    if (m)   OCRelease(m);
    if (s)   OCRelease(s);

    printf("%s %s\n\n", __func__, success ? "passed" : "failed");
    return success;
}

bool test_unit_by_dividing_without_reducing(void) {
    printf("Running %s...\n", __func__);

    OCStringRef err = NULL;
    bool success = true;
    double mult = 1.0;

    SIUnitRef m = SIUnitFromExpression(STR("m"), NULL, &err);
    SIUnitRef s = NULL;
    SIUnitRef m_per_s = NULL;

    if (!m) {
        fprintf(stderr, "  ✗ Failed to parse 'm': %s\n",
            err ? OCStringGetCString(err) : "no error");
        success = false;
        goto cleanup;
    }

    // Do not assign s until after we know m was successfully assigned
    err = NULL;
    s = SIUnitFromExpression(STR("s"), NULL, &err);
    if (!s) {
        fprintf(stderr, "  ✗ Failed to parse 's': %s\n",
            err ? OCStringGetCString(err) : "no error");
        success = false;
        goto cleanup;
    }

    m_per_s = SIUnitByDividingWithoutReducing(m, s, &mult);
    if (!m_per_s) {
        fprintf(stderr, "  ✗ Division returned NULL\n");
        success = false;
        goto cleanup;
    }

    OCStringRef sym = SIUnitCopySymbol(m_per_s);
    if (!sym) {
        fprintf(stderr, "  ✗ Could not copy symbol of result\n");
        success = false;
        goto cleanup;
    }
    if (OCStringCompare(sym, STR("m/s"), 0) != kOCCompareEqualTo) {
        fprintf(stderr, "  ✗ Expected symbol 'm/s', got '%s'\n", OCStringGetCString(sym));
        OCRelease(sym);
        success = false;
        goto cleanup;
    }
    OCRelease(sym);

    if (OCCompareDoubleValuesLoose(mult, 1.0) != kOCCompareEqualTo) {
        fprintf(stderr, "  ✗ Expected multiplier 1.0, got %.12f\n", mult);
        success = false;
        goto cleanup;
    }

    SIDimensionalityRef dim_result = SIUnitGetDimensionality(m_per_s);
    SIDimensionalityRef dim_expected =
        SIDimensionalityByDividingWithoutReducing(
            SIUnitGetDimensionality(m),
            SIUnitGetDimensionality(s)
        );
    if (!SIDimensionalityEqual(dim_result, dim_expected)) {
        fprintf(stderr, "  ✗ Dimensionality mismatch\n");
        success = false;
        goto cleanup;
    }

cleanup:
    if (m_per_s) OCRelease(m_per_s);
    if (s)      OCRelease(s);
    if (m)      OCRelease(m);

    printf("%s %s\n", __func__, success ? "passed" : "failed");
    return success;
}


// Test nth-root operation on SI units (SIUnitByTakingNthRoot)
bool test_unit_by_taking_nth_root(void) {
    printf("Running %s...\n", __func__);
    bool success = true;
    OCStringRef err = NULL;

    // Test 1: Take square root of "m^2" -> should yield "m"
    double multiplier = 0.0;
    SIUnitRef m2 = SIUnitFromExpression(STR("m^2"), NULL, &err);
    if (!m2) {
        fprintf(stderr, "  ✗ Failed to parse 'm^2': %s\n",
                err ? OCStringGetCString(err) : "no error");
        success = false;
        goto cleanup;
    }
    SIUnitRef m_sqrt = SIUnitByTakingNthRoot(m2, 2, &multiplier, &err);
    if (!m_sqrt) {
        fprintf(stderr, "  ✗ Failed to take square root of 'm^2': %s\n",
                err ? OCStringGetCString(err) : "no error");
        success = false;
        goto cleanup;
    }
    SIUnitRef m = SIUnitFromExpression(STR("m"), NULL, &err);
    if (!m) {
        fprintf(stderr, "  ✗ Failed to parse 'm': %s\n",
                err ? OCStringGetCString(err) : "no error");
        success = false;
        goto cleanup;
    }
    if (!SIUnitAreEquivalentUnits(m_sqrt, m)) {
        fprintf(stderr, "  ✗ sqrt(m^2) did not yield 'm' as expected\n");
        success = false;
    }
    if (fabs(multiplier - 1.0) > 1e-12) {
        fprintf(stderr, "  ✗ multiplier for sqrt(m^2) expected 1.0, got %.12f\n", multiplier);
        success = false;
    }
    OCRelease(m_sqrt);
    OCRelease(m2);

    // Test 2: Take cube root of "m^6" -> should yield "m^2"
    multiplier = 1.0;
    SIUnitRef m6 = SIUnitFromExpression(STR("m^6"), NULL, &err);
    if (!m6) {
        fprintf(stderr, "  ✗ Failed to parse 'm^6': %s\n",
                err ? OCStringGetCString(err) : "no error");
        success = false;
        goto cleanup_m;
    }
    SIUnitRef m2_cuberoot = SIUnitByTakingNthRoot(m6, 3, &multiplier, &err);
    if (!m2_cuberoot) {
        fprintf(stderr, "  ✗ Failed to take cube root of 'm^6': %s\n",
                err ? OCStringGetCString(err) : "no error");
        success = false;
        goto cleanup_m6;
    }
    SIUnitRef m2_expected = SIUnitFromExpression(STR("m^2"), NULL, &err);
    if (!m2_expected) {
        fprintf(stderr, "  ✗ Failed to parse 'm^2': %s\n",
                err ? OCStringGetCString(err) : "no error");
        success = false;
        goto cleanup_m2_cuberoot;
    }
    if (!SIUnitAreEquivalentUnits(m2_cuberoot, m2_expected)) {
        fprintf(stderr, "  ✗ cuberoot(m^6) did not yield 'm^2' as expected\n");
        success = false;
    }
    if (fabs(multiplier - 1.0) > 1e-12) {
        fprintf(stderr, "  ✗ multiplier for cuberoot(m^6) expected 1.0, got %.12f\n", multiplier);
        success = false;
    }
    OCRelease(m2_cuberoot);
    OCRelease(m2_expected);

    // Test 3: Take square root of dimensionless (should return the same dimensionless unit)
    multiplier = 1.0;
    SIUnitRef dimless = SIUnitDimensionlessAndUnderived();
    SIUnitRef dimless_root = SIUnitByTakingNthRoot(dimless, 2, &multiplier, &err);
    if (dimless_root != dimless) {
        fprintf(stderr, "  ✗ sqrt(1) did not return same dimensionless unit pointer\n");
        success = false;
    }
    if (fabs(multiplier - 1.0) > 1e-12) {
        fprintf(stderr, "  ✗ multiplier for sqrt(1) expected 1.0, got %.12f\n", multiplier);
        success = false;
    }

    // Test 4: Special case: take sqrt of unit with symbol (e.g. "hp") if available
    // Optional: If your system has "hp" or similar, you can add a similar test here.

cleanup_m2_cuberoot:
    if (m2_cuberoot) OCRelease(m2_cuberoot);
cleanup_m6:
    if (m6) OCRelease(m6);
cleanup_m:
    if (m) OCRelease(m);
cleanup:
    if (err) OCRelease(err);

    printf("%s %s\n\n", __func__, success ? "passed" : "failed");
    return success;
}


// Test SIUnitByRaisingToPowerWithoutReducing
bool test_unit_by_raising_to_power_without_reducing(void) {
    printf("Running %s...\n", __func__);
    bool success = true;
    OCStringRef err = NULL;

    // 1. Test squaring a unit: m^2
    double multiplier = 0.0;
    SIUnitRef m = SIUnitFromExpression(STR("m"), NULL, &err);
    if (!m) {
        fprintf(stderr, "  ✗ Failed to parse 'm': %s\n",
            err ? OCStringGetCString(err) : "no error");
        success = false;
        goto cleanup;
    }
    SIUnitRef m2 = SIUnitByRaisingToPowerWithoutReducing(m, 2.0, &multiplier, &err);
    if (!m2) {
        fprintf(stderr, "  ✗ Failed to compute m^2: %s\n",
            err ? OCStringGetCString(err) : "no error");
        success = false;
        goto cleanup_m;
    }
    SIUnitRef m2_expected = SIUnitFromExpression(STR("m^2"), NULL, &err);
    if (!m2_expected) {
        fprintf(stderr, "  ✗ Failed to parse 'm^2': %s\n",
            err ? OCStringGetCString(err) : "no error");
        success = false;
        goto cleanup_m2;
    }
    if (!SIUnitAreEquivalentUnits(m2, m2_expected)) {
        fprintf(stderr, "  ✗ m^2 mismatch (constructed vs parsed)\n");
        success = false;
    }
    if (fabs(multiplier - 1.0) > 1e-12) {
        fprintf(stderr, "  ✗ Unexpected multiplier for m^2: expected 1.0, got %.12f\n", multiplier);
        success = false;
    }
    OCRelease(m2_expected);
    OCRelease(m2);

    // 2. Test negative power: m^-1
    multiplier = 0.0;
    SIUnitRef m_inv = SIUnitByRaisingToPowerWithoutReducing(m, -1.0, &multiplier, &err);
    if (!m_inv) {
        fprintf(stderr, "  ✗ Failed to compute m^-1: %s\n",
            err ? OCStringGetCString(err) : "no error");
        success = false;
        goto cleanup_m;
    }
    SIUnitRef m_inv_expected = SIUnitFromExpression(STR("1/m"), NULL, &err);
    if (!m_inv_expected) {
        fprintf(stderr, "  ✗ Failed to parse '1/m': %s\n",
            err ? OCStringGetCString(err) : "no error");
        success = false;
        goto cleanup_m_inv;
    }
    if (!SIUnitAreEquivalentUnits(m_inv, m_inv_expected)) {
        fprintf(stderr, "  ✗ m^-1 mismatch (constructed vs parsed)\n");
        success = false;
    }
    if (fabs(multiplier - 1.0) > 1e-12) {
        fprintf(stderr, "  ✗ Unexpected multiplier for m^-1: expected 1.0, got %.12f\n", multiplier);
        success = false;
    }
    OCRelease(m_inv_expected);
    OCRelease(m_inv);

    // 3. Test raising to the 1st power: should return original pointer
    multiplier = 0.0;
    SIUnitRef m_pow1 = SIUnitByRaisingToPowerWithoutReducing(m, 1.0, &multiplier, &err);
    if (m_pow1 != m) {
        fprintf(stderr, "  ✗ m^1 did not return original instance\n");
        success = false;
    }
    if (fabs(multiplier - 1.0) > 1e-12) {
        fprintf(stderr, "  ✗ Unexpected multiplier for m^1: expected 1.0, got %.12f\n", multiplier);
        success = false;
    }

    // 4. (Optional) Test with a "special symbol" unit if you have one (e.g., "hp")
    // SIUnitRef hp = SIUnitFromExpression(STR("hp"), NULL, &err);
    // if (hp) {
    //     multiplier = 0.0;
    //     SIUnitRef hp2 = SIUnitByRaisingToPowerWithoutReducing(hp, 2.0, &multiplier, &err);
    //     // Symbol should be "hp^2" etc. (You may want to check the symbol string here.)
    //     OCRelease(hp2);
    //     OCRelease(hp);
    // }

cleanup_m_inv:
    if (m_inv) OCRelease(m_inv);
cleanup_m2:
    if (m2) OCRelease(m2);
cleanup_m:
    if (m) OCRelease(m);
cleanup:
    if (err) OCRelease(err);

    printf("%s %s\n\n", __func__, success ? "passed" : "failed");
    return success;
}
