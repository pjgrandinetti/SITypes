/**
 * @file SIScalar.h
 * @brief Declares the SIScalar interface for representing and manipulating scalar values in the SI system.
 *
 * SIScalar provides an interface for scalar values, supporting arithmetic operations, type queries, and conversions.
 * The API is CoreFoundation-style, with explicit memory management and reference counting.
 *
 * SIScalar objects are immutable by default; use SIMutableScalarRef for mutable variants.
 *
 * This interface supports querying the value, type, and conversions of a scalar, as well as arithmetic and comparison operations.
 *
 * @author Philip Grandinetti
 * @copyright PhySy Ltd.
 */

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

/**
 * @brief SIScalar represents a scalar physical quantity. It is a concrete subtype of SIQuantity.
 * It has three essential attributes: a unit, an elementType, and a numerical value.
 * SIScalar supports four elementTypes: float, double, float complex, and double complex.
 * 
 * For additional details visit http://www.PhySy.org
 */
typedef union __SINumber
{
    float   floatValue;
    double  doubleValue;
    float complex floatComplexValue;
    double complex doubleComplexValue;
} __SINumber;

/**
 * @typedef SIScalarRef
 * @brief This is the type of a reference to immutable SIScalar.
 */
typedef const struct __SIScalar *SIScalarRef;

/**
 * @typedef SIMutableScalarRef
 * @brief This is the type of a reference to mutable SIScalar.
 */
typedef struct __SIScalar *SIMutableScalarRef;

/**
 * @brief Returns the type ID for SIScalar.
 * @return The type ID.
 */
OCTypeID SIScalarGetTypeID(void);

#pragma mark Creators
/**
 * @brief Creates a copy of a scalar.
 * @param theScalar The scalar.
 * @return A copy of the scalar.
 */
SIScalarRef SIScalarCreateCopy(SIScalarRef theScalar);

/**
 * @brief Creates a mutable copy of a scalar.
 * @param theScalar The scalar.
 * @return A mutable copy of the scalar.
 */
SIMutableScalarRef SIScalarCreateMutableCopy(SIScalarRef theScalar);

/**
 * @brief Creates a scalar from float value and unit.
 * @param input_value The float value.
 * @param unit The unit.
 * @return A SIScalar object.
 */
SIScalarRef SIScalarCreateWithFloat(float input_value, SIUnitRef unit);

/**
 * @brief Creates a mutable scalar from float value and unit.
 * @param input_value The float value.
 * @param unit The unit.
 * @return A SIMutableScalar object.
 */
SIMutableScalarRef SIScalarCreateMutableWithFloat(float input_value, SIUnitRef unit);

/**
 * @brief Creates a scalar from double value and unit.
 * @param input_value The double value.
 * @param unit The unit.
 * @return A SIScalar object.
 */
SIScalarRef SIScalarCreateWithDouble(double input_value, SIUnitRef unit);

/**
 * @brief Creates a mutable scalar from double value and unit.
 * @param input_value The double value.
 * @param unit The unit.
 * @return A SIMutableScalar object.
 */
SIMutableScalarRef SIScalarCreateMutableWithDouble(double input_value, SIUnitRef unit);

/**
 * @brief Creates a scalar from float complex value and unit.
 * @param input_value The float complex value.
 * @param unit The unit.
 * @return A SIScalar object.
 */
SIScalarRef SIScalarCreateWithFloatComplex(float complex input_value, SIUnitRef unit);

/**
 * @brief Creates a mutable scalar from float complex value and unit.
 * @param input_value The float complex value.
 * @param unit The unit.
 * @return A SIMutableScalar object.
 */
SIMutableScalarRef SIScalarCreateMutableWithFloatComplex(float complex input_value, SIUnitRef unit);

/**
 * @brief Creates a scalar from double complex value and unit.
 * @param input_value The double complex value.
 * @param unit The unit.
 * @return A SIScalar object.
 */
SIScalarRef SIScalarCreateWithDoubleComplex(double complex input_value, SIUnitRef unit);

/**
 * @brief Creates a mutable scalar from double complex value and unit.
 * @param input_value The double complex value.
 * @param unit The unit.
 * @return A SIMutableScalar object.
 */
SIMutableScalarRef SIScalarCreateMutableWithDoubleComplex(double complex input_value, SIUnitRef unit);

/**
 * @brief Creates a new SIScalar object from a string containing a single unit name from the unit library.
 *
 * This function parses the provided OCStringRef, which should contain a single valid unit name (e.g., "m", "kg", "s").
 * It returns a new immutable SIScalarRef representing the parsed unit with a value of 1.0, or NULL if the string is invalid or the unit is not found.
 *
 * @param theString An OCStringRef containing the name or symbol of a single unit to look up in the unit library.
 * @return A new SIScalarRef representing the unit with value 1.0, or NULL if the string is invalid or the unit is not found.
 *
 * @note The caller is responsible for releasing the returned SIScalarRef.
 * @note This function does not support parsing compound units or expressions; only single unit names are accepted.
 */
SIScalarRef SIScalarCreateWithStringContainingSingleUnitFromLibrary(OCStringRef theString);

#pragma mark Accessors
/**
 * @brief Gets the value of a scalar.
 * @param theScalar The scalar.
 * @return The value of the scalar.
 */
__SINumber SIScalarGetValue(SIScalarRef theScalar);

/**
 * @brief Sets the float value of a mutable scalar.
 * @param theScalar The mutable scalar.
 * @param value The float value.
 */
void SIScalarSetFloatValue(SIMutableScalarRef theScalar, float value);

/**
 * @brief Sets the double value of a mutable scalar.
 * @param theScalar The mutable scalar.
 * @param value The double value.
 */
void SIScalarSetDoubleValue(SIMutableScalarRef theScalar, double value);

/**
 * @brief Sets the float complex value of a mutable scalar.
 * @param theScalar The mutable scalar.
 * @param value The float complex value.
 */
void SIScalarSetFloatComplexValue(SIMutableScalarRef theScalar, float complex value);

/**
 * @brief Sets the double complex value of a mutable scalar.
 * @param theScalar The mutable scalar.
 * @param value The double complex value.
 */
void SIScalarSetDoubleComplexValue(SIMutableScalarRef theScalar, double complex value);

/**
 * @brief Sets the element type of a mutable scalar.
 * @param theScalar The mutable scalar.
 * @param elementType The element type.
 */
void SIScalarSetElementType(SIMutableScalarRef theScalar, numberType elementType);

/**
 * @brief Returns a float value.
 * @param theScalar The scalar.
 * @return A float value.
 */
float SIScalarFloatValue(SIScalarRef theScalar);

/**
 * @brief Returns a double value.
 * @param theScalar The scalar.
 * @return A double value.
 */
double SIScalarDoubleValue(SIScalarRef theScalar);

/**
 * @brief Returns a float complex value.
 * @param theScalar The scalar.
 * @return A float complex value.
 */
float complex SIScalarFloatComplexValue(SIScalarRef theScalar);

/**
 * @brief Returns a double complex value.
 * @param theScalar The scalar.
 * @return A double complex value.
 */
double complex SIScalarDoubleComplexValue(SIScalarRef theScalar);

/**
 * @brief Calculates and returns magnitude.
 * @param theScalar The scalar.
 * @return A double value.
 */
double SIScalarMagnitudeValue(SIScalarRef theScalar);

/**
 * @brief Returns the argument value of scalar.
 * @param theScalar The scalar.
 * @return The argument.
 */
double SIScalarArgumentValue(SIScalarRef theScalar);

/**
 * @brief Calculates and returns float value after converting to unit.
 * @param theScalar The scalar.
 * @param unit The unit.
 * @param success Pointer to boolean that returns true if operation was successful.
 * @return A float value.
 */
float SIScalarFloatValueInUnit(SIScalarRef theScalar, SIUnitRef unit, bool *success);

/**
 * @brief Calculates and returns double value after converting to unit.
 * @param theScalar The scalar.
 * @param unit The unit.
 * @param success Pointer to boolean that returns true if operation was successful.
 * @return A double value.
 */
double SIScalarDoubleValueInUnit(SIScalarRef theScalar, SIUnitRef unit, bool *success);

/**
 * @brief Calculates and returns float complex value after converting to unit.
 * @param theScalar The scalar.
 * @param unit The unit.
 * @param success Pointer to boolean that returns true if operation was successful.
 * @return A float complex value.
 */
float complex SIScalarFloatComplexValueInUnit(SIScalarRef theScalar, SIUnitRef unit, bool *success);

/**
 * @brief Calculates and returns double complex value after converting to unit.
 * @param theScalar The scalar.
 * @param unit The unit.
 * @param success Pointer to boolean that returns true if operation was successful.
 * @return A double complex value.
 */
double complex SIScalarDoubleComplexValueInUnit(SIScalarRef theScalar, SIUnitRef unit, bool *success);

/**
 * @brief Calculates and returns float value after converting to its coherent SI unit.
 * @param theScalar The scalar.
 * @return A float value.
 */
float SIScalarFloatValueInCoherentUnit(SIScalarRef theScalar);

/**
 * @brief Calculates and returns double value after converting to its coherent SI unit.
 * @param theScalar The scalar.
 * @return A double value.
 */
double SIScalarDoubleValueInCoherentUnit(SIScalarRef theScalar);

/**
 * @brief Calculates and returns float complex value after converting to its coherent SI unit.
 * @param theScalar The scalar.
 * @return A float complex value.
 */
float complex SIScalarFloatComplexValueInCoherentUnit(SIScalarRef theScalar);

/**
 * @brief Calculates and returns double complex value after converting to its coherent SI unit.
 * @param theScalar The scalar.
 * @return A double complex value.
 */
double complex SIScalarDoubleComplexValueInCoherentUnit(SIScalarRef theScalar);

#pragma mark Operations
/**
 * @brief Creates a copy of a scalar as elementType.
 * @param theScalar The scalar.
 * @param elementType The element type.
 * @return A copy of the scalar.
 * @details Note that down conversion from double to float loses precision
 * and down conversion from complex to real loses the imaginary part
 * of the quantity.
 */
SIScalarRef SIScalarCreateByConvertingToNumberType(SIScalarRef theScalar, numberType elementType);

/**
 * @brief Takes the complex number part of a scalar.
 * @param theScalar The scalar.
 * @param part The complex number part.
 * @return True if successful, false otherwise.
 * @details Complex number parts are kSIRealPart, kSIImaginaryPart, kSIMagnitudePart, and kSIArgumentPart.
 */
bool SIScalarTakeComplexPart(SIMutableScalarRef theScalar, complexPart part);

/**
 * @brief Creates a scalar from complex number part.
 * @param theScalar The scalar.
 * @param part The complex number part.
 * @return A SIScalar object.
 * @details Complex number parts are kSIRealPart, kSIImaginaryPart, kSIMagnitudePart, and kSIArgumentPart.
 */
SIScalarRef SIScalarCreateByTakingComplexPart(SIScalarRef theScalar, complexPart part);

/**
 * @brief Creates a scalar by parsing string of number with unit.
 * @param string The string.
 * @param error An OCString describing the error.
 * @return A SIScalar object, or NULL if string cannot be parsed.
 */
SIScalarRef SIScalarCreateWithOCString(OCStringRef string, OCStringRef *error);

/**
 * @brief Creates a scalar by reducing its unit to the numerator and denominator exponents to their lowest integer values.
 * @param theScalar The input scalar.
 * @return A SIScalar object.
 */
SIScalarRef SIScalarCreateByReducingUnit(SIScalarRef theScalar);

/**
 * @brief Reduces the unit's numerator and denominator exponents to their lowest integer values.
 * @param theScalar The input scalar.
 * @return True if successful, false otherwise.
 */
bool SIScalarReduceUnit(SIMutableScalarRef theScalar);

/**
 * @brief Converts a scalar to another unit of the same dimensionality.
 * @param theScalar The input scalar.
 * @param unit The new unit.
 * @param error An OCString describing the error.
 * @return True if successful, false otherwise.
 */
bool SIScalarConvertToUnit(SIMutableScalarRef theScalar, SIUnitRef unit, OCStringRef *error);

/**
 * @brief Creates a scalar by converting to another unit of the same dimensionality.
 * @param theScalar The input scalar.
 * @param unit The new unit.
 * @param error An OCString describing the error.
 * @return A SIScalar object, or NULL if conversion cannot be done.
 */
SIScalarRef SIScalarCreateByConvertingToUnit(SIScalarRef theScalar, SIUnitRef unit, OCStringRef *error);

/**
 * @brief Converts a scalar to a coherent SI unit of the same dimensionality.
 * @param theScalar The input scalar.
 * @param error An OCString describing the error.
 * @return True if successful, false otherwise.
 */
bool SIScalarConvertToCoherentUnit(SIMutableScalarRef theScalar, OCStringRef *error);

/**
 * @brief Creates a scalar by converting to a coherent SI unit of the same dimensionality.
 * @param theScalar The input scalar.
 * @param error An OCString describing the error.
 * @return A SIScalar object, or NULL if conversion cannot be done.
 */
SIScalarRef SIScalarCreateByConvertingToCoherentUnit(SIScalarRef theScalar, OCStringRef *error);

/**
 * @brief Creates a scalar by adding two scalars of the same dimensionality.
 * @param input1 The first scalar.
 * @param input2 The second scalar.
 * @param error An OCString describing the error.
 * @return A SIScalar object, or NULL if operation cannot be done.
 */
SIScalarRef SIScalarCreateByAdding(SIScalarRef input1, SIScalarRef input2, OCStringRef *error);

/**
 * @brief Adds a scalar of the same dimensionality.
 * @param target The mutable scalar.
 * @param input2 The scalar to be added.
 * @param error An OCString describing the error.
 * @return True or false, indicating success of operation.
 */
bool SIScalarAdd(SIMutableScalarRef target, SIScalarRef input2, OCStringRef *error);

/**
 * @brief Creates a scalar by subtracting two scalars of the same dimensionality.
 * @param input1 The subtracted scalar.
 * @param input2 The subtracting scalar.
 * @param error An OCString describing the error.
 * @return A SIScalar object, or NULL if operation cannot be done.
 */
SIScalarRef SIScalarCreateBySubtracting(SIScalarRef input1, SIScalarRef input2, OCStringRef *error);

/**
 * @brief Subtracts a scalar of the same dimensionality.
 * @param target The mutable scalar.
 * @param input2 The scalar to be subtracted.
 * @param error An OCString describing the error.
 * @return True or false, indicating success of operation.
 */
bool SIScalarSubtract(SIMutableScalarRef target, SIScalarRef input2, OCStringRef *error);

/**
 * @brief Creates a scalar by multiplying two scalars without reducing unit.
 * @param input1 The first scalar.
 * @param input2 The second scalar.
 * @param error An OCString describing the error.
 * @return A SIScalar object, or NULL if operation cannot be done.
 */
SIScalarRef SIScalarCreateByMultiplyingWithoutReducingUnit(SIScalarRef input1, SIScalarRef input2, OCStringRef *error);

/**
 * @brief Multiplies by a scalar without reducing unit.
 * @param target The mutable scalar.
 * @param input2 The scalar to be multiplied.
 * @param error An OCString describing the error.
 * @return True or false, indicating success of operation.
 */
bool SIScalarMultiplyWithoutReducingUnit(SIMutableScalarRef target, SIScalarRef input2, OCStringRef *error);

/**
 * @brief Creates a scalar by multiplying two scalars.
 * @param input1 The first scalar.
 * @param input2 The second scalar.
 * @param error An OCString describing the error.
 * @return A SIScalar object, or NULL if operation cannot be done.
 */
SIScalarRef SIScalarCreateByMultiplying(SIScalarRef input1, SIScalarRef input2, OCStringRef *error);

/**
 * @brief Multiplies by a scalar.
 * @param target The mutable scalar.
 * @param input2 The scalar to be multiplied.
 * @param error An OCString describing the error.
 * @return True or false, indicating success of operation.
 */
bool SIScalarMultiply(SIMutableScalarRef target, SIScalarRef input2, OCStringRef *error);

/**
 * @brief Creates a scalar by dividing two scalars without reducing unit.
 * @param input1 The numerator scalar.
 * @param input2 The denominator scalar.
 * @param error An OCString describing the error.
 * @return A SIScalar object, or NULL if operation cannot be done.
 */
SIScalarRef SIScalarCreateByDividingWithoutReducingUnit(SIScalarRef input1, SIScalarRef input2, OCStringRef *error);

/**
 * @brief Divides by a scalar without reducing unit.
 * @param target The mutable scalar.
 * @param input2 The scalar to be divided by.
 * @param error An OCString describing the error.
 * @return True or false, indicating success of operation.
 */
bool SIScalarDivideWithoutReducingUnit(SIMutableScalarRef target, SIScalarRef input2, OCStringRef *error);

/**
 * @brief Creates a scalar by dividing two scalars.
 * @param input1 The numerator scalar.
 * @param input2 The denominator scalar.
 * @param error An OCString describing the error.
 * @return A SIScalar object, or NULL if operation cannot be done.
 */
SIScalarRef SIScalarCreateByDividing(SIScalarRef input1, SIScalarRef input2, OCStringRef *error);

/**
 * @brief Divides by a scalar.
 * @param target The mutable scalar.
 * @param input2 The scalar to be divided by.
 * @param error An OCString describing the error.
 * @return True or false, indicating success of operation.
 */
bool SIScalarDivide(SIMutableScalarRef target, SIScalarRef input2, OCStringRef *error);

/**
 * @brief Creates a scalar by raising to an integer power without reducing unit.
 * @param theScalar The scalar.
 * @param power The integer power.
 * @param error An OCString describing the error.
 * @return A SIScalar object, or NULL if operation cannot be done.
 */
SIScalarRef SIScalarCreateByRaisingToAPowerWithoutReducingUnit(SIScalarRef theScalar, double power, OCStringRef *error);

/**
 * @brief Raises a scalar to an integer power without reducing unit.
 * @param theScalar The scalar.
 * @param power The integer power.
 * @param error An OCString describing the error.
 * @return True or false, indicating success of operation.
 */
bool SIScalarRaiseToAPowerWithoutReducingUnit(SIMutableScalarRef theScalar, double power, OCStringRef *error);

/**
 * @brief Creates a scalar by raising to an integer power.
 * @param theScalar The scalar.
 * @param power The integer power.
 * @param error An OCString describing the error.
 * @return A SIScalar object, or NULL if operation cannot be done.
 */
SIScalarRef SIScalarCreateByRaisingToAPower(SIScalarRef theScalar, double power, OCStringRef *error);

/**
 * @brief Raises a scalar to an integer power.
 * @param theScalar The scalar.
 * @param power The integer power.
 * @param error An OCString describing the error.
 * @return True or false, indicating success of operation.
 */
bool SIScalarRaiseToAPower(SIMutableScalarRef theScalar, double power, OCStringRef *error);

/**
 * @brief Creates a scalar by taking absolute values.
 * @param theScalar The scalar.
 * @param error An OCString describing the error.
 * @return A SIScalar object, or NULL if operation cannot be done.
 */
SIScalarRef SIScalarCreateByTakingAbsoluteValue(SIScalarRef theScalar, OCStringRef *error);

/**
 * @brief Takes absolute values of a scalar.
 * @param theScalar The scalar.
 * @param error An OCString describing the error.
 * @return True or false, indicating success of operation.
 */
bool SIScalarTakeAbsoluteValue(SIMutableScalarRef theScalar, OCStringRef *error);

/**
 * @brief Creates a scalar with Gamma function without reducing unit.
 * @param theScalar The scalar.
 * @param error An OCString describing the error.
 * @return A SIScalar object, or NULL if operation cannot be done.
 */
SIScalarRef SIScalarCreateByGammaFunctionWithoutReducingUnit(SIScalarRef theScalar, OCStringRef *error);

/**
 * @brief Creates a scalar by taking the Nth root.
 * @param theScalar The scalar.
 * @param root The integer root.
 * @param error An OCString describing the error.
 * @return A SIScalar object, or NULL if operation cannot be done.
 */
SIScalarRef SIScalarCreateByTakingNthRoot(SIScalarRef theScalar, uint8_t root, OCStringRef *error);

/**
 * @brief Takes the Nth root of the scalar.
 * @param theScalar The scalar.
 * @param root The integer root.
 * @param error An OCString describing the error.
 * @return True or false, indicating success of operation.
 */
bool SIScalarTakeNthRoot(SIMutableScalarRef theScalar, uint8_t root, OCStringRef *error);

/**
 * @brief Takes the log10 of the scalar.
 * @param theScalar The scalar.
 * @param error An OCString describing the error.
 * @return True or false, indicating success of operation.
 */
bool SIScalarTakeLog10(SIMutableScalarRef theScalar, OCStringRef *error);

/**
 * @brief Creates a scalar by zeroing part of its value.
 * @param theScalar The scalar.
 * @param part The part to zero.
 * @return A SIScalar object.
 */
SIScalarRef SIScalarCreateByZeroingPart(SIScalarRef theScalar, complexPart part);

/**
 * @brief Zero part of the scalar's value.
 * @param theScalar The scalar.
 * @param part The part to zero.
 * @return True or false, indicating success of operation.
 */
bool SIScalarZeroPart(SIMutableScalarRef theScalar, complexPart part);

/**
 * @brief Multiplies a scalar by a dimensionless real constant.
 * @param theScalar The scalar.
 * @param constant The dimensionless constant.
 * @return True or false, indicating success of operation.
 */
bool SIScalarMultiplyByDimensionlessRealConstant(SIMutableScalarRef theScalar, double constant);

/**
 * @brief Creates a scalar by multiplying by a dimensionless real constant.
 * @param theScalar The scalar.
 * @param constant The dimensionless real constant.
 * @return A SIScalar object.
 */
SIScalarRef SIScalarCreateByMultiplyingByDimensionlessRealConstant(SIScalarRef theScalar, double constant);

/**
 * @brief Creates a scalar by multiplying by a dimensionless complex constant.
 * @param theScalar The scalar.
 * @param constant The dimensionless complex constant.
 * @return A SIScalar object.
 */
SIScalarRef SIScalarCreateByMultiplyingByDimensionlessComplexConstant(SIScalarRef theScalar, double complex constant);

/**
 * @brief Multiplies a scalar by a dimensionless complex constant.
 * @param theScalar The scalar.
 * @param constant The dimensionless constant.
 * @return True or false, indicating success of operation.
 */
bool SIScalarMultiplyByDimensionlessComplexConstant(SIMutableScalarRef theScalar, double complex constant);

/**
 * @brief Creates a scalar by conjugation.
 * @param theScalar The scalar.
 * @return A SIScalar object.
 */
SIScalarRef SIScalarCreateByConjugation(SIScalarRef theScalar);

/**
 * @brief Conjugates a scalar.
 * @param theScalar The scalar.
 * @return True or false, indicating success of operation.
 */
bool SIScalarConjugate(SIMutableScalarRef theScalar);

#pragma mark Strings and Archiving
/**
 * @brief Prints a short descriptor of the scalar.
 * @param theScalar The scalar.
 */
void SIScalarShow(SIScalarRef theScalar);

/**
 * @brief Creates a string value representation of the scalar.
 * @param theScalar The scalar.
 * @return An OCString object.
 */
OCStringRef SIScalarCreateStringValue(SIScalarRef theScalar);

/**
 * @brief Creates a string value representation of the numeric part of the scalar.
 * @param theScalar The scalar.
 * @return An OCString object.
 */
OCStringRef SIScalarCreateNumericStringValue(SIScalarRef theScalar);

/**
 * @brief Creates a string value representation for the part of the scalar.
 * @param theScalar The scalar.
 * @param thePart The part of the scalar.
 * @return An OCString object.
 */
OCStringRef SIScalarCreateStringValueForPart(SIScalarRef theScalar, complexPart thePart);

/**
 * @brief Creates a string value representation of the unit of the scalar.
 * @param theScalar The scalar.
 * @return An OCString object.
 */
OCStringRef SIScalarCreateUnitString(SIScalarRef theScalar);

/**
 * @brief Creates a string value representation of the scalar using format.
 * @param theScalar The scalar.
 * @param format The format.
 * @return An OCString object.
 */
OCStringRef SIScalarCreateStringValueWithFormat(SIScalarRef theScalar, OCStringRef format);

/**
 * @brief Creates a string value representation of the numeric part of the scalar using format.
 * @param theScalar The scalar.
 * @param format The format.
 * @return An OCString object.
 */
OCStringRef SIScalarCreateNumericStringValueWithFormat(SIScalarRef theScalar, OCStringRef format);

/**
 * @brief Adds the scalar to an array as a string value.
 * @param theScalar The scalar.
 * @param array The array.
 */
void SIScalarAddToArrayAsStringValue(SIScalarRef theScalar, OCMutableArrayRef array);

#pragma mark Tests
/**
 * @brief Tests if scalar is real.
 * @param theScalar The scalar.
 * @return True or false.
 */
bool SIScalarIsReal(SIScalarRef theScalar);

/**
 * @brief Tests if scalar is purely imaginary.
 * @param theScalar The scalar.
 * @return True or false.
 */
bool SIScalarIsImaginary(SIScalarRef theScalar);

/**
 * @brief Tests if scalar is complex.
 * @param theScalar The scalar.
 * @return True or false.
 */
bool SIScalarIsComplex(SIScalarRef theScalar);

/**
 * @brief Tests if scalar is zero.
 * @param theScalar The scalar.
 * @return True or false.
 */
bool SIScalarIsZero(SIScalarRef theScalar);

/**
 * @brief Tests if scalar is infinite.
 * @param theScalar The scalar.
 * @return True or false.
 */
bool SIScalarIsInfinite(SIScalarRef theScalar);

/**
 * @brief Tests if scalar is real and a non-negative integer.
 * @param theScalar The scalar.
 * @return True or false.
 */
bool SIScalarIsRealNonNegativeInteger(SIScalarRef theScalar);

/**
 * @brief Validates whether a proposed string contains a scalar quantity with the same dimensionality as the scalar.
 * @param theScalar The scalar.
 * @param proposedStringValue Proposed string containing a quantity.
 * @param error Pointer to error message.
 * @return Boolean indicating true or false.
 * @details It is the responsibility of the calling method to release the CFError object.
 */
bool SIScalarValidateProposedStringValue(SIScalarRef theScalar, OCStringRef proposedStringValue, OCStringRef *error);

/**
 * @brief Determines if the two scalars are equal in every attribute.
 * @param input1 The first scalar.
 * @param input2 The second scalar.
 * @return True or false.
 */
bool SIScalarEqual(SIScalarRef input1, SIScalarRef input2);

/**
 * @brief Compares two scalars and returns a comparison result.
 * @param scalar The first scalar object to compare.
 * @param otherScalar The second scalar object to compare.
 * @return A OCComparisonResult constant that indicates whether number is equal to, less than, or greater than otherNumber.
 * Possible values are kOCCompareLessThan, kOCCompareEqualTo, kOCCompareGreaterThan, or kOCCompareUnequalDimensionalities.
 */
OCComparisonResult SIScalarCompare(SIScalarRef scalar, SIScalarRef otherScalar);

/**
 * @brief Compares two scalars in reduced units and returns a comparison result.
 * @param theScalar The first scalar object to compare.
 * @param theOtherScalar The second scalar object to compare.
 * @return A OCComparisonResult constant that indicates whether number is equal to, less than, or greater than otherNumber.
 * Possible values are kOCCompareLessThan, kOCCompareEqualTo, kOCCompareGreaterThan, or kOCCompareUnequalDimensionalities.
 */
OCComparisonResult SIScalarCompareReduced(SIScalarRef theScalar, SIScalarRef theOtherScalar);

/**
 * @brief Compares two scalars loosely and returns a comparison result.
 * @param theScalar The first scalar object to compare.
 * @param theOtherScalar The second scalar object to compare.
 * @return A OCComparisonResult constant that indicates whether number is equal to, less than, or greater than otherNumber.
 */
OCComparisonResult SIScalarCompareLoose(SIScalarRef theScalar, SIScalarRef theOtherScalar);

#endif /* SIScalar_h */
