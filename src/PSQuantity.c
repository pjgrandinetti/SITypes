//
//  PSQuantity.c
//  SITypes
//
//  Created by Philip Grandinetti on 6/13/17.
//  Copyright © 2017 PhySy Ltd. All rights reserved.
//

#include "SILibrary.h"

// PSQuantity Opaque Type
struct __PSQuantity {
    OCBase _base;

    // __PSQuantity Type attributes
    SIUnitRef       unit;
    numberType      type;
};

SIUnitRef PSQuantityGetUnit(PSQuantityRef quantity)
{
    PSQuantityRef theQuantity = (PSQuantityRef) quantity;
    return theQuantity->unit;
}

void PSQuantitySetUnit(PSMutableQuantityRef quantity, SIUnitRef unit)
{
    IF_NO_OBJECT_EXISTS_RETURN(quantity,);
    IF_NO_OBJECT_EXISTS_RETURN(unit,);
    PSMutableQuantityRef theQuantity = (PSMutableQuantityRef) quantity;
    theQuantity->unit = unit;
}

SIDimensionalityRef PSQuantityGetUnitDimensionality(PSQuantityRef quantity)
{
    IF_NO_OBJECT_EXISTS_RETURN(quantity,NULL);
    PSQuantityRef theQuantity = (PSQuantityRef) quantity;
    return SIUnitGetDimensionality(theQuantity->unit);
}

numberType PSQuantityGetElementType(PSQuantityRef quantity)
{
    IF_NO_OBJECT_EXISTS_RETURN(quantity,kPSNumberFloat32Type);
    PSQuantityRef theQuantity = (PSQuantityRef) quantity;
    return theQuantity->type;
}

bool PSQuantityHasElementType(PSQuantityRef quantity, numberType elementType)
{
    IF_NO_OBJECT_EXISTS_RETURN(quantity,NULL);
    PSQuantityRef theQuantity = (PSQuantityRef) quantity;
    if(elementType != theQuantity->type) return false;
    return true;
}

bool PSQuantityIsComplexType(PSQuantityRef theQuantity)
{
    IF_NO_OBJECT_EXISTS_RETURN(theQuantity,false);
    if(theQuantity->type==kPSNumberFloat32ComplexType || theQuantity->type==kPSNumberFloat64ComplexType) return true;
    return false;
}

int PSQuantityElementSize(PSQuantityRef quantity)
{
    IF_NO_OBJECT_EXISTS_RETURN(quantity,kOCNotFound);
    PSQuantityRef theQuantity = (PSQuantityRef) quantity;
    return OCNumberTypeSize((OCNumberType) theQuantity->type);
}

bool PSQuantityHasDimensionality(PSQuantityRef quantity, SIDimensionalityRef theDimensionality)
{
    IF_NO_OBJECT_EXISTS_RETURN(quantity,false);
    PSQuantityRef quantity1 = (PSQuantityRef) quantity;
    
    return SIDimensionalityEqual(SIUnitGetDimensionality(quantity1->unit), theDimensionality);
    //    return SIDimensionalityHasSameReducedDimensionality(SIUnitGetDimensionality(quantity1->unit), theDimensionality);
}

bool PSQuantityHasSameReducedDimensionality(PSQuantityRef input1, PSQuantityRef input2)
{
    if(input1==NULL || input2 == NULL) return false;
    PSQuantityRef quantity1 = (PSQuantityRef) input1;
    PSQuantityRef quantity2 = (PSQuantityRef) input2;
    
    return SIDimensionalityHasSameReducedDimensionality(PSQuantityGetUnitDimensionality(quantity1), PSQuantityGetUnitDimensionality(quantity2));
}

bool PSQuantityHasSameDimensionality(PSQuantityRef input1, PSQuantityRef input2)
{
    if(input1==NULL || input2 == NULL) return false;
    PSQuantityRef quantity1 = (PSQuantityRef) input1;
    PSQuantityRef quantity2 = (PSQuantityRef) input2;
    
    return SIDimensionalityEqual(PSQuantityGetUnitDimensionality(quantity1), PSQuantityGetUnitDimensionality(quantity2));
}

numberType PSQuantityBestElementType(PSQuantityRef input1, PSQuantityRef input2)
{
    PSQuantityRef quantity1 = (PSQuantityRef) input1;
    PSQuantityRef quantity2 = (PSQuantityRef) input2;
    switch (quantity1->type) {
        case kPSNumberFloat32Type: {
            switch (quantity2->type) {
                case kPSNumberFloat32Type:
                    return kPSNumberFloat32Type;
                case kPSNumberFloat64Type:
                    return kPSNumberFloat64Type;
                case kPSNumberFloat32ComplexType:
                    return kPSNumberFloat32ComplexType;
                case kPSNumberFloat64ComplexType:
                    return kPSNumberFloat64ComplexType;
            }
        }
        case kPSNumberFloat64Type: {
            switch (quantity2->type) {
                case kPSNumberFloat32Type:
                    return kPSNumberFloat64Type;
                case kPSNumberFloat64Type:
                    return kPSNumberFloat64Type;
                    return kPSNumberFloat64ComplexType;
                case kPSNumberFloat32ComplexType:
                    return kPSNumberFloat64ComplexType;
                case kPSNumberFloat64ComplexType:
                    return kPSNumberFloat64ComplexType;
            }
        }
        case kPSNumberFloat32ComplexType: {
            switch (quantity2->type) {
                case kPSNumberFloat32Type:
                    return kPSNumberFloat32ComplexType;
                case kPSNumberFloat64Type:
                    return kPSNumberFloat64ComplexType;
                case kPSNumberFloat32ComplexType:
                    return kPSNumberFloat32ComplexType;
                case kPSNumberFloat64ComplexType:
                    return kPSNumberFloat64ComplexType;
            }
        }
        case kPSNumberFloat64ComplexType:
            return kPSNumberFloat64ComplexType;
    }
}

numberType PSQuantityLargerElementType(PSQuantityRef input1, PSQuantityRef input2)
{
    PSQuantityRef quantity1 = (PSQuantityRef) input1;
    PSQuantityRef quantity2 = (PSQuantityRef) input2;
    return (quantity1->type > quantity2->type) ? quantity1->type: quantity2->type;
}

numberType PSQuantitySmallerElementType(PSQuantityRef input1, PSQuantityRef input2)
{
    PSQuantityRef quantity1 = (PSQuantityRef) input1;
    PSQuantityRef quantity2 = (PSQuantityRef) input2;
    return (quantity1->type < quantity2->type) ? quantity1->type: quantity2->type;
}

