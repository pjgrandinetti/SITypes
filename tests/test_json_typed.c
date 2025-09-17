#include "test_json_typed.h"
#include <complex.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "SITypes.h"
// Helper function to compare floating point values with tolerance
static bool float_equal(double a, double b, double tolerance) {
    return fabs(a - b) < tolerance;
}
// Helper function to compare complex values with tolerance
static bool complex_equal(double complex a, double complex b, double tolerance) {
    return float_equal(creal(a), creal(b), tolerance) &&
           float_equal(cimag(a), cimag(b), tolerance);
}
bool test_SIScalar_json_typed_roundtrip_simple(void) {
    // Test with a simple scalar value
    double unit_multiplier = 1.0;
    OCStringRef unit_error = NULL;
    SIUnitRef unit = SIUnitFromExpression(STR("m"), &unit_multiplier, &unit_error);
    if (!unit) {
        printf("  Failed to create unit: %s\n", unit_error ? OCStringGetCString(unit_error) : "unknown error");
        if (unit_error) OCRelease(unit_error);
        return false;
    }
    if (unit_error) OCRelease(unit_error);
    SIScalarRef original = SIScalarCreateWithDouble(42.5, unit);
    if (!original) {
        printf("  Failed to create original SIScalar\n");
        return false;
    }
    // Serialize to JSONTyped
    cJSON *json = OCTypeCopyJSON((OCTypeRef)original, true, NULL);
    if (!json) {
        printf("  Failed to serialize SIScalar to JSONTyped\n");
        OCRelease(original);
        return false;
    }
    // Deserialize back from JSONTyped
    SIScalarRef restored = (SIScalarRef)OCTypeCreateFromJSONTyped(json, NULL);
    if (!restored) {
        printf("  Failed to deserialize SIScalar from JSONTyped\n");
        cJSON_Delete(json);
        OCRelease(original);
        return false;
    }
    // Compare values
    double originalValue = SIScalarDoubleValue(original);
    double restoredValue = SIScalarDoubleValue(restored);
    if (!float_equal(originalValue, restoredValue, 1e-10)) {
        printf("  Value mismatch: original=%.10f, restored=%.10f\n", originalValue, restoredValue);
        cJSON_Delete(json);
        OCRelease(original);
        OCRelease(restored);
        return false;
    }
    // Compare units by converting both to string representation
    OCStringRef originalUnit = SIScalarCopyUnitSymbol(original);
    OCStringRef restoredUnit = SIScalarCopyUnitSymbol(restored);
    bool unitsMatch = OCStringEqual(originalUnit, restoredUnit);
    if (!unitsMatch) {
        printf("  Unit mismatch: original='%s', restored='%s'\n",
               OCStringGetCString(originalUnit), OCStringGetCString(restoredUnit));
    }
    // Cleanup
    cJSON_Delete(json);
    OCRelease(original);
    OCRelease(restored);
    OCRelease(originalUnit);
    OCRelease(restoredUnit);
    if (unitsMatch) {
    }
    return unitsMatch;
}
bool test_SIScalar_json_typed_roundtrip_complex(void) {
    // Test with a complex scalar value
    double complex complexValue = 3.0 + 4.0 * I;
    double unit_multiplier = 1.0;
    OCStringRef unit_error = NULL;
    SIUnitRef unit = SIUnitFromExpression(STR("V"), &unit_multiplier, &unit_error);
    if (!unit) {
        printf("  Failed to create unit: %s\n", unit_error ? OCStringGetCString(unit_error) : "unknown error");
        if (unit_error) OCRelease(unit_error);
        return false;
    }
    if (unit_error) OCRelease(unit_error);
    SIScalarRef original = SIScalarCreateWithDoubleComplex(complexValue, unit);
    if (!original) {
        printf("  Failed to create original complex SIScalar\n");
        return false;
    }
    // Serialize to JSONTyped
    cJSON *json = OCTypeCopyJSON((OCTypeRef)original, true, NULL);
    if (!json) {
        printf("  Failed to serialize complex SIScalar to JSONTyped\n");
        OCRelease(original);
        return false;
    }
    // Deserialize back from JSONTyped
    SIScalarRef restored = (SIScalarRef)OCTypeCreateFromJSONTyped(json, NULL);
    if (!restored) {
        printf("  Failed to deserialize complex SIScalar from JSONTyped\n");
        cJSON_Delete(json);
        OCRelease(original);
        return false;
    }
    // Compare complex values
    double complex originalValue = SIScalarDoubleComplexValue(original);
    double complex restoredValue = SIScalarDoubleComplexValue(restored);
    if (!complex_equal(originalValue, restoredValue, 1e-10)) {
        printf("  Complex value mismatch: original=%.3f+%.3fi, restored=%.3f+%.3fi\n",
               creal(originalValue), cimag(originalValue),
               creal(restoredValue), cimag(restoredValue));
        cJSON_Delete(json);
        OCRelease(original);
        OCRelease(restored);
        return false;
    }
    // Compare units
    OCStringRef originalUnit = SIScalarCopyUnitSymbol(original);
    OCStringRef restoredUnit = SIScalarCopyUnitSymbol(restored);
    bool unitsMatch = OCStringEqual(originalUnit, restoredUnit);
    if (!unitsMatch) {
        printf("  Unit mismatch: original='%s', restored='%s'\n",
               OCStringGetCString(originalUnit), OCStringGetCString(restoredUnit));
    }
    // Cleanup
    cJSON_Delete(json);
    OCRelease(original);
    OCRelease(restored);
    OCRelease(originalUnit);
    OCRelease(restoredUnit);
    if (unitsMatch) {
    }
    return unitsMatch;
}
bool test_SIScalar_json_typed_roundtrip_with_units(void) {
    // Test with compound units
    double unit_multiplier = 1.0;
    OCStringRef unit_error = NULL;
    SIUnitRef unit = SIUnitFromExpression(STR("m/s^2"), &unit_multiplier, &unit_error);
    if (!unit) {
        printf("  Failed to create unit: %s\n", unit_error ? OCStringGetCString(unit_error) : "unknown error");
        if (unit_error) OCRelease(unit_error);
        return false;
    }
    if (unit_error) OCRelease(unit_error);
    SIScalarRef original = SIScalarCreateWithDouble(9.81, unit);
    if (!original) {
        printf("  Failed to create original SIScalar with compound units\n");
        return false;
    }
    // Serialize to JSONTyped
    cJSON *json = OCTypeCopyJSON((OCTypeRef)original, true, NULL);
    if (!json) {
        printf("  Failed to serialize SIScalar with compound units to JSONTyped\n");
        OCRelease(original);
        return false;
    }
    // Deserialize back from JSONTyped
    SIScalarRef restored = (SIScalarRef)OCTypeCreateFromJSONTyped(json, NULL);
    if (!restored) {
        printf("  Failed to deserialize SIScalar with compound units from JSONTyped\n");
        cJSON_Delete(json);
        OCRelease(original);
        return false;
    }
    // Compare values and units
    bool valuesMatch = float_equal(SIScalarDoubleValue(original), SIScalarDoubleValue(restored), 1e-10);
    OCStringRef originalUnit = SIScalarCopyUnitSymbol(original);
    OCStringRef restoredUnit = SIScalarCopyUnitSymbol(restored);
    bool unitsMatch = OCStringEqual(originalUnit, restoredUnit);
    if (!valuesMatch) {
        printf("  Value mismatch: original=%.10f, restored=%.10f\n",
               SIScalarDoubleValue(original), SIScalarDoubleValue(restored));
    }
    if (!unitsMatch) {
        printf("  Unit mismatch: original='%s', restored='%s'\n",
               OCStringGetCString(originalUnit), OCStringGetCString(restoredUnit));
    }
    // Cleanup
    cJSON_Delete(json);
    OCRelease(original);
    OCRelease(restored);
    OCRelease(originalUnit);
    OCRelease(restoredUnit);
    bool success = valuesMatch && unitsMatch;
    if (success) {
    }
    return success;
}
bool test_SIUnit_json_typed_roundtrip_basic(void) {
    // Test with a basic unit
    double unit_multiplier = 1.0;
    OCStringRef unit_error = NULL;
    SIUnitRef original = SIUnitFromExpression(STR("kg"), &unit_multiplier, &unit_error);
    if (!original) {
        printf("  Failed to create original SIUnit: %s\n", unit_error ? OCStringGetCString(unit_error) : "unknown error");
        if (unit_error) OCRelease(unit_error);
        return false;
    }
    if (unit_error) OCRelease(unit_error);
    // Serialize to JSONTyped
    cJSON *json = OCTypeCopyJSON((OCTypeRef)original, true, NULL);
    if (!json) {
        printf("  Failed to serialize SIUnit to JSONTyped\n");
        return false;
    }
    // Deserialize back from JSONTyped
    SIUnitRef restored = (SIUnitRef)OCTypeCreateFromJSONTyped(json, NULL);
    if (!restored) {
        printf("  Failed to deserialize SIUnit from JSONTyped\n");
        cJSON_Delete(json);
        return false;
    }
    // Compare units by symbol
    OCStringRef originalSymbol = SIUnitCopySymbol(original);
    OCStringRef restoredSymbol = SIUnitCopySymbol(restored);
    bool symbolsMatch = OCStringEqual(originalSymbol, restoredSymbol);
    if (!symbolsMatch) {
        printf("  Symbol mismatch: original='%s', restored='%s'\n",
               OCStringGetCString(originalSymbol), OCStringGetCString(restoredSymbol));
    }
    // Cleanup
    cJSON_Delete(json);
    OCRelease(originalSymbol);
    OCRelease(restoredSymbol);
    // Note: SIUnit objects from SIUnitFromExpression are typically static/cached, so we don't OCRelease them
    if (symbolsMatch) {
    }
    return symbolsMatch;
}
bool test_SIUnit_json_typed_roundtrip_compound(void) {
    // Test with a compound unit
    double unit_multiplier = 1.0;
    OCStringRef unit_error = NULL;
    SIUnitRef original = SIUnitFromExpression(STR("N*m/s^2"), &unit_multiplier, &unit_error);
    if (!original) {
        printf("  Failed to create original compound SIUnit: %s\n", unit_error ? OCStringGetCString(unit_error) : "unknown error");
        if (unit_error) OCRelease(unit_error);
        return false;
    }
    if (unit_error) OCRelease(unit_error);
    // Serialize to JSONTyped
    cJSON *json = OCTypeCopyJSON((OCTypeRef)original, true, NULL);
    if (!json) {
        printf("  Failed to serialize compound SIUnit to JSONTyped\n");
        return false;
    }
    // Deserialize back from JSONTyped
    SIUnitRef restored = (SIUnitRef)OCTypeCreateFromJSONTyped(json, NULL);
    if (!restored) {
        printf("  Failed to deserialize compound SIUnit from JSONTyped\n");
        cJSON_Delete(json);
        return false;
    }
    // Compare units by creating canonical expressions
    OCStringRef originalSymbol = SIUnitCopySymbol(original);
    OCStringRef restoredSymbol = SIUnitCopySymbol(restored);
    bool exprsMatch = OCStringEqual(originalSymbol, restoredSymbol);
    if (!exprsMatch) {
        printf("  Symbol mismatch: original='%s', restored='%s'\n",
               OCStringGetCString(originalSymbol), OCStringGetCString(restoredSymbol));
    }
    // Cleanup
    cJSON_Delete(json);
    OCRelease(originalSymbol);
    OCRelease(restoredSymbol);
    if (exprsMatch) {
    }
    return exprsMatch;
}
bool test_SIDimensionality_json_typed_roundtrip(void) {
    // Test with a dimensionality
    OCStringRef dim_error = NULL;
    SIDimensionalityRef original = SIDimensionalityFromExpression(STR("L/T^2"), &dim_error);
    if (!original) {
        printf("  Failed to create original SIDimensionality: %s\n", dim_error ? OCStringGetCString(dim_error) : "unknown error");
        if (dim_error) OCRelease(dim_error);
        return false;
    }
    if (dim_error) OCRelease(dim_error);
    // Serialize to JSONTyped
    cJSON *json = OCTypeCopyJSON((OCTypeRef)original, true, NULL);
    if (!json) {
        printf("  Failed to serialize SIDimensionality to JSONTyped\n");
        OCRelease(original);
        return false;
    }
    // Deserialize back from JSONTyped
    SIDimensionalityRef restored = (SIDimensionalityRef)OCTypeCreateFromJSONTyped(json, NULL);
    if (!restored) {
        printf("  Failed to deserialize SIDimensionality from JSONTyped\n");
        cJSON_Delete(json);
        OCRelease(original);
        return false;
    }
    // Compare dimensionalities by symbol
    OCStringRef originalSymbol = SIDimensionalityCopySymbol(original);
    OCStringRef restoredSymbol = SIDimensionalityCopySymbol(restored);
    bool symbolsMatch = OCStringEqual(originalSymbol, restoredSymbol);
    if (!symbolsMatch) {
        printf("  Symbol mismatch: original='%s', restored='%s'\n",
               OCStringGetCString(originalSymbol), OCStringGetCString(restoredSymbol));
    }
    // Cleanup
    cJSON_Delete(json);
    OCRelease(original);
    OCRelease(restored);
    OCRelease(originalSymbol);
    OCRelease(restoredSymbol);
    if (symbolsMatch) {
    }
    return symbolsMatch;
}
bool test_json_typed_error_handling(void) {
    // Test with invalid typed JSON (object with unknown type)
    cJSON *invalidJson = cJSON_CreateObject();
    cJSON_AddStringToObject(invalidJson, "type", "UnknownType");
    cJSON_AddStringToObject(invalidJson, "value", "some value");
    OCTypeRef result = OCTypeCreateFromJSONTyped(invalidJson, NULL);
    if (result != NULL) {
        printf("  Expected NULL for invalid JSON, but got a result\n");
        cJSON_Delete(invalidJson);
        OCRelease(result);
        return false;
    }
    cJSON_Delete(invalidJson);
    // Test with NULL JSON
    result = OCTypeCreateFromJSONTyped(NULL, NULL);
    if (result != NULL) {
        printf("  Expected NULL for NULL JSON, but got a result\n");
        OCRelease(result);
        return false;
    }
    // Test with malformed typed JSON (missing required fields)
    cJSON *malformedJson = cJSON_CreateObject();
    cJSON_AddStringToObject(malformedJson, "type", "SIScalar");
    // Missing subtype and value fields
    result = OCTypeCreateFromJSONTyped(malformedJson, NULL);
    if (result != NULL) {
        printf("  Expected NULL for malformed JSON, but got a result\n");
        cJSON_Delete(malformedJson);
        OCRelease(result);
        return false;
    }
    cJSON_Delete(malformedJson);
    return true;
}
bool test_json_typed_comprehensive_coverage(void) {
    // Test multiple scalar types in sequence
    struct {
        const char *expression;
        const char *description;
    } testCases[] = {
        {"42", "dimensionless scalar"},
        {"3.14159 rad", "angle in radians"},
        {"-273.15 K", "negative temperature"},
        {"1.23e-6 F", "scientific notation"},
        {"(3+4*I) A", "complex current"},
        {"0 m/s", "zero velocity"},
        {"inf Hz", "infinite frequency"}};
    int numCases = sizeof(testCases) / sizeof(testCases[0]);
    int successCount = 0;
    for (int i = 0; i < numCases; i++) {
        OCStringRef expr = OCStringCreateWithCString(testCases[i].expression);
        OCStringRef error = NULL;
        SIScalarRef original = SIScalarCreateFromExpression(expr, &error);
        OCRelease(expr);
        if (!original) {
            printf("  Failed to create scalar for case %d (%s): %s\n",
                   i, testCases[i].description,
                   error ? OCStringGetCString(error) : "unknown error");
            if (error) OCRelease(error);
            continue;
        }
        if (error) OCRelease(error);
        // Test roundtrip
        cJSON *json = OCTypeCopyJSON((OCTypeRef)original, true, NULL);
        if (!json) {
            printf("  Failed to serialize case %d (%s)\n", i, testCases[i].description);
            OCRelease(original);
            continue;
        }
        SIScalarRef restored = (SIScalarRef)OCTypeCreateFromJSONTyped(json, NULL);
        if (!restored) {
            printf("  Failed to deserialize case %d (%s)\n", i, testCases[i].description);
            cJSON_Delete(json);
            OCRelease(original);
            continue;
        }
        // Basic comparison - at minimum they should both be valid scalars
        if (SIScalarGetTypeID() == OCGetTypeID(restored)) {
            successCount++;
        } else {
            printf("  Case %d (%s): Type mismatch after roundtrip\n", i, testCases[i].description);
        }
        cJSON_Delete(json);
        OCRelease(original);
        OCRelease(restored);
    }
    bool success = (successCount == numCases);
    if (success) {
    } else {
        printf("test_json_typed_comprehensive_coverage failed (%d/%d cases)\n", successCount, numCases);
    }
    return success;
}
