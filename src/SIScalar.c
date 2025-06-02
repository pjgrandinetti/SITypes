//
//  SIScalar.c
//  SITypes
//
//  Created by philip on 6/14/17.
//  Copyright © 2017 PhySy Ltd. All rights reserved.
//

#include "SILibrary.h"
#include <stdio.h> // Already likely there or in a header, but good to ensure for printf, etc.
#include <stdlib.h> // For malloc, free, abs
#include <math.h>   // For math functions like nan, fabsf, log10, pow, etc.
#include <string.h>

static OCTypeID kSIScalarID = _kOCNotATypeID;

// SIScalar Opaque Type
struct __SIScalar {
    OCBase _base;

    // __SIQuantity Type attributes
    SIUnitRef       unit;
    SINumberType    type;

    // __SIScalar Type attributes
    __SINumber      value;
};

static bool __SIScalarEqual(const void * theType1, const void * theType2)
{
    SIScalarRef theNumber1 = (SIScalarRef) theType1;
    SIScalarRef theNumber2 = (SIScalarRef) theType2;
    if(theNumber1->_base.typeID != theNumber2->_base.typeID) return false;
    
    if(NULL == theNumber1 || NULL == theNumber2) return false;
    if(theNumber1 == theNumber2) return true;
    if(theNumber1->type != theNumber2->type) return false;
    if(!OCTypeEqual(theNumber1->unit, theNumber2->unit)) return false;

    switch (theNumber1->type) {
        case kOCNumberFloat32Type: {
            if(theNumber1->value.floatValue != theNumber2->value.floatValue) return false;
            break;
        }
        case kSINumberFloat64Type: {
            if(theNumber1->value.doubleValue != theNumber2->value.doubleValue) return false;
            break;
        }
        case kOCNumberFloat32ComplexType: {
            if(theNumber1->value.floatComplexValue != theNumber2->value.floatComplexValue) return false;
            break;
        }
        case kSINumberFloat64ComplexType: {
            if(theNumber1->value.floatComplexValue != theNumber2->value.doubleComplexValue) return false;
            break;
        }
    }
    return true;
}

static void __SIScalarFinalize(const void * theType)
{
    if(NULL == theType) return;
    SIScalarRef theNumber = (SIScalarRef) theType;
    if(theNumber->unit) OCRelease(theNumber->unit);
    free((void *)theNumber);
    theNumber = NULL; // Set to NULL to avoid dangling pointer
}

static OCStringRef __SIScalarCopyFormattingDescription(OCTypeRef theType)
{
    SIScalarRef theNumber = (SIScalarRef) theType;
    switch (theNumber->type) {
        case kOCNumberFloat32Type: {
            return  OCStringCreateWithFormat(STR("%f %@"), theNumber->value.floatValue, theNumber->unit);
            break;
        }
        case kSINumberFloat64Type: {
            return  OCStringCreateWithFormat(STR("%lf %@"), theNumber->value.doubleValue, theNumber->unit);
            break;
        }
        case kOCNumberFloat32ComplexType: {
            return  OCStringCreateWithFormat(STR("%f+I•%f %@"), crealf(theNumber->value.floatComplexValue), cimagf(theNumber->value.floatComplexValue), theNumber->unit);
            break;
        }
        case kSINumberFloat64ComplexType: {
            return  OCStringCreateWithFormat(STR("%lf+I•%lf %@"), creal(theNumber->value.doubleComplexValue), cimag(theNumber->value.doubleComplexValue), theNumber->unit);
            break;
        }
    }
}

// Expose the formatting description function
OCStringRef SIScalarCopyFormattingDescription(SIScalarRef scalar) {
    return __SIScalarCopyFormattingDescription((OCTypeRef) scalar);
}

OCTypeID SIScalarGetTypeID(void)
{
    if(kSIScalarID == _kOCNotATypeID) kSIScalarID = OCRegisterType("SIScalar");
    return kSIScalarID;
}

static struct __SIScalar *SIScalarAllocate()
{
    struct __SIScalar *obj = malloc(sizeof(struct __SIScalar));
    if(NULL == obj) {
        fprintf(stderr, "Failed to allocate memory for SIScalarAllocate\n");
        return NULL;
    }

    obj->_base.typeID = SIScalarGetTypeID();
    obj->_base.static_instance = false; 
    obj->_base.finalize = __SIScalarFinalize;
    obj->_base.equal = __SIScalarEqual;
    obj->_base.copyFormattingDesc = __SIScalarCopyFormattingDescription;
    obj->_base.retainCount = 1;
    obj->_base.finalized = false;

    obj->unit = NULL;
    obj->type = kSINumberFloat32Type;
    memset(&obj->value, 0, sizeof(obj->value));
    return obj;
}


SIScalarRef SIScalarCreate(SIUnitRef unit, SINumberType type, void *value)
{
    struct __SIScalar *theNumber = SIScalarAllocate();
    if(NULL == theNumber) return NULL;
    
    theNumber->type = type;
    switch (type) {
        case kOCNumberFloat32Type: {
            float *ptr = (float *) value;
            theNumber->value.floatValue = *ptr;
            break;
        }
        case kSINumberFloat64Type: {
            double *ptr = (double *) value;
            theNumber->value.doubleValue = *ptr;
            break;
        }
        case kOCNumberFloat32ComplexType: {
            float complex *ptr = (float complex *) value;
            theNumber->value.floatComplexValue = *ptr;
            break;
        }
        case kSINumberFloat64ComplexType: {
            double complex *ptr = (double complex *) value;
            theNumber->value.doubleComplexValue = *ptr;
            break;
        }
    }
    if(unit) theNumber->unit = unit;
    else theNumber->unit = SIUnitDimensionlessAndUnderived();
    return (SIScalarRef) theNumber;

    return theNumber;
}


static SIMutableScalarRef SIScalarCreateMutable(SIUnitRef unit,  SINumberType elementType, void *value)
{
    return (SIMutableScalarRef) SIScalarCreate(unit, elementType, value);
}

/*
 @function SIScalarCreateCopy
 @abstract Creates a copy of a scalar
 @param theScalar The scalar.
 @result a copy of the scalar.
 */
SIScalarRef SIScalarCreateCopy(SIScalarRef theScalar)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,NULL);
    return SIScalarCreate(theScalar->unit, theScalar->type, (void *) &theScalar->value);
}

SIMutableScalarRef SIScalarCreateMutableCopy(SIScalarRef theScalar)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,NULL);
    return (SIMutableScalarRef) SIScalarCreate(theScalar->unit, theScalar->type, (void *) &theScalar->value);
}

//SIScalarRef SIScalarCreateWithInt32(int32_t input_value)
//{
//   return SIScalarCreate(NULL, kOCNumberSInt32Type, &input_value);
//}
//
//SIMutableScalarRef SIScalarCreateMutableWithInt32(int32_t input_value)
//{
//    return SIScalarCreateMutable(NULL, kOCNumberSInt32Type, &input_value);
//}
//
//SIScalarRef SIScalarCreateWithInt64(int64_t input_value)
//{
//    return SIScalarCreate(NULL, kOCNumberSInt64Type, &input_value);
//}
//
//SIMutableScalarRef SIScalarCreateMutableWithInt64(int64_t input_value)
//{
//    return SIScalarCreateMutable(NULL, kOCNumberSInt64Type, &input_value);
//}
//
SIScalarRef SIScalarCreateWithFloat(float input_value, SIUnitRef unit)
{
    return SIScalarCreate(unit, kSINumberFloat32Type, &input_value);
}

SIMutableScalarRef SIScalarCreateMutableWithFloat(float input_value, SIUnitRef unit)
{
    return SIScalarCreateMutable(unit, kSINumberFloat32Type, &input_value);
}

SIScalarRef SIScalarCreateWithDouble(double input_value, SIUnitRef unit)
{
    return SIScalarCreate(unit, kSINumberFloat64Type, &input_value);
}

SIMutableScalarRef SIScalarCreateMutableWithDouble(double input_value, SIUnitRef unit)
{
    return SIScalarCreateMutable(unit, kSINumberFloat64Type, &input_value);
}

SIScalarRef SIScalarCreateWithFloatComplex(float complex input_value, SIUnitRef unit)
{
    return SIScalarCreate(unit, kSINumberFloat32ComplexType, &input_value);
}

SIMutableScalarRef SIScalarCreateMutableWithFloatComplex(float complex input_value, SIUnitRef unit)
{
    return SIScalarCreateMutable(unit, kSINumberFloat32ComplexType, &input_value);
}

SIScalarRef SIScalarCreateWithDoubleComplex(double complex input_value, SIUnitRef unit)
{
    return SIScalarCreate(unit, kSINumberFloat64ComplexType, &input_value);
}

SIMutableScalarRef SIScalarCreateMutableWithDoubleComplex(double complex input_value, SIUnitRef unit)
{
    return SIScalarCreateMutable(unit, kSINumberFloat64ComplexType, &input_value);
}

#pragma mark Accessors

void SIScalarSetElementType(SIMutableScalarRef theScalar, SINumberType elementType)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,);
    switch (theScalar->type) {
        case kSINumberFloat32Type:{
            float value = theScalar->value.floatValue;
            theScalar->type = elementType;
            switch (elementType) {
                case kSINumberFloat32Type:
                    theScalar->value.floatValue = value;
                    return;
                case kSINumberFloat64Type:
                    theScalar->value.doubleValue = value;
                    return;
                case kSINumberFloat32ComplexType:
                    theScalar->value.floatComplexValue = value;
                    return;
                case kSINumberFloat64ComplexType:
                    theScalar->value.doubleComplexValue = value;
                    return;
            }
            break;
        }
        case kSINumberFloat64Type:{
            double value = theScalar->value.doubleValue;
            theScalar->type = elementType;
            switch (elementType) {
                case kSINumberFloat32Type:
                    theScalar->value.floatValue = value;
                    return;
                case kSINumberFloat64Type:
                    theScalar->value.doubleValue = value;
                    return;
                case kSINumberFloat32ComplexType:
                    theScalar->value.floatComplexValue = value;
                    return;
                case kSINumberFloat64ComplexType:
                    theScalar->value.doubleComplexValue = value;
                    return;
            }
            break;
        }
        case kSINumberFloat32ComplexType:{
            float complex value = theScalar->value.floatComplexValue;
            theScalar->type = elementType;
            switch (elementType) {
                case kSINumberFloat32Type:
                    theScalar->value.floatValue = value;
                    return;
                case kSINumberFloat64Type:
                    theScalar->value.doubleValue = value;
                    return;
                case kSINumberFloat32ComplexType:
                    theScalar->value.floatComplexValue = value;
                    return;
                case kSINumberFloat64ComplexType:
                    theScalar->value.doubleComplexValue = value;
                    return;
            }
            break;
        }
        case kSINumberFloat64ComplexType:{
            double complex value = theScalar->value.doubleComplexValue;
            theScalar->type = elementType;
            switch (elementType) {
                case kSINumberFloat32Type:
                    theScalar->value.floatValue = value;
                    return;
                case kSINumberFloat64Type:
                    theScalar->value.doubleValue = value;
                    return;
                case kSINumberFloat32ComplexType:
                    theScalar->value.floatComplexValue = value;
                    return;
                case kSINumberFloat64ComplexType:
                    theScalar->value.doubleComplexValue = value;
                    return;
            }
            break;
        }
    }
    
    return;
}

__SINumber SIScalarGetValue(SIScalarRef theScalar)
{
    return theScalar->value;
}

void SIScalarSetFloatValue(SIMutableScalarRef theScalar, float value)
{
    theScalar->type= kSINumberFloat32Type;
    theScalar->value.floatValue = value;
}

void SIScalarSetDoubleValue(SIMutableScalarRef theScalar, double value)
{
    theScalar->type= kSINumberFloat64Type;
    theScalar->value.doubleValue = value;
}

void SIScalarSetFloatComplexValue(SIMutableScalarRef theScalar, float complex value)
{
    theScalar->type= kSINumberFloat32ComplexType;
    theScalar->value.floatComplexValue = value;
}

void SIScalarSetDoubleComplexValue(SIMutableScalarRef theScalar, double complex value)
{
    theScalar->type= kSINumberFloat64ComplexType;
    theScalar->value.doubleComplexValue = value;
}


float SIScalarFloatValue(SIScalarRef theScalar)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,0);
    switch (theScalar->type) {
        case kSINumberFloat32Type:
            return (float) theScalar->value.floatValue;
        case kSINumberFloat64Type:
            return (float) theScalar->value.doubleValue;
        case kSINumberFloat32ComplexType:
            return (float) theScalar->value.floatComplexValue;
        case kSINumberFloat64ComplexType:
            return (float) theScalar->value.doubleComplexValue;
    }
    return nan(NULL);
}

double SIScalarDoubleValue(SIScalarRef theScalar)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,0);
    switch (theScalar->type) {
        case kSINumberFloat32Type:
            return (double) theScalar->value.floatValue;
        case kSINumberFloat64Type:
            return (double) theScalar->value.doubleValue;
        case kSINumberFloat32ComplexType:
            return (double) theScalar->value.floatComplexValue;
        case kSINumberFloat64ComplexType:
            return (double) theScalar->value.doubleComplexValue;
    }
    return nan(NULL);
}

float complex SIScalarFloatComplexValue(SIScalarRef theScalar)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,0);
    switch (theScalar->type) {
        case kSINumberFloat32Type:
            return (float complex) theScalar->value.floatValue;
        case kSINumberFloat64Type:
            return (float complex) theScalar->value.doubleValue;
        case kSINumberFloat32ComplexType:
            return (float complex) theScalar->value.floatComplexValue;
        case kSINumberFloat64ComplexType:
            return (float complex) theScalar->value.doubleComplexValue;
    }
    return nan(NULL);
}

double complex SIScalarDoubleComplexValue(SIScalarRef theScalar)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,0);
    switch (theScalar->type) {
        case kSINumberFloat32Type:
            return (double complex) theScalar->value.floatValue;
        case kSINumberFloat64Type:
            return (double complex) theScalar->value.doubleValue;
        case kSINumberFloat32ComplexType:
            return (double complex) theScalar->value.floatComplexValue;
        case kSINumberFloat64ComplexType:
            return (double complex) theScalar->value.doubleComplexValue;
    }
    return nan(NULL);
}

double SIScalarMagnitudeValue(SIScalarRef theScalar)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,0);
    double complex value = SIScalarDoubleComplexValue(theScalar);
    return cabs(value);
}

double SIScalarArgumentValue(SIScalarRef theScalar)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,0);
    double complex value = SIScalarDoubleComplexValue(theScalar);
    return cargument(value);
}

float SIScalarFloatValueInCoherentUnit(SIScalarRef theScalar)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,nan(NULL));
    SIUnitRef coherentUnit = SIUnitFindCoherentSIUnitWithDimensionality(SIUnitGetDimensionality(theScalar->unit));
    double conversion = SIUnitConversion(theScalar->unit,coherentUnit);
    
    switch(theScalar->type) {
        case kSINumberFloat32Type: {
            return theScalar->value.floatValue*conversion;
        }
        case kSINumberFloat64Type: {
            return theScalar->value.doubleValue*conversion;
        }
        case kSINumberFloat32ComplexType: {
            return theScalar->value.floatComplexValue*conversion;
        }
        case kSINumberFloat64ComplexType: {
            return theScalar->value.doubleComplexValue*conversion;
        }
    }
}

double SIScalarDoubleValueInCoherentUnit(SIScalarRef theScalar)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,nan(NULL));
    SIUnitRef coherentUnit = SIUnitFindCoherentSIUnitWithDimensionality(SIUnitGetDimensionality(theScalar->unit));
    double conversion = SIUnitConversion(theScalar->unit,coherentUnit);
    
    switch(theScalar->type) {
        case kSINumberFloat32Type: {
            return theScalar->value.floatValue*conversion;
        }
        case kSINumberFloat64Type: {
            return theScalar->value.doubleValue*conversion;
        }
        case kSINumberFloat32ComplexType: {
            return theScalar->value.floatComplexValue*conversion;
        }
        case kSINumberFloat64ComplexType: {
            return theScalar->value.doubleComplexValue*conversion;
        }
    }
}

float complex SIScalarFloatComplexValueInCoherentUnit(SIScalarRef theScalar)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,nan(NULL));
    SIUnitRef coherentUnit = SIUnitFindCoherentSIUnitWithDimensionality(SIUnitGetDimensionality(theScalar->unit));
    double conversion = SIUnitConversion(theScalar->unit,coherentUnit);
    
    switch(theScalar->type) {
        case kSINumberFloat32Type: {
            return theScalar->value.floatValue*conversion;
        }
        case kSINumberFloat64Type: {
            return theScalar->value.doubleValue*conversion;
        }
        case kSINumberFloat32ComplexType: {
            return theScalar->value.floatComplexValue*conversion;
        }
        case kSINumberFloat64ComplexType: {
            return theScalar->value.doubleComplexValue*conversion;
        }
    }
}

double complex SIScalarDoubleComplexValueInCoherentUnit(SIScalarRef theScalar)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,nan(NULL));
    SIUnitRef coherentUnit = SIUnitFindCoherentSIUnitWithDimensionality(SIUnitGetDimensionality(theScalar->unit));
    double conversion = SIUnitConversion(theScalar->unit,coherentUnit);
    
    switch(theScalar->type) {
        case kSINumberFloat32Type: {
            return theScalar->value.floatValue*conversion;
        }
        case kSINumberFloat64Type: {
            return theScalar->value.doubleValue*conversion;
        }
        case kSINumberFloat32ComplexType: {
            return theScalar->value.floatComplexValue*conversion;
        }
        case kSINumberFloat64ComplexType: {
            return theScalar->value.doubleComplexValue*conversion;
        }
    }
}

float SIScalarFloatValueInUnit(SIScalarRef theScalar, SIUnitRef unit, bool *success)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,nan(NULL));
    if(SIDimensionalityHasSameReducedDimensionality(SIQuantityGetUnitDimensionality((SIQuantityRef) theScalar),SIUnitGetDimensionality(unit))) {
        double conversion = SIUnitConversion(theScalar->unit,unit);
        switch(theScalar->type) {
            case kSINumberFloat32Type: {
                return theScalar->value.floatValue*conversion;
            }
            case kSINumberFloat64Type: {
                return theScalar->value.doubleValue*conversion;
            }
            case kSINumberFloat32ComplexType: {
                return theScalar->value.floatComplexValue*conversion;
            }
            case kSINumberFloat64ComplexType: {
                return theScalar->value.doubleComplexValue*conversion;
            }
        }
    }
    if(success) *success *= false;
    return nan(NULL);
}

double SIScalarDoubleValueInUnit(SIScalarRef theScalar, SIUnitRef unit, bool *success)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,nan(NULL));
    if(SIDimensionalityHasSameReducedDimensionality(SIQuantityGetUnitDimensionality((SIQuantityRef) theScalar),SIUnitGetDimensionality(unit))) {
        double conversion = SIUnitConversion(theScalar->unit,unit);
        switch(theScalar->type) {
            case kSINumberFloat32Type: {
                return theScalar->value.floatValue*conversion;
            }
            case kSINumberFloat64Type: {
                return theScalar->value.doubleValue*conversion;
            }
            case kSINumberFloat32ComplexType: {
                return theScalar->value.floatComplexValue*conversion;
            }
            case kSINumberFloat64ComplexType: {
                return theScalar->value.doubleComplexValue*conversion;
            }
        }
    }
    if(success) *success *= false;
    return nan(NULL);
}

float complex SIScalarFloatComplexValueInUnit(SIScalarRef theScalar, SIUnitRef unit, bool *success)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,nan(NULL));
    if(SIDimensionalityHasSameReducedDimensionality(SIQuantityGetUnitDimensionality((SIQuantityRef) theScalar),SIUnitGetDimensionality(unit))) {
        double conversion = SIUnitConversion(theScalar->unit,unit);
        switch(theScalar->type) {
            case kSINumberFloat32Type: {
                return theScalar->value.floatValue*conversion;
            }
            case kSINumberFloat64Type: {
                return theScalar->value.doubleValue*conversion;
            }
            case kSINumberFloat32ComplexType: {
                return theScalar->value.floatComplexValue*conversion;
            }
            case kSINumberFloat64ComplexType: {
                return theScalar->value.doubleComplexValue*conversion;
            }
        }
    }
    if(success) *success *= false;
    return nan(NULL);
}

double complex SIScalarDoubleComplexValueInUnit(SIScalarRef theScalar, SIUnitRef unit, bool *success)
{
    if(NULL==theScalar) {
        *success = false;
        return nan(NULL);
    }
    if(SIDimensionalityHasSameReducedDimensionality(SIQuantityGetUnitDimensionality((SIQuantityRef) theScalar),SIUnitGetDimensionality(unit))) {
        double conversion = SIUnitConversion(theScalar->unit,unit);
        switch(theScalar->type) {
            case kSINumberFloat32Type: {
                return theScalar->value.floatValue*conversion;
            }
            case kSINumberFloat64Type: {
                return theScalar->value.doubleValue*conversion;
            }
            case kSINumberFloat32ComplexType: {
                return theScalar->value.floatComplexValue*conversion;
            }
            case kSINumberFloat64ComplexType: {
                return theScalar->value.doubleComplexValue*conversion;
            }
        }
    }
    if(success) *success *= false;
    return nan(NULL);
}

#pragma mark Operations

SIScalarRef SIScalarCreateByConvertingToNumberType(SIScalarRef theScalar, SINumberType elementType)
{
    IF_NO_OBJECT_EXISTS_RETURN(theScalar,NULL);
    SIScalarRef result = SIScalarCreateCopy(theScalar);
    SIScalarSetElementType((SIMutableScalarRef) result, elementType);
    return result;
}

bool SIScalarTakeComplexPart(SIMutableScalarRef theScalar, complexPart part)
{
    IF_NO_OBJECT_EXISTS_RETURN(theScalar,false);
    switch (theScalar->type) {
        case kSINumberFloat32Type:{
            if(part == kSIImaginaryPart || part == kSIArgumentPart) {
                theScalar->value.floatValue = 0;
                return true;
            }
            if(part == kSIMagnitudePart) {
                theScalar->value.floatValue = fabsf(theScalar->value.floatValue);
                return true;
            }
            if(part == kSIRealPart) return true;
            break;
        }
        case kSINumberFloat64Type: {
            if(part == kSIImaginaryPart || part == kSIArgumentPart) {
                theScalar->value.doubleValue = 0;
                return true;
            }
            if(part == kSIMagnitudePart) {
                theScalar->value.doubleValue = fabs(theScalar->value.doubleValue);
                return true;
            }
            if(part == kSIRealPart) return true;
            break;
        }
        case kSINumberFloat32ComplexType: {
            if(part == kSIRealPart) {
                theScalar->value.floatValue = creal(theScalar->value.floatComplexValue);
                theScalar->type = kSINumberFloat32Type;
                return true;
            }
            if(part == kSIImaginaryPart) {
                theScalar->value.floatValue = cimag(theScalar->value.floatComplexValue);
                theScalar->type = kSINumberFloat32Type;
                return true;
            }
            if(part == kSIArgumentPart) {
                theScalar->value.floatValue = cargument(theScalar->value.floatComplexValue);
                theScalar->type = kSINumberFloat32Type;
                theScalar->unit = SIUnitForUnderivedSymbol(STR("rad"));
                return true;
            }
            if(part == kSIMagnitudePart) {
                theScalar->value.floatValue = cabs(theScalar->value.floatComplexValue);
                theScalar->type = kSINumberFloat32Type;
                return true;
            }
            break;
        }
        case kSINumberFloat64ComplexType: {
            if(part == kSIRealPart) {
                theScalar->value.doubleValue = creal(theScalar->value.doubleComplexValue);
                theScalar->type = kSINumberFloat64Type;
                return true;
            }
            if(part == kSIImaginaryPart) {
                theScalar->value.doubleValue = cimag(theScalar->value.doubleComplexValue);
                theScalar->type = kSINumberFloat64Type;
                return true;
            }
            if(part == kSIArgumentPart) {
                theScalar->value.doubleValue = cargument(theScalar->value.doubleComplexValue);
                theScalar->type = kSINumberFloat64Type;
                theScalar->unit = SIUnitForUnderivedSymbol(STR("rad"));
                return true;
            }
            if(part == kSIMagnitudePart) {
                theScalar->value.doubleValue = cabs(theScalar->value.doubleComplexValue);
                theScalar->type = kSINumberFloat64Type;
                return true;
            }
            break;
        }
    }
    return false;
}

SIScalarRef SIScalarCreateByTakingComplexPart(SIScalarRef theScalar, complexPart part)
{
    IF_NO_OBJECT_EXISTS_RETURN(theScalar,NULL);
    SIScalarRef result = SIScalarCreateCopy(theScalar);
    if(SIScalarTakeComplexPart((SIMutableScalarRef) result, part)) return result;
    OCRelease(result);
    return NULL;
}

bool SIScalarReduceUnit(SIMutableScalarRef theScalar)
{
    IF_NO_OBJECT_EXISTS_RETURN(theScalar,false);
    switch (theScalar->type) {
        case kSINumberFloat32Type:
        {
            double unit_multiplier = 1.0;
            SIUnitRef reducedUnit = SIUnitByReducing(theScalar->unit,&unit_multiplier);
            theScalar->unit = reducedUnit;
            theScalar->value.floatValue = theScalar->value.floatValue*unit_multiplier;
            return true;
        }
        case kSINumberFloat64Type:
        {
            double unit_multiplier = 1.0;
            SIUnitRef reducedUnit = SIUnitByReducing(theScalar->unit,&unit_multiplier);
            theScalar->unit = reducedUnit;
            theScalar->value.doubleValue = theScalar->value.doubleValue*unit_multiplier;
            return true;
        }
        case kSINumberFloat32ComplexType:
        {
            double unit_multiplier = 1.0;
            SIUnitRef reducedUnit = SIUnitByReducing(theScalar->unit,&unit_multiplier);
            theScalar->unit = reducedUnit;
            theScalar->value.floatComplexValue = theScalar->value.floatComplexValue*unit_multiplier;
            return true;
        }
        case kSINumberFloat64ComplexType:
        {
            double unit_multiplier = 1.0;
            SIUnitRef reducedUnit = SIUnitByReducing(theScalar->unit,&unit_multiplier);
            theScalar->unit = reducedUnit;
            theScalar->value.doubleComplexValue = theScalar->value.doubleComplexValue*unit_multiplier;
            return true;
        }
    }
    return false;
}

SIScalarRef SIScalarCreateByReducingUnit(SIScalarRef theScalar)
{
    IF_NO_OBJECT_EXISTS_RETURN(theScalar,NULL);
    SIScalarRef result = SIScalarCreateCopy(theScalar);
    if(SIScalarReduceUnit((SIMutableScalarRef) result)) return result;
    if(result) OCRelease(result);
    return NULL;
}

bool SIScalarConvertToUnit(SIMutableScalarRef theScalar, SIUnitRef unit, OCStringRef *error)
{
    if(error) if(*error) return false;
    IF_NO_OBJECT_EXISTS_RETURN(theScalar,false);
    if(!SIDimensionalityHasSameReducedDimensionality(SIUnitGetDimensionality(theScalar->unit),SIUnitGetDimensionality(unit))) {
        if(error==NULL) return false;
        *error = STR("Incompatible Dimensionalities.");
        return false;
    }
    double conversion = SIUnitConversion(theScalar->unit,unit);
    theScalar->unit = unit;
    
    switch(theScalar->type) {
            //        case kOCNumberSInt32Type: {
            //            theScalar->value.int32Value = theScalar->value.int32Value*conversion;
            //            return true;
            //        }
            //        case kOCNumberSInt64Type: {
            //            theScalar->value.int64Value = theScalar->value.int64Value*conversion;
            //            return true;
            //        }
        case kSINumberFloat32Type: {
            theScalar->value.floatValue = theScalar->value.floatValue*conversion;
            return true;
        }
        case kSINumberFloat64Type: {
            theScalar->value.doubleValue = theScalar->value.doubleValue*conversion;
            return true;
        }
        case kSINumberFloat32ComplexType: {
            theScalar->value.floatComplexValue = theScalar->value.floatComplexValue*conversion;
            return true;
        }
        case kSINumberFloat64ComplexType: {
            theScalar->value.doubleComplexValue = theScalar->value.doubleComplexValue*conversion;
            return true;
        }
            
    }
    return false;
}

SIScalarRef SIScalarCreateByConvertingToUnit(SIScalarRef theScalar, SIUnitRef unit, OCStringRef *error)
{
    if(error) if(*error) return NULL;
    IF_NO_OBJECT_EXISTS_RETURN(theScalar,NULL);
    SIScalarRef result = SIScalarCreateCopy(theScalar);
    if(SIScalarConvertToUnit((SIMutableScalarRef) result, unit, error)) return result;
    if(result) OCRelease(result);
    return NULL;
}

bool SIScalarConvertToCoherentUnit(SIMutableScalarRef theScalar, OCStringRef *error)
{
    if(error) if(*error) return false;
    IF_NO_OBJECT_EXISTS_RETURN(theScalar,false);
    SIUnitRef coherentUnit = SIUnitFindCoherentSIUnitWithDimensionality(SIUnitGetDimensionality(theScalar->unit));
    return SIScalarConvertToUnit(theScalar, coherentUnit, error);
}

SIScalarRef SIScalarCreateByConvertingToCoherentUnit(SIScalarRef theScalar, OCStringRef *error)
{
    if(error) if(*error) return NULL;
    IF_NO_OBJECT_EXISTS_RETURN(theScalar,NULL);
    SIScalarRef result = SIScalarCreateCopy(theScalar);
    if(SIScalarConvertToCoherentUnit((SIMutableScalarRef) result, error)) return result;
    if(result)  OCRelease(result);
    return NULL;
}

bool SIScalarBestConversionForQuantity(SIMutableScalarRef theScalar, OCStringRef quantity, OCStringRef *error)
{
    if(error) if(*error) return NULL;
    if(NULL==quantity) return false;
    OCArrayRef units = SIUnitCreateArrayOfUnitsForQuantity(quantity);
    if(units==NULL) return false;
    
    SIMutableScalarRef trialScalar = SIScalarCreateMutableCopy(theScalar);
    
    if(!SIScalarConvertToUnit(trialScalar, OCArrayGetValueAtIndex(units, 0), error)) {
        OCRelease(units);
        OCRelease(trialScalar);
        return false;
    }
    int originalMagnitude = fabs(log10(fabs(SIScalarDoubleValue(theScalar))));
    int magnitude = originalMagnitude;
    uint64_t best = 0;
    for(uint64_t index=1; index<OCArrayGetCount(units); index++) {
        SIUnitRef unit = OCArrayGetValueAtIndex(units, index);
        if(!SIScalarConvertToUnit(trialScalar, unit, error)) {
            OCRelease(trialScalar);
            OCRelease(units);
            return false;
        }
        int trialMagnitude = fabs(log10(fabs(SIScalarDoubleValue(trialScalar))));
        if(trialMagnitude < magnitude) {
            best = index;
            magnitude = trialMagnitude;
        }
    }
    bool result = true;
    if(abs(originalMagnitude - magnitude)>2) {
        result = SIScalarConvertToUnit(theScalar, OCArrayGetValueAtIndex(units, best), error);
    }
    OCRelease(trialScalar);
    OCRelease(units);
    return result;
}

bool SIScalarAdd(SIMutableScalarRef target, SIScalarRef input2, OCStringRef *error)
{
    if(error) if(*error) return false;
   	IF_NO_OBJECT_EXISTS_RETURN(target,false);
   	IF_NO_OBJECT_EXISTS_RETURN(input2,false);
    
    // Rules for addition and subtraction:
    //	- numbers must have the same dimensionality
    //	- returned SIScalar with have elementType of target argument
    //	- returned SIScalar will have unit of the target argument
    
    if(!SIDimensionalityHasSameReducedDimensionality(SIUnitGetDimensionality(target->unit),SIUnitGetDimensionality(input2->unit))) {
        if(error) {
            *error = STR("Incompatible Dimensionalities.");
        }
        return NULL;
    }
    
    switch(input2->type) {
        case kSINumberFloat32Type: {
            float value = SIScalarFloatValueInUnit(input2, target->unit, NULL);
            switch (target->type) {
                case kOCNumberFloat32Type: {
                    target->value.floatValue = target->value.floatValue + value;
                    return true;
                }
                case kSINumberFloat64Type: {
                    target->value.doubleValue = target->value.doubleValue + value;
                    return true;
                }
                case kOCNumberFloat32ComplexType: {
                    target->value.floatComplexValue = target->value.floatComplexValue + value;
                    return true;
                }
                case kSINumberFloat64ComplexType: {
                    target->value.doubleComplexValue = target->value.doubleComplexValue + value;
                    return true;
                }
            }
        }
        case kSINumberFloat64Type: {
            double value = SIScalarDoubleValueInUnit(input2, target->unit, NULL);
            switch (target->type) {
                case kOCNumberFloat32Type: {
                    target->value.floatValue = target->value.floatValue + value;
                    return true;
                }
                case kSINumberFloat64Type: {
                    target->value.doubleValue = target->value.doubleValue + value;
                    return true;
                }
                case kOCNumberFloat32ComplexType: {
                    target->value.floatComplexValue = target->value.floatComplexValue + value;
                    return true;
                }
                case kSINumberFloat64ComplexType: {
                    target->value.doubleComplexValue = target->value.doubleComplexValue + value;
                    return true;
                }
            }
        }
        case kOCNumberFloat32ComplexType: {
            float complex value = SIScalarFloatComplexValueInUnit(input2, target->unit, NULL);
            switch (target->type) {
                case kOCNumberFloat32Type: {
                    target->value.floatValue = target->value.floatValue + value;
                    return true;
                }
                case kSINumberFloat64Type: {
                    target->value.doubleValue = target->value.doubleValue + value;
                    return true;
                }
                case kOCNumberFloat32ComplexType: {
                    target->value.floatComplexValue = target->value.floatComplexValue + value;
                    return true;
                }
                case kSINumberFloat64ComplexType: {
                    target->value.doubleComplexValue = target->value.doubleComplexValue + value;
                    return true;
                }
            }
        }
        case kSINumberFloat64ComplexType: {
            double complex value = SIScalarDoubleComplexValueInUnit(input2, target->unit, NULL);
            switch (target->type) {
                case kOCNumberFloat32Type: {
                    target->value.floatValue = target->value.floatValue + value;
                    return true;
                }
                case kSINumberFloat64Type: {
                    target->value.doubleValue = target->value.doubleValue + value;
                    return true;
                }
                case kOCNumberFloat32ComplexType: {
                    target->value.floatComplexValue = target->value.floatComplexValue + value;
                    return true;
                }
                case kSINumberFloat64ComplexType: {
                    target->value.doubleComplexValue = target->value.doubleComplexValue + value;
                    return true;
                }
            }
        }
    }
    return false;
}

SIScalarRef SIScalarCreateByAdding(SIScalarRef input1, SIScalarRef input2, OCStringRef *error)
{
    if(error) if(*error) return NULL;
   	IF_NO_OBJECT_EXISTS_RETURN(input1,NULL);
   	IF_NO_OBJECT_EXISTS_RETURN(input2,NULL);
    SIScalarRef result = SIScalarCreateByConvertingToNumberType(input1, SIQuantityBestElementType((SIQuantityRef) input1, (SIQuantityRef) input2));
    if(SIScalarAdd((SIMutableScalarRef) result, input2, error)) return result;
    OCRelease(result);
    return NULL;
}

bool SIScalarSubtract(SIMutableScalarRef target, SIScalarRef input2, OCStringRef *error)
{
    if(error) if(*error) return false;
   	IF_NO_OBJECT_EXISTS_RETURN(target,false);
   	IF_NO_OBJECT_EXISTS_RETURN(input2,false);
    
    // Rules for addition and subtraction:
    //	- numbers must have the same dimensionality
    //	- returned SIScalar with have elementType of target argument
    //	- returned SIScalar will have unit of the target argument
    
    if(!SIDimensionalityHasSameReducedDimensionality(SIUnitGetDimensionality(target->unit),SIUnitGetDimensionality(input2->unit))) {
        if(error) {
            *error = STR("Incompatible Dimensionalities.");
        }
        return NULL;
    }
    
    switch(input2->type) {
        case kSINumberFloat32Type: {
            float value = SIScalarFloatValueInUnit(input2, target->unit, NULL);
            switch (target->type) {
                case kOCNumberFloat32Type: {
                    target->value.floatValue = target->value.floatValue - value;
                    return true;
                }
                case kSINumberFloat64Type: {
                    target->value.doubleValue = target->value.doubleValue - value;
                    return true;
                }
                case kOCNumberFloat32ComplexType: {
                    target->value.floatComplexValue = target->value.floatComplexValue - value;
                    return true;
                }
                case kSINumberFloat64ComplexType: {
                    target->value.doubleComplexValue = target->value.doubleComplexValue - value;
                    return true;
                }
            }
        }
        case kSINumberFloat64Type: {
            double value = SIScalarDoubleValueInUnit(input2, target->unit, NULL);
            switch (target->type) {
                case kOCNumberFloat32Type: {
                    target->value.floatValue = target->value.floatValue - value;
                    return true;
                }
                case kSINumberFloat64Type: {
                    target->value.doubleValue = target->value.doubleValue - value;
                    return true;
                }
                case kOCNumberFloat32ComplexType: {
                    target->value.floatComplexValue = target->value.floatComplexValue - value;
                    return true;
                }
                case kSINumberFloat64ComplexType: {
                    target->value.doubleComplexValue = target->value.doubleComplexValue - value;
                    return true;
                }
            }
        }
        case kOCNumberFloat32ComplexType: {
            float complex value = SIScalarFloatComplexValueInUnit(input2, target->unit, NULL);
            switch (target->type) {
                case kOCNumberFloat32Type: {
                    target->value.floatValue = target->value.floatValue - value;
                    return true;
                }
                case kSINumberFloat64Type: {
                    target->value.doubleValue = target->value.doubleValue - value;
                    return true;
                }
                case kOCNumberFloat32ComplexType: {
                    target->value.floatComplexValue = target->value.floatComplexValue - value;
                    return true;
                }
                case kSINumberFloat64ComplexType: {
                    target->value.doubleComplexValue = target->value.doubleComplexValue - value;
                    return true;
                }
            }
        }
        case kSINumberFloat64ComplexType: {
            double complex value = SIScalarDoubleComplexValueInUnit(input2, target->unit, NULL);
            switch (target->type) {
                case kOCNumberFloat32Type: {
                    target->value.floatValue = target->value.floatValue - value;
                    return true;
                }
                case kSINumberFloat64Type: {
                    target->value.doubleValue = target->value.doubleValue - value;
                    return true;
                }
                case kOCNumberFloat32ComplexType: {
                    target->value.floatComplexValue = target->value.floatComplexValue - value;
                    return true;
                }
                case kSINumberFloat64ComplexType: {
                    target->value.doubleComplexValue =(long double) target->value.doubleComplexValue -  (long double) value;
                    return true;
                }
            }
        }
    }
    return false;
}

SIScalarRef SIScalarCreateBySubtracting(SIScalarRef input1, SIScalarRef input2, OCStringRef *error)
{
    if(error) if(*error) return NULL;
   	IF_NO_OBJECT_EXISTS_RETURN(input1,NULL);
   	IF_NO_OBJECT_EXISTS_RETURN(input2,NULL);
    SIScalarRef result = SIScalarCreateByConvertingToNumberType(input1, SIQuantityBestElementType((SIQuantityRef) input1, (SIQuantityRef) input2));
    if(SIScalarSubtract((SIMutableScalarRef) result, input2, error)) return result;
    OCRelease(result);
    return NULL;
}

bool SIScalarMultiplyWithoutReducingUnit(SIMutableScalarRef target, SIScalarRef input2, OCStringRef *error)
{
    if(error) if(*error) return NULL;
    IF_NO_OBJECT_EXISTS_RETURN(target,false);
   	IF_NO_OBJECT_EXISTS_RETURN(input2,false);
    
    double unit_multiplier = 1;
    SIUnitRef unit = SIUnitByMultiplyingWithoutReducing(target->unit, input2->unit, &unit_multiplier, error);
    target->unit = unit;
    
    switch(input2->type) {
        case kOCNumberFloat32Type: {
            float value = input2->value.floatValue;
            switch (target->type) {
                case kOCNumberFloat32Type: {
                    target->value.floatValue = target->value.floatValue * value*unit_multiplier;
                    return true;
                }
                case kSINumberFloat64Type: {
                    target->value.doubleValue = target->value.doubleValue * value*unit_multiplier;
                    return true;
                }
                case kOCNumberFloat32ComplexType: {
                    target->value.floatComplexValue = target->value.floatComplexValue * value*unit_multiplier;
                    return true;
                }
                case kSINumberFloat64ComplexType: {
                    target->value.doubleComplexValue = target->value.doubleComplexValue * value*unit_multiplier;
                    return true;
                }
            }
        }
        case kSINumberFloat64Type: {
            double value = input2->value.doubleValue;
            switch (target->type) {
                case kOCNumberFloat32Type: {
                    target->value.floatValue = target->value.floatValue * value*unit_multiplier;
                    return true;
                }
                case kSINumberFloat64Type: {
                    target->value.doubleValue = target->value.doubleValue * value*unit_multiplier;
                    return true;
                }
                case kOCNumberFloat32ComplexType: {
                    target->value.floatComplexValue = target->value.floatComplexValue * value*unit_multiplier;
                    return true;
                }
                case kSINumberFloat64ComplexType: {
                    target->value.doubleComplexValue = target->value.doubleComplexValue * value*unit_multiplier;
                    return true;
                }
            }
        }
        case kOCNumberFloat32ComplexType: {
            float complex value = input2->value.floatComplexValue;
            switch (target->type) {
                case kOCNumberFloat32Type: {
                    target->value.floatValue = target->value.floatValue * value*unit_multiplier;
                    return true;
                }
                case kSINumberFloat64Type: {
                    target->value.doubleValue = target->value.doubleValue * value*unit_multiplier;
                    return true;
                }
                case kOCNumberFloat32ComplexType: {
                    target->value.floatComplexValue = target->value.floatComplexValue * value*unit_multiplier;
                    return true;
                }
                case kSINumberFloat64ComplexType: {
                    target->value.doubleComplexValue = target->value.doubleComplexValue * value*unit_multiplier;
                    return true;
                }
            }
        }
        case kSINumberFloat64ComplexType: {
            double complex value = input2->value.doubleComplexValue;
            switch (target->type) {
                case kOCNumberFloat32Type: {
                    target->value.floatValue = target->value.floatValue * value*unit_multiplier;
                    return true;
                }
                case kSINumberFloat64Type: {
                    target->value.doubleValue = target->value.doubleValue * value*unit_multiplier;
                    return true;
                }
                case kOCNumberFloat32ComplexType: {
                    target->value.floatComplexValue = target->value.floatComplexValue * value*unit_multiplier;
                    return true;
                }
                case kSINumberFloat64ComplexType: {
                    target->value.doubleComplexValue = target->value.doubleComplexValue * value*unit_multiplier;
                    return true;
                }
            }
        }
    }
    return false;
}

SIScalarRef SIScalarCreateByMultiplyingWithoutReducingUnit(SIScalarRef input1, SIScalarRef input2, OCStringRef *error)
{
    if(error) if(*error) return false;
   	IF_NO_OBJECT_EXISTS_RETURN(input1,false);
   	IF_NO_OBJECT_EXISTS_RETURN(input2,false);
    SIScalarRef result = SIScalarCreateByConvertingToNumberType(input1, SIQuantityBestElementType((SIQuantityRef) input1, (SIQuantityRef) input2));
    SIScalarMultiplyWithoutReducingUnit((SIMutableScalarRef) result, input2, error);
    return result;
}

bool SIScalarMultiply(SIMutableScalarRef target, SIScalarRef input2, OCStringRef *error)
{
    if(error) if(*error) return false;
   	IF_NO_OBJECT_EXISTS_RETURN(target,false);
   	IF_NO_OBJECT_EXISTS_RETURN(input2,false);
    
    double unit_multiplier = 1;
    SIUnitRef unit = SIUnitByMultiplying(target->unit, input2->unit, &unit_multiplier, error);
    target->unit = unit;
    
    switch(input2->type) {
        case kOCNumberFloat32Type: {
            float value = input2->value.floatValue;
            switch (target->type) {
                case kOCNumberFloat32Type: {
                    target->value.floatValue = target->value.floatValue * value*unit_multiplier;
                    return true;
                }
                case kSINumberFloat64Type: {
                    target->value.doubleValue = target->value.doubleValue * value*unit_multiplier;
                    return true;
                }
                case kOCNumberFloat32ComplexType: {
                    target->value.floatComplexValue = target->value.floatComplexValue * value*unit_multiplier;
                    return true;
                }
                case kSINumberFloat64ComplexType: {
                    target->value.doubleComplexValue = target->value.doubleComplexValue * value*unit_multiplier;
                    return true;
                }
            }
        }
        case kSINumberFloat64Type: {
            float value = input2->value.doubleValue;
            switch (target->type) {
                case kOCNumberFloat32Type: {
                    target->value.floatValue = target->value.floatValue * value*unit_multiplier;
                    return true;
                }
                case kSINumberFloat64Type: {
                    target->value.doubleValue = target->value.doubleValue * value*unit_multiplier;
                    return true;
                }
                case kOCNumberFloat32ComplexType: {
                    target->value.floatComplexValue = target->value.floatComplexValue * value*unit_multiplier;
                    return true;
                }
                case kSINumberFloat64ComplexType: {
                    target->value.doubleComplexValue = target->value.doubleComplexValue * value*unit_multiplier;
                    return true;
                }
            }
        }
        case kOCNumberFloat32ComplexType: {
            float value = input2->value.floatComplexValue;
            switch (target->type) {
                case kOCNumberFloat32Type: {
                    target->value.floatValue = target->value.floatValue * value*unit_multiplier;
                    return true;
                }
                case kSINumberFloat64Type: {
                    target->value.doubleValue = target->value.doubleValue * value*unit_multiplier;
                    return true;
                }
                case kOCNumberFloat32ComplexType: {
                    target->value.floatComplexValue = target->value.floatComplexValue * value*unit_multiplier;
                    return true;
                }
                case kSINumberFloat64ComplexType: {
                    target->value.doubleComplexValue = target->value.doubleComplexValue * value*unit_multiplier;
                    return true;
                }
            }
        }
        case kSINumberFloat64ComplexType: {
            float value = input2->value.doubleComplexValue;
            switch (target->type) {
                case kOCNumberFloat32Type: {
                    target->value.floatValue = target->value.floatValue * value*unit_multiplier;
                    return true;
                }
                case kSINumberFloat64Type: {
                    target->value.doubleValue = target->value.doubleValue * value*unit_multiplier;
                    return true;
                }
                case kOCNumberFloat32ComplexType: {
                    target->value.floatComplexValue = target->value.floatComplexValue * value*unit_multiplier;
                    return true;
                }
                case kSINumberFloat64ComplexType: {
                    target->value.doubleComplexValue = target->value.doubleComplexValue * value*unit_multiplier;
                    return true;
                }
            }
        }
    }
    return false;
}

SIScalarRef SIScalarCreateByMultiplying(SIScalarRef input1, SIScalarRef input2, OCStringRef *error)
{
    if(error) if(*error) return false;
   	IF_NO_OBJECT_EXISTS_RETURN(input1,NULL);
   	IF_NO_OBJECT_EXISTS_RETURN(input2,NULL);
    SIScalarRef result = SIScalarCreateByConvertingToNumberType(input1, SIQuantityBestElementType((SIQuantityRef) input1, (SIQuantityRef) input2));
    SIScalarMultiply((SIMutableScalarRef) result, input2, error);
    return result;
}

bool SIScalarDivideWithoutReducingUnit(SIMutableScalarRef target, SIScalarRef input2, OCStringRef *error)
{
    if(error) if(*error) return false;
   	IF_NO_OBJECT_EXISTS_RETURN(target,false);
   	IF_NO_OBJECT_EXISTS_RETURN(input2,false);
    
    double unit_multiplier = 1;
    SIUnitRef unit = SIUnitByDividingWithoutReducing(target->unit, input2->unit, &unit_multiplier);
    target->unit = unit;
    
    switch(input2->type) {
        case kOCNumberFloat32Type: {
            float value = input2->value.floatValue;
            if(value==0) {
                if(error==NULL) return false;
                *error = STR("Division by zero.");
                return false;
            }
            switch (target->type) {
                case kOCNumberFloat32Type: {
                    target->value.floatValue = target->value.floatValue * unit_multiplier/value;
                    return true;
                }
                case kSINumberFloat64Type: {
                    target->value.doubleValue = target->value.doubleValue * unit_multiplier/value;
                    return true;
                }
                case kOCNumberFloat32ComplexType: {
                    target->value.floatComplexValue = target->value.floatComplexValue * unit_multiplier/value;
                    return true;
                }
                case kSINumberFloat64ComplexType: {
                    target->value.doubleComplexValue = target->value.doubleComplexValue * unit_multiplier/value;
                    return true;
                }
            }
        }
        case kSINumberFloat64Type: {
            double value = input2->value.doubleValue;
            if(value==0) {
                if(error==NULL) return false;
                *error = STR("Division by zero.");
                return false;
            }
            switch (target->type) {
                case kOCNumberFloat32Type: {
                    target->value.floatValue = target->value.floatValue * unit_multiplier/value;
                    return true;
                }
                case kSINumberFloat64Type: {
                    target->value.doubleValue = target->value.doubleValue * unit_multiplier/value;
                    return true;
                }
                case kOCNumberFloat32ComplexType: {
                    target->value.floatComplexValue = target->value.floatComplexValue * unit_multiplier/value;
                    return true;
                }
                case kSINumberFloat64ComplexType: {
                    target->value.doubleComplexValue = target->value.doubleComplexValue * unit_multiplier/value;
                    return true;
                }
            }
        }
        case kOCNumberFloat32ComplexType: {
            float complex value = input2->value.floatComplexValue;
            if(value==0) {
                if(error==NULL) return false;
                *error = STR("Division by zero.");
                return false;
            }
            switch (target->type) {
                case kOCNumberFloat32Type: {
                    target->value.floatValue = target->value.floatValue * unit_multiplier/value;
                    return true;
                }
                case kSINumberFloat64Type: {
                    target->value.doubleValue = target->value.doubleValue * unit_multiplier/value;
                    return true;
                }
                case kOCNumberFloat32ComplexType: {
                    target->value.floatComplexValue = target->value.floatComplexValue * unit_multiplier/value;
                    return true;
                }
                case kSINumberFloat64ComplexType: {
                    target->value.doubleComplexValue = target->value.doubleComplexValue * unit_multiplier/value;
                    return true;
                }
            }
        }
        case kSINumberFloat64ComplexType: {
            double complex value = input2->value.doubleComplexValue;
            if(value==0) {
                if(error==NULL) return false;
                *error = STR("Division by zero.");
                return false;
            }
            switch (target->type) {
                case kOCNumberFloat32Type: {
                    target->value.floatValue = target->value.floatValue* unit_multiplier/value;
                    return true;
                }
                case kSINumberFloat64Type: {
                    target->value.doubleValue = target->value.doubleValue* unit_multiplier/value;
                    return true;
                }
                case kOCNumberFloat32ComplexType: {
                    target->value.floatComplexValue = target->value.floatComplexValue * unit_multiplier/value;
                    return true;
                }
                case kSINumberFloat64ComplexType: {
                    target->value.doubleComplexValue = target->value.doubleComplexValue * unit_multiplier/value;
                    return true;
                }
            }
        }
    }
    return false;
}

bool SIScalarDivide(SIMutableScalarRef target, SIScalarRef input2, OCStringRef *error)
{
    if(error) if(*error) return false;
    if(NULL==target) {
        IF_NO_OBJECT_EXISTS_RETURN(target,false);
        
    }
    if(NULL==input2) {
        IF_NO_OBJECT_EXISTS_RETURN(input2,false);
    }
    
    double unit_multiplier = 1;
    SIUnitRef unit = SIUnitByDividing(target->unit, input2->unit, &unit_multiplier);
    target->unit = unit;
    
    switch(input2->type) {
        case kOCNumberFloat32Type: {
            float value = input2->value.floatValue;
            if(value==0) {
                if(error==NULL) return false;
                *error = STR("Division by zero.");
                return false;
            }
            switch (target->type) {
                case kOCNumberFloat32Type: {
                    target->value.floatValue = target->value.floatValue * unit_multiplier/value;
                    return true;
                }
                case kSINumberFloat64Type: {
                    target->value.doubleValue = target->value.doubleValue * unit_multiplier/value;
                    return true;
                }
                case kOCNumberFloat32ComplexType: {
                    target->value.floatComplexValue = target->value.floatComplexValue * unit_multiplier/value;
                    return true;
                }
                case kSINumberFloat64ComplexType: {
                    target->value.doubleComplexValue = target->value.doubleComplexValue * unit_multiplier/value;
                    return true;
                }
            }
        }
        case kSINumberFloat64Type: {
            double value = input2->value.doubleValue;
            if(value==0) {
                if(error==NULL) return false;
                *error = STR("Division by zero.");
                return false;
            }
            switch (target->type) {
                case kOCNumberFloat32Type: {
                    target->value.floatValue = target->value.floatValue * unit_multiplier/value;
                    return true;
                }
                case kSINumberFloat64Type: {
                    target->value.doubleValue = target->value.doubleValue * unit_multiplier/value;
                    return true;
                }
                case kOCNumberFloat32ComplexType: {
                    target->value.floatComplexValue = target->value.floatComplexValue * unit_multiplier/value;
                    return true;
                }
                case kSINumberFloat64ComplexType: {
                    target->value.doubleComplexValue = target->value.doubleComplexValue * unit_multiplier/value;
                    return true;
                }
            }
        }
        case kOCNumberFloat32ComplexType: {
            float complex value = input2->value.floatComplexValue;
            if(value==0) {
                if(error==NULL) return false;
                *error = STR("Division by zero.");
                return false;
            }
            switch (target->type) {
                case kOCNumberFloat32Type: {
                    target->value.floatValue = target->value.floatValue * unit_multiplier/value;
                    return true;
                }
                case kSINumberFloat64Type: {
                    target->value.doubleValue = target->value.doubleValue * unit_multiplier/value;
                    return true;
                }
                case kOCNumberFloat32ComplexType: {
                    target->value.floatComplexValue = target->value.floatComplexValue * unit_multiplier/value;
                    return true;
                }
                case kSINumberFloat64ComplexType: {
                    target->value.doubleComplexValue = target->value.doubleComplexValue * unit_multiplier/value;
                    return true;
                }
            }
        }
        case kSINumberFloat64ComplexType: {
            double complex value = input2->value.doubleComplexValue;
            if(value==0) {
                if(error==NULL) return false;
                *error = STR("Division by zero.");
                return false;
            }
            switch (target->type) {
                case kOCNumberFloat32Type: {
                    target->value.floatValue = target->value.floatValue * unit_multiplier/value;
                    return true;
                }
                case kSINumberFloat64Type: {
                    target->value.doubleValue = target->value.doubleValue * unit_multiplier/value;
                    return true;
                }
                case kOCNumberFloat32ComplexType: {
                    target->value.floatComplexValue = target->value.floatComplexValue * unit_multiplier/value;
                    return true;
                }
                case kSINumberFloat64ComplexType: {
                    target->value.doubleComplexValue = target->value.doubleComplexValue * unit_multiplier/value;
                    return true;
                }
            }
        }
    }
    return false;
}

SIScalarRef SIScalarCreateByDividingWithoutReducingUnit(SIScalarRef input1, SIScalarRef input2, OCStringRef *error)
{
    if(error) if(*error) return NULL;
   	IF_NO_OBJECT_EXISTS_RETURN(input1,NULL);
   	IF_NO_OBJECT_EXISTS_RETURN(input2,NULL);
    SIScalarRef result = SIScalarCreateByConvertingToNumberType(input1, SIQuantityBestElementType((SIQuantityRef) input1, (SIQuantityRef) input2));
    if(SIScalarDivideWithoutReducingUnit((SIMutableScalarRef) result, input2, error)) return result;
    OCRelease(result);
    return NULL;
}

SIScalarRef SIScalarCreateByDividing(SIScalarRef input1, SIScalarRef input2, OCStringRef *error)
{
    if(error) if(*error) return NULL;
   	IF_NO_OBJECT_EXISTS_RETURN(input1,NULL);
   	IF_NO_OBJECT_EXISTS_RETURN(input2,NULL);
    SIScalarRef result = SIScalarCreateByConvertingToNumberType(input1, SIQuantityBestElementType((SIQuantityRef) input1, (SIQuantityRef) input2));
    if(SIScalarDivide((SIMutableScalarRef) result, input2, error)) return result;
    OCRelease(result);
    return NULL;
}

bool SIScalarRaiseToAPowerWithoutReducingUnit(SIMutableScalarRef theScalar, double power, OCStringRef *error)
{
    if(error) if(*error) return false;
    
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,false);
    double unit_multiplier = 1;
    SIUnitRef unit = SIUnitByRaisingToPowerWithoutReducing(theScalar->unit, power, &unit_multiplier, error);
    if(error) {
        if(*error) return false;
    }
   	IF_NO_OBJECT_EXISTS_RETURN(unit,false);
    
    theScalar->unit = unit;
    switch(theScalar->type) {
        case kOCNumberFloat32Type:
            theScalar->value.floatValue = pow(theScalar->value.floatValue,power)*unit_multiplier;
            return true;
        case kSINumberFloat64Type:
            theScalar->value.doubleValue = pow(theScalar->value.doubleValue,power)*unit_multiplier;
            return true;
        case kOCNumberFloat32ComplexType:
            theScalar->value.floatComplexValue = cpow(theScalar->value.floatComplexValue,power)*unit_multiplier;
            return true;
        case kSINumberFloat64ComplexType:
            theScalar->value.doubleComplexValue = cpow(theScalar->value.doubleComplexValue,power)*unit_multiplier;
            return true;
    }
    return false;
}

SIScalarRef SIScalarCreateByRaisingToAPowerWithoutReducingUnit(SIScalarRef theScalar, double power, OCStringRef *error)
{
    if(error) if(*error) return NULL;
    
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,NULL);
    SIScalarRef result = SIScalarCreateCopy(theScalar);
    if(SIScalarRaiseToAPowerWithoutReducingUnit((SIMutableScalarRef) result, power, error)) return result;
    if(result) OCRelease(result);
    return NULL;
}

bool SIScalarRaiseToAPower(SIMutableScalarRef theScalar, double power, OCStringRef *error)
{
    if(error) if(*error) return false;
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,false);
    
    // Rules for multiplication and division:
    //	- returned SIScalar with have whichever elementType is greatest of two method arguments
    //	- returned SIScalar unit will be in coherent SI units
    
    double unit_multiplier = 1;
    SIUnitRef unit = SIUnitByRaisingToPower(theScalar->unit, power, &unit_multiplier, error);
    if(error) {
        if(*error) return false;
    }
    
    theScalar->unit = unit;
    switch(theScalar->type) {
        case kOCNumberFloat32Type:
            theScalar->value.floatValue = pow(theScalar->value.floatValue,power)*unit_multiplier;
            return true;
        case kSINumberFloat64Type:
            theScalar->value.doubleValue = pow(theScalar->value.doubleValue,power)*unit_multiplier;
            return true;
        case kOCNumberFloat32ComplexType:
            theScalar->value.floatComplexValue = cpow(theScalar->value.floatComplexValue,power)*unit_multiplier;
            return true;
        case kSINumberFloat64ComplexType:
            theScalar->value.doubleComplexValue = cpow(theScalar->value.doubleComplexValue,power)*unit_multiplier;
            return true;
    }
    return false;
}

SIScalarRef SIScalarCreateByRaisingToAPower(SIScalarRef theScalar, double power, OCStringRef *error)
{
    if(error) if(*error) return NULL;
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,NULL);
    SIScalarRef result = SIScalarCreateCopy(theScalar);
    if(SIScalarRaiseToAPower((SIMutableScalarRef) result, power, error)) return result;
    if(result) OCRelease(result);
    return NULL;
}

bool SIScalarTakeAbsoluteValue(SIMutableScalarRef theScalar, OCStringRef *error)
{
    if(error) if(*error) return false;
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,false);
    
    switch(theScalar->type) {
        case kOCNumberFloat32Type:
            theScalar->value.floatValue = fabsf(theScalar->value.floatValue);
            return true;
        case kSINumberFloat64Type:
            theScalar->value.doubleValue = fabs(theScalar->value.doubleValue);
            return true;
        case kOCNumberFloat32ComplexType:
            theScalar->value.floatComplexValue = cabsf(theScalar->value.floatComplexValue);
            return true;
        case kSINumberFloat64ComplexType:
            theScalar->value.doubleComplexValue = cabs(theScalar->value.doubleComplexValue);
            return true;
    }
    return false;
}

SIScalarRef SIScalarCreateByTakingAbsoluteValue(SIScalarRef theScalar, OCStringRef *error)
{
    if(error) if(*error) return NULL;
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,NULL);
    SIScalarRef result = SIScalarCreateCopy(theScalar);
    if(SIScalarTakeAbsoluteValue((SIMutableScalarRef) result, error)) return result;
    if(result) OCRelease(result);
    return NULL;
}

SIScalarRef SIScalarCreateByGammaFunctionWithoutReducingUnit(SIScalarRef theScalar, OCStringRef *error)
{
    if(error) if(*error) return NULL;
    if(SIScalarIsComplex(theScalar)) {
        if(error) {
            *error = STR("Gamma function of complex number not implemented.");
        }
        return NULL;
    }
    
    if(SIUnitIsDimensionless(theScalar->unit)) {
        SIMutableScalarRef temp = SIScalarCreateMutableCopy(theScalar);
        SIScalarReduceUnit(temp);
        double argument = SIScalarDoubleValue(temp)+1;
        double result = tgamma(argument);
        OCRelease(temp);
        return SIScalarCreate(SIUnitDimensionlessAndUnderived(), kSINumberFloat64Type, &result);
    }
    else if(SIScalarIsRealNonNegativeInteger(theScalar)) {
        int64_t integerValue = (int64_t) SIScalarDoubleValue(theScalar);
        double unit_multiplier = 1;
        SIUnitRef newUnit = SIUnitByRaisingToPower(theScalar->unit, integerValue, &unit_multiplier, error);
        double argument = SIScalarDoubleValue(theScalar)+1;
        double result = tgamma(argument);
        return SIScalarCreate(newUnit, kSINumberFloat64Type, &result);
    }
    return NULL;
}

bool SIScalarMultiplyByDimensionlessRealConstant(SIMutableScalarRef theScalar, double constant)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,false);
    switch(theScalar->type) {
        case kOCNumberFloat32Type:
            theScalar->value.floatValue *= constant;
            return true;
        case kSINumberFloat64Type:
            theScalar->value.doubleValue *= constant;
            return true;
        case kOCNumberFloat32ComplexType:
            theScalar->value.floatComplexValue *= constant;
            return true;
        case kSINumberFloat64ComplexType:
            theScalar->value.doubleComplexValue *= constant;
            return true;
    }
    return false;
}

SIScalarRef SIScalarCreateByMultiplyingByDimensionlessRealConstant(SIScalarRef theScalar, double constant)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,NULL);
    SINumberType elementType = kSINumberFloat64Type;
    if(elementType<theScalar->type) elementType = theScalar->type;
    SIScalarRef result = SIScalarCreateByConvertingToNumberType(theScalar, elementType);
    if(SIScalarMultiplyByDimensionlessRealConstant((SIMutableScalarRef) result, constant)) return result;
    if(result) OCRelease(result);
    return NULL;
}

bool SIScalarMultiplyByDimensionlessComplexConstant(SIMutableScalarRef theScalar, double complex constant)
{
    IF_NO_OBJECT_EXISTS_RETURN(theScalar,false);
    
    // Check if constant has a non-zero imaginary part
    if (cimag(constant) != 0.0) {
        // If scalar is real but constant is complex, we need to convert to complex type
        switch(theScalar->type) {
            case kOCNumberFloat32Type: {
                // Convert float to float complex
                float complex value = theScalar->value.floatValue * constant;
                theScalar->type = kSINumberFloat32ComplexType;
                theScalar->value.floatComplexValue = value;
                return true;
            }
            case kSINumberFloat64Type: {
                // Convert double to double complex
                double complex value = theScalar->value.doubleValue * constant;
                theScalar->type = kSINumberFloat64ComplexType;
                theScalar->value.doubleComplexValue = value;
                return true;
            }
            case kOCNumberFloat32ComplexType:
                theScalar->value.floatComplexValue = theScalar->value.floatComplexValue * constant;
                return true;
            case kSINumberFloat64ComplexType:
                theScalar->value.doubleComplexValue = theScalar->value.doubleComplexValue * constant;
                return true;
        }
    } else {
        // Constant is purely real, so no type conversion needed
        switch(theScalar->type) {
            case kOCNumberFloat32Type:
                theScalar->value.floatValue = theScalar->value.floatValue * creal(constant);
                return true;
            case kSINumberFloat64Type:
                theScalar->value.doubleValue = theScalar->value.doubleValue * creal(constant);
                return true;
            case kOCNumberFloat32ComplexType:
                theScalar->value.floatComplexValue = theScalar->value.floatComplexValue * constant;
                return true;
            case kSINumberFloat64ComplexType:
                theScalar->value.doubleComplexValue = theScalar->value.doubleComplexValue * constant;
                return true;
        }
    }
    return false;
}

SIScalarRef SIScalarCreateByMultiplyingByDimensionlessComplexConstant(SIScalarRef theScalar, double complex constant)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,NULL);
    SIScalarRef result = SIScalarCreateByConvertingToNumberType(theScalar, kSINumberFloat64ComplexType);
    if(SIScalarMultiplyByDimensionlessComplexConstant((SIMutableScalarRef) result, constant)) return result;
    if(result) OCRelease(result);
    return NULL;
}

bool SIScalarConjugate(SIMutableScalarRef theScalar)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,false);
    switch(theScalar->type) {
            //		case kOCNumberSInt32Type:
            //		case kOCNumberSInt64Type:
        case kOCNumberFloat32Type:
        case kSINumberFloat64Type:
            return true;
        case kOCNumberFloat32ComplexType: {
            theScalar->value.floatComplexValue = creal(theScalar->value.floatComplexValue) - I*cimag(theScalar->value.floatComplexValue);
            return true;
        }
        case kSINumberFloat64ComplexType:
            theScalar->value.doubleComplexValue = creal(theScalar->value.doubleComplexValue) - I*cimag(theScalar->value.doubleComplexValue);
            return true;
    }
    return false;
}

SIScalarRef SIScalarCreateByConjugation(SIScalarRef theScalar)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,NULL);
    SIScalarRef result = SIScalarCreateCopy(theScalar);
    if(SIScalarConjugate((SIMutableScalarRef) result)) return result;
    if(result) OCRelease(result);
    return NULL;
}

bool SIScalarTakeNthRoot(SIMutableScalarRef theScalar, uint8_t root, OCStringRef *error)
{
    if(error) if(*error) return false;
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,false);
    
    double multiplier = 1;
    SIUnitRef newUnit = SIUnitByTakingNthRoot(theScalar->unit, root, &multiplier, error);
    if(error) {
        if(*error) return false;
    }
    theScalar->unit = newUnit;
    switch(theScalar->type) {
        case kOCNumberFloat32Type:
            if(root==2) theScalar->value.floatValue = sqrtf(theScalar->value.floatValue)*multiplier;
            else theScalar->value.floatValue = pow(theScalar->value.floatValue,1./root)*multiplier;
            break;
        case kSINumberFloat64Type:
            if(root==2) theScalar->value.doubleValue = sqrt(theScalar->value.doubleValue)*multiplier;
            else theScalar->value.doubleValue = pow(theScalar->value.doubleValue,1./root)*multiplier;
            break;
        case kOCNumberFloat32ComplexType:
            if(root==2) theScalar->value.floatComplexValue = csqrtf(theScalar->value.floatComplexValue)*multiplier;
            else theScalar->value.floatComplexValue = cpow(theScalar->value.floatComplexValue,1./root)*multiplier;
            break;
        case kSINumberFloat64ComplexType:
            if(root==2) theScalar->value.doubleComplexValue = csqrt(theScalar->value.doubleComplexValue)*multiplier;
            else theScalar->value.doubleComplexValue = cpow(theScalar->value.doubleComplexValue,1./root)*multiplier;
            break;
    }
    return true;
}

SIScalarRef SIScalarCreateByTakingNthRoot(SIScalarRef theScalar, uint8_t root, OCStringRef *error)
{
    if(error) if(*error) return NULL;
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,NULL);
    SIScalarRef result = SIScalarCreateCopy(theScalar);
    if(SIScalarTakeNthRoot((SIMutableScalarRef) result, root, error)) return result;
    if(result) OCRelease(result);
    return NULL;
}


bool SIScalarTakeLog10(SIMutableScalarRef theScalar, OCStringRef *error)
{
    IF_NO_OBJECT_EXISTS_RETURN(theScalar,false);
    if(!SIDimensionalityIsDimensionless(SIQuantityGetUnitDimensionality((SIQuantityRef) theScalar))) {
        if(error) {
            *error = STR("Log10 requires dimensionless unit.");
        }
    }
    switch (theScalar->type) {
        case kOCNumberFloat32Type: {
            theScalar->value.floatValue = log10f(theScalar->value.floatValue);
            break;
        }
        case kSINumberFloat64Type:{
            theScalar->value.doubleValue = log10(theScalar->value.doubleValue);
            break;
        }
        case kOCNumberFloat32ComplexType: {
            theScalar->value.floatComplexValue = clogf(theScalar->value.floatComplexValue)/logf(10);
            break;
               }
        case kSINumberFloat64ComplexType: {
            theScalar->value.doubleComplexValue = clog(theScalar->value.doubleComplexValue)/log(10);
            break;
        }
    }
    return true;
}


bool SIScalarZeroPart(SIMutableScalarRef theScalar, complexPart part)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,false);
    switch (theScalar->type) {
        case kOCNumberFloat32Type:
            if(part == kSIRealPart || part == kSIMagnitudePart) {
                theScalar->value.floatValue = 0;
                return true;
            }
            if(part == kSIImaginaryPart || part == kSIArgumentPart) return true;
            break;
        case kSINumberFloat64Type:
            if(part == kSIRealPart || part == kSIMagnitudePart) {
                theScalar->value.doubleValue = 0;
                return true;
            }
            if(part == kSIImaginaryPart || part == kSIArgumentPart) return true;
            break;
        case kOCNumberFloat32ComplexType: {
            if(part == kSIMagnitudePart) {
                theScalar->value.floatComplexValue = 0;
                return true;
            }
            if(part == kSIRealPart) {
                theScalar->value.floatComplexValue = cimag(theScalar->value.floatComplexValue);
                return true;
            }
            if(part == kSIImaginaryPart) {
                theScalar->value.floatComplexValue = creal(theScalar->value.floatComplexValue);
                return true;
            }
            if(part == kSIArgumentPart) {
                theScalar->value.floatComplexValue = cabs(theScalar->value.floatComplexValue);
                return true;
            }
            break;
        }
        case kSINumberFloat64ComplexType: {
            if(part == kSIMagnitudePart) {
                theScalar->value.doubleComplexValue = 0;
                return true;
            }
            if(part == kSIRealPart) {
                theScalar->value.doubleComplexValue = cimag(theScalar->value.doubleComplexValue);
                return true;
            }
            if(part == kSIImaginaryPart) {
                theScalar->value.doubleComplexValue = creal(theScalar->value.doubleComplexValue);
                return true;
            }
            if(part == kSIArgumentPart) {
                theScalar->value.doubleComplexValue = cabs(theScalar->value.doubleComplexValue);
                return true;
            }
            break;
        }
    }
    return false;
}

SIScalarRef SIScalarCreateByZeroingPart(SIScalarRef theScalar, complexPart part)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,NULL);
    SIScalarRef result = SIScalarCreateCopy(theScalar);
    if(SIScalarZeroPart((SIMutableScalarRef) result, part)) return result;
    OCRelease(result);
    return NULL;
}

static OCStringRef SIScalarCreateStringValueSplitByUnits(SIScalarRef theScalar, OCArrayRef units, bool doubleCheck, OCStringRef *error)
{
    if(error) if(*error) return NULL;
    if(!SIScalarIsReal(theScalar)) return NULL;
    bool isPositive = false;
    if(SIScalarDoubleValue(theScalar) >0.0) isPositive = true;
    
    OCMutableStringRef stringValue = OCStringCreateMutable(0);
    SIMutableScalarRef scalar = SIScalarCreateMutableCopy(theScalar);
    uint64_t count = OCArrayGetCount(units);
    uint64_t finalCount = 0;
    for(uint64_t index=0;index<count; index++) {
        SIUnitRef unit = OCArrayGetValueAtIndex(units, index);
        OCStringRef symbol = SIUnitCopySymbol(unit);
        SIScalarConvertToUnit(scalar, unit, error);
        double value = SIScalarDoubleValue(scalar);
        if(index<count-1) {
            if(isPositive) {
                value *= 100.;
                value = round(value);
                value /= 100.;
                value = OCDoubleFloor(value);
            }
            else {
                value *= 100.;
                value = round(value);
                value /= 100.;
                value = OCDoubleCeil(value);
            }
        }
        else {
            value *= 100.;
            value = round(value);
            value /= 100.;
        }
        
        if((value > 0.0 && isPositive) || (value < 0.0 && !isPositive)) {
            OCStringRef valueString = OCDoubleComplexCreateStringValue(value,NULL);
            finalCount++;
            if(OCStringGetLength(stringValue)>0) {
                if(isPositive) OCStringAppend(stringValue, STR(" + "));
                else OCStringAppend(stringValue, STR(" "));
            }
            OCStringAppend(stringValue, valueString);
            OCStringAppend(stringValue, STR(" "));
            OCStringAppend(stringValue, symbol);
            OCRelease(valueString);
            
            SIScalarRef scalarInUnit = SIScalarCreate(unit, kSINumberFloat64Type, &value);
            SIScalarSubtract(scalar, scalarInUnit, error);
            OCRelease(scalarInUnit);
        }
        OCRelease(symbol);
    }
    
    if(OCStringGetLength(stringValue) ==0 || finalCount == 1) {
        if(stringValue) OCRelease(stringValue);
        if(scalar) OCRelease(scalar);
        return NULL;
    }
    if(scalar) OCRelease(scalar);
    
    if(doubleCheck) {
        SIScalarRef check = SIScalarCreateWithOCString(stringValue, error);
        if(check) {
            if(SIScalarCompare(theScalar, check)==kOCCompareEqualTo) {
                OCRelease(check);
                return stringValue;
            }
        }
        OCRelease(stringValue);
        return NULL;
    }
    return stringValue;
}

static OCComparisonResult compareOnlyTheStrings(const void *val1, const void *val2, void *context)
{
    OCTypeID type1 = OCGetTypeID(val1);
    OCTypeID type2 = OCGetTypeID(val2);
    OCTypeID stringType = OCStringGetTypeID();
    
    if(type1==type2) {
        if(type1 == stringType)  {
            return OCStringCompare((OCStringRef) val1, (OCStringRef) val2, (OCStringCompareFlags) context);
        }
    }
    return kOCCompareLessThan;
}


OCArrayRef SIScalarCreateArrayOfConversionQuantitiesScalarsAndStringValues(SIScalarRef theScalar, OCStringRef quantity, OCStringRef *error)
{
    if(error) if(*error) return NULL;
    OCMutableArrayRef result = OCArrayCreateMutable(0,&kOCTypeArrayCallBacks);
    SIUnitRef fahrenheit = SIUnitForUnderivedSymbol(STR("°F"));
    SIUnitRef celsius = SIUnitForUnderivedSymbol(STR("°C"));
    
    if(SIDimensionalityHasSameReducedDimensionality(SIDimensionalityForQuantity(kSIQuantityTemperature,NULL), SIQuantityGetUnitDimensionality((SIQuantityRef)  theScalar))) {
        SIUnitRef theUnit = SIQuantityGetUnit((SIQuantityRef) theScalar);
        if(theUnit == fahrenheit) {
            OCArrayAppendValue(result, theScalar);
            
            double complex value = (SIScalarDoubleComplexValue(theScalar) -32)*5./9.;
            SIScalarRef scalar = SIScalarCreate(celsius, kSINumberFloat64ComplexType, &value);
            OCArrayAppendValue(result, scalar);
            OCRelease(scalar);
            return result;
        }
        if(theUnit == celsius) {
            OCArrayAppendValue(result, theScalar);
            
            double complex value = SIScalarDoubleValue(theScalar)*9./5. + 32;
            SIScalarRef scalar = SIScalarCreate(fahrenheit, kSINumberFloat64ComplexType, &value);
            OCArrayAppendValue(result, scalar);
            OCRelease(scalar);
            return result;
        }
    }
    
    OCArrayRef units = NULL;
    OCArrayRef quantities = NULL;
    
    if(theScalar) {
        if(NULL == quantity) {
            quantities = SIDimensionalityCreateArrayOfQuantitiesWithSameReducedDimensionality(SIQuantityGetUnitDimensionality((SIQuantityRef) theScalar));
            if(quantities) {
                OCArrayAppendArray(result, quantities,OCRangeMake(0, OCArrayGetCount(quantities)));
                OCRelease(quantities);
            }
        }
        if(NULL == quantity) units = SIUnitCreateArrayOfConversionUnits(SIQuantityGetUnit((SIQuantityRef) theScalar));
        else units = SIUnitCreateArrayOfUnitsForQuantity(quantity);
        
        if(units) {
            for(uint64_t index = 0; index<OCArrayGetCount(units); index++) {
                SIUnitRef unit = (SIUnitRef) OCArrayGetValueAtIndex(units, index);
                
                if(unit && fahrenheit!=unit && celsius !=unit) {
                    SIScalarRef newScalar = SIScalarCreateByConvertingToUnit(theScalar, unit, error);
                    if(*error) {
                        if(newScalar) OCRelease(newScalar);
                        if(units) OCRelease(units);
                        return NULL;
                    }
                    
                    if(newScalar) {
                        if(SIUnitIsCoherentDerivedUnit(unit)) {
                            OCArrayAppendValue(result, newScalar);
                        }
                        else {
                            int magnitude = log10(fabs(SIScalarDoubleValue(newScalar)));
                            if(abs(magnitude)<6) {
                                OCArrayAppendValue(result, newScalar);
                            }
                        }
                        OCRelease(newScalar);
                    }
                }
            }
            OCRelease(units);
        }
        if(SIDimensionalityHasSameReducedDimensionality(SIDimensionalityForQuantity(kSIQuantityTime,NULL), SIQuantityGetUnitDimensionality((SIQuantityRef)  theScalar)) && SIScalarIsReal(theScalar)) {
            SIUnitRef unit1 = SIUnitForUnderivedSymbol(STR("yr"));
            SIUnitRef unit2 = SIUnitForUnderivedSymbol(STR("month"));
            SIUnitRef unit3 = SIUnitForUnderivedSymbol(STR("wk"));
            SIUnitRef unit4 = SIUnitForUnderivedSymbol(STR("d"));
            SIUnitRef unit5 = SIUnitForUnderivedSymbol(STR("h"));
            SIUnitRef unit6 = SIUnitForUnderivedSymbol(STR("min"));
            SIUnitRef unit7 = SIUnitForUnderivedSymbol(STR("s"));
            SIUnitRef theUnits[7] = {unit1,unit2,unit3,unit4, unit5, unit6, unit7};
            OCArrayRef units = OCArrayCreate((const void **) theUnits, 7,&kOCTypeArrayCallBacks);
            OCStringRef stringValue = SIScalarCreateStringValueSplitByUnits(theScalar, units, true, error);
            OCRelease(units);
            if(stringValue) {
                OCArrayAppendValue(result, stringValue);
                OCRelease(stringValue);
            }
        }
        if(SIDimensionalityHasSameReducedDimensionality(SIDimensionalityForQuantity(kSIQuantityVolume,NULL), SIQuantityGetUnitDimensionality((SIQuantityRef)  theScalar)) && SIScalarIsReal(theScalar)) {
            SIUnitRef unit1 = SIUnitForUnderivedSymbol(STR("gal"));
            SIUnitRef unit2 = SIUnitForUnderivedSymbol(STR("qt"));
            SIUnitRef unit3 = SIUnitForUnderivedSymbol(STR("pt"));
            SIUnitRef unit4 = SIUnitForUnderivedSymbol(STR("cup"));
            SIUnitRef unit5 = SIUnitForUnderivedSymbol(STR("floz"));
            SIUnitRef unit6 = SIUnitForUnderivedSymbol(STR("tbsp"));
            SIUnitRef unit7 = SIUnitForUnderivedSymbol(STR("tsp"));
            SIUnitRef unit8 = SIUnitForUnderivedSymbol(STR("halftsp"));
            SIUnitRef unit9 = SIUnitForUnderivedSymbol(STR("quartertsp"));
            SIUnitRef theUnits[9] = {unit1,unit2,unit3,unit4, unit5, unit6, unit7,unit8,unit9};
            OCArrayRef units = OCArrayCreate((const void **) theUnits, 9,&kOCTypeArrayCallBacks);
            OCStringRef stringValue = SIScalarCreateStringValueSplitByUnits(theScalar, units, false, error);
            OCRelease(units);
            if(stringValue) {
                OCArrayAppendValue(result, stringValue);
                OCRelease(stringValue);
            }
        }
        if(SIDimensionalityHasSameReducedDimensionality(SIDimensionalityForQuantity(kSIQuantityLength,NULL), SIQuantityGetUnitDimensionality((SIQuantityRef)  theScalar)) && SIScalarIsReal(theScalar)) {
            SIUnitRef unit1 = SIUnitForUnderivedSymbol(STR("mi"));
            SIUnitRef unit2 = SIUnitForUnderivedSymbol(STR("ft"));
            SIUnitRef unit3 = SIUnitForUnderivedSymbol(STR("in"));
            SIUnitRef theUnits[3] = {unit1,unit2,unit3};
            OCArrayRef units = OCArrayCreate((const void **) theUnits, 3,&kOCTypeArrayCallBacks);
            OCStringRef stringValue = SIScalarCreateStringValueSplitByUnits(theScalar, units, false, error);
            OCRelease(units);
            if(stringValue) {
                OCArrayAppendValue(result, stringValue);
                OCRelease(stringValue);
            }
        }
        if(SIDimensionalityHasSameReducedDimensionality(SIDimensionalityForQuantity(kSIQuantityLength,NULL), SIQuantityGetUnitDimensionality((SIQuantityRef)  theScalar)) && SIScalarIsReal(theScalar)) {
            SIUnitRef unit1 = SIUnitForUnderivedSymbol(STR("mi"));
            SIUnitRef unit2 = SIUnitForUnderivedSymbol(STR("yd"));
            SIUnitRef unit3 = SIUnitForUnderivedSymbol(STR("ft"));
            SIUnitRef unit4 = SIUnitForUnderivedSymbol(STR("in"));
            SIUnitRef theUnits[4] = {unit1,unit2,unit3,unit4};
            OCArrayRef units = OCArrayCreate((const void **) theUnits, 4,&kOCTypeArrayCallBacks);
            OCStringRef stringValue = SIScalarCreateStringValueSplitByUnits(theScalar, units, false, error);
            OCRelease(units);
            if(stringValue) {
                if(OCArrayBSearchValues(result,OCRangeMake(0, OCArrayGetCount(result)),stringValue,
                                        (OCComparatorFunction)compareOnlyTheStrings,NULL) >= OCArrayGetCount(result) ) {
                    OCArrayAppendValue(result, stringValue);
                    OCRelease(stringValue);
                }
            }
        }
        if(SIDimensionalityHasSameReducedDimensionality(SIDimensionalityForQuantity(kSIQuantityMass,NULL), SIQuantityGetUnitDimensionality((SIQuantityRef)  theScalar)) && SIScalarIsReal(theScalar)) {
            SIUnitRef unit1 = SIUnitForUnderivedSymbol(STR("ton"));
            SIUnitRef unit2 = SIUnitForUnderivedSymbol(STR("lb"));
            SIUnitRef unit3 = SIUnitForUnderivedSymbol(STR("oz"));
            SIUnitRef theUnits[3] = {unit1,unit2,unit3};
            OCArrayRef units = OCArrayCreate((const void **) theUnits, 3,&kOCTypeArrayCallBacks);
            OCStringRef stringValue = SIScalarCreateStringValueSplitByUnits(theScalar, units, false, error);
            OCRelease(units);
            if(stringValue) {
                OCArrayAppendValue(result, stringValue);
                OCRelease(stringValue);
            }
        }
        if(SIDimensionalityHasSameReducedDimensionality(SIDimensionalityForQuantity(kSIQuantityMass,NULL), SIQuantityGetUnitDimensionality((SIQuantityRef)  theScalar)) && SIScalarIsReal(theScalar)) {
            SIUnitRef unit1 = SIUnitForUnderivedSymbol(STR("ton"));
            SIUnitRef unit2 = SIUnitForUnderivedSymbol(STR("st"));
            SIUnitRef unit3 = SIUnitForUnderivedSymbol(STR("lb"));
            SIUnitRef unit4 = SIUnitForUnderivedSymbol(STR("oz"));
            SIUnitRef theUnits[4] = {unit1,unit2,unit3,unit4};
            OCArrayRef units = OCArrayCreate((const void **) theUnits, 4,&kOCTypeArrayCallBacks);
            OCStringRef stringValue = SIScalarCreateStringValueSplitByUnits(theScalar, units, false, error);
            OCRelease(units);
            if(stringValue) {
                if(OCArrayBSearchValues(result,OCRangeMake(0, OCArrayGetCount(result)),stringValue,
                                        (OCComparatorFunction)compareOnlyTheStrings,NULL)>=OCArrayGetCount(result) ) {
                    
                    OCArrayAppendValue(result, stringValue);
                    OCRelease(stringValue);
                }
            }
        }
    }
    return NULL;
}

OCArrayRef SIScalarCreateArrayOfConversionQuantitiesAndUnits(SIScalarRef theScalar, OCStringRef quantity, OCStringRef *error)
{
    if(error) if(*error) return NULL;
    OCArrayRef units = NULL;
    OCArrayRef quantities = NULL;
    OCMutableArrayRef result = OCArrayCreateMutable(0,&kOCTypeArrayCallBacks);
    
    if(theScalar) {
        if(NULL == quantity) {
            quantities = SIDimensionalityCreateArrayOfQuantitiesWithSameReducedDimensionality(SIQuantityGetUnitDimensionality((SIQuantityRef) theScalar));
            if(quantities) {
                OCArrayAppendArray(result, quantities,OCRangeMake(0, OCArrayGetCount(quantities)));
                OCRelease(quantities);
            }
        }
        if(NULL == quantity) units = SIUnitCreateArrayOfConversionUnits(SIQuantityGetUnit((SIQuantityRef) theScalar));
        else units = SIUnitCreateArrayOfUnitsForQuantity(quantity);
        
        if(units) {
            for(uint64_t index = 0; index<OCArrayGetCount(units); index++) {
                SIUnitRef unit = (SIUnitRef) OCArrayGetValueAtIndex(units, index);
                if(unit) {
                    SIScalarRef newScalar = SIScalarCreateByConvertingToUnit(theScalar, unit, error);
                    if(error) {
                        if(*error) {
                            if(newScalar) OCRelease(newScalar);
                            if(units) OCRelease(units);
                            return NULL;
                        }
                    }
                    if(newScalar) {
                        if(SIUnitIsCoherentDerivedUnit(unit)) OCArrayAppendValue(result, unit);
                        else {
                            int magnitude = log10(fabs(SIScalarDoubleValue(newScalar)));
                            if(abs(magnitude)<6)OCArrayAppendValue(result, unit);                            }
                        OCRelease(newScalar);
                    }
                }
            }
            OCRelease(units);
        }
        return result;
    }
    return NULL;
}



#pragma mark Strings and Archiving

void SIScalarShow(SIScalarRef theScalar)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,);
    OCStringRef cf_string = SIScalarCreateStringValue(theScalar);
    if(cf_string) {
        OCStringShow(cf_string);
        OCStringShow(STR("\n"));
        OCRelease(cf_string);
    }
    else fprintf(stdout,"invalid value.");
}

bool SIScalarValidateProposedStringValue(SIScalarRef theScalar, OCStringRef proposedStringValue, OCStringRef *error)
{
    if(error) if(*error) return false;
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,false);
   	IF_NO_OBJECT_EXISTS_RETURN(proposedStringValue,false);
    SIScalarRef proposedValue = SIScalarCreateWithOCString(proposedStringValue,error);
    if(error) if(*error) OCRelease(*error);
    if(proposedValue==NULL) {
        if(error) {
            SIDimensionalityRef dimensionality = SIQuantityGetUnitDimensionality((SIQuantityRef) theScalar);
            OCStringRef dimensionalitySymbol = SIDimensionalityGetSymbol(dimensionality);
            *error = OCStringCreateWithFormat(STR("Unrecognized input. Value must have dimensionality: %@"),dimensionalitySymbol);
        }
        return false;
    }
    else if(!SIQuantityHasSameReducedDimensionality((SIQuantityRef) proposedValue, (SIQuantityRef) theScalar)) {
        if(error) {
            SIDimensionalityRef dimensionality = SIQuantityGetUnitDimensionality((SIQuantityRef) theScalar);
            OCStringRef dimensionalitySymbol = SIDimensionalityGetSymbol(dimensionality);
            *error = OCStringCreateWithFormat(STR("Unrecognized input. Value must have dimensionality: %@"),dimensionalitySymbol);
        }
        OCRelease(proposedValue);
        return false;
    }
    OCRelease(proposedValue);
    return true;
}

OCStringRef SIScalarCreateNumericStringValue(SIScalarRef theScalar)
{
    if(theScalar==NULL) return STR("");
    OCStringRef stringValue = NULL;
    
    switch (theScalar->type) {
        case kOCNumberFloat32Type:
        case kOCNumberFloat32ComplexType:
            stringValue = SIScalarCreateNumericStringValueWithFormat(theScalar, STR("%.7g"));
            break;
        case kSINumberFloat64Type:
        case kSINumberFloat64ComplexType:
            //            stringValue = SIScalarCreateNumericStringValueWithFormat(theScalar, STR("%.16lg"));
            stringValue = SIScalarCreateNumericStringValueWithFormat(theScalar, STR("%.14lg"));
            break;
    }
    return stringValue;
}

OCStringRef SIScalarCreateStringValue(SIScalarRef theScalar)
{
    OCStringRef stringValue = NULL;
    if(theScalar==NULL) return stringValue;
    
    if(OCGetTypeID(theScalar)==OCNumberGetTypeID()) {
        return OCNumberCreateStringValue((OCNumberRef) ((OCTypeRef) theScalar));
    }
    switch (theScalar->type) {
        case kOCNumberFloat32Type:
        case kOCNumberFloat32ComplexType:
            stringValue = SIScalarCreateStringValueWithFormat(theScalar, STR("%.7g"));
            break;
        case kSINumberFloat64Type:
        case kSINumberFloat64ComplexType:
            stringValue = SIScalarCreateStringValueWithFormat(theScalar, STR("%.16lg"));
            break;
    }
    return stringValue;
}

OCStringRef SIScalarCreateStringValueForPart(SIScalarRef theScalar, complexPart thePart)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,NULL);
    SIScalarRef temp = SIScalarCreateByTakingComplexPart(theScalar, thePart);
    OCStringRef string = SIScalarCreateStringValue(temp);
    OCRelease(temp);
    return string;
}

OCStringRef SIScalarCreateNumericStringValueWithFormat(SIScalarRef theScalar, OCStringRef format)
{
    if(theScalar==NULL) return STR("");
    
    switch (theScalar->type) {
        case kOCNumberFloat32Type: {
            float value = SIScalarFloatValue(theScalar);
            if(OCCompareFloatValues(value, 0.0) == kOCCompareEqualTo) value = 0.0;
            
            OCStringRef numericString = OCFloatCreateStringValue(value);
            return numericString;
        }
        case kSINumberFloat64Type: {
            double value = SIScalarDoubleValue(theScalar);
            if(OCCompareDoubleValues(value, 0.0) == kOCCompareEqualTo) value = 0.0;
            
            OCStringRef numericString = OCDoubleComplexCreateStringValue(value,format);
            
            return numericString;
        }
        case kOCNumberFloat32ComplexType: {
            float complex value = SIScalarFloatComplexValue(theScalar);
            OCMutableStringRef cf_string = OCStringCreateMutable(0);
            if(crealf(value)!=0.0 && cimagf(value)!=0.0) OCStringAppend(cf_string, STR("("));
            OCStringRef numericString = OCFloatComplexCreateStringValue(value,format);
            OCStringAppend(cf_string,numericString);
            OCRelease(numericString);
            if(crealf(value)!=0.0 && cimagf(value)!=0.0) OCStringAppend(cf_string, STR(")"));
            return cf_string;
        }
        case kSINumberFloat64ComplexType: {
            double complex value = SIScalarDoubleComplexValue(theScalar);
            OCMutableStringRef cf_string = OCStringCreateMutable(0);
            if(creal(value)!=0.0 && cimag(value)!=0.0) OCStringAppend(cf_string, STR("("));
            OCStringRef numericString = OCDoubleComplexCreateStringValue(value,format);
            OCStringAppend(cf_string,numericString);
            OCRelease(numericString);
            if(creal(value)!=0.0 && cimag(value)!=0.0) OCStringAppend(cf_string, STR(")"));
            return cf_string;
        }
    }
    return NULL;
}

OCStringRef SIScalarCreateUnitString(SIScalarRef theScalar)
{
    OCStringRef unit_symbol;
    if(SIUnitIsDimensionlessAndUnderived(theScalar->unit)) unit_symbol = STR("");
    else unit_symbol = SIUnitCopySymbol(theScalar->unit);
    return unit_symbol;
}

OCStringRef SIScalarCreateStringValueWithFormat(SIScalarRef theScalar, OCStringRef format)
{
    if(theScalar==NULL) return STR("");
    
    OCStringRef unit_symbol = SIScalarCreateUnitString(theScalar);
    
    switch (theScalar->type) {
        case kOCNumberFloat32Type: {
            float value = SIScalarFloatValue(theScalar);
            if(OCCompareFloatValues(value, 0.0) == kOCCompareEqualTo) value = 0.0;
            
            OCStringRef numericString = OCFloatCreateStringValue(value);
            
            OCMutableStringRef cf_string = OCStringCreateMutableCopy(numericString);
            OCRelease(numericString);
            
            if(theScalar->unit) {
                OCStringAppend(cf_string,STR(" "));
                OCStringAppend(cf_string, unit_symbol);
            }
            OCStringTrimWhitespace (cf_string);
            OCRelease(unit_symbol);
            return cf_string;
        }
        case kSINumberFloat64Type: {
            double value = SIScalarDoubleValue(theScalar);
            if(OCCompareDoubleValues(value, 0.0) == kOCCompareEqualTo) value = 0.0;
            OCStringRef numericString = OCDoubleCreateStringValue(value);
            
            OCMutableStringRef cf_string = OCStringCreateMutableCopy(numericString);
            OCRelease(numericString);
            
            if(theScalar->unit) {
                OCStringAppend(cf_string,STR(" "));
                OCStringAppend(cf_string, unit_symbol);
            }
            OCStringTrimWhitespace (cf_string);
            OCRelease(unit_symbol);
            return cf_string;
        }
        case kOCNumberFloat32ComplexType: {
            float complex value = SIScalarFloatComplexValue(theScalar);
            OCMutableStringRef cf_string = OCStringCreateMutable(0);
            if(crealf(value)!=0.0 && cimagf(value)!=0.0) OCStringAppend(cf_string, STR("("));
            OCStringRef numericString = OCFloatComplexCreateStringValue(value,format);
            OCStringAppend(cf_string,numericString);
            OCRelease(numericString);
            if(crealf(value)!=0.0 && cimagf(value)!=0.0) OCStringAppend(cf_string, STR(")"));
            if(theScalar->unit) {
                OCStringAppend(cf_string,STR(" "));
                OCStringAppend(cf_string, unit_symbol);
            }
            OCStringTrimWhitespace (cf_string);
            OCRelease(unit_symbol);
            return cf_string;
        }
        case kSINumberFloat64ComplexType: {
            double complex value = SIScalarDoubleComplexValue(theScalar);
            OCMutableStringRef cf_string = OCStringCreateMutable(0);
            if(creal(value)!=0.0 && cimag(value)!=0.0) OCStringAppend(cf_string, STR("("));
            OCStringRef numericString = OCDoubleComplexCreateStringValue(value,format);
            OCStringAppend(cf_string,numericString);
            OCRelease(numericString);
            if(creal(value)!=0.0 && cimag(value)!=0.0) OCStringAppend(cf_string, STR(")"));
            if(theScalar->unit) {
                OCStringAppend(cf_string,STR(" "));
                OCStringAppend(cf_string, unit_symbol);
            }
            OCStringTrimWhitespace (cf_string);
            OCRelease(unit_symbol);
            return cf_string;
        }
    }
    return NULL;
}

void SIScalarAddToArrayAsStringValue(SIScalarRef theScalar, OCMutableArrayRef array)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,);
   	IF_NO_OBJECT_EXISTS_RETURN(array,);
    OCStringRef stringValue = SIScalarCreateStringValue(theScalar);
    OCArrayAppendValue(array, stringValue);
    OCRelease(stringValue);
}

#pragma mark Tests

bool SIScalarIsReal(SIScalarRef theScalar)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,false);
    double complex value = SIScalarDoubleComplexValue(theScalar);
    if(cimag(value)==0.0) return true;
    return false;
}

bool SIScalarIsImaginary(SIScalarRef theScalar)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,false);
    double complex value = SIScalarDoubleComplexValue(theScalar);
    if(creal(value)==0.0 && cabs(value) != 0.0) return true;
    return false;
}

bool SIScalarIsComplex(SIScalarRef theScalar)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,false);
    double complex value = SIScalarDoubleComplexValue(theScalar);
    if(cimag(value)==0.0) return false;
    return true;
}

bool SIScalarIsZero(SIScalarRef theScalar)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,false);
    double complex value = SIScalarDoubleComplexValue(theScalar);
    if(cimag(value)!=0.0) return false;
    if(creal(value)!=0.0) return false;
    return true;
}

bool SIScalarIsInfinite(SIScalarRef theScalar)
{
    IF_NO_OBJECT_EXISTS_RETURN(theScalar,false);
    switch (SIQuantityGetElementType((SIQuantityRef) theScalar)) {
        case kOCNumberFloat32Type:
            if(isinf(theScalar->value.floatValue)) return true;
        case kSINumberFloat64Type:
            if(isinf(theScalar->value.doubleValue)) return true;
        case kOCNumberFloat32ComplexType:
            if(isinf(crealf(theScalar->value.floatComplexValue))) return true;
            if(isinf(cimagf(theScalar->value.floatComplexValue))) return true;
        case kSINumberFloat64ComplexType:
            if(isinf(creal(theScalar->value.doubleComplexValue))) return true;
            if(isinf(cimag(theScalar->value.doubleComplexValue))) return true;
    }
    return false;
}

bool SIScalarIsRealNonNegativeInteger(SIScalarRef theScalar)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,false);
    if(SIScalarIsComplex(theScalar)) return false;
    double value = SIScalarDoubleValue(theScalar);
    double integerPart;
    double fractionalPart = modf(value, &integerPart);
    if(fractionalPart != 0.0) return false;
    if(integerPart>=0) return true;
    return false;
}

bool SIScalarEqual(SIScalarRef input1,SIScalarRef input2)
{
   	IF_NO_OBJECT_EXISTS_RETURN(input1,false);
    IF_NO_OBJECT_EXISTS_RETURN(input2,false);
    if(input1 == input2) return true;
    
    if(input1->type != input2->type) return false;
    if(!SIUnitEqual(input1->unit, input2->unit)) return false;
    
    switch (input1->type) {
        case kOCNumberFloat32Type: {
            if(input1->value.floatValue != input2->value.floatValue) return false;
            break;
        }
        case kSINumberFloat64Type: {
            if(input1->value.doubleValue != input2->value.doubleValue) return false;
            break;
        }
        case kOCNumberFloat32ComplexType: {
            if(input1->value.floatComplexValue != input2->value.floatComplexValue) return false;
            break;
        }
        case kSINumberFloat64ComplexType: {
            if(input1->value.doubleComplexValue != input2->value.doubleComplexValue) return false;
            break;
        }
    }
    return true;
}

OCComparisonResult SIScalarCompare(SIScalarRef theScalar, SIScalarRef theOtherScalar)
{
    if(NULL==theScalar) {
        IF_NO_OBJECT_EXISTS_RETURN(theScalar,kOCCompareError);
        
    }
    if(NULL==theOtherScalar) {
        IF_NO_OBJECT_EXISTS_RETURN(theOtherScalar,kOCCompareError);
    }
    
    if(!SIDimensionalityHasSameReducedDimensionality(SIQuantityGetUnitDimensionality((SIQuantityRef) theScalar),
                                                     SIQuantityGetUnitDimensionality((SIQuantityRef) theOtherScalar))) return kOCCompareUnequalDimensionalities;
    
    SIMutableScalarRef theOtherConverted = SIScalarCreateMutableCopy(theOtherScalar);
    if(NULL==theOtherConverted) {
        IF_NO_OBJECT_EXISTS_RETURN(theOtherConverted,kOCCompareError);
    }

    SIScalarConvertToUnit(theOtherConverted, SIQuantityGetUnit((SIQuantityRef) theScalar), NULL);
    
    OCComparisonResult result = kOCCompareError;
    switch (theScalar->type) {
        case kOCNumberFloat32Type: {
            switch (theOtherConverted->type) {
                case kOCNumberFloat32Type: {
                    result = OCCompareFloatValues((float) theScalar->value.floatValue, (float) theOtherConverted->value.floatValue);
                    break;
                }
                case kSINumberFloat64Type: {
                    result = OCCompareFloatValues((float) theScalar->value.floatValue, (float) theOtherConverted->value.doubleValue);
                    break;
                }
                case kOCNumberFloat32ComplexType: {
                    if(!SIScalarIsReal(theOtherConverted)) {
                        result =  kOCCompareNoSingleValue;
                        break;
                    }
                    result = OCCompareFloatValues((float) theScalar->value.floatValue, (float) crealf(theOtherConverted->value.floatComplexValue));
                    break;
                }
                case kSINumberFloat64ComplexType: {
                    if(!SIScalarIsReal(theOtherConverted)) {
                        result =  kOCCompareNoSingleValue;
                        break;
                    }
                    result = OCCompareFloatValues((float) theScalar->value.floatValue, (float) creal(theOtherConverted->value.doubleComplexValue));
                    break;
                }
            }
            break;
        }
        case kSINumberFloat64Type: {
            switch (theOtherConverted->type) {
                case kOCNumberFloat32Type: {
                    result = OCCompareFloatValues((float) theScalar->value.doubleValue, (float) theOtherConverted->value.floatValue);
                    break;
                }
                case kSINumberFloat64Type: {
                    result = OCCompareDoubleValues((double) theScalar->value.doubleValue, (double) theOtherConverted->value.doubleValue);
                    break;
                }
                case kOCNumberFloat32ComplexType: {
                    if(!SIScalarIsReal(theOtherConverted)) {
                        result =  kOCCompareNoSingleValue;
                        break;
                    }
                    result = OCCompareFloatValues((float) theScalar->value.doubleValue, (float) crealf(theOtherConverted->value.floatComplexValue));
                    break;
                }
                case kSINumberFloat64ComplexType: {
                    if(!SIScalarIsReal(theOtherConverted)) {
                        result =  kOCCompareNoSingleValue;
                        break;
                    }
                    result = OCCompareDoubleValues((double) theScalar->value.doubleValue, creal(theOtherConverted->value.doubleComplexValue));
                    break;
                }
            }
            break;
        }
        case kOCNumberFloat32ComplexType: {
            switch (theOtherConverted->type) {
                case kOCNumberFloat32Type: {
                    if(!SIScalarIsReal(theScalar)) {
                        result =  kOCCompareNoSingleValue;
                        break;
                    }
                    result = OCCompareFloatValues((float) creal(theScalar->value.floatComplexValue), (float) theOtherConverted->value.floatValue);
                    break;
                }
                case kSINumberFloat64Type: {
                    if(!SIScalarIsReal(theScalar)) {
                        result =  kOCCompareNoSingleValue;
                        break;
                    }
                    result = OCCompareFloatValues((float) crealf(theScalar->value.floatComplexValue), (float) theOtherConverted->value.doubleValue);
                    break;
                }
                case kOCNumberFloat32ComplexType: {
                    OCComparisonResult realResult =  OCCompareFloatValues((float) crealf(theScalar->value.floatComplexValue), (float) crealf(theOtherConverted->value.floatComplexValue));
                    
                    OCComparisonResult imagResult = OCCompareFloatValues((float) cimagf(theScalar->value.floatComplexValue), (float) cimagf(theOtherConverted->value.floatComplexValue));
                    
                    if(realResult == kOCCompareEqualTo && imagResult == kOCCompareEqualTo) result = kOCCompareEqualTo;
                    else result = kOCCompareNoSingleValue;
                    break;
                }
                case kSINumberFloat64ComplexType: {
                    OCComparisonResult realResult = OCCompareFloatValues((float) crealf(theScalar->value.floatComplexValue), (float) creal(theOtherConverted->value.doubleComplexValue));
                    
                    OCComparisonResult imagResult = OCCompareFloatValues((float) cimagf(theScalar->value.floatComplexValue), (float) cimag(theOtherConverted->value.doubleComplexValue));
                    
                    if(realResult == kOCCompareEqualTo && imagResult == kOCCompareEqualTo) result = kOCCompareEqualTo;
                    else result = kOCCompareNoSingleValue;
                    break;
                }
            }
            break;
        }
        case kSINumberFloat64ComplexType: {
            switch (theOtherConverted->type) {
                case kOCNumberFloat32Type: {
                    if(!SIScalarIsReal(theScalar)) {
                        result = kOCCompareNoSingleValue;
                        break;
                    }
                    result = OCCompareFloatValues((float) creal(theScalar->value.doubleComplexValue), (float) theOtherConverted->value.floatValue);
                    break;
                }
                case kSINumberFloat64Type: {
                    if(!SIScalarIsReal(theScalar)) {
                        result = kOCCompareNoSingleValue;
                        break;
                    }
                    result = OCCompareDoubleValues((double) creal(theScalar->value.doubleComplexValue), (double) theOtherConverted->value.doubleValue);
                    break;
                }
                case kOCNumberFloat32ComplexType: {
                    OCComparisonResult realResult = OCCompareFloatValues((float) creal(theScalar->value.doubleComplexValue), (float) crealf(theOtherConverted->value.floatComplexValue));
                    
                    OCComparisonResult imagResult = OCCompareFloatValues((float) cimag(theScalar->value.doubleComplexValue), (float) cimagf(theOtherConverted->value.floatComplexValue));
                    
                    if(realResult == kOCCompareEqualTo && imagResult == kOCCompareEqualTo) result = kOCCompareEqualTo;
                    else result = kOCCompareNoSingleValue;
                    break;
                }
                case kSINumberFloat64ComplexType: {
                    OCComparisonResult realResult = OCCompareDoubleValues((double) creal(theScalar->value.doubleComplexValue), (double) creal(theOtherConverted->value.doubleComplexValue));
                    
                    OCComparisonResult imagResult = OCCompareDoubleValues((double) cimag(theScalar->value.doubleComplexValue), (double) cimag(theOtherConverted->value.doubleComplexValue));
                    
                    if(realResult == kOCCompareEqualTo && imagResult == kOCCompareEqualTo) result = kOCCompareEqualTo;
                    else result = kOCCompareNoSingleValue;
                    break;
                }
            }
            break;
        }
    }
    OCRelease(theOtherConverted);
    return result;
}

OCComparisonResult SIScalarCompareReduced(SIScalarRef theScalar, SIScalarRef theOtherScalar)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,kOCCompareError);
   	IF_NO_OBJECT_EXISTS_RETURN(theOtherScalar,kOCCompareError);
    SIScalarRef theScalarReduced = SIScalarCreateByReducingUnit(theScalar);
    SIScalarRef theOtherScalarReduced = SIScalarCreateByReducingUnit(theOtherScalar);
    OCComparisonResult result = SIScalarCompare(theScalarReduced, theOtherScalarReduced);
    OCRelease(theScalarReduced);
    OCRelease(theOtherScalarReduced);
    return result;
}

OCComparisonResult SIScalarCompareLoose(SIScalarRef theScalar, SIScalarRef theOtherScalar)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,kOCCompareError);
   	IF_NO_OBJECT_EXISTS_RETURN(theOtherScalar,kOCCompareError);
    
    if(!SIDimensionalityEqual(SIQuantityGetUnitDimensionality((SIQuantityRef) theScalar),
                              SIQuantityGetUnitDimensionality((SIQuantityRef) theOtherScalar))) return kOCCompareUnequalDimensionalities;
    
    SIMutableScalarRef theOtherConverted = SIScalarCreateMutableCopy(theOtherScalar);
    SIScalarConvertToUnit(theOtherConverted, SIQuantityGetUnit((SIQuantityRef) theScalar), NULL);
    
    OCComparisonResult result = kOCCompareError;
    switch (theScalar->type) {
        case kOCNumberFloat32Type: {
            switch (theOtherConverted->type) {
                case kOCNumberFloat32Type: {
                    result = OCCompareFloatValuesLoose((float) theScalar->value.floatValue, (float) theOtherConverted->value.floatValue);
                    break;
                }
                case kSINumberFloat64Type: {
                    result = OCCompareFloatValuesLoose((float) theScalar->value.floatValue, (float) theOtherConverted->value.doubleValue);
                    break;
                }
                case kOCNumberFloat32ComplexType: {
                    if(!SIScalarIsReal(theOtherConverted)) {
                        result =  kOCCompareNoSingleValue;
                        break;
                    }
                    result = OCCompareFloatValuesLoose((float) theScalar->value.floatValue, (float) crealf(theOtherConverted->value.floatComplexValue));
                    break;
                }
                case kSINumberFloat64ComplexType: {
                    if(!SIScalarIsReal(theOtherConverted)) {
                        result =  kOCCompareNoSingleValue;
                        break;
                    }
                    result = OCCompareFloatValuesLoose((float) theScalar->value.floatValue, (float) creal(theOtherConverted->value.doubleComplexValue));
                    break;
                }
            }
            break;
        }
        case kSINumberFloat64Type: {
            switch (theOtherConverted->type) {
                case kOCNumberFloat32Type: {
                    result = OCCompareFloatValuesLoose((float) theScalar->value.doubleValue, (float) theOtherConverted->value.floatValue);
                    break;
                }
                case kSINumberFloat64Type: {
                    result = OCCompareDoubleValuesLoose((double) theScalar->value.doubleValue, (double) theOtherConverted->value.doubleValue);
                    break;
                }
                case kOCNumberFloat32ComplexType: {
                    if(!SIScalarIsReal(theOtherConverted)) {
                        result =  kOCCompareNoSingleValue;
                        break;
                    }
                    result = OCCompareFloatValuesLoose((float) theScalar->value.doubleValue, (float) crealf(theOtherConverted->value.floatComplexValue));
                    break;
                }
                case kSINumberFloat64ComplexType: {
                    if(!SIScalarIsReal(theOtherConverted)) {
                        result =  kOCCompareNoSingleValue;
                        break;
                    }
                    result = OCCompareDoubleValuesLoose((double) theScalar->value.doubleValue, creal(theOtherConverted->value.doubleComplexValue));
                    break;
                }
            }
            break;
        }
        case kOCNumberFloat32ComplexType: {
            switch (theOtherConverted->type) {
                case kOCNumberFloat32Type: {
                    if(!SIScalarIsReal(theScalar)) {
                        result =  kOCCompareNoSingleValue;
                        break;
                    }
                    result = OCCompareFloatValuesLoose((float) creal(theScalar->value.floatComplexValue), (float) theOtherConverted->value.floatValue);
                    break;
                }
                case kSINumberFloat64Type: {
                    if(!SIScalarIsReal(theScalar)) {
                        result =  kOCCompareNoSingleValue;
                        break;
                    }
                    result = OCCompareFloatValuesLoose((float) crealf(theScalar->value.floatComplexValue), (float) theOtherConverted->value.doubleValue);
                    break;
                }
                case kOCNumberFloat32ComplexType: {
                    OCComparisonResult realResult =  OCCompareFloatValuesLoose((float) crealf(theScalar->value.floatComplexValue), (float) crealf(theOtherConverted->value.floatComplexValue));
                    
                    OCComparisonResult imagResult = OCCompareFloatValuesLoose((float) cimagf(theScalar->value.floatComplexValue), (float) cimagf(theOtherConverted->value.floatComplexValue));
                    
                    if(realResult == kOCCompareEqualTo && imagResult == kOCCompareEqualTo) result = kOCCompareEqualTo;
                    else result = kOCCompareNoSingleValue;
                    break;
                }
                case kSINumberFloat64ComplexType: {
                    OCComparisonResult realResult = OCCompareFloatValuesLoose((float) crealf(theScalar->value.floatComplexValue), (float) creal(theOtherConverted->value.doubleComplexValue));
                    
                    OCComparisonResult imagResult = OCCompareFloatValuesLoose((float) cimagf(theScalar->value.floatComplexValue), (float) cimag(theOtherConverted->value.doubleComplexValue));
                    
                    if(realResult == kOCCompareEqualTo && imagResult == kOCCompareEqualTo) result = kOCCompareEqualTo;
                    else result = kOCCompareNoSingleValue;
                    break;
                }
            }
            break;
        }
        case kSINumberFloat64ComplexType: {
            switch (theOtherConverted->type) {
                case kOCNumberFloat32Type: {
                    if(!SIScalarIsReal(theScalar)) {
                        result = kOCCompareNoSingleValue;
                        break;
                    }
                    result = OCCompareFloatValuesLoose((float) creal(theScalar->value.doubleComplexValue), (float) theOtherConverted->value.floatValue);
                    break;
                }
                case kSINumberFloat64Type: {
                    if(!SIScalarIsReal(theScalar)) {
                        result = kOCCompareNoSingleValue;
                        break;
                    }
                    result = OCCompareDoubleValuesLoose((double) creal(theScalar->value.doubleComplexValue), (double) theOtherConverted->value.doubleValue);
                    break;
                }
                case kOCNumberFloat32ComplexType: {
                    OCComparisonResult realResult = OCCompareFloatValuesLoose((float) creal(theScalar->value.doubleComplexValue), (float) crealf(theOtherConverted->value.floatComplexValue));
                    
                    OCComparisonResult imagResult = OCCompareFloatValuesLoose((float) cimag(theScalar->value.doubleComplexValue), (float) cimag(theOtherConverted->value.floatComplexValue));
                    
                    if(realResult == kOCCompareEqualTo && imagResult == kOCCompareEqualTo) result = kOCCompareEqualTo;
                    else result = kOCCompareNoSingleValue;
                    break;
                }
                case kSINumberFloat64ComplexType: {
                    OCComparisonResult realResult = OCCompareDoubleValuesLoose((double) creal(theScalar->value.doubleComplexValue), (double) creal(theOtherConverted->value.doubleComplexValue));
                    
                    OCComparisonResult imagResult = OCCompareDoubleValuesLoose((double) cimag(theScalar->value.doubleComplexValue), (double) cimag(theOtherConverted->value.doubleComplexValue));
                    
                    if(realResult == kOCCompareEqualTo && imagResult == kOCCompareEqualTo) result = kOCCompareEqualTo;
                    else result = kOCCompareNoSingleValue;
                    break;
                }
            }
            break;
        }
    }
    OCRelease(theOtherConverted);
    return result;
}
