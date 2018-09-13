//
//  PSScalar.h
//  SITypes
//
//  Created by philip on 6/14/17.
//  Copyright © 2017 PhySy Ltd. All rights reserved.
//

#ifndef PSScalar_h
#define PSScalar_h

#include "SILibrary.h"

/*!
 @header PSScalar
 @copyright PhySy Ltd
 PSScalar represents a scalar physical quantity. It is a concrete subtype of PSQuantity.
 It has three essential attributes: a unit, an elementType, and a numerical value.
 PSScalar supports four elementTypes, float, double, float complex, and double complex.
 
 For additional details visit <a href="http://www.PhySy.org">http://www.PhySy.org</a>
 
 @unsorted
 */

typedef union __PSNumber
{
    float   floatValue;
    double  doubleValue;
    float complex floatComplexValue;
    double complex doubleComplexValue;
} __PSNumber;



/*!
 @typedef PSScalarRef
 This is the type of a reference to immutable PSScalar.
 */
typedef const struct __PSScalar *PSScalarRef;

/*!
 @typedef PSMutableScalarRef
 This is the type of a reference to mutable PSScalar.
 */
typedef struct __PSScalar *PSMutableScalarRef;

OCTypeID PSScalarGetTypeID(void);

#pragma mark Creators
/*!
 @functiongroup Creators
 */

/*
 @function PSScalarCreateCopy
 @abstract Creates a copy of a scalar
 @param theScalar The scalar.
 @result a copy of the scalar.
 */
PSScalarRef PSScalarCreateCopy(PSScalarRef theScalar);

/*
 @function PSScalarCreateMutableCopy
 @abstract Creates a mutable copy of a scalar
 @param theScalar The scalar.
 @result a mutable copy of the scalar.
 */
PSMutableScalarRef PSScalarCreateMutableCopy(PSScalarRef theScalar);

/*!
 @function PSScalarCreateWithFloat
 @abstract Creates a scalar from float value and unit.
 @param input_value The float value.
 @param unit The unit.
 @result a PSScalar object
 */
PSScalarRef PSScalarCreateWithFloat(float input_value, SIUnitRef unit);

/*!
 @function PSScalarCreateMutableWithFloat
 @abstract Creates a mutable scalar from float value and unit.
 @param input_value The float value.
 @param unit The unit.
 @result a PSMutableScalar object
 */
PSMutableScalarRef PSScalarCreateMutableWithFloat(float input_value, SIUnitRef unit);

/*!
 @function PSScalarCreateWithDouble
 @abstract Creates a scalar from double value and unit.
 @param input_value The double value.
 @param unit The unit.
 @result a PSScalar object
 */
PSScalarRef PSScalarCreateWithDouble(double input_value, SIUnitRef unit);

/*!
 @function PSScalarCreateMutableWithDouble
 @abstract Creates a mutable scalar from double value and unit.
 @param input_value The double value.
 @param unit The unit.
 @result a PSMutableScalar object
 */
PSMutableScalarRef PSScalarCreateMutableWithDouble(double input_value, SIUnitRef unit);

/*!
 @function PSScalarCreateWithFloatComplex
 @abstract Creates a scalar from float complex value and unit.
 @param input_value The float complex value.
 @param unit The unit.
 @result a PSScalar object
 */
PSScalarRef PSScalarCreateWithFloatComplex(float complex input_value, SIUnitRef unit);

/*!
 @function PSScalarCreateMutableWithFloatComplex
 @abstract Creates a mutable scalar from float complex value and unit.
 @param input_value The float complex value.
 @param unit The unit.
 @result a PSMutableScalar object
 */
PSMutableScalarRef PSScalarCreateMutableWithFloatComplex(float complex input_value, SIUnitRef unit);

/*!
 @function PSScalarCreateWithDoubleComplex
 @abstract Creates a scalar from double complex value and unit.
 @param input_value The double complex value.
 @param unit The unit.
 @result a PSScalar object
 */
PSScalarRef PSScalarCreateWithDoubleComplex(double complex input_value, SIUnitRef unit);

/*!
 @function PSScalarCreateMutableWithDoubleComplex
 @abstract Creates a mutable scalar from double complex value and unit.
 @param input_value The double complex value.
 @param unit The unit.
 @result a PSMutableScalar object
 */
PSMutableScalarRef PSScalarCreateMutableWithDoubleComplex(double complex input_value, SIUnitRef unit);

#pragma mark Accessors
/*!
 @functiongroup Accessors
 */

__PSNumber PSScalarGetValue(PSScalarRef theScalar);

void PSScalarSetFloatValue(PSMutableScalarRef theScalar, float value);
void PSScalarSetDoubleValue(PSMutableScalarRef theScalar, double value);
void PSScalarSetFloatComplexValue(PSMutableScalarRef theScalar, float complex value);
void PSScalarSetDoubleComplexValue(PSMutableScalarRef theScalar, double complex value);

void PSScalarSetElementType(PSMutableScalarRef theScalar, numberType elementType);

/*!
 @function PSScalarFloatValue
 @abstract Returns a float value.
 @param theScalar The scalar.
 @result a float value
 */
float PSScalarFloatValue(PSScalarRef theScalar);

/*!
 @function PSScalarDoubleValue
 @abstract Returns a double value.
 @param theScalar The scalar.
 @result a double value
 */
double PSScalarDoubleValue(PSScalarRef theScalar);

/*!
 @function PSScalarFloatComplexValue
 @abstract Returns a float complex value.
 @param theScalar The scalar.
 @result a float complex value
 */
float complex PSScalarFloatComplexValue(PSScalarRef theScalar);

/*!
 @function PSScalarDoubleComplexValue
 @abstract Returns a double complex value.
 @param theScalar The scalar.
 @result a double complex value
 */
double complex PSScalarDoubleComplexValue(PSScalarRef theScalar);

/*!
 @function PSScalarMagnitudeValue
 @abstract Calculates and returns magnitude.
 @param theScalar The scalar.
 @result a double value
 */
double PSScalarMagnitudeValue(PSScalarRef theScalar);

/*!
 @function PSScalarArgumentValue
 @abstract Returns the argument value of scalar.
 @param theScalar The scalar.
 @result the argument
 */
double PSScalarArgumentValue(PSScalarRef theScalar);

/*!
 @function PSScalarFloatValueInUnit
 @abstract Calculates and returns float value after converting to unit.
 @param theScalar The scalar.
 @param unit The unit.
 @param success pointer to boolean that returns true if operation was successful.
 @result a float value
 */
float PSScalarFloatValueInUnit(PSScalarRef theScalar, SIUnitRef unit, bool *success);

/*!
 @function PSScalarDoubleValueInUnit
 @abstract Calculates and returns double value after converting to unit.
 @param theScalar The scalar.
 @param success pointer to boolean that returns true if operation was successful.
 @result a double value
 */
double PSScalarDoubleValueInUnit(PSScalarRef theScalar, SIUnitRef unit, bool *success);

/*!
 @function PSScalarFloatComplexValueInUnit
 @abstract Calculates and returns float complex value after converting to unit.
 @param theScalar The scalar.
 @param success pointer to boolean that returns true if operation was successful.
 @result a float complex value
 */
float complex PSScalarFloatComplexValueInUnit(PSScalarRef theScalar, SIUnitRef unit, bool *success);

/*!
 @function PSScalarDoubleComplexValueInUnit
 @abstract Calculates and returns double complex value after converting to unit.
 @param theScalar The scalar.
 @param success pointer to boolean that returns true if operation was successful.
 @result a double complex value
 */
double complex PSScalarDoubleComplexValueInUnit(PSScalarRef theScalar, SIUnitRef unit, bool *success);

/*!
 @function PSScalarFloatValueInCoherentUnit
 @abstract Calculates and returns float value after converting to its coherent SI unit.
 @param theScalar The scalar.
 @result a float value
 */
float PSScalarFloatValueInCoherentUnit(PSScalarRef theScalar);

/*!
 @function PSScalarDoubleValueInCoherentUnit
 @abstract Calculates and returns double value after converting to its coherent SI unit.
 @param theScalar The scalar.
 @result a double value
 */
double PSScalarDoubleValueInCoherentUnit(PSScalarRef theScalar);

/*!
 @function PSScalarFloatComplexValueInCoherentUnit
 @abstract Calculates and returns float complex value after converting to its coherent SI unit.
 @param theScalar The scalar.
 @result a float complex value
 */
float complex PSScalarFloatComplexValueInCoherentUnit(PSScalarRef theScalar);

/*!
 @function PSScalarDoubleComplexValueInCoherentUnit
 @abstract Calculates and returns double complex value after converting to its coherent SI unit.
 @param theScalar The scalar.
 @result a double complex value
 */
double complex PSScalarDoubleComplexValueInCoherentUnit(PSScalarRef theScalar);

#pragma mark Operations
/*!
 @functiongroup Operations
 */

/*
 @function PSScalarCreateByConvertingToNumberType
 @abstract Creates a copy of a scalar as elementType
 @param theScalar The scalar.
 @result a copy of the scalar.
 @discussion Note that down conversion from double to float loses precision
 and down conversion from complex to real loses the imaginary part
 of the quantity.
 */
PSScalarRef PSScalarCreateByConvertingToNumberType(PSScalarRef theScalar, numberType elementType);

/*!
 @function PSScalarTakeComplexPart
 @abstract Takes the complex number part of a scalar.
 @param theScalar The scalar.
 @param part The complex number part.
 @result true if successful, false otherwise
 @discussion complex number parts are kPSRealPart, kPSImaginaryPart, kPSMagnitudePart, and kPSArgumentPart
 */
bool PSScalarTakeComplexPart(PSMutableScalarRef theScalar, complexPart part);

/*!
 @function PSScalarCreateByTakingComplexPart
 @abstract Creates a scalar from complex number part.
 @param theScalar The scalar.
 @param part The complex number part.
 @result a PSScalar object
 @discussion complex number parts are kPSRealPart, kPSImaginaryPart, kPSMagnitudePart, and kPSArgumentPart
 */
PSScalarRef PSScalarCreateByTakingComplexPart(PSScalarRef theScalar, complexPart part);

PSScalarRef PSScalarCreateWithStringContainingSingleUnitFromLibrary(OCStringRef theString);

/*!
 @function PSScalarCreateWithOCString
 @abstract Creates a scalar by parsing string of number with unit.
 @param string The string.
 @param error an OCString describing the error;
 @result a PSScalar object, or NULL if string cannot be parsed.
 */
PSScalarRef PSScalarCreateWithOCString(OCStringRef string, OCStringRef *error);

/*!
 @function PSScalarCreateByReducingUnit
 @abstract Creates a scalar by reducing its unit to the numerator and denominator exponents to their lowest integer values.
 @param theScalar The input scalar.
 @result a PSScalar object.
 */
PSScalarRef PSScalarCreateByReducingUnit(PSScalarRef theScalar);

/*!
 @function PSScalarReduceUnit
 @abstract Reduces the unit's numerator and denominator exponents to their lowest integer values.
 @param theScalar The input scalar.
 @result a PSScalar object.
 */
bool PSScalarReduceUnit(PSMutableScalarRef theScalar);

/*!
 @function PSScalarConvertToUnit
 @abstract Converts a scalar to another unit of the same dimensionality.
 @param theScalar The input scalar.
 @param unit The new unit.
 @param error an OCString describing the error;
 @result true if successful, false otherwise.
 */
bool PSScalarConvertToUnit(PSMutableScalarRef theScalar, SIUnitRef unit, OCStringRef *error);

/*!
 @function PSScalarCreateByConvertingToUnit
 @abstract Creates a scalar by converting to another unit of the same dimensionality.
 @param theScalar The input scalar.
 @param unit The new unit.
 @param error an OCString describing the error;
 @result a PSScalar object, or NULL if conversion cannot be done.
 */
PSScalarRef PSScalarCreateByConvertingToUnit(PSScalarRef theScalar, SIUnitRef unit, OCStringRef *error);

/*!
 @function PSScalarConvertToCoherentUnit
 @abstract Converts a scalar to a coherent SI unit of the same dimensionality.
 @param theScalar The input scalar.
 @param error an OCString describing the error;
 @result true if successful, false otherwise.
 */
bool PSScalarConvertToCoherentUnit(PSMutableScalarRef theScalar, OCStringRef *error);

/*!
 @function PSScalarCreateByConvertingToCoherentUnit
 @abstract Creates a scalar by converting to a coherent SI unit of the same dimensionality.
 @param theScalar The input scalar.
 @param error an OCString describing the error;
 @result a PSScalar object, or NULL if conversion cannot be done.
 */
PSScalarRef PSScalarCreateByConvertingToCoherentUnit(PSScalarRef theScalar, OCStringRef *error);

/*!
 @function PSScalarCreateByAdding
 @abstract Creates a scalar by adding two scalars of the same dimensionality.
 @param input1 The first scalar.
 @param input2 The second scalar.
 @param error an OCString describing the error;
 @result a PSScalar object, or NULL if operation cannot be done.
 */
PSScalarRef PSScalarCreateByAdding(PSScalarRef input1, PSScalarRef input2, OCStringRef *error);

/*!
 @function PSScalarAdd
 @abstract Adds a scalar of the same dimensionality.
 @param target The mutable scalar.
 @param input2 The scalar to be added.
 @param error an OCString describing the error;
 @result true or false, indicating success of operation.
 */
bool PSScalarAdd(PSMutableScalarRef target, PSScalarRef input2, OCStringRef *error);

/*!
 @function PSScalarCreateBySubtracting
 @abstract Creates a scalar by subtracting two scalars of the same dimensionality.
 @param input1 The subtracted scalar.
 @param input2 The subtracting scalar.
 @param error an OCString describing the error;
 @result a PSScalar object, or NULL if operation cannot be done.
 */
PSScalarRef PSScalarCreateBySubtracting(PSScalarRef input1, PSScalarRef input2, OCStringRef *error);

/*!
 @function PSScalarSubtract
 @abstract Subtracts a scalar of the same dimensionality.
 @param target The mutable scalar.
 @param input2 The scalar to be subtracted.
 @param error an OCString describing the error;
 @result true or false, indicating success of operation.
 */
bool PSScalarSubtract(PSMutableScalarRef target, PSScalarRef input2, OCStringRef *error);

/*!
 @function PSScalarCreateByMultiplyingWithoutReducingUnit
 @abstract Creates a scalar by multiplying two scalars.
 @param input1 The first scalar.
 @param input2 The second scalar.
 @result a PSScalar object, or NULL if operation cannot be done.
 */
PSScalarRef PSScalarCreateByMultiplyingWithoutReducingUnit(PSScalarRef input1, PSScalarRef input2, OCStringRef *error);

/*!
 @function PSScalarMultiplyWithoutReducingUnit
 @abstract Multiplies by a scalar.
 @param target The mutable scalar.
 @param input2 The scalar to be multiplied.
 @result true or false, indicating success of operation.
 */
bool PSScalarMultiplyWithoutReducingUnit(PSMutableScalarRef target, PSScalarRef input2, OCStringRef *error);

/*!
 @function PSScalarCreateByMultiplying
 @abstract Creates a scalar by multiplying two scalars.
 @param input1 The first scalar.
 @param input2 The second scalar.
 @result a PSScalar object, or NULL if operation cannot be done.
 */
PSScalarRef PSScalarCreateByMultiplying(PSScalarRef input1, PSScalarRef input2, OCStringRef *error);

/*!
 @function PSScalarMultiply
 @abstract Multiplies by a scalar.
 @param target The mutable scalar.
 @param input2 The scalar to be multiplied.
 @result true or false, indicating success of operation.
 */
bool PSScalarMultiply(PSMutableScalarRef target, PSScalarRef input2, OCStringRef *error);

/*!
 @function PSScalarCreateByDividingWithoutReducingUnit
 @abstract Creates a scalar by dividing two scalars.
 @param input1 The numerator scalar.
 @param input2 The denominator scalar.
 @param error an OCString describing the error;
 @result a PSScalar object, or NULL if operation cannot be done.
 */
PSScalarRef PSScalarCreateByDividingWithoutReducingUnit(PSScalarRef input1, PSScalarRef input2, OCStringRef *error);

/*!
 @function PSScalarDivideWithoutReducingUnit
 @abstract Divides by a scalar.
 @param target The mutable scalar.
 @param input2 The scalar to be divided by.
 @param error an OCString describing the error;
 @result true or false, indicating success of operation.
 */
bool PSScalarDivideWithoutReducingUnit(PSMutableScalarRef target, PSScalarRef input2, OCStringRef *error);

/*!
 @function PSScalarCreateByDividing
 @abstract Creates a scalar by dividing two scalars.
 @param input1 The numerator scalar.
 @param input2 The denominator scalar.
 @param error an OCString describing the error;
 @result a PSScalar object, or NULL if operation cannot be done.
 */
PSScalarRef PSScalarCreateByDividing(PSScalarRef input1, PSScalarRef input2, OCStringRef *error);

/*!
 @function PSScalarDivide
 @abstract Divides by a scalar.
 @param target The mutable scalar.
 @param input2 The scalar to be divided by.
 @param error an OCString describing the error;
 @result true or false, indicating success of operation.
 */
bool PSScalarDivide(PSMutableScalarRef target, PSScalarRef input2, OCStringRef *error);

/*!
 @function PSScalarCreateByRaisingToAPowerWithoutReducingUnit
 @abstract Creates a scalar by raising to an integer power.
 @param theScalar The  scalar.
 @param power The integer power.
 @result a PSScalar object, or NULL if operation cannot be done.
 */
PSScalarRef PSScalarCreateByRaisingToAPowerWithoutReducingUnit(PSScalarRef theScalar, double power, OCStringRef *error);

/*!
 @function PSScalarRaiseToAPowerWithoutReducingUnit
 @abstract Raise a scalar to an integer power.
 @param theScalar The  scalar.
 @param power The integer power.
 @result true or false, indicating success of operation.
 */
bool PSScalarRaiseToAPowerWithoutReducingUnit(PSMutableScalarRef theScalar, double power, OCStringRef *error);

/*!
 @function PSScalarCreateByRaisingToAPower
 @abstract Creates a scalar by raising to an integer power.
 @param theScalar The  scalar.
 @param power The integer power.
 @result a PSScalar object, or NULL if operation cannot be done.
 */
PSScalarRef PSScalarCreateByRaisingToAPower(PSScalarRef theScalar, double power, OCStringRef *error);

/*!
 @function PSScalarRaiseToAPower
 @abstract Raise a scalar to an integer power.
 @param theScalar The  scalar.
 @param power The integer power.
 @result true or false, indicating success of operation.
 */
bool PSScalarRaiseToAPower(PSMutableScalarRef theScalar, double power, OCStringRef *error);

/*!
 @function PSScalarCreateByTakingAbsoluteValue
 @abstract Take absolute values of a scalar.
 @param theScalar The  scalar.
 @result a PSScalar object, or NULL if operation cannot be done.
 */
PSScalarRef PSScalarCreateByTakingAbsoluteValue(PSScalarRef theScalar, OCStringRef *error);

/*!
 @function PSScalarTakeAbsoluteValue
 @abstract Take absolute values of a scalar.
 @param theScalar The  scalar.
 @result true or false, indicating success of operation.
 */
bool PSScalarTakeAbsoluteValue(PSMutableScalarRef theScalar, OCStringRef *error);

/*!
 @function PSScalarCreateByGammaFunctionWithoutReducingUnit
 @abstract Creates a scalar with Gamma function.
 @param theScalar The  scalar.
 @param error an OCString describing the error;
 @result a PSScalar object, or NULL if operation cannot be done.
 */
PSScalarRef PSScalarCreateByGammaFunctionWithoutReducingUnit(PSScalarRef theScalar, OCStringRef *error);

/*!
 @function PSScalarCreateByTakingNthRoot
 @abstract Creates a scalar by taking the Nth root.
 @param theScalar The  scalar.
 @param root The integer root.
 @param error an OCString describing the error;
 @result a PSScalar object, or NULL if operation cannot be done.
 */
PSScalarRef PSScalarCreateByTakingNthRoot(PSScalarRef theScalar, uint8_t root, OCStringRef *error);

/*!
 @function PSScalarTakeNthRoot
 @abstract Takes the Nth root of the scalar.
 @param theScalar The  scalar.
 @param root The integer root.
 @param error an OCString describing the error;
 @result true or false, indicating success of operation.
 */
bool PSScalarTakeNthRoot(PSMutableScalarRef theScalar, uint8_t root, OCStringRef *error);

/*!
 @function PSScalarTakeLog10
 @abstract Takes the log10 of the scalar.
 @param theScalar The  scalar.
 @param error an OCString describing the error;
 @result true or false, indicating success of operation.
 */
bool PSScalarTakeLog10(PSMutableScalarRef theScalar, OCStringRef *error);

/*!
 @function PSScalarCreateByZeroingPart
 @abstract Creates a scalar by zeroing part of its value.
 @param theScalar The  scalar.
 @param part The part to zero.
 @result a PSScalar object.
 */
PSScalarRef PSScalarCreateByZeroingPart(PSScalarRef theScalar, complexPart part);

/*!
 @function PSScalarZeroPart
 @abstract Zero part of the scalar's value.
 @param theScalar The  scalar.
 @param part The part to zero.
 @result true or false, indicating success of operation.
 */
bool PSScalarZeroPart(PSMutableScalarRef theScalar, complexPart part);

/*!
 @function PSScalarMultiplyByDimensionlessRealConstant
 @abstract Multiplies a scalar by a dimensionless real constant.
 @param theScalar The  scalar.
 @param constant The dimensionless constant.
 @result true or false, indicating success of operation.
 */
bool PSScalarMultiplyByDimensionlessRealConstant(PSMutableScalarRef theScalar, double constant);

/*!
 @function PSScalarCreateByMultiplyingByDimensionlessRealConstant
 @abstract Creates a scalar by multiplying by a dimensionless real constant.
 @param theScalar The  scalar.
 @param constant The dimensionless real constant.
 @result a PSScalar object.
 */
PSScalarRef PSScalarCreateByMultiplyingByDimensionlessRealConstant(PSScalarRef theScalar, double constant);

/*!
 @function PSScalarCreateByMultiplyingByDimensionlessComplexConstant
 @abstract Creates a scalar by multiplying by a dimensionless complex constant.
 @param theScalar The  scalar.
 @param constant The dimensionless complex constant.
 @result a PSScalar object.
 */
PSScalarRef PSScalarCreateByMultiplyingByDimensionlessComplexConstant(PSScalarRef theScalar, double complex constant);

/*!
 @function PSScalarMultiplyByDimensionlessComplexConstant
 @abstract Multiplies a scalar by a dimensionless complex constant.
 @param theScalar The  scalar.
 @param constant The dimensionless constant.
 @result true or false, indicating success of operation.
 */
bool PSScalarMultiplyByDimensionlessComplexConstant(PSMutableScalarRef theScalar, double complex constant);

/*!
 @function PSScalarCreateByConjugation
 @abstract Creates a scalar by conjugation.
 @param theScalar The  scalar.
 @result a PSScalar object.
 */
PSScalarRef PSScalarCreateByConjugation(PSScalarRef theScalar);

/*!
 @function PSScalarConjugate
 @abstract Conjugates a scalar.
 @param theScalar The  scalar.
 @result true or false, indicating success of operation.
 */
bool PSScalarConjugate(PSMutableScalarRef theScalar);

OCArrayRef PSScalarCreateArrayOfConversionQuantitiesScalarsAndStringValues(PSScalarRef theScalar, OCStringRef quantity, OCStringRef *error);
OCArrayRef PSScalarCreateArrayOfConversionQuantitiesAndUnits(PSScalarRef theScalar, OCStringRef quantity, OCStringRef *error);
bool PSScalarBestConversionForQuantity(PSMutableScalarRef theScalar, OCStringRef quantity, OCStringRef *error);

#pragma mark Strings and Archiving
/*!
 @functiongroup Strings and Archiving
 */

/*!
 @function PSScalarShow
 @abstract Prints a short descriptor of the scalar
 @param theScalar The scalar.
 */
void PSScalarShow(PSScalarRef theScalar);

/*!
 @function PSScalarCreateStringValue
 @abstract Creates a string value representation of the scalar.
 @param theScalar The scalar.
 @result a OCString object
 */
OCStringRef PSScalarCreateStringValue(PSScalarRef theScalar);

/*!
 @function PSScalarCreateNumericStringValue
 @abstract Creates a string value representation of the numeric part of the scalar.
 @param theScalar The scalar.
 @result a OCString object
 */
OCStringRef PSScalarCreateNumericStringValue(PSScalarRef theScalar);

/*!
 @function PSScalarCreateStringValueForPart
 @abstract Creates a string value representation for the part of the scalar.
 @param theScalar The scalar.
 @result a OCString object
 */
OCStringRef PSScalarCreateStringValueForPart(PSScalarRef theScalar, complexPart thePart);

/*!
 @function PSScalarCreateUnitString
 @abstract Creates a string value representation of the unit of the scalar.
 @param theScalar The scalar.
 @result a OCString object
 */
OCStringRef PSScalarCreateUnitString(PSScalarRef theScalar);

/*!
 @function PSScalarCreateStringValueWithFormat
 @abstract Creates a string value representation of the scalar using format.
 @param theScalar The scalar.
 @result a OCString object
 */
OCStringRef PSScalarCreateStringValueWithFormat(PSScalarRef theScalar, OCStringRef format);

/*!
 @function PSScalarCreateNumericStringValueWithFormat
 @abstract Creates a string value representation of the numeric part of the scalar using format.
 @param theScalar The scalar.
 @result a OCString object
 */
OCStringRef PSScalarCreateNumericStringValueWithFormat(PSScalarRef theScalar, OCStringRef format);


void PSScalarAddToArrayAsStringValue(PSScalarRef theScalar, OCMutableArrayRef array);

#pragma mark Tests
/*!
 @functiongroup Tests
 */

/*!
 @function PSScalarIsReal
 @abstract Tests if scalar is real.
 @param theScalar The scalar.
 @result true or false.
 */
bool PSScalarIsReal(PSScalarRef theScalar);

/*!
 @function PSScalarIsImaginary
 @abstract Tests if scalar is purely imaginary.
 @param theScalar The scalar.
 @result true or false.
 */
bool PSScalarIsImaginary(PSScalarRef theScalar);

/*!
 @function PSScalarIsComplex
 @abstract Tests if scalar is complex.
 @param theScalar The scalar.
 @result true or false.
 */
bool PSScalarIsComplex(PSScalarRef theScalar);

/*!
 @function PSScalarIsZero
 @abstract Tests if scalar is zero.
 @param theScalar The scalar.
 @result true or false.
 */
bool PSScalarIsZero(PSScalarRef theScalar);

/*!
 @function PSScalarIsInfinite
 @abstract Tests if scalar is infinite.
 @param theScalar The scalar.
 @result true or false.
 */
bool PSScalarIsInfinite(PSScalarRef theScalar);

/*!
 @function PSScalarIsRealNonNegativeInteger
 @abstract Tests if scalar is real and a non-negative integer.
 @param theScalar The scalar.
 @result true or false.
 */
bool PSScalarIsRealNonNegativeInteger(PSScalarRef theScalar);

/*!
 @function PSScalarValidateProposedStringValue
 @abstract Validates where a proposed string contains a scalar quantity with the same dimensionality as the scalar.
 @param theScalar The scalar.
 @param proposedStringValue proposed string contains a quantity.
 @param error pointer to error message.
 @result boolean indicated true or false.
 @discussion It is the responsibility of the calling method to release the CFError object.
 */
bool PSScalarValidateProposedStringValue(PSScalarRef theScalar,OCStringRef proposedStringValue, OCStringRef *error);

/*!
 @function PSScalarEqual
 @abstract Determines if the two scalars are equal in every attribute.
 @param input1 The first scalar.
 @param input2 The second scalar.
 @result true or false.
 */
bool PSScalarEqual(PSScalarRef input1,PSScalarRef input2);

/*!
 @function PSScalarCompare
 @abstract Compares two scalars and returns a comparison result.
 @param scalar The first scalar object to compare..
 @param otherScalar The second scalar object to compare.
 @result A OCComparisonResult constant that indicates whether number is equal to, less than, or greater than otherNumber.
 Possible values are kOCCompareLessThan, kOCCompareEqualTo, kOCCompareGreaterThan, or kOCCompareUnequalDimensionalities
 */
OCComparisonResult PSScalarCompare(PSScalarRef scalar,PSScalarRef otherScalar);

/*!
 @function PSScalarCompareReduced
 @abstract Compares two scalars in reduced units and returns a comparison result.
 @param theScalar The first scalar object to compare..
 @param theOtherScalar The second scalar object to compare.
 @result A OCComparisonResult constant that indicates whether number is equal to, less than, or greater than otherNumber.
 Possible values are kOCCompareLessThan, kOCCompareEqualTo, kOCCompareGreaterThan, or kOCCompareUnequalDimensionalities
 */
OCComparisonResult PSScalarCompareReduced(PSScalarRef theScalar, PSScalarRef theOtherScalar);


OCComparisonResult PSScalarCompareLoose(PSScalarRef theScalar, PSScalarRef theOtherScalar);

/*!
 @author PhySy Ltd
 @copyright PhySy Ltd
 */

#endif /* PSScalar_h */
