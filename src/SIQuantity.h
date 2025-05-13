//
//  SIQuantity.h
//  SITypes
//
//  Created by Philip Grandinetti on 6/13/17.
//  Copyright © 2017 PhySy Ltd. All rights reserved.
//

#ifndef SIQuantity_h
#define SIQuantity_h

#include "SILibrary.h"

/*!
 @enum complexPart
 @constant kSIRealPart real part of complex number.
 @constant kSIImaginaryPart imaginary part of complex number.
 @constant kSIMagnitudePart magnitude part of complex number.
 @constant kSIArgumentPart argument part of complex number.
 */
typedef enum complexPart {
    kSIRealPart,
    kSIImaginaryPart,
    kSIMagnitudePart,
    kSIArgumentPart,
} complexPart;


/*!
 @typedef SIQuantityRef
 This is the type of a reference to a SIQuantity.
 */
typedef const struct __SIQuantity * SIQuantityRef;

/*!
 @typedef SIMutableQuantity
 This is the type of a reference to mutable SIQuantity.
 */
typedef struct __SIQuantity * SIMutableQuantityRef;

/*!
 @header SIQuantity
 SIQuantity represents a physical quantity and has two attributes: a element type and a unit.
 SIQuantity is an abstract type.
 
 @copyright PhySy Ltd
 */

#pragma mark Accessors
/*!
 @functiongroup Accessors
 */

/*!
 @function SIQuantityGetUnit
 @abstract Returns the quantity's unit.
 @param quantity The quantity.
 @result a SIUnit object
 */
SIUnitRef SIQuantityGetUnit(SIQuantityRef quantity);

/*!
 @function SIQuantitySetUnit
 @abstract Set the quantity's unit.
 @param quantity The quantity.
 @param unit The unit.
 */
void SIQuantitySetUnit(SIMutableQuantityRef quantity, SIUnitRef unit);

/*!
 @function SIQuantityGetUnitDimensionality
 @abstract Returns the quantity's dimensionality.
 @param quantity The quantity.
 @result a SIDimensionality object
 */
SIDimensionalityRef SIQuantityGetUnitDimensionality(SIQuantityRef quantity);

/*!
 @function SIQuantityGetElementType
 @abstract Returns the type used by a quantity to store its values.
 @param quantity The quantity.
 @result the element type.  Possible values are kOCNumberFloatType, kOCNumberDoubleType, kOCNumberFloatComplexType, and kOCNumberDoubleComplexType.
 */
numberType SIQuantityGetElementType(SIQuantityRef quantity);

/*!
 @function SIQuantityElementSize
 @abstract Returns size (in bytes) of a quantity element.
 @param quantity The quantity.
 @result the size
 */
int SIQuantityElementSize(SIQuantityRef quantity);

#pragma mark Tests
/*!
 @functiongroup Tests
 */

/*!
 @function SIQuantityHasElementType
 @abstract Tests if quantity has a specific element type.
 @param quantity The quantity.
 @param elementType The element type.
 @result true or false.
 */
bool SIQuantityHasElementType(SIQuantityRef quantity, numberType elementType);

/*!
 @function SIQuantityIsComplexType
 @abstract Tests if quantity has a complex element type.
 @param theQuantity The quantity.
 @result true or false.
 */
bool SIQuantityIsComplexType(SIQuantityRef theQuantity);

/*!
 @function SIQuantityHasDimensionality
 @abstract Tests if quantity has a specific dimensionality.
 @param quantity The quantity.
 @param theDimensionality The dimensionality.
 @result true or false.
 */
bool SIQuantityHasDimensionality(SIQuantityRef quantity, SIDimensionalityRef theDimensionality);

/*!
 @function SIQuantityHasSameDimensionality
 @abstract Determines if two quantities have the same dimensionality exponents,
 @param input1 The first quantity.
 @param input2 The second quantity.
 @result true or false.
 */
bool SIQuantityHasSameDimensionality(SIQuantityRef input1, SIQuantityRef input2);

/*!
 @function SIQuantityHasSameReducedDimensionality
 @abstract Determines if two quantities have the same reduced dimensionality exponents,
 @param input1 The first quantity.
 @param input2 The second quantity.
 @result true or false.
 */
bool SIQuantityHasSameReducedDimensionality(SIQuantityRef input1, SIQuantityRef input2);

/*!
 @function SIQuantityLargerElementType
 @abstract Returns larger element type for the two input quantities.
 @param input1 The first numberType.
 @param input2 The second numberType.
 @result the larger numberType of the two quantities
 */
numberType SIQuantityLargerElementType(SIQuantityRef input1,SIQuantityRef input2);

/*!
 @function SIQuantitySmallerElementType
 @abstract Returns smaller element type for the two input quantities.
 @param input1 The first numberType.
 @param input2 The second numberType.
 @result the smaller numberType of the two quantities
 */
numberType SIQuantitySmallerElementType(SIQuantityRef input1, SIQuantityRef input2);

/*!
 @function SIQuantityBestElementType
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
numberType SIQuantityBestElementType(SIQuantityRef input1, SIQuantityRef input2);

#endif /* SIQuantity_h */
