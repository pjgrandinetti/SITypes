#include "test_unit_expression.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../src/SILibrary.h"
// Helper function to test cleaned expression creation
static bool test_unit_expression_cleaner_creation(const char* input, const char* expected, const char* test_name) {
    OCStringRef input_str = OCStringCreateWithCString(input);
    OCStringRef result = SIUnitCreateCleanedExpression(input_str);
    if (!result) {
        printf("  ✗ %s: '%s' - SIUnitCreateCleanedExpression returned NULL\n", test_name, input);
        OCRelease(input_str);
        return false;
    }
    const char* result_cstr = OCStringGetCString(result);
    bool passed = (strcmp(result_cstr, expected) == 0);
    if (!passed) {
        printf("  ✗ %s: '%s'\n", test_name, input);
        printf("    Expected: '%s'\n", expected);
        printf("    Actual:   '%s'\n", result_cstr);
    }
    OCRelease(result);
    OCRelease(input_str);
    return passed;
}
// Helper function to test equivalence
static bool test_equivalence(const char* expr1, const char* expr2, bool should_be_equal, const char* test_name) {
    OCStringRef str1 = OCStringCreateWithCString(expr1);
    OCStringRef str2 = OCStringCreateWithCString(expr2);
    bool are_equivalent = SIUnitAreExpressionsEquivalent(str1, str2);
    bool passed = (are_equivalent == should_be_equal);
    if (!passed) {
        printf("  ✗ %s:\n", test_name);
        printf("    '%s' vs '%s'\n", expr1, expr2);
        printf("    Equivalent: %s (expected: %s)\n",
               are_equivalent ? "YES" : "NO",
               should_be_equal ? "YES" : "NO");
    }
    OCRelease(str1);
    OCRelease(str2);
    return passed;
}
bool test_unit_expression_cleaner_basic_canonicalization(void) {
    printf("Running %s...\n", __func__);
    bool success = true;
    // Test basic single units - should remain unchanged
    struct {
        const char* input;
        const char* expected;
    } tests[] = {
        {"m", "m"},
        {"kg", "kg"},
        {"s", "s"},
        {"A", "A"},
        {"K", "K"},
        {"mol", "mol"},
        {"cd", "cd"},
        {"N", "N"},
        {"Pa", "Pa"},
        {"J", "J"},
        {"W", "W"},
        {"Hz", "Hz"}};
    int num_tests = sizeof(tests) / sizeof(tests[0]);
    for (int i = 0; i < num_tests; i++) {
        OCStringRef input = OCStringCreateWithCString(tests[i].input);
        OCStringRef result = SIUnitCreateCleanedExpression(input);
        OCStringRef expected = OCStringCreateWithCString(tests[i].expected);
        if (OCStringCompare(result, expected, 0) != kOCCompareEqualTo) {
            printf("FAILED: '%s' should canonicalize to '%s', got '%s'\n",
                   tests[i].input, tests[i].expected, OCStringGetCString(result));
            success = false;
        }
        OCRelease(input);
        OCRelease(expected);
        OCRelease(result);
    }
    printf("%s %s\n", __func__, success ? "passed" : "failed");
    return success;
}
bool test_unit_expression_cleaner_power_notation(void) {
    printf("Running %s...\n", __func__);
    bool success = true;
    // Test power notation cleaning
    OCStringRef tests[][2] = {
        {STR("m^2"), STR("m^2")},
        {STR("m^3"), STR("m^3")},
        {STR("m^(-1)"), STR("(1/m)")},
        {STR("m^(-2)"), STR("(1/m^2)")},
        {STR("kg^2"), STR("kg^2")},
        {STR("s^(-1)"), STR("(1/s)")},
        {STR("m^(2)"), STR("m^2")},
        {STR("m^(3)"), STR("m^3")},
        {STR("m^(-3)"), STR("(1/m^3)")}};
    int num_tests = sizeof(tests) / sizeof(tests[0]);
    for (int i = 0; i < num_tests; i++) {
        OCStringRef result = SIUnitCreateCleanedExpression(tests[i][0]);
        if (OCStringCompare(result, tests[i][1], 0) != kOCCompareEqualTo) {
            printf("FAILED: '%s' should canonicalize to '%s', got '%s'\n",
                   OCStringGetCString(tests[i][0]),
                   OCStringGetCString(tests[i][1]),
                   OCStringGetCString(result));
            success = false;
        }
        OCRelease(result);
    }
    printf("%s %s\n", __func__, success ? "passed" : "failed");
    return success;
}
bool test_unit_expression_cleaner_multiplication_ordering(void) {
    printf("Running %s...\n", __func__);
    bool success = true;
    // Test that multiplication expressions are cleaned and grouped
    OCStringRef tests[][2] = {
        {STR("m*kg"), STR("kg*m")},
        {STR("s*m"), STR("m*s")},
        {STR("A*kg*m"), STR("kg*m*A")},
        {STR("mol*cd*K"), STR("K*cd*mol")},
        {STR("N*m"), STR("m*N")},
        {STR("Pa*s"), STR("s*Pa")}};
    int num_tests = sizeof(tests) / sizeof(tests[0]);
    for (int i = 0; i < num_tests; i++) {
        OCStringRef result1 = SIUnitCreateCleanedExpression(tests[i][0]);
        OCStringRef result2 = SIUnitCreateCleanedExpression(tests[i][1]);
        if (OCStringCompare(result1, result2, 0) != kOCCompareEqualTo) {
            printf("FAILED: '%s' and '%s' should have same canonical form\n",
                   OCStringGetCString(tests[i][0]),
                   OCStringGetCString(tests[i][1]));
            printf("  '%s' -> '%s'\n", OCStringGetCString(tests[i][0]), OCStringGetCString(result1));
            printf("  '%s' -> '%s'\n", OCStringGetCString(tests[i][1]), OCStringGetCString(result2));
            success = false;
        }
        OCRelease(result1);
        OCRelease(result2);
    }
    printf("%s %s\n", __func__, success ? "passed" : "failed");
    return success;
}
bool test_unit_expression_cleaner_power_consolidation(void) {
    printf("Running %s...\n", __func__);
    bool success = true;
    // Test power consolidation for repeated units (grouping identical symbols)
    OCStringRef tests[][2] = {
        {STR("m*m"), STR("m^2")},
        {STR("m*m*m"), STR("m^3")},
        {STR("kg*kg"), STR("kg^2")},
        {STR("s*s*s*s"), STR("s^4")},
        {STR("m^2*m"), STR("m^3")},
        {STR("m*m^2"), STR("m^3")},
        {STR("m^2*m^3"), STR("m^5")},
        {STR("ft*ft*lb"), STR("ft^2•lb")},
        {STR("kg*m*m"), STR("kg•m^2")},
        {STR("A*A*A"), STR("A^3")}};
    int num_tests = sizeof(tests) / sizeof(tests[0]);
    for (int i = 0; i < num_tests; i++) {
        OCStringRef result = SIUnitCreateCleanedExpression(tests[i][0]);
        if (OCStringCompare(result, tests[i][1], 0) != kOCCompareEqualTo) {
            printf("FAILED: '%s' should canonicalize to '%s', got '%s'\n",
                   OCStringGetCString(tests[i][0]),
                   OCStringGetCString(tests[i][1]),
                   OCStringGetCString(result));
            success = false;
        }
        OCRelease(result);
    }
    printf("%s %s\n", __func__, success ? "passed" : "failed");
    return success;
}
bool test_unit_expression_cleaner_division_operations(void) {
    printf("Running %s...\n", __func__);
    bool success = true;
    // Test division operations
    OCStringRef tests[][2] = {
        {STR("m/s"), STR("m/s")},
        {STR("kg/m"), STR("kg/m")},
        {STR("m/s^2"), STR("m/s^2")},
        {STR("kg*m/s^2"), STR("kg•m/s^2")},
        {STR("m^2/s"), STR("m^2/s")},
        {STR("J/K"), STR("J/K")},
        {STR("N/m"), STR("N/m")},
        {STR("Pa*s"), STR("Pa•s")},
        {STR("kg/(m*s)"), STR("kg/(m•s)")},
        {STR("m/(s*s)"), STR("m/s^2")}};
    int num_tests = sizeof(tests) / sizeof(tests[0]);
    for (int i = 0; i < num_tests; i++) {
        OCStringRef result = SIUnitCreateCleanedExpression(tests[i][0]);
        if (OCStringCompare(result, tests[i][1], 0) != kOCCompareEqualTo) {
            printf("FAILED: '%s' should canonicalize to '%s', got '%s'\n",
                   OCStringGetCString(tests[i][0]),
                   OCStringGetCString(tests[i][1]),
                   OCStringGetCString(result));
            success = false;
        }
        OCRelease(result);
    }
    printf("%s %s\n", __func__, success ? "passed" : "failed");
    return success;
}
bool test_unit_expression_cleaner_complex_expressions(void) {
    printf("Running %s...\n", __func__);
    bool success = true;
    // Test complex expressions with multiple operations
    struct {
        const char* input;
        const char* expected;
    } tests[] = {
        {"kg*m^2/s^2", "kg•m^2/s^2"},
        {"kg*m^2/(s^2)", "kg•m^2/s^2"},
        {"(kg*m^2)/s^2", "kg•m^2/s^2"},
        {"A^2*s^4/(kg*m^3)", "A^2•s^4/(kg•m^3)"},
        {"mol*K/(Pa*s)", "K•mol/(Pa•s)"},
        {"cd*sr/m^2", "cd•sr/m^2"},
        {"kg/(A*s^3)", "kg/(A•s^3)"},
        {"m^3/(kg*s^2)", "m^3/(kg•s^2)"},
        // Include some no-cancellation cases naturally
        {"J*s/J", "J•s/J"},
        {"kg*m*s/kg/s", "kg•m•s/(kg•s)"},
        {"Pa*m/Pa", "Pa•m/Pa"},
        {"N*m^2/N/m", "N•m^2/(N•m)"}};
    int num_tests = sizeof(tests) / sizeof(tests[0]);
    for (int i = 0; i < num_tests; i++) {
        OCStringRef input = OCStringCreateWithCString(tests[i].input);
        OCStringRef expected = OCStringCreateWithCString(tests[i].expected);
        OCStringRef result = SIUnitCreateCleanedExpression(input);
        if (OCStringCompare(result, expected, 0) != kOCCompareEqualTo) {
            printf("FAILED: '%s' should canonicalize to '%s', got '%s'\n",
                   tests[i].input, tests[i].expected, OCStringGetCString(result));
            success = false;
        }
        OCRelease(input);
        OCRelease(expected);
        OCRelease(result);
    }
    printf("%s %s\n", __func__, success ? "passed" : "failed");
    return success;
}
bool test_unit_expression_cleaner_unicode_operators(void) {
    printf("Running %s...\n", __func__);
    bool success = true;
    // Test Unicode multiplication and division operators
    struct {
        const char* input;
        const char* expected;
    } tests[] = {
        {"m×s", "m•s"},
        {"m•s", "m•s"},
        {"m⋅s", "m•s"},
        {"m∙s", "m•s"},
        {"m·s", "m•s"},
        {"m÷s", "m/s"},
        {"m∕s", "m/s"},
        {"m⁄s", "m/s"},
        {"μΩ×cm", "cm•µΩ"},
        {"kg⋅m÷s^2", "kg•m/s^2"},
        {"A•mol∙cd", "A•cd•mol"},
        {"Pa⋅m^3÷K", "Pa•m^3/K"}};
    int num_tests = sizeof(tests) / sizeof(tests[0]);
    for (int i = 0; i < num_tests; i++) {
        OCStringRef input = OCStringCreateWithCString(tests[i].input);
        OCStringRef expected = OCStringCreateWithCString(tests[i].expected);
        OCStringRef result = SIUnitCreateCleanedExpression(input);
        if (OCStringCompare(result, expected, 0) != kOCCompareEqualTo) {
            printf("FAILED: '%s' should canonicalize to '%s', got '%s'\n",
                   tests[i].input, tests[i].expected, OCStringGetCString(result));
            success = false;
        }
        OCRelease(input);
        OCRelease(expected);
        OCRelease(result);
    }
    printf("%s %s\n", __func__, success ? "passed" : "failed");
    return success;
}
bool test_unit_expression_cleaner_unicode_normalization(void) {
    printf("Running %s...\n", __func__);
    bool success = true;
    // Test Unicode normalization (Greek mu vs micro sign)
    struct {
        const char* input1;
        const char* input2;
        const char* description;
    } tests[] = {
        {"μm", "µm", "Greek mu vs micro sign"},
        {"μΩ×cm", "µΩ•cm", "Greek mu in complex expression"},
        {"μF/s", "µF/s", "Greek mu in division"},
        {"kg×μm^2", "kg•µm^2", "Greek mu with powers"},
        {"μmol÷μs", "µmol/µs", "Multiple mu symbols"}};
    int num_tests = sizeof(tests) / sizeof(tests[0]);
    for (int i = 0; i < num_tests; i++) {
        OCStringRef input1 = OCStringCreateWithCString(tests[i].input1);
        OCStringRef input2 = OCStringCreateWithCString(tests[i].input2);
        OCStringRef result1 = SIUnitCreateCleanedExpression(input1);
        OCStringRef result2 = SIUnitCreateCleanedExpression(input2);
        if (OCStringCompare(result1, result2, 0) != kOCCompareEqualTo) {
            printf("FAILED: Unicode normalization for %s\n", tests[i].description);
            printf("  '%s' -> '%s'\n", tests[i].input1, OCStringGetCString(result1));
            printf("  '%s' -> '%s'\n", tests[i].input2, OCStringGetCString(result2));
            success = false;
        }
        OCRelease(input1);
        OCRelease(input2);
        OCRelease(result1);
        OCRelease(result2);
    }
    printf("%s %s\n", __func__, success ? "passed" : "failed");
    return success;
}
bool test_unit_expression_cleaner_expression_equivalence(void) {
    printf("Running %s...\n", __func__);
    bool success = true;
    // Test that equivalent expressions produce identical library keys
    const char* equivalent_groups[][6] = {
        // Imperial/SI energy expressions
        {"lb*ft^2/s^2", "ft^2*lb/s^2", "(ft^2*lb)/s^2", "ft*ft*lb/s/s", "lb*ft*ft/(s*s)", NULL},
        // Force expressions
        {"kg*m/s^2", "m*kg/s^2", "kg*m/(s*s)", "m*kg/(s^2)", NULL, NULL},
        // Simple multiplication variants
        {"m*kg", "kg*m", NULL, NULL, NULL, NULL},
        // Power consolidation
        {"m*m", "m^2", NULL, NULL, NULL, NULL},
        // Division variants
        {"m/s", "m*s^(-1)", NULL, NULL, NULL, NULL},
        // Complex expressions with Unicode
        {"μΩ•cm", "cm×µΩ", "µΩ*cm", NULL, NULL, NULL},
        // No-cancellation preservation
        {"m/m", "m÷m", NULL, NULL, NULL, NULL},
        {"kg*m/kg", "m*kg/kg", NULL, NULL, NULL, NULL},
        {"s^2/s^2", "s^2÷s^2", NULL, NULL, NULL, NULL}};
    int num_groups = sizeof(equivalent_groups) / sizeof(equivalent_groups[0]);
    for (int group = 0; group < num_groups; group++) {
        // Get canonical form of first expression
        OCStringRef first_expr = OCStringCreateWithCString(equivalent_groups[group][0]);
        OCStringRef first_canonical = SIUnitCreateCleanedExpression(first_expr);
        // Test all other expressions in the group
        for (int i = 1; i < 6 && equivalent_groups[group][i] != NULL; i++) {
            OCStringRef expr = OCStringCreateWithCString(equivalent_groups[group][i]);
            OCStringRef canonical = SIUnitCreateCleanedExpression(expr);
            if (OCStringCompare(first_canonical, canonical, 0) != kOCCompareEqualTo) {
                printf("  ✗ Equivalence group %d failure:\n", group + 1);
                printf("    '%s' -> '%s'\n", equivalent_groups[group][0], OCStringGetCString(first_canonical));
                printf("    '%s' -> '%s'\n", equivalent_groups[group][i], OCStringGetCString(canonical));
                printf("    Should be equivalent but are different\n");
                success = false;
            }
            OCRelease(expr);
            OCRelease(canonical);
        }
        OCRelease(first_expr);
        OCRelease(first_canonical);
    }
    printf("%s %s\n", __func__, success ? "passed" : "failed");
    return success;
}
bool test_unit_expression_cleaner_edge_cases(void) {
    printf("Running %s...\n", __func__);
    bool success = true;
    // Test edge cases and special behaviors (including no-cancellation)
    struct {
        const char* input;
        const char* expected;
        const char* description;
    } tests[] = {
        {"1", " ", "dimensionless unity"},
        {"m/m", "m/m", "no cancellation - plane angle"},
        {"m^2/m^2", "m^2/m^2", "no cancellation - solid angle"},
        {"kg*m/kg", "kg•m/kg", "no cancellation - mixed units"},
        {"s/s", "s/s", "no cancellation - temporal ratio"},
        {"N*m/N", "N•m/N", "no cancellation - torque ratio"},
        {"kg^0", " ", "zero power"},
        {"((m))", "m", "excessive parentheses"},
        {"m^(1)", "m", "unnecessary power notation"},
        {"", " ", "empty string"},
        {"   m   ", "m", "whitespace trimming"},
        {"m^(-1)*s^(-1)", "(1/(m•s))", "multiple negative powers"},
        {"ft*ft*ft*ft*ft", "ft^5", "many repeated units"},
        {"μm×μm×μm", "µm^3", "Unicode repeated units"},
        {"Pa/Pa", "Pa/Pa", "pressure ratio - no cancellation"},
        {"mol/mol", "mol/mol", "molar ratio - no cancellation"}};
    int num_tests = sizeof(tests) / sizeof(tests[0]);
    for (int i = 0; i < num_tests; i++) {
        OCStringRef input = OCStringCreateWithCString(tests[i].input);
        OCStringRef expected = OCStringCreateWithCString(tests[i].expected);
        OCStringRef result = SIUnitCreateCleanedExpression(input);
        if (OCStringCompare(result, expected, 0) != kOCCompareEqualTo) {
            printf("FAILED: %s\n", tests[i].description);
            printf("  Input: '%s'\n", tests[i].input);
            printf("  Expected: '%s'\n", tests[i].expected);
            printf("  Got: '%s'\n", OCStringGetCString(result));
            success = false;
        }
        OCRelease(input);
        OCRelease(expected);
        OCRelease(result);
    }
    printf("%s %s\n", __func__, success ? "passed" : "failed");
    return success;
}
bool test_unit_expression_cleaner_consistency(void) {
    printf("Running %s...\n", __func__);
    bool success = true;
    // Test that repeated calls produce identical results
    const char* test_expressions[] = {
        "m",
        "kg*m/s^2",
        "μΩ•cm",
        "ft^2*lb/s^2",
        "A^2*s^4/(kg*m^3)",
        "mol*K/(Pa⋅s)",
        // Include no-cancellation cases
        "m/m",
        "Pa/Pa",
        "kg*m/kg"};
    int num_tests = sizeof(test_expressions) / sizeof(test_expressions[0]);
    for (int i = 0; i < num_tests; i++) {
        OCStringRef expr = OCStringCreateWithCString(test_expressions[i]);
        // Call multiple times and ensure identical results
        OCStringRef result1 = SIUnitCreateCleanedExpression(expr);
        OCStringRef result2 = SIUnitCreateCleanedExpression(expr);
        OCStringRef result3 = SIUnitCreateCleanedExpression(expr);
        if (OCStringCompare(result1, result2, 0) != kOCCompareEqualTo ||
            OCStringCompare(result2, result3, 0) != kOCCompareEqualTo) {
            printf("FAILED: Inconsistent results for '%s'\n", test_expressions[i]);
            printf("  Call 1: '%s'\n", OCStringGetCString(result1));
            printf("  Call 2: '%s'\n", OCStringGetCString(result2));
            printf("  Call 3: '%s'\n", OCStringGetCString(result3));
            success = false;
        }
        OCRelease(expr);
        OCRelease(result1);
        OCRelease(result2);
        OCRelease(result3);
    }
    printf("%s %s\n", __func__, success ? "passed" : "failed");
    return success;
}
bool test_unit_expression_cleaner_parenthetical_powers(void) {
    printf("Running %s...\n", __func__);
    bool success = true;
    // Test parenthetical power expansion
    struct {
        const char* input;
        const char* expected;
        const char* description;
    } tests[] = {
        {"(m•kg)^2", "kg^2•m^2", "simple parenthetical power"},
        {"(m^2•kg/s)^4", "kg^4•m^8/s^4", "complex parenthetical power"},
        {"(kg/m)^3", "kg^3/m^3", "parenthetical power with division"},
        {"(m*s^2)^2", "m^2•s^4", "parenthetical power with existing powers"},
        {"(N•m)^2", "N^2•m^2", "parenthetical power with derived units"},
        {"(m/s)^(-2)", "s^2/m^2", "negative parenthetical power"},
        {"(A^4)*(K^5)", "A^4•K^5", "multiplication of parenthetical expressions"}};
    int num_tests = sizeof(tests) / sizeof(tests[0]);
    for (int i = 0; i < num_tests; i++) {
        OCStringRef input = OCStringCreateWithCString(tests[i].input);
        OCStringRef expected = OCStringCreateWithCString(tests[i].expected);
        OCStringRef result = SIUnitCreateCleanedExpression(input);
        if (!result || OCStringCompare(result, expected, 0) != kOCCompareEqualTo) {
            printf("FAILED: %s\n", tests[i].description);
            printf("  Input: '%s'\n", tests[i].input);
            printf("  Expected: '%s'\n", tests[i].expected);
            printf("  Got: '%s'\n", result ? OCStringGetCString(result) : "NULL");
            success = false;
        }
        OCRelease(input);
        OCRelease(expected);
        if (result) OCRelease(result);
    }
    printf("%s %s\n", __func__, success ? "passed" : "failed");
    return success;
}
bool test_unit_expression_cleaner_reciprocal_expressions(void) {
    printf("Running %s...\n", __func__);
    bool success = true;
    // Test reciprocal expressions (1/unit) - these were the critical bug cases
    struct {
        const char* input;
        const char* expected;
        const char* description;
    } tests[] = {
        {"1/m", "(1/m)", "reciprocal meter"},
        {"1/s", "(1/s)", "reciprocal second (frequency)"},
        {"1/kg", "(1/kg)", "reciprocal kilogram"},
        {"1/(m*s)", "(1/(m•s))", "reciprocal of meter-second product"},
        {"1/m^2", "(1/m^2)", "reciprocal square meter"},
        {"1/s^2", "(1/s^2)", "reciprocal square second"},
        {"1/(m^2*kg)", "(1/(kg•m^2))", "reciprocal of complex expression"},
        {"1/(kg*m*s)", "(1/(kg•m•s))", "reciprocal of three-unit product"},
        // Note: Complex expressions like "1/(m/s)" are not currently supported
        // and return NULL - this is acceptable for now as it requires more complex parsing
        // Test that standalone 1 still behaves correctly (dimensionless)
        {"1", " ", "standalone dimensionless number"},
        // Test that numeric coefficients other than 1 are rejected
        // These should return NULL (tested separately below)
    };
    int num_tests = sizeof(tests) / sizeof(tests[0]);
    for (int i = 0; i < num_tests; i++) {
        OCStringRef input = OCStringCreateWithCString(tests[i].input);
        OCStringRef expected = OCStringCreateWithCString(tests[i].expected);
        OCStringRef result = SIUnitCreateCleanedExpression(input);
        if (!result) {
            printf("  ✗ %s: '%s' returned NULL (expected '%s')\n",
                   tests[i].description, tests[i].input, tests[i].expected);
            success = false;
        } else if (OCStringCompare(result, expected, 0) != kOCCompareEqualTo) {
            printf("  ✗ %s: '%s'\n", tests[i].description, tests[i].input);
            printf("    Expected: '%s'\n", tests[i].expected);
            printf("    Actual:   '%s'\n", OCStringGetCString(result));
            success = false;
        }
        OCRelease(input);
        OCRelease(expected);
        if (result) OCRelease(result);
    }
    // Test that invalid numeric coefficients return NULL
    const char* invalid_tests[] = {
        "2/m",
        "3/s",
        "0/kg",
        "-1/m",
        "2.5/m",
        "m/1",     // This should also be NULL - units divided by numbers aren't allowed
        "1/(m/s)"  // Complex nested division not currently supported
    };
    int num_invalid = sizeof(invalid_tests) / sizeof(invalid_tests[0]);
    for (int i = 0; i < num_invalid; i++) {
        OCStringRef input = OCStringCreateWithCString(invalid_tests[i]);
        OCStringRef result = SIUnitCreateCleanedExpression(input);
        if (result != NULL) {
            printf("  ✗ Invalid expression '%s' should return NULL but got '%s'\n",
                   invalid_tests[i], OCStringGetCString(result));
            success = false;
            OCRelease(result);
        }
        OCRelease(input);
    }
    printf("%s %s\n", __func__, success ? "passed" : "failed");
    return success;
}
bool test_unit_expression_cleaner_comprehensive(void) {
    printf("Running comprehensive SIUnitCreateCleanedExpression test suite...\n\n");
    bool overall_success = true;
    overall_success &= test_unit_expression_cleaner_basic_canonicalization();
    overall_success &= test_unit_expression_cleaner_power_notation();
    overall_success &= test_unit_expression_cleaner_multiplication_ordering();
    overall_success &= test_unit_expression_cleaner_power_consolidation();
    overall_success &= test_unit_expression_cleaner_division_operations();
    overall_success &= test_unit_expression_cleaner_complex_expressions();
    overall_success &= test_unit_expression_cleaner_unicode_operators();
    overall_success &= test_unit_expression_cleaner_unicode_normalization();
    overall_success &= test_unit_expression_cleaner_expression_equivalence();
    overall_success &= test_unit_expression_cleaner_edge_cases();
    overall_success &= test_unit_expression_cleaner_parenthetical_powers();
    overall_success &= test_unit_expression_cleaner_consistency();
    overall_success &= test_unit_expression_cleaner_reciprocal_expressions();
    printf("\n=== SIUnitCreateCleanedExpression Comprehensive Test Results ===\n");
    printf("Overall result: %s\n", overall_success ? "ALL TESTS PASSED" : "SOME TESTS FAILED");
    return overall_success;
}
