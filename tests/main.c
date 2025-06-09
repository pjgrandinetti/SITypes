#include <sanitizer/lsan_interface.h>
#include <stdio.h>
#include "../src/SILibrary.h"
#include "test_dimensionality.h"
#include "test_octypes.h"
#include "test_scalar.h"
#include "test_scalar_parser.h"
#include "test_unit.h"
#ifdef LEAK_SANITIZER
#include <sanitizer/lsan_interface.h>
extern void __lsan_do_leak_check() __attribute__((weak));
#endif
int failed_tests = 0;
#define TRACK_SCALAR_LEAK(test_fn)                                                  \
    do {                                                                            \
        size_t before = OCLeakCountForType(SIScalarGetTypeID());                    \
        if (!test_fn()) {                                                           \
            printf("[FAIL] %s\n", #test_fn);                                        \
            failed_tests++;                                                         \
        }                                                                           \
        size_t after = OCLeakCountForType(SIScalarGetTypeID());                     \
        if (after > before) {                                                       \
            printf("[LEAK] %s leaked %zu SIScalar(s)\n", #test_fn, after - before); \
        }                                                                           \
    } while (0)
#define TRACK(test_fn)                       \
    do {                                     \
        if (!test_fn()) {                    \
            printf("[FAIL] %s\n", #test_fn); \
            failed_tests++;                  \
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
    printf("\n=== SIScalar Parser Tests ===\n");
    TRACK_SCALAR_LEAK(test_scalar_parser_1);
    TRACK_SCALAR_LEAK(test_scalar_parser_2);
    TRACK_SCALAR_LEAK(test_scalar_parser_3);
    TRACK_SCALAR_LEAK(test_scalar_parser_4);
    TRACK_SCALAR_LEAK(test_scalar_parser_5);
    TRACK_SCALAR_LEAK(test_scalar_parser_6);
    TRACK_SCALAR_LEAK(test_scalar_parser_7);
    TRACK_SCALAR_LEAK(test_scalar_parser_8);
    TRACK_SCALAR_LEAK(test_scalar_parser_9);
    TRACK_SCALAR_LEAK(test_scalar_parser_10);
    printf("\n=== SIScalar Tests ===\n");
    TRACK_SCALAR_LEAK(test_SIScalarGetTypeID);
    TRACK_SCALAR_LEAK(test_SIScalarCreateCopy);
    TRACK_SCALAR_LEAK(test_SIScalarCreateMutableCopy);
    TRACK_SCALAR_LEAK(test_SIScalarCreateWithFloat);
    TRACK_SCALAR_LEAK(test_SIScalarCreateMutableWithFloat);
    TRACK_SCALAR_LEAK(test_SIScalarCreateWithDouble);
    TRACK_SCALAR_LEAK(test_SIScalarCreateMutableWithDouble);
    TRACK_SCALAR_LEAK(test_SIScalarCreateWithFloatComplex);
    TRACK_SCALAR_LEAK(test_SIScalarCreateMutableWithFloatComplex);
    TRACK_SCALAR_LEAK(test_SIScalarCreateWithDoubleComplex);
    TRACK_SCALAR_LEAK(test_SIScalarCreateMutableWithDoubleComplex);
    TRACK_SCALAR_LEAK(test_SIScalarGetValue);
    TRACK_SCALAR_LEAK(test_SIScalarSetFloatValue);
    TRACK_SCALAR_LEAK(test_SIScalarSetDoubleValue);
    TRACK_SCALAR_LEAK(test_SIScalarSetFloatComplexValue);
    TRACK_SCALAR_LEAK(test_SIScalarSetDoubleComplexValue);
    TRACK_SCALAR_LEAK(test_SIScalarSetElementType);
    TRACK_SCALAR_LEAK(test_SIScalarFloatValue);
    TRACK_SCALAR_LEAK(test_SIScalarDoubleValue);
    TRACK_SCALAR_LEAK(test_SIScalarFloatComplexValue);
    TRACK_SCALAR_LEAK(test_SIScalarDoubleComplexValue);
    TRACK_SCALAR_LEAK(test_SIScalarCreateByConvertingToNumberType);
    TRACK_SCALAR_LEAK(test_SIScalarTakeComplexPart);
    TRACK_SCALAR_LEAK(test_SIScalarCreateByTakingComplexPart);
    TRACK_SCALAR_LEAK(test_SIScalarCreateByReducingUnit);
    TRACK_SCALAR_LEAK(test_SIScalarReduceUnit);
    TRACK_SCALAR_LEAK(test_SIScalarConvertToUnit);
    TRACK_SCALAR_LEAK(test_SIScalarCreateByConvertingToUnit);
    TRACK_SCALAR_LEAK(test_SIScalarConvertToCoherentUnit);
    TRACK_SCALAR_LEAK(test_SIScalarCreateByConvertingToCoherentUnit);
    TRACK_SCALAR_LEAK(test_SIScalarCreateByAdding);
    TRACK_SCALAR_LEAK(test_SIScalarAdd);
    TRACK_SCALAR_LEAK(test_SIScalarCreateBySubtracting);
    TRACK_SCALAR_LEAK(test_SIScalarSubtract);
    TRACK_SCALAR_LEAK(test_SIScalarCreateByMultiplyingWithoutReducingUnit);
    TRACK_SCALAR_LEAK(test_SIScalarMultiplyWithoutReducingUnit);
    TRACK_SCALAR_LEAK(test_SIScalarCreateByMultiplying);
    TRACK_SCALAR_LEAK(test_SIScalarMultiply);
    TRACK_SCALAR_LEAK(test_SIScalarCreateByDividingWithoutReducingUnit);
    TRACK_SCALAR_LEAK(test_SIScalarDivideWithoutReducingUnit);
    TRACK_SCALAR_LEAK(test_SIScalarCreateByDividing);
    TRACK_SCALAR_LEAK(test_SIScalarDivide);
    TRACK_SCALAR_LEAK(test_SIScalarCreateByRaisingToPowerWithoutReducingUnit);
    TRACK_SCALAR_LEAK(test_SIScalarRaiseToAPowerWithoutReducingUnit);
    TRACK_SCALAR_LEAK(test_SIScalarCreateByRaisingToPower);
    TRACK_SCALAR_LEAK(test_SIScalarRaiseToAPower);
    TRACK_SCALAR_LEAK(test_SIScalarCreateByTakingAbsoluteValue);
    TRACK_SCALAR_LEAK(test_SIScalarTakeAbsoluteValue);
    TRACK_SCALAR_LEAK(test_SIScalarCreateByGammaFunctionWithoutReducingUnit);
    TRACK_SCALAR_LEAK(test_SIScalarCreateByTakingNthRoot);
    TRACK_SCALAR_LEAK(test_SIScalarTakeNthRoot);
    TRACK_SCALAR_LEAK(test_SIScalarTakeLog10);
    TRACK_SCALAR_LEAK(test_SIScalarCreateByZeroingPart);
    TRACK_SCALAR_LEAK(test_SIScalarZeroPart);
    TRACK_SCALAR_LEAK(test_SIScalarMultiplyByDimensionlessRealConstant);
    TRACK_SCALAR_LEAK(test_SIScalarCreateByMultiplyingByDimensionlessRealConstant);
    TRACK_SCALAR_LEAK(test_SIScalarCreateByMultiplyingByDimensionlessComplexConstant);
    TRACK_SCALAR_LEAK(test_SIScalarMultiplyByDimensionlessComplexConstant);
    TRACK_SCALAR_LEAK(test_SIScalarCreateByConjugation);
    TRACK_SCALAR_LEAK(test_SIScalarConjugate);
    TRACK_SCALAR_LEAK(test_SIScalarShow);
    TRACK_SCALAR_LEAK(test_SIScalarCreateStringValue);
    TRACK_SCALAR_LEAK(test_SIScalarCreateNumericStringValue);
    TRACK_SCALAR_LEAK(test_SIScalarCreateStringValueForPart);
    TRACK_SCALAR_LEAK(test_SIScalarCreateUnitString);
    TRACK_SCALAR_LEAK(test_SIScalarCreateStringValueWithFormat);
    TRACK_SCALAR_LEAK(test_SIScalarCreateNumericStringValueWithFormat);
    TRACK_SCALAR_LEAK(test_SIScalarAddToArrayAsStringValue);
    TRACK_SCALAR_LEAK(test_SIScalarIsReal);
    TRACK_SCALAR_LEAK(test_SIScalarIsImaginary);
    TRACK_SCALAR_LEAK(test_SIScalarIsComplex);
    TRACK_SCALAR_LEAK(test_SIScalarIsZero);
    TRACK_SCALAR_LEAK(test_SIScalarIsInfinite);
    TRACK_SCALAR_LEAK(test_SIScalarIsRealNonNegativeInteger);
    TRACK_SCALAR_LEAK(test_SIScalarValidateProposedStringValue);
    TRACK_SCALAR_LEAK(test_SIScalarEqual);
    TRACK_SCALAR_LEAK(test_SIScalarCompare);
    TRACK_SCALAR_LEAK(test_SIScalarCompareReduced);
    TRACK_SCALAR_LEAK(test_SIScalarCompareLoose);
    TRACK_SCALAR_LEAK(test_SIScalarBestConversionForQuantity);
    TRACK_SCALAR_LEAK(test_SIScalarBestConversionForQuantity_large);
    TRACK_SCALAR_LEAK(test_SIScalarBestConversionForQuantity_tiny);
    TRACK_SCALAR_LEAK(test_SIScalarBestConversionForQuantity_noop);
    TRACK_SCALAR_LEAK(test_SIScalarBestConversionForQuantity_zero);
    TRACK_SCALAR_LEAK(test_SIScalarBestConversionForQuantity_negative);
    printf("\n%d test(s) failed\n", failed_tests);
#ifdef LEAK_SANITIZER
    if (&__lsan_do_leak_check) {
        __lsan_do_leak_check();
    }
#endif
    return failed_tests > 0 ? 1 : 0;
}
