//
//  SIQuantity.c
//  SITypes
//
//  Created by Philip Grandinetti on 6/13/17.
//  Copyright © 2017 PhySy Ltd. All rights reserved.
//

#include "SILibrary.h"
#include <stdio.h> // For printf used in IF_NO_OBJECT_EXISTS_RETURN macro

// SIQuantity Opaque Type
struct __SIQuantity {
    OCBase _base;

    // __SIQuantity Type attributes
    SIUnitRef       unit;
    numberType      type;
};

SIUnitRef SIQuantityGetUnit(SIQuantityRef quantity)
{
    SIQuantityRef theQuantity = (SIQuantityRef) quantity;
    return theQuantity->unit;
}

void SIQuantitySetUnit(SIMutableQuantityRef quantity, SIUnitRef unit)
{
    IF_NO_OBJECT_EXISTS_RETURN(quantity,);
    IF_NO_OBJECT_EXISTS_RETURN(unit,);
    SIMutableQuantityRef theQuantity = (SIMutableQuantityRef) quantity;
    theQuantity->unit = unit;
}

SIDimensionalityRef SIQuantityGetUnitDimensionality(SIQuantityRef quantity)
{
    IF_NO_OBJECT_EXISTS_RETURN(quantity,NULL);
    SIQuantityRef theQuantity = (SIQuantityRef) quantity;
    return SIUnitGetDimensionality(theQuantity->unit);
}

numberType SIQuantityGetElementType(SIQuantityRef quantity)
{
    IF_NO_OBJECT_EXISTS_RETURN(quantity,kOCNumberFloat32Type);
    SIQuantityRef theQuantity = (SIQuantityRef) quantity;
    return theQuantity->type;
}

bool SIQuantityHasElementType(SIQuantityRef quantity, numberType elementType)
{
    IF_NO_OBJECT_EXISTS_RETURN(quantity,NULL);
    SIQuantityRef theQuantity = (SIQuantityRef) quantity;
    if(elementType != theQuantity->type) return false;
    return true;
}

bool SIQuantityIsComplexType(SIQuantityRef theQuantity)
{
    IF_NO_OBJECT_EXISTS_RETURN(theQuantity,false);
    if(theQuantity->type==kOCNumberFloat32ComplexType || theQuantity->type==kOCNumberFloat64ComplexType) return true;
    return false;
}

int SIQuantityElementSize(SIQuantityRef quantity)
{
    IF_NO_OBJECT_EXISTS_RETURN(quantity,kOCNotFound);
    SIQuantityRef theQuantity = (SIQuantityRef) quantity;
    return OCNumberTypeSize((OCNumberType) theQuantity->type);
}

bool SIQuantityHasDimensionality(SIQuantityRef quantity, SIDimensionalityRef theDimensionality)
{
    IF_NO_OBJECT_EXISTS_RETURN(quantity,false);
    SIQuantityRef quantity1 = (SIQuantityRef) quantity;
    
    return SIDimensionalityEqual(SIUnitGetDimensionality(quantity1->unit), theDimensionality);
    //    return SIDimensionalityHasSameReducedDimensionality(SIUnitGetDimensionality(quantity1->unit), theDimensionality);
}

bool SIQuantityHasSameReducedDimensionality(SIQuantityRef input1, SIQuantityRef input2)
{
    if(input1==NULL || input2 == NULL) return false;
    SIQuantityRef quantity1 = (SIQuantityRef) input1;
    SIQuantityRef quantity2 = (SIQuantityRef) input2;
    
    return SIDimensionalityHasSameReducedDimensionality(SIQuantityGetUnitDimensionality(quantity1), SIQuantityGetUnitDimensionality(quantity2));
}

bool SIQuantityHasSameDimensionality(SIQuantityRef input1, SIQuantityRef input2)
{
    if(input1==NULL || input2 == NULL) return false;
    SIQuantityRef quantity1 = (SIQuantityRef) input1;
    SIQuantityRef quantity2 = (SIQuantityRef) input2;
    
    return SIDimensionalityEqual(SIQuantityGetUnitDimensionality(quantity1), SIQuantityGetUnitDimensionality(quantity2));
}

numberType SIQuantityBestElementType(SIQuantityRef input1, SIQuantityRef input2)
{
    SIQuantityRef quantity1 = (SIQuantityRef) input1;
    SIQuantityRef quantity2 = (SIQuantityRef) input2;
    switch (quantity1->type) {
        case kOCNumberFloat32Type: {
            switch (quantity2->type) {
                case kOCNumberFloat32Type:
                    return kOCNumberFloat32Type;
                case kOCNumberFloat64Type:
                    return kOCNumberFloat64Type;
                case kOCNumberFloat32ComplexType:
                    return kOCNumberFloat32ComplexType;
                case kOCNumberFloat64ComplexType:
                    return kOCNumberFloat64ComplexType;
            }
        }
        case kOCNumberFloat64Type: {
            switch (quantity2->type) {
                case kOCNumberFloat32Type:
                    return kOCNumberFloat64Type;
                case kOCNumberFloat64Type:
                    return kOCNumberFloat64Type;
                    return kOCNumberFloat64ComplexType;
                case kOCNumberFloat32ComplexType:
                    return kOCNumberFloat64ComplexType;
                case kOCNumberFloat64ComplexType:
                    return kOCNumberFloat64ComplexType;
            }
        }
        case kOCNumberFloat32ComplexType: {
            switch (quantity2->type) {
                case kOCNumberFloat32Type:
                    return kOCNumberFloat32ComplexType;
                case kOCNumberFloat64Type:
                    return kOCNumberFloat64ComplexType;
                case kOCNumberFloat32ComplexType:
                    return kOCNumberFloat32ComplexType;
                case kOCNumberFloat64ComplexType:
                    return kOCNumberFloat64ComplexType;
            }
        }
        case kOCNumberFloat64ComplexType:
            return kOCNumberFloat64ComplexType;
    }
}

numberType SIQuantityLargerElementType(SIQuantityRef input1, SIQuantityRef input2)
{
    SIQuantityRef quantity1 = (SIQuantityRef) input1;
    SIQuantityRef quantity2 = (SIQuantityRef) input2;
    return (quantity1->type > quantity2->type) ? quantity1->type: quantity2->type;
}

numberType SIQuantitySmallerElementType(SIQuantityRef input1, SIQuantityRef input2)
{
    SIQuantityRef quantity1 = (SIQuantityRef) input1;
    SIQuantityRef quantity2 = (SIQuantityRef) input2;
    return (quantity1->type < quantity2->type) ? quantity1->type: quantity2->type;
}

