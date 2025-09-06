//
//  SIQuantity.c
//  SITypes
//
//  Created by Philip Grandinetti on 6/13/17.
//  Copyright © 2017 PhySy Ltd. All rights reserved.
//
#include <stdio.h>  // For printf used in IF_NO_OBJECT_EXISTS_RETURN macro
#include "SITypes.h"
// SIQuantity Opaque Type
struct impl_SIQuantity {
    OCBase base;
    // impl_SIQuantity Type attributes
    SIUnitRef unit;
    SINumberType type;
};
SIUnitRef SIQuantityGetUnit(SIQuantityRef quantity) {
    return quantity->unit;
}
bool SIQuantitySetUnit(SIMutableQuantityRef quantity, SIUnitRef unit) {
    IF_NO_OBJECT_EXISTS_RETURN(quantity, false);
    IF_NO_OBJECT_EXISTS_RETURN(unit, false);
    SIMutableQuantityRef theQuantity = (SIMutableQuantityRef)quantity;
    theQuantity->unit = unit;
    return true;
}
OCStringRef SIQuantityCopyUnitSymbol(SIQuantityRef quantity) {
    return SIUnitCopySymbol(quantity->unit);
}
SIDimensionalityRef SIQuantityGetUnitDimensionality(SIQuantityRef quantity) {
    IF_NO_OBJECT_EXISTS_RETURN(quantity, NULL);
    SIQuantityRef theQuantity = (SIQuantityRef)quantity;
    return SIUnitGetDimensionality(theQuantity->unit);
}
SINumberType SIQuantityGetNumericType(SIQuantityRef quantity) {
    IF_NO_OBJECT_EXISTS_RETURN(quantity, kSINumberFloat32Type);
    SIQuantityRef theQuantity = (SIQuantityRef)quantity;
    return theQuantity->type;
}
bool SIQuantityHasNumericType(SIQuantityRef quantity, SINumberType numericType) {
    IF_NO_OBJECT_EXISTS_RETURN(quantity, NULL);
    SIQuantityRef theQuantity = (SIQuantityRef)quantity;
    if (numericType != theQuantity->type) return false;
    return true;
}
bool SIQuantityIsComplexType(SIQuantityRef theQuantity) {
    IF_NO_OBJECT_EXISTS_RETURN(theQuantity, false);
    if (theQuantity->type == kSINumberComplex64Type || theQuantity->type == kSINumberComplex128Type) return true;
    return false;
}
int SIQuantityElementSize(SIQuantityRef quantity) {
    IF_NO_OBJECT_EXISTS_RETURN(quantity, kOCNotFound);
    SIQuantityRef theQuantity = (SIQuantityRef)quantity;
    return OCNumberTypeSize((OCNumberType)theQuantity->type);
}
bool SIQuantityHasDimensionality(SIQuantityRef quantity, SIDimensionalityRef theDimensionality) {
    IF_NO_OBJECT_EXISTS_RETURN(quantity, false);
    SIQuantityRef quantity1 = (SIQuantityRef)quantity;
    return SIDimensionalityEqual(SIUnitGetDimensionality(quantity1->unit), theDimensionality);
    //    return SIDimensionalityHasSameReducedDimensionality(SIUnitGetDimensionality(quantity1->unit), theDimensionality);
}
bool SIQuantityHasSameReducedDimensionality(SIQuantityRef input1, SIQuantityRef input2) {
    if (input1 == NULL || input2 == NULL) return false;
    SIQuantityRef quantity1 = (SIQuantityRef)input1;
    SIQuantityRef quantity2 = (SIQuantityRef)input2;
    return SIDimensionalityHasSameReducedDimensionality(SIQuantityGetUnitDimensionality(quantity1), SIQuantityGetUnitDimensionality(quantity2));
}
bool SIQuantityHasSameDimensionality(SIQuantityRef input1, SIQuantityRef input2) {
    if (input1 == NULL || input2 == NULL) return false;
    SIQuantityRef quantity1 = (SIQuantityRef)input1;
    SIQuantityRef quantity2 = (SIQuantityRef)input2;
    return SIDimensionalityEqual(SIQuantityGetUnitDimensionality(quantity1), SIQuantityGetUnitDimensionality(quantity2));
}
// Helper function to safely check if an object has SIQuantity-compatible structure
static bool isQuantityLike(void *obj) {
    if (!obj) return false;
    // Step 1: Basic pointer sanity check
    uintptr_t addr = (uintptr_t)obj;
    if (addr < 0x1000) return false;  // Avoid NULL and very low addresses
    // Step 2: Try to safely access OCBase
    OCBase *base = (OCBase *)obj;
    // Step 3: Validate OCBase structure
    // Check retain count is reasonable (live objects should have retainCount > 0)
    if (base->retainCount == 0 || base->retainCount > 65535) return false;
    // Check that typeID is not kOCNotATypeID
    if (base->typeID == kOCNotATypeID) return false;
    // Step 4: Try OCGetTypeID - this validates the object more thoroughly
    OCTypeID typeID = OCGetTypeID(obj);
    if (typeID == kOCNotATypeID) return false;
    // Step 5: Now safely check if it has SIQuantity structure
    // We know it's a valid OCBase object, so we can probe the structure
    SIQuantityRef qty = (SIQuantityRef)obj;
    // Check that the SINumberType enum is in valid range
    if (qty->type < kSINumberFloat32Type || qty->type > kSINumberComplex128Type) {
        return false;
    }
    // Check unit pointer (can be NULL, but if not NULL should be valid OCBase)
    if (qty->unit != NULL) {
        OCTypeID unitTypeID = OCGetTypeID(qty->unit);
        if (unitTypeID == kOCNotATypeID) return false;
    }
    // Step 6: Final test - try to call SIQuantity function
    // If this works without crashing, we have a compatible structure
    SIDimensionalityRef dim = SIQuantityGetUnitDimensionality(qty);
    return (dim != NULL || qty->unit == NULL);  // NULL unit is OK (dimensionless)
}
bool SIQuantityValidateMixedArrayForDimensionality(OCArrayRef array, SIDimensionalityRef dimensionality, OCStringRef *outError) {
    if (outError) *outError = NULL;
    if (!array) {
        if (outError) *outError = STR("Input array is NULL");
        return false;
    }
    if (!dimensionality) {
        if (outError) *outError = STR("Dimensionality is NULL");
        return false;
    }
    OCIndex count = OCArrayGetCount(array);
    if (count <= 0) {
        // Empty arrays are valid - they don't violate dimensionality requirements
        return true;
    }
    // Check once if the target dimensionality is dimensionless
    bool isDimensionless = SIDimensionalityIsDimensionless(dimensionality);
    for (OCIndex i = 0; i < count; ++i) {
        OCTypeRef obj = OCArrayGetValueAtIndex(array, i);
        if (!obj) {
            if (outError) *outError = STR("Array contains NULL element");
            return false;
        }
        OCTypeID objType = OCGetTypeID(obj);
        // Check if it's a quantity-like object using safe duck typing
        // This will work for SIScalar and any other types that follow the SIQuantity structure pattern
        if (isQuantityLike((void *)obj)) {
            // It's a quantity-like object - check dimensionality
            SIDimensionalityRef objDim = SIQuantityGetUnitDimensionality((SIQuantityRef)obj);
            if (!SIDimensionalityHasSameReducedDimensionality(objDim, dimensionality)) {
                if (outError) {
                    *outError = STR("Element has incompatible dimensionality");
                }
                return false;
            }
        } else if (objType == OCNumberGetTypeID()) {
            // It's an OCNumber - only allow if target dimensionality is dimensionless
            if (!isDimensionless) {
                if (outError) {
                    *outError = STR("OCNumber elements are only allowed for dimensionless arrays");
                }
                return false;
            }
        } else if (objType == OCStringGetTypeID()) {
            // It's an OCString - try to parse as SIScalar and check dimensionality
            OCStringRef str = (OCStringRef)obj;
            OCStringRef parse_error = NULL;
            SIScalarRef scalar = SIScalarCreateFromExpression(str, &parse_error);
            if (!scalar) {
                if (outError) {
                    *outError = STR("Invalid expression in string element");
                }
                return false;
            }
            // Check dimensionality of the parsed scalar
            SIDimensionalityRef scalarDim = SIQuantityGetUnitDimensionality((SIQuantityRef)scalar);
            bool compatible = SIDimensionalityHasSameReducedDimensionality(scalarDim, dimensionality);
            OCRelease(scalar);
            if (!compatible) {
                if (outError) {
                    *outError = STR("String element has incompatible dimensionality");
                }
                return false;
            }
        } else {
            // Unsupported type
            if (outError) *outError = STR("Array contains unsupported element type");
            return false;
        }
    }
    return true;
}
SINumberType SIQuantityBestNumericType(SIQuantityRef input1, SIQuantityRef input2) {
    SIQuantityRef quantity1 = (SIQuantityRef)input1;
    SIQuantityRef quantity2 = (SIQuantityRef)input2;
    switch (quantity1->type) {
        case kSINumberFloat32Type: {
            switch (quantity2->type) {
                case kSINumberFloat32Type:
                    return kSINumberFloat32Type;
                case kSINumberFloat64Type:
                    return kSINumberFloat64Type;
                case kSINumberComplex64Type:
                    return kSINumberComplex64Type;
                case kSINumberComplex128Type:
                    return kSINumberComplex128Type;
            }
        }
        case kSINumberFloat64Type: {
            switch (quantity2->type) {
                case kSINumberFloat32Type:
                    return kSINumberFloat64Type;
                case kSINumberFloat64Type:
                    return kSINumberFloat64Type;
                    return kSINumberComplex128Type;
                case kSINumberComplex64Type:
                    return kSINumberComplex128Type;
                case kSINumberComplex128Type:
                    return kSINumberComplex128Type;
            }
        }
        case kSINumberComplex64Type: {
            switch (quantity2->type) {
                case kSINumberFloat32Type:
                    return kSINumberComplex64Type;
                case kSINumberFloat64Type:
                    return kSINumberComplex128Type;
                case kSINumberComplex64Type:
                    return kSINumberComplex64Type;
                case kSINumberComplex128Type:
                    return kSINumberComplex128Type;
            }
        }
        case kSINumberComplex128Type:
            return kSINumberComplex128Type;
    }
}
SINumberType SIQuantityLargerNumericType(SIQuantityRef input1, SIQuantityRef input2) {
    SIQuantityRef quantity1 = (SIQuantityRef)input1;
    SIQuantityRef quantity2 = (SIQuantityRef)input2;
    return (quantity1->type > quantity2->type) ? quantity1->type : quantity2->type;
}
SINumberType SIQuantitySmallerNumericType(SIQuantityRef input1, SIQuantityRef input2) {
    SIQuantityRef quantity1 = (SIQuantityRef)input1;
    SIQuantityRef quantity2 = (SIQuantityRef)input2;
    return (quantity1->type < quantity2->type) ? quantity1->type : quantity2->type;
}
