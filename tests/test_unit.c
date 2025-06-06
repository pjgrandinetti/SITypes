#include "../src/SILibrary.h" // Updated include path to resolve missing header issue
#include <assert.h>
#include <stdio.h>
#include "test_utils.h" // Include the test utilities header

bool test_unit_0(void) {
    printf("Running %s...\n", __func__);
    OCStringRef errorString = NULL;

    SIUnitRef unit = SIUnitForParsedSymbol(STR("m•kg^2•s^3•A^4•K^5•mol^6•cd^7/(m^2•kg^3•s^4•A^5•K^6•mol^7•cd^8)"), NULL, &errorString);
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

    SIUnitRef unit2 = SIUnitForParsedSymbol(plist, NULL, &errorString);
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

    SIDimensionalityRef dimensionality = SIDimensionalityForSymbol(STR("M/(L*T^2)"), &errorString);
    if (!dimensionality) {
        if (errorString) {
            printf("test_unit_1 failed: Failed to parse dimensionality: %s\n", OCStringGetCString(errorString));
            OCRelease(errorString);
        } else {
            printf("test_unit_1 failed: Failed to parse dimensionality (no error string provided)\n");
        }
        return false;
    }

    SIUnitRef unit = SIUnitForUnderivedSymbol(STR("bar"));
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

    SIUnitRef unit = SIUnitForUnderivedSymbol(STR("N"));
    if (!unit) {
        printf("test_unit_3 failed: Failed to retrieve unit 'N'\n");
        return false;
    }

    unit = SIUnitForUnderivedSymbol(STR("m"));
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

    SIUnitRef unit = SIUnitForParsedSymbol(STR("km"), &multiplier, &errorString);
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

    SIUnitRef unit = SIUnitForUnderivedSymbol(STR("g"));
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

    SIUnitRef unit = SIUnitForUnderivedSymbol(STR("N"));
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

    SIUnitRef unit2 = SIUnitForParsedSymbol(symbol, &multiplier, &errorString);
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

    SIUnitRef unit = SIUnitForParsedSymbol(STR("kg"), NULL, &err);
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

    SIUnitRef unit = SIUnitForParsedSymbol(STR("kg*m/s^2"), &multiplier, &err);
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

    SIUnitRef N = SIUnitForUnderivedSymbol(STR("N"));
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

    SIUnitRef unit = SIUnitForParsedSymbol(STR("kN"), &multiplier, &err);
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

    SIUnitRef baseN = SIUnitForUnderivedSymbol(STR("N"));
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

    SIUnitRef unit = SIUnitForParsedSymbol(STR("Pa"), NULL, &err);
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

    SIUnitRef unit = SIUnitForParsedSymbol(STR("in/s"), &multiplier, &err);
    if (!unit) {
        if (err) {
            printf("test_unit_11 failed: Error parsing 'in/s': %s\n", OCStringGetCString(err));
            OCRelease(err);
        } else {
            printf("test_unit_11 failed: Failed to parse 'in/s' with no error message\n");
        }
        return false;
    }

    if (err) {
        OCRelease(err);
        err = NULL;
    }

    if (!(multiplier > 0.025 && multiplier < 0.026)) {
        printf("test_unit_11 failed: Multiplier for 'in/s' out of expected range: %.8f\n", multiplier);
        OCRelease(unit);
        return false;
    }

    SIUnitRef ms = SIUnitForParsedSymbol(STR("m/s"), NULL, &err);
    if (!ms) {
        if (err) {
            printf("test_unit_11 failed: Error parsing 'm/s': %s\n", OCStringGetCString(err));
            OCRelease(err);
        } else {
            printf("test_unit_11 failed: Failed to parse 'm/s' with no error message\n");
        }
        OCRelease(unit);
        return false;
    }

    if (!SIUnitAreEquivalentUnits(unit, ms)) {
        printf("test_unit_11 failed: 'in/s' is not equivalent to 'm/s'\n");
        OCRelease(unit);
        OCRelease(ms);
        return false;
    }

    OCRelease(unit);
    OCRelease(ms);
    if (err) OCRelease(err);

    printf("%s passed\n", __func__);
    return true;
}

// Test PSI unit (pound-force per square inch)
bool test_unit_12(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;
    double multiplier = 1.0;

    SIUnitRef unit = SIUnitForParsedSymbol(STR("lbf/in^2"), &multiplier, &err);
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
    SIUnitRef pa = SIUnitForUnderivedSymbol(STR("Pa"));
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
    SIUnitRef unit_lb = SIUnitForParsedSymbol(STR("lb"), &multiplier1, &err);
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

    SIUnitRef kg = SIUnitForUnderivedSymbol(STR("kg"));
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
    SIUnitRef unit_lbf = SIUnitForParsedSymbol(STR("lbf"), &multiplier2, &err);
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

    SIUnitRef N = SIUnitForUnderivedSymbol(STR("N"));
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

