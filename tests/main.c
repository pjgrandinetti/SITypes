#ifdef LEAK_SANITIZER
#include <sanitizer/lsan_interface.h>
#endif
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SITypes.h"
#include "test_dimensionality.h"
#include "test_duplicate_units.h"
#include "test_octypes.h"
#include "test_scalar.h"
#include "test_scalar_parser.h"
#include "test_unit.h"
#include "test_unit_expression.h"
#include "test_unit_from_expression_robust.h"
#include "test_unit_power_operations.h"
#ifdef LEAK_SANITIZER
extern void __lsan_do_leak_check() __attribute__((weak));
#endif
int failures = 0;
#define TRACK(test_fn)                       \
    do {                                     \
        if (!test_fn()) {                    \
            printf("[FAIL] %s\n", #test_fn); \
            failures++;                      \
        }                                    \
    } while (0)
int main(int argc, const char* argv[]) {
    printf("=== OCTypes Tests ===\n");
    TRACK(octypesTest1);
    TRACK(octypesTest2);
    TRACK(octypesTest3);
    TRACK(octypesTest4);
    TRACK(octypesTest5);
    TRACK(octypesTest6);
    printf("\n=== Dimensionality Tests ===\n");
    TRACK(test_dimensionality_0);
    TRACK(test_dimensionality_1);
    TRACK(test_dimensionality_2);
    TRACK(test_dimensionality_3);
    TRACK(test_dimensionality_show);
    TRACK(test_dimensionality_symbol_acceleration);
    TRACK(test_dimensionality_divide_mass);
    TRACK(test_dimensionality_multiply_work);
    TRACK(test_dimensionality_power_area);
    TRACK(test_dimensionality_reduction_behavior);
    TRACK(test_dimensionality_deep_copy);
    TRACK(test_dimensionality_parser_strictness);
    printf("\n=== SIUnit Tests ===\n");
    TRACK(test_unit_0);
    TRACK(test_unit_1);
    TRACK(test_unit_3);
    TRACK(test_unit_4);
    TRACK(test_unit_5);
    TRACK(test_unit_6);
    TRACK(test_unit_7);
    TRACK(test_unit_8);
    TRACK(test_unit_9);
    TRACK(test_unit_10);
    TRACK(test_unit_11);
    TRACK(test_unit_12);
    TRACK(test_unit_13);
    TRACK(test_unit_14);
    TRACK(test_unit_by_multiplying_without_reducing);
    TRACK(test_unit_by_dividing_without_reducing);
    TRACK(test_unit_by_taking_nth_root);
    TRACK(test_unit_by_raising_to_power_without_reducing);
    TRACK(test_unit_unicode_normalization);
    TRACK(test_unit_registration);
    TRACK(test_unit_canonical_expressions);
    TRACK(test_unit_from_expression_equivalence);
    TRACK(test_unit_count_token_symbols);
    printf("\n=== SIUnitCreateCleanedExpression Comprehensive Tests ===\n");
    TRACK(test_unit_expression_cleaner_comprehensive);
    printf("\n=== Duplicate Units Tests ===\n");
    TRACK(test_create_potential_duplicates);
    TRACK(test_check_for_duplicate_units);
    printf("\n=== Unit Power Operations Tests ===\n");
    TRACK(test_unit_negative_power_operations);
    TRACK(test_unit_inverse_expression_parsing);
    TRACK(test_unit_negative_exponent_parsing);
    printf("\n=== SIUnitFromExpression Robust Tests ===\n");
    TRACK(test_unit_from_expression_robust);
    printf("\n=== SIScalar Parser Tests ===\n");
    TRACK(test_scalar_parser_1);
    TRACK(test_scalar_parser_2);
    TRACK(test_scalar_parser_3);
    TRACK(test_scalar_parser_4);
    TRACK(test_scalar_parser_5);
    TRACK(test_scalar_parser_6);
    TRACK(test_scalar_parser_7);
    TRACK(test_scalar_parser_8);
    TRACK(test_scalar_parser_9);
    TRACK(test_scalar_parser_10);
    TRACK(test_scalar_parser_11);
    TRACK(test_scalar_parser_12);
    printf("\n=== NMR Function Tests ===\n");
    TRACK(test_nmr_functions);
    printf("\n=== SIScalar Tests ===\n");
    TRACK(test_SIScalarGetTypeID);
    TRACK(test_SIScalarCreateCopy);
    TRACK(test_SIScalarCreateMutableCopy);
    TRACK(test_SIScalarCreateWithFloat);
    TRACK(test_SIScalarCreateMutableWithFloat);
    TRACK(test_SIScalarCreateWithDouble);
    TRACK(test_SIScalarCreateMutableWithDouble);
    TRACK(test_SIScalarCreateWithFloatComplex);
    TRACK(test_SIScalarCreateMutableWithFloatComplex);
    TRACK(test_SIScalarCreateWithDoubleComplex);
    TRACK(test_SIScalarCreateMutableWithDoubleComplex);
    TRACK(test_SIScalarGetValue);
    TRACK(test_SIScalarSetFloatValue);
    TRACK(test_SIScalarSetDoubleValue);
    TRACK(test_SIScalarSetFloatComplexValue);
    TRACK(test_SIScalarSetDoubleComplexValue);
    TRACK(test_SIScalarSetNumericType);
    TRACK(test_SIScalarFloatValue);
    TRACK(test_SIScalarDoubleValue);
    TRACK(test_SIScalarFloatComplexValue);
    TRACK(test_SIScalarDoubleComplexValue);
    TRACK(test_SIScalarCreateByConvertingToNumberType);
    TRACK(test_SIScalarTakeComplexPart);
    TRACK(test_SIScalarCreateByTakingComplexPart);
    TRACK(test_SIScalarCreateByReducingUnit);
    TRACK(test_SIScalarReduceUnit);
    TRACK(test_SIScalarConvertToUnit);
    TRACK(test_SIScalarCreateByConvertingToUnit);
    TRACK(test_SIScalarConvertToCoherentUnit);
    TRACK(test_SIScalarCreateByConvertingToCoherentUnit);
    TRACK(test_SIScalarCreateByAdding);
    TRACK(test_SIScalarAdd);
    TRACK(test_SIScalarCreateBySubtracting);
    TRACK(test_SIScalarSubtract);
    TRACK(test_SIScalarCreateByMultiplyingWithoutReducingUnit);
    TRACK(test_SIScalarMultiplyWithoutReducingUnit);
    TRACK(test_SIScalarCreateByMultiplying);
    TRACK(test_SIScalarMultiply);
    TRACK(test_SIScalarCreateByDividingWithoutReducingUnit);
    TRACK(test_SIScalarDivideWithoutReducingUnit);
    TRACK(test_SIScalarCreateByDividing);
    TRACK(test_SIScalarDivide);
    TRACK(test_SIScalarCreateByRaisingToPowerWithoutReducingUnit);
    TRACK(test_SIScalarRaiseToAPowerWithoutReducingUnit);
    TRACK(test_SIScalarCreateByRaisingToPower);
    TRACK(test_SIScalarRaiseToAPower);
    TRACK(test_SIScalarCreateByTakingAbsoluteValue);
    TRACK(test_SIScalarTakeAbsoluteValue);
    TRACK(test_SIScalarCreateByGammaFunctionWithoutReducingUnit);
    TRACK(test_SIScalarCreateByTakingNthRoot);
    TRACK(test_SIScalarTakeNthRoot);
    TRACK(test_SIScalarTakeLog10);
    TRACK(test_SIScalarCreateByZeroingPart);
    TRACK(test_SIScalarZeroPart);
    TRACK(test_SIScalarMultiplyByDimensionlessRealConstant);
    TRACK(test_SIScalarCreateByMultiplyingByDimensionlessRealConstant);
    TRACK(test_SIScalarCreateByMultiplyingByDimensionlessComplexConstant);
    TRACK(test_SIScalarMultiplyByDimensionlessComplexConstant);
    TRACK(test_SIScalarCreateByConjugation);
    TRACK(test_SIScalarConjugate);
    TRACK(test_SIScalarShow);
    TRACK(test_SIScalarCreateStringValue);
    TRACK(test_SIScalarCreateNumericStringValue);
    TRACK(test_SIScalarCreateStringValueForPart);
    TRACK(test_SIScalarCopyUnitSymbol);
    TRACK(test_SIScalarCreateStringValueWithFormat);
    TRACK(test_SIScalarCreateNumericStringValueWithFormat);
    TRACK(test_SIScalarAddToArrayAsStringValue);
    TRACK(test_SIScalarIsReal);
    TRACK(test_SIScalarIsImaginary);
    TRACK(test_SIScalarIsComplex);
    TRACK(test_SIScalarIsZero);
    TRACK(test_SIScalarIsInfinite);
    TRACK(test_SIScalarIsRealNonNegativeInteger);
    TRACK(test_SIScalarValidateProposedStringValue);
    TRACK(test_SIScalarEqual);
    TRACK(test_SIScalarCompare);
    TRACK(test_SIScalarCompareReduced);
    TRACK(test_SIScalarCompareLoose);
    TRACK(test_SIScalarBestConversionForQuantity);
    TRACK(test_SIScalarBestConversionForQuantity_large);
    TRACK(test_SIScalarBestConversionForQuantity_tiny);
    TRACK(test_SIScalarBestConversionForQuantity_noop);
    TRACK(test_SIScalarBestConversionForQuantity_zero);
    TRACK(test_SIScalarBestConversionForQuantity_negative);
    TRACK(test_SIScalarWriteReadJSON_simple);
    TRACK(test_SIScalarWriteReadJSON_negative);
    TRACK(test_SIScalarWriteReadJSON_complex_unit);
    TRACK(test_SIScalarWriteReadJSON_array_and_dictionary);
    if (failures) {
        fprintf(stderr, "\n%d test(s) failed.\n", failures);
    } else
        printf("\n%d test(s) failed\n", failures);
#ifdef LEAK_SANITIZER
    if (&__lsan_do_leak_check) {
        __lsan_do_leak_check();
    }
#endif
    SITypesShutdown();
    return failures > 0 ? 1 : 0;
}
