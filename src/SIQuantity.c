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
struct impl_SIQuantity {
    OCBase base;

    // impl_SIQuantity Type attributes
    SIUnitRef       unit;
    SINumberType    type;
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

SINumberType SIQuantityGetElementType(SIQuantityRef quantity)
{
    IF_NO_OBJECT_EXISTS_RETURN(quantity,kSINumberFloat32Type);
    SIQuantityRef theQuantity = (SIQuantityRef) quantity;
    return theQuantity->type;
}

bool SIQuantityHasElementType(SIQuantityRef quantity, SINumberType elementType)
{
    IF_NO_OBJECT_EXISTS_RETURN(quantity,NULL);
    SIQuantityRef theQuantity = (SIQuantityRef) quantity;
    if(elementType != theQuantity->type) return false;
    return true;
}

bool SIQuantityIsComplexType(SIQuantityRef theQuantity)
{
    IF_NO_OBJECT_EXISTS_RETURN(theQuantity,false);
    if(theQuantity->type==kSINumberFloat32ComplexType || theQuantity->type==kSINumberFloat64ComplexType) return true;
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


 SINumberType SIQuantityBestElementType(SIQuantityRef input1, SIQuantityRef input2)
 {
     SIQuantityRef quantity1 = (SIQuantityRef) input1;
     SIQuantityRef quantity2 = (SIQuantityRef) input2;
     switch (quantity1->type) {
         case kSINumberFloat32Type: {
             switch (quantity2->type) {
                 case kSINumberFloat32Type:
                     return kSINumberFloat32Type;
                 case kSINumberFloat64Type:
                     return kSINumberFloat64Type;
                 case kSINumberFloat32ComplexType:
                     return kSINumberFloat32ComplexType;
                 case kSINumberFloat64ComplexType:
                     return kSINumberFloat64ComplexType;
             }
         }
         case kSINumberFloat64Type: {
             switch (quantity2->type) {
                 case kSINumberFloat32Type:
                     return kSINumberFloat64Type;
                 case kSINumberFloat64Type:
                     return kSINumberFloat64Type;
                     return kSINumberFloat64ComplexType;
                 case kSINumberFloat32ComplexType:
                     return kSINumberFloat64ComplexType;
                 case kSINumberFloat64ComplexType:
                     return kSINumberFloat64ComplexType;
             }
         }
         case kSINumberFloat32ComplexType: {
             switch (quantity2->type) {
                 case kSINumberFloat32Type:
                     return kSINumberFloat32ComplexType;
                 case kSINumberFloat64Type:
                     return kSINumberFloat64ComplexType;
                 case kSINumberFloat32ComplexType:
                     return kSINumberFloat32ComplexType;
                 case kSINumberFloat64ComplexType:
                     return kSINumberFloat64ComplexType;
             }
         }
         case kSINumberFloat64ComplexType:
             return kSINumberFloat64ComplexType;
     }
 }

SINumberType SIQuantityLargerElementType(SIQuantityRef input1, SIQuantityRef input2)
{
    SIQuantityRef quantity1 = (SIQuantityRef) input1;
    SIQuantityRef quantity2 = (SIQuantityRef) input2;
    return (quantity1->type > quantity2->type) ? quantity1->type: quantity2->type;
}

SINumberType SIQuantitySmallerElementType(SIQuantityRef input1, SIQuantityRef input2)
{
    SIQuantityRef quantity1 = (SIQuantityRef) input1;
    SIQuantityRef quantity2 = (SIQuantityRef) input2;
    return (quantity1->type < quantity2->type) ? quantity1->type: quantity2->type;
}

