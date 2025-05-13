//
//  SIScalar.h
//  SITypes
//
//  Created by philip on 6/14/17.
//  Copyright © 2017 PhySy Ltd. All rights reserved.
//

#ifndef SIScalar_h
#define SIScalar_h

#include "SILibrary.h"

/*!
 @header SIScalar
 @copyright PhySy Ltd
 SIScalar represents a scalar physical quantity. It is a concrete subtype of SIQuantity.
 It has three essential attributes: a unit, an elementType, and a numerical value.
 SIScalar supports four elementTypes, float, double, float complex, and double complex.
 
 For additional details visit <a href="http://www.PhySy.org">http://www.PhySy.org</a>
 
 @unsorted
 */

typedef union __SINumber
{
    float   floatValue;
    double  doubleValue;
    float complex floatComplexValue;
    double complex doubleComplexValue;
} __SINumber;



/*!
 @typedef SIScalarRef
 This is the type of a reference to immutable SIScalar.
 */
typedef const struct __SIScalar *SIScalarRef;

/*!
 @typedef SIMutableScalarRef
 This is the type of a reference to mutable SIScalar.
 */
typedef struct __SIScalar *SIMutableScalarRef;

OCTypeID SIScalarGetTypeID(void);

#pragma mark Creators
/*!
 @functiongroup Creators
 */

/*
 @function SIScalarCreateCopy
 @abstract Creates a copy of a scalar
 @param theScalar The scalar.
 @result a copy of the scalar.
 */
SIScalarRef SIScalarCreateCopy(SIScalarRef theScalar);

/*
 @function SIScalarCreateMutableCopy
 @abstract Creates a mutable copy of a scalar
 @param theScalar The scalar.
 @result a mutable copy of the scalar.
 */
SIMutableScalarRef SIScalarCreateMutableCopy(SIScalarRef theScalar);

/*!
 @function SIScalarCreateWithFloat
 @abstract Creates a scalar from float value and unit.
 @param input_value The float value.
 @param unit The unit.
 @result a SIScalar object
 */
SIScalarRef SIScalarCreateWithFloat(float input_value, SIUnitRef unit);

/*!
 @function SIScalarCreateMutableWithFloat
 @abstract Creates a mutable scalar from float value and unit.
 @param input_value The float value.
 @param unit The unit.
 @result a SIMutableScalar object
 */
SIMutableScalarRef SIScalarCreateMutableWithFloat(float input_value, SIUnitRef unit);

/*!
 @function SIScalarCreateWithDouble
 @abstract Creates a scalar from double value and unit.
 @param input_value The double value.
 @param unit The unit.
 @result a SIScalar object
 */
SIScalarRef SIScalarCreateWithDouble(double input_value, SIUnitRef unit);

/*!
 @function SIScalarCreateMutableWithDouble
 @abstract Creates a mutable scalar from double value and unit.
 @param input_value The double value.
 @param unit The unit.
 @result a SIMutableScalar object
 */
SIMutableScalarRef SIScalarCreateMutableWithDouble(double input_value, SIUnitRef unit);

/*!
 @function SIScalarCreateWithFloatComplex
 @abstract Creates a scalar from float complex value and unit.
 @param input_value The float complex value.
 @param unit The unit.
 @result a SIScalar object
 */
SIScalarRef SIScalarCreateWithFloatComplex(float complex input_value, SIUnitRef unit);

/*!
 @function SIScalarCreateMutableWithFloatComplex
 @abstract Creates a mutable scalar from float complex value and unit.
 @param input_value The float complex value.
 @param unit The unit.
 @result a SIMutableScalar object
 */
SIMutableScalarRef SIScalarCreateMutableWithFloatComplex(float complex input_value, SIUnitRef unit);

/*!
 @function SIScalarCreateWithDoubleComplex
 @abstract Creates a scalar from double complex value and unit.
 @param input_value The double complex value.
 @param unit The unit.
 @result a SIScalar object
 */
SIScalarRef SIScalarCreateWithDoubleComplex(double complex input_value, SIUnitRef unit);

/*!
 @function SIScalarCreateMutableWithDoubleComplex
 @abstract Creates a mutable scalar from double complex value and unit.
 @param input_value The double complex value.
 @param unit The unit.
 @result a SIMutableScalar object
 */
SIMutableScalarRef SIScalarCreateMutableWithDoubleComplex(double complex input_value, SIUnitRef unit);

#pragma mark Accessors
/*!
 @functiongroup Accessors
 */

__SINumber SIScalarGetValue(SIScalarRef theScalar);

void SIScalarSetFloatValue(SIMutableScalarRef theScalar, float value);
void SIScalarSetDoubleValue(SIMutableScalarRef theScalar, double value);
void SIScalarSetFloatComplexValue(SIMutableScalarRef theScalar, float complex value);
void SIScalarSetDoubleComplexValue(SIMutableScalarRef theScalar, double complex value);

void SIScalarSetElementType(SIMutableScalarRef theScalar, numberType elementType);

/*!
 @function SIScalarFloatValue
 @abstract Returns a float value.
 @param theScalar The scalar.
 @result a float value
 */
float SIScalarFloatValue(SIScalarRef theScalar);

/*!
 @function SIScalarDoubleValue
 @abstract Returns a double value.
 @param theScalar The scalar.
 @result a double value
 */
double SIScalarDoubleValue(SIScalarRef theScalar);

/*!
 @function SIScalarFloatComplexValue
 @abstract Returns a float complex value.
 @param theScalar The scalar.
 @result a float complex value
 */
float complex SIScalarFloatComplexValue(SIScalarRef theScalar);

/*!
 @function SIScalarDoubleComplexValue
 @abstract Returns a double complex value.
 @param theScalar The scalar.
 @result a double complex value
 */
double complex SIScalarDoubleComplexValue(SIScalarRef theScalar);

/*!
 @function SIScalarMagnitudeValue
 @abstract Calculates and returns magnitude.
 @param theScalar The scalar.
 @result a double value
 */
double SIScalarMagnitudeValue(SIScalarRef theScalar);

/*!
 @function SIScalarArgumentValue
 @abstract Returns the argument value of scalar.
 @param theScalar The scalar.
 @result the argument
 */
double SIScalarArgumentValue(SIScalarRef theScalar);

/*!
 @function SIScalarFloatValueInUnit
 @abstract Calculates and returns float value after converting to unit.
 @param theScalar The scalar.
 @param unit The unit.
 @param success pointer to boolean that returns true if operation was successful.
 @result a float value
 */
float SIScalarFloatValueInUnit(SIScalarRef theScalar, SIUnitRef unit, bool *success);

/*!
 @function SIScalarDoubleValueInUnit
 @abstract Calculates and returns double value after converting to unit.
 @param theScalar The scalar.
 @param success pointer to boolean that returns true if operation was successful.
 @result a double value
 */
double SIScalarDoubleValueInUnit(SIScalarRef theScalar, SIUnitRef unit, bool *success);

/*!
 @function SIScalarFloatComplexValueInUnit
 @abstract Calculates and returns float complex value after converting to unit.
 @param theScalar The scalar.
 @param success pointer to boolean that returns true if operation was successful.
 @result a float complex value
 */
float complex SIScalarFloatComplexValueInUnit(SIScalarRef theScalar, SIUnitRef unit, bool *success);

/*!
 @function SIScalarDoubleComplexValueInUnit
 @abstract Calculates and returns double complex value after converting to unit.
 @param theScalar The scalar.
 @param success pointer to boolean that returns true if operation was successful.
 @result a double complex value
 */
double complex SIScalarDoubleComplexValueInUnit(SIScalarRef theScalar, SIUnitRef unit, bool *success);

/*!
 @function SIScalarFloatValueInCoherentUnit
 @abstract Calculates and returns float value after converting to its coherent SI unit.
 @param theScalar The scalar.
 @result a float value
 */
float SIScalarFloatValueInCoherentUnit(SIScalarRef theScalar);

/*!
 @function SIScalarDoubleValueInCoherentUnit
 @abstract Calculates and returns double value after converting to its coherent SI unit.
 @param theScalar The scalar.
 @result a double value
 */
double SIScalarDoubleValueInCoherentUnit(SIScalarRef theScalar);

/*!
 @function SIScalarFloatComplexValueInCoherentUnit
 @abstract Calculates and returns float complex value after converting to its coherent SI unit.
 @param theScalar The scalar.
 @result a float complex value
 */
float complex SIScalarFloatComplexValueInCoherentUnit(SIScalarRef theScalar);

/*!
 @function SIScalarDoubleComplexValueInCoherentUnit
 @abstract Calculates and returns double complex value after converting to its coherent SI unit.
 @param theScalar The scalar.
 @result a double complex value
 */
double complex SIScalarDoubleComplexValueInCoherentUnit(SIScalarRef theScalar);

#pragma mark Operations
/*!
 @functiongroup Operations
 */

/*
 @function SIScalarCreateByConvertingToNumberType
 @abstract Creates a copy of a scalar as elementType
 @param theScalar The scalar.
 @result a copy of the scalar.
 @discussion Note that down conversion from double to float loses precision
 and down conversion from complex to real loses the imaginary part
 of the quantity.
 */
SIScalarRef SIScalarCreateByConvertingToNumberType(SIScalarRef theScalar, numberType elementType);

/*!
 @function SIScalarTakeComplexPart
 @abstract Takes the complex number part of a scalar.
 @param theScalar The scalar.
 @param part The complex number part.
 @result true if successful, false otherwise
 @discussion complex number parts are kSIRealPart, kSIImaginaryPart, kSIMagnitudePart, and kSIArgumentPart
 */
bool SIScalarTakeComplexPart(SIMutableScalarRef theScalar, complexPart part);

/*!
 @function SIScalarCreateByTakingComplexPart
 @abstract Creates a scalar from complex number part.
 @param theScalar The scalar.
 @param part The complex number part.
 @result a SIScalar object
 @discussion complex number parts are kSIRealPart, kSIImaginaryPart, kSIMagnitudePart, and kSIArgumentPart
 */
SIScalarRef SIScalarCreateByTakingComplexPart(SIScalarRef theScalar, complexPart part);

SIScalarRef SIScalarCreateWithStringContainingSingleUnitFromLibrary(OCStringRef theString);

/*!
 @function SIScalarCreateWithOCString
 @abstract Creates a scalar by parsing string of number with unit.
 @param string The string.
 @param error an OCString describing the error;
 @result a SIScalar object, or NULL if string cannot be parsed.
 */
SIScalarRef SIScalarCreateWithOCString(OCStringRef string, OCStringRef *error);

/*!
 @function SIScalarCreateByReducingUnit
 @abstract Creates a scalar by reducing its unit to the numerator and denominator exponents to their lowest integer values.
 @param theScalar The input scalar.
 @result a SIScalar object.
 */
SIScalarRef SIScalarCreateByReducingUnit(SIScalarRef theScalar);

/*!
 @function SIScalarReduceUnit
 @abstract Reduces the unit's numerator and denominator exponents to their lowest integer values.
 @param theScalar The input scalar.
 @result a SIScalar object.
 */
bool SIScalarReduceUnit(SIMutableScalarRef theScalar);

/*!
 @function SIScalarConvertToUnit
 @abstract Converts a scalar to another unit of the same dimensionality.
 @param theScalar The input scalar.
 @param unit The new unit.
 @param error an OCString describing the error;
 @result true if successful, false otherwise.
 */
bool SIScalarConvertToUnit(SIMutableScalarRef theScalar, SIUnitRef unit, OCStringRef *error);

/*!
 @function SIScalarCreateByConvertingToUnit
 @abstract Creates a scalar by converting to another unit of the same dimensionality.
 @param theScalar The input scalar.
 @param unit The new unit.
 @param error an OCString describing the error;
 @result a SIScalar object, or NULL if conversion cannot be done.
 */
SIScalarRef SIScalarCreateByConvertingToUnit(SIScalarRef theScalar, SIUnitRef unit, OCStringRef *error);

/*!
 @function SIScalarConvertToCoherentUnit
 @abstract Converts a scalar to a coherent SI unit of the same dimensionality.
 @param theScalar The input scalar.
 @param error an OCString describing the error;
 @result true if successful, false otherwise.
 */
bool SIScalarConvertToCoherentUnit(SIMutableScalarRef theScalar, OCStringRef *error);

/*!
 @function SIScalarCreateByConvertingToCoherentUnit
 @abstract Creates a scalar by converting to a coherent SI unit of the same dimensionality.
 @param theScalar The input scalar.
 @param error an OCString describing the error;
 @result a SIScalar object, or NULL if conversion cannot be done.
 */
SIScalarRef SIScalarCreateByConvertingToCoherentUnit(SIScalarRef theScalar, OCStringRef *error);

/*!
 @function SIScalarCreateByAdding
 @abstract Creates a scalar by adding two scalars of the same dimensionality.
 @param input1 The first scalar.
 @param input2 The second scalar.
 @param error an OCString describing the error;
 @result a SIScalar object, or NULL if operation cannot be done.
 */
SIScalarRef SIScalarCreateByAdding(SIScalarRef input1, SIScalarRef input2, OCStringRef *error);

/*!
 @function SIScalarAdd
 @abstract Adds a scalar of the same dimensionality.
 @param target The mutable scalar.
 @param input2 The scalar to be added.
 @param error an OCString describing the error;
 @result true or false, indicating success of operation.
 */
bool SIScalarAdd(SIMutableScalarRef target, SIScalarRef input2, OCStringRef *error);

/*!
 @function SIScalarCreateBySubtracting
 @abstract Creates a scalar by subtracting two scalars of the same dimensionality.
 @param input1 The subtracted scalar.
 @param input2 The subtracting scalar.
 @param error an OCString describing the error;
 @result a SIScalar object, or NULL if operation cannot be done.
 */
SIScalarRef SIScalarCreateBySubtracting(SIScalarRef input1, SIScalarRef input2, OCStringRef *error);

/*!
 @function SIScalarSubtract
 @abstract Subtracts a scalar of the same dimensionality.
 @param target The mutable scalar.
 @param input2 The scalar to be subtracted.
 @param error an OCString describing the error;
 @result true or false, indicating success of operation.
 */
bool SIScalarSubtract(SIMutableScalarRef target, SIScalarRef input2, OCStringRef *error);

/*!
 @function SIScalarCreateByMultiplyingWithoutReducingUnit
 @abstract Creates a scalar by multiplying two scalars.
 @param input1 The first scalar.
 @param input2 The second scalar.
 @result a SIScalar object, or NULL if operation cannot be done.
 */
SIScalarRef SIScalarCreateByMultiplyingWithoutReducingUnit(SIScalarRef input1, SIScalarRef input2, OCStringRef *error);

/*!
 @function SIScalarMultiplyWithoutReducingUnit
 @abstract Multiplies by a scalar.
 @param target The mutable scalar.
 @param input2 The scalar to be multiplied.
 @result true or false, indicating success of operation.
 */
bool SIScalarMultiplyWithoutReducingUnit(SIMutableScalarRef target, SIScalarRef input2, OCStringRef *error);

/*!
 @function SIScalarCreateByMultiplying
 @abstract Creates a scalar by multiplying two scalars.
 @param input1 The first scalar.
 @param input2 The second scalar.
 @result a SIScalar object, or NULL if operation cannot be done.
 */
SIScalarRef SIScalarCreateByMultiplying(SIScalarRef input1, SIScalarRef input2, OCStringRef *error);

/*!
 @function SIScalarMultiply
 @abstract Multiplies by a scalar.
 @param target The mutable scalar.
 @param input2 The scalar to be multiplied.
 @result true or false, indicating success of operation.
 */
bool SIScalarMultiply(SIMutableScalarRef target, SIScalarRef input2, OCStringRef *error);

/*!
 @function SIScalarCreateByDividingWithoutReducingUnit
 @abstract Creates a scalar by dividing two scalars.
 @param input1 The numerator scalar.
 @param input2 The denominator scalar.
 @param error an OCString describing the error;
 @result a SIScalar object, or NULL if operation cannot be done.
 */
SIScalarRef SIScalarCreateByDividingWithoutReducingUnit(SIScalarRef input1, SIScalarRef input2, OCStringRef *error);

/*!
 @function SIScalarDivideWithoutReducingUnit
 @abstract Divides by a scalar.
 @param target The mutable scalar.
 @param input2 The scalar to be divided by.
 @param error an OCString describing the error;
 @result true or false, indicating success of operation.
 */
bool SIScalarDivideWithoutReducingUnit(SIMutableScalarRef target, SIScalarRef input2, OCStringRef *error);

/*!
 @function SIScalarCreateByDividing
 @abstract Creates a scalar by dividing two scalars.
 @param input1 The numerator scalar.
 @param input2 The denominator scalar.
 @param error an OCString describing the error;
 @result a SIScalar object, or NULL if operation cannot be done.
 */
SIScalarRef SIScalarCreateByDividing(SIScalarRef input1, SIScalarRef input2, OCStringRef *error);

/*!
 @function SIScalarDivide
 @abstract Divides by a scalar.
 @param target The mutable scalar.
 @param input2 The scalar to be divided by.
 @param error an OCString describing the error;
 @result true or false, indicating success of operation.
 */
bool SIScalarDivide(SIMutableScalarRef target, SIScalarRef input2, OCStringRef *error);

/*!
 @function SIScalarCreateByRaisingToAPowerWithoutReducingUnit
 @abstract Creates a scalar by raising to an integer power.
 @param theScalar The  scalar.
 @param power The integer power.
 @result a SIScalar object, or NULL if operation cannot be done.
 */
SIScalarRef SIScalarCreateByRaisingToAPowerWithoutReducingUnit(SIScalarRef theScalar, double power, OCStringRef *error);

/*!
 @function SIScalarRaiseToAPowerWithoutReducingUnit
 @abstract Raise a scalar to an integer power.
 @param theScalar The  scalar.
 @param power The integer power.
 @result true or false, indicating success of operation.
 */
bool SIScalarRaiseToAPowerWithoutReducingUnit(SIMutableScalarRef theScalar, double power, OCStringRef *error);

/*!
 @function SIScalarCreateByRaisingToAPower
 @abstract Creates a scalar by raising to an integer power.
 @param theScalar The  scalar.
 @param power The integer power.
 @result a SIScalar object, or NULL if operation cannot be done.
 */
SIScalarRef SIScalarCreateByRaisingToAPower(SIScalarRef theScalar, double power, OCStringRef *error);

/*!
 @function SIScalarRaiseToAPower
 @abstract Raise a scalar to an integer power.
 @param theScalar The  scalar.
 @param power The integer power.
 @result true or false, indicating success of operation.
 */
bool SIScalarRaiseToAPower(SIMutableScalarRef theScalar, double power, OCStringRef *error);

/*!
 @function SIScalarCreateByTakingAbsoluteValue
 @abstract Take absolute values of a scalar.
 @param theScalar The  scalar.
 @result a SIScalar object, or NULL if operation cannot be done.
 */
SIScalarRef SIScalarCreateByTakingAbsoluteValue(SIScalarRef theScalar, OCStringRef *error);

/*!
 @function SIScalarTakeAbsoluteValue
 @abstract Take absolute values of a scalar.
 @param theScalar The  scalar.
 @result true or false, indicating success of operation.
 */
bool SIScalarTakeAbsoluteValue(SIMutableScalarRef theScalar, OCStringRef *error);

/*!
 @function SIScalarCreateByGammaFunctionWithoutReducingUnit
 @abstract Creates a scalar with Gamma function.
 @param theScalar The  scalar.
 @param error an OCString describing the error;
 @result a SIScalar object, or NULL if operation cannot be done.
 */
SIScalarRef SIScalarCreateByGammaFunctionWithoutReducingUnit(SIScalarRef theScalar, OCStringRef *error);

/*!
 @function SIScalarCreateByTakingNthRoot
 @abstract Creates a scalar by taking the Nth root.
 @param theScalar The  scalar.
 @param root The integer root.
 @param error an OCString describing the error;
 @result a SIScalar object, or NULL if operation cannot be done.
 */
SIScalarRef SIScalarCreateByTakingNthRoot(SIScalarRef theScalar, uint8_t root, OCStringRef *error);

/*!
 @function SIScalarTakeNthRoot
 @abstract Takes the Nth root of the scalar.
 @param theScalar The  scalar.
 @param root The integer root.
 @param error an OCString describing the error;
 @result true or false, indicating success of operation.
 */
bool SIScalarTakeNthRoot(SIMutableScalarRef theScalar, uint8_t root, OCStringRef *error);

/*!
 @function SIScalarTakeLog10
 @abstract Takes the log10 of the scalar.
 @param theScalar The  scalar.
 @param error an OCString describing the error;
 @result true or false, indicating success of operation.
 */
bool SIScalarTakeLog10(SIMutableScalarRef theScalar, OCStringRef *error);

/*!
 @function SIScalarCreateByZeroingPart
 @abstract Creates a scalar by zeroing part of its value.
 @param theScalar The  scalar.
 @param part The part to zero.
 @result a SIScalar object.
 */
SIScalarRef SIScalarCreateByZeroingPart(SIScalarRef theScalar, complexPart part);

/*!
 @function SIScalarZeroPart
 @abstract Zero part of the scalar's value.
 @param theScalar The  scalar.
 @param part The part to zero.
 @result true or false, indicating success of operation.
 */
bool SIScalarZeroPart(SIMutableScalarRef theScalar, complexPart part);

/*!
 @function SIScalarMultiplyByDimensionlessRealConstant
 @abstract Multiplies a scalar by a dimensionless real constant.
 @param theScalar The  scalar.
 @param constant The dimensionless constant.
 @result true or false, indicating success of operation.
 */
bool SIScalarMultiplyByDimensionlessRealConstant(SIMutableScalarRef theScalar, double constant);

/*!
 @function SIScalarCreateByMultiplyingByDimensionlessRealConstant
 @abstract Creates a scalar by multiplying by a dimensionless real constant.
 @param theScalar The  scalar.
 @param constant The dimensionless real constant.
 @result a SIScalar object.
 */
SIScalarRef SIScalarCreateByMultiplyingByDimensionlessRealConstant(SIScalarRef theScalar, double constant);

/*!
 @function SIScalarCreateByMultiplyingByDimensionlessComplexConstant
 @abstract Creates a scalar by multiplying by a dimensionless complex constant.
 @param theScalar The  scalar.
 @param constant The dimensionless complex constant.
 @result a SIScalar object.
 */
SIScalarRef SIScalarCreateByMultiplyingByDimensionlessComplexConstant(SIScalarRef theScalar, double complex constant);

/*!
 @function SIScalarMultiplyByDimensionlessComplexConstant
 @abstract Multiplies a scalar by a dimensionless complex constant.
 @param theScalar The  scalar.
 @param constant The dimensionless constant.
 @result true or false, indicating success of operation.
 */
bool SIScalarMultiplyByDimensionlessComplexConstant(SIMutableScalarRef theScalar, double complex constant);

/*!
 @function SIScalarCreateByConjugation
 @abstract Creates a scalar by conjugation.
 @param theScalar The  scalar.
 @result a SIScalar object.
 */
SIScalarRef SIScalarCreateByConjugation(SIScalarRef theScalar);

/*!
 @function SIScalarConjugate
 @abstract Conjugates a scalar.
 @param theScalar The  scalar.
 @result true or false, indicating success of operation.
 */
bool SIScalarConjugate(SIMutableScalarRef theScalar);

OCArrayRef SIScalarCreateArrayOfConversionQuantitiesScalarsAndStringValues(SIScalarRef theScalar, OCStringRef quantity, OCStringRef *error);
OCArrayRef SIScalarCreateArrayOfConversionQuantitiesAndUnits(SIScalarRef theScalar, OCStringRef quantity, OCStringRef *error);
bool SIScalarBestConversionForQuantity(SIMutableScalarRef theScalar, OCStringRef quantity, OCStringRef *error);

#pragma mark Strings and Archiving
/*!
 @functiongroup Strings and Archiving
 */

/*!
 @function SIScalarShow
 @abstract Prints a short descriptor of the scalar
 @param theScalar The scalar.
 */
void SIScalarShow(SIScalarRef theScalar);

/*!
 @function SIScalarCreateStringValue
 @abstract Creates a string value representation of the scalar.
 @param theScalar The scalar.
 @result a OCString object
 */
OCStringRef SIScalarCreateStringValue(SIScalarRef theScalar);

/*!
 @function SIScalarCreateNumericStringValue
 @abstract Creates a string value representation of the numeric part of the scalar.
 @param theScalar The scalar.
 @result a OCString object
 */
OCStringRef SIScalarCreateNumericStringValue(SIScalarRef theScalar);

/*!
 @function SIScalarCreateStringValueForPart
 @abstract Creates a string value representation for the part of the scalar.
 @param theScalar The scalar.
 @result a OCString object
 */
OCStringRef SIScalarCreateStringValueForPart(SIScalarRef theScalar, complexPart thePart);

/*!
 @function SIScalarCreateUnitString
 @abstract Creates a string value representation of the unit of the scalar.
 @param theScalar The scalar.
 @result a OCString object
 */
OCStringRef SIScalarCreateUnitString(SIScalarRef theScalar);

/*!
 @function SIScalarCreateStringValueWithFormat
 @abstract Creates a string value representation of the scalar using format.
 @param theScalar The scalar.
 @result a OCString object
 */
OCStringRef SIScalarCreateStringValueWithFormat(SIScalarRef theScalar, OCStringRef format);

/*!
 @function SIScalarCreateNumericStringValueWithFormat
 @abstract Creates a string value representation of the numeric part of the scalar using format.
 @param theScalar The scalar.
 @result a OCString object
 */
OCStringRef SIScalarCreateNumericStringValueWithFormat(SIScalarRef theScalar, OCStringRef format);


void SIScalarAddToArrayAsStringValue(SIScalarRef theScalar, OCMutableArrayRef array);

#pragma mark Tests
/*!
 @functiongroup Tests
 */

/*!
 @function SIScalarIsReal
 @abstract Tests if scalar is real.
 @param theScalar The scalar.
 @result true or false.
 */
bool SIScalarIsReal(SIScalarRef theScalar);

/*!
 @function SIScalarIsImaginary
 @abstract Tests if scalar is purely imaginary.
 @param theScalar The scalar.
 @result true or false.
 */
bool SIScalarIsImaginary(SIScalarRef theScalar);

/*!
 @function SIScalarIsComplex
 @abstract Tests if scalar is complex.
 @param theScalar The scalar.
 @result true or false.
 */
bool SIScalarIsComplex(SIScalarRef theScalar);

/*!
 @function SIScalarIsZero
 @abstract Tests if scalar is zero.
 @param theScalar The scalar.
 @result true or false.
 */
bool SIScalarIsZero(SIScalarRef theScalar);

/*!
 @function SIScalarIsInfinite
 @abstract Tests if scalar is infinite.
 @param theScalar The scalar.
 @result true or false.
 */
bool SIScalarIsInfinite(SIScalarRef theScalar);

/*!
 @function SIScalarIsRealNonNegativeInteger
 @abstract Tests if scalar is real and a non-negative integer.
 @param theScalar The scalar.
 @result true or false.
 */
bool SIScalarIsRealNonNegativeInteger(SIScalarRef theScalar);

/*!
 @function SIScalarValidateProposedStringValue
 @abstract Validates where a proposed string contains a scalar quantity with the same dimensionality as the scalar.
 @param theScalar The scalar.
 @param proposedStringValue proposed string contains a quantity.
 @param error pointer to error message.
 @result boolean indicated true or false.
 @discussion It is the responsibility of the calling method to release the CFError object.
 */
bool SIScalarValidateProposedStringValue(SIScalarRef theScalar,OCStringRef proposedStringValue, OCStringRef *error);

/*!
 @function SIScalarEqual
 @abstract Determines if the two scalars are equal in every attribute.
 @param input1 The first scalar.
 @param input2 The second scalar.
 @result true or false.
 */
bool SIScalarEqual(SIScalarRef input1,SIScalarRef input2);

/*!
 @function SIScalarCompare
 @abstract Compares two scalars and returns a comparison result.
 @param scalar The first scalar object to compare..
 @param otherScalar The second scalar object to compare.
 @result A OCComparisonResult constant that indicates whether number is equal to, less than, or greater than otherNumber.
 Possible values are kOCCompareLessThan, kOCCompareEqualTo, kOCCompareGreaterThan, or kOCCompareUnequalDimensionalities
 */
OCComparisonResult SIScalarCompare(SIScalarRef scalar,SIScalarRef otherScalar);

/*!
 @function SIScalarCompareReduced
 @abstract Compares two scalars in reduced units and returns a comparison result.
 @param theScalar The first scalar object to compare..
 @param theOtherScalar The second scalar object to compare.
 @result A OCComparisonResult constant that indicates whether number is equal to, less than, or greater than otherNumber.
 Possible values are kOCCompareLessThan, kOCCompareEqualTo, kOCCompareGreaterThan, or kOCCompareUnequalDimensionalities
 */
OCComparisonResult SIScalarCompareReduced(SIScalarRef theScalar, SIScalarRef theOtherScalar);


OCComparisonResult SIScalarCompareLoose(SIScalarRef theScalar, SIScalarRef theOtherScalar);

/*!
 @author PhySy Ltd
 @copyright PhySy Ltd
 */

#endif /* SIScalar_h */
