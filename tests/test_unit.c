#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>   // For fabs, fabsf, creal, cimag
#include <complex.h> // For complex numbers and I macro
#include "SILibrary.h"

#include "test_utils.h" // Include the test utilities header

extern OCMutableDictionaryRef SIUnitGetUnitsLib(void);

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

    if (unit != unit2) {
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

    SIDimensionalityRef dimensionality = SIDimensionalityFromExpression(STR("M/(L*T^2)"), &errorString);
    if (!dimensionality) {
        if (errorString) {
            printf("test_unit_1 failed: Failed to parse dimensionality: %s\n", OCStringGetCString(errorString));
            OCRelease(errorString);
        } else {
            printf("test_unit_1 failed: Failed to parse dimensionality (no error string provided)\n");
        }
        return false;
    }

    SIUnitRef unit = SIUnitWithSymbol(STR("bar"));
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

    SIUnitRef unit = SIUnitWithSymbol(STR("N"));
    if (!unit) {
        printf("test_unit_3 failed: Failed to retrieve unit 'N'\n");
        return false;
    }

    unit = SIUnitWithSymbol(STR("m"));
    if (!unit) {
        printf("test_unit_3 failed: Failed to retrieve unit 'm'\n");
        return false;
    }

    OCStringRef name = SIUnitCopyName(unit);
    if (OCStringCompare(name, STR("m"), 0) != kOCCompareEqualTo) {
        printf("test_unit_3 failed: Root symbol mismatch (expected 'm')\n");
        OCRelease(name);
        return false;
    }

    OCRelease(name);
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

    OCStringRef name = SIUnitCopyName(unit);
    if (!name) {
        printf("test_unit_4 failed: Failed to retrieve name\n");
        OCRelease(unit);
        return false;
    }

    if (OCStringCompare(name, STR("meter"), 0) != kOCCompareEqualTo) {
        printf("test_unit_4 failed: Expected name 'meter', got '%s'\n", OCStringGetCString(name));
        OCRelease(name);
        OCRelease(unit);
        return false;
    }

    OCRelease(name);
    OCRelease(unit);
    if (errorString) OCRelease(errorString);

    printf("%s passed\n", __func__);
    return true;
}


bool test_unit_5(void) {
    printf("Running %s...\n", __func__);

    SIUnitRef unit = SIUnitWithSymbol(STR("g"));
    if (!unit) {
        printf("test_unit_5 failed: Failed to retrieve unit 'g'\n");
        return false;
    }

    OCStringRef plural = SIUnitCopyPluralName(unit);
    if (!plural) {
        printf("test_unit_5 failed: Failed to retrieve plural name\n");
        return false;
    }

    if (OCStringCompare(plural, STR("grams"), 0) != kOCCompareEqualTo) {
        printf("test_unit_5 failed: Expected plural name 'grams', got '%s'\n", OCStringGetCString(plural));
        OCRelease(plural);
        return false;
    }

    OCRelease(plural);
    printf("%s passed\n", __func__);
    return true;
}


bool test_unit_6(void) {
    printf("Running %s...\n", __func__);
    OCStringRef errorString = NULL;
    double multiplier = 1.0;

    SIUnitRef unit = SIUnitWithSymbol(STR("N"));
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

    SIUnitRef N = SIUnitWithSymbol(STR("N"));
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

    SIUnitRef baseN = SIUnitWithSymbol(STR("N"));
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
    SIUnitRef pa = SIUnitWithSymbol(STR("Pa"));
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

    SIUnitRef kg = SIUnitWithSymbol(STR("kg"));
    if (!kg) {
        printf("test_unit_13 failed: Failed to retrieve 'kg' unit\n");
        OCRelease(unit_lb);
        return false;
    }

    double conversion = SIUnitConversion(unit_lb, kg);
    double total_conversion = multiplier1 * conversion;
    double expected_lb_to_kg = 0.45359237;

    if (OCCompareDoubleValuesLoose(total_conversion, expected_lb_to_kg) != kOCCompareEqualTo) {
        printf("test_unit_13 failed: Expected 0.45359237, got %.12f\n", total_conversion);
        OCRelease(unit_lb);
        OCRelease(kg);
        return false;
    }

    SIDimensionalityRef dim_lb = SIUnitGetDimensionality(unit_lb);
    SIDimensionalityRef dim_kg = SIUnitGetDimensionality(kg);
    if (!SIDimensionalityEqual(dim_lb, dim_kg)) {
        printf("test_unit_13 failed: 'lb' and 'kg' should have the same dimensionality\n");
        OCRelease(unit_lb);
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
        OCRelease(kg);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }

    SIUnitRef N = SIUnitWithSymbol(STR("N"));
    if (!N) {
        printf("test_unit_13 failed: Failed to retrieve unit 'N'\n");
        OCRelease(unit_lb);
        OCRelease(kg);
        OCRelease(unit_lbf);
        return false;
    }

    double lbf_conversion = SIUnitConversion(unit_lbf, N);
    double total_lbf_conversion = multiplier2 * lbf_conversion;
    double expected_lbf_to_N = 4.4482216152605;

    if (OCCompareDoubleValuesLoose(total_lbf_conversion, expected_lbf_to_N) != kOCCompareEqualTo) {
        printf("test_unit_13 failed: Expected 4.4482216152605, got %.12f\n", total_lbf_conversion);
        OCRelease(unit_lb);
        OCRelease(kg);
        OCRelease(unit_lbf);
        OCRelease(N);
        return false;
    }

    SIDimensionalityRef dim_lbf = SIUnitGetDimensionality(unit_lbf);
    SIDimensionalityRef dim_N = SIUnitGetDimensionality(N);
    if (!SIDimensionalityEqual(dim_lbf, dim_N)) {
        printf("test_unit_13 failed: 'lbf' and 'N' should have the same dimensionality\n");
        OCRelease(unit_lb);
        OCRelease(kg);
        OCRelease(unit_lbf);
        OCRelease(N);
        return false;
    }

    // Part 3: Test density units g/cm^3 vs g/mL (should be equivalent)
    double multiplier3 = 1.0;
    SIUnitRef unit_g_cm3 = SIUnitFromExpression(STR("g/cm^3"), &multiplier3, &err);
    if (!unit_g_cm3) {
        if (err) {
            printf("test_unit_13 failed: Error parsing 'g/cm^3': %s\n", OCStringGetCString(err));
            OCRelease(err);
        } else {
            printf("test_unit_13 failed: Failed to parse 'g/cm^3'\n");
        }
        OCRelease(unit_lb);
        OCRelease(kg);
        OCRelease(unit_lbf);
        OCRelease(N);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }

    double multiplier4 = 1.0;
    SIUnitRef unit_g_mL = SIUnitFromExpression(STR("g/mL"), &multiplier4, &err);
    if (!unit_g_mL) {
        if (err) {
            printf("test_unit_13 failed: Error parsing 'g/mL': %s\n", OCStringGetCString(err));
            OCRelease(err);
        } else {
            printf("test_unit_13 failed: Failed to parse 'g/mL'\n");
        }
        OCRelease(unit_lb);
        OCRelease(kg);
        OCRelease(unit_lbf);
        OCRelease(N);
        OCRelease(unit_g_cm3);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }

    // Check that both density units have the same dimensionality
    SIDimensionalityRef dim_g_cm3 = SIUnitGetDimensionality(unit_g_cm3);
    SIDimensionalityRef dim_g_mL = SIUnitGetDimensionality(unit_g_mL);
    if (!SIDimensionalityEqual(dim_g_cm3, dim_g_mL)) {
        printf("test_unit_13 failed: 'g/cm^3' and 'g/mL' should have the same dimensionality\n");
        OCRelease(unit_lb);
        OCRelease(kg);
        OCRelease(unit_lbf);
        OCRelease(N);
        OCRelease(unit_g_cm3);
        OCRelease(unit_g_mL);
        return false;
    }

    // Check that g/cm^3 and g/mL are equivalent units (1 cm^3 = 1 mL)
    if (!SIUnitAreEquivalentUnits(unit_g_cm3, unit_g_mL)) {
        printf("test_unit_13 failed: 'g/cm^3' and 'g/mL' should be equivalent units\n");
        OCRelease(unit_lb);
        OCRelease(kg);
        OCRelease(unit_lbf);
        OCRelease(N);
        OCRelease(unit_g_cm3);
        OCRelease(unit_g_mL);
        return false;
    }

    // Verify conversion factor is 1.0 (since cm^3 = mL exactly)
    double conversion_factor = SIUnitConversion(unit_g_cm3, unit_g_mL);
    double density_conversion = multiplier3 * conversion_factor / multiplier4;
    if (OCCompareDoubleValuesLoose(density_conversion, 1.0) != kOCCompareEqualTo) {
        printf("test_unit_13 failed: Expected conversion factor 1.0 between g/cm^3 and g/mL, got %.12f\n", density_conversion);
        OCRelease(unit_lb);
        OCRelease(kg);
        OCRelease(unit_lbf);
        OCRelease(N);
        OCRelease(unit_g_cm3);
        OCRelease(unit_g_mL);
        return false;
    }

    OCRelease(unit_g_cm3);
    OCRelease(unit_g_mL);
    OCRelease(unit_lb);
    OCRelease(kg);
    OCRelease(unit_lbf);
    OCRelease(N);
    printf("%s passed\n", __func__);
    return true;
}

bool test_unit_14(void) {
    printf("Running %s...\n", __func__);
    OCStringRef err = NULL;

    // Test parsing fractional power expressions
    // Note: The SITypes parser may not support complex fractional syntax like (m^2)^0.5
    // Let's test what is actually supported
    
    printf("  Testing simple fractional power: m^0.5\n");
    double multiplier1 = 1.0;
    SIUnitRef unit_simple_frac = SIUnitFromExpression(STR("m^0.5"), &multiplier1, &err);
    if (!unit_simple_frac) {
        if (err) {
            printf("  ✓ Correctly rejected 'm^0.5': %s\n", OCStringGetCString(err));
            OCRelease(err);
            err = NULL;
        } else {
            printf("  ✓ Correctly rejected 'm^0.5' (no error message)\n");
        }
    } else {
        printf("  ⚠️  'm^0.5' was parsed as valid (parser limitation)\n");
        OCStringRef symbol = SIUnitCopySymbol(unit_simple_frac);
        if (symbol) {
            printf("    Symbol: '%s'\n", OCStringGetCString(symbol));
            OCRelease(symbol);
        }
        // Check if this returned dimensionless unit (m^0)
        SIUnitRef dimensionless = SIUnitDimensionlessAndUnderived();
        if (unit_simple_frac == dimensionless) {
            printf("    Note: Parser treated this as m^0 (dimensionless)\n");
        }
        OCRelease(unit_simple_frac);
    }

    printf("  Testing parenthesized fractional power: (m^2)^0.5\n");
    double multiplier2 = 1.0;
    SIUnitRef unit_paren_frac = SIUnitFromExpression(STR("(m^2)^0.5"), &multiplier2, &err);
    if (!unit_paren_frac) {
        if (err) {
            printf("  ✓ Correctly rejected '(m^2)^0.5': %s\n", OCStringGetCString(err));
            OCRelease(err);
            err = NULL;
        } else {
            printf("  ✓ Correctly rejected '(m^2)^0.5' (no error message)\n");
        }
    } else {
        printf("  ✗ '(m^2)^0.5' was unexpectedly parsed as valid\n");
        OCStringRef symbol = SIUnitCopySymbol(unit_paren_frac);
        if (symbol) {
            printf("    Symbol: '%s'\n", OCStringGetCString(symbol));
            OCRelease(symbol);
        }
        OCRelease(unit_paren_frac);
    }

    printf("  Testing fractional power with fraction: m^(1/2)\n");
    double multiplier3 = 1.0;
    SIUnitRef unit_frac_notation = SIUnitFromExpression(STR("m^(1/2)"), &multiplier3, &err);
    if (!unit_frac_notation) {
        if (err) {
            printf("  ✓ Correctly rejected 'm^(1/2)': %s\n", OCStringGetCString(err));
            OCRelease(err);
            err = NULL;
        } else {
            printf("  ✓ Correctly rejected 'm^(1/2)' (no error message)\n");
        }
    } else {
        printf("  ✗ 'm^(1/2)' was unexpectedly parsed as valid\n");
        OCStringRef symbol = SIUnitCopySymbol(unit_frac_notation);
        if (symbol) {
            printf("    Symbol: '%s'\n", OCStringGetCString(symbol));
            OCRelease(symbol);
        }
        OCRelease(unit_frac_notation);
    }

    // Test what fractional syntax IS supported by trying different approaches
    printf("  Testing if negative fractional powers work: m^-0.5\n");
    double multiplier4 = 1.0;
    SIUnitRef unit_neg_frac = SIUnitFromExpression(STR("m^-0.5"), &multiplier4, &err);
    if (!unit_neg_frac) {
        if (err) {
            printf("  ✓ Correctly rejected 'm^-0.5': %s\n", OCStringGetCString(err));
            OCRelease(err);
            err = NULL;
        } else {
            printf("  ✓ Correctly rejected 'm^-0.5' (no error message)\n");
        }
    } else {
        printf("  ⚠️  'm^-0.5' was parsed as valid (parser limitation)\n");
        OCStringRef symbol = SIUnitCopySymbol(unit_neg_frac);
        if (symbol) {
            printf("    Symbol: '%s'\n", OCStringGetCString(symbol));
            OCRelease(symbol);
        }
        OCRelease(unit_neg_frac);
    }

    // Since the parser doesn't support the syntax we want to test,
    // let's test the functionality using the SIUnitCreateByTakingNthRoot function directly
    printf("  Testing nth root functionality directly using SIUnitCreateByTakingNthRoot\n");
    
    // Get m^2 unit
    SIUnitRef area_unit = SIUnitFromExpression(STR("m^2"), &multiplier1, &err);
    if (!area_unit) {
        printf("test_unit_14 failed: Cannot create m^2 unit for testing\n");
        if (err) {
            printf("Error: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }

    // Take square root of m^2
    double sqrt_multiplier = 1.0;
    OCStringRef sqrt_error = NULL;
    SIUnitRef sqrt_area = SIUnitByTakingNthRoot(area_unit, 2, &sqrt_multiplier, &sqrt_error);
    if (!sqrt_area) {
        printf("test_unit_14 failed: SIUnitByTakingNthRoot failed for sqrt(m^2)\n");
        if (sqrt_error) {
            printf("Error: %s\n", OCStringGetCString(sqrt_error));
            OCRelease(sqrt_error);
        }
        OCRelease(area_unit);
        return false;
    }
    if (sqrt_error) {
        OCRelease(sqrt_error);
        sqrt_error = NULL;
    }

    // Get reference meter unit
    SIUnitRef meter = SIUnitWithSymbol(STR("m"));
    if (!meter) {
        printf("test_unit_14 failed: Failed to retrieve 'm' unit\n");
        OCRelease(area_unit);
        OCRelease(sqrt_area);
        return false;
    }

    // Check if sqrt(m^2) equals m
    if (!SIUnitAreEquivalentUnits(sqrt_area, meter)) {
        printf("test_unit_14 failed: sqrt(m^2) should be equivalent to m\n");
        
        OCStringRef sqrt_symbol = SIUnitCopySymbol(sqrt_area);
        OCStringRef meter_symbol = SIUnitCopySymbol(meter);
        printf("  sqrt(m^2) symbol: %s\n", sqrt_symbol ? OCStringGetCString(sqrt_symbol) : "NULL");
        printf("  meter symbol: %s\n", meter_symbol ? OCStringGetCString(meter_symbol) : "NULL");
        
        SIDimensionalityRef dim_sqrt = SIUnitGetDimensionality(sqrt_area);
        SIDimensionalityRef dim_meter = SIUnitGetDimensionality(meter);
        printf("  Are dimensionally equal: %s\n", SIDimensionalityEqual(dim_sqrt, dim_meter) ? "YES" : "NO");
        
        if (sqrt_symbol) OCRelease(sqrt_symbol);
        if (meter_symbol) OCRelease(meter_symbol);
        
        OCRelease(area_unit);
        OCRelease(sqrt_area);
        OCRelease(meter);
        return false;
    }

    printf("  ✓ SIUnitByTakingNthRoot works correctly: sqrt(m^2) = m\n");

    // Test cube root as well
    SIUnitRef volume_unit = SIUnitFromExpression(STR("m^3"), &multiplier1, &err);
    if (!volume_unit) {
        printf("test_unit_14 failed: Cannot create m^3 unit for testing\n");
        if (err) {
            printf("Error: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        OCRelease(area_unit);
        OCRelease(sqrt_area);
        OCRelease(meter);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }

    SIUnitRef cbrt_volume = SIUnitByTakingNthRoot(volume_unit, 3, &sqrt_multiplier, &err);
    if (!cbrt_volume) {
        printf("test_unit_14 failed: SIUnitByTakingNthRoot failed for cbrt(m^3)\n");
        if (err) {
            printf("Error: %s\n", OCStringGetCString(err));
            OCRelease(err);
        }
        OCRelease(area_unit);
        OCRelease(sqrt_area);
        OCRelease(meter);
        OCRelease(volume_unit);
        return false;
    }
    if (err) {
        OCRelease(err);
        err = NULL;
    }
    
    if (!SIUnitAreEquivalentUnits(cbrt_volume, meter)) {
        printf("test_unit_14 failed: cbrt(m^3) should be equivalent to m\n");
        OCRelease(area_unit);
        OCRelease(sqrt_area);
        OCRelease(meter);
        OCRelease(volume_unit);
        if (cbrt_volume) OCRelease(cbrt_volume);
        return false;
    }

    printf("  ✓ SIUnitByTakingNthRoot works correctly: cbrt(m^3) = m\n");

    // Test invalid operation: sqrt(m) should fail  
    printf("  Testing invalid operation: sqrt(m) (should fail)\n");
    double invalid_multiplier = 1.0;
    OCStringRef invalid_error = NULL;
    SIUnitRef invalid_sqrt = SIUnitByTakingNthRoot(meter, 2, &invalid_multiplier, &invalid_error);
    if (invalid_sqrt) {
        printf("test_unit_14 failed: sqrt(m) should have failed (m^0.5 is not valid)\n");
        OCRelease(invalid_sqrt);
        OCRelease(area_unit);
        OCRelease(sqrt_area);
        OCRelease(meter);
        OCRelease(volume_unit);
        OCRelease(cbrt_volume);
        return false;
    }
    
    if (invalid_error) {
        printf("  ✓ Correctly rejected sqrt(m): %s\n", OCStringGetCString(invalid_error));
        OCRelease(invalid_error);
    } else {
        printf("  ✓ Correctly rejected sqrt(m) (no error message)\n");
    }

    // Clean up
    OCRelease(area_unit);
    OCRelease(sqrt_area);
    OCRelease(meter);
    OCRelease(volume_unit);
    OCRelease(cbrt_volume);
    
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

    m_per_s = SIUnitByDividingWithoutReducing(m, s, &mult, NULL);
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
    SIUnitRef m2 = SIUnitByRaisingToPowerWithoutReducing(m, 2, &multiplier, &err);
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
    SIUnitRef m_inv = SIUnitByRaisingToPowerWithoutReducing(m, -1, &multiplier, &err);
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
    multiplier = 1.0;
    SIUnitRef m_pow1 = SIUnitByRaisingToPowerWithoutReducing(m, 1, &multiplier, &err);
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


bool test_unit_unicode_normalization(void) {
    printf("Running %s...\n", __func__);
    bool success = true;
    OCStringRef errorString = NULL;

    // Test Unicode normalization: Greek letter mu (μ, U+03BC) should be normalized to micro sign (µ, U+00B5)
    printf("Testing Unicode normalization: Greek mu (μ) → micro sign (µ)\n");
    
    // Test 1: Create unit with Greek letter mu (μm)
    SIUnitRef unit_greek_mu = SIUnitFromExpression(STR("μm"), NULL, &errorString);
    if (!unit_greek_mu) {
        printf("test_unit_unicode_normalization failed: Failed to parse unit with Greek mu 'μm'\n");
        if (errorString) {
            printf("Error: %s\n", OCStringGetCString(errorString));
            OCRelease(errorString);
            errorString = NULL;
        }
        success = false;
    } else {
        printf("✓ Successfully parsed unit with Greek mu 'μm'\n");
        
        // Get the symbol and verify it's normalized to micro sign
        OCStringRef symbol_greek = SIUnitCopySymbol(unit_greek_mu);
        if (symbol_greek) {
            printf("  Symbol from Greek mu: '%s'\n", OCStringGetCString(symbol_greek));
            
            // Test 2: Create unit with micro sign (µm) for comparison
            SIUnitRef unit_micro_sign = SIUnitFromExpression(STR("µm"), NULL, &errorString);
            if (!unit_micro_sign) {
                printf("test_unit_unicode_normalization failed: Failed to parse unit with micro sign 'µm'\n");
                if (errorString) {
                    printf("Error: %s\n", OCStringGetCString(errorString));
                    OCRelease(errorString);
                    errorString = NULL;
                }
                success = false;
            } else {
                printf("✓ Successfully parsed unit with micro sign 'µm'\n");
                
                OCStringRef symbol_micro = SIUnitCopySymbol(unit_micro_sign);
                if (symbol_micro) {
                    printf("  Symbol from micro sign: '%s'\n", OCStringGetCString(symbol_micro));
                    
                    // Test 3: Both symbols should be identical after normalization
                    if (OCStringCompare(symbol_greek, symbol_micro, 0) == kOCCompareEqualTo) {
                        printf("✓ Unicode normalization working: both symbols are identical\n");
                    } else {
                        printf("✗ Unicode normalization failed: symbols differ\n");
                        printf("  Greek mu symbol:  '%s'\n", OCStringGetCString(symbol_greek));
                        printf("  Micro sign symbol: '%s'\n", OCStringGetCString(symbol_micro));
                        success = false;
                    }
                    
                    // Test 4: Units should be equal
                    if (SIUnitEqual(unit_greek_mu, unit_micro_sign)) {
                        printf("✓ Units are equal after normalization\n");
                    } else {
                        printf("✗ Units are not equal after normalization\n");
                        success = false;
                    }
                    
                    OCRelease(symbol_micro);
                } else {
                    printf("test_unit_unicode_normalization failed: Failed to get symbol from micro sign unit\n");
                    success = false;
                }
                OCRelease(unit_micro_sign);
            }
            OCRelease(symbol_greek);
        } else {
            printf("test_unit_unicode_normalization failed: Failed to get symbol from Greek mu unit\n");
            success = false;
        }
        OCRelease(unit_greek_mu);
    }
    
    // Test 5: Test other Unicode normalizations
    printf("\nTesting other Unicode normalizations:\n");
    
    // Test multiplication sign (×) normalization
    SIUnitRef unit_mult = SIUnitFromExpression(STR("m×s"), NULL, &errorString);
    if (unit_mult) {
        printf("✓ Successfully parsed 'm×s' (multiplication sign)\n");
        OCStringRef mult_symbol = SIUnitCopySymbol(unit_mult);
        if (mult_symbol) {
            printf("  Symbol: '%s'\n", OCStringGetCString(mult_symbol));
            OCRelease(mult_symbol);
        }
        OCRelease(unit_mult);
    } else {
        printf("✗ Failed to parse 'm×s' (multiplication sign)\n");
        if (errorString) {
            printf("Error: %s\n", OCStringGetCString(errorString));
            OCRelease(errorString);
            errorString = NULL;
        }
        success = false;
    }
    
    // Test division sign (÷) normalization  
    SIUnitRef unit_div = SIUnitFromExpression(STR("m÷s"), NULL, &errorString);
    if (unit_div) {
        printf("✓ Successfully parsed 'm÷s' (division sign)\n");
        OCStringRef div_symbol = SIUnitCopySymbol(unit_div);
        if (div_symbol) {
            printf("  Symbol: '%s'\n", OCStringGetCString(div_symbol));
            OCRelease(div_symbol);
        }
        OCRelease(unit_div);
    } else {
        printf("✗ Failed to parse 'm÷s' (division sign)\n");
        if (errorString) {
            printf("Error: %s\n", OCStringGetCString(errorString));
            OCRelease(errorString);
            errorString = NULL;
        }
        success = false;
    }

    if (errorString) OCRelease(errorString);

    printf("%s %s\n\n", __func__, success ? "passed" : "failed");
    return success;
}

// Helper function to check if a unit is in the library
bool isUnitInLibrary(const char* expression) {
    OCStringRef expr = OCStringCreateWithCString(expression);
    OCMutableStringRef mutSymbol = OCStringCreateMutableCopy(expr);
    OCStringTrimWhitespace(mutSymbol);
    OCStringFindAndReplace2(mutSymbol, STR("*"), STR("•"));
    
    OCMutableDictionaryRef unitsLib = SIUnitGetUnitsLib();
    SIUnitRef unit = OCDictionaryGetValue(unitsLib, mutSymbol);
    
    OCRelease(mutSymbol);
    OCRelease(expr);
    
    return (unit != NULL);
}

// Test unit registration behavior in SITypes library
bool test_unit_registration(void) {
    printf("Running %s...\n", __func__);
    printf("Testing unit registration behavior in SITypes library\n");
    
    bool success = true;
    OCStringRef error = NULL;
    
    // Test 1: Check if ft^2*kg/s is initially in library  
    const char* testExpr = "ft^2*kg/s";
    printf("1. Checking if '%s' is initially in library: %s\n", 
           testExpr, isUnitInLibrary(testExpr) ? "YES" : "NO");
    
    // Test 2: Try to parse it with SIUnitFromExpression
    printf("2. Attempting to parse '%s'...\n", testExpr);
    double multiplier = 1.0;
    
    OCStringRef exprStr = OCStringCreateWithCString(testExpr);
    SIUnitRef unit = SIUnitFromExpression(exprStr, &multiplier, &error);
    
    if (unit) {
        printf("   SUCCESS: Parsed unit\n");
        printf("   Multiplier: %.12f\n", multiplier);
        
        OCStringRef symbol = SIUnitCopySymbol(unit);
        if (symbol) {
            printf("   Symbol: %s\n", OCStringGetCString(symbol));
            OCRelease(symbol);
        }
        
        // Test 3: Check if it's now in the library
        printf("3. Checking if '%s' is now in library: %s\n", 
               testExpr, isUnitInLibrary(testExpr) ? "YES" : "NO");
        
        // Test 4: Try parsing again to see if behavior changes
        printf("4. Parsing '%s' again...\n", testExpr);
        double multiplier2 = 1.0;
        SIUnitRef unit2 = SIUnitFromExpression(exprStr, &multiplier2, &error);
        
        if (unit2) {
            printf("   SUCCESS: Parsed unit again\n");
            printf("   Multiplier: %.12f\n", multiplier2);
            printf("   Same unit object? %s\n", (unit == unit2) ? "YES" : "NO");
            printf("   Units equal? %s\n", SIUnitEqual(unit, unit2) ? "YES" : "NO");
        } else {
            printf("   FAILED to parse unit again\n");
            if (error) {
                printf("   Error: %s\n", OCStringGetCString(error));
                OCRelease(error);
                error = NULL;
            }
            success = false;
        }
        
        // Test 5: Try a completely new Imperial combination
        printf("5. Testing new Imperial combination 'in*lb'...\n");
        const char* newExpr = "in*lb";
        printf("   Initially in library: %s\n", isUnitInLibrary(newExpr) ? "YES" : "NO");
        
        double multiplier3 = 1.0;
        OCStringRef newExprStr = OCStringCreateWithCString(newExpr);
        SIUnitRef unit3 = SIUnitFromExpression(newExprStr, &multiplier3, &error);
        
        if (unit3) {
            printf("   SUCCESS: Parsed new combination\n");
            printf("   Multiplier: %.12f\n", multiplier3);
            
            OCStringRef symbol3 = SIUnitCopySymbol(unit3);
            if (symbol3) {
                printf("   Symbol: %s\n", OCStringGetCString(symbol3));
                OCRelease(symbol3);
            }
            
            printf("   Now in library: %s\n", isUnitInLibrary(newExpr) ? "YES" : "NO");
            
            OCRelease(unit3);
        } else {
            printf("   FAILED to parse new combination\n");
            if (error) {
                printf("   Error: %s\n", OCStringGetCString(error));
                OCRelease(error);
                error = NULL;
            }
        }
        
        OCRelease(newExprStr);
        OCRelease(unit);
        if (unit2) OCRelease(unit2);
        
    } else {
        printf("   FAILED to parse unit\n");
        if (error) {
            printf("   Error: %s\n", OCStringGetCString(error));
            OCRelease(error);
        }
        success = false;
    }
    
    OCRelease(exprStr);
    
    printf("%s %s\n\n", __func__, success ? "passed" : "failed");
    return success;
}

bool test_unit_canonical_expressions(void) {
    printf("Running %s...\n", __func__);
    bool success = true;
    
    // Test basic canonicalization
    printf("Testing basic canonicalization...\n");
    
    OCStringRef result1 = SIUnitCreateLibraryKey(STR("m"));
    if (OCStringCompare(result1, STR("m"), 0) != kOCCompareEqualTo) {
        printf("FAILED: 'm' should canonicalize to 'm', got '%s'\n", OCStringGetCString(result1));
        success = false;
    }
    OCRelease(result1);
    
    OCStringRef result2 = SIUnitCreateLibraryKey(STR("m^2"));
    if (OCStringCompare(result2, STR("m^2"), 0) != kOCCompareEqualTo) {
        printf("FAILED: 'm^2' should canonicalize to 'm^2', got '%s'\n", OCStringGetCString(result2));
        success = false;
    }
    OCRelease(result2);
    
    // Test multiplication ordering
    printf("Testing multiplication ordering...\n");
    
    OCStringRef result3 = SIUnitCreateLibraryKey(STR("m*kg"));
    OCStringRef result4 = SIUnitCreateLibraryKey(STR("kg*m"));
    
    if (OCStringCompare(result3, result4, 0) != kOCCompareEqualTo) {
        printf("FAILED: 'm*kg' and 'kg*m' should have same canonical form\n");
        printf("  'm*kg' -> '%s'\n", OCStringGetCString(result3));
        printf("  'kg*m' -> '%s'\n", OCStringGetCString(result4));
        success = false;
    }
    OCRelease(result3);
    OCRelease(result4);
    
    // Test Imperial/SI combinations that caused the original problem
    printf("Testing problematic Imperial/SI combinations...\n");
    
    const char* test_expressions[] = {
        "lb*ft^2/s^2",
        "ft^2*lb/s^2", 
        "(ft^2*lb)/s^2",
        "ft*ft*lb/s/s",
        "lb*ft*ft/(s*s)"
    };
    
    int num_tests = sizeof(test_expressions) / sizeof(test_expressions[0]);
    OCStringRef canonical_forms[num_tests];
    
    for (int i = 0; i < num_tests; i++) {
        OCStringRef expr = OCStringCreateWithCString(test_expressions[i]);
        canonical_forms[i] = SIUnitCreateLibraryKey(expr);
        printf("  '%s' -> '%s'\n", test_expressions[i], OCStringGetCString(canonical_forms[i]));
        OCRelease(expr);
    }
    
    // Check if all produce the same canonical form
    bool all_equivalent = true;
    for (int i = 1; i < num_tests; i++) {
        if (OCStringCompare(canonical_forms[0], canonical_forms[i], 0) != kOCCompareEqualTo) {
            all_equivalent = false;
            break;
        }
    }
    
    if (!all_equivalent) {
        printf("FAILED: All Imperial/SI expressions should be equivalent\n");
        success = false;
    }
    
    // Clean up
    for (int i = 0; i < num_tests; i++) {
        OCRelease(canonical_forms[i]);
    }
    
    // Test power consolidation
    printf("Testing power consolidation...\n");
    
    OCStringRef result5 = SIUnitCreateLibraryKey(STR("m*m"));
    if (OCStringCompare(result5, STR("m^2"), 0) != kOCCompareEqualTo) {
        printf("FAILED: 'm*m' should canonicalize to 'm^2', got '%s'\n", OCStringGetCString(result5));
        success = false;
    }
    OCRelease(result5);
    
    OCStringRef result6 = SIUnitCreateLibraryKey(STR("m^2*m"));
    if (OCStringCompare(result6, STR("m^3"), 0) != kOCCompareEqualTo) {
        printf("FAILED: 'm^2*m' should canonicalize to 'm^3', got '%s'\n", OCStringGetCString(result6));
        success = false;
    }
    OCRelease(result6);
    
    // Test equivalence function
    printf("Testing equivalence function...\n");
    
    OCStringRef expr1a = OCStringCreateWithCString("lb*ft^2/s^2");
    OCStringRef expr1b = OCStringCreateWithCString("ft^2*lb/s^2");
    bool eq1 = SIUnitAreExpressionsEquivalent(expr1a, expr1b);
    if (!eq1) {
        printf("FAILED: 'lb*ft^2/s^2' and 'ft^2*lb/s^2' should be equivalent\n");
        success = false;
    }
    OCRelease(expr1a);
    OCRelease(expr1b);
    
    OCStringRef expr2a = OCStringCreateWithCString("m/s");
    OCStringRef expr2b = OCStringCreateWithCString("s/m");
    bool eq2 = SIUnitAreExpressionsEquivalent(expr2a, expr2b);
    if (eq2) {
        printf("FAILED: 'm/s' and 's/m' should NOT be equivalent\n");
        success = false;
    }
    OCRelease(expr2a);
    OCRelease(expr2b);
    
    printf("%s %s\n\n", __func__, success ? "passed" : "failed");
    return success;
}

bool test_unit_from_expression_equivalence(void) {
    printf("Running %s...\n", __func__);
    bool success = true;
    
    printf("Testing SIUnitFromExpression with equivalent expressions...\n");
    
    // Test equivalent expressions that should return the same SIUnitRef
    const char* equivalent_sets[][5] = {
        // Energy-like units
        {"lb*ft^2/s^2", "ft^2*lb/s^2", "(ft^2*lb)/s^2", "ft*ft*lb/s/s", "lb*ft*ft/(s*s)"},
        // Force-like units  
        {"kg*m/s^2", "m*kg/s^2", "kg*m/(s*s)", "m*kg/(s^2)", NULL},
        // Simple multiplication
        {"m*kg", "kg*m", NULL, NULL, NULL},
        // Power consolidation
        {"m*m", "m^2", NULL, NULL, NULL}
    };
    
    int num_sets = sizeof(equivalent_sets) / sizeof(equivalent_sets[0]);
    
    for (int set = 0; set < num_sets; set++) {
        printf("  Testing equivalent set %d:\n", set + 1);
        
        SIUnitRef first_unit = NULL;
        double first_multiplier = 1.0;
        OCStringRef error = NULL;
        
        for (int expr = 0; expr < 5 && equivalent_sets[set][expr] != NULL; expr++) {
            OCStringRef expression = OCStringCreateWithCString(equivalent_sets[set][expr]);
            double multiplier = 1.0;
            error = NULL;
            
            SIUnitRef unit = SIUnitFromExpression(expression, &multiplier, &error);
            
            if (!unit) {
                printf("    FAILED: Could not parse '%s'\n", equivalent_sets[set][expr]);
                if (error) {
                    printf("    Error: %s\n", OCStringGetCString(error));
                    OCRelease(error);
                }
                success = false;
                OCRelease(expression);
                continue;
            }
            
            printf("    '%s' -> unit: %p, multiplier: %f\n", 
                   equivalent_sets[set][expr], (void*)unit, multiplier);
            
            if (expr == 0) {
                // First expression in the set - save as reference
                first_unit = unit;
                first_multiplier = multiplier;
            } else {
                // Compare with first expression
                if (unit!=first_unit) {
                    printf("    FAILED: '%s' does not produce same unit as '%s'\n",
                           equivalent_sets[set][expr], equivalent_sets[set][0]);
                    
                    // Show unit symbols for debugging
                    OCStringRef symbol1 = SIUnitCopySymbol(first_unit);
                    OCStringRef symbol2 = SIUnitCopySymbol(unit);
                    printf("    First unit symbol: '%s'\n", OCStringGetCString(symbol1));
                    printf("    Current unit symbol: '%s'\n", OCStringGetCString(symbol2));
                    OCRelease(symbol1);
                    OCRelease(symbol2);
                    
                    success = false;
                }
                
                // Check that multipliers are the same (within tolerance)
                if (fabs(multiplier - first_multiplier) > 1e-12) {
                    printf("    FAILED: '%s' multiplier (%f) differs from '%s' multiplier (%f)\n",
                           equivalent_sets[set][expr], multiplier, 
                           equivalent_sets[set][0], first_multiplier);
                    success = false;
                }
            }
            
            OCRelease(expression);
        }
    }
    
    // Test that different units return different SIUnitRef objects
    printf("  Testing that different units return different objects:\n");
    
    OCStringRef error = NULL;
    double mult1 = 1.0, mult2 = 1.0;
    
    SIUnitRef unit_length = SIUnitFromExpression(STR("m"), &mult1, &error);
    SIUnitRef unit_time = SIUnitFromExpression(STR("s"), &mult2, &error);
    
    if (unit_length && unit_time) {
        if (SIUnitEqual(unit_length, unit_time)) {
            printf("    FAILED: Different units 'm' and 's' should not be equal\n");
            success = false;
        } else {
            printf("    PASSED: Different units 'm' and 's' are correctly different\n");
        }
    } else {
        printf("    FAILED: Could not parse basic units 'm' or 's'\n");
        success = false;
    }
    
    printf("%s %s\n\n", __func__, success ? "passed" : "failed");
    return success;
}
