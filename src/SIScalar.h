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
 */

/** @cond INTERNAL */
/**
 * @brief SIScalar represents a scalar physical quantity. It is a concrete subtype of SIQuantity.
 * It has three essential attributes: a unit, an elementType, and a numerical value.
 * SIScalar supports four elementTypes: float, double, float complex, and double complex.
 * 
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
/** @endcond */

/**
 * @brief Returns the unique type identifier for SIScalar objects.
 *
 * Every SIScalar instance is associated with a globally unique OCTypeID.
 * This identifier can be used for runtime type checking, assertions,
 * or bridging with other OC-based frameworks.
 *
 *
 * @return The OCTypeID corresponding to the SIScalar “class.”
 *
 * @see OCGetTypeID, OCTypeID
 */
OCTypeID SIScalarGetTypeID(void);

#pragma mark Creators

/**
 * @brief Creates a deep copy of a SIScalar object.
 *
 * SIScalarRef inherits from OCTypeRef. This function allocates and returns a new OCType-compatible
 * SIScalar instance with an initial retain count of 1. The new instance contains the same numeric
 * value, SI unit, and metadata as the original.
 *
 * Use this when you require an independent SIScalar for modification, thread-safe operations,
 * or to manage object lifetimes separately.
 *
 * @param theScalar The SIScalarRef to copy. Must not be NULL.
 *
 *
 * @return A new SIScalarRef (an OCTypeRef) representing the copied scalar. The caller owns
 *         this reference and must balance it with OCRelease when it is no longer needed.
 *
 * @see OCRelease, OCRetain, OCTypeGetRetainCount
 */
SIScalarRef SIScalarCreateCopy(SIScalarRef theScalar);

/**
 * @brief Creates a mutable deep copy of a SIScalar object.
 *
 * SIScalarRef inherits from OCTypeRef. This function allocates and returns a new OCType-compatible
 * SIMutableScalar instance with an initial retain count of 1. The new instance is initialized with
 * the same numeric value, SI unit, and metadata as the original, but allows modification of its
 * value and other mutable properties.
 *
 * Use this when you need an independently modifiable SIScalar for operations that alter its state,
 * while preserving the original instance.
 *
 * @param theScalar The SIScalarRef to copy. Must not be NULL.
 *
 *
 * @return A new SIMutableScalarRef representing the mutable copy. The caller owns this reference and
 *         must balance it with OCRelease when it is no longer needed.
 *
 * @see OCRelease, OCRetain, OCTypeGetRetainCount
 */
SIMutableScalarRef SIScalarCreateMutableCopy(SIScalarRef theScalar);

/**
 * @brief Creates an immutable SIScalar object with a specified float value and SI unit.
 *
 * SIScalarRef inherits from OCTypeRef. This function allocates and returns a new OCType-compatible
 * SIScalar instance, initialized with the provided numeric value and unit. The new instance has an
 * initial retain count of 1.
 *
 * @param input_value The float value for the scalar.
 * @param unit        The SIUnitRef representing the unit. Must not be NULL.
 *
 *
 * @return A new SIScalarRef representing the scalar. The caller owns this reference and must balance
 *         it with OCRelease when it is no longer needed.
 *
 * @see OCRelease, OCRetain, OCTypeGetRetainCount
 */
SIScalarRef SIScalarCreateWithFloat(float input_value, SIUnitRef unit);

/**
 * @brief Creates a mutable SIScalar object with a specified float value and SI unit.
 *
 * SIMutableScalarRef inherits from OCTypeRef. This function allocates and returns a new
 * OCType-compatible SIMutableScalar instance, initialized with the provided numeric value and unit.
 * The instance has an initial retain count of 1 and supports subsequent modification of its state.
 *
 * @param input_value The float value for the scalar.
 * @param unit        The SIUnitRef representing the unit. Must not be NULL.
 *
 *
 * @return A new SIMutableScalarRef representing the mutable scalar. The caller owns this reference
 *         and must balance it with OCRelease when it is no longer needed.
 *
 * @see OCRelease, OCRetain, OCTypeGetRetainCount
 */

/**
 * @brief Creates a mutable SIScalar object with a specified float value and SI unit.
 *
 * SIMutableScalarRef inherits from OCTypeRef. This function allocates and returns a new
 * OCType-compatible SIMutableScalar instance, initialized with the provided float value
 * and unit. The instance has an initial retain count of 1 and supports subsequent modification
 * of its internal state.
 *
 * @param input_value The float value for the scalar.
 * @param unit        The SIUnitRef representing the unit. Must not be NULL.
 *
 *
 * @return A new SIMutableScalarRef representing the mutable scalar. The caller owns this reference
 *         and must balance it with OCRelease when it is no longer needed.
 *
 * @see OCRelease, OCRetain, OCTypeGetRetainCount
 */
SIMutableScalarRef SIScalarCreateMutableWithFloat(float input_value, SIUnitRef unit);

/**
 * @brief Creates an immutable SIScalar object with a specified double value and SI unit.
 *
 * SIScalarRef inherits from OCTypeRef. This function allocates and returns a new
 * OCType-compatible SIScalar instance, initialized with the provided double value
 * and unit. The new instance has an initial retain count of 1.
 *
 * @param input_value The double value for the scalar.
 * @param unit        The SIUnitRef representing the unit. Must not be NULL.
 *
 *
 * @return A new SIScalarRef representing the scalar. The caller owns this reference
 *         and must balance it with OCRelease when it is no longer needed.
 *
 * @see OCRelease, OCRetain, OCTypeGetRetainCount
 */
SIScalarRef SIScalarCreateWithDouble(double input_value, SIUnitRef unit);

/**
 * @brief Creates a mutable SIScalar object with a specified double value and SI unit.
 *
 * SIMutableScalarRef inherits from OCTypeRef. This function allocates and returns a new
 * OCType-compatible SIMutableScalar instance, initialized with the provided double value
 * and unit. The instance has an initial retain count of 1 and supports subsequent modification
 * of its internal state.
 *
 * @param input_value The double value for the scalar.
 * @param unit        The SIUnitRef representing the unit. Must not be NULL.
 *
 *
 * @return A new SIMutableScalarRef representing the mutable scalar. The caller owns this reference
 *         and must balance it with OCRelease when it is no longer needed.
 *
 * @see OCRelease, OCRetain, OCTypeGetRetainCount
 */
SIMutableScalarRef SIScalarCreateMutableWithDouble(double input_value, SIUnitRef unit);

/**
 * @brief Creates an immutable SIScalar object with a specified float complex value and SI unit.
 *
 * SIScalarRef inherits from OCTypeRef. This function allocates and returns a new
 * OCType-compatible SIScalar instance, initialized with the provided float complex value
 * (containing real and imaginary components) and unit. The new instance has an initial
 * retain count of 1.
 *
 * @param input_value The float complex value for the scalar.
 * @param unit        The SIUnitRef representing the unit. Must not be NULL.
 *
 *
 * @return A new SIScalarRef representing the complex-valued scalar. The caller owns this reference
 *         and must balance it with OCRelease when it is no longer needed.
 *
 * @see OCRelease, OCRetain, OCTypeGetRetainCount
 */
SIScalarRef SIScalarCreateWithFloatComplex(float complex input_value, SIUnitRef unit);

/**
 * @brief Creates a mutable SIScalar object with a specified float complex value and SI unit.
 *
 * SIMutableScalarRef inherits from OCTypeRef. This function allocates and returns a new
 * OCType-compatible SIMutableScalar instance, initialized with the provided float complex
 * value (real and imaginary parts) and unit. The instance has an initial retain count of 1
 * and supports modification of its complex value.
 *
 * @param input_value The float complex value for the scalar.
 * @param unit        The SIUnitRef representing the unit. Must not be NULL.
 *
 *
 * @return A new SIMutableScalarRef representing the mutable complex-valued scalar. The caller owns
 *         this reference and must balance it with OCRelease when it is no longer needed.
 *
 * @see OCRelease, OCRetain, OCTypeGetRetainCount
 */
SIMutableScalarRef SIScalarCreateMutableWithFloatComplex(float complex input_value, SIUnitRef unit);

/**
 * @brief Creates an immutable SIScalar object with a specified double complex value and SI unit.
 *
 * SIScalarRef inherits from OCTypeRef. This function allocates and returns a new
 * OCType-compatible SIScalar instance, initialized with the provided double complex
 * value (real and imaginary parts) and unit. The new instance has an initial retain count of 1.
 *
 * @param input_value The double complex value for the scalar.
 * @param unit        The SIUnitRef representing the unit. Must not be NULL.
 *
 *
 * @return A new SIScalarRef representing the complex-valued scalar. The caller owns this reference
 *         and must balance it with OCRelease when it is no longer needed.
 *
 * @see OCRelease, OCRetain, OCTypeGetRetainCount
 */
SIScalarRef SIScalarCreateWithDoubleComplex(double complex input_value, SIUnitRef unit);

/**
 * @brief Creates a mutable SIScalar object with a specified double complex value and SI unit.
 *
 * SIMutableScalarRef inherits from OCTypeRef. This function allocates and returns a new
 * OCType-compatible SIMutableScalar instance, initialized with the provided double complex
 * value (real and imaginary parts) and unit. The instance has an initial retain count of 1
 * and supports modification of its complex value.
 *
 * @param input_value The double complex value for the scalar.
 * @param unit        The SIUnitRef representing the unit. Must not be NULL.
 *
 *
 * @return A new SIMutableScalarRef representing the mutable complex-valued scalar. The caller owns
 *         this reference and must balance it with OCRelease when it is no longer needed.
 *
 * @see OCRelease, OCRetain, OCTypeGetRetainCount
 */
SIMutableScalarRef SIScalarCreateMutableWithDoubleComplex(double complex input_value, SIUnitRef unit);

/**
 * @brief Creates an immutable SIScalar object from a single unit name string.
 *
 * SIScalarRef inherits from OCTypeRef. This function parses the provided OCStringRef, which must
 * contain exactly one valid unit symbol or name (e.g., "m", "kg", "s"), and returns a new
 * OCType-compatible SIScalar instance with a value of 1.0 and the corresponding unit. The instance
 * has an initial retain count of 1.
 *
 * Parsing of compound units or expressions is not supported; only single, simple unit names are accepted.
 *
 * @param theString An OCStringRef containing a single unit name or symbol. Must not be NULL.
 *
 *
 * @return A new SIScalarRef representing the unit scalar with value 1.0, or NULL if the string is invalid
 *         or the unit is not found. The caller owns this reference and must balance it with OCRelease when done.
 *
 * @see OCRelease, OCRetain, OCTypeGetRetainCount
 */
SIScalarRef SIScalarCreateWithStringContainingSingleUnitFromLibrary(OCStringRef theString);

#pragma mark Accessors
/**
 * @brief Retrieves the numeric value of a SIScalar instance.
 *
 * SIScalarRef inherits from OCTypeRef. This function returns an __SINumber struct representing
 * the scalar’s current numeric value (float, double, or complex), as stored internally.
 *
 * @param theScalar The SIScalarRef whose value is to be retrieved. Must not be NULL.
 *
 * @thread_safety Safe to call from any thread on distinct instances; concurrent calls on the same
 *                instance are not synchronized.
 *
 * @return An __SINumber containing the scalar’s current value.
 *
 * @see SIScalarSetFloatValue, SIScalarSetDoubleValue
 */
__SINumber SIScalarGetValue(SIScalarRef theScalar);

/**
 * @brief Sets the numeric value of a mutable scalar to a float.
 *
 * SIMutableScalarRef inherits from OCTypeRef. This function updates the scalar’s internal
 * numeric value to the provided float, preserving its associated unit and metadata.
 *
 * @param theScalar The SIMutableScalarRef to modify. Must not be NULL.
 * @param value     The new float value to assign.
 *
 * @thread_safety Safe to call from any thread on distinct instances; concurrent calls on the same
 *                instance are not synchronized.
 *
 * @see SIScalarGetValue
 */
void SIScalarSetFloatValue(SIMutableScalarRef theScalar, float value);

/**
 * @brief Sets the numeric value of a mutable scalar to a double.
 *
 * SIMutableScalarRef inherits from OCTypeRef. This function updates the scalar’s internal
 * numeric value to the provided double, preserving its associated unit and metadata.
 *
 * @param theScalar The SIMutableScalarRef to modify. Must not be NULL.
 * @param value     The new double value to assign.
 *
 * @thread_safety Safe to call from any thread on distinct instances; concurrent calls on the same
 *                instance are not synchronized.
 *
 * @see SIScalarGetValue
 */
void SIScalarSetDoubleValue(SIMutableScalarRef theScalar, double value);


/**
 * @brief Updates the internal float complex value of a mutable scalar.
 *
 * SIMutableScalarRef inherits from OCTypeRef. This function sets the scalar’s current
 * numeric value to the provided float complex (real + imaginary) while preserving its
 * associated SI unit and metadata.
 *
 * @param theScalar The SIMutableScalarRef to modify. Must not be NULL.
 * @param value     The new float complex value to assign.
 *
 * @thread_safety Safe to call from any thread on distinct instances; concurrent calls
 *                on the same instance are not synchronized.
 *
 * @see SIScalarGetValue, OCRelease, OCRetain
 */
void SIScalarSetFloatComplexValue(SIMutableScalarRef theScalar, float complex value);

/**
 * @brief Updates the internal double complex value of a mutable scalar.
 *
 * SIMutableScalarRef inherits from OCTypeRef. This function sets the scalar’s current
 * numeric value to the provided double complex (real + imaginary) while preserving its
 * associated SI unit and metadata.
 *
 * @param theScalar The SIMutableScalarRef to modify. Must not be NULL.
 * @param value     The new double complex value to assign.
 *
 * @thread_safety Safe to call from any thread on distinct instances; concurrent calls
 *                on the same instance are not synchronized.
 *
 * @see SIScalarGetValue, OCRelease, OCRetain
 */
void SIScalarSetDoubleComplexValue(SIMutableScalarRef theScalar, double complex value);

/**
 * @brief Sets the element type indicator for a mutable scalar.
 *
 * SIMutableScalarRef inherits from OCTypeRef. This function configures the scalar’s internal
 * storage type (e.g., float, double, float complex, double complex) as specified by a
 * SINumberType enum, without changing its current numeric value or unit.
 *
 * @param theScalar   The SIMutableScalarRef to modify. Must not be NULL.
 * @param elementType The SINumberType value indicating the desired numeric representation.
 *
 * @thread_safety Safe to call from any thread on distinct instances; concurrent calls
 *                on the same instance are not synchronized.
 *
 * @see OCRelease, OCRetain, SIScalarGetValue
 */
void SIScalarSetElementType(SIMutableScalarRef theScalar, SINumberType elementType);

/**
 * @brief Retrieves the scalar’s value as a native float.
 *
 * SIScalarRef inherits from OCTypeRef. This function converts the scalar’s current
 * numeric value—regardless of its stored type—to a float and returns it. Precision
 * may be lost if the original value is double or complex.
 *
 * @param theScalar The SIScalarRef whose value is to be retrieved. Must not be NULL.
 *
 * @thread_safety Safe to call from any thread on distinct instances; concurrent calls
 *                on the same instance are not synchronized.
 *
 * @return The scalar’s numeric value converted to float.
 *
 * @see SIScalarGetValue, OCRelease, OCRetain
 */
float SIScalarFloatValue(SIScalarRef theScalar);

/**
 * @brief Retrieves the scalar’s value as a native double.
 *
 * SIScalarRef inherits from OCTypeRef. This function converts the scalar’s current
 * numeric value—regardless of its stored type—to a double and returns it. Precision
 * may be lost if the original value is complex.
 *
 * @param theScalar The SIScalarRef whose value is to be retrieved. Must not be NULL.
 *
 * @thread_safety Safe to call from any thread on distinct instances; concurrent calls
 *                on the same instance are not synchronized.
 *
 * @return The scalar’s numeric value converted to double.
 *
 * @see SIScalarGetValue, OCRelease, OCRetain
 */
double SIScalarDoubleValue(SIScalarRef theScalar);

/**
 * @brief Retrieves the scalar’s value as a float complex number.
 *
 * SIScalarRef inherits from OCTypeRef. This function returns the scalar’s current
 * numeric value cast to a float complex, preserving real and imaginary components
 * where possible. Precision may be reduced for double complex originals.
 *
 * @param theScalar The SIScalarRef whose value is to be retrieved. Must not be NULL.
 *
 * @thread_safety Safe to call from any thread on distinct instances; concurrent calls
 *                on the same instance are not synchronized.
 *
 * @return The scalar’s numeric value as a float complex.
 *
 * @see SIScalarGetValue, OCRelease, OCRetain
 */
float complex SIScalarFloatComplexValue(SIScalarRef theScalar);

/**
 * @brief Retrieves the scalar’s value as a double complex number.
 *
 * SIScalarRef inherits from OCTypeRef. This function returns the scalar’s current
 * numeric value cast to a double complex, preserving real and imaginary components.
 *
 * @param theScalar The SIScalarRef whose value is to be retrieved. Must not be NULL.
 *
 * @thread_safety Safe to call from any thread on distinct instances; concurrent calls
 *                on the same instance are not synchronized.
 *
 * @return The scalar’s numeric value as a double complex.
 *
 * @see SIScalarGetValue, OCRelease, OCRetain
 */
double complex SIScalarDoubleComplexValue(SIScalarRef theScalar);

/**
 * @brief Computes the magnitude (absolute value) of the scalar.
 *
 * SIScalarRef inherits from OCTypeRef. For real-valued scalars, this returns the
 * absolute value. For complex-valued scalars, it returns √(real² + imag²).
 *
 * @param theScalar The SIScalarRef whose magnitude is to be calculated. Must not be NULL.
 *
 * @thread_safety Safe to call from any thread on distinct instances; concurrent calls
 *                on the same instance are not synchronized.
 *
 * @return The magnitude of the scalar as a double.
 *
 * @see SIScalarArgumentValue, SIScalarGetValue
 */
double SIScalarMagnitudeValue(SIScalarRef theScalar);

/**
 * @brief Computes the argument (phase angle) of the scalar in radians.
 *
 * SIScalarRef inherits from OCTypeRef. For real-valued scalars, this returns 0 for
 * positive values and π for negative values. For complex-valued scalars, it returns
 * atan2(imag, real).
 *
 * @param theScalar The SIScalarRef whose argument is to be calculated. Must not be NULL.
 *
 * @thread_safety Safe to call from any thread on distinct instances; concurrent calls
 *                on the same instance are not synchronized.
 *
 * @return The argument (phase) of the scalar in radians as a double.
 *
 * @see SIScalarMagnitudeValue, SIScalarGetValue
 */
double SIScalarArgumentValue(SIScalarRef theScalar);

/**
 * @brief Converts the scalar’s value to the specified unit and returns it as a float.
 *
 * SIScalarRef inherits from OCTypeRef. This function attempts to convert the scalar’s stored
 * numeric value into the provided SIUnitRef and returns the result as a float. Use the
 * success flag to determine whether the conversion was valid (e.g., units are compatible).
 *
 * @param theScalar The SIScalarRef whose value is to be converted. Must not be NULL.
 * @param unit      The SIUnitRef representing the target unit. Must not be NULL.
 * @param success   A pointer to a bool that will be set to true if the conversion succeeded,
 *                  or false if the units are incompatible or an error occurred.
 *
 * @thread_safety Safe to call from any thread on distinct instances; concurrent calls on the same
 *                instance are not synchronized.
 *
 * @return The converted value as a float. If conversion fails, returns 0.0 and *success is false.
 *
 * @see SIScalarDoubleValueInUnit, SIScalarGetValue
 */
float SIScalarFloatValueInUnit(SIScalarRef theScalar, SIUnitRef unit, bool *success);

/**
 * @brief Converts the scalar’s value to the specified unit and returns it as a double.
 *
 * SIScalarRef inherits from OCTypeRef. This function attempts to convert the scalar’s stored
 * numeric value into the provided SIUnitRef and returns the result as a double. Use the
 * success flag to determine whether the conversion was valid (e.g., units are compatible).
 *
 * @param theScalar The SIScalarRef whose value is to be converted. Must not be NULL.
 * @param unit      The SIUnitRef representing the target unit. Must not be NULL.
 * @param success   A pointer to a bool that will be set to true if the conversion succeeded,
 *                  or false if the units are incompatible or an error occurred.
 *
 * @thread_safety Safe to call from any thread on distinct instances; concurrent calls on the same
 *                instance are not synchronized.
 *
 * @return The converted value as a double. If conversion fails, returns 0.0 and *success is false.
 *
 * @see SIScalarFloatValueInUnit, SIScalarGetValue
 */
double SIScalarDoubleValueInUnit(SIScalarRef theScalar, SIUnitRef unit, bool *success);

/**
 * @brief Converts the scalar’s value to the specified unit and returns it as a float complex.
 *
 * SIScalarRef inherits from OCTypeRef. This function attempts to convert the scalar’s stored
 * numeric value into the provided SIUnitRef and returns the result as a float complex number,
 * preserving real and imaginary components. Use the success flag to determine whether the
 * conversion was valid.
 *
 * @param theScalar The SIScalarRef whose value is to be converted. Must not be NULL.
 * @param unit      The SIUnitRef representing the target unit. Must not be NULL.
 * @param success   A pointer to a bool that will be set to true if the conversion succeeded,
 *                  or false if the units are incompatible or an error occurred.
 *
 * @thread_safety Safe to call from any thread on distinct instances; concurrent calls on the same
 *                instance are not synchronized.
 *
 * @return The converted value as a float complex. If conversion fails, returns 0.0 + 0.0i
 *         and *success is false.
 *
 * @see SIScalarDoubleComplexValueInUnit, SIScalarGetValue
 */
float complex SIScalarFloatComplexValueInUnit(SIScalarRef theScalar, SIUnitRef unit, bool *success);

/**
 * @brief Converts the scalar’s value to the specified unit and returns it as a double complex.
 *
 * SIScalarRef inherits from OCTypeRef. This function attempts to convert the scalar’s stored
 * numeric value into the provided SIUnitRef and returns the result as a double complex number,
 * preserving real and imaginary components. Use the success flag to determine whether the
 * conversion was valid.
 *
 * @param theScalar The SIScalarRef whose value is to be converted. Must not be NULL.
 * @param unit      The SIUnitRef representing the target unit. Must not be NULL.
 * @param success   A pointer to a bool that will be set to true if the conversion succeeded,
 *                  or false if the units are incompatible or an error occurred.
 *
 * @thread_safety Safe to call from any thread on distinct instances; concurrent calls on the same
 *                instance are not synchronized.
 *
 * @return The converted value as a double complex. If conversion fails, returns 0.0 + 0.0i
 *         and *success is false.
 *
 * @see SIScalarFloatComplexValueInUnit, SIScalarGetValue
 */
double complex SIScalarDoubleComplexValueInUnit(SIScalarRef theScalar, SIUnitRef unit, bool *success);

/**
 * @brief Converts the scalar’s value into its coherent SI unit and returns it as a float.
 *
 * SIScalarRef inherits from OCTypeRef. This function determines the scalar’s value in the
 * base (coherent) SI unit corresponding to its current unit (e.g., converts km to m),
 * performs the conversion, and returns the result as a float. Precision may be lost
 * if the original value is double or complex.
 *
 * @param theScalar The SIScalarRef whose value is to be converted. Must not be NULL.
 *
 * @thread_safety Safe to call from any thread on distinct instances; concurrent calls
 *                on the same instance are not synchronized.
 *
 * @return The scalar’s value converted to its coherent SI unit, as a float.
 *
 * @see SIScalarDoubleValueInCoherentUnit, SIScalarGetValue
 */
float SIScalarFloatValueInCoherentUnit(SIScalarRef theScalar);

/**
 * @brief Converts the scalar’s value into its coherent SI unit and returns it as a double.
 *
 * SIScalarRef inherits from OCTypeRef. This function determines the scalar’s value in the
 * base (coherent) SI unit corresponding to its current unit, performs the conversion,
 * and returns the result as a double. Precision may be lost if the original value is complex.
 *
 * @param theScalar The SIScalarRef whose value is to be converted. Must not be NULL.
 *
 * @thread_safety Safe to call from any thread on distinct instances; concurrent calls
 *                on the same instance are not synchronized.
 *
 * @return The scalar’s value converted to its coherent SI unit, as a double.
 *
 * @see SIScalarFloatValueInCoherentUnit, SIScalarGetValue
 */
double SIScalarDoubleValueInCoherentUnit(SIScalarRef theScalar);

/**
 * @brief Converts the scalar’s value into its coherent SI unit and returns it as a float complex.
 *
 * SIScalarRef inherits from OCTypeRef. This function determines the scalar’s value in the
 * base (coherent) SI unit corresponding to its current unit, performs the conversion,
 * and returns the result as a float complex (real + imaginary). Precision may be reduced
 * for double complex originals.
 *
 * @param theScalar The SIScalarRef whose value is to be converted. Must not be NULL.
 *
 * @thread_safety Safe to call from any thread on distinct instances; concurrent calls
 *                on the same instance are not synchronized.
 *
 * @return The scalar’s value converted to its coherent SI unit, as a float complex.
 *
 * @see SIScalarDoubleComplexValueInCoherentUnit, SIScalarGetValue
 */
float complex SIScalarFloatComplexValueInCoherentUnit(SIScalarRef theScalar);

/**
 * @brief Converts the scalar’s value into its coherent SI unit and returns it as a double complex.
 *
 * SIScalarRef inherits from OCTypeRef. This function determines the scalar’s value in the
 * base (coherent) SI unit corresponding to its current unit, performs the conversion,
 * and returns the result as a double complex (real + imaginary).
 *
 * @param theScalar The SIScalarRef whose value is to be converted. Must not be NULL.
 *
 * @thread_safety Safe to call from any thread on distinct instances; concurrent calls
 *                on the same instance are not synchronized.
 *
 * @return The scalar’s value converted to its coherent SI unit, as a double complex.
 *
 * @see SIScalarFloatComplexValueInCoherentUnit, SIScalarGetValue
 */
double complex SIScalarDoubleComplexValueInCoherentUnit(SIScalarRef theScalar);


#pragma mark Operations

/**
 * @brief Creates a new SIScalar by converting the numeric representation to a specified element type.
 *
 * SIScalarRef inherits from OCTypeRef. This function allocates and returns a new OCType-compatible
 * SIScalar instance whose internal storage type is converted to the given SINumberType (e.g., float,
 * double, float complex, double complex). Conversions that reduce precision or drop imaginary components
 * are performed as follows:
 *   - Double → Float: precision may be lost.
 *   - Complex → Real: imaginary component is discarded.
 *
 * @param theScalar    The SIScalarRef to convert. Must not be NULL.
 * @param elementType  The SINumberType indicating the target numeric representation.
 *
 *
 * @return A new SIScalarRef representing the converted scalar. The caller owns this reference and must
 *         balance it with OCRelease when it is no longer needed.
 *
 * @see OCRelease, OCRetain, SIScalarGetValue
 */
SIScalarRef SIScalarCreateByConvertingToNumberType(SIScalarRef theScalar, SINumberType elementType);

/**
 * @brief Replaces the numeric value of a mutable scalar with one of its complex components.
 *
 * SIMutableScalarRef inherits from OCTypeRef. This function extracts the specified component from the
 * scalar’s current complex value (real, imaginary, magnitude, or argument) and updates the scalar’s
 * internal storage to that real-valued component. The unit remains unchanged.
 *
 * @param theScalar The SIMutableScalarRef to modify. Must not be NULL.
 * @param part      The complexPart enum value specifying which component to extract:
 *                  kSIRealPart, kSIImaginaryPart, kSIMagnitudePart, or kSIArgumentPart.
 *
 *
 * @return true if the extraction and update succeeded; false if the scalar’s value is not complex
 *         or the specified part could not be obtained.
 *
 * @see SIScalarCreateByTakingComplexPart
 */
bool SIScalarTakeComplexPart(SIMutableScalarRef theScalar, complexPart part);

/**
 * @brief Creates a new SIScalar from a specified complex component of an existing scalar.
 *
 * SIScalarRef inherits from OCTypeRef. This function allocates and returns a new OCType-compatible
 * SIScalar instance whose numeric value is the selected component (real, imaginary, magnitude, or
 * argument) of the original scalar’s complex value. The unit of the new scalar matches the original.
 *
 * @param theScalar The SIScalarRef from which to extract the component. Must not be NULL.
 * @param part      The complexPart enum value specifying which component to extract:
 *                  kSIRealPart, kSIImaginaryPart, kSIMagnitudePart, or kSIArgumentPart.
 *
 *
 * @return A new SIScalarRef representing the extracted component. The caller owns this reference and
 *         must balance it with OCRelease when it is no longer needed.
 *
 * @see OCRelease, OCRetain, SIScalarTakeComplexPart
 */
SIScalarRef SIScalarCreateByTakingComplexPart(SIScalarRef theScalar, complexPart part);


/**
 * @brief Parses a numeric string with unit and creates a SIScalar.
 *
 * SIScalarRef inherits from OCTypeRef. This function parses the provided OCStringRef, which
 * should contain a numeric value followed by a valid SI unit (e.g., "9.81 m/s^2"). On success,
 * it returns a new OCType-compatible SIScalar instance with the parsed value and unit.
 *
 * @param string The OCStringRef containing the textual representation of the scalar. Must not be NULL.
 * @param error  A pointer to an OCStringRef that, on failure, will be set to a descriptive error
 *               message. The caller is responsible for releasing this string. May be NULL if
 *               the caller does not require error details.
 *
 *
 * @return A new SIScalarRef representing the parsed value and unit, or NULL if parsing fails.
 *         On failure, *error (if provided) will contain the error description.
 *
 * @see OCRelease, OCRetain, OCTypeGetRetainCount
 */
SIScalarRef SIScalarCreateWithOCString(OCStringRef string, OCStringRef *error);

/**
 * @brief Creates a new SIScalar with its unit exponents reduced to lowest integers.
 *
 * SIScalarRef inherits from OCTypeRef. This function analyzes the unit of the input scalar,
 * reduces its numerator and denominator exponents to their simplest integer ratio (e.g., m^2·s^-2
 * remains unchanged, but kg·m·s^-1 becomes kg·m/s), and returns a new OCType-compatible SIScalar
 * instance with the same value and the simplified unit.
 *
 * @param theScalar The SIScalarRef whose unit is to be reduced. Must not be NULL.
 *
 *
 * @return A new SIScalarRef with reduced unit exponents. The caller owns this reference and must
 *         balance it with OCRelease when it is no longer needed.
 *
 * @see OCRelease, OCRetain, SIScalarReduceUnit
 */
SIScalarRef SIScalarCreateByReducingUnit(SIScalarRef theScalar);

/**
 * @brief Reduces a mutable scalar’s unit exponents in place to lowest integers.
 *
 * SIMutableScalarRef inherits from OCTypeRef. This function simplifies the unit of the provided
 * mutable scalar by reducing its numerator and denominator exponents to their smallest integer
 * ratio. The scalar’s numeric value and retain count are unchanged.
 *
 * @param theScalar The SIMutableScalarRef to modify. Must not be NULL.
 *
 *
 * @return true if the unit was successfully reduced (or was already in simplest form), false on error.
 *
 * @see SIScalarCreateByReducingUnit
 */
bool SIScalarReduceUnit(SIMutableScalarRef theScalar);

/**
 * @brief Converts a mutable scalar’s value and unit to another compatible unit.
 *
 * SIMutableScalarRef inherits from OCTypeRef. This function attempts to convert the scalar’s current
 * numeric value into the specified SIUnitRef of the same dimensionality (e.g., m → cm). On failure
 * (e.g., incompatible units), no change is made and an error message is provided.
 *
 * @param theScalar The SIMutableScalarRef to convert. Must not be NULL.
 * @param unit      The SIUnitRef representing the target unit. Must not be NULL.
 * @param error     A pointer to an OCStringRef that, on failure, will be set to a descriptive error
 *                  message. The caller is responsible for releasing this string. May be NULL if
 *                  the caller does not require error details.
 *
 *
 * @return true if conversion succeeded, false otherwise. On failure, *error (if provided) will contain
 *         an explanation.
 *
 * @see OCRelease, OCRetain, OCTypeGetRetainCount
 */
bool SIScalarConvertToUnit(SIMutableScalarRef theScalar, SIUnitRef unit, OCStringRef *error);


#pragma mark Operations

/**
 * @brief Creates a new SIScalar by converting to another unit of the same dimensionality.
 *
 * SIScalarRef inherits from OCTypeRef. Allocates and returns a new OCType-compatible SIScalar
 * instance whose numeric value and unit are converted to the specified unit. If the units are
 * incompatible or conversion fails, no instance is created.
 *
 * @param theScalar The SIScalarRef to convert. Must not be NULL.
 * @param unit      The target SIUnitRef of the same dimensionality. Must not be NULL.
 * @param error     A pointer to an OCStringRef that, on failure, will be set to a descriptive error
 *                  message. The caller is responsible for releasing this string. May be NULL.
 *
 *
 * @return A new SIScalarRef representing the converted scalar, or NULL if conversion fails.
 *         On failure, *error (if provided) will contain the error description.
 *
 * @see OCRelease, OCRetain, SIScalarConvertToUnit
 */
SIScalarRef SIScalarCreateByConvertingToUnit(SIScalarRef theScalar,
                                              SIUnitRef unit,
                                              OCStringRef *error);

/**
 * @brief Converts a mutable scalar to its coherent SI unit in place.
 *
 * SIMutableScalarRef inherits from OCTypeRef. Updates the scalar’s unit and numeric value to the
 * corresponding coherent SI unit (e.g., km → m). If conversion fails, the scalar is left unchanged.
 *
 * @param theScalar The SIMutableScalarRef to convert. Must not be NULL.
 * @param error     A pointer to an OCStringRef that, on failure, will be set to a descriptive error
 *                  message. The caller is responsible for releasing this string. May be NULL.
 *
 *
 * @return true if conversion succeeded; false otherwise. On failure, *error (if provided) will contain
 *         the error description.
 *
 * @see SIScalarCreateByConvertingToCoherentUnit
 */
bool SIScalarConvertToCoherentUnit(SIMutableScalarRef theScalar,
                                   OCStringRef *error);

/**
 * @brief Creates a new SIScalar by converting to its coherent SI unit.
 *
 * SIScalarRef inherits from OCTypeRef. Allocates and returns a new OCType-compatible SIScalar
 * instance whose unit and value are converted to the base SI unit (e.g., km → m). If conversion
 * fails, no instance is created.
 *
 * @param theScalar The SIScalarRef to convert. Must not be NULL.
 * @param error     A pointer to an OCStringRef that, on failure, will be set to a descriptive error
 *                  message. The caller is responsible for releasing this string. May be NULL.
 *
 *
 * @return A new SIScalarRef representing the converted scalar, or NULL if conversion fails.
 *         On failure, *error (if provided) will contain the error description.
 *
 * @see OCRelease, OCRetain, SIScalarConvertToCoherentUnit
 */
SIScalarRef SIScalarCreateByConvertingToCoherentUnit(SIScalarRef theScalar,
                                                     OCStringRef *error);

/**
 * @brief Creates a new SIScalar by adding two scalars of the same dimensionality.
 *
 * SIScalarRef inherits from OCTypeRef. Allocates and returns a new SIScalar instance whose
 * numeric value is the sum of input1 and input2. If the units are incompatible or addition
 * fails, no instance is created.
 *
 * @param input1 The first SIScalarRef operand. Must not be NULL.
 * @param input2 The second SIScalarRef operand. Must not be NULL.
 * @param error  A pointer to an OCStringRef that, on failure, will be set to a descriptive error
 *               message. The caller is responsible for releasing this string. May be NULL.
 *
 *
 * @return A new SIScalarRef representing the sum, or NULL if addition fails.
 *         On failure, *error (if provided) will contain the error description.
 *
 * @see OCRelease, OCRetain, SIScalarAdd
 */
SIScalarRef SIScalarCreateByAdding(SIScalarRef input1,
                                   SIScalarRef input2,
                                   OCStringRef *error);

/**
 * @brief Adds another scalar into a mutable scalar in place.
 *
 * SIMutableScalarRef inherits from OCTypeRef. Updates target by adding the numeric value of
 * input2 to it. Units must be compatible; on failure, target remains unchanged.
 *
 * @param target The SIMutableScalarRef to be modified. Must not be NULL.
 * @param input2 The SIScalarRef whose value will be added. Must not be NULL.
 * @param error  A pointer to an OCStringRef that, on failure, will be set to a descriptive error
 *               message. The caller is responsible for releasing this string. May be NULL.
 *
 *
 * @return true if addition succeeded; false otherwise. On failure, *error (if provided) will contain
 *         the error description.
 *
 * @see SIScalarCreateByAdding
 */
bool SIScalarAdd(SIMutableScalarRef target,
                 SIScalarRef input2,
                 OCStringRef *error);

/**
 * @brief Creates a new SIScalar by subtracting one scalar from another.
 *
 * SIScalarRef inherits from OCTypeRef. Allocates and returns a new OCType-compatible SIScalar
 * instance whose numeric value equals input1 − input2, with units verified for compatibility.
 * If units are incompatible or subtraction fails, no instance is created.
 *
 * @param input1 The minuend SIScalarRef. Must not be NULL.
 * @param input2 The subtrahend SIScalarRef. Must not be NULL.
 * @param error  A pointer to an OCStringRef that, on failure, will be set to a descriptive error
 *               message. The caller is responsible for releasing this string. May be NULL.
 *
 *
 * @return A new SIScalarRef representing the difference, or NULL if subtraction fails.
 *         On failure, *error (if provided) will contain the error description.
 *
 * @see OCRelease, OCRetain, SIScalarSubtract
 */
SIScalarRef SIScalarCreateBySubtracting(SIScalarRef input1,
                                        SIScalarRef input2,
                                        OCStringRef *error);

/**
 * @brief Subtracts a scalar from a mutable scalar in place.
 *
 * SIMutableScalarRef inherits from OCTypeRef. Updates target by computing target − input2.
 * Units must be compatible; on failure, target remains unchanged.
 *
 * @param target The SIMutableScalarRef to modify. Must not be NULL.
 * @param input2 The SIScalarRef to subtract. Must not be NULL.
 * @param error  A pointer to an OCStringRef that, on failure, will be set to a descriptive error
 *               message. The caller is responsible for releasing this string. May be NULL.
 *
 *
 * @return true if subtraction succeeded; false otherwise. On failure, *error (if provided) will contain
 *         the error description.
 *
 * @see SIScalarCreateBySubtracting
 */
bool SIScalarSubtract(SIMutableScalarRef target,
                      SIScalarRef input2,
                      OCStringRef *error);

/**
 * @brief Creates a new SIScalar by multiplying two scalars without simplifying the unit.
 *
 * SIScalarRef inherits from OCTypeRef. Allocates and returns a new OCType-compatible SIScalar
 * instance whose numeric value equals input1 × input2, and whose unit is the product of the two
 * units, without reducing exponent factors. If units are incompatible or multiplication fails,
 * no instance is created.
 *
 * @param input1 The first SIScalarRef operand. Must not be NULL.
 * @param input2 The second SIScalarRef operand. Must not be NULL.
 * @param error  A pointer to an OCStringRef that, on failure, will be set to a descriptive error
 *               message. The caller is responsible for releasing this string. May be NULL.
 *
 *
 * @return A new SIScalarRef representing the product, or NULL if multiplication fails.
 *         On failure, *error (if provided) will contain the error description.
 *
 * @see OCRelease, OCRetain, SIScalarMultiplyWithoutReducingUnit
 */
SIScalarRef SIScalarCreateByMultiplyingWithoutReducingUnit(SIScalarRef input1,
                                                           SIScalarRef input2,
                                                           OCStringRef *error);

/**
 * @brief Multiplies a mutable scalar by another scalar without simplifying the unit.
 *
 * SIMutableScalarRef inherits from OCTypeRef. Updates target by computing target × input2.
 * The resulting unit is the product of the two units, without reducing exponent factors.
 * On failure, target remains unchanged.
 *
 * @param target The SIMutableScalarRef to modify. Must not be NULL.
 * @param input2 The SIScalarRef multiplier. Must not be NULL.
 * @param error  A pointer to an OCStringRef that, on failure, will be set to a descriptive error
 *               message. The caller is responsible for releasing this string. May be NULL.
 *
 *
 * @return true if multiplication succeeded; false otherwise. On failure, *error (if provided) will contain
 *         the error description.
 *
 * @see SIScalarCreateByMultiplyingWithoutReducingUnit
 */
bool SIScalarMultiplyWithoutReducingUnit(SIMutableScalarRef target,
                                         SIScalarRef input2,
                                         OCStringRef *error);

/**
 * @brief Creates a new SIScalar by multiplying two scalars.
 *
 * SIScalarRef inherits from OCTypeRef. Allocates and returns a new OCType-compatible SIScalar
 * instance whose numeric value equals input1 × input2, with units verified for compatibility
 * and simplified according to canonical rules. If units are incompatible or multiplication fails,
 * no instance is created.
 *
 * @param input1 The first SIScalarRef operand. Must not be NULL.
 * @param input2 The second SIScalarRef operand. Must not be NULL.
 * @param error  A pointer to an OCStringRef that, on failure, will be set to a descriptive error
 *               message. The caller is responsible for releasing this string. May be NULL.
 *
 *
 * @return A new SIScalarRef representing the product, or NULL if multiplication fails.
 *         On failure, *error (if provided) will contain the error description.
 *
 * @see OCRelease, OCRetain, SIScalarMultiply
 */
SIScalarRef SIScalarCreateByMultiplying(SIScalarRef input1,
                                        SIScalarRef input2,
                                        OCStringRef *error);

/**
 * @brief Multiplies a mutable scalar by another scalar in place.
 *
 * SIMutableScalarRef inherits from OCTypeRef. Updates target by computing target × input2,
 * simplifying the resulting unit according to canonical rules. On failure, target remains unchanged.
 *
 * @param target The SIMutableScalarRef to modify. Must not be NULL.
 * @param input2 The SIScalarRef multiplier. Must not be NULL.
 * @param error  A pointer to an OCStringRef that, on failure, will be set to a descriptive error
 *               message. The caller is responsible for releasing this string. May be NULL.
 *
 *
 * @return true if multiplication succeeded; false otherwise. On failure, *error (if provided) will
 *         contain the error description.
 *
 * @see SIScalarCreateByMultiplying
 */
bool SIScalarMultiply(SIMutableScalarRef target,
                      SIScalarRef input2,
                      OCStringRef *error);

/**
 * @brief Creates a new SIScalar by dividing two scalars without simplifying the unit.
 *
 * SIScalarRef inherits from OCTypeRef. Allocates and returns a new OCType-compatible SIScalar
 * instance whose numeric value equals input1 ÷ input2, with the unit formed by dividing the
 * two units without reducing exponent factors. If units are incompatible or division fails,
 * no instance is created.
 *
 * @param input1 The numerator SIScalarRef. Must not be NULL.
 * @param input2 The denominator SIScalarRef. Must not be NULL.
 * @param error  A pointer to an OCStringRef that, on failure, will be set to a descriptive error
 *               message. The caller is responsible for releasing this string. May be NULL.
 *
 *
 * @return A new SIScalarRef representing the quotient, or NULL if division fails.
 *         On failure, *error (if provided) will contain the error description.
 *
 * @see OCRelease, OCRetain, SIScalarDivideWithoutReducingUnit
 */
SIScalarRef SIScalarCreateByDividingWithoutReducingUnit(SIScalarRef input1,
                                                        SIScalarRef input2,
                                                        OCStringRef *error);

/**
 * @brief Divides a mutable scalar by another scalar in place without simplifying the unit.
 *
 * SIMutableScalarRef inherits from OCTypeRef. Updates target by computing target ÷ input2,
 * forming the resulting unit by dividing the two units without reducing exponent factors.
 * On failure, target remains unchanged.
 *
 * @param target The SIMutableScalarRef to modify. Must not be NULL.
 * @param input2 The SIScalarRef divisor. Must not be NULL.
 * @param error  A pointer to an OCStringRef that, on failure, will be set to a descriptive error
 *               message. The caller is responsible for releasing this string. May be NULL.
 *
 *
 * @return true if division succeeded; false otherwise. On failure, *error (if provided) will contain
 *         the error description.
 *
 * @see SIScalarCreateByDividingWithoutReducingUnit
 */
bool SIScalarDivideWithoutReducingUnit(SIMutableScalarRef target,
                                       SIScalarRef input2,
                                       OCStringRef *error);

/**
 * @brief Creates a new SIScalar by dividing two scalars.
 *
 * SIScalarRef inherits from OCTypeRef. Allocates and returns a new OCType-compatible SIScalar
 * instance whose numeric value equals input1 ÷ input2, with units verified for compatibility
 * and simplified according to canonical rules. If units are incompatible or division fails,
 * no instance is created.
 *
 * @param input1 The numerator SIScalarRef. Must not be NULL.
 * @param input2 The denominator SIScalarRef. Must not be NULL.
 * @param error  A pointer to an OCStringRef that, on failure, will be set to a descriptive error
 *               message. The caller is responsible for releasing this string. May be NULL.
 *
 *
 * @return A new SIScalarRef representing the quotient, or NULL if division fails.
 *         On failure, *error (if provided) will contain the error description.
 *
 * @see OCRelease, OCRetain, SIScalarDivideWithoutReducingUnit
 */
SIScalarRef SIScalarCreateByDividing(SIScalarRef input1,
                                     SIScalarRef input2,
                                     OCStringRef *error);

/**
 * @brief Divides a mutable scalar by another scalar in place.
 *
 * SIMutableScalarRef inherits from OCTypeRef. Updates target by computing target ÷ input2,
 * simplifying the resulting unit according to canonical rules. On failure, target remains unchanged.
 *
 * @param target The SIMutableScalarRef to modify. Must not be NULL.
 * @param input2 The SIScalarRef divisor. Must not be NULL.
 * @param error  A pointer to an OCStringRef that, on failure, will be set to a descriptive error
 *               message. The caller is responsible for releasing this string. May be NULL.
 *
 *
 * @return true if division succeeded; false otherwise. On failure, *error (if provided) will contain
 *         the error description.
 *
 * @see SIScalarCreateByDividing, SIScalarDivideWithoutReducingUnit
 */
bool SIScalarDivide(SIMutableScalarRef target,
                    SIScalarRef input2,
                    OCStringRef *error);

/**
 * @brief Creates a new SIScalar by raising a scalar to a power without simplifying the unit.
 *
 * SIScalarRef inherits from OCTypeRef. Allocates and returns a new OCType-compatible SIScalar
 * instance whose numeric value equals theScalar^power, with the unit’s exponents each multiplied
 * by power, without reducing exponent factors. If the operation fails, no instance is created.
 *
 * @param theScalar The SIScalarRef to raise. Must not be NULL.
 * @param power     The integer power to apply (as a double). Must be finite.
 * @param error     A pointer to an OCStringRef that, on failure, will be set to a descriptive error
 *                  message. The caller is responsible for releasing this string. May be NULL.
 *
 *
 * @return A new SIScalarRef representing the result, or NULL if the operation fails.
 *         On failure, *error (if provided) will contain the error description.
 *
 * @see SIScalarRaiseToAPowerWithoutReducingUnit
 */
SIScalarRef SIScalarCreateByRaisingToAPowerWithoutReducingUnit(SIScalarRef theScalar,
                                                               double power,
                                                               OCStringRef *error);

/**
 * @brief Raises a mutable scalar to a power without simplifying the unit in place.
 *
 * SIMutableScalarRef inherits from OCTypeRef. Updates theScalar by computing theScalar^power,
 * multiplying each unit exponent by power without reducing exponent factors. On failure,
 * theScalar remains unchanged.
 *
 * @param theScalar The SIMutableScalarRef to modify. Must not be NULL.
 * @param power     The integer power to apply (as a double). Must be finite.
 * @param error     A pointer to an OCStringRef that, on failure, will be set to a descriptive error
 *                  message. The caller is responsible for releasing this string. May be NULL.
 *
 *
 * @return true if the operation succeeded; false otherwise. On failure, *error (if provided)
 *         will contain the error description.
 *
 * @see SIScalarCreateByRaisingToAPowerWithoutReducingUnit
 */
bool SIScalarRaiseToAPowerWithoutReducingUnit(SIMutableScalarRef theScalar,
                                              double power,
                                              OCStringRef *error);

/**
 * @brief Creates a new SIScalar by raising a scalar to a power.
 *
 * SIScalarRef inherits from OCTypeRef. Allocates and returns a new OCType-compatible SIScalar
 * instance whose numeric value equals theScalar^power, and whose unit is simplified to its
 * canonical form after exponentiation. If the operation fails, no instance is created.
 *
 * @param theScalar The SIScalarRef to raise. Must not be NULL.
 * @param power     The integer power to apply (as a double). Must be finite.
 * @param error     A pointer to an OCStringRef that, on failure, will be set to a descriptive error
 *                  message. The caller is responsible for releasing this string. May be NULL.
 *
 *
 * @return A new SIScalarRef representing the result, or NULL if the operation fails.
 *         On failure, *error (if provided) will contain the error description.
 *
 * @see SIScalarRaiseToAPower
 */
SIScalarRef SIScalarCreateByRaisingToAPower(SIScalarRef theScalar,
                                            double power,
                                            OCStringRef *error);

/**
 * @brief Raises a mutable scalar to a power in place.
 *
 * SIMutableScalarRef inherits from OCTypeRef. Updates theScalar by computing theScalar^power,
 * then simplifies its unit to canonical form. On failure, theScalar remains unchanged.
 *
 * @param theScalar The SIMutableScalarRef to modify. Must not be NULL.
 * @param power     The integer power to apply (as a double). Must be finite.
 * @param error     A pointer to an OCStringRef that, on failure, will be set to a descriptive error
 *                  message. The caller is responsible for releasing this string. May be NULL.
 *
 *
 * @return true if the operation succeeded; false otherwise. On failure, *error (if provided)
 *         will contain the error description.
 *
 * @see SIScalarCreateByRaisingToAPower, SIScalarRaiseToAPowerWithoutReducingUnit
 */
bool SIScalarRaiseToAPower(SIMutableScalarRef theScalar,
                           double power,
                           OCStringRef *error);

/**
 * @brief Creates a new SIScalar by taking the absolute value of another scalar.
 *
 * SIScalarRef inherits from OCTypeRef. Allocates and returns a new OCType-compatible
 * SIScalar instance whose numeric value is the absolute value of theScalar. The unit
 * remains unchanged. If the operation fails (e.g., non-numeric value), no instance is created.
 *
 * @param theScalar The SIScalarRef whose absolute value is to be taken. Must not be NULL.
 * @param error     A pointer to an OCStringRef that, on failure, will be set to a descriptive error
 *                  message. The caller is responsible for releasing this string. May be NULL.
 *
 *
 * @return A new SIScalarRef representing the absolute value, or NULL if the operation fails.
 *         On failure, *error (if provided) will contain the error description.
 *
 * @see SIScalarTakeAbsoluteValue
 */
SIScalarRef SIScalarCreateByTakingAbsoluteValue(SIScalarRef theScalar,
                                                OCStringRef *error);

/**
 * @brief Replaces a mutable scalar’s value with its absolute value in place.
 *
 * SIMutableScalarRef inherits from OCTypeRef. Updates theScalar by setting its numeric value
 * to |value|. The unit remains unchanged. On failure, theScalar remains unchanged.
 *
 * @param theScalar The SIMutableScalarRef to modify. Must not be NULL.
 * @param error     A pointer to an OCStringRef that, on failure, will be set to a descriptive error
 *                  message. The caller is responsible for releasing this string. May be NULL.
 *
 *
 * @return true if the operation succeeded; false otherwise.
 *
 * @see SIScalarCreateByTakingAbsoluteValue
 */
bool SIScalarTakeAbsoluteValue(SIMutableScalarRef theScalar,
                               OCStringRef *error);

/**
 * @brief Creates a new SIScalar by applying the Gamma function without simplifying the unit.
 *
 * SIScalarRef inherits from OCTypeRef. Allocates and returns a new OCType-compatible SIScalar
 * instance whose numeric value is Γ(value) for theScalar. The unit’s exponents are each
 * multiplied by (value − 1), but not reduced. If the unit is incompatible or the operation fails,
 * no instance is created.
 *
 * @param theScalar The SIScalarRef to process. Must not be NULL.
 * @param error     A pointer to an OCStringRef that, on failure, will be set to a descriptive error
 *                  message. The caller is responsible for releasing this string. May be NULL.
 *
 *
 * @return A new SIScalarRef representing the Gamma result, or NULL if the operation fails.
 *         On failure, *error (if provided) will contain the error description.
 *
 * @see OCRelease, OCRetain
 */
SIScalarRef SIScalarCreateByGammaFunctionWithoutReducingUnit(SIScalarRef theScalar,
                                                             OCStringRef *error);

/**
 * @brief Creates a new SIScalar by taking the Nth root of another scalar.
 *
 * SIScalarRef inherits from OCTypeRef. Allocates and returns a new OCType-compatible SIScalar
 * instance whose numeric value is theNthRoot(value) (i.e., value^(1/root)). The unit’s exponents
 * are each divided by root and simplified to canonical form. If the operation fails, no instance
 * is created.
 *
 * @param theScalar The SIScalarRef to process. Must not be NULL.
 * @param root      The integer root to apply (must be ≥1).
 * @param error     A pointer to an OCStringRef that, on failure, will be set to a descriptive error
 *                  message. The caller is responsible for releasing this string. May be NULL.
 *
 *
 * @return A new SIScalarRef representing the Nth root, or NULL if the operation fails.
 *         On failure, *error (if provided) will contain the error description.
 *
 * @see SIScalarTakeNthRoot
 */
SIScalarRef SIScalarCreateByTakingNthRoot(SIScalarRef theScalar,
                                          uint8_t root,
                                          OCStringRef *error);

/**
 * @brief Replaces a mutable scalar’s value with its Nth root in place.
 *
 * SIMutableScalarRef inherits from OCTypeRef. Updates theScalar by setting its numeric value
 * to value^(1/root) and simplifying its unit’s exponents by dividing by root. On failure,
 * theScalar remains unchanged.
 *
 * @param theScalar The SIMutableScalarRef to modify. Must not be NULL.
 * @param root      The integer root to apply (must be ≥1).
 * @param error     A pointer to an OCStringRef that, on failure, will be set to a descriptive error
 *                  message. The caller is responsible for releasing this string. May be NULL.
 *
 *
 * @return true if the operation succeeded; false otherwise.
 *
 * @see SIScalarCreateByTakingNthRoot
 */
bool SIScalarTakeNthRoot(SIMutableScalarRef theScalar,
                         uint8_t root,
                         OCStringRef *error);

/**
 * @brief Replaces a mutable scalar’s value with its base-10 logarithm in place.
 *
 * SIMutableScalarRef inherits from OCTypeRef. Updates theScalar by setting its numeric value
 * to log10(value). The unit is converted to dimensionless (unit⁰). On failure (e.g., negative value),
 * theScalar remains unchanged.
 *
 * @param theScalar The SIMutableScalarRef to modify. Must not be NULL.
 * @param error     A pointer to an OCStringRef that, on failure, will be set to a descriptive error
 *                  message. The caller is responsible for releasing this string. May be NULL.
 *
 *
 * @return true if the operation succeeded; false otherwise.
 *
 * @see SIScalarCreateByTakingAbsoluteValue
 */
bool SIScalarTakeLog10(SIMutableScalarRef theScalar,
                       OCStringRef *error);

#pragma mark Operations

/**
 * @brief Creates a new SIScalar by zeroing a specified component of its value.
 *
 * SIScalarRef inherits from OCTypeRef. Allocates and returns a new OCType-compatible
 * SIScalar instance whose numeric value has the specified complex component set to zero.
 * The other components and unit remain unchanged.
 *
 * @param theScalar The SIScalarRef to process. Must not be NULL.
 * @param part      The complexPart enum value specifying which component to zero:
 *                  kSIRealPart, kSIImaginaryPart, kSIMagnitudePart, or kSIArgumentPart.
 *
 *
 * @return A new SIScalarRef with the specified part zeroed. Caller owns this reference and must
 *         balance it with OCRelease when it is no longer needed.
 *
 * @see SIScalarZeroPart
 */
SIScalarRef SIScalarCreateByZeroingPart(SIScalarRef theScalar, complexPart part);

/**
 * @brief Zeroes a specified component of a mutable scalar’s value in place.
 *
 * SIMutableScalarRef inherits from OCTypeRef. Updates the scalar by setting the specified
 * complex component to zero. The other components and unit are preserved.
 *
 * @param theScalar The SIMutableScalarRef to modify. Must not be NULL.
 * @param part      The complexPart enum value specifying which component to zero:
 *                  kSIRealPart, kSIImaginaryPart, kSIMagnitudePart, or kSIArgumentPart.
 *
 *
 * @return true if the operation succeeded; false otherwise.
 *
 * @see SIScalarCreateByZeroingPart
 */
bool SIScalarZeroPart(SIMutableScalarRef theScalar, complexPart part);

/**
 * @brief Multiplies a mutable scalar by a dimensionless real constant in place.
 *
 * SIMutableScalarRef inherits from OCTypeRef. Updates the scalar’s numeric value by
 * multiplying it with the provided real constant. The unit remains unchanged.
 *
 * @param theScalar The SIMutableScalarRef to modify. Must not be NULL.
 * @param constant  The dimensionless real constant multiplier.
 *
 *
 * @return true if the operation succeeded; false otherwise.
 *
 * @see SIScalarCreateByMultiplyingByDimensionlessRealConstant
 */
bool SIScalarMultiplyByDimensionlessRealConstant(SIMutableScalarRef theScalar, double constant);

/**
 * @brief Creates a new SIScalar by multiplying by a dimensionless real constant.
 *
 * SIScalarRef inherits from OCTypeRef. Allocates and returns a new OCType-compatible
 * SIScalar instance whose numeric value equals theScalar × constant. The unit remains unchanged.
 *
 * @param theScalar The SIScalarRef to process. Must not be NULL.
 * @param constant  The dimensionless real constant multiplier.
 *
 *
 * @return A new SIScalarRef representing the scaled value. Caller owns this reference and must
 *         balance it with OCRelease when it is no longer needed.
 *
 * @see SIScalarMultiplyByDimensionlessRealConstant
 */
SIScalarRef SIScalarCreateByMultiplyingByDimensionlessRealConstant(SIScalarRef theScalar, double constant);

/**
 * @brief Creates a new SIScalar by multiplying by a dimensionless complex constant.
 *
 * SIScalarRef inherits from OCTypeRef. Allocates and returns a new OCType-compatible
 * SIScalar instance whose numeric value equals theScalar × constant (complex multiplier).
 * The unit remains unchanged.
 *
 * @param theScalar The SIScalarRef to process. Must not be NULL.
 * @param constant  The dimensionless complex constant multiplier.
 *
 *
 * @return A new SIScalarRef representing the complex-scaled value. Caller owns this reference and
 *         must balance it with OCRelease when it is no longer needed.
 *
 * @see SIScalarMultiplyByDimensionlessComplexConstant
 */
SIScalarRef SIScalarCreateByMultiplyingByDimensionlessComplexConstant(SIScalarRef theScalar, double complex constant);

/**
 * @brief Multiplies a mutable scalar by a dimensionless complex constant in place.
 *
 * SIMutableScalarRef inherits from OCTypeRef. Updates the scalar’s numeric value by
 * multiplying it with the provided complex constant. The unit remains unchanged.
 *
 * @param theScalar The SIMutableScalarRef to modify. Must not be NULL.
 * @param constant  The dimensionless complex constant multiplier.
 *
 *
 * @return true if the operation succeeded; false otherwise.
 *
 * @see SIScalarCreateByMultiplyingByDimensionlessComplexConstant
 */
bool SIScalarMultiplyByDimensionlessComplexConstant(SIMutableScalarRef theScalar, double complex constant);

/**
 * @brief Creates a new SIScalar by taking the complex conjugate of another scalar.
 *
 * SIScalarRef inherits from OCTypeRef. Allocates and returns a new OCType-compatible
 * SIScalar instance whose numeric value is the conjugate (real − i·imag) of the original.
 * The unit remains unchanged.
 *
 * @param theScalar The SIScalarRef to conjugate. Must not be NULL.
 *
 *
 * @return A new SIScalarRef representing the conjugated value. Caller owns this reference and
 *         must balance it with OCRelease when it is no longer needed.
 *
 * @see SIScalarConjugate
 */
SIScalarRef SIScalarCreateByConjugation(SIScalarRef theScalar);

/**
 * @brief Conjugates a mutable scalar in place.
 *
 * SIMutableScalarRef inherits from OCTypeRef. Updates the scalar’s numeric value to its
 * complex conjugate (real − i·imag). The unit remains unchanged.
 *
 * @param theScalar The SIMutableScalarRef to modify. Must not be NULL.
 *
 *
 * @return true if the operation succeeded; false otherwise.
 *
 * @see SIScalarCreateByConjugation
 */
bool SIScalarConjugate(SIMutableScalarRef theScalar);


#pragma mark Strings and Archiving

/**
 * @brief Prints a concise description of a scalar to the console.
 *
 * SIScalarRef inherits from OCTypeRef. This function formats a short representation
 * of the scalar’s numeric value and unit and writes it to the standard output or log.
 * Intended for quick debugging or inspection.
 *
 * @param theScalar The SIScalarRef to display. Must not be NULL.
 *
 *
 * @see SIScalarCreateStringValue, OCLog
 */
void SIScalarShow(SIScalarRef theScalar);

/**
 * @brief Creates a string representation of a scalar including value and unit.
 *
 * SIScalarRef inherits from OCTypeRef. Allocates and returns a new OCStringRef
 * containing a formatted "<value> <unit>" description (e.g., "3.14 m"). The string
 * uses a default formatting style.
 *
 * @param theScalar The SIScalarRef to stringify. Must not be NULL.
 *
 *
 * @return A new OCStringRef representing the scalar. The caller owns this reference
 *         and must balance it with OCRelease when no longer needed.
 *
 * @see OCRelease, SIScalarCreateStringValueWithFormat
 */
OCStringRef SIScalarCreateStringValue(SIScalarRef theScalar);

/**
 * @brief Creates a string representation of only the numeric part of a scalar.
 *
 * SIScalarRef inherits from OCTypeRef. Allocates and returns a new OCStringRef
 * containing the formatted numeric value without the unit (e.g., "3.14").
 *
 * @param theScalar The SIScalarRef whose numeric part is to be stringified. Must not be NULL.
 *
 *
 * @return A new OCStringRef representing the numeric value. The caller owns this
 *         reference and must balance it with OCRelease when no longer needed.
 *
 * @see OCRelease, SIScalarCreateNumericStringValueWithFormat
 */
OCStringRef SIScalarCreateNumericStringValue(SIScalarRef theScalar);

/**
 * @brief Creates a string representation of a specific component of a scalar.
 *
 * SIScalarRef inherits from OCTypeRef. Allocates and returns a new OCStringRef
 * containing the formatted value of the specified complexPart (real, imaginary,
 * magnitude, or argument), without the unit.
 *
 * @param theScalar The SIScalarRef to inspect. Must not be NULL.
 * @param thePart   The complexPart enum value specifying which component to stringify.
 *
 *
 * @return A new OCStringRef representing the requested component. The caller owns
 *         this reference and must balance it with OCRelease when no longer needed.
 *
 * @see OCRelease
 */
OCStringRef SIScalarCreateStringValueForPart(SIScalarRef theScalar,
                                             complexPart thePart);

/**
 * @brief Creates a string representation of only the unit of a scalar.
 *
 * SIScalarRef inherits from OCTypeRef. Allocates and returns a new OCStringRef
 * containing the unit symbol or name (e.g., "m/s²").
 *
 * @param theScalar The SIScalarRef whose unit is to be stringified. Must not be NULL.
 *
 *
 * @return A new OCStringRef representing the unit. The caller owns this reference
 *         and must balance it with OCRelease when no longer needed.
 *
 * @see OCRelease
 */
OCStringRef SIScalarCreateUnitString(SIScalarRef theScalar);

/**
 * @brief Creates a formatted string representation of a scalar using a custom format.
 *
 * SIScalarRef inherits from OCTypeRef. Allocates and returns a new OCStringRef
 * using the provided OCStringRef format specifier (e.g., "%.2f %s"), where format
 * may include standard printf-style placeholders for value and unit.
 *
 * @param theScalar The SIScalarRef to format. Must not be NULL.
 * @param format    An OCStringRef containing a valid format specifier. Must not be NULL.
 *
 *
 * @return A new OCStringRef containing the formatted result. The caller owns this
 *         reference and must balance it with OCRelease when no longer needed.
 *
 * @see OCRelease, SIScalarCreateStringValue
 */
OCStringRef SIScalarCreateStringValueWithFormat(SIScalarRef theScalar,
                                                OCStringRef format);

/**
 * @brief Creates a formatted string of the numeric part using a custom format.
 *
 * SIScalarRef inherits from OCTypeRef. Allocates and returns a new OCStringRef
 * using the provided OCStringRef format specifier to represent only the numeric
 * value (e.g., "%.3e").
 *
 * @param theScalar The SIScalarRef whose numeric part is to be formatted. Must not be NULL.
 * @param format    An OCStringRef containing a valid format specifier. Must not be NULL.
 *
 *
 * @return A new OCStringRef containing the formatted numeric value. The caller owns
 *         this reference and must balance it with OCRelease when no longer needed.
 *
 * @see OCRelease, SIScalarCreateNumericStringValue
 */
OCStringRef SIScalarCreateNumericStringValueWithFormat(SIScalarRef theScalar,
                                                       OCStringRef format);

/**
 * @brief Appends the scalar’s value and unit as a string to a mutable array.
 *
 * SIScalarRef inherits from OCTypeRef. This function creates a string representation
 * of the scalar (via SIScalarCreateStringValue) and adds it to the provided
 * OCMutableArrayRef.
 *
 * @param theScalar The SIScalarRef to stringify and append. Must not be NULL.
 * @param array     The OCMutableArrayRef to which the string is added. Must not be NULL.
 *
 *
 * @see SIScalarCreateStringValue, OCMutableArrayAddObject
 */
void SIScalarAddToArrayAsStringValue(SIScalarRef theScalar,
                                     OCMutableArrayRef array);


#pragma mark Tests

/**
 * @brief Determines whether a scalar is a real number (imaginary part is zero).
 *
 * SIScalarRef inherits from OCTypeRef. This function inspects the scalar’s numeric value
 * and returns true if its imaginary component is zero.
 *
 * @param theScalar The SIScalarRef to test. Must not be NULL.
 *
 *
 * @return true if the scalar is real; false otherwise.
 *
 * @see SIScalarIsComplex, SIScalarIsImaginary
 */
bool SIScalarIsReal(SIScalarRef theScalar);

/**
 * @brief Determines whether a scalar is purely imaginary (real part is zero and imaginary part non-zero).
 *
 * SIScalarRef inherits from OCTypeRef. This function inspects the scalar’s numeric value
 * and returns true if its real component is zero and its imaginary component is non-zero.
 *
 * @param theScalar The SIScalarRef to test. Must not be NULL.
 *
 *
 * @return true if the scalar is purely imaginary; false otherwise.
 *
 * @see SIScalarIsReal, SIScalarIsComplex
 */
bool SIScalarIsImaginary(SIScalarRef theScalar);

/**
 * @brief Determines whether a scalar has a non-zero imaginary component.
 *
 * SIScalarRef inherits from OCTypeRef. This function inspects the scalar’s numeric value
 * and returns true if its imaginary component is non-zero.
 *
 * @param theScalar The SIScalarRef to test. Must not be NULL.
 *
 *
 * @return true if the scalar is complex (imaginary part ≠ 0); false otherwise.
 *
 * @see SIScalarIsReal
 */
bool SIScalarIsComplex(SIScalarRef theScalar);

/**
 * @brief Determines whether a scalar’s numeric value is exactly zero.
 *
 * SIScalarRef inherits from OCTypeRef. This function returns true if both the real and
 * imaginary components (if any) are zero.
 *
 * @param theScalar The SIScalarRef to test. Must not be NULL.
 *
 *
 * @return true if the scalar is zero; false otherwise.
 */
bool SIScalarIsZero(SIScalarRef theScalar);

/**
 * @brief Determines whether a scalar’s numeric value is infinite (±∞).
 *
 * SIScalarRef inherits from OCTypeRef. This function returns true if either the real or
 * imaginary component is infinite.
 *
 * @param theScalar The SIScalarRef to test. Must not be NULL.
 *
 *
 * @return true if the scalar is infinite; false otherwise.
 */
bool SIScalarIsInfinite(SIScalarRef theScalar);

/**
 * @brief Determines whether a scalar is a real, non-negative integer.
 *
 * SIScalarRef inherits from OCTypeRef. This function returns true if the scalar is real
 * (imaginary part is zero), its real component is ≥ 0, and its fractional part is zero.
 *
 * @param theScalar The SIScalarRef to test. Must not be NULL.
 *
 *
 * @return true if the scalar is a non-negative integer; false otherwise.
 */
bool SIScalarIsRealNonNegativeInteger(SIScalarRef theScalar);

/**
 * @brief Validates whether a proposed string represents a scalar with the same dimensionality.
 *
 * SIScalarRef inherits from OCTypeRef. This function parses the provided OCStringRef and
 * checks that the parsed unit matches the dimensionality of theScalar. On failure, an error
 * description is provided.
 *
 * @param theScalar            The SIScalarRef whose dimensionality to match. Must not be NULL.
 * @param proposedStringValue  An OCStringRef containing the candidate quantity string. Must not be NULL.
 * @param error                A pointer to an OCStringRef that, on failure, will be set to a descriptive error
 *                             message. The caller is responsible for releasing this string. May be NULL.
 *
 *
 * @return true if the proposed string is valid and matches dimensionality; false otherwise.
 */
bool SIScalarValidateProposedStringValue(SIScalarRef theScalar,
                                         OCStringRef proposedStringValue,
                                         OCStringRef *error);

/**
 * @brief Tests whether two scalars are equal in value, unit, and metadata.
 *
 * SIScalarRef inherits from OCTypeRef. This function compares all attributes of the two
 * scalar instances for exact equality.
 *
 * @param input1 The first SIScalarRef. Must not be NULL.
 * @param input2 The second SIScalarRef. Must not be NULL.
 *
 *
 * @return true if all attributes match exactly; false otherwise.
 */
bool SIScalarEqual(SIScalarRef input1, SIScalarRef input2);

/**
 * @brief Compares two scalars and returns an ordering result.
 *
 * SIScalarRef inherits from OCTypeRef. This function compares values after converting to
 * compatible units. Returns kOCCompareLessThan, kOCCompareEqualTo, kOCCompareGreaterThan,
 * or kOCCompareUnequalDimensionalities if units differ.
 *
 * @param scalar      The first SIScalarRef. Must not be NULL.
 * @param otherScalar The second SIScalarRef. Must not be NULL.
 *
 *
 * @return An OCComparisonResult indicating comparison outcome.
 */
OCComparisonResult SIScalarCompare(SIScalarRef scalar, SIScalarRef otherScalar);

/**
 * @brief Compares two scalars in reduced units and returns an ordering result.
 *
 * SIScalarRef inherits from OCTypeRef. Converts both scalars to their reduced unit forms
 * before comparison. Returns kOCCompareLessThan, kOCCompareEqualTo,
 * kOCCompareGreaterThan, or kOCCompareUnequalDimensionalities.
 *
 * @param theScalar       The first SIScalarRef. Must not be NULL.
 * @param theOtherScalar  The second SIScalarRef. Must not be NULL.
 *
 *
 * @return An OCComparisonResult indicating comparison outcome.
 */
OCComparisonResult SIScalarCompareReduced(SIScalarRef theScalar, SIScalarRef theOtherScalar);

/**
 * @brief Performs a “loose” comparison between two scalars and returns an ordering result.
 *
 * SIScalarRef inherits from OCTypeRef. This function compares values with a tolerance
 * to account for floating-point discrepancies. Units must be compatible. Returns
 * kOCCompareLessThan, kOCCompareEqualTo, or kOCCompareGreaterThan.
 *
 * @param theScalar       The first SIScalarRef. Must not be NULL.
 * @param theOtherScalar  The second SIScalarRef. Must not be NULL.
 *
 *
 * @return An OCComparisonResult indicating comparison outcome.
 */
OCComparisonResult SIScalarCompareLoose(SIScalarRef theScalar, SIScalarRef theOtherScalar);


#endif /* SIScalar_h */
