//
//  main.c
//  SILibTests
//
//  Created by Philip Grandinetti on 6/13/17.
//  Copyright © 2017 PhySy Ltd. All rights reserved.
//

// Using <assert.h> in tests; main just invokes test functions

#include <stdio.h>
#include "../src/SILibrary.h" // Updated include path to resolve missing header issue
#include "test_octypes.h"
#include "test_dimensionality.h"
#include "test_unit.h"
#include "test_scalar.h"
#include "test_scalar_parser.h"
#include <sanitizer/lsan_interface.h>
extern void __lsan_do_leak_check() __attribute__((weak));


int main(int argc, const char * argv[]) {


    printf("=== OCTypes Tests ===\n");
    octypesTest1();
    octypesTest2();
    octypesTest3();
    octypesTest4();
    octypesTest5();
    octypesTest6();


    printf("\n=== Dimensionality Tests ===\n");
    test_dimensionality_0();
    test_dimensionality_1();
    test_dimensionality_2();
    test_dimensionality_3();
    test_dimensionality_show();
    test_dimensionality_symbol_acceleration();
    test_dimensionality_divide_mass();
    test_dimensionality_multiply_work();
    test_dimensionality_power_area();
    test_dimensionality_reduction_behavior();

    printf("\n=== SIUnit Tests ===\n");
    test_unit_0();
    test_unit_1();
    test_unit_3();
    test_unit_4();
    test_unit_5();
    test_unit_6();
    test_unit_7();
    test_unit_8();
    test_unit_9();
    test_unit_10();
    test_unit_11();
    test_unit_12();
    test_unit_13();

    printf("\n=== SIScalar Parser Tests ===\n");
    test_scalar_parser_1();
    test_scalar_parser_2();
    test_scalar_parser_3();
    test_scalar_parser_4();
    test_scalar_parser_5();
    test_scalar_parser_6();
    test_scalar_parser_7();
    test_scalar_parser_8();
    test_scalar_parser_9();
    test_scalar_parser_10();

    printf("\n=== SIScalar Tests ===\n");
    test_SIScalarGetTypeID();
    test_SIScalarCreateCopy();
    test_SIScalarCreateMutableCopy();
    test_SIScalarCreateWithFloat();
    test_SIScalarCreateMutableWithFloat();
    test_SIScalarCreateWithDouble();
    test_SIScalarCreateMutableWithDouble();
    test_SIScalarCreateWithFloatComplex();
    test_SIScalarCreateMutableWithFloatComplex();
    test_SIScalarCreateWithDoubleComplex();
    test_SIScalarCreateMutableWithDoubleComplex();
    test_SIScalarCreateWithStringContainingSingleUnitFromLibrary();
    test_SIScalarGetValue();
    test_SIScalarSetFloatValue();
    test_SIScalarSetDoubleValue();
    test_SIScalarSetFloatComplexValue();
    test_SIScalarSetDoubleComplexValue();
    test_SIScalarSetElementType();
    test_SIScalarFloatValue();
    test_SIScalarDoubleValue();
    test_SIScalarFloatComplexValue();
    test_SIScalarDoubleComplexValue();
    // test_SIScalarMagnitudeValue();
    // test_SIScalarArgumentValue();
    // test_SIScalarFloatValueInUnit();
    // test_SIScalarDoubleValueInUnit();
    // test_SIScalarFloatComplexValueInUnit();
    // test_SIScalarDoubleComplexValueInUnit();
    // test_SIScalarFloatValueInCoherentUnit();
    // test_SIScalarDoubleValueInCoherentUnit();
    // test_SIScalarFloatComplexValueInCoherentUnit();
    // test_SIScalarDoubleComplexValueInCoherentUnit();
    test_SIScalarCreateByConvertingToNumberType();
    test_SIScalarTakeComplexPart();
    test_SIScalarCreateByTakingComplexPart();
    test_SIScalarCreateByReducingUnit();
    test_SIScalarReduceUnit();
    test_SIScalarConvertToUnit();
    test_SIScalarCreateByConvertingToUnit();
    test_SIScalarConvertToCoherentUnit();
    test_SIScalarCreateByConvertingToCoherentUnit();
    test_SIScalarCreateByAdding();
    test_SIScalarAdd();
    test_SIScalarCreateBySubtracting();
    test_SIScalarSubtract();
    test_SIScalarCreateByMultiplyingWithoutReducingUnit();
    test_SIScalarMultiplyWithoutReducingUnit();
    test_SIScalarCreateByMultiplying();
    test_SIScalarMultiply();
    test_SIScalarCreateByDividingWithoutReducingUnit();
    test_SIScalarDivideWithoutReducingUnit();
    test_SIScalarCreateByDividing();
    test_SIScalarDivide();
    test_SIScalarCreateByRaisingToAPowerWithoutReducingUnit();
    test_SIScalarRaiseToAPowerWithoutReducingUnit();
    test_SIScalarCreateByRaisingToAPower();
    test_SIScalarRaiseToAPower();
    test_SIScalarCreateByTakingAbsoluteValue();
    test_SIScalarTakeAbsoluteValue();
    test_SIScalarCreateByGammaFunctionWithoutReducingUnit();
    test_SIScalarCreateByTakingNthRoot();
    test_SIScalarTakeNthRoot();
    test_SIScalarTakeLog10();
    test_SIScalarCreateByZeroingPart();
    test_SIScalarZeroPart();
    test_SIScalarMultiplyByDimensionlessRealConstant();
    test_SIScalarCreateByMultiplyingByDimensionlessRealConstant();
    test_SIScalarCreateByMultiplyingByDimensionlessComplexConstant();
    test_SIScalarMultiplyByDimensionlessComplexConstant();
    test_SIScalarCreateByConjugation();
    test_SIScalarConjugate();
    test_SIScalarShow();
    test_SIScalarCreateStringValue();
    test_SIScalarCreateNumericStringValue();
    test_SIScalarCreateStringValueForPart();
    test_SIScalarCreateUnitString();
    test_SIScalarCreateStringValueWithFormat();
    test_SIScalarCreateNumericStringValueWithFormat();
    test_SIScalarAddToArrayAsStringValue();
    test_SIScalarIsReal();
    test_SIScalarIsImaginary();
    test_SIScalarIsComplex();
    test_SIScalarIsZero();
    test_SIScalarIsInfinite();
    test_SIScalarIsRealNonNegativeInteger();
    test_SIScalarValidateProposedStringValue();
    test_SIScalarEqual();
    test_SIScalarCompare();
    test_SIScalarCompareReduced();
    test_SIScalarCompareLoose();


    // Print summary message if all tests pass
    printf("\nAll tests passed\n");

    // If LSAN is present, do an explicit leak check here:
    if (&__lsan_do_leak_check) {
        __lsan_do_leak_check();
    }

   return 0;

}
