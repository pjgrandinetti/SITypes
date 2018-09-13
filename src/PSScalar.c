//
//  PSScalar.c
//  SITypes
//
//  Created by philip on 6/14/17.
//  Copyright © 2017 PhySy Ltd. All rights reserved.
//

#include "SILibrary.h"

static OCTypeID kPSScalarID = _kOCNotATypeID;

// PSScalar Opaque Type
struct __PSScalar {
    OCBase _base;

    // __PSQuantity Type attributes
    SIUnitRef       unit;
    numberType      type;
    __PSNumber      value;
};

static bool __PSScalarEqual(const void * theType1, const void * theType2)
{
    PSScalarRef theNumber1 = (PSScalarRef) theType1;
    PSScalarRef theNumber2 = (PSScalarRef) theType2;
    if(theNumber1->_base.typeID != theNumber2->_base.typeID) return false;
    
    if(NULL == theNumber1 || NULL == theNumber2) return false;
    if(theNumber1 == theNumber2) return true;
    if(theNumber1->type != theNumber2->type) return false;
    if(!OCTypeEqual(theNumber1->unit, theNumber2->unit)) return false;

    switch (theNumber1->type) {
        case kPSNumberFloat32Type: {
            if(theNumber1->value.floatValue != theNumber2->value.floatValue) return false;
            break;
        }
        case kPSNumberFloat64Type: {
            if(theNumber1->value.doubleValue != theNumber2->value.doubleValue) return false;
            break;
        }
        case kPSNumberFloat32ComplexType: {
            if(theNumber1->value.floatComplexValue != theNumber2->value.floatComplexValue) return false;
            break;
        }
        case kPSNumberFloat64ComplexType: {
            if(theNumber1->value.floatComplexValue != theNumber2->value.doubleComplexValue) return false;
            break;
        }
    }
    return true;
}

static void __PSScalarFinalize(const void * theType)
{
    if(NULL == theType) return;
    PSScalarRef theNumber = (PSScalarRef) theType;
    free((void *)theNumber);
}

static OCStringRef __PSScalarCopyFormattingDescription(OCTypeRef theType)
{
    PSScalarRef theNumber = (PSScalarRef) theType;
    switch (theNumber->type) {
        case kPSNumberFloat32Type: {
            return  OCStringCreateWithFormat(STR("%f %@"), theNumber->value.floatValue, theNumber->unit);
            break;
        }
        case kPSNumberFloat64Type: {
            return  OCStringCreateWithFormat(STR("%lf %@"), theNumber->value.doubleValue, theNumber->unit);
            break;
        }
        case kPSNumberFloat32ComplexType: {
            return  OCStringCreateWithFormat(STR("%f+I•%f %@"), crealf(theNumber->value.floatComplexValue), cimagf(theNumber->value.floatComplexValue), theNumber->unit);
            break;
        }
        case kPSNumberFloat64ComplexType: {
            return  OCStringCreateWithFormat(STR("%lf+I•%lf %@"), creal(theNumber->value.doubleComplexValue), cimag(theNumber->value.doubleComplexValue), theNumber->unit);
            break;
        }
    }
}

OCTypeID PSScalarGetTypeID(void)
{
    if(kPSScalarID == _kOCNotATypeID) kPSScalarID = OCRegisterType("PSScalar");
    return kPSScalarID;
}

static struct __PSScalar *PSScalarAllocate()
{
    struct __PSScalar *theNumber = malloc(sizeof(struct __PSScalar));
    if(NULL == theNumber) return NULL;
    theNumber->_base.typeID = PSScalarGetTypeID();
    theNumber->_base.retainCount = 1;
    theNumber->_base.finalize = __PSScalarFinalize;
    theNumber->_base.equal = __PSScalarEqual;
    theNumber->_base.copyFormattingDesc = __PSScalarCopyFormattingDescription;
    
    theNumber->unit = NULL;
    theNumber->type = kPSNumberFloat32Type;
    return theNumber;
}


PSScalarRef PSScalarCreate(SIUnitRef unit, numberType type, void *value)
{
    struct __PSScalar *theNumber = PSScalarAllocate();
    if(NULL == theNumber) return NULL;
    
    theNumber->type = type;
    switch (type) {
        case kPSNumberFloat32Type: {
            float *ptr = (float *) value;
            theNumber->value.floatValue = *ptr;
            break;
        }
        case kPSNumberFloat64Type: {
            double *ptr = (double *) value;
            theNumber->value.doubleValue = *ptr;
            break;
        }
        case kPSNumberFloat32ComplexType: {
            float complex *ptr = (float complex *) value;
            theNumber->value.floatComplexValue = *ptr;
            break;
        }
        case kPSNumberFloat64ComplexType: {
            double complex *ptr = (double complex *) value;
            theNumber->value.doubleComplexValue = *ptr;
            break;
        }
    }
    if(unit) theNumber->unit = unit;
    else theNumber->unit = SIUnitDimensionlessAndUnderived();
    return (PSScalarRef) theNumber;

    return theNumber;
}


static PSMutableScalarRef PSScalarCreateMutable(SIUnitRef unit,  numberType elementType, void *value)
{
    return (PSMutableScalarRef) PSScalarCreate(unit, elementType, value);
}

/*
 @function PSScalarCreateCopy
 @abstract Creates a copy of a scalar
 @param theScalar The scalar.
 @result a copy of the scalar.
 */
PSScalarRef PSScalarCreateCopy(PSScalarRef theScalar)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,NULL);
    return PSScalarCreate(theScalar->unit, theScalar->type, (void *) &theScalar->value);
}

PSMutableScalarRef PSScalarCreateMutableCopy(PSScalarRef theScalar)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,NULL);
    return PSScalarCreateMutable(theScalar->unit,theScalar->type,(void *) &theScalar->value);
}

//PSScalarRef PSScalarCreateWithInt32(int32_t input_value)
//{
//   return PSScalarCreate(NULL, kPSNumberSInt32Type, &input_value);
//}
//
//PSMutableScalarRef PSScalarCreateMutableWithInt32(int32_t input_value)
//{
//    return PSScalarCreateMutable(NULL, kPSNumberSInt32Type, &input_value);
//}
//
//PSScalarRef PSScalarCreateWithInt64(int64_t input_value)
//{
//    return PSScalarCreate(NULL, kPSNumberSInt64Type, &input_value);
//}
//
//PSMutableScalarRef PSScalarCreateMutableWithInt64(int64_t input_value)
//{
//    return PSScalarCreateMutable(NULL, kPSNumberSInt64Type, &input_value);
//}
//
PSScalarRef PSScalarCreateWithFloat(float input_value, SIUnitRef unit)
{
    return PSScalarCreate(unit, kPSNumberFloat32Type, &input_value);
}

PSMutableScalarRef PSScalarCreateMutableWithFloat(float input_value, SIUnitRef unit)
{
    return PSScalarCreateMutable(unit, kPSNumberFloat32Type, &input_value);
}

PSScalarRef PSScalarCreateWithDouble(double input_value, SIUnitRef unit)
{
    return PSScalarCreate(unit, kPSNumberFloat64Type, &input_value);
}

PSMutableScalarRef PSScalarCreateMutableWithDouble(double input_value, SIUnitRef unit)
{
    return PSScalarCreateMutable(unit, kPSNumberFloat64Type, &input_value);
}

PSScalarRef PSScalarCreateWithFloatComplex(float complex input_value, SIUnitRef unit)
{
    return PSScalarCreate(unit, kPSNumberFloat32ComplexType, &input_value);
}

PSMutableScalarRef PSScalarCreateMutableWithFloatComplex(float complex input_value, SIUnitRef unit)
{
    return PSScalarCreateMutable(unit, kPSNumberFloat32ComplexType, &input_value);
}

PSScalarRef PSScalarCreateWithDoubleComplex(double complex input_value, SIUnitRef unit)
{
    return PSScalarCreate(unit, kPSNumberFloat64ComplexType, &input_value);
}

PSMutableScalarRef PSScalarCreateMutableWithDoubleComplex(double complex input_value, SIUnitRef unit)
{
    return PSScalarCreateMutable(unit, kPSNumberFloat64ComplexType, &input_value);
}

#pragma mark Accessors

void PSScalarSetElementType(PSMutableScalarRef theScalar, numberType elementType)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,);
    switch (theScalar->type) {
        case kPSNumberFloat32Type:{
            float value = theScalar->value.floatValue;
            theScalar->type = elementType;
            switch (elementType) {
                case kPSNumberFloat32Type:
                    theScalar->value.floatValue = value;
                    return;
                case kPSNumberFloat64Type:
                    theScalar->value.doubleValue = value;
                    return;
                case kPSNumberFloat32ComplexType:
                    theScalar->value.floatComplexValue = value;
                    return;
                case kPSNumberFloat64ComplexType:
                    theScalar->value.doubleComplexValue = value;
                    return;
            }
            break;
        }
        case kPSNumberFloat64Type:{
            double value = theScalar->value.doubleValue;
            theScalar->type = elementType;
            switch (elementType) {
                case kPSNumberFloat32Type:
                    theScalar->value.floatValue = value;
                    return;
                case kPSNumberFloat64Type:
                    theScalar->value.doubleValue = value;
                    return;
                case kPSNumberFloat32ComplexType:
                    theScalar->value.floatComplexValue = value;
                    return;
                case kPSNumberFloat64ComplexType:
                    theScalar->value.doubleComplexValue = value;
                    return;
            }
            break;
        }
        case kPSNumberFloat32ComplexType:{
            float complex value = theScalar->value.floatComplexValue;
            theScalar->type = elementType;
            switch (elementType) {
                case kPSNumberFloat32Type:
                    theScalar->value.floatValue = value;
                    return;
                case kPSNumberFloat64Type:
                    theScalar->value.doubleValue = value;
                    return;
                case kPSNumberFloat32ComplexType:
                    theScalar->value.floatComplexValue = value;
                    return;
                case kPSNumberFloat64ComplexType:
                    theScalar->value.doubleComplexValue = value;
                    return;
            }
            break;
        }
        case kPSNumberFloat64ComplexType:{
            double complex value = theScalar->value.doubleComplexValue;
            theScalar->type = elementType;
            switch (elementType) {
                case kPSNumberFloat32Type:
                    theScalar->value.floatValue = value;
                    return;
                case kPSNumberFloat64Type:
                    theScalar->value.doubleValue = value;
                    return;
                case kPSNumberFloat32ComplexType:
                    theScalar->value.floatComplexValue = value;
                    return;
                case kPSNumberFloat64ComplexType:
                    theScalar->value.doubleComplexValue = value;
                    return;
            }
            break;
        }
    }
    
    return;
}

__PSNumber PSScalarGetValue(PSScalarRef theScalar)
{
    return theScalar->value;
}

void PSScalarSetFloatValue(PSMutableScalarRef theScalar, float value)
{
    theScalar->type= kPSNumberFloat32Type;
    theScalar->value.floatValue = value;
}

void PSScalarSetDoubleValue(PSMutableScalarRef theScalar, double value)
{
    theScalar->type= kPSNumberFloat64Type;
    theScalar->value.doubleValue = value;
}

void PSScalarSetFloatComplexValue(PSMutableScalarRef theScalar, float complex value)
{
    theScalar->type= kPSNumberFloat32ComplexType;
    theScalar->value.floatComplexValue = value;
}

void PSScalarSetDoubleComplexValue(PSMutableScalarRef theScalar, double complex value)
{
    theScalar->type= kPSNumberFloat64ComplexType;
    theScalar->value.doubleComplexValue = value;
}


float PSScalarFloatValue(PSScalarRef theScalar)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,0);
    switch (theScalar->type) {
        case kPSNumberFloat32Type:
            return (float) theScalar->value.floatValue;
        case kPSNumberFloat64Type:
            return (float) theScalar->value.doubleValue;
        case kPSNumberFloat32ComplexType:
            return (float) theScalar->value.floatComplexValue;
        case kPSNumberFloat64ComplexType:
            return (float) theScalar->value.doubleComplexValue;
    }
    return nan(NULL);
}

double PSScalarDoubleValue(PSScalarRef theScalar)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,0);
    switch (theScalar->type) {
        case kPSNumberFloat32Type:
            return (double) theScalar->value.floatValue;
        case kPSNumberFloat64Type:
            return (double) theScalar->value.doubleValue;
        case kPSNumberFloat32ComplexType:
            return (double) theScalar->value.floatComplexValue;
        case kPSNumberFloat64ComplexType:
            return (double) theScalar->value.doubleComplexValue;
    }
    return nan(NULL);
}

float complex PSScalarFloatComplexValue(PSScalarRef theScalar)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,0);
    switch (theScalar->type) {
        case kPSNumberFloat32Type:
            return (float complex) theScalar->value.floatValue;
        case kPSNumberFloat64Type:
            return (float complex) theScalar->value.doubleValue;
        case kPSNumberFloat32ComplexType:
            return (float complex) theScalar->value.floatComplexValue;
        case kPSNumberFloat64ComplexType:
            return (float complex) theScalar->value.doubleComplexValue;
    }
    return nan(NULL);
}

double complex PSScalarDoubleComplexValue(PSScalarRef theScalar)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,0);
    switch (theScalar->type) {
        case kPSNumberFloat32Type:
            return (double complex) theScalar->value.floatValue;
        case kPSNumberFloat64Type:
            return (double complex) theScalar->value.doubleValue;
        case kPSNumberFloat32ComplexType:
            return (double complex) theScalar->value.floatComplexValue;
        case kPSNumberFloat64ComplexType:
            return (double complex) theScalar->value.doubleComplexValue;
    }
    return nan(NULL);
}

double PSScalarMagnitudeValue(PSScalarRef theScalar)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,0);
    double complex value = PSScalarDoubleComplexValue(theScalar);
    return cabs(value);
}

double PSScalarArgumentValue(PSScalarRef theScalar)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,0);
    double complex value = PSScalarDoubleComplexValue(theScalar);
    return cargument(value);
}

float PSScalarFloatValueInCoherentUnit(PSScalarRef theScalar)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,nan(NULL));
    SIUnitRef coherentUnit = SIUnitFindCoherentSIUnitWithDimensionality(SIUnitGetDimensionality(theScalar->unit));
    double conversion = SIUnitConversion(theScalar->unit,coherentUnit);
    
    switch(theScalar->type) {
        case kPSNumberFloat32Type: {
            return theScalar->value.floatValue*conversion;
        }
        case kPSNumberFloat64Type: {
            return theScalar->value.doubleValue*conversion;
        }
        case kPSNumberFloat32ComplexType: {
            return theScalar->value.floatComplexValue*conversion;
        }
        case kPSNumberFloat64ComplexType: {
            return theScalar->value.doubleComplexValue*conversion;
        }
    }
}

double PSScalarDoubleValueInCoherentUnit(PSScalarRef theScalar)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,nan(NULL));
    SIUnitRef coherentUnit = SIUnitFindCoherentSIUnitWithDimensionality(SIUnitGetDimensionality(theScalar->unit));
    double conversion = SIUnitConversion(theScalar->unit,coherentUnit);
    
    switch(theScalar->type) {
        case kPSNumberFloat32Type: {
            return theScalar->value.floatValue*conversion;
        }
        case kPSNumberFloat64Type: {
            return theScalar->value.doubleValue*conversion;
        }
        case kPSNumberFloat32ComplexType: {
            return theScalar->value.floatComplexValue*conversion;
        }
        case kPSNumberFloat64ComplexType: {
            return theScalar->value.doubleComplexValue*conversion;
        }
    }
}

float complex PSScalarFloatComplexValueInCoherentUnit(PSScalarRef theScalar)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,nan(NULL));
    SIUnitRef coherentUnit = SIUnitFindCoherentSIUnitWithDimensionality(SIUnitGetDimensionality(theScalar->unit));
    double conversion = SIUnitConversion(theScalar->unit,coherentUnit);
    
    switch(theScalar->type) {
        case kPSNumberFloat32Type: {
            return theScalar->value.floatValue*conversion;
        }
        case kPSNumberFloat64Type: {
            return theScalar->value.doubleValue*conversion;
        }
        case kPSNumberFloat32ComplexType: {
            return theScalar->value.floatComplexValue*conversion;
        }
        case kPSNumberFloat64ComplexType: {
            return theScalar->value.doubleComplexValue*conversion;
        }
    }
}

double complex PSScalarDoubleComplexValueInCoherentUnit(PSScalarRef theScalar)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,nan(NULL));
    SIUnitRef coherentUnit = SIUnitFindCoherentSIUnitWithDimensionality(SIUnitGetDimensionality(theScalar->unit));
    double conversion = SIUnitConversion(theScalar->unit,coherentUnit);
    
    switch(theScalar->type) {
        case kPSNumberFloat32Type: {
            return theScalar->value.floatValue*conversion;
        }
        case kPSNumberFloat64Type: {
            return theScalar->value.doubleValue*conversion;
        }
        case kPSNumberFloat32ComplexType: {
            return theScalar->value.floatComplexValue*conversion;
        }
        case kPSNumberFloat64ComplexType: {
            return theScalar->value.doubleComplexValue*conversion;
        }
    }
}

float PSScalarFloatValueInUnit(PSScalarRef theScalar, SIUnitRef unit, bool *success)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,nan(NULL));
    if(SIDimensionalityHasSameReducedDimensionality(PSQuantityGetUnitDimensionality((PSQuantityRef) theScalar),SIUnitGetDimensionality(unit))) {
        double conversion = SIUnitConversion(theScalar->unit,unit);
        switch(theScalar->type) {
            case kPSNumberFloat32Type: {
                return theScalar->value.floatValue*conversion;
            }
            case kPSNumberFloat64Type: {
                return theScalar->value.doubleValue*conversion;
            }
            case kPSNumberFloat32ComplexType: {
                return theScalar->value.floatComplexValue*conversion;
            }
            case kPSNumberFloat64ComplexType: {
                return theScalar->value.doubleComplexValue*conversion;
            }
        }
    }
    if(success) *success *= false;
    return nan(NULL);
}

double PSScalarDoubleValueInUnit(PSScalarRef theScalar, SIUnitRef unit, bool *success)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,nan(NULL));
    if(SIDimensionalityHasSameReducedDimensionality(PSQuantityGetUnitDimensionality((PSQuantityRef) theScalar),SIUnitGetDimensionality(unit))) {
        double conversion = SIUnitConversion(theScalar->unit,unit);
        switch(theScalar->type) {
            case kPSNumberFloat32Type: {
                return theScalar->value.floatValue*conversion;
            }
            case kPSNumberFloat64Type: {
                return theScalar->value.doubleValue*conversion;
            }
            case kPSNumberFloat32ComplexType: {
                return theScalar->value.floatComplexValue*conversion;
            }
            case kPSNumberFloat64ComplexType: {
                return theScalar->value.doubleComplexValue*conversion;
            }
        }
    }
    if(success) *success *= false;
    return nan(NULL);
}

float complex PSScalarFloatComplexValueInUnit(PSScalarRef theScalar, SIUnitRef unit, bool *success)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,nan(NULL));
    if(SIDimensionalityHasSameReducedDimensionality(PSQuantityGetUnitDimensionality((PSQuantityRef) theScalar),SIUnitGetDimensionality(unit))) {
        double conversion = SIUnitConversion(theScalar->unit,unit);
        switch(theScalar->type) {
            case kPSNumberFloat32Type: {
                return theScalar->value.floatValue*conversion;
            }
            case kPSNumberFloat64Type: {
                return theScalar->value.doubleValue*conversion;
            }
            case kPSNumberFloat32ComplexType: {
                return theScalar->value.floatComplexValue*conversion;
            }
            case kPSNumberFloat64ComplexType: {
                return theScalar->value.doubleComplexValue*conversion;
            }
        }
    }
    if(success) *success *= false;
    return nan(NULL);
}

double complex PSScalarDoubleComplexValueInUnit(PSScalarRef theScalar, SIUnitRef unit, bool *success)
{
    if(NULL==theScalar) {
        *success = false;
        return nan(NULL);
    }
    if(SIDimensionalityHasSameReducedDimensionality(PSQuantityGetUnitDimensionality((PSQuantityRef) theScalar),SIUnitGetDimensionality(unit))) {
        double conversion = SIUnitConversion(theScalar->unit,unit);
        switch(theScalar->type) {
            case kPSNumberFloat32Type: {
                return theScalar->value.floatValue*conversion;
            }
            case kPSNumberFloat64Type: {
                return theScalar->value.doubleValue*conversion;
            }
            case kPSNumberFloat32ComplexType: {
                return theScalar->value.floatComplexValue*conversion;
            }
            case kPSNumberFloat64ComplexType: {
                return theScalar->value.doubleComplexValue*conversion;
            }
        }
    }
    if(success) *success *= false;
    return nan(NULL);
}

#pragma mark Operations

PSScalarRef PSScalarCreateByConvertingToNumberType(PSScalarRef theScalar, numberType elementType)
{
    IF_NO_OBJECT_EXISTS_RETURN(theScalar,NULL);
    PSScalarRef result = PSScalarCreateCopy(theScalar);
    PSScalarSetElementType((PSMutableScalarRef) result, elementType);
    return result;
}

bool PSScalarTakeComplexPart(PSMutableScalarRef theScalar, complexPart part)
{
    IF_NO_OBJECT_EXISTS_RETURN(theScalar,false);
    switch (theScalar->type) {
        case kPSNumberFloat32Type:{
            if(part == kPSImaginaryPart || part == kPSArgumentPart) {
                theScalar->value.floatValue = 0;
                return true;
            }
            if(part == kPSMagnitudePart) {
                theScalar->value.floatValue = fabsf(theScalar->value.floatValue);
                return true;
            }
            if(part == kPSRealPart) return true;
            break;
        }
        case kPSNumberFloat64Type: {
            if(part == kPSImaginaryPart || part == kPSArgumentPart) {
                theScalar->value.doubleValue = 0;
                return true;
            }
            if(part == kPSMagnitudePart) {
                theScalar->value.doubleValue = fabs(theScalar->value.doubleValue);
                return true;
            }
            if(part == kPSRealPart) return true;
            break;
        }
        case kPSNumberFloat32ComplexType: {
            if(part == kPSRealPart) {
                theScalar->value.floatValue = creal(theScalar->value.floatComplexValue);
                theScalar->type = kPSNumberFloat32Type;
                return true;
            }
            if(part == kPSImaginaryPart) {
                theScalar->value.floatValue = cimag(theScalar->value.floatComplexValue);
                theScalar->type = kPSNumberFloat32Type;
                return true;
            }
            if(part == kPSArgumentPart) {
                theScalar->value.floatValue = cargument(theScalar->value.floatComplexValue);
                theScalar->type = kPSNumberFloat32Type;
                theScalar->unit = SIUnitForUnderivedSymbol(STR("rad"));
                return true;
            }
            if(part == kPSMagnitudePart) {
                theScalar->value.floatValue = cabs(theScalar->value.floatComplexValue);
                theScalar->type = kPSNumberFloat32Type;
                return true;
            }
            break;
        }
        case kPSNumberFloat64ComplexType: {
            if(part == kPSRealPart) {
                theScalar->value.doubleValue = creal(theScalar->value.doubleComplexValue);
                theScalar->type = kPSNumberFloat64Type;
                return true;
            }
            if(part == kPSImaginaryPart) {
                theScalar->value.doubleValue = cimag(theScalar->value.doubleComplexValue);
                theScalar->type = kPSNumberFloat64Type;
                return true;
            }
            if(part == kPSArgumentPart) {
                theScalar->value.doubleValue = cargument(theScalar->value.doubleComplexValue);
                theScalar->type = kPSNumberFloat64Type;
                theScalar->unit = SIUnitForUnderivedSymbol(STR("rad"));
                return true;
            }
            if(part == kPSMagnitudePart) {
                theScalar->value.doubleValue = cabs(theScalar->value.doubleComplexValue);
                theScalar->type = kPSNumberFloat64Type;
                return true;
            }
            break;
        }
    }
    return false;
}

PSScalarRef PSScalarCreateByTakingComplexPart(PSScalarRef theScalar, complexPart part)
{
    IF_NO_OBJECT_EXISTS_RETURN(theScalar,NULL);
    PSScalarRef result = PSScalarCreateCopy(theScalar);
    if(PSScalarTakeComplexPart((PSMutableScalarRef) result, part)) return result;
    OCRelease(result);
    return NULL;
}

bool PSScalarReduceUnit(PSMutableScalarRef theScalar)
{
    IF_NO_OBJECT_EXISTS_RETURN(theScalar,false);
    switch (theScalar->type) {
        case kPSNumberFloat32Type:
        {
            double unit_multiplier = 1.0;
            SIUnitRef reducedUnit = SIUnitByReducing(theScalar->unit,&unit_multiplier);
            theScalar->unit = reducedUnit;
            theScalar->value.floatValue = theScalar->value.floatValue*unit_multiplier;
            return true;
        }
        case kPSNumberFloat64Type:
        {
            double unit_multiplier = 1.0;
            SIUnitRef reducedUnit = SIUnitByReducing(theScalar->unit,&unit_multiplier);
            theScalar->unit = reducedUnit;
            theScalar->value.doubleValue = theScalar->value.doubleValue*unit_multiplier;
            return true;
        }
        case kPSNumberFloat32ComplexType:
        {
            double unit_multiplier = 1.0;
            SIUnitRef reducedUnit = SIUnitByReducing(theScalar->unit,&unit_multiplier);
            theScalar->unit = reducedUnit;
            theScalar->value.floatComplexValue = theScalar->value.floatComplexValue*unit_multiplier;
            return true;
        }
        case kPSNumberFloat64ComplexType:
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

PSScalarRef PSScalarCreateByReducingUnit(PSScalarRef theScalar)
{
    IF_NO_OBJECT_EXISTS_RETURN(theScalar,NULL);
    PSScalarRef result = PSScalarCreateCopy(theScalar);
    if(PSScalarReduceUnit((PSMutableScalarRef) result)) return result;
    if(result) OCRelease(result);
    return NULL;
}

bool PSScalarConvertToUnit(PSMutableScalarRef theScalar, SIUnitRef unit, OCStringRef *error)
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
            //        case kPSNumberSInt32Type: {
            //            theScalar->value.int32Value = theScalar->value.int32Value*conversion;
            //            return true;
            //        }
            //        case kPSNumberSInt64Type: {
            //            theScalar->value.int64Value = theScalar->value.int64Value*conversion;
            //            return true;
            //        }
        case kPSNumberFloat32Type: {
            theScalar->value.floatValue = theScalar->value.floatValue*conversion;
            return true;
        }
        case kPSNumberFloat64Type: {
            theScalar->value.doubleValue = theScalar->value.doubleValue*conversion;
            return true;
        }
        case kPSNumberFloat32ComplexType: {
            theScalar->value.floatComplexValue = theScalar->value.floatComplexValue*conversion;
            return true;
        }
        case kPSNumberFloat64ComplexType: {
            theScalar->value.doubleComplexValue = theScalar->value.doubleComplexValue*conversion;
            return true;
        }
            
    }
    return false;
}

PSScalarRef PSScalarCreateByConvertingToUnit(PSScalarRef theScalar, SIUnitRef unit, OCStringRef *error)
{
    if(error) if(*error) return NULL;
    IF_NO_OBJECT_EXISTS_RETURN(theScalar,NULL);
    PSScalarRef result = PSScalarCreateCopy(theScalar);
    if(PSScalarConvertToUnit((PSMutableScalarRef) result, unit, error)) return result;
    if(result) OCRelease(result);
    return NULL;
}

bool PSScalarConvertToCoherentUnit(PSMutableScalarRef theScalar, OCStringRef *error)
{
    if(error) if(*error) return false;
    IF_NO_OBJECT_EXISTS_RETURN(theScalar,false);
    SIUnitRef coherentUnit = SIUnitFindCoherentSIUnitWithDimensionality(SIUnitGetDimensionality(theScalar->unit));
    return PSScalarConvertToUnit(theScalar, coherentUnit, error);
}

PSScalarRef PSScalarCreateByConvertingToCoherentUnit(PSScalarRef theScalar, OCStringRef *error)
{
    if(error) if(*error) return NULL;
    IF_NO_OBJECT_EXISTS_RETURN(theScalar,NULL);
    PSScalarRef result = PSScalarCreateCopy(theScalar);
    if(PSScalarConvertToCoherentUnit((PSMutableScalarRef) result, error)) return result;
    if(result)  OCRelease(result);
    return NULL;
}

bool PSScalarBestConversionForQuantity(PSMutableScalarRef theScalar, OCStringRef quantity, OCStringRef *error)
{
    if(error) if(*error) return NULL;
    if(NULL==quantity) return false;
    OCArrayRef units = SIUnitCreateArrayOfUnitsForQuantity(quantity);
    if(units==NULL) return false;
    
    PSMutableScalarRef trialScalar = PSScalarCreateMutableCopy(theScalar);
    
    if(!PSScalarConvertToUnit(trialScalar, OCArrayGetValueAtIndex(units, 0), error)) {
        OCRelease(units);
        OCRelease(trialScalar);
        return false;
    }
    int originalMagnitude = fabs(log10(fabs(PSScalarDoubleValue(theScalar))));
    int magnitude = originalMagnitude;
    uint64_t best = 0;
    for(uint64_t index=1; index<OCArrayGetCount(units); index++) {
        SIUnitRef unit = OCArrayGetValueAtIndex(units, index);
        if(!PSScalarConvertToUnit(trialScalar, unit, error)) {
            OCRelease(trialScalar);
            OCRelease(units);
            return false;
        }
        int trialMagnitude = fabs(log10(fabs(PSScalarDoubleValue(trialScalar))));
        if(trialMagnitude < magnitude) {
            best = index;
            magnitude = trialMagnitude;
        }
    }
    bool result = true;
    if(abs(originalMagnitude - magnitude)>2) {
        result = PSScalarConvertToUnit(theScalar, OCArrayGetValueAtIndex(units, best), error);
    }
    OCRelease(trialScalar);
    OCRelease(units);
    return result;
}

bool PSScalarAdd(PSMutableScalarRef target, PSScalarRef input2, OCStringRef *error)
{
    if(error) if(*error) return false;
   	IF_NO_OBJECT_EXISTS_RETURN(target,false);
   	IF_NO_OBJECT_EXISTS_RETURN(input2,false);
    
    // Rules for addition and subtraction:
    //	- numbers must have the same dimensionality
    //	- returned PSScalar with have elementType of target argument
    //	- returned PSScalar will have unit of the target argument
    
    if(!SIDimensionalityHasSameReducedDimensionality(SIUnitGetDimensionality(target->unit),SIUnitGetDimensionality(input2->unit))) {
        if(error) {
            *error = STR("Incompatible Dimensionalities.");
        }
        return NULL;
    }
    
    switch(input2->type) {
        case kPSNumberFloat32Type: {
            float value = PSScalarFloatValueInUnit(input2, target->unit, NULL);
            switch (target->type) {
                case kPSNumberFloat32Type: {
                    target->value.floatValue = target->value.floatValue + value;
                    return true;
                }
                case kPSNumberFloat64Type: {
                    target->value.doubleValue = target->value.doubleValue + value;
                    return true;
                }
                case kPSNumberFloat32ComplexType: {
                    target->value.floatComplexValue = target->value.floatComplexValue + value;
                    return true;
                }
                case kPSNumberFloat64ComplexType: {
                    target->value.doubleComplexValue = target->value.doubleComplexValue + value;
                    return true;
                }
            }
        }
        case kPSNumberFloat64Type: {
            double value = PSScalarDoubleValueInUnit(input2, target->unit, NULL);
            switch (target->type) {
                case kPSNumberFloat32Type: {
                    target->value.floatValue = target->value.floatValue + value;
                    return true;
                }
                case kPSNumberFloat64Type: {
                    target->value.doubleValue = target->value.doubleValue + value;
                    return true;
                }
                case kPSNumberFloat32ComplexType: {
                    target->value.floatComplexValue = target->value.floatComplexValue + value;
                    return true;
                }
                case kPSNumberFloat64ComplexType: {
                    target->value.doubleComplexValue = target->value.doubleComplexValue + value;
                    return true;
                }
            }
        }
        case kPSNumberFloat32ComplexType: {
            float complex value = PSScalarFloatComplexValueInUnit(input2, target->unit, NULL);
            switch (target->type) {
                case kPSNumberFloat32Type: {
                    target->value.floatValue = target->value.floatValue + value;
                    return true;
                }
                case kPSNumberFloat64Type: {
                    target->value.doubleValue = target->value.doubleValue + value;
                    return true;
                }
                case kPSNumberFloat32ComplexType: {
                    target->value.floatComplexValue = target->value.floatComplexValue + value;
                    return true;
                }
                case kPSNumberFloat64ComplexType: {
                    target->value.doubleComplexValue = target->value.doubleComplexValue + value;
                    return true;
                }
            }
        }
        case kPSNumberFloat64ComplexType: {
            double complex value = PSScalarDoubleComplexValueInUnit(input2, target->unit, NULL);
            switch (target->type) {
                case kPSNumberFloat32Type: {
                    target->value.floatValue = target->value.floatValue + value;
                    return true;
                }
                case kPSNumberFloat64Type: {
                    target->value.doubleValue = target->value.doubleValue + value;
                    return true;
                }
                case kPSNumberFloat32ComplexType: {
                    target->value.floatComplexValue = target->value.floatComplexValue + value;
                    return true;
                }
                case kPSNumberFloat64ComplexType: {
                    target->value.doubleComplexValue = target->value.doubleComplexValue + value;
                    return true;
                }
            }
        }
    }
    return false;
}

PSScalarRef PSScalarCreateByAdding(PSScalarRef input1, PSScalarRef input2, OCStringRef *error)
{
    if(error) if(*error) return NULL;
   	IF_NO_OBJECT_EXISTS_RETURN(input1,NULL);
   	IF_NO_OBJECT_EXISTS_RETURN(input2,NULL);
    PSScalarRef result = PSScalarCreateByConvertingToNumberType(input1, PSQuantityBestElementType((PSQuantityRef) input1, (PSQuantityRef) input2));
    if(PSScalarAdd((PSMutableScalarRef) result, input2, error)) return result;
    OCRelease(result);
    return NULL;
}

bool PSScalarSubtract(PSMutableScalarRef target, PSScalarRef input2, OCStringRef *error)
{
    if(error) if(*error) return false;
   	IF_NO_OBJECT_EXISTS_RETURN(target,false);
   	IF_NO_OBJECT_EXISTS_RETURN(input2,false);
    
    // Rules for addition and subtraction:
    //	- numbers must have the same dimensionality
    //	- returned PSScalar with have elementType of target argument
    //	- returned PSScalar will have unit of the target argument
    
    if(!SIDimensionalityHasSameReducedDimensionality(SIUnitGetDimensionality(target->unit),SIUnitGetDimensionality(input2->unit))) {
        if(error) {
            *error = STR("Incompatible Dimensionalities.");
        }
        return NULL;
    }
    
    switch(input2->type) {
        case kPSNumberFloat32Type: {
            float value = PSScalarFloatValueInUnit(input2, target->unit, NULL);
            switch (target->type) {
                case kPSNumberFloat32Type: {
                    target->value.floatValue = target->value.floatValue - value;
                    return true;
                }
                case kPSNumberFloat64Type: {
                    target->value.doubleValue = target->value.doubleValue - value;
                    return true;
                }
                case kPSNumberFloat32ComplexType: {
                    target->value.floatComplexValue = target->value.floatComplexValue - value;
                    return true;
                }
                case kPSNumberFloat64ComplexType: {
                    target->value.doubleComplexValue = target->value.doubleComplexValue - value;
                    return true;
                }
            }
        }
        case kPSNumberFloat64Type: {
            double value = PSScalarDoubleValueInUnit(input2, target->unit, NULL);
            switch (target->type) {
                case kPSNumberFloat32Type: {
                    target->value.floatValue = target->value.floatValue - value;
                    return true;
                }
                case kPSNumberFloat64Type: {
                    target->value.doubleValue = target->value.doubleValue - value;
                    return true;
                }
                case kPSNumberFloat32ComplexType: {
                    target->value.floatComplexValue = target->value.floatComplexValue - value;
                    return true;
                }
                case kPSNumberFloat64ComplexType: {
                    target->value.doubleComplexValue = target->value.doubleComplexValue - value;
                    return true;
                }
            }
        }
        case kPSNumberFloat32ComplexType: {
            float complex value = PSScalarFloatComplexValueInUnit(input2, target->unit, NULL);
            switch (target->type) {
                case kPSNumberFloat32Type: {
                    target->value.floatValue = target->value.floatValue - value;
                    return true;
                }
                case kPSNumberFloat64Type: {
                    target->value.doubleValue = target->value.doubleValue - value;
                    return true;
                }
                case kPSNumberFloat32ComplexType: {
                    target->value.floatComplexValue = target->value.floatComplexValue - value;
                    return true;
                }
                case kPSNumberFloat64ComplexType: {
                    target->value.doubleComplexValue = target->value.doubleComplexValue - value;
                    return true;
                }
            }
        }
        case kPSNumberFloat64ComplexType: {
            double complex value = PSScalarDoubleComplexValueInUnit(input2, target->unit, NULL);
            switch (target->type) {
                case kPSNumberFloat32Type: {
                    target->value.floatValue = target->value.floatValue - value;
                    return true;
                }
                case kPSNumberFloat64Type: {
                    target->value.doubleValue = target->value.doubleValue - value;
                    return true;
                }
                case kPSNumberFloat32ComplexType: {
                    target->value.floatComplexValue = target->value.floatComplexValue - value;
                    return true;
                }
                case kPSNumberFloat64ComplexType: {
                    target->value.doubleComplexValue =(long double) target->value.doubleComplexValue -  (long double) value;
                    return true;
                }
            }
        }
    }
    return false;
}

PSScalarRef PSScalarCreateBySubtracting(PSScalarRef input1, PSScalarRef input2, OCStringRef *error)
{
    if(error) if(*error) return NULL;
   	IF_NO_OBJECT_EXISTS_RETURN(input1,NULL);
   	IF_NO_OBJECT_EXISTS_RETURN(input2,NULL);
    PSScalarRef result = PSScalarCreateByConvertingToNumberType(input1, PSQuantityBestElementType((PSQuantityRef) input1, (PSQuantityRef) input2));
    if(PSScalarSubtract((PSMutableScalarRef) result, input2, error)) return result;
    OCRelease(result);
    return NULL;
}

bool PSScalarMultiplyWithoutReducingUnit(PSMutableScalarRef target, PSScalarRef input2, OCStringRef *error)
{
    if(error) if(*error) return NULL;
    IF_NO_OBJECT_EXISTS_RETURN(target,false);
   	IF_NO_OBJECT_EXISTS_RETURN(input2,false);
    
    double unit_multiplier = 1;
    SIUnitRef unit = SIUnitByMultiplyingWithoutReducing(target->unit, input2->unit, &unit_multiplier, error);
    target->unit = unit;
    
    switch(input2->type) {
        case kPSNumberFloat32Type: {
            float value = input2->value.floatValue;
            switch (target->type) {
                case kPSNumberFloat32Type: {
                    target->value.floatValue = target->value.floatValue * value*unit_multiplier;
                    return true;
                }
                case kPSNumberFloat64Type: {
                    target->value.doubleValue = target->value.doubleValue * value*unit_multiplier;
                    return true;
                }
                case kPSNumberFloat32ComplexType: {
                    target->value.floatComplexValue = target->value.floatComplexValue * value*unit_multiplier;
                    return true;
                }
                case kPSNumberFloat64ComplexType: {
                    target->value.doubleComplexValue = target->value.doubleComplexValue * value*unit_multiplier;
                    return true;
                }
            }
        }
        case kPSNumberFloat64Type: {
            double value = input2->value.doubleValue;
            switch (target->type) {
                case kPSNumberFloat32Type: {
                    target->value.floatValue = target->value.floatValue * value*unit_multiplier;
                    return true;
                }
                case kPSNumberFloat64Type: {
                    target->value.doubleValue = target->value.doubleValue * value*unit_multiplier;
                    return true;
                }
                case kPSNumberFloat32ComplexType: {
                    target->value.floatComplexValue = target->value.floatComplexValue * value*unit_multiplier;
                    return true;
                }
                case kPSNumberFloat64ComplexType: {
                    target->value.doubleComplexValue = target->value.doubleComplexValue * value*unit_multiplier;
                    return true;
                }
            }
        }
        case kPSNumberFloat32ComplexType: {
            float complex value = input2->value.floatComplexValue;
            switch (target->type) {
                case kPSNumberFloat32Type: {
                    target->value.floatValue = target->value.floatValue * value*unit_multiplier;
                    return true;
                }
                case kPSNumberFloat64Type: {
                    target->value.doubleValue = target->value.doubleValue * value*unit_multiplier;
                    return true;
                }
                case kPSNumberFloat32ComplexType: {
                    target->value.floatComplexValue = target->value.floatComplexValue * value*unit_multiplier;
                    return true;
                }
                case kPSNumberFloat64ComplexType: {
                    target->value.doubleComplexValue = target->value.doubleComplexValue * value*unit_multiplier;
                    return true;
                }
            }
        }
        case kPSNumberFloat64ComplexType: {
            double complex value = input2->value.doubleComplexValue;
            switch (target->type) {
                case kPSNumberFloat32Type: {
                    target->value.floatValue = target->value.floatValue * value*unit_multiplier;
                    return true;
                }
                case kPSNumberFloat64Type: {
                    target->value.doubleValue = target->value.doubleValue * value*unit_multiplier;
                    return true;
                }
                case kPSNumberFloat32ComplexType: {
                    target->value.floatComplexValue = target->value.floatComplexValue * value*unit_multiplier;
                    return true;
                }
                case kPSNumberFloat64ComplexType: {
                    target->value.doubleComplexValue = target->value.doubleComplexValue * value*unit_multiplier;
                    return true;
                }
            }
        }
    }
    return false;
}

PSScalarRef PSScalarCreateByMultiplyingWithoutReducingUnit(PSScalarRef input1, PSScalarRef input2, OCStringRef *error)
{
    if(error) if(*error) return false;
   	IF_NO_OBJECT_EXISTS_RETURN(input1,false);
   	IF_NO_OBJECT_EXISTS_RETURN(input2,false);
    PSScalarRef result = PSScalarCreateByConvertingToNumberType(input1, PSQuantityBestElementType((PSQuantityRef) input1, (PSQuantityRef) input2));
    PSScalarMultiplyWithoutReducingUnit((PSMutableScalarRef) result, input2, error);
    return result;
}

bool PSScalarMultiply(PSMutableScalarRef target, PSScalarRef input2, OCStringRef *error)
{
    if(error) if(*error) return false;
   	IF_NO_OBJECT_EXISTS_RETURN(target,false);
   	IF_NO_OBJECT_EXISTS_RETURN(input2,false);
    
    double unit_multiplier = 1;
    SIUnitRef unit = SIUnitByMultiplying(target->unit, input2->unit, &unit_multiplier, error);
    target->unit = unit;
    
    switch(input2->type) {
        case kPSNumberFloat32Type: {
            float value = input2->value.floatValue;
            switch (target->type) {
                case kPSNumberFloat32Type: {
                    target->value.floatValue = target->value.floatValue * value*unit_multiplier;
                    return true;
                }
                case kPSNumberFloat64Type: {
                    target->value.doubleValue = target->value.doubleValue * value*unit_multiplier;
                    return true;
                }
                case kPSNumberFloat32ComplexType: {
                    target->value.floatComplexValue = target->value.floatComplexValue * value*unit_multiplier;
                    return true;
                }
                case kPSNumberFloat64ComplexType: {
                    target->value.doubleComplexValue = target->value.doubleComplexValue * value*unit_multiplier;
                    return true;
                }
            }
        }
        case kPSNumberFloat64Type: {
            float value = input2->value.doubleValue;
            switch (target->type) {
                case kPSNumberFloat32Type: {
                    target->value.floatValue = target->value.floatValue * value*unit_multiplier;
                    return true;
                }
                case kPSNumberFloat64Type: {
                    target->value.doubleValue = target->value.doubleValue * value*unit_multiplier;
                    return true;
                }
                case kPSNumberFloat32ComplexType: {
                    target->value.floatComplexValue = target->value.floatComplexValue * value*unit_multiplier;
                    return true;
                }
                case kPSNumberFloat64ComplexType: {
                    target->value.doubleComplexValue = target->value.doubleComplexValue * value*unit_multiplier;
                    return true;
                }
            }
        }
        case kPSNumberFloat32ComplexType: {
            float value = input2->value.floatComplexValue;
            switch (target->type) {
                case kPSNumberFloat32Type: {
                    target->value.floatValue = target->value.floatValue * value*unit_multiplier;
                    return true;
                }
                case kPSNumberFloat64Type: {
                    target->value.doubleValue = target->value.doubleValue * value*unit_multiplier;
                    return true;
                }
                case kPSNumberFloat32ComplexType: {
                    target->value.floatComplexValue = target->value.floatComplexValue * value*unit_multiplier;
                    return true;
                }
                case kPSNumberFloat64ComplexType: {
                    target->value.doubleComplexValue = target->value.doubleComplexValue * value*unit_multiplier;
                    return true;
                }
            }
        }
        case kPSNumberFloat64ComplexType: {
            float value = input2->value.doubleComplexValue;
            switch (target->type) {
                case kPSNumberFloat32Type: {
                    target->value.floatValue = target->value.floatValue * value*unit_multiplier;
                    return true;
                }
                case kPSNumberFloat64Type: {
                    target->value.doubleValue = target->value.doubleValue * value*unit_multiplier;
                    return true;
                }
                case kPSNumberFloat32ComplexType: {
                    target->value.floatComplexValue = target->value.floatComplexValue * value*unit_multiplier;
                    return true;
                }
                case kPSNumberFloat64ComplexType: {
                    target->value.doubleComplexValue = target->value.doubleComplexValue * value*unit_multiplier;
                    return true;
                }
            }
        }
    }
    return false;
}

PSScalarRef PSScalarCreateByMultiplying(PSScalarRef input1, PSScalarRef input2, OCStringRef *error)
{
    if(error) if(*error) return false;
   	IF_NO_OBJECT_EXISTS_RETURN(input1,NULL);
   	IF_NO_OBJECT_EXISTS_RETURN(input2,NULL);
    PSScalarRef result = PSScalarCreateByConvertingToNumberType(input1, PSQuantityBestElementType((PSQuantityRef) input1, (PSQuantityRef) input2));
    PSScalarMultiply((PSMutableScalarRef) result, input2, error);
    return result;
}

bool PSScalarDivideWithoutReducingUnit(PSMutableScalarRef target, PSScalarRef input2, OCStringRef *error)
{
    if(error) if(*error) return false;
   	IF_NO_OBJECT_EXISTS_RETURN(target,false);
   	IF_NO_OBJECT_EXISTS_RETURN(input2,false);
    
    double unit_multiplier = 1;
    SIUnitRef unit = SIUnitByDividingWithoutReducing(target->unit, input2->unit, &unit_multiplier);
    target->unit = unit;
    
    switch(input2->type) {
        case kPSNumberFloat32Type: {
            float value = input2->value.floatValue;
            if(value==0) {
                if(error==NULL) return false;
                *error = STR("Division by zero.");
                return false;
            }
            switch (target->type) {
                case kPSNumberFloat32Type: {
                    target->value.floatValue = target->value.floatValue * unit_multiplier/value;
                    return true;
                }
                case kPSNumberFloat64Type: {
                    target->value.doubleValue = target->value.doubleValue * unit_multiplier/value;
                    return true;
                }
                case kPSNumberFloat32ComplexType: {
                    target->value.floatComplexValue = target->value.floatComplexValue * unit_multiplier/value;
                    return true;
                }
                case kPSNumberFloat64ComplexType: {
                    target->value.doubleComplexValue = target->value.doubleComplexValue * unit_multiplier/value;
                    return true;
                }
            }
        }
        case kPSNumberFloat64Type: {
            double value = input2->value.doubleValue;
            if(value==0) {
                if(error==NULL) return false;
                *error = STR("Division by zero.");
                return false;
            }
            switch (target->type) {
                case kPSNumberFloat32Type: {
                    target->value.floatValue = target->value.floatValue * unit_multiplier/value;
                    return true;
                }
                case kPSNumberFloat64Type: {
                    target->value.doubleValue = target->value.doubleValue * unit_multiplier/value;
                    return true;
                }
                case kPSNumberFloat32ComplexType: {
                    target->value.floatComplexValue = target->value.floatComplexValue * unit_multiplier/value;
                    return true;
                }
                case kPSNumberFloat64ComplexType: {
                    target->value.doubleComplexValue = target->value.doubleComplexValue * unit_multiplier/value;
                    return true;
                }
            }
        }
        case kPSNumberFloat32ComplexType: {
            float complex value = input2->value.floatComplexValue;
            if(value==0) {
                if(error==NULL) return false;
                *error = STR("Division by zero.");
                return false;
            }
            switch (target->type) {
                case kPSNumberFloat32Type: {
                    target->value.floatValue = target->value.floatValue * unit_multiplier/value;
                    return true;
                }
                case kPSNumberFloat64Type: {
                    target->value.doubleValue = target->value.doubleValue * unit_multiplier/value;
                    return true;
                }
                case kPSNumberFloat32ComplexType: {
                    target->value.floatComplexValue = target->value.floatComplexValue * unit_multiplier/value;
                    return true;
                }
                case kPSNumberFloat64ComplexType: {
                    target->value.doubleComplexValue = target->value.doubleComplexValue * unit_multiplier/value;
                    return true;
                }
            }
        }
        case kPSNumberFloat64ComplexType: {
            double complex value = input2->value.doubleComplexValue;
            if(value==0) {
                if(error==NULL) return false;
                *error = STR("Division by zero.");
                return false;
            }
            switch (target->type) {
                case kPSNumberFloat32Type: {
                    target->value.floatValue = target->value.floatValue* unit_multiplier/value;
                    return true;
                }
                case kPSNumberFloat64Type: {
                    target->value.doubleValue = target->value.doubleValue* unit_multiplier/value;
                    return true;
                }
                case kPSNumberFloat32ComplexType: {
                    target->value.floatComplexValue = target->value.floatComplexValue * unit_multiplier/value;
                    return true;
                }
                case kPSNumberFloat64ComplexType: {
                    target->value.doubleComplexValue = target->value.doubleComplexValue * unit_multiplier/value;
                    return true;
                }
            }
        }
    }
    return false;
}

bool PSScalarDivide(PSMutableScalarRef target, PSScalarRef input2, OCStringRef *error)
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
        case kPSNumberFloat32Type: {
            float value = input2->value.floatValue;
            if(value==0) {
                if(error==NULL) return false;
                *error = STR("Division by zero.");
                return false;
            }
            switch (target->type) {
                case kPSNumberFloat32Type: {
                    target->value.floatValue = target->value.floatValue * unit_multiplier/value;
                    return true;
                }
                case kPSNumberFloat64Type: {
                    target->value.doubleValue = target->value.doubleValue * unit_multiplier/value;
                    return true;
                }
                case kPSNumberFloat32ComplexType: {
                    target->value.floatComplexValue = target->value.floatComplexValue * unit_multiplier/value;
                    return true;
                }
                case kPSNumberFloat64ComplexType: {
                    target->value.doubleComplexValue = target->value.doubleComplexValue * unit_multiplier/value;
                    return true;
                }
            }
        }
        case kPSNumberFloat64Type: {
            double value = input2->value.doubleValue;
            if(value==0) {
                if(error==NULL) return false;
                *error = STR("Division by zero.");
                return false;
            }
            switch (target->type) {
                case kPSNumberFloat32Type: {
                    target->value.floatValue = target->value.floatValue * unit_multiplier/value;
                    return true;
                }
                case kPSNumberFloat64Type: {
                    target->value.doubleValue = target->value.doubleValue * unit_multiplier/value;
                    return true;
                }
                case kPSNumberFloat32ComplexType: {
                    target->value.floatComplexValue = target->value.floatComplexValue * unit_multiplier/value;
                    return true;
                }
                case kPSNumberFloat64ComplexType: {
                    target->value.doubleComplexValue = target->value.doubleComplexValue * unit_multiplier/value;
                    return true;
                }
            }
        }
        case kPSNumberFloat32ComplexType: {
            float complex value = input2->value.floatComplexValue;
            if(value==0) {
                if(error==NULL) return false;
                *error = STR("Division by zero.");
                return false;
            }
            switch (target->type) {
                case kPSNumberFloat32Type: {
                    target->value.floatValue = target->value.floatValue * unit_multiplier/value;
                    return true;
                }
                case kPSNumberFloat64Type: {
                    target->value.doubleValue = target->value.doubleValue * unit_multiplier/value;
                    return true;
                }
                case kPSNumberFloat32ComplexType: {
                    target->value.floatComplexValue = target->value.floatComplexValue * unit_multiplier/value;
                    return true;
                }
                case kPSNumberFloat64ComplexType: {
                    target->value.doubleComplexValue = target->value.doubleComplexValue * unit_multiplier/value;
                    return true;
                }
            }
        }
        case kPSNumberFloat64ComplexType: {
            double complex value = input2->value.doubleComplexValue;
            if(value==0) {
                if(error==NULL) return false;
                *error = STR("Division by zero.");
                return false;
            }
            switch (target->type) {
                case kPSNumberFloat32Type: {
                    target->value.floatValue = target->value.floatValue * unit_multiplier/value;
                    return true;
                }
                case kPSNumberFloat64Type: {
                    target->value.doubleValue = target->value.doubleValue * unit_multiplier/value;
                    return true;
                }
                case kPSNumberFloat32ComplexType: {
                    target->value.floatComplexValue = target->value.floatComplexValue * unit_multiplier/value;
                    return true;
                }
                case kPSNumberFloat64ComplexType: {
                    target->value.doubleComplexValue = target->value.doubleComplexValue * unit_multiplier/value;
                    return true;
                }
            }
        }
    }
    return false;
}

PSScalarRef PSScalarCreateByDividingWithoutReducingUnit(PSScalarRef input1, PSScalarRef input2, OCStringRef *error)
{
    if(error) if(*error) return NULL;
   	IF_NO_OBJECT_EXISTS_RETURN(input1,NULL);
   	IF_NO_OBJECT_EXISTS_RETURN(input2,NULL);
    PSScalarRef result = PSScalarCreateByConvertingToNumberType(input1, PSQuantityBestElementType((PSQuantityRef) input1, (PSQuantityRef) input2));
    if(PSScalarDivideWithoutReducingUnit((PSMutableScalarRef) result, input2, error)) return result;
    OCRelease(result);
    return NULL;
}

PSScalarRef PSScalarCreateByDividing(PSScalarRef input1, PSScalarRef input2, OCStringRef *error)
{
    if(error) if(*error) return NULL;
   	IF_NO_OBJECT_EXISTS_RETURN(input1,NULL);
   	IF_NO_OBJECT_EXISTS_RETURN(input2,NULL);
    PSScalarRef result = PSScalarCreateByConvertingToNumberType(input1, PSQuantityBestElementType((PSQuantityRef) input1, (PSQuantityRef) input2));
    if(PSScalarDivide((PSMutableScalarRef) result, input2, error)) return result;
    OCRelease(result);
    return NULL;
}

bool PSScalarRaiseToAPowerWithoutReducingUnit(PSMutableScalarRef theScalar, double power, OCStringRef *error)
{
    if(error) if(*error) return false;
    
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,false);
    double unit_multiplier = 1;
    SIUnitRef unit = SIUnitByRaisingToAPowerWithoutReducing(theScalar->unit, power, &unit_multiplier, error);
    if(error) {
        if(*error) return false;
    }
   	IF_NO_OBJECT_EXISTS_RETURN(unit,false);
    
    theScalar->unit = unit;
    switch(theScalar->type) {
        case kPSNumberFloat32Type:
            theScalar->value.floatValue = pow(theScalar->value.floatValue,power)*unit_multiplier;
            return true;
        case kPSNumberFloat64Type:
            theScalar->value.doubleValue = pow(theScalar->value.doubleValue,power)*unit_multiplier;
            return true;
        case kPSNumberFloat32ComplexType:
            theScalar->value.floatComplexValue = cpow(theScalar->value.floatComplexValue,power)*unit_multiplier;
            return true;
        case kPSNumberFloat64ComplexType:
            theScalar->value.doubleComplexValue = cpow(theScalar->value.doubleComplexValue,power)*unit_multiplier;
            return true;
    }
    return false;
}

PSScalarRef PSScalarCreateByRaisingToAPowerWithoutReducingUnit(PSScalarRef theScalar, double power, OCStringRef *error)
{
    if(error) if(*error) return NULL;
    
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,NULL);
    PSScalarRef result = PSScalarCreateCopy(theScalar);
    if(PSScalarRaiseToAPowerWithoutReducingUnit((PSMutableScalarRef) result, power, error)) return result;
    if(result) OCRelease(result);
    return NULL;
}

bool PSScalarRaiseToAPower(PSMutableScalarRef theScalar, double power, OCStringRef *error)
{
    if(error) if(*error) return false;
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,false);
    
    // Rules for multiplication and division:
    //	- returned PSScalar with have whichever elementType is greatest of two method arguments
    //	- returned PSScalar unit will be in coherent SI units
    
    double unit_multiplier = 1;
    SIUnitRef unit = SIUnitByRaisingToAPower(theScalar->unit, power, &unit_multiplier, error);
    if(error) {
        if(*error) return false;
    }
    
    theScalar->unit = unit;
    switch(theScalar->type) {
        case kPSNumberFloat32Type:
            theScalar->value.floatValue = pow(theScalar->value.floatValue,power)*unit_multiplier;
            return true;
        case kPSNumberFloat64Type:
            theScalar->value.doubleValue = pow(theScalar->value.doubleValue,power)*unit_multiplier;
            return true;
        case kPSNumberFloat32ComplexType:
            theScalar->value.floatComplexValue = cpow(theScalar->value.floatComplexValue,power)*unit_multiplier;
            return true;
        case kPSNumberFloat64ComplexType:
            theScalar->value.doubleComplexValue = cpow(theScalar->value.doubleComplexValue,power)*unit_multiplier;
            return true;
    }
    return false;
}

PSScalarRef PSScalarCreateByRaisingToAPower(PSScalarRef theScalar, double power, OCStringRef *error)
{
    if(error) if(*error) return NULL;
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,NULL);
    PSScalarRef result = PSScalarCreateCopy(theScalar);
    if(PSScalarRaiseToAPower((PSMutableScalarRef) result, power, error)) return result;
    if(result) OCRelease(result);
    return NULL;
}

bool PSScalarTakeAbsoluteValue(PSMutableScalarRef theScalar, OCStringRef *error)
{
    if(error) if(*error) return false;
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,false);
    
    switch(theScalar->type) {
        case kPSNumberFloat32Type:
            theScalar->value.floatValue = fabsf(theScalar->value.floatValue);
            return true;
        case kPSNumberFloat64Type:
            theScalar->value.doubleValue = fabs(theScalar->value.doubleValue);
            return true;
        case kPSNumberFloat32ComplexType:
            theScalar->value.floatComplexValue = cabsf(theScalar->value.floatComplexValue);
            return true;
        case kPSNumberFloat64ComplexType:
            theScalar->value.doubleComplexValue = cabs(theScalar->value.doubleComplexValue);
            return true;
    }
    return false;
}

PSScalarRef PSScalarCreateByTakingAbsoluteValue(PSScalarRef theScalar, OCStringRef *error)
{
    if(error) if(*error) return NULL;
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,NULL);
    PSScalarRef result = PSScalarCreateCopy(theScalar);
    if(PSScalarTakeAbsoluteValue((PSMutableScalarRef) result, error)) return result;
    if(result) OCRelease(result);
    return NULL;
}

PSScalarRef PSScalarCreateByGammaFunctionWithoutReducingUnit(PSScalarRef theScalar, OCStringRef *error)
{
    if(error) if(*error) return NULL;
    if(PSScalarIsComplex(theScalar)) {
        if(error) {
            *error = STR("Gamma function of complex number not implemented.");
        }
        return NULL;
    }
    
    if(SIUnitIsDimensionless(theScalar->unit)) {
        PSMutableScalarRef temp = PSScalarCreateMutableCopy(theScalar);
        PSScalarReduceUnit(temp);
        double argument = PSScalarDoubleValue(temp)+1;
        double result = tgamma(argument);
        OCRelease(temp);
        return PSScalarCreate(SIUnitDimensionlessAndUnderived(), kPSNumberFloat64Type, &result);
    }
    else if(PSScalarIsRealNonNegativeInteger(theScalar)) {
        int64_t integerValue = (int64_t) PSScalarDoubleValue(theScalar);
        double unit_multiplier = 1;
        SIUnitRef newUnit = SIUnitByRaisingToAPower(theScalar->unit, integerValue, &unit_multiplier, error);
        double argument = PSScalarDoubleValue(theScalar)+1;
        double result = tgamma(argument);
        return PSScalarCreate(newUnit, kPSNumberFloat64Type, &result);
    }
    return NULL;
}

bool PSScalarMultiplyByDimensionlessRealConstant(PSMutableScalarRef theScalar, double constant)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,false);
    switch(theScalar->type) {
        case kPSNumberFloat32Type:
            theScalar->value.floatValue *= constant;
            return true;
        case kPSNumberFloat64Type:
            theScalar->value.doubleValue *= constant;
            return true;
        case kPSNumberFloat32ComplexType:
            theScalar->value.floatComplexValue *= constant;
            return true;
        case kPSNumberFloat64ComplexType:
            theScalar->value.doubleComplexValue *= constant;
            return true;
    }
    return false;
}

PSScalarRef PSScalarCreateByMultiplyingByDimensionlessRealConstant(PSScalarRef theScalar, double constant)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,NULL);
    numberType elementType = kPSNumberFloat64Type;
    if(elementType<theScalar->type) elementType = theScalar->type;
    PSScalarRef result = PSScalarCreateByConvertingToNumberType(theScalar, elementType);
    if(PSScalarMultiplyByDimensionlessRealConstant((PSMutableScalarRef) result, constant)) return result;
    if(result) OCRelease(result);
    return NULL;
}

bool PSScalarMultiplyByDimensionlessComplexConstant(PSMutableScalarRef theScalar, double complex constant)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,false);
    // PSScalar elementType remains the same after multiplication, so information is loss
    switch(theScalar->type) {
        case kPSNumberFloat32Type:
            theScalar->value.floatValue = theScalar->value.floatValue * constant;
            return true;
        case kPSNumberFloat64Type:
            theScalar->value.doubleValue = theScalar->value.doubleValue * constant;
            return true;
        case kPSNumberFloat32ComplexType:
            theScalar->value.floatComplexValue = theScalar->value.floatComplexValue * constant;
            return true;
        case kPSNumberFloat64ComplexType:
            theScalar->value.doubleComplexValue = theScalar->value.doubleComplexValue * constant;
            return true;
    }
    return false;
}

PSScalarRef PSScalarCreateByMultiplyingByDimensionlessComplexConstant(PSScalarRef theScalar, double complex constant)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,NULL);
    PSScalarRef result = PSScalarCreateByConvertingToNumberType(theScalar, kPSNumberFloat64ComplexType);
    if(PSScalarMultiplyByDimensionlessComplexConstant((PSMutableScalarRef) result, constant)) return result;
    if(result) OCRelease(result);
    return NULL;
}

bool PSScalarConjugate(PSMutableScalarRef theScalar)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,false);
    switch(theScalar->type) {
            //		case kPSNumberSInt32Type:
            //		case kPSNumberSInt64Type:
        case kPSNumberFloat32Type:
        case kPSNumberFloat64Type:
            return true;
        case kPSNumberFloat32ComplexType: {
            theScalar->value.floatComplexValue = creal(theScalar->value.floatComplexValue) - I*cimag(theScalar->value.floatComplexValue);
            return true;
        }
        case kPSNumberFloat64ComplexType:
            theScalar->value.doubleComplexValue = creal(theScalar->value.doubleComplexValue) - I*cimag(theScalar->value.doubleComplexValue);
            return true;
    }
    return false;
}

PSScalarRef PSScalarCreateByConjugation(PSScalarRef theScalar)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,NULL);
    PSScalarRef result = PSScalarCreateCopy(theScalar);
    if(PSScalarConjugate((PSMutableScalarRef) result)) return result;
    if(result) OCRelease(result);
    return NULL;
}

bool PSScalarTakeNthRoot(PSMutableScalarRef theScalar, uint8_t root, OCStringRef *error)
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
        case kPSNumberFloat32Type:
            if(root==2) theScalar->value.floatValue = sqrtf(theScalar->value.floatValue)*multiplier;
            else theScalar->value.floatValue = pow(theScalar->value.floatValue,1./root)*multiplier;
            break;
        case kPSNumberFloat64Type:
            if(root==2) theScalar->value.doubleValue = sqrt(theScalar->value.doubleValue)*multiplier;
            else theScalar->value.doubleValue = pow(theScalar->value.doubleValue,1./root)*multiplier;
            break;
        case kPSNumberFloat32ComplexType:
            if(root==2) theScalar->value.floatComplexValue = csqrtf(theScalar->value.floatComplexValue)*multiplier;
            else theScalar->value.floatComplexValue = cpow(theScalar->value.floatComplexValue,1./root)*multiplier;
            break;
        case kPSNumberFloat64ComplexType:
            if(root==2) theScalar->value.doubleComplexValue = csqrt(theScalar->value.doubleComplexValue)*multiplier;
            else theScalar->value.doubleComplexValue = cpow(theScalar->value.doubleComplexValue,1./root)*multiplier;
            break;
    }
    return true;
}

PSScalarRef PSScalarCreateByTakingNthRoot(PSScalarRef theScalar, uint8_t root, OCStringRef *error)
{
    if(error) if(*error) return NULL;
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,NULL);
    PSScalarRef result = PSScalarCreateCopy(theScalar);
    if(PSScalarTakeNthRoot((PSMutableScalarRef) result, root, error)) return result;
    if(result) OCRelease(result);
    return NULL;
}


bool PSScalarTakeLog10(PSMutableScalarRef theScalar, OCStringRef *error)
{
    IF_NO_OBJECT_EXISTS_RETURN(theScalar,false);
    if(!SIDimensionalityIsDimensionless(PSQuantityGetUnitDimensionality((PSQuantityRef) theScalar))) {
        if(error) {
            *error = STR("Log10 requires dimensionless unit.");
        }
    }
    switch (theScalar->type) {
        case kPSNumberFloat32Type: {
            theScalar->value.floatValue = log10f(theScalar->value.floatValue);
            break;
        }
        case kPSNumberFloat64Type:{
            theScalar->value.doubleValue = log10(theScalar->value.doubleValue);
            break;
        }
        case kPSNumberFloat32ComplexType: {
            theScalar->value.floatComplexValue = clogf(theScalar->value.floatComplexValue)/logf(10);
            break;
        }
        case kPSNumberFloat64ComplexType: {
            theScalar->value.doubleComplexValue = clog(theScalar->value.doubleComplexValue)/log(10);
            break;
        }
    }
    return true;
}


bool PSScalarZeroPart(PSMutableScalarRef theScalar, complexPart part)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,false);
    switch (theScalar->type) {
        case kPSNumberFloat32Type:
            if(part == kPSRealPart || part == kPSMagnitudePart) {
                theScalar->value.floatValue = 0;
                return true;
            }
            if(part == kPSImaginaryPart || part == kPSArgumentPart) return true;
            break;
        case kPSNumberFloat64Type:
            if(part == kPSRealPart || part == kPSMagnitudePart) {
                theScalar->value.doubleValue = 0;
                return true;
            }
            if(part == kPSImaginaryPart || part == kPSArgumentPart) return true;
            break;
        case kPSNumberFloat32ComplexType: {
            if(part == kPSMagnitudePart) {
                theScalar->value.floatComplexValue = 0;
                return true;
            }
            if(part == kPSRealPart) {
                theScalar->value.floatComplexValue = cimag(theScalar->value.floatComplexValue);
                return true;
            }
            if(part == kPSImaginaryPart) {
                theScalar->value.floatComplexValue = creal(theScalar->value.floatComplexValue);
                return true;
            }
            if(part == kPSArgumentPart) {
                theScalar->value.floatComplexValue = cabs(theScalar->value.floatComplexValue);
                return true;
            }
            break;
        }
        case kPSNumberFloat64ComplexType: {
            if(part == kPSMagnitudePart) {
                theScalar->value.doubleComplexValue = 0;
                return true;
            }
            if(part == kPSRealPart) {
                theScalar->value.doubleComplexValue = cimag(theScalar->value.doubleComplexValue);
                return true;
            }
            if(part == kPSImaginaryPart) {
                theScalar->value.doubleComplexValue = creal(theScalar->value.doubleComplexValue);
                return true;
            }
            if(part == kPSArgumentPart) {
                theScalar->value.doubleComplexValue = cabs(theScalar->value.doubleComplexValue);
                return true;
            }
            break;
        }
    }
    return false;
}

PSScalarRef PSScalarCreateByZeroingPart(PSScalarRef theScalar, complexPart part)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,NULL);
    PSScalarRef result = PSScalarCreateCopy(theScalar);
    if(PSScalarZeroPart((PSMutableScalarRef) result, part)) return result;
    OCRelease(result);
    return NULL;
}

PSScalarRef PSScalarCreateWithStringContainingSingleUnitFromLibrary(OCStringRef theString)
{
    OCMutableStringRef  mutString = OCStringCreateMutableCopy (theString);
    OCStringFindAndReplace2 (mutString,STR("×"), STR("*"));
    OCStringFindAndReplace2 (mutString,STR("÷"), STR("/"));
    OCStringFindAndReplace2 (mutString,STR("−"), STR("-"));
    OCStringFindAndReplace2 (mutString,STR("\n"), STR(""));
    OCStringFindAndReplace2 (mutString,STR("+"), STR("+"));
    OCStringFindAndReplace2 (mutString,STR("μ"), STR("µ"));
    OCStringFindAndReplace2 (mutString,STR("γ"), STR("𝛾"));
    OCStringFindAndReplace2 (mutString,STR("º"), STR("°"));
    OCStringFindAndReplace2 (mutString,STR("ɣ"), STR("𝛾"));
    OCStringFindAndReplace2 (mutString,STR(" "), STR(""));
    OCStringFindAndReplace2 (mutString,STR(")("), STR(")*("));
    
    OCRange plusRange = OCStringFind(mutString, STR("+"), 0);
    if(plusRange.location != kOCNotFound && plusRange.location!=0) return NULL;
    OCRange minusRange = OCStringFind(mutString, STR("-"), 0);
    if(minusRange.location != kOCNotFound && minusRange.location!=0) return NULL;
    
    OCStringTrimMatchingParentheses(mutString);
    
    uint64_t length = OCStringGetLength(mutString);
    
    // See how many unit symbols are in the string
    // For ones that are found, save their values, range, and determine the largest length symbol present
    OCMutableArrayRef unitsFound = OCArrayCreateMutable(0,&kOCTypeArrayCallBacks);
    OCMutableArrayRef ranges = OCArrayCreateMutable(0,&kOCTypeArrayCallBacks);
    uint64_t maximumLength = 0;
    OCRange fullRange = OCRangeMake(0, length);
    
    OCDictionaryRef unitsLibrary = SIUnitGetLibrary();
    if(unitsLibrary == NULL) return NULL;
    OCArrayRef keys = OCDictionaryCreateArrayWithAllKeys(unitsLibrary);
    for(uint64_t index=0;index<OCArrayGetCount(keys); index++) {
        OCStringRef symbol = OCArrayGetValueAtIndex(keys, index);
        OCMutableStringRef mutSymbol = OCStringCreateMutableCopy(symbol);
        OCStringFindAndReplace2 (mutSymbol,STR(" "), STR(""));
        
        OCArrayRef findResults = OCStringCreateArrayWithFindResults (mutString,
                                                                     mutSymbol,
                                                                     fullRange,
                                                                     0);
        OCRelease(mutSymbol);
        if(findResults) {
            for(uint64_t foundIndex=0; foundIndex<OCArrayGetCount(findResults);foundIndex++) {
                OCRange *foundRange = (OCRange *) OCArrayGetValueAtIndex(findResults, foundIndex);
                OCRange *rangeFound = (OCRange *) malloc(sizeof(OCRange));
                rangeFound->location = foundRange->location;
                rangeFound->length = foundRange->length;
                OCArrayAppendValue(ranges, rangeFound);
                OCArrayAppendValue(unitsFound, symbol);
                uint64_t length = OCStringGetLength(symbol);
                if(length>maximumLength) maximumLength = length;
            }
            OCRelease(findResults);
        }
    }
    
    OCRelease(keys);
    
    uint64_t unitsFoundCount = OCArrayGetCount(unitsFound);
    if(unitsFoundCount==0) {
        OCRelease(unitsFound);
        for(uint64_t i=0;i<OCArrayGetCount(ranges); i++) {
            OCRange *rangeFound = (OCRange *) OCArrayGetValueAtIndex(ranges, i);
            if(rangeFound) free(rangeFound);
        }
        OCRelease(ranges);
        OCRelease(mutString);
        return NULL;
    }
    
    // some unit symbols found.  Which one and how many are the largest
    uint64_t largestSymbols = 0;
    uint64_t largestSymbolIndex = 0;
    for(uint64_t index=0; index<unitsFoundCount; index++) {
        if(OCStringGetLength(OCArrayGetValueAtIndex(unitsFound, index)) == maximumLength) {
            largestSymbols++;
            largestSymbolIndex = index;
        }
    }
    // More than one unit symbol of the same length -> abort
    if(largestSymbols>1) {
        OCRelease(unitsFound);
        for(uint64_t i=0;i<OCArrayGetCount(ranges); i++) {
            OCRange *rangeFound = (OCRange *) OCArrayGetValueAtIndex(ranges, i);
            if(rangeFound) free(rangeFound);
        }
        OCRelease(ranges);
        OCRelease(mutString);
        return NULL;
    }
    
    OCRange *largestSymbolRange  =  (OCRange *) OCArrayGetValueAtIndex(ranges, largestSymbolIndex);
    OCStringRef largestSymbol = OCArrayGetValueAtIndex(unitsFound, largestSymbolIndex);
    
    // Still need to make sure unitsFoundCount is correct;
    // Units inside the largest unit can be safely ignored and
    // for this method to work other units can't be outside the largest unit.
    // If units are outside, then game over and we return null;
    // If units are inside largest symbol, then remove them from unitsFound array
    for(int64_t index=unitsFoundCount-1; index>=0; index--) {
        if(index != largestSymbolIndex) {
            //All the smaller symbols must be present in the larger length symbol
            OCStringRef smallerSymbol = OCArrayGetValueAtIndex(unitsFound, index);
            OCRange range = OCStringFind(largestSymbol, smallerSymbol, 0);
            if(range.location == kOCNotFound) {
                OCRelease(unitsFound);
                for(uint64_t i=0;i<OCArrayGetCount(ranges); i++) {
                    OCRange *rangeFound = (OCRange *) OCArrayGetValueAtIndex(ranges, i);
                    if(rangeFound) free(rangeFound);
                }
                OCRelease(ranges);
                OCRelease(mutString);
                return NULL;
            }
            else OCArrayRemoveValueAtIndex(unitsFound, index);
            
            OCRange *rangeFound = (OCRange *) OCArrayGetValueAtIndex(ranges, index);
            if(rangeFound->location <largestSymbolRange->location) {
                OCRelease(unitsFound);
                for(uint64_t i=0;i<OCArrayGetCount(ranges); i++) {
                    OCRange *rangeFound = (OCRange *) OCArrayGetValueAtIndex(ranges, i);
                    if(rangeFound) free(rangeFound);
                }
                OCRelease(ranges);
                OCRelease(mutString);
                return NULL;
            }
            if(rangeFound->location >largestSymbolRange->location+largestSymbolRange->length-1) {
                OCRelease(unitsFound);
                for(uint64_t i=0;i<OCArrayGetCount(ranges); i++) {
                    OCRange *rangeFound = (OCRange *) OCArrayGetValueAtIndex(ranges, i);
                    if(rangeFound) free(rangeFound);
                }
                OCRelease(ranges);
                OCRelease(mutString);
                return NULL;
            }
        }
    }
    
    
    unitsFoundCount = OCArrayGetCount(unitsFound);
    // if none, then return null
    if(unitsFoundCount == 0) {
        OCRelease(unitsFound);
        for(uint64_t i=0;i<OCArrayGetCount(ranges); i++) {
            OCRange *rangeFound = (OCRange *) OCArrayGetValueAtIndex(ranges, i);
            if(rangeFound) free(rangeFound);
        }
        OCRelease(ranges);
        OCRelease(mutString);
        return NULL;
    }
    
    // if one, then try to construct scalar
    if(unitsFoundCount == 1) {
        // Unit should not be followed by anything except spaces
        OCMutableStringRef mutLargestSymbol = OCStringCreateMutableCopy(largestSymbol);
        OCStringFindAndReplace2 (mutLargestSymbol,STR(" "), STR(""));
        OCRange range = OCStringFind(mutString, mutLargestSymbol, 0);
        
        for(uint64_t index = range.location+range.length; index<length; index++) {
            char character = OCStringGetCharacterAtIndex(mutString, index);
            if(character != ' ') {
                OCRelease(unitsFound);
                for(uint64_t i=0;i<OCArrayGetCount(ranges); i++) {
                    OCRange *rangeFound = (OCRange *) OCArrayGetValueAtIndex(ranges, i);
                    if(rangeFound) free(rangeFound);
                }
                OCRelease(ranges);
                OCRelease(mutString);
                OCRelease(mutLargestSymbol);
                return NULL;
            }
        }
        
        // Create string with unit removed
        OCMutableStringRef numericPart = OCStringCreateMutableCopy(mutString);
        OCStringFindAndReplace2(numericPart, mutLargestSymbol, STR(""));
        double complex numericValue = 1.0;
        if(OCStringGetLength(numericPart)!=0) numericValue = OCStringGetDoubleComplexValue(numericPart);
        if(isnan(numericValue)) {
            // Abort!
            OCRelease(unitsFound);
            for(uint64_t i=0;i<OCArrayGetCount(ranges); i++) {
                OCRange *rangeFound = (OCRange *) OCArrayGetValueAtIndex(ranges, i);
                if(rangeFound) free(rangeFound);
            }
            OCRelease(ranges);
            OCRelease(mutString);
            OCRelease(mutLargestSymbol);
            OCRelease(numericPart);
            return NULL;
        }
        SIUnitRef unit = SIUnitForUnderivedSymbol(largestSymbol);
        PSScalarRef scalar = PSScalarCreateWithDoubleComplex(numericValue, unit);
        if(numericPart) OCRelease(numericPart);
        
        OCRelease(mutLargestSymbol);
        OCRelease(unitsFound);
        OCRange *rangeFound = (OCRange *) OCArrayGetValueAtIndex(ranges, 0);
        if(rangeFound) free(rangeFound);
        OCRelease(ranges);
        
        if(PSScalarIsReal(scalar)) {
            PSScalarRef realResult = PSScalarCreateByTakingComplexPart(scalar,kPSRealPart);
            OCRelease(mutString);
            OCRelease(scalar);
            return realResult;
        }
        OCRelease(mutString);
        return scalar;
    }
    
    
    // If we're here, then we have a single derived unit symbol at the end of the string and should be able to create a scalar
    // Unit should not be followed by anything
    if(largestSymbolRange->location+largestSymbolRange->length != length) {
        OCRelease(unitsFound);
        for(uint64_t i=0;i<OCArrayGetCount(ranges); i++) {
            OCRange *rangeFound = (OCRange *) OCArrayGetValueAtIndex(ranges, i);
            if(rangeFound) free(rangeFound);
        }
        OCRelease(unitsFound);
        for(uint64_t i=0;i<OCArrayGetCount(ranges); i++) {
            OCRange *rangeFound = (OCRange *) OCArrayGetValueAtIndex(ranges, i);
            if(rangeFound) free(rangeFound);
        }
        OCRelease(ranges);
        OCRelease(mutString);
        return NULL;
    }
    
    
    // Create string with unit removed
    
    OCMutableStringRef mutLargestSymbol = OCStringCreateMutableCopy(largestSymbol);
    OCStringFindAndReplace2 (mutLargestSymbol,STR(" "), STR(""));
    
    OCMutableStringRef numericPart = OCStringCreateMutableCopy( mutString);
    OCStringFindAndReplace2(numericPart, mutLargestSymbol, STR(""));
    double complex numericValue = OCStringGetDoubleComplexValue(numericPart);
    SIUnitRef unit = SIUnitForUnderivedSymbol(largestSymbol);
    PSScalarRef scalar = PSScalarCreateWithDoubleComplex(numericValue, unit);
    if(numericPart) OCRelease(numericPart);
    OCRelease(mutLargestSymbol);
    
    OCRelease(unitsFound);
    for(uint64_t i=0;i<OCArrayGetCount(ranges); i++) {
        OCRange *rangeFound = (OCRange *) OCArrayGetValueAtIndex(ranges, i);
        if(rangeFound) free(rangeFound);
    }
    OCRelease(ranges);
    
    if(PSScalarIsReal(scalar)) {
        PSScalarRef realResult = PSScalarCreateByTakingComplexPart(scalar,kPSRealPart);
        OCRelease(scalar);
        OCRelease(mutString);
        return realResult;
    }
    OCRelease(mutString);
    return scalar;
}

static OCStringRef PSScalarCreateStringValueSplitByUnits(PSScalarRef theScalar, OCArrayRef units, bool doubleCheck, OCStringRef *error)
{
    if(error) if(*error) return NULL;
    if(!PSScalarIsReal(theScalar)) return NULL;
    bool isPositive = false;
    if(PSScalarDoubleValue(theScalar) >0.0) isPositive = true;
    
    OCMutableStringRef stringValue = OCStringCreateMutable(0);
    PSMutableScalarRef scalar = PSScalarCreateMutableCopy(theScalar);
    uint64_t count = OCArrayGetCount(units);
    uint64_t finalCount = 0;
    for(uint64_t index=0;index<count; index++) {
        SIUnitRef unit = OCArrayGetValueAtIndex(units, index);
        OCStringRef symbol = SIUnitCopySymbol(unit);
        PSScalarConvertToUnit(scalar, unit, error);
        double value = PSScalarDoubleValue(scalar);
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
            OCStringRef valueString = PSDoubleComplexCreateStringValue(value,NULL);
            finalCount++;
            if(OCStringGetLength(stringValue)>0) {
                if(isPositive) OCStringAppend(stringValue, STR(" + "));
                else OCStringAppend(stringValue, STR(" "));
            }
            OCStringAppend(stringValue, valueString);
            OCStringAppend(stringValue, STR(" "));
            OCStringAppend(stringValue, symbol);
            OCRelease(valueString);
            
            PSScalarRef scalarInUnit = PSScalarCreate(unit, kPSNumberFloat64Type, &value);
            PSScalarSubtract(scalar, scalarInUnit, error);
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
        PSScalarRef check = PSScalarCreateWithOCString(stringValue, error);
        if(check) {
            if(PSScalarCompare(theScalar, check)==kOCCompareEqualTo) {
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


OCArrayRef PSScalarCreateArrayOfConversionQuantitiesScalarsAndStringValues(PSScalarRef theScalar, OCStringRef quantity, OCStringRef *error)
{
    if(error) if(*error) return NULL;
    OCMutableArrayRef result = OCArrayCreateMutable(0,&kOCTypeArrayCallBacks);
    SIUnitRef fahrenheit = SIUnitForUnderivedSymbol(STR("°F"));
    SIUnitRef celsius = SIUnitForUnderivedSymbol(STR("°C"));
    
    if(SIDimensionalityHasSameReducedDimensionality(SIDimensionalityForQuantity(kPSQuantityTemperature,NULL), PSQuantityGetUnitDimensionality((PSQuantityRef)  theScalar))) {
        SIUnitRef theUnit = PSQuantityGetUnit((PSQuantityRef) theScalar);
        if(theUnit == fahrenheit) {
            OCArrayAppendValue(result, theScalar);
            
            double complex value = (PSScalarDoubleComplexValue(theScalar) -32)*5./9.;
            PSScalarRef scalar = PSScalarCreate(celsius, kPSNumberFloat64ComplexType, &value);
            OCArrayAppendValue(result, scalar);
            OCRelease(scalar);
            return result;
        }
        if(theUnit == celsius) {
            OCArrayAppendValue(result, theScalar);
            
            double complex value = PSScalarDoubleComplexValue(theScalar)*9./5. + 32;
            PSScalarRef scalar = PSScalarCreate(fahrenheit, kPSNumberFloat64ComplexType, &value);
            OCArrayAppendValue(result, scalar);
            OCRelease(scalar);
            return result;
        }
    }
    
    OCArrayRef units = NULL;
    OCArrayRef quantities = NULL;
    
    if(theScalar) {
        if(NULL == quantity) {
            quantities = SIDimensionalityCreateArrayOfQuantitiesWithSameReducedDimensionality(PSQuantityGetUnitDimensionality((PSQuantityRef) theScalar));
            if(quantities) {
                OCArrayAppendArray(result, quantities,OCRangeMake(0, OCArrayGetCount(quantities)));
                OCRelease(quantities);
            }
        }
        if(NULL == quantity) units = SIUnitCreateArrayOfConversionUnits(PSQuantityGetUnit((PSQuantityRef) theScalar));
        else units = SIUnitCreateArrayOfUnitsForQuantity(quantity);
        
        if(units) {
            for(uint64_t index = 0; index<OCArrayGetCount(units); index++) {
                SIUnitRef unit = (SIUnitRef) OCArrayGetValueAtIndex(units, index);
                
                if(unit && fahrenheit!=unit && celsius !=unit) {
                    PSScalarRef newScalar = PSScalarCreateByConvertingToUnit(theScalar, unit, error);
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
                            int magnitude = log10(fabs(PSScalarDoubleValue(newScalar)));
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
        if(SIDimensionalityHasSameReducedDimensionality(SIDimensionalityForQuantity(kPSQuantityTime,NULL), PSQuantityGetUnitDimensionality((PSQuantityRef)  theScalar)) && PSScalarIsReal(theScalar)) {
            SIUnitRef unit1 = SIUnitForUnderivedSymbol(STR("yr"));
            SIUnitRef unit2 = SIUnitForUnderivedSymbol(STR("month"));
            SIUnitRef unit3 = SIUnitForUnderivedSymbol(STR("wk"));
            SIUnitRef unit4 = SIUnitForUnderivedSymbol(STR("d"));
            SIUnitRef unit5 = SIUnitForUnderivedSymbol(STR("h"));
            SIUnitRef unit6 = SIUnitForUnderivedSymbol(STR("min"));
            SIUnitRef unit7 = SIUnitForUnderivedSymbol(STR("s"));
            SIUnitRef theUnits[7] = {unit1,unit2,unit3,unit4, unit5, unit6, unit7};
            OCArrayRef units = OCArrayCreate((const void **) theUnits, 7,&kOCTypeArrayCallBacks);
            OCStringRef stringValue = PSScalarCreateStringValueSplitByUnits(theScalar, units, true, error);
            OCRelease(units);
            if(stringValue) {
                OCArrayAppendValue(result, stringValue);
                OCRelease(stringValue);
            }
        }
        if(SIDimensionalityHasSameReducedDimensionality(SIDimensionalityForQuantity(kPSQuantityVolume,NULL), PSQuantityGetUnitDimensionality((PSQuantityRef)  theScalar)) && PSScalarIsReal(theScalar)) {
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
            OCStringRef stringValue = PSScalarCreateStringValueSplitByUnits(theScalar, units, false, error);
            OCRelease(units);
            if(stringValue) {
                OCArrayAppendValue(result, stringValue);
                OCRelease(stringValue);
            }
        }
        if(SIDimensionalityHasSameReducedDimensionality(SIDimensionalityForQuantity(kPSQuantityLength,NULL), PSQuantityGetUnitDimensionality((PSQuantityRef)  theScalar)) && PSScalarIsReal(theScalar)) {
            SIUnitRef unit1 = SIUnitForUnderivedSymbol(STR("mi"));
            SIUnitRef unit2 = SIUnitForUnderivedSymbol(STR("ft"));
            SIUnitRef unit3 = SIUnitForUnderivedSymbol(STR("in"));
            SIUnitRef theUnits[3] = {unit1,unit2,unit3};
            OCArrayRef units = OCArrayCreate((const void **) theUnits, 3,&kOCTypeArrayCallBacks);
            OCStringRef stringValue = PSScalarCreateStringValueSplitByUnits(theScalar, units, false, error);
            OCRelease(units);
            if(stringValue) {
                OCArrayAppendValue(result, stringValue);
                OCRelease(stringValue);
            }
        }
        if(SIDimensionalityHasSameReducedDimensionality(SIDimensionalityForQuantity(kPSQuantityLength,NULL), PSQuantityGetUnitDimensionality((PSQuantityRef)  theScalar)) && PSScalarIsReal(theScalar)) {
            SIUnitRef unit1 = SIUnitForUnderivedSymbol(STR("mi"));
            SIUnitRef unit2 = SIUnitForUnderivedSymbol(STR("yd"));
            SIUnitRef unit3 = SIUnitForUnderivedSymbol(STR("ft"));
            SIUnitRef unit4 = SIUnitForUnderivedSymbol(STR("in"));
            SIUnitRef theUnits[4] = {unit1,unit2,unit3,unit4};
            OCArrayRef units = OCArrayCreate((const void **) theUnits, 4,&kOCTypeArrayCallBacks);
            OCStringRef stringValue = PSScalarCreateStringValueSplitByUnits(theScalar, units, false, error);
            OCRelease(units);
            if(stringValue) {
                if(OCArrayBSearchValues(result,OCRangeMake(0, OCArrayGetCount(result)),stringValue,
                                        (OCComparatorFunction)compareOnlyTheStrings,NULL) >= OCArrayGetCount(result) ) {
                    OCArrayAppendValue(result, stringValue);
                    OCRelease(stringValue);
                }
            }
        }
        if(SIDimensionalityHasSameReducedDimensionality(SIDimensionalityForQuantity(kPSQuantityMass,NULL), PSQuantityGetUnitDimensionality((PSQuantityRef)  theScalar)) && PSScalarIsReal(theScalar)) {
            SIUnitRef unit1 = SIUnitForUnderivedSymbol(STR("ton"));
            SIUnitRef unit2 = SIUnitForUnderivedSymbol(STR("lb"));
            SIUnitRef unit3 = SIUnitForUnderivedSymbol(STR("oz"));
            SIUnitRef theUnits[3] = {unit1,unit2,unit3};
            OCArrayRef units = OCArrayCreate((const void **) theUnits, 3,&kOCTypeArrayCallBacks);
            OCStringRef stringValue = PSScalarCreateStringValueSplitByUnits(theScalar, units, false, error);
            OCRelease(units);
            if(stringValue) {
                OCArrayAppendValue(result, stringValue);
                OCRelease(stringValue);
            }
        }
        if(SIDimensionalityHasSameReducedDimensionality(SIDimensionalityForQuantity(kPSQuantityMass,NULL), PSQuantityGetUnitDimensionality((PSQuantityRef)  theScalar)) && PSScalarIsReal(theScalar)) {
            SIUnitRef unit1 = SIUnitForUnderivedSymbol(STR("ton"));
            SIUnitRef unit2 = SIUnitForUnderivedSymbol(STR("st"));
            SIUnitRef unit3 = SIUnitForUnderivedSymbol(STR("lb"));
            SIUnitRef unit4 = SIUnitForUnderivedSymbol(STR("oz"));
            SIUnitRef theUnits[4] = {unit1,unit2,unit3,unit4};
            OCArrayRef units = OCArrayCreate((const void **) theUnits, 4,&kOCTypeArrayCallBacks);
            OCStringRef stringValue = PSScalarCreateStringValueSplitByUnits(theScalar, units, false, error);
            OCRelease(units);
            if(stringValue) {
                if(OCArrayBSearchValues(result,OCRangeMake(0, OCArrayGetCount(result)),stringValue,
                                        (OCComparatorFunction)compareOnlyTheStrings,NULL)>=OCArrayGetCount(result) ) {
                    
                    OCArrayAppendValue(result, stringValue);
                    OCRelease(stringValue);
                }
            }
        }
        return result;
    }
    return NULL;
}

OCArrayRef PSScalarCreateArrayOfConversionQuantitiesAndUnits(PSScalarRef theScalar, OCStringRef quantity, OCStringRef *error)
{
    if(error) if(*error) return NULL;
    OCArrayRef units = NULL;
    OCArrayRef quantities = NULL;
    OCMutableArrayRef result = OCArrayCreateMutable(0,&kOCTypeArrayCallBacks);
    
    if(theScalar) {
        if(NULL == quantity) {
            quantities = SIDimensionalityCreateArrayOfQuantitiesWithSameReducedDimensionality(PSQuantityGetUnitDimensionality((PSQuantityRef) theScalar));
            if(quantities) {
                OCArrayAppendArray(result, quantities,OCRangeMake(0, OCArrayGetCount(quantities)));
                OCRelease(quantities);
            }
        }
        if(NULL == quantity) units = SIUnitCreateArrayOfConversionUnits(PSQuantityGetUnit((PSQuantityRef) theScalar));
        else units = SIUnitCreateArrayOfUnitsForQuantity(quantity);
        
        if(units) {
            for(uint64_t index = 0; index<OCArrayGetCount(units); index++) {
                SIUnitRef unit = (SIUnitRef) OCArrayGetValueAtIndex(units, index);
                if(unit) {
                    PSScalarRef newScalar = PSScalarCreateByConvertingToUnit(theScalar, unit, error);
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
                            int magnitude = log10(fabs(PSScalarDoubleValue(newScalar)));
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

void PSScalarShow(PSScalarRef theScalar)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,);
    OCStringRef cf_string = PSScalarCreateStringValue(theScalar);
    if(cf_string) {
        OCStringShow(cf_string);
        OCStringShow(STR("\n"));
        OCRelease(cf_string);
    }
    else fprintf(stdout,"invalid value.");
}

bool PSScalarValidateProposedStringValue(PSScalarRef theScalar,OCStringRef proposedStringValue, OCStringRef *error)
{
    if(error) if(*error) return false;
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,false);
   	IF_NO_OBJECT_EXISTS_RETURN(proposedStringValue,false);
    PSScalarRef proposedValue = PSScalarCreateWithOCString(proposedStringValue,error);
    if(proposedValue==NULL) {
        if(error) {
            SIDimensionalityRef dimensionality = PSQuantityGetUnitDimensionality((PSQuantityRef) theScalar);
            OCStringRef dimensionalitySymbol = SIDimensionalityGetSymbol(dimensionality);
            *error = OCStringCreateWithFormat(STR("Unrecognized input. Value must have dimensionality: %@"),dimensionalitySymbol);
            
        }
        return false;
    }
    else if(!PSQuantityHasSameReducedDimensionality((PSQuantityRef) proposedValue, (PSQuantityRef) theScalar)) {
        if(error) {
            SIDimensionalityRef dimensionality = PSQuantityGetUnitDimensionality((PSQuantityRef) theScalar);
            OCStringRef dimensionalitySymbol = SIDimensionalityGetSymbol(dimensionality);
            *error = OCStringCreateWithFormat(STR("Unrecognized input. Value must have dimensionality: %@"),dimensionalitySymbol);
        }
        return false;
    }
    return true;
}

OCStringRef PSScalarCreateNumericStringValue(PSScalarRef theScalar)
{
    if(theScalar==NULL) return STR("");
    OCStringRef stringValue = NULL;
    
    switch (theScalar->type) {
        case kPSNumberFloat32Type:
        case kPSNumberFloat32ComplexType:
            stringValue = PSScalarCreateNumericStringValueWithFormat(theScalar, STR("%.7g"));
            break;
        case kPSNumberFloat64Type:
        case kPSNumberFloat64ComplexType:
            //            stringValue = PSScalarCreateNumericStringValueWithFormat(theScalar, STR("%.16lg"));
            stringValue = PSScalarCreateNumericStringValueWithFormat(theScalar, STR("%.14lg"));
            break;
    }
    return stringValue;
}

OCStringRef PSScalarCreateStringValue(PSScalarRef theScalar)
{
    OCStringRef stringValue = NULL;
    if(theScalar==NULL) return stringValue;
    
    if(OCGetTypeID(theScalar)==OCNumberGetTypeID()) {
        return OCNumberCreateStringValue((OCNumberRef) ((OCTypeRef) theScalar));
    }
    switch (theScalar->type) {
        case kPSNumberFloat32Type:
        case kPSNumberFloat32ComplexType:
            stringValue = PSScalarCreateStringValueWithFormat(theScalar, STR("%.7g"));
            break;
        case kPSNumberFloat64Type:
        case kPSNumberFloat64ComplexType:
            stringValue = PSScalarCreateStringValueWithFormat(theScalar, STR("%.16lg"));
            break;
    }
    return stringValue;
}

OCStringRef PSScalarCreateStringValueForPart(PSScalarRef theScalar, complexPart thePart)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,NULL);
    PSScalarRef temp = PSScalarCreateByTakingComplexPart(theScalar, thePart);
    OCStringRef string = PSScalarCreateStringValue(temp);
    OCRelease(temp);
    return string;
}

OCStringRef PSScalarCreateNumericStringValueWithFormat(PSScalarRef theScalar, OCStringRef format)
{
    if(theScalar==NULL) return STR("");
    
    switch (theScalar->type) {
        case kPSNumberFloat32Type: {
            float value = PSScalarFloatValue(theScalar);
            if(OCCompareFloatValues(value, 0.0) == kOCCompareEqualTo) value = 0.0;
            
            OCStringRef numericString = OCFloatComplexCreateStringValue(value,format);
            return numericString;
        }
        case kPSNumberFloat64Type: {
            double value = PSScalarDoubleValue(theScalar);
            if(OCCompareDoubleValues(value, 0.0) == kOCCompareEqualTo) value = 0.0;
            
            OCStringRef numericString = PSDoubleComplexCreateStringValue(value,format);
            
            return numericString;
        }
        case kPSNumberFloat32ComplexType: {
            float complex value = PSScalarFloatComplexValue(theScalar);
            OCMutableStringRef cf_string = OCStringCreateMutable(0);
            if(crealf(value)!=0.0 && cimagf(value)!=0.0) OCStringAppend(cf_string, STR("("));
            OCStringRef numericString = OCFloatComplexCreateStringValue(value,format);
            OCStringAppend(cf_string,numericString);
            OCRelease(numericString);
            if(crealf(value)!=0.0 && cimagf(value)!=0.0) OCStringAppend(cf_string, STR(")"));
            return cf_string;
        }
        case kPSNumberFloat64ComplexType: {
            double complex value = PSScalarDoubleComplexValue(theScalar);
            OCMutableStringRef cf_string = OCStringCreateMutable(0);
            if(creal(value)!=0.0 && cimag(value)!=0.0) OCStringAppend(cf_string, STR("("));
            OCStringRef numericString = PSDoubleComplexCreateStringValue(value,format);
            OCStringAppend(cf_string,numericString);
            OCRelease(numericString);
            if(creal(value)!=0.0 && cimag(value)!=0.0) OCStringAppend(cf_string, STR(")"));
            return cf_string;
        }
    }
    return NULL;
}

OCStringRef PSScalarCreateUnitString(PSScalarRef theScalar)
{
    OCStringRef unit_symbol;
    if(SIUnitIsDimensionlessAndUnderived(theScalar->unit)) unit_symbol = STR("");
    else unit_symbol = SIUnitCopySymbol(theScalar->unit);
    return unit_symbol;
}

OCStringRef PSScalarCreateStringValueWithFormat(PSScalarRef theScalar, OCStringRef format)
{
    if(theScalar==NULL) return STR("");
    
    OCStringRef unit_symbol = PSScalarCreateUnitString(theScalar);
    
    switch (theScalar->type) {
        case kPSNumberFloat32Type: {
            float value = PSScalarFloatValue(theScalar);
            if(OCCompareFloatValues(value, 0.0) == kOCCompareEqualTo) value = 0.0;
            
            //            OCStringRef numericString = PSFloatComplexCreateStringValue(value,format);
            OCStringRef numericString = PSFloatCreateStringValue(value);
            
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
        case kPSNumberFloat64Type: {
            double value = PSScalarDoubleValue(theScalar);
            if(OCCompareDoubleValues(value, 0.0) == kOCCompareEqualTo) value = 0.0;
            OCStringRef numericString = OCDoubleCreateStringValue(value);
            
            //            OCStringRef numericString = PSDoubleComplexCreateStringValue(value,format);
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
        case kPSNumberFloat32ComplexType: {
            float complex value = PSScalarFloatComplexValue(theScalar);
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
        case kPSNumberFloat64ComplexType: {
            double complex value = PSScalarDoubleComplexValue(theScalar);
            OCMutableStringRef cf_string = OCStringCreateMutable(0);
            if(creal(value)!=0.0 && cimag(value)!=0.0) OCStringAppend(cf_string, STR("("));
            OCStringRef numericString = PSDoubleComplexCreateStringValue(value,format);
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

void PSScalarAddToArrayAsStringValue(PSScalarRef theScalar, OCMutableArrayRef array)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,);
   	IF_NO_OBJECT_EXISTS_RETURN(array,);
    OCStringRef stringValue = PSScalarCreateStringValue(theScalar);
    OCArrayAppendValue(array, stringValue);
    OCRelease(stringValue);
}

#pragma mark Tests

bool PSScalarIsReal(PSScalarRef theScalar)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,false);
    double complex value = PSScalarDoubleComplexValue(theScalar);
    if(cimag(value)==0.0) return true;
    return false;
}

bool PSScalarIsImaginary(PSScalarRef theScalar)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,false);
    double complex value = PSScalarDoubleComplexValue(theScalar);
    if(creal(value)==0.0 && cabs(value) != 0.0) return true;
    return false;
}

bool PSScalarIsComplex(PSScalarRef theScalar)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,false);
    double complex value = PSScalarDoubleComplexValue(theScalar);
    if(cimag(value)==0.0) return false;
    return true;
}

bool PSScalarIsZero(PSScalarRef theScalar)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,false);
    double complex value = PSScalarDoubleComplexValue(theScalar);
    if(cimag(value)!=0.0) return false;
    if(creal(value)!=0.0) return false;
    return true;
}

bool PSScalarIsInfinite(PSScalarRef theScalar)
{
    IF_NO_OBJECT_EXISTS_RETURN(theScalar,false);
    switch (PSQuantityGetElementType((PSQuantityRef) theScalar)) {
        case kPSNumberFloat32Type:
            if(isinf(theScalar->value.floatValue)) return true;
        case kPSNumberFloat64Type:
            if(isinf(theScalar->value.doubleValue)) return true;
        case kPSNumberFloat32ComplexType:
            if(isinf(crealf(theScalar->value.floatComplexValue))) return true;
            if(isinf(cimagf(theScalar->value.floatComplexValue))) return true;
        case kPSNumberFloat64ComplexType:
            if(isinf(creal(theScalar->value.floatComplexValue))) return true;
            if(isinf(cimag(theScalar->value.floatComplexValue))) return true;
    }
    return false;
}

bool PSScalarIsRealNonNegativeInteger(PSScalarRef theScalar)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,false);
    if(PSScalarIsComplex(theScalar)) return false;
    double value = PSScalarDoubleValue(theScalar);
    double integerPart;
    double fractionalPart = modf(value, &integerPart);
    if(fractionalPart != 0.0) return false;
    if(integerPart>=0) return true;
    return false;
}

bool PSScalarEqual(PSScalarRef input1,PSScalarRef input2)
{
   	IF_NO_OBJECT_EXISTS_RETURN(input1,false);
    IF_NO_OBJECT_EXISTS_RETURN(input2,false);
    if(input1 == input2) return true;
    
    if(input1->type != input2->type) return false;
    if(!SIUnitEqual(input1->unit, input2->unit)) return false;
    
    switch (input1->type) {
        case kPSNumberFloat32Type: {
            if(input1->value.floatValue != input2->value.floatValue) return false;
            break;
        }
        case kPSNumberFloat64Type: {
            if(input1->value.doubleValue != input2->value.doubleValue) return false;
            break;
        }
        case kPSNumberFloat32ComplexType: {
            if(input1->value.floatComplexValue != input2->value.floatComplexValue) return false;
            break;
        }
        case kPSNumberFloat64ComplexType: {
            if(input1->value.doubleComplexValue != input2->value.doubleComplexValue) return false;
            break;
        }
    }
    return true;
}

OCComparisonResult PSScalarCompare(PSScalarRef theScalar, PSScalarRef theOtherScalar)
{
    if(NULL==theScalar) {
        IF_NO_OBJECT_EXISTS_RETURN(theScalar,kOCCompareError);
    }
    if(NULL==theOtherScalar) {
        IF_NO_OBJECT_EXISTS_RETURN(theOtherScalar,kOCCompareError);
    }
    
    if(!SIDimensionalityHasSameReducedDimensionality(PSQuantityGetUnitDimensionality((PSQuantityRef) theScalar),
                                                     PSQuantityGetUnitDimensionality((PSQuantityRef) theOtherScalar))) return kOCCompareUnequalDimensionalities;
    
    PSMutableScalarRef theOtherConverted = PSScalarCreateMutableCopy(theOtherScalar);
    PSScalarConvertToUnit(theOtherConverted, PSQuantityGetUnit((PSQuantityRef) theScalar), NULL);
    
    OCComparisonResult result = kOCCompareError;
    switch (theScalar->type) {
        case kPSNumberFloat32Type: {
            switch (theOtherConverted->type) {
                case kPSNumberFloat32Type: {
                    result = OCCompareFloatValues((float) theScalar->value.floatValue, (float) theOtherConverted->value.floatValue);
                    break;
                }
                case kPSNumberFloat64Type: {
                    result = OCCompareFloatValues((float) theScalar->value.floatValue, (float) theOtherConverted->value.doubleValue);
                    break;
                }
                case kPSNumberFloat32ComplexType: {
                    if(!PSScalarIsReal(theOtherConverted)) {
                        result =  kOCCompareNoSingleValue;
                        break;
                    }
                    result = OCCompareFloatValues((float) theScalar->value.floatValue, (float) crealf(theOtherConverted->value.floatComplexValue));
                    break;
                }
                case kPSNumberFloat64ComplexType: {
                    if(!PSScalarIsReal(theOtherConverted)) {
                        result =  kOCCompareNoSingleValue;
                        break;
                    }
                    result = OCCompareFloatValues((float) theScalar->value.floatValue, (float) creal(theOtherConverted->value.doubleComplexValue));
                    break;
                }
            }
            break;
        }
        case kPSNumberFloat64Type: {
            switch (theOtherConverted->type) {
                case kPSNumberFloat32Type: {
                    result = OCCompareFloatValues((float) theScalar->value.doubleValue, (float) theOtherConverted->value.floatValue);
                    break;
                }
                case kPSNumberFloat64Type: {
                    result = OCCompareDoubleValues((double) theScalar->value.doubleValue, (double) theOtherConverted->value.doubleValue);
                    break;
                }
                case kPSNumberFloat32ComplexType: {
                    if(!PSScalarIsReal(theOtherConverted)) {
                        result =  kOCCompareNoSingleValue;
                        break;
                    }
                    result = OCCompareFloatValues((float) theScalar->value.doubleValue, (float) crealf(theOtherConverted->value.floatComplexValue));
                    break;
                }
                case kPSNumberFloat64ComplexType: {
                    if(!PSScalarIsReal(theOtherConverted)) {
                        result =  kOCCompareNoSingleValue;
                        break;
                    }
                    result = OCCompareDoubleValues((double) theScalar->value.doubleValue, creal(theOtherConverted->value.doubleComplexValue));
                    break;
                }
            }
            break;
        }
        case kPSNumberFloat32ComplexType: {
            switch (theOtherConverted->type) {
                case kPSNumberFloat32Type: {
                    if(!PSScalarIsReal(theScalar)) {
                        result =  kOCCompareNoSingleValue;
                        break;
                    }
                    result = OCCompareFloatValues((float) creal(theScalar->value.floatComplexValue), (float) theOtherConverted->value.floatValue);
                    break;
                }
                case kPSNumberFloat64Type: {
                    if(!PSScalarIsReal(theScalar)) {
                        result =  kOCCompareNoSingleValue;
                        break;
                    }
                    result = OCCompareFloatValues((float) crealf(theScalar->value.floatComplexValue), (float) theOtherConverted->value.doubleValue);
                    break;
                }
                case kPSNumberFloat32ComplexType: {
                    OCComparisonResult realResult =  OCCompareFloatValues((float) crealf(theScalar->value.floatComplexValue), (float) crealf(theOtherConverted->value.floatComplexValue));
                    
                    OCComparisonResult imagResult = OCCompareFloatValues((float) cimagf(theScalar->value.floatComplexValue), (float) cimagf(theOtherConverted->value.floatComplexValue));
                    
                    if(realResult == kOCCompareEqualTo && imagResult == kOCCompareEqualTo) result = kOCCompareEqualTo;
                    else result = kOCCompareNoSingleValue;
                    break;
                }
                case kPSNumberFloat64ComplexType: {
                    OCComparisonResult realResult = OCCompareFloatValues((float) crealf(theScalar->value.floatComplexValue), (float) creal(theOtherConverted->value.doubleComplexValue));
                    
                    OCComparisonResult imagResult = OCCompareFloatValues((float) cimagf(theScalar->value.floatComplexValue), (float) cimag(theOtherConverted->value.doubleComplexValue));
                    
                    if(realResult == kOCCompareEqualTo && imagResult == kOCCompareEqualTo) result = kOCCompareEqualTo;
                    else result = kOCCompareNoSingleValue;
                    break;
                }
            }
            break;
        }
        case kPSNumberFloat64ComplexType: {
            switch (theOtherConverted->type) {
                case kPSNumberFloat32Type: {
                    if(!PSScalarIsReal(theScalar)) {
                        result = kOCCompareNoSingleValue;
                        break;
                    }
                    result = OCCompareFloatValues((float) creal(theScalar->value.doubleComplexValue), (float) theOtherConverted->value.floatValue);
                    break;
                }
                case kPSNumberFloat64Type: {
                    if(!PSScalarIsReal(theScalar)) {
                        result = kOCCompareNoSingleValue;
                        break;
                    }
                    result = OCCompareDoubleValues((double) creal(theScalar->value.doubleComplexValue), (double) theOtherConverted->value.doubleValue);
                    break;
                }
                case kPSNumberFloat32ComplexType: {
                    OCComparisonResult realResult = OCCompareFloatValues((float) creal(theScalar->value.doubleComplexValue), (float) crealf(theOtherConverted->value.floatComplexValue));
                    
                    OCComparisonResult imagResult = OCCompareFloatValues((float) cimag(theScalar->value.doubleComplexValue), (float) cimagf(theOtherConverted->value.floatComplexValue));
                    
                    if(realResult == kOCCompareEqualTo && imagResult == kOCCompareEqualTo) result = kOCCompareEqualTo;
                    else result = kOCCompareNoSingleValue;
                    break;
                }
                case kPSNumberFloat64ComplexType: {
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

OCComparisonResult PSScalarCompareReduced(PSScalarRef theScalar, PSScalarRef theOtherScalar)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,kOCCompareError);
   	IF_NO_OBJECT_EXISTS_RETURN(theOtherScalar,kOCCompareError);
    PSScalarRef theScalarReduced = PSScalarCreateByReducingUnit(theScalar);
    PSScalarRef theOtherScalarReduced = PSScalarCreateByReducingUnit(theOtherScalar);
    OCComparisonResult result = PSScalarCompare(theScalarReduced, theOtherScalarReduced);
    OCRelease(theScalarReduced);
    OCRelease(theOtherScalarReduced);
    return result;
}

OCComparisonResult PSScalarCompareLoose(PSScalarRef theScalar, PSScalarRef theOtherScalar)
{
   	IF_NO_OBJECT_EXISTS_RETURN(theScalar,kOCCompareError);
   	IF_NO_OBJECT_EXISTS_RETURN(theOtherScalar,kOCCompareError);
    
    if(!SIDimensionalityEqual(PSQuantityGetUnitDimensionality((PSQuantityRef) theScalar),
                              PSQuantityGetUnitDimensionality((PSQuantityRef) theOtherScalar))) return kOCCompareUnequalDimensionalities;
    
    PSMutableScalarRef theOtherConverted = PSScalarCreateMutableCopy(theOtherScalar);
    PSScalarConvertToUnit(theOtherConverted, PSQuantityGetUnit((PSQuantityRef) theScalar), NULL);
    
    OCComparisonResult result = kOCCompareError;
    switch (theScalar->type) {
        case kPSNumberFloat32Type: {
            switch (theOtherConverted->type) {
                case kPSNumberFloat32Type: {
                    result = OCCompareFloatValuesLoose((float) theScalar->value.floatValue, (float) theOtherConverted->value.floatValue);
                    break;
                }
                case kPSNumberFloat64Type: {
                    result = OCCompareFloatValuesLoose((float) theScalar->value.floatValue, (float) theOtherConverted->value.doubleValue);
                    break;
                }
                case kPSNumberFloat32ComplexType: {
                    if(!PSScalarIsReal(theOtherConverted)) {
                        result =  kOCCompareNoSingleValue;
                        break;
                    }
                    result = OCCompareFloatValuesLoose((float) theScalar->value.floatValue, (float) crealf(theOtherConverted->value.floatComplexValue));
                    break;
                }
                case kPSNumberFloat64ComplexType: {
                    if(!PSScalarIsReal(theOtherConverted)) {
                        result =  kOCCompareNoSingleValue;
                        break;
                    }
                    result = OCCompareFloatValuesLoose((float) theScalar->value.floatValue, (float) creal(theOtherConverted->value.doubleComplexValue));
                    break;
                }
            }
            break;
        }
        case kPSNumberFloat64Type: {
            switch (theOtherConverted->type) {
                case kPSNumberFloat32Type: {
                    result = OCCompareFloatValuesLoose((float) theScalar->value.doubleValue, (float) theOtherConverted->value.floatValue);
                    break;
                }
                case kPSNumberFloat64Type: {
                    result = OCCompareDoubleValuesLoose((double) theScalar->value.doubleValue, (double) theOtherConverted->value.doubleValue);
                    break;
                }
                case kPSNumberFloat32ComplexType: {
                    if(!PSScalarIsReal(theOtherConverted)) {
                        result =  kOCCompareNoSingleValue;
                        break;
                    }
                    result = OCCompareFloatValuesLoose((float) theScalar->value.doubleValue, (float) crealf(theOtherConverted->value.floatComplexValue));
                    break;
                }
                case kPSNumberFloat64ComplexType: {
                    if(!PSScalarIsReal(theOtherConverted)) {
                        result =  kOCCompareNoSingleValue;
                        break;
                    }
                    result = OCCompareDoubleValuesLoose((double) theScalar->value.doubleValue, creal(theOtherConverted->value.doubleComplexValue));
                    break;
                }
            }
            break;
        }
        case kPSNumberFloat32ComplexType: {
            switch (theOtherConverted->type) {
                case kPSNumberFloat32Type: {
                    if(!PSScalarIsReal(theScalar)) {
                        result =  kOCCompareNoSingleValue;
                        break;
                    }
                    result = OCCompareFloatValuesLoose((float) creal(theScalar->value.floatComplexValue), (float) theOtherConverted->value.floatValue);
                    break;
                }
                case kPSNumberFloat64Type: {
                    if(!PSScalarIsReal(theScalar)) {
                        result =  kOCCompareNoSingleValue;
                        break;
                    }
                    result = OCCompareFloatValuesLoose((float) crealf(theScalar->value.floatComplexValue), (float) theOtherConverted->value.doubleValue);
                    break;
                }
                case kPSNumberFloat32ComplexType: {
                    OCComparisonResult realResult =  OCCompareFloatValuesLoose((float) crealf(theScalar->value.floatComplexValue), (float) crealf(theOtherConverted->value.floatComplexValue));
                    
                    OCComparisonResult imagResult = OCCompareFloatValuesLoose((float) cimagf(theScalar->value.floatComplexValue), (float) cimagf(theOtherConverted->value.floatComplexValue));
                    
                    if(realResult == kOCCompareEqualTo && imagResult == kOCCompareEqualTo) result = kOCCompareEqualTo;
                    else result = kOCCompareNoSingleValue;
                    break;
                }
                case kPSNumberFloat64ComplexType: {
                    OCComparisonResult realResult = OCCompareFloatValuesLoose((float) crealf(theScalar->value.floatComplexValue), (float) creal(theOtherConverted->value.doubleComplexValue));
                    
                    OCComparisonResult imagResult = OCCompareFloatValuesLoose((float) cimagf(theScalar->value.floatComplexValue), (float) cimag(theOtherConverted->value.doubleComplexValue));
                    
                    if(realResult == kOCCompareEqualTo && imagResult == kOCCompareEqualTo) result = kOCCompareEqualTo;
                    else result = kOCCompareNoSingleValue;
                    break;
                }
            }
            break;
        }
        case kPSNumberFloat64ComplexType: {
            switch (theOtherConverted->type) {
                case kPSNumberFloat32Type: {
                    if(!PSScalarIsReal(theScalar)) {
                        result = kOCCompareNoSingleValue;
                        break;
                    }
                    result = OCCompareFloatValuesLoose((float) creal(theScalar->value.doubleComplexValue), (float) theOtherConverted->value.floatValue);
                    break;
                }
                case kPSNumberFloat64Type: {
                    if(!PSScalarIsReal(theScalar)) {
                        result = kOCCompareNoSingleValue;
                        break;
                    }
                    result = OCCompareDoubleValuesLoose((double) creal(theScalar->value.doubleComplexValue), (double) theOtherConverted->value.doubleValue);
                    break;
                }
                case kPSNumberFloat32ComplexType: {
                    OCComparisonResult realResult = OCCompareFloatValuesLoose((float) creal(theScalar->value.doubleComplexValue), (float) crealf(theOtherConverted->value.floatComplexValue));
                    
                    OCComparisonResult imagResult = OCCompareFloatValuesLoose((float) cimag(theScalar->value.doubleComplexValue), (float) cimagf(theOtherConverted->value.floatComplexValue));
                    
                    if(realResult == kOCCompareEqualTo && imagResult == kOCCompareEqualTo) result = kOCCompareEqualTo;
                    else result = kOCCompareNoSingleValue;
                    break;
                }
                case kPSNumberFloat64ComplexType: {
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
