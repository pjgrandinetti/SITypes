//
//  PSQuantity.h
//  SITypes
//
//  Created by Philip Grandinetti on 6/13/17.
//  Copyright © 2017 PhySy Ltd. All rights reserved.
//

#ifndef PSQuantity_h
#define PSQuantity_h

#include "SILibrary.h"

/*!
 @enum complexPart
 @constant kPSRealPart real part of complex number.
 @constant kPSImaginaryPart imaginary part of complex number.
 @constant kPSMagnitudePart magnitude part of complex number.
 @constant kPSArgumentPart argument part of complex number.
 */
typedef enum complexPart {
    kPSRealPart,
    kPSImaginaryPart,
    kPSMagnitudePart,
    kPSArgumentPart,
} complexPart;


/*!
 @typedef PSQuantityRef
 This is the type of a reference to a PSQuantity.
 */
typedef const struct __PSQuantity * PSQuantityRef;

/*!
 @typedef PSMutableQuantity
 This is the type of a reference to mutable PSQuantity.
 */
typedef struct __PSQuantity * PSMutableQuantityRef;

/*!
 @header PSQuantity
 PSQuantity represents a physical quantity and has two attributes: a element type and a unit.
 PSQuantity is an abstract type.
 
 @copyright PhySy Ltd
 */

#pragma mark Accessors
/*!
 @functiongroup Accessors
 */

/*!
 @function PSQuantityGetUnit
 @abstract Returns the quantity's unit.
 @param quantity The quantity.
 @result a SIUnit object
 */
SIUnitRef PSQuantityGetUnit(PSQuantityRef quantity);

/*!
 @function PSQuantitySetUnit
 @abstract Set the quantity's unit.
 @param quantity The quantity.
 @param unit The unit.
 */
void PSQuantitySetUnit(PSMutableQuantityRef quantity, SIUnitRef unit);

/*!
 @function PSQuantityGetUnitDimensionality
 @abstract Returns the quantity's dimensionality.
 @param quantity The quantity.
 @result a SIDimensionality object
 */
SIDimensionalityRef PSQuantityGetUnitDimensionality(PSQuantityRef quantity);

/*!
 @function PSQuantityGetElementType
 @abstract Returns the type used by a quantity to store its values.
 @param quantity The quantity.
 @result the element type.  Possible values are kPSNumberFloatType, kPSNumberDoubleType, kPSNumberFloatComplexType, and kPSNumberDoubleComplexType.
 */
numberType PSQuantityGetElementType(PSQuantityRef quantity);

/*!
 @function PSQuantityElementSize
 @abstract Returns size (in bytes) of a quantity element.
 @param quantity The quantity.
 @result the size
 */
int PSQuantityElementSize(PSQuantityRef quantity);

#pragma mark Tests
/*!
 @functiongroup Tests
 */

/*!
 @function PSQuantityHasElementType
 @abstract Tests if quantity has a specific element type.
 @param quantity The quantity.
 @param elementType The element type.
 @result true or false.
 */
bool PSQuantityHasElementType(PSQuantityRef quantity, numberType elementType);

/*!
 @function PSQuantityIsComplexType
 @abstract Tests if quantity has a complex element type.
 @param theQuantity The quantity.
 @result true or false.
 */
bool PSQuantityIsComplexType(PSQuantityRef theQuantity);

/*!
 @function PSQuantityHasDimensionality
 @abstract Tests if quantity has a specific dimensionality.
 @param quantity The quantity.
 @param theDimensionality The dimensionality.
 @result true or false.
 */
bool PSQuantityHasDimensionality(PSQuantityRef quantity, SIDimensionalityRef theDimensionality);

/*!
 @function PSQuantityHasSameDimensionality
 @abstract Determines if two quantities have the same dimensionality exponents,
 @param input1 The first quantity.
 @param input2 The second quantity.
 @result true or false.
 */
bool PSQuantityHasSameDimensionality(PSQuantityRef input1, PSQuantityRef input2);

/*!
 @function PSQuantityHasSameReducedDimensionality
 @abstract Determines if two quantities have the same reduced dimensionality exponents,
 @param input1 The first quantity.
 @param input2 The second quantity.
 @result true or false.
 */
bool PSQuantityHasSameReducedDimensionality(PSQuantityRef input1, PSQuantityRef input2);

/*!
 @function PSQuantityLargerElementType
 @abstract Returns larger element type for the two input quantities.
 @param input1 The first numberType.
 @param input2 The second numberType.
 @result the larger numberType of the two quantities
 */
numberType PSQuantityLargerElementType(PSQuantityRef input1,PSQuantityRef input2);

/*!
 @function PSQuantitySmallerElementType
 @abstract Returns smaller element type for the two input quantities.
 @param input1 The first numberType.
 @param input2 The second numberType.
 @result the smaller numberType of the two quantities
 */
numberType PSQuantitySmallerElementType(PSQuantityRef input1, PSQuantityRef input2);

/*!
 @function PSQuantityBestElementType
 @abstract Returns the best element type for the two input quantities.
 @param input1 The first numberType.
 @param input2 The second numberType.
 @result the best numberType from the two quantities
 @discussion Returns the best element type for the two input quantities which loses no precision
 when the quantities are combined in any way: add, subtract, multiply, divide.  Input element
 types and outputs are:
 
 (float and float) => float
 
 (float and double) => double
 
 (float and float complex) => float complex
 
 (float and double complex) => double complex
 
 (double and double) => double
 
 (double and float complex) => double complex
 
 (double and double complex) => double complex
 
 (float complex and float complex) => float complex
 
 (float complex and double complex) => double complex
 
 (double complex and double complex) => double complex
 */
numberType PSQuantityBestElementType(PSQuantityRef input1, PSQuantityRef input2);

#endif /* PSQuantity_h */
