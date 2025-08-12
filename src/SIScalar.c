//
//  SIScalar.c
//  SITypes
//
//  Created by philip on 6/14/17.
//  Copyright © 2017 PhySy Ltd. All rights reserved.
//
#include <math.h>    // For math functions like nan, fabsf, log10, pow, etc.
#include <stdio.h>   // Already likely there or in a header, but good to ensure for printf, etc.
#include <stdlib.h>  // For malloc, free, abs
#include <string.h>
#include "SITypes.h"
static OCTypeID kSIScalarID = kOCNotATypeID;
// SIScalar Opaque Type
struct impl_SIScalar {
    OCBase base;
    // impl_SIQuantity Type attributes
    SIUnitRef unit;
    SINumberType type;
    // impl_SIScalar Type attributes
    impl_SINumber value;
};
OCTypeID SIScalarGetTypeID(void) {
    if (kSIScalarID == kOCNotATypeID) kSIScalarID = OCRegisterType("SIScalar");
    return kSIScalarID;
}
static bool impl_SIScalarEqual(const void *theType1, const void *theType2) {
    if (theType1 == theType2)
        return true;
    if (theType1 == NULL || theType2 == NULL)
        return false;
    SIScalarRef s1 = (SIScalarRef)theType1;
    SIScalarRef s2 = (SIScalarRef)theType2;
    if (s1->base.typeID != s2->base.typeID)
        return false;
    if (s1->type != s2->type)
        return false;
    if (!OCTypeEqual(s1->unit, s2->unit))
        return false;
    switch (s1->type) {
        case kSINumberFloat32Type:
            return s1->value.floatValue == s2->value.floatValue;
        case kSINumberFloat64Type:
            return s1->value.doubleValue == s2->value.doubleValue;
        case kSINumberComplex64Type:
            return s1->value.floatComplexValue == s2->value.floatComplexValue;
        case kSINumberComplex128Type:
            return s1->value.doubleComplexValue == s2->value.doubleComplexValue;
        default:
            return false;
    }
}
static void impl_SIScalarFinalize(const void *theType) {
    if (theType == NULL) return;
    // Cast away const to modify internal fields (not the object itself)
    SIScalarRef scalar = (SIScalarRef)(uintptr_t)theType;
    if (scalar->unit) {
        OCRelease(scalar->unit);
        // Cast away const to allow nulling the field
        ((struct impl_SIScalar *)scalar)->unit = NULL;
    }
}
static OCStringRef impl_SIScalarCopyFormattingDescription(OCTypeRef theType) {
    if (!theType) return OCStringCreateWithCString("(null)");
    SIScalarRef scalar = (SIScalarRef)theType;
    OCStringRef unitDesc = scalar->unit ? OCTypeCopyFormattingDesc(scalar->unit) : STR("");
    OCStringRef result = NULL;
    switch (scalar->type) {
        case kSINumberFloat32Type:
            result = OCStringCreateWithFormat(STR("%f %@"),
                                              scalar->value.floatValue,
                                              unitDesc);
            break;
        case kSINumberFloat64Type:
            result = OCStringCreateWithFormat(STR("%lf %@"),
                                              scalar->value.doubleValue,
                                              unitDesc);
            break;
        case kSINumberComplex64Type:
            result = OCStringCreateWithFormat(STR("%f+I•%f %@"),
                                              crealf(scalar->value.floatComplexValue),
                                              cimagf(scalar->value.floatComplexValue),
                                              unitDesc);
            break;
        case kSINumberComplex128Type:
            result = OCStringCreateWithFormat(STR("%lf+I•%lf %@"),
                                              creal(scalar->value.doubleComplexValue),
                                              cimag(scalar->value.doubleComplexValue),
                                              unitDesc);
            break;
        default:
            result = OCStringCreateWithCString("<Invalid SIScalar>");
            break;
    }
    if (scalar->unit)
        OCRelease(unitDesc);
    return result;
}
static SIScalarRef SIScalarCreate(SIUnitRef unit, SINumberType type, void *value);
static SIMutableScalarRef SIScalarCreateMutable(SIUnitRef unit, SINumberType type, void *value);
static void *impl_SIScalarDeepCopy(const void *theType) {
    if (!theType) return NULL;
    SIScalarRef original = (SIScalarRef)theType;
    SIScalarRef copy = SIScalarCreate(original->unit, original->type, (void *)&original->value);
    return (void *)copy;
}
static void *impl_SIScalarDeepCopyMutable(const void *theType) {
    if (!theType) return NULL;
    SIScalarRef original = (SIScalarRef)theType;
    SIMutableScalarRef copy = SIScalarCreateMutable(original->unit, original->type, (void *)&original->value);
    return (void *)copy;
}
// Expose the formatting description function
OCStringRef SIScalarCopyFormattingDescription(SIScalarRef scalar) {
    return impl_SIScalarCopyFormattingDescription((OCTypeRef)scalar);
}
static cJSON *impl_SIScalarCopyJSON(const void *obj) {
    return SIScalarCreateJSON((SIScalarRef)obj);
}
static struct impl_SIScalar *SIScalarAllocate(void) {
    struct impl_SIScalar *obj = OCTypeAlloc(struct impl_SIScalar,
                                            SIScalarGetTypeID(),
                                            impl_SIScalarFinalize,
                                            impl_SIScalarEqual,
                                            impl_SIScalarCopyFormattingDescription,
                                            impl_SIScalarCopyJSON,
                                            impl_SIScalarDeepCopy,
                                            impl_SIScalarDeepCopyMutable);
    obj->unit = NULL;
    obj->type = kSINumberFloat32Type;
    memset(&obj->value, 0, sizeof(obj->value));
    return obj;
}
SIScalarRef SIScalarCreate(SIUnitRef unit, SINumberType type, void *value) {
    if (!value) return NULL;
    struct impl_SIScalar *scalar = SIScalarAllocate();
    if (!scalar) return NULL;
    scalar->type = type;
    switch (type) {
        case kSINumberFloat32Type:
            scalar->value.floatValue = *(float *)value;
            break;
        case kSINumberFloat64Type:
            scalar->value.doubleValue = *(double *)value;
            break;
        case kSINumberComplex64Type:
            scalar->value.floatComplexValue = *(float complex *)value;
            break;
        case kSINumberComplex128Type:
            scalar->value.doubleComplexValue = *(double complex *)value;
            break;
        default:
            // Defensive: Invalid type; clean up and return NULL.
            OCRelease(scalar);
            return NULL;
    }
    scalar->unit = unit ? unit : SIUnitDimensionlessAndUnderived();
    return (SIScalarRef)scalar;
}
static SIMutableScalarRef SIScalarCreateMutable(SIUnitRef unit, SINumberType elementType, void *value) {
    return (SIMutableScalarRef)SIScalarCreate(unit, elementType, value);
}
cJSON *SIScalarCreateJSON(SIScalarRef scalar) {
    if (!scalar) return cJSON_CreateNull();
    OCStringRef stringValue = SIScalarCreateStringValue(scalar);
    if (!stringValue) {
        fprintf(stderr, "SIScalarCreateJSON: Failed to get symbol.\n");
        return cJSON_CreateNull();
    }
    const char *s = OCStringGetCString(stringValue);
    cJSON *node = cJSON_CreateString(s ? s : "");
    OCRelease(stringValue);
    return node;
}
SIScalarRef SIScalarCreateFromJSON(cJSON *json) {
    if (!json || !cJSON_IsString(json)) return NULL;
    const char *str = json->valuestring;
    if (!str) return NULL;
    OCStringRef expressions = OCStringCreateWithCString(str);
    if (!expressions) return NULL;
    OCStringRef err = NULL;
    SIScalarRef scalar = SIScalarCreateFromExpression(expressions, &err);
    OCRelease(expressions);
    OCRelease(err);
    return scalar;
}
/*
 @function SIScalarCreateCopy
 @abstract Creates a copy of a scalar
 @param theScalar The scalar.
 @result a copy of the scalar.
 */
SIScalarRef SIScalarCreateCopy(SIScalarRef theScalar) {
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, NULL);
    return SIScalarCreate(theScalar->unit, theScalar->type, (void *)&theScalar->value);
}
SIMutableScalarRef SIScalarCreateMutableCopy(SIScalarRef theScalar) {
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, NULL);
    return (SIMutableScalarRef)SIScalarCreate(theScalar->unit, theScalar->type, (void *)&theScalar->value);
}
SIScalarRef SIScalarCreateWithFloat(float input_value, SIUnitRef unit) {
    return SIScalarCreate(unit, kSINumberFloat32Type, &input_value);
}
SIMutableScalarRef SIScalarCreateMutableWithFloat(float input_value, SIUnitRef unit) {
    return SIScalarCreateMutable(unit, kSINumberFloat32Type, &input_value);
}
SIScalarRef SIScalarCreateWithDouble(double input_value, SIUnitRef unit) {
    return SIScalarCreate(unit, kSINumberFloat64Type, &input_value);
}
SIMutableScalarRef SIScalarCreateMutableWithDouble(double input_value, SIUnitRef unit) {
    return SIScalarCreateMutable(unit, kSINumberFloat64Type, &input_value);
}
SIScalarRef SIScalarCreateWithFloatComplex(float complex input_value, SIUnitRef unit) {
    return SIScalarCreate(unit, kSINumberComplex64Type, &input_value);
}
SIMutableScalarRef SIScalarCreateMutableWithFloatComplex(float complex input_value, SIUnitRef unit) {
    return SIScalarCreateMutable(unit, kSINumberComplex64Type, &input_value);
}
SIScalarRef SIScalarCreateWithDoubleComplex(double complex input_value, SIUnitRef unit) {
    return SIScalarCreate(unit, kSINumberComplex128Type, &input_value);
}
SIMutableScalarRef SIScalarCreateMutableWithDoubleComplex(double complex input_value, SIUnitRef unit) {
    return SIScalarCreateMutable(unit, kSINumberComplex128Type, &input_value);
}
// helper: take a mutable scalar and re‐prefix it so its mantissa ∈ [1,1000)
static bool __NormalizeScalarMantissaToSI3(SIMutableScalarRef sc) {
    if (!sc) return false;
    // (1) compute our “raw” magnitude in coherent‐SI units
    bool wasComplex =
        (sc->type == kSINumberComplex64Type ||
         sc->type == kSINumberComplex128Type);
    double raw;
    if (wasComplex) {
        // make a temporary copy, convert to coherent, then take its magnitude
        SIMutableScalarRef tmp = SIScalarCreateMutableCopy(sc);
        SIUnitRef cohTest =
            SIUnitCoherentUnitFromDimensionality(
                SIUnitGetDimensionality(tmp->unit));
        SIScalarConvertToUnit(tmp, cohTest, NULL);
        raw = SIScalarMagnitudeValue(tmp);
        OCRelease(tmp);
    } else {
        raw = SIScalarDoubleValueInCoherentUnit(sc);
    }
    // zero stays zero
    if (!(raw > 0.0 || raw < 0.0)) return true;
    // (2) pick nearest multiple-of-3 exponent in [–24, +24]
    int e3 = (int)floor(log10(fabs(raw)) / 3.0) * 3;
    if (e3 > 24)
        e3 = 24;
    else if (e3 < -24)
        e3 = -24;
    // (3) convert *first* into the coherent‐SI unit for this dimensionality
    SIUnitRef origUnit = SIQuantityGetUnit((SIQuantityRef)sc);
    SIDimensionalityRef dim = SIUnitGetDimensionality(origUnit);
    SIUnitRef cohUnit = SIUnitCoherentUnitFromDimensionality(dim);
    if (!SIScalarConvertToUnit(sc, cohUnit, NULL)) return false;
    // Disabled this code with SIUnit refactor
    // // (4) if prefixes are allowed, apply the matching SI prefix
    // if (SIUnitAllowsSIPrefix(cohUnit)) {
    //     static const struct {
    //         int exp;
    //         const char *sym;
    //     } prefixes[] = {
    //         {-24, "y"}, {-21, "z"}, {-18, "a"}, {-15, "f"}, {-12, "p"}, {-9, "n"}, {-6, "µ"}, {-3, "m"}, {+3, "k"}, {+6, "M"}, {+9, "G"}, {+12, "T"}, {+15, "P"}, {+18, "E"}, {+21, "Z"}, {+24, "Y"}};
    //     const char *preSym = NULL;
    //     for (size_t i = 0; i < sizeof(prefixes) / sizeof(*prefixes); ++i) {
    //         if (prefixes[i].exp == e3) {
    //             preSym = prefixes[i].sym;
    //             break;
    //         }
    //     }
    //     if (preSym) {
    //         // build the symbol, e.g. "k" + "m" → "km"
    //         OCStringRef rootSym = SIUnitCopyRootSymbol(cohUnit);
    //         OCMutableStringRef tmp = OCMutableStringCreateWithCString(preSym);
    //         OCStringAppend(tmp, rootSym);
    //         OCRelease(rootSym);
    //         // look up the prefixed unit (borrowed reference)
    //         SIUnitRef prefixed = SIUnitWithSymbol(tmp);
    //         OCRelease(tmp);
    //         if (prefixed) {
    //             SIScalarConvertToUnit(sc, prefixed, NULL);
    //         }
    //     }
    // }
    // (5) finish with a 64-bit type, preserving real vs. complex
    SIScalarSetNumericType(
        sc,
        wasComplex ? kSINumberComplex128Type
                   : kSINumberFloat64Type);
    return true;
}
#pragma mark Accessors
void SIScalarSetNumericType(SIMutableScalarRef theScalar, SINumberType numericType) {
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, );
    switch (theScalar->type) {
        case kSINumberFloat32Type: {
            float value = theScalar->value.floatValue;
            theScalar->type = numericType;
            switch (numericType) {
                case kSINumberFloat32Type:
                    theScalar->value.floatValue = value;
                    return;
                case kSINumberFloat64Type:
                    theScalar->value.doubleValue = value;
                    return;
                case kSINumberComplex64Type:
                    theScalar->value.floatComplexValue = value;
                    return;
                case kSINumberComplex128Type:
                    theScalar->value.doubleComplexValue = value;
                    return;
            }
            break;
        }
        case kSINumberFloat64Type: {
            double value = theScalar->value.doubleValue;
            theScalar->type = numericType;
            switch (numericType) {
                case kSINumberFloat32Type:
                    theScalar->value.floatValue = value;
                    return;
                case kSINumberFloat64Type:
                    theScalar->value.doubleValue = value;
                    return;
                case kSINumberComplex64Type:
                    theScalar->value.floatComplexValue = value;
                    return;
                case kSINumberComplex128Type:
                    theScalar->value.doubleComplexValue = value;
                    return;
            }
            break;
        }
        case kSINumberComplex64Type: {
            float complex value = theScalar->value.floatComplexValue;
            theScalar->type = numericType;
            switch (numericType) {
                case kSINumberFloat32Type:
                    theScalar->value.floatValue = value;
                    return;
                case kSINumberFloat64Type:
                    theScalar->value.doubleValue = value;
                    return;
                case kSINumberComplex64Type:
                    theScalar->value.floatComplexValue = value;
                    return;
                case kSINumberComplex128Type:
                    theScalar->value.doubleComplexValue = value;
                    return;
            }
            break;
        }
        case kSINumberComplex128Type: {
            double complex value = theScalar->value.doubleComplexValue;
            theScalar->type = numericType;
            switch (numericType) {
                case kSINumberFloat32Type:
                    theScalar->value.floatValue = value;
                    return;
                case kSINumberFloat64Type:
                    theScalar->value.doubleValue = value;
                    return;
                case kSINumberComplex64Type:
                    theScalar->value.floatComplexValue = value;
                    return;
                case kSINumberComplex128Type:
                    theScalar->value.doubleComplexValue = value;
                    return;
            }
            break;
        }
    }
    return;
}
impl_SINumber SIScalarGetValue(SIScalarRef theScalar) {
    return theScalar->value;
}
void SIScalarSetFloatValue(SIMutableScalarRef theScalar, float value) {
    theScalar->type = kSINumberFloat32Type;
    theScalar->value.floatValue = value;
    __NormalizeScalarMantissaToSI3(theScalar);
}
void SIScalarSetDoubleValue(SIMutableScalarRef theScalar, double value) {
    theScalar->type = kSINumberFloat64Type;
    theScalar->value.doubleValue = value;
    __NormalizeScalarMantissaToSI3(theScalar);
}
void SIScalarSetFloatComplexValue(SIMutableScalarRef theScalar, float complex value) {
    theScalar->type = kSINumberComplex64Type;
    theScalar->value.floatComplexValue = value;
    __NormalizeScalarMantissaToSI3(theScalar);
}
void SIScalarSetDoubleComplexValue(SIMutableScalarRef theScalar, double complex value) {
    theScalar->type = kSINumberComplex128Type;
    theScalar->value.doubleComplexValue = value;
    __NormalizeScalarMantissaToSI3(theScalar);
}
float SIScalarFloatValue(SIScalarRef theScalar) {
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, 0);
    switch (theScalar->type) {
        case kSINumberFloat32Type:
            return (float)theScalar->value.floatValue;
        case kSINumberFloat64Type:
            return (float)theScalar->value.doubleValue;
        case kSINumberComplex64Type:
            return (float)theScalar->value.floatComplexValue;
        case kSINumberComplex128Type:
            return (float)theScalar->value.doubleComplexValue;
    }
    return nan(NULL);
}
double SIScalarDoubleValue(SIScalarRef theScalar) {
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, 0);
    switch (theScalar->type) {
        case kSINumberFloat32Type:
            return (double)theScalar->value.floatValue;
        case kSINumberFloat64Type:
            return (double)theScalar->value.doubleValue;
        case kSINumberComplex64Type:
            return (double)theScalar->value.floatComplexValue;
        case kSINumberComplex128Type:
            return (double)theScalar->value.doubleComplexValue;
    }
    return nan(NULL);
}
float complex SIScalarFloatComplexValue(SIScalarRef theScalar) {
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, 0);
    switch (theScalar->type) {
        case kSINumberFloat32Type:
            return (float complex)theScalar->value.floatValue;
        case kSINumberFloat64Type:
            return (float complex)theScalar->value.doubleValue;
        case kSINumberComplex64Type:
            return (float complex)theScalar->value.floatComplexValue;
        case kSINumberComplex128Type:
            return (float complex)theScalar->value.doubleComplexValue;
    }
    return nan(NULL);
}
double complex SIScalarDoubleComplexValue(SIScalarRef theScalar) {
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, 0);
    switch (theScalar->type) {
        case kSINumberFloat32Type:
            return (double complex)theScalar->value.floatValue;
        case kSINumberFloat64Type:
            return (double complex)theScalar->value.doubleValue;
        case kSINumberComplex64Type:
            return (double complex)theScalar->value.floatComplexValue;
        case kSINumberComplex128Type:
            return (double complex)theScalar->value.doubleComplexValue;
    }
    return nan(NULL);
}
double SIScalarMagnitudeValue(SIScalarRef theScalar) {
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, 0);
    double complex value = SIScalarDoubleComplexValue(theScalar);
    return cabs(value);
}
double SIScalarArgumentValue(SIScalarRef theScalar) {
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, 0);
    double complex value = SIScalarDoubleComplexValue(theScalar);
    return cargument(value);
}
float SIScalarFloatValueInCoherentUnit(SIScalarRef theScalar) {
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, nan(NULL));
    SIUnitRef coherentUnit = SIUnitCoherentUnitFromDimensionality(SIUnitGetDimensionality(theScalar->unit));
    double conversion = SIUnitConversion(theScalar->unit, coherentUnit);
    switch (theScalar->type) {
        case kSINumberFloat32Type: {
            return theScalar->value.floatValue * conversion;
        }
        case kSINumberFloat64Type: {
            return theScalar->value.doubleValue * conversion;
        }
        case kSINumberComplex64Type: {
            return theScalar->value.floatComplexValue * conversion;
        }
        case kSINumberComplex128Type: {
            return theScalar->value.doubleComplexValue * conversion;
        }
    }
}
double SIScalarDoubleValueInCoherentUnit(SIScalarRef theScalar) {
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, nan(NULL));
    SIUnitRef coherentUnit = SIUnitCoherentUnitFromDimensionality(SIUnitGetDimensionality(theScalar->unit));
    double conversion = SIUnitConversion(theScalar->unit, coherentUnit);
    switch (theScalar->type) {
        case kSINumberFloat32Type: {
            return theScalar->value.floatValue * conversion;
        }
        case kSINumberFloat64Type: {
            return theScalar->value.doubleValue * conversion;
        }
        case kSINumberComplex64Type: {
            return theScalar->value.floatComplexValue * conversion;
        }
        case kSINumberComplex128Type: {
            return theScalar->value.doubleComplexValue * conversion;
        }
    }
}
float complex SIScalarFloatComplexValueInCoherentUnit(SIScalarRef theScalar) {
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, nan(NULL));
    SIUnitRef coherentUnit = SIUnitCoherentUnitFromDimensionality(SIUnitGetDimensionality(theScalar->unit));
    double conversion = SIUnitConversion(theScalar->unit, coherentUnit);
    switch (theScalar->type) {
        case kSINumberFloat32Type: {
            return theScalar->value.floatValue * conversion;
        }
        case kSINumberFloat64Type: {
            return theScalar->value.doubleValue * conversion;
        }
        case kSINumberComplex64Type: {
            return theScalar->value.floatComplexValue * conversion;
        }
        case kSINumberComplex128Type: {
            return theScalar->value.doubleComplexValue * conversion;
        }
    }
}
double complex SIScalarDoubleComplexValueInCoherentUnit(SIScalarRef theScalar) {
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, nan(NULL));
    SIUnitRef coherentUnit = SIUnitCoherentUnitFromDimensionality(SIUnitGetDimensionality(theScalar->unit));
    double conversion = SIUnitConversion(theScalar->unit, coherentUnit);
    switch (theScalar->type) {
        case kSINumberFloat32Type: {
            return theScalar->value.floatValue * conversion;
        }
        case kSINumberFloat64Type: {
            return theScalar->value.doubleValue * conversion;
        }
        case kSINumberComplex64Type: {
            return theScalar->value.floatComplexValue * conversion;
        }
        case kSINumberComplex128Type: {
            return theScalar->value.doubleComplexValue * conversion;
        }
    }
}
float SIScalarFloatValueInUnit(SIScalarRef theScalar, SIUnitRef unit, bool *success) {
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, nan(NULL));
    if (SIDimensionalityHasSameReducedDimensionality(SIQuantityGetUnitDimensionality((SIQuantityRef)theScalar), SIUnitGetDimensionality(unit))) {
        double conversion = SIUnitConversion(theScalar->unit, unit);
        switch (theScalar->type) {
            case kSINumberFloat32Type: {
                return theScalar->value.floatValue * conversion;
            }
            case kSINumberFloat64Type: {
                return theScalar->value.doubleValue * conversion;
            }
            case kSINumberComplex64Type: {
                return theScalar->value.floatComplexValue * conversion;
            }
            case kSINumberComplex128Type: {
                return theScalar->value.doubleComplexValue * conversion;
            }
        }
    }
    if (success) *success *= false;
    return nan(NULL);
}
double SIScalarDoubleValueInUnit(SIScalarRef theScalar, SIUnitRef unit, bool *success) {
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, nan(NULL));
    if (SIDimensionalityHasSameReducedDimensionality(SIQuantityGetUnitDimensionality((SIQuantityRef)theScalar), SIUnitGetDimensionality(unit))) {
        double conversion = SIUnitConversion(theScalar->unit, unit);
        switch (theScalar->type) {
            case kSINumberFloat32Type: {
                return theScalar->value.floatValue * conversion;
            }
            case kSINumberFloat64Type: {
                return theScalar->value.doubleValue * conversion;
            }
            case kSINumberComplex64Type: {
                return theScalar->value.floatComplexValue * conversion;
            }
            case kSINumberComplex128Type: {
                return theScalar->value.doubleComplexValue * conversion;
            }
        }
    }
    if (success) *success *= false;
    return nan(NULL);
}
float complex SIScalarFloatComplexValueInUnit(SIScalarRef theScalar, SIUnitRef unit, bool *success) {
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, nan(NULL));
    if (SIDimensionalityHasSameReducedDimensionality(SIQuantityGetUnitDimensionality((SIQuantityRef)theScalar), SIUnitGetDimensionality(unit))) {
        double conversion = SIUnitConversion(theScalar->unit, unit);
        switch (theScalar->type) {
            case kSINumberFloat32Type: {
                return theScalar->value.floatValue * conversion;
            }
            case kSINumberFloat64Type: {
                return theScalar->value.doubleValue * conversion;
            }
            case kSINumberComplex64Type: {
                return theScalar->value.floatComplexValue * conversion;
            }
            case kSINumberComplex128Type: {
                return theScalar->value.doubleComplexValue * conversion;
            }
        }
    }
    if (success) *success *= false;
    return nan(NULL);
}
double complex SIScalarDoubleComplexValueInUnit(SIScalarRef theScalar, SIUnitRef unit, bool *success) {
    if (NULL == theScalar) {
        *success = false;
        return nan(NULL);
    }
    if (SIDimensionalityHasSameReducedDimensionality(SIQuantityGetUnitDimensionality((SIQuantityRef)theScalar), SIUnitGetDimensionality(unit))) {
        double conversion = SIUnitConversion(theScalar->unit, unit);
        switch (theScalar->type) {
            case kSINumberFloat32Type: {
                return theScalar->value.floatValue * conversion;
            }
            case kSINumberFloat64Type: {
                return theScalar->value.doubleValue * conversion;
            }
            case kSINumberComplex64Type: {
                return theScalar->value.floatComplexValue * conversion;
            }
            case kSINumberComplex128Type: {
                return theScalar->value.doubleComplexValue * conversion;
            }
        }
    }
    if (success) *success *= false;
    return nan(NULL);
}
#pragma mark Operations
SIScalarRef SIScalarCreateByConvertingToNumberType(SIScalarRef theScalar, SINumberType numericType) {
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, NULL);
    SIScalarRef result = SIScalarCreateCopy(theScalar);
    SIScalarSetNumericType((SIMutableScalarRef)result, numericType);
    return result;
}
bool SIScalarTakeComplexPart(SIMutableScalarRef scalar, complexPart part) {
    IF_NO_OBJECT_EXISTS_RETURN(scalar, false);
    // bool isComplex = SIQuantityIsComplexType((SIQuantityRef)scalar);
    switch (scalar->type) {
        case kSINumberFloat32Type:
        case kSINumberFloat64Type: {
            if (part == kSIMagnitudePart) {
                if (scalar->type == kSINumberFloat32Type)
                    scalar->value.floatValue = fabsf(scalar->value.floatValue);
                else
                    scalar->value.doubleValue = fabs(scalar->value.doubleValue);
                return true;
            }
            if (part == kSIImaginaryPart || part == kSIArgumentPart) {
                if (scalar->type == kSINumberFloat32Type)
                    scalar->value.floatValue = 0.0f;
                else
                    scalar->value.doubleValue = 0.0;
                return true;
            }
            return true;  // Real part is already the value
        }
        case kSINumberComplex64Type:
        case kSINumberComplex128Type: {
            double complex z = (scalar->type == kSINumberComplex64Type)
                                   ? (double complex)(scalar->value.floatComplexValue)
                                   : scalar->value.doubleComplexValue;
            double result = 0.0;
            switch (part) {
                case kSIRealPart:
                    result = creal(z);
                    break;
                case kSIImaginaryPart:
                    result = cimag(z);
                    break;
                case kSIArgumentPart:
                    result = cargument(z);
                    break;
                case kSIMagnitudePart:
                    result = cabs(z);
                    break;
                default:
                    return false;
            }
            if (scalar->type == kSINumberComplex64Type) {
                scalar->value.floatValue = (float)result;
                scalar->type = kSINumberFloat32Type;
            } else {
                scalar->value.doubleValue = result;
                scalar->type = kSINumberFloat64Type;
            }
            if (part == kSIArgumentPart)
                scalar->unit = SIUnitWithSymbol(STR("rad"));
            return true;
        }
    }
    return false;
}
SIScalarRef SIScalarCreateByTakingComplexPart(SIScalarRef theScalar, complexPart part) {
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, NULL);
    SIScalarRef result = SIScalarCreateCopy(theScalar);
    if (SIScalarTakeComplexPart((SIMutableScalarRef)result, part)) {
        return result;
    }
    OCRelease(result);
    return NULL;
}
bool SIScalarReduceUnit(SIMutableScalarRef theScalar) {
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, false);
    switch (theScalar->type) {
        case kSINumberFloat32Type: {
            double unit_multiplier = 1.0;
            SIUnitRef reducedUnit = SIUnitByReducing(theScalar->unit, &unit_multiplier);
            theScalar->unit = reducedUnit;
            theScalar->value.floatValue = theScalar->value.floatValue * unit_multiplier;
            return true;
        }
        case kSINumberFloat64Type: {
            double unit_multiplier = 1.0;
            SIUnitRef reducedUnit = SIUnitByReducing(theScalar->unit, &unit_multiplier);
            theScalar->unit = reducedUnit;
            theScalar->value.doubleValue = theScalar->value.doubleValue * unit_multiplier;
            return true;
        }
        case kSINumberComplex64Type: {
            double unit_multiplier = 1.0;
            SIUnitRef reducedUnit = SIUnitByReducing(theScalar->unit, &unit_multiplier);
            theScalar->unit = reducedUnit;
            theScalar->value.floatComplexValue = theScalar->value.floatComplexValue * unit_multiplier;
            return true;
        }
        case kSINumberComplex128Type: {
            double unit_multiplier = 1.0;
            SIUnitRef reducedUnit = SIUnitByReducing(theScalar->unit, &unit_multiplier);
            theScalar->unit = reducedUnit;
            theScalar->value.doubleComplexValue = theScalar->value.doubleComplexValue * unit_multiplier;
            return true;
        }
    }
    return false;
}
SIScalarRef SIScalarCreateByReducingUnit(SIScalarRef theScalar) {
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, NULL);
    SIScalarRef result = SIScalarCreateCopy(theScalar);
    if (SIScalarReduceUnit((SIMutableScalarRef)result)) {
        return result;
    }
    if (result) OCRelease(result);
    return NULL;
}
bool SIScalarConvertToUnit(SIMutableScalarRef theScalar, SIUnitRef unit, OCStringRef *error) {
    if (error)
        if (*error) return false;
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, false);
    if (!SIDimensionalityHasSameReducedDimensionality(SIUnitGetDimensionality(theScalar->unit), SIUnitGetDimensionality(unit))) {
        if (error == NULL) return false;
        *error = STR("Incompatible Dimensionalities.");
        return false;
    }
    double conversion = SIUnitConversion(theScalar->unit, unit);
    theScalar->unit = unit;
    switch (theScalar->type) {
        case kSINumberFloat32Type: {
            theScalar->value.floatValue = theScalar->value.floatValue * conversion;
            return true;
        }
        case kSINumberFloat64Type: {
            theScalar->value.doubleValue = theScalar->value.doubleValue * conversion;
            return true;
        }
        case kSINumberComplex64Type: {
            theScalar->value.floatComplexValue = theScalar->value.floatComplexValue * conversion;
            return true;
        }
        case kSINumberComplex128Type: {
            theScalar->value.doubleComplexValue = theScalar->value.doubleComplexValue * conversion;
            return true;
        }
    }
    return false;
}
bool SIScalarConvertToUnitWithString(SIMutableScalarRef theScalar, OCStringRef unitString, OCStringRef *error) {
    if (error)
        if (*error) return false;
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, false);
    double conversion = 1.0;
    SIUnitRef unit = SIUnitFromExpression(unitString, &conversion, error);
    if (!unit) {
        if (error == NULL) return false;
        *error = STR("Invalid unit string.");
        return false;
    }
    if (!SIDimensionalityHasSameReducedDimensionality(SIUnitGetDimensionality(theScalar->unit), SIUnitGetDimensionality(unit))) {
        if (error == NULL) return false;
        *error = STR("Incompatible Dimensionalities.");
        return false;
    }
    conversion *= SIUnitConversion(theScalar->unit, unit);
    theScalar->unit = unit;
    switch (theScalar->type) {
        case kSINumberFloat32Type: {
            theScalar->value.floatValue = theScalar->value.floatValue * conversion;
            return true;
        }
        case kSINumberFloat64Type: {
            theScalar->value.doubleValue = theScalar->value.doubleValue * conversion;
            return true;
        }
        case kSINumberComplex64Type: {
            theScalar->value.floatComplexValue = theScalar->value.floatComplexValue * conversion;
            return true;
        }
        case kSINumberComplex128Type: {
            theScalar->value.doubleComplexValue = theScalar->value.doubleComplexValue * conversion;
            return true;
        }
    }
    return false;
}
SIScalarRef SIScalarCreateByConvertingToUnit(SIScalarRef theScalar, SIUnitRef unit, OCStringRef *error) {
    if (error)
        if (*error) return NULL;
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, NULL);
    SIScalarRef result = SIScalarCreateCopy(theScalar);
    if (SIScalarConvertToUnit((SIMutableScalarRef)result, unit, error)) {
        return result;
    }
    if (result) OCRelease(result);
    return NULL;
}
SIScalarRef SIScalarCreateByConvertingToUnitWithString(SIScalarRef theScalar, OCStringRef unitString, OCStringRef *error) {
    if (error)
        if (*error) return NULL;
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, NULL);
    SIScalarRef result = SIScalarCreateCopy(theScalar);
    if (SIScalarConvertToUnitWithString((SIMutableScalarRef)result, unitString, error)) {
        return result;
    }
    if (result) OCRelease(result);
    return NULL;
}
bool SIScalarConvertToCoherentUnit(SIMutableScalarRef theScalar, OCStringRef *error) {
    if (error)
        if (*error) return false;
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, false);
    SIUnitRef coherentUnit = SIUnitCoherentUnitFromDimensionality(SIUnitGetDimensionality(theScalar->unit));
    return SIScalarConvertToUnit(theScalar, coherentUnit, error);
}
SIScalarRef SIScalarCreateByConvertingToCoherentUnit(SIScalarRef theScalar, OCStringRef *error) {
    if (error)
        if (*error) return NULL;
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, NULL);
    SIScalarRef result = SIScalarCreateCopy(theScalar);
    if (SIScalarConvertToCoherentUnit((SIMutableScalarRef)result, error)) return result;
    if (result) OCRelease(result);
    return NULL;
}
#include <float.h>
#include <math.h>
bool SIScalarBestConversionForQuantity(SIMutableScalarRef theScalar,
                                       OCStringRef quantity,
                                       OCStringRef *outError) {
    if (outError) *outError = NULL;
    if (!quantity) {
        if (outError) *outError = STR("quantity is NULL");
        return false;
    }
    OCArrayRef units = SIUnitCreateArrayOfUnitsForQuantity(quantity);
    if (!units) {
        if (outError) *outError = STR("no units available for quantity");
        return false;
    }
    // If exactly zero, nothing to do
    double v0 = fabs(SIScalarDoubleValueInCoherentUnit(theScalar));
    if (v0 == 0.0) {
        OCRelease(units);
        return true;
    }
    size_t nUnits = OCArrayGetCount(units);
    int bestIdx = -1;
    double bestScore = DBL_MAX;
    // Try each unit
    for (size_t i = 0; i < nUnits; ++i) {
        SIUnitRef u = OCArrayGetValueAtIndex(units, i);
        // work on a fresh copy
        SIMutableScalarRef trial = SIScalarCreateMutableCopy(theScalar);
        if (!trial) {
            OCRelease(units);
            if (outError) *outError = STR("couldn't copy scalar");
            return false;
        }
        if (!SIScalarConvertToUnit(trial, u, outError)) {
            OCRelease(trial);
            OCRelease(units);
            return false;
        }
        double val = SIScalarDoubleValue(trial);
        double av = fabs(val);
        OCRelease(trial);
        // only consider those in [1,1000)
        if (av >= 1.0 && av < 1000.0) {
            double score = fabs(log10(av));
            if (score < bestScore) {
                bestScore = score;
                bestIdx = (int)i;
            }
        }
    }
    // if we found a good “[1,1000)” unit, convert to it
    if (bestIdx >= 0) {
        SIUnitRef bestU = OCArrayGetValueAtIndex(units, bestIdx);
        if (!SIScalarConvertToUnit(theScalar, bestU, outError)) {
            OCRelease(units);
            return false;
        }
    }
    OCRelease(units);
    return true;
}
bool SIScalarAdd(SIMutableScalarRef target, SIScalarRef input2, OCStringRef *error) {
    IF_NO_OBJECT_EXISTS_RETURN(target, false);
    IF_NO_OBJECT_EXISTS_RETURN(input2, false);
    if (error && *error) return false;
    if (!SIDimensionalityHasSameReducedDimensionality(SIUnitGetDimensionality(target->unit),
                                                      SIUnitGetDimensionality(input2->unit))) {
        if (error) {
            *error = STR("Incompatible dimensionalities.");
        }
        return false;
    }
    // Convert input2 to target's unit
    double complex value;
    switch (input2->type) {
        case kSINumberFloat32Type:
            value = SIScalarFloatValueInUnit(input2, target->unit, NULL);
            break;
        case kSINumberFloat64Type:
            value = SIScalarDoubleValueInUnit(input2, target->unit, NULL);
            break;
        case kSINumberComplex64Type:
            value = SIScalarFloatComplexValueInUnit(input2, target->unit, NULL);
            break;
        case kSINumberComplex128Type:
            value = SIScalarDoubleComplexValueInUnit(input2, target->unit, NULL);
            break;
        default:
            return false;
    }
    // Add value to target
    switch (target->type) {
        case kSINumberFloat32Type:
            target->value.floatValue += crealf(value);
            return true;
        case kSINumberFloat64Type:
            target->value.doubleValue += creal(value);
            return true;
        case kSINumberComplex64Type:
            target->value.floatComplexValue += (float complex)value;
            return true;
        case kSINumberComplex128Type:
            target->value.doubleComplexValue += value;
            return true;
        default:
            return false;
    }
}
SIScalarRef SIScalarCreateByAdding(SIScalarRef input1, SIScalarRef input2, OCStringRef *error) {
    if (error)
        if (*error) return NULL;
    IF_NO_OBJECT_EXISTS_RETURN(input1, NULL);
    IF_NO_OBJECT_EXISTS_RETURN(input2, NULL);
    SIScalarRef result = SIScalarCreateByConvertingToNumberType(input1, SIQuantityBestNumericType((SIQuantityRef)input1, (SIQuantityRef)input2));
    if (SIScalarAdd((SIMutableScalarRef)result, input2, error)) return result;
    OCRelease(result);
    return NULL;
}
bool SIScalarSubtract(SIMutableScalarRef target, SIScalarRef input2, OCStringRef *error) {
    IF_NO_OBJECT_EXISTS_RETURN(target, false);
    IF_NO_OBJECT_EXISTS_RETURN(input2, false);
    if (error && *error) return false;
    if (!SIDimensionalityHasSameReducedDimensionality(SIUnitGetDimensionality(target->unit),
                                                      SIUnitGetDimensionality(input2->unit))) {
        if (error) {
            *error = STR("Incompatible dimensionalities.");
        }
        return false;
    }
    // Convert input2 to the target's unit
    double complex value;
    switch (input2->type) {
        case kSINumberFloat32Type:
            value = SIScalarFloatValueInUnit(input2, target->unit, NULL);
            break;
        case kSINumberFloat64Type:
            value = SIScalarDoubleValueInUnit(input2, target->unit, NULL);
            break;
        case kSINumberComplex64Type:
            value = SIScalarFloatComplexValueInUnit(input2, target->unit, NULL);
            break;
        case kSINumberComplex128Type:
            value = SIScalarDoubleComplexValueInUnit(input2, target->unit, NULL);
            break;
        default:
            return false;
    }
    // Subtract value from target
    switch (target->type) {
        case kSINumberFloat32Type:
            target->value.floatValue -= crealf(value);
            return true;
        case kSINumberFloat64Type:
            target->value.doubleValue -= creal(value);
            return true;
        case kSINumberComplex64Type:
            target->value.floatComplexValue -= (float complex)value;
            return true;
        case kSINumberComplex128Type:
            target->value.doubleComplexValue -= value;
            return true;
        default:
            return false;
    }
}
SIScalarRef SIScalarCreateBySubtracting(SIScalarRef input1, SIScalarRef input2, OCStringRef *error) {
    if (error)
        if (*error) return NULL;
    IF_NO_OBJECT_EXISTS_RETURN(input1, NULL);
    IF_NO_OBJECT_EXISTS_RETURN(input2, NULL);
    SIScalarRef result = SIScalarCreateByConvertingToNumberType(input1, SIQuantityBestNumericType((SIQuantityRef)input1, (SIQuantityRef)input2));
    if (SIScalarSubtract((SIMutableScalarRef)result, input2, error)) return result;
    OCRelease(result);
    return NULL;
}
bool SIScalarMultiplyWithoutReducingUnit(SIMutableScalarRef target, SIScalarRef input2, OCStringRef *error) {
    IF_NO_OBJECT_EXISTS_RETURN(target, false);
    IF_NO_OBJECT_EXISTS_RETURN(input2, false);
    if (error && *error) return false;
    double unit_multiplier = 1.0;
    SIUnitRef newUnit = SIUnitByMultiplyingWithoutReducing(target->unit, input2->unit, &unit_multiplier, error);
    if (!newUnit) return false;
    target->unit = newUnit;
    // Extract numeric value from input2
    double complex multiplier;
    switch (input2->type) {
        case kSINumberFloat32Type:
            multiplier = input2->value.floatValue;
            break;
        case kSINumberFloat64Type:
            multiplier = input2->value.doubleValue;
            break;
        case kSINumberComplex64Type:
            multiplier = input2->value.floatComplexValue;
            break;
        case kSINumberComplex128Type:
            multiplier = input2->value.doubleComplexValue;
            break;
        default:
            return false;
    }
    multiplier *= unit_multiplier;

    // Check for infinity cases before normal multiplication
    double complex target_value;
    switch (target->type) {
        case kSINumberFloat32Type:
            target_value = target->value.floatValue;
            break;
        case kSINumberFloat64Type:
            target_value = target->value.doubleValue;
            break;
        case kSINumberComplex64Type:
            target_value = target->value.floatComplexValue;
            break;
        case kSINumberComplex128Type:
            target_value = target->value.doubleComplexValue;
            break;
        default:
            return false;
    }

    // Handle infinity multiplication cases
    bool target_is_inf = isinf(cabs(target_value));
    bool multiplier_is_inf = isinf(cabs(multiplier));
    bool target_is_zero = (cabs(target_value) == 0.0);
    bool multiplier_is_zero = (cabs(multiplier) == 0.0);

    if ((target_is_inf && multiplier_is_zero) || (target_is_zero && multiplier_is_inf)) {
        // 0 × ∞ = ∞ (practical for physics calculations)
        switch (target->type) {
            case kSINumberFloat32Type:
                target->value.floatValue = INFINITY;
                return true;
            case kSINumberFloat64Type:
                target->value.doubleValue = INFINITY;
                return true;
            case kSINumberComplex64Type:
                target->value.floatComplexValue = (float complex)INFINITY;
                return true;
            case kSINumberComplex128Type:
                target->value.doubleComplexValue = INFINITY;
                return true;
            default:
                return false;
        }
    } else if (target_is_inf || multiplier_is_inf) {
        // If either is infinite (and the other is non-zero), result is infinite
        switch (target->type) {
            case kSINumberFloat32Type:
                target->value.floatValue = INFINITY * unit_multiplier;
                return true;
            case kSINumberFloat64Type:
                target->value.doubleValue = INFINITY * unit_multiplier;
                return true;
            case kSINumberComplex64Type:
                target->value.floatComplexValue = (float complex)(INFINITY * unit_multiplier);
                return true;
            case kSINumberComplex128Type:
                target->value.doubleComplexValue = INFINITY * unit_multiplier;
                return true;
            default:
                return false;
        }
    }

    // Multiply into target
    switch (target->type) {
        case kSINumberFloat32Type:
            target->value.floatValue *= crealf(multiplier);
            return true;
        case kSINumberFloat64Type:
            target->value.doubleValue *= creal(multiplier);
            return true;
        case kSINumberComplex64Type:
            target->value.floatComplexValue *= (float complex)multiplier;
            return true;
        case kSINumberComplex128Type:
            target->value.doubleComplexValue *= multiplier;
            return true;
        default:
            return false;
    }
}
SIScalarRef SIScalarCreateByMultiplyingWithoutReducingUnit(SIScalarRef input1, SIScalarRef input2, OCStringRef *error) {
    if (error)
        if (*error) return false;
    IF_NO_OBJECT_EXISTS_RETURN(input1, false);
    IF_NO_OBJECT_EXISTS_RETURN(input2, false);
    SIScalarRef result = SIScalarCreateByConvertingToNumberType(input1, SIQuantityBestNumericType((SIQuantityRef)input1, (SIQuantityRef)input2));
    SIScalarMultiplyWithoutReducingUnit((SIMutableScalarRef)result, input2, error);
    return result;
}
bool SIScalarMultiply(SIMutableScalarRef target, SIScalarRef input2, OCStringRef *error) {
    IF_NO_OBJECT_EXISTS_RETURN(target, false);
    IF_NO_OBJECT_EXISTS_RETURN(input2, false);
    if (error && *error) return false;
    double unit_multiplier = 1.0;
    SIUnitRef newUnit = SIUnitByMultiplying(target->unit, input2->unit, &unit_multiplier, error);
    if (!newUnit) return false;
    target->unit = newUnit;
    // Extract numeric multiplier from input2
    double complex multiplier;
    switch (input2->type) {
        case kSINumberFloat32Type:
            multiplier = input2->value.floatValue;
            break;
        case kSINumberFloat64Type:
            multiplier = input2->value.doubleValue;
            break;
        case kSINumberComplex64Type:
            multiplier = input2->value.floatComplexValue;
            break;
        case kSINumberComplex128Type:
            multiplier = input2->value.doubleComplexValue;
            break;
        default:
            return false;
    }
    multiplier *= unit_multiplier;

    // Check for infinity cases before normal multiplication
    double complex target_value;
    switch (target->type) {
        case kSINumberFloat32Type:
            target_value = target->value.floatValue;
            break;
        case kSINumberFloat64Type:
            target_value = target->value.doubleValue;
            break;
        case kSINumberComplex64Type:
            target_value = target->value.floatComplexValue;
            break;
        case kSINumberComplex128Type:
            target_value = target->value.doubleComplexValue;
            break;
        default:
            return false;
    }

    // Handle infinity multiplication cases
    bool target_is_inf = isinf(cabs(target_value));
    bool multiplier_is_inf = isinf(cabs(multiplier));
    bool target_is_zero = (cabs(target_value) == 0.0);
    bool multiplier_is_zero = (cabs(multiplier) == 0.0);

    if ((target_is_inf && multiplier_is_zero) || (target_is_zero && multiplier_is_inf)) {
        // 0 × ∞ = ∞ (practical for physics calculations)
        switch (target->type) {
            case kSINumberFloat32Type:
                target->value.floatValue = INFINITY;
                return true;
            case kSINumberFloat64Type:
                target->value.doubleValue = INFINITY;
                return true;
            case kSINumberComplex64Type:
                target->value.floatComplexValue = (float complex)INFINITY;
                return true;
            case kSINumberComplex128Type:
                target->value.doubleComplexValue = INFINITY;
                return true;
            default:
                return false;
        }
    } else if (target_is_inf || multiplier_is_inf) {
        // If either is infinite (and the other is non-zero), result is infinite
        switch (target->type) {
            case kSINumberFloat32Type:
                target->value.floatValue = INFINITY * unit_multiplier;
                return true;
            case kSINumberFloat64Type:
                target->value.doubleValue = INFINITY * unit_multiplier;
                return true;
            case kSINumberComplex64Type:
                target->value.floatComplexValue = (float complex)(INFINITY * unit_multiplier);
                return true;
            case kSINumberComplex128Type:
                target->value.doubleComplexValue = INFINITY * unit_multiplier;
                return true;
            default:
                return false;
        }
    }

    // Apply normal multiplication to target
    switch (target->type) {
        case kSINumberFloat32Type:
            target->value.floatValue *= crealf(multiplier);
            return true;
        case kSINumberFloat64Type:
            target->value.doubleValue *= creal(multiplier);
            return true;
        case kSINumberComplex64Type:
            target->value.floatComplexValue *= (float complex)multiplier;
            return true;
        case kSINumberComplex128Type:
            target->value.doubleComplexValue *= multiplier;
            return true;
        default:
            return false;
    }
}
SIScalarRef SIScalarCreateByMultiplying(SIScalarRef input1, SIScalarRef input2, OCStringRef *error) {
    if (error)
        if (*error) return false;
    IF_NO_OBJECT_EXISTS_RETURN(input1, NULL);
    IF_NO_OBJECT_EXISTS_RETURN(input2, NULL);
    SIScalarRef result = SIScalarCreateByConvertingToNumberType(input1, SIQuantityBestNumericType((SIQuantityRef)input1, (SIQuantityRef)input2));
    SIScalarMultiply((SIMutableScalarRef)result, input2, error);
    return result;
}
bool SIScalarDivideWithoutReducingUnit(SIMutableScalarRef target, SIScalarRef input2, OCStringRef *error) {
    IF_NO_OBJECT_EXISTS_RETURN(target, false);
    IF_NO_OBJECT_EXISTS_RETURN(input2, false);
    if (error && *error) return false;
    double unit_multiplier = 1.0;
    SIUnitRef unit = SIUnitByDividingWithoutReducing(target->unit, input2->unit, &unit_multiplier, error);
    if (!unit) return false;
    target->unit = unit;
    // Extract divisor
    double complex divisor;
    switch (input2->type) {
        case kSINumberFloat32Type:
            divisor = input2->value.floatValue;
            break;
        case kSINumberFloat64Type:
            divisor = input2->value.doubleValue;
            break;
        case kSINumberComplex64Type:
            divisor = input2->value.floatComplexValue;
            break;
        case kSINumberComplex128Type:
            divisor = input2->value.doubleComplexValue;
            break;
        default:
            return false;
    }
    // Check division by zero - set to infinity instead of error
    if (cabs(divisor) == 0.0) {
        // Set the target value to infinity based on its type
        switch (target->type) {
            case kSINumberFloat32Type:
                target->value.floatValue = INFINITY * unit_multiplier;
                return true;
            case kSINumberFloat64Type:
                target->value.doubleValue = INFINITY * unit_multiplier;
                return true;
            case kSINumberComplex64Type:
                target->value.floatComplexValue = (float complex)(INFINITY * unit_multiplier);
                return true;
            case kSINumberComplex128Type:
                target->value.doubleComplexValue = INFINITY * unit_multiplier;
                return true;
            default:
                return false;
        }
    }
    // Check division by infinity - set result to zero
    if (isinf(cabs(divisor))) {
        // Set the target value to zero based on its type
        switch (target->type) {
            case kSINumberFloat32Type:
                target->value.floatValue = 0.0f;
                return true;
            case kSINumberFloat64Type:
                target->value.doubleValue = 0.0;
                return true;
            case kSINumberComplex64Type:
                target->value.floatComplexValue = 0.0f + 0.0f * I;
                return true;
            case kSINumberComplex128Type:
                target->value.doubleComplexValue = 0.0 + 0.0 * I;
                return true;
            default:
                return false;
        }
    }
    double complex factor = unit_multiplier / divisor;
    // Apply division
    switch (target->type) {
        case kSINumberFloat32Type:
            target->value.floatValue *= crealf(factor);
            return true;
        case kSINumberFloat64Type:
            target->value.doubleValue *= creal(factor);
            return true;
        case kSINumberComplex64Type:
            target->value.floatComplexValue *= (float complex)factor;
            return true;
        case kSINumberComplex128Type:
            target->value.doubleComplexValue *= factor;
            return true;
        default:
            return false;
    }
}
bool SIScalarDivide(SIMutableScalarRef target, SIScalarRef input2, OCStringRef *error) {
    IF_NO_OBJECT_EXISTS_RETURN(target, false);
    IF_NO_OBJECT_EXISTS_RETURN(input2, false);
    if (error && *error) return false;
    double unit_multiplier = 1.0;
    SIUnitRef unit = SIUnitByDividing(target->unit, input2->unit, &unit_multiplier, error);
    if (!unit) return false;
    target->unit = unit;
    // Get divisor
    double complex divisor;
    switch (input2->type) {
        case kSINumberFloat32Type:
            divisor = input2->value.floatValue;
            break;
        case kSINumberFloat64Type:
            divisor = input2->value.doubleValue;
            break;
        case kSINumberComplex64Type:
            divisor = input2->value.floatComplexValue;
            break;
        case kSINumberComplex128Type:
            divisor = input2->value.doubleComplexValue;
            break;
        default:
            return false;
    }
    // Division by zero check - set to infinity instead of error
    if (cabs(divisor) == 0.0) {
        // Set the target value to infinity based on its type
        switch (target->type) {
            case kSINumberFloat32Type:
                target->value.floatValue = INFINITY * unit_multiplier;
                return true;
            case kSINumberFloat64Type:
                target->value.doubleValue = INFINITY * unit_multiplier;
                return true;
            case kSINumberComplex64Type:
                target->value.floatComplexValue = (float complex)(INFINITY * unit_multiplier);
                return true;
            case kSINumberComplex128Type:
                target->value.doubleComplexValue = INFINITY * unit_multiplier;
                return true;
            default:
                return false;
        }
    }

    // Division by infinity check - set to zero
    if (isinf(cabs(divisor))) {
        // Set the target value to zero based on its type
        switch (target->type) {
            case kSINumberFloat32Type:
                target->value.floatValue = 0.0f;
                return true;
            case kSINumberFloat64Type:
                target->value.doubleValue = 0.0;
                return true;
            case kSINumberComplex64Type:
                target->value.floatComplexValue = 0.0f;
                return true;
            case kSINumberComplex128Type:
                target->value.doubleComplexValue = 0.0;
                return true;
            default:
                return false;
        }
    }

    // Compute scaling factor
    double complex factor = unit_multiplier / divisor;
    // Apply division (multiply by reciprocal)
    switch (target->type) {
        case kSINumberFloat32Type:
            target->value.floatValue *= crealf(factor);
            return true;
        case kSINumberFloat64Type:
            target->value.doubleValue *= creal(factor);
            return true;
        case kSINumberComplex64Type:
            target->value.floatComplexValue *= (float complex)factor;
            return true;
        case kSINumberComplex128Type:
            target->value.doubleComplexValue *= factor;
            return true;
        default:
            return false;
    }
}
SIScalarRef SIScalarCreateByDividingWithoutReducingUnit(SIScalarRef input1, SIScalarRef input2, OCStringRef *error) {
    if (error)
        if (*error) return NULL;
    IF_NO_OBJECT_EXISTS_RETURN(input1, NULL);
    IF_NO_OBJECT_EXISTS_RETURN(input2, NULL);
    SIScalarRef result = SIScalarCreateByConvertingToNumberType(input1, SIQuantityBestNumericType((SIQuantityRef)input1, (SIQuantityRef)input2));
    if (SIScalarDivideWithoutReducingUnit((SIMutableScalarRef)result, input2, error)) return result;
    OCRelease(result);
    return NULL;
}
SIScalarRef SIScalarCreateByDividing(SIScalarRef input1, SIScalarRef input2, OCStringRef *error) {
    if (error)
        if (*error) return NULL;
    IF_NO_OBJECT_EXISTS_RETURN(input1, NULL);
    IF_NO_OBJECT_EXISTS_RETURN(input2, NULL);
    SIScalarRef result = SIScalarCreateByConvertingToNumberType(input1, SIQuantityBestNumericType((SIQuantityRef)input1, (SIQuantityRef)input2));
    if (SIScalarDivide((SIMutableScalarRef)result, input2, error)) return result;
    OCRelease(result);
    return NULL;
}
bool SIScalarRaiseToAPowerWithoutReducingUnit(SIMutableScalarRef theScalar, int power, OCStringRef *error) {
    if (error)
        if (*error) return false;
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, false);
    double power_double = (double)power;
    double unit_multiplier = 1;
    SIUnitRef unit = SIUnitByRaisingToPowerWithoutReducing(theScalar->unit, power, &unit_multiplier, error);
    if (error) {
        if (*error) return false;
    }
    IF_NO_OBJECT_EXISTS_RETURN(unit, false);
    theScalar->unit = unit;

    // Special case: anything to the power of 0 equals 1
    if (power == 0) {
        switch (theScalar->type) {
            case kSINumberFloat32Type:
                theScalar->value.floatValue = 1.0f * unit_multiplier;
                return true;
            case kSINumberFloat64Type:
                theScalar->value.doubleValue = 1.0 * unit_multiplier;
                return true;
            case kSINumberComplex64Type:
                theScalar->value.floatComplexValue = (float complex)(1.0f * unit_multiplier);
                return true;
            case kSINumberComplex128Type:
                theScalar->value.doubleComplexValue = 1.0 * unit_multiplier;
                return true;
        }
    }

    // Check if the scalar value is already infinity
    bool is_infinite = false;
    switch (theScalar->type) {
        case kSINumberFloat32Type:
            is_infinite = isinf(theScalar->value.floatValue);
            break;
        case kSINumberFloat64Type:
            is_infinite = isinf(theScalar->value.doubleValue);
            break;
        case kSINumberComplex64Type:
            is_infinite = isinf(cabs(theScalar->value.floatComplexValue));
            break;
        case kSINumberComplex128Type:
            is_infinite = isinf(cabs(theScalar->value.doubleComplexValue));
            break;
    }

    // If already infinite, keep it infinite
    if (is_infinite) {
        switch (theScalar->type) {
            case kSINumberFloat32Type:
                theScalar->value.floatValue = INFINITY * unit_multiplier;
                return true;
            case kSINumberFloat64Type:
                theScalar->value.doubleValue = INFINITY * unit_multiplier;
                return true;
            case kSINumberComplex64Type:
                theScalar->value.floatComplexValue = (float complex)(INFINITY * unit_multiplier);
                return true;
            case kSINumberComplex128Type:
                theScalar->value.doubleComplexValue = INFINITY * unit_multiplier;
                return true;
        }
    }

    // Normal power operation for finite values
    switch (theScalar->type) {
        case kSINumberFloat32Type:
            theScalar->value.floatValue = pow(theScalar->value.floatValue, power_double) * unit_multiplier;
            return true;
        case kSINumberFloat64Type:
            theScalar->value.doubleValue = pow(theScalar->value.doubleValue, power_double) * unit_multiplier;
            return true;
        case kSINumberComplex64Type:
            theScalar->value.floatComplexValue = cpow(theScalar->value.floatComplexValue, power_double) * unit_multiplier;
            return true;
        case kSINumberComplex128Type:
            theScalar->value.doubleComplexValue = cpow(theScalar->value.doubleComplexValue, power_double) * unit_multiplier;
            return true;
    }
    return false;
}
SIScalarRef SIScalarCreateByRaisingToPowerWithoutReducingUnit(SIScalarRef theScalar, int power, OCStringRef *error) {
    if (error)
        if (*error) return NULL;
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, NULL);
    SIScalarRef result = SIScalarCreateCopy(theScalar);
    if (SIScalarRaiseToAPowerWithoutReducingUnit((SIMutableScalarRef)result, power, error)) return result;
    if (result) OCRelease(result);
    return NULL;
}
bool SIScalarRaiseToAPower(SIMutableScalarRef theScalar, int power, OCStringRef *error) {
    if (error)
        if (*error) return false;
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, false);
    // Rules for multiplication and division:
    //	- returned SIScalar with have whichever elementType is greatest of two method arguments
    //	- returned SIScalar unit will be in coherent SI units
    double power_double = (double)power;
    double unit_multiplier = 1;
    SIUnitRef unit = SIUnitByRaisingToPower(theScalar->unit, power, &unit_multiplier, error);
    if (error) {
        if (*error) return false;
    }
    theScalar->unit = unit;

    // Special case: anything to the power of 0 equals 1
    if (power == 0) {
        switch (theScalar->type) {
            case kSINumberFloat32Type:
                theScalar->value.floatValue = 1.0f * unit_multiplier;
                return true;
            case kSINumberFloat64Type:
                theScalar->value.doubleValue = 1.0 * unit_multiplier;
                return true;
            case kSINumberComplex64Type:
                theScalar->value.floatComplexValue = (float complex)(1.0f * unit_multiplier);
                return true;
            case kSINumberComplex128Type:
                theScalar->value.doubleComplexValue = 1.0 * unit_multiplier;
                return true;
        }
    }

    // Check if the scalar value is already infinity
    bool is_infinite = false;
    switch (theScalar->type) {
        case kSINumberFloat32Type:
            is_infinite = isinf(theScalar->value.floatValue);
            break;
        case kSINumberFloat64Type:
            is_infinite = isinf(theScalar->value.doubleValue);
            break;
        case kSINumberComplex64Type:
            is_infinite = isinf(cabs(theScalar->value.floatComplexValue));
            break;
        case kSINumberComplex128Type:
            is_infinite = isinf(cabs(theScalar->value.doubleComplexValue));
            break;
    }

    // If already infinite, keep it infinite
    if (is_infinite) {
        switch (theScalar->type) {
            case kSINumberFloat32Type:
                theScalar->value.floatValue = INFINITY * unit_multiplier;
                return true;
            case kSINumberFloat64Type:
                theScalar->value.doubleValue = INFINITY * unit_multiplier;
                return true;
            case kSINumberComplex64Type:
                theScalar->value.floatComplexValue = (float complex)(INFINITY * unit_multiplier);
                return true;
            case kSINumberComplex128Type:
                theScalar->value.doubleComplexValue = INFINITY * unit_multiplier;
                return true;
        }
    }

    // Normal power operation for finite values
    switch (theScalar->type) {
        case kSINumberFloat32Type:
            theScalar->value.floatValue = pow(theScalar->value.floatValue, power_double) * unit_multiplier;
            return true;
        case kSINumberFloat64Type:
            theScalar->value.doubleValue = pow(theScalar->value.doubleValue, power_double) * unit_multiplier;
            return true;
        case kSINumberComplex64Type:
            theScalar->value.floatComplexValue = cpow(theScalar->value.floatComplexValue, power_double) * unit_multiplier;
            return true;
        case kSINumberComplex128Type:
            theScalar->value.doubleComplexValue = cpow(theScalar->value.doubleComplexValue, power_double) * unit_multiplier;
            return true;
    }
    return false;
}
SIScalarRef SIScalarCreateByRaisingToPower(SIScalarRef theScalar, int power, OCStringRef *error) {
    if (error)
        if (*error) return NULL;
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, NULL);
    SIScalarRef result = SIScalarCreateCopy(theScalar);
    if (SIScalarRaiseToAPower((SIMutableScalarRef)result, power, error)) return result;
    if (result) OCRelease(result);
    return NULL;
}
bool SIScalarTakeAbsoluteValue(SIMutableScalarRef theScalar, OCStringRef *error) {
    if (error)
        if (*error) return false;
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, false);
    switch (theScalar->type) {
        case kSINumberFloat32Type:
            theScalar->value.floatValue = fabsf(theScalar->value.floatValue);
            return true;
        case kSINumberFloat64Type:
            theScalar->value.doubleValue = fabs(theScalar->value.doubleValue);
            return true;
        case kSINumberComplex64Type:
            theScalar->value.floatComplexValue = cabsf(theScalar->value.floatComplexValue);
            return true;
        case kSINumberComplex128Type:
            theScalar->value.doubleComplexValue = cabs(theScalar->value.doubleComplexValue);
            return true;
    }
    return false;
}
SIScalarRef SIScalarCreateByTakingAbsoluteValue(SIScalarRef theScalar, OCStringRef *error) {
    if (error)
        if (*error) return NULL;
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, NULL);
    SIScalarRef result = SIScalarCreateCopy(theScalar);
    if (SIScalarTakeAbsoluteValue((SIMutableScalarRef)result, error)) return result;
    if (result) OCRelease(result);
    return NULL;
}
SIScalarRef SIScalarCreateByGammaFunctionWithoutReducingUnit(SIScalarRef theScalar, OCStringRef *error) {
    if (error)
        if (*error) return NULL;
    if (SIScalarIsComplex(theScalar)) {
        if (error) {
            *error = STR("Gamma function of complex number not implemented.");
        }
        return NULL;
    }
    if (SIUnitIsDimensionless(theScalar->unit)) {
        SIMutableScalarRef temp = SIScalarCreateMutableCopy(theScalar);
        SIScalarReduceUnit(temp);
        double argument = SIScalarDoubleValue(temp) + 1;
        double result = tgamma(argument);
        OCRelease(temp);
        return SIScalarCreate(SIUnitDimensionlessAndUnderived(), kSINumberFloat64Type, &result);
    } else if (SIScalarIsRealNonNegativeInteger(theScalar)) {
        int64_t integerValue = (int64_t)SIScalarDoubleValue(theScalar);
        double unit_multiplier = 1;
        SIUnitRef newUnit = SIUnitByRaisingToPower(theScalar->unit, integerValue, &unit_multiplier, error);
        double argument = SIScalarDoubleValue(theScalar) + 1;
        double result = tgamma(argument);
        return SIScalarCreate(newUnit, kSINumberFloat64Type, &result);
    }
    return NULL;
}
bool SIScalarMultiplyByDimensionlessRealConstant(SIMutableScalarRef theScalar, double constant) {
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, false);

    // Get current scalar value as complex for infinity checking
    double complex scalar_value;
    switch (theScalar->type) {
        case kSINumberFloat32Type:
            scalar_value = theScalar->value.floatValue;
            break;
        case kSINumberFloat64Type:
            scalar_value = theScalar->value.doubleValue;
            break;
        case kSINumberComplex64Type:
            scalar_value = theScalar->value.floatComplexValue;
            break;
        case kSINumberComplex128Type:
            scalar_value = theScalar->value.doubleComplexValue;
            break;
        default:
            return false;
    }

    // Handle infinity multiplication cases
    bool scalar_is_inf = isinf(cabs(scalar_value));
    bool constant_is_inf = isinf(constant);
    bool scalar_is_zero = (cabs(scalar_value) == 0.0);
    bool constant_is_zero = (constant == 0.0);

    if ((scalar_is_inf && constant_is_zero) || (scalar_is_zero && constant_is_inf)) {
        // 0 × ∞ = ∞ (practical for physics calculations)
        switch (theScalar->type) {
            case kSINumberFloat32Type:
                theScalar->value.floatValue = INFINITY;
                return true;
            case kSINumberFloat64Type:
                theScalar->value.doubleValue = INFINITY;
                return true;
            case kSINumberComplex64Type:
                theScalar->value.floatComplexValue = (float complex)INFINITY;
                return true;
            case kSINumberComplex128Type:
                theScalar->value.doubleComplexValue = INFINITY;
                return true;
        }
    } else if (scalar_is_inf || constant_is_inf) {
        // If either is infinite (and the other is non-zero), result is infinite
        // Preserve the sign by determining the sign of the result
        double result_sign = (scalar_is_inf ? (creal(scalar_value) < 0 ? -1.0 : 1.0) : 1.0) *
                            (constant < 0 ? -1.0 : 1.0);
        double signed_infinity = result_sign * INFINITY;

        switch (theScalar->type) {
            case kSINumberFloat32Type:
                theScalar->value.floatValue = signed_infinity;
                return true;
            case kSINumberFloat64Type:
                theScalar->value.doubleValue = signed_infinity;
                return true;
            case kSINumberComplex64Type:
                theScalar->value.floatComplexValue = (float complex)signed_infinity;
                return true;
            case kSINumberComplex128Type:
                theScalar->value.doubleComplexValue = signed_infinity;
                return true;
        }
    }

    // Normal multiplication for finite values
    switch (theScalar->type) {
        case kSINumberFloat32Type:
            theScalar->value.floatValue *= constant;
            return true;
        case kSINumberFloat64Type:
            theScalar->value.doubleValue *= constant;
            return true;
        case kSINumberComplex64Type:
            theScalar->value.floatComplexValue *= constant;
            return true;
        case kSINumberComplex128Type:
            theScalar->value.doubleComplexValue *= constant;
            return true;
    }
    return false;
}
SIScalarRef SIScalarCreateByMultiplyingByDimensionlessRealConstant(SIScalarRef theScalar, double constant) {
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, NULL);
    SINumberType elementType = kSINumberFloat64Type;
    if (elementType < theScalar->type) elementType = theScalar->type;
    SIScalarRef result = SIScalarCreateByConvertingToNumberType(theScalar, elementType);
    if (SIScalarMultiplyByDimensionlessRealConstant((SIMutableScalarRef)result, constant)) return result;
    if (result) OCRelease(result);
    return NULL;
}
bool SIScalarMultiplyByDimensionlessComplexConstant(SIMutableScalarRef theScalar, double complex constant) {
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, false);

    // Get current scalar value as complex for infinity checking
    double complex scalar_value;
    switch (theScalar->type) {
        case kSINumberFloat32Type:
            scalar_value = theScalar->value.floatValue;
            break;
        case kSINumberFloat64Type:
            scalar_value = theScalar->value.doubleValue;
            break;
        case kSINumberComplex64Type:
            scalar_value = theScalar->value.floatComplexValue;
            break;
        case kSINumberComplex128Type:
            scalar_value = theScalar->value.doubleComplexValue;
            break;
        default:
            return false;
    }

    // Handle infinity multiplication cases
    bool scalar_is_inf = isinf(cabs(scalar_value));
    bool constant_is_inf = isinf(cabs(constant));
    bool scalar_is_zero = (cabs(scalar_value) == 0.0);
    bool constant_is_zero = (cabs(constant) == 0.0);

    if ((scalar_is_inf && constant_is_zero) || (scalar_is_zero && constant_is_inf)) {
        // 0 × ∞ = ∞ (practical for physics calculations)
        switch (theScalar->type) {
            case kSINumberFloat32Type:
                theScalar->value.floatValue = INFINITY;
                return true;
            case kSINumberFloat64Type:
                theScalar->value.doubleValue = INFINITY;
                return true;
            case kSINumberComplex64Type:
                theScalar->value.floatComplexValue = (float complex)INFINITY;
                return true;
            case kSINumberComplex128Type:
                theScalar->value.doubleComplexValue = INFINITY;
                return true;
        }
    } else if (scalar_is_inf || constant_is_inf) {
        // If either is infinite (and the other is non-zero), result is infinite
        switch (theScalar->type) {
            case kSINumberFloat32Type:
                theScalar->value.floatValue = INFINITY;
                return true;
            case kSINumberFloat64Type:
                theScalar->value.doubleValue = INFINITY;
                return true;
            case kSINumberComplex64Type:
                theScalar->value.floatComplexValue = (float complex)INFINITY;
                return true;
            case kSINumberComplex128Type:
                theScalar->value.doubleComplexValue = INFINITY;
                return true;
        }
    }

    // Normal multiplication for finite values
    bool isComplex = (cimag(constant) != 0.0);
    double realPart = creal(constant);
    switch (theScalar->type) {
        case kSINumberFloat32Type:
            if (isComplex) {
                theScalar->type = kSINumberComplex64Type;
                theScalar->value.floatComplexValue = (float complex)(theScalar->value.floatValue * constant);
            } else {
                theScalar->value.floatValue *= (float)realPart;
            }
            return true;
        case kSINumberFloat64Type:
            if (isComplex) {
                theScalar->type = kSINumberComplex128Type;
                theScalar->value.doubleComplexValue = theScalar->value.doubleValue * constant;
            } else {
                theScalar->value.doubleValue *= realPart;
            }
            return true;
        case kSINumberComplex64Type:
            theScalar->value.floatComplexValue *= (float complex)constant;
            return true;
        case kSINumberComplex128Type:
            theScalar->value.doubleComplexValue *= constant;
            return true;
    }
    return false;
}
SIScalarRef SIScalarCreateByMultiplyingByDimensionlessComplexConstant(SIScalarRef theScalar, double complex constant) {
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, NULL);
    SIScalarRef result = SIScalarCreateByConvertingToNumberType(theScalar, kSINumberComplex128Type);
    if (SIScalarMultiplyByDimensionlessComplexConstant((SIMutableScalarRef)result, constant)) return result;
    if (result) OCRelease(result);
    return NULL;
}
bool SIScalarConjugate(SIMutableScalarRef theScalar) {
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, false);
    switch (theScalar->type) {
        case kSINumberFloat32Type:
        case kSINumberFloat64Type:
            return true;
        case kSINumberComplex64Type: {
            theScalar->value.floatComplexValue = creal(theScalar->value.floatComplexValue) - I * cimag(theScalar->value.floatComplexValue);
            return true;
        }
        case kSINumberComplex128Type:
            theScalar->value.doubleComplexValue = creal(theScalar->value.doubleComplexValue) - I * cimag(theScalar->value.doubleComplexValue);
            return true;
    }
    return false;
}
SIScalarRef SIScalarCreateByConjugation(SIScalarRef theScalar) {
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, NULL);
    SIScalarRef result = SIScalarCreateCopy(theScalar);
    if (SIScalarConjugate((SIMutableScalarRef)result)) return result;
    if (result) OCRelease(result);
    return NULL;
}
bool SIScalarTakeNthRoot(SIMutableScalarRef theScalar, uint8_t root, OCStringRef *error) {
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, false);
    if (root == 0) {
        if (error) *error = STR("Cannot take 0th root.");
        return false;
    }
    double multiplier = 1.0;
    SIUnitRef newUnit = SIUnitByTakingNthRoot(theScalar->unit, root, &multiplier, error);
    if (!newUnit || (error && *error)) return false;
    theScalar->unit = newUnit;
    double reciprocal = 1.0 / root;

    // Check if the scalar value is already infinity
    bool is_infinite = false;
    switch (theScalar->type) {
        case kSINumberFloat32Type:
            is_infinite = isinf(theScalar->value.floatValue);
            break;
        case kSINumberFloat64Type:
            is_infinite = isinf(theScalar->value.doubleValue);
            break;
        case kSINumberComplex64Type:
            is_infinite = isinf(cabs(theScalar->value.floatComplexValue));
            break;
        case kSINumberComplex128Type:
            is_infinite = isinf(cabs(theScalar->value.doubleComplexValue));
            break;
    }

    // If already infinite, keep it infinite (nth root of infinity is infinity)
    if (is_infinite) {
        switch (theScalar->type) {
            case kSINumberFloat32Type:
                theScalar->value.floatValue = INFINITY * multiplier;
                return true;
            case kSINumberFloat64Type:
                theScalar->value.doubleValue = INFINITY * multiplier;
                return true;
            case kSINumberComplex64Type:
                theScalar->value.floatComplexValue = (float complex)(INFINITY * multiplier);
                return true;
            case kSINumberComplex128Type:
                theScalar->value.doubleComplexValue = INFINITY * multiplier;
                return true;
        }
    }

    // Normal nth root operation for finite values
    switch (theScalar->type) {
        case kSINumberFloat32Type:
            theScalar->value.floatValue = powf(theScalar->value.floatValue, reciprocal) * multiplier;
            break;
        case kSINumberFloat64Type:
            theScalar->value.doubleValue = pow(theScalar->value.doubleValue, reciprocal) * multiplier;
            break;
        case kSINumberComplex64Type:
            theScalar->value.floatComplexValue = cpowf(theScalar->value.floatComplexValue, reciprocal) * multiplier;
            break;
        case kSINumberComplex128Type:
            theScalar->value.doubleComplexValue = cpow(theScalar->value.doubleComplexValue, reciprocal) * multiplier;
            break;
        default:
            if (error) *error = STR("Unsupported number type.");
            return false;
    }
    return true;
}
SIScalarRef SIScalarCreateByTakingNthRoot(SIScalarRef theScalar, uint8_t root, OCStringRef *error) {
    if (error)
        if (*error) return NULL;
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, NULL);
    SIScalarRef result = SIScalarCreateCopy(theScalar);
    if (SIScalarTakeNthRoot((SIMutableScalarRef)result, root, error)) return result;
    if (result) OCRelease(result);
    return NULL;
}
bool SIScalarTakeLog10(SIMutableScalarRef theScalar, OCStringRef *error) {
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, false);
    if (!SIDimensionalityIsDimensionless(SIQuantityGetUnitDimensionality((SIQuantityRef)theScalar))) {
        if (error) {
            *error = STR("Log10 requires dimensionless unit.");
        }
    }
    switch (theScalar->type) {
        case kSINumberFloat32Type: {
            theScalar->value.floatValue = log10f(theScalar->value.floatValue);
            break;
        }
        case kSINumberFloat64Type: {
            theScalar->value.doubleValue = log10(theScalar->value.doubleValue);
            break;
        }
        case kSINumberComplex64Type: {
            theScalar->value.floatComplexValue = clogf(theScalar->value.floatComplexValue) / logf(10);
            break;
        }
        case kSINumberComplex128Type: {
            theScalar->value.doubleComplexValue = clog(theScalar->value.doubleComplexValue) / log(10);
            break;
        }
    }
    return true;
}
bool SIScalarZeroPart(SIMutableScalarRef theScalar, complexPart part) {
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, false);
    switch (theScalar->type) {
        case kSINumberFloat32Type:
            if (part == kSIRealPart || part == kSIMagnitudePart) {
                theScalar->value.floatValue = 0;
                return true;
            }
            if (part == kSIImaginaryPart || part == kSIArgumentPart) return true;
            break;
        case kSINumberFloat64Type:
            if (part == kSIRealPart || part == kSIMagnitudePart) {
                theScalar->value.doubleValue = 0;
                return true;
            }
            if (part == kSIImaginaryPart || part == kSIArgumentPart) return true;
            break;
        case kSINumberComplex64Type: {
            if (part == kSIMagnitudePart) {
                theScalar->value.floatComplexValue = 0;
                return true;
            }
            if (part == kSIRealPart) {
                theScalar->value.floatComplexValue = cimag(theScalar->value.floatComplexValue);
                return true;
            }
            if (part == kSIImaginaryPart) {
                theScalar->value.floatComplexValue = creal(theScalar->value.floatComplexValue);
                return true;
            }
            if (part == kSIArgumentPart) {
                theScalar->value.floatComplexValue = cabs(theScalar->value.floatComplexValue);
                return true;
            }
            break;
        }
        case kSINumberComplex128Type: {
            if (part == kSIMagnitudePart) {
                theScalar->value.doubleComplexValue = 0;
                return true;
            }
            if (part == kSIRealPart) {
                theScalar->value.doubleComplexValue = cimag(theScalar->value.doubleComplexValue);
                return true;
            }
            if (part == kSIImaginaryPart) {
                theScalar->value.doubleComplexValue = creal(theScalar->value.doubleComplexValue);
                return true;
            }
            if (part == kSIArgumentPart) {
                theScalar->value.doubleComplexValue = cabs(theScalar->value.doubleComplexValue);
                return true;
            }
            break;
        }
    }
    return false;
}
SIScalarRef SIScalarCreateByZeroingPart(SIScalarRef theScalar, complexPart part) {
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, NULL);
    SIScalarRef result = SIScalarCreateCopy(theScalar);
    if (SIScalarZeroPart((SIMutableScalarRef)result, part)) return result;
    OCRelease(result);
    return NULL;
}
static OCStringRef SIScalarCreateStringValueSplitByUnits(SIScalarRef theScalar, OCArrayRef units, bool doubleCheck, OCStringRef *error) {
    if (error)
        if (*error) return NULL;
    if (!SIScalarIsReal(theScalar)) return NULL;
    bool isPositive = false;
    if (SIScalarDoubleValue(theScalar) > 0.0) isPositive = true;
    OCMutableStringRef stringValue = OCStringCreateMutable(0);
    SIMutableScalarRef scalar = SIScalarCreateMutableCopy(theScalar);
    uint64_t count = OCArrayGetCount(units);
    uint64_t finalCount = 0;
    for (uint64_t index = 0; index < count; index++) {
        SIUnitRef unit = OCArrayGetValueAtIndex(units, index);
        OCStringRef symbol = SIUnitCopySymbol(unit);
        SIScalarConvertToUnit(scalar, unit, error);
        double value = SIScalarDoubleValue(scalar);
        if (index < count - 1) {
            if (isPositive) {
                value *= 100.;
                value = round(value);
                value /= 100.;
                value = OCDoubleFloor(value);
            } else {
                value *= 100.;
                value = round(value);
                value /= 100.;
                value = OCDoubleCeil(value);
            }
        } else {
            value *= 100.;
            value = round(value);
            value /= 100.;
        }
        if ((value > 0.0 && isPositive) || (value < 0.0 && !isPositive)) {
            OCStringRef valueString = OCDoubleComplexCreateStringValue(value, NULL);
            finalCount++;
            if (OCStringGetLength(stringValue) > 0) {
                if (isPositive)
                    OCStringAppend(stringValue, STR(" + "));
                else
                    OCStringAppend(stringValue, STR(" "));
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
    if (OCStringGetLength(stringValue) == 0 || finalCount == 1) {
        if (stringValue) OCRelease(stringValue);
        if (scalar) OCRelease(scalar);
        return NULL;
    }
    if (scalar) OCRelease(scalar);
    if (doubleCheck) {
        SIScalarRef check = SIScalarCreateFromExpression(stringValue, error);
        if (check) {
            if (SIScalarCompare(theScalar, check) == kOCCompareEqualTo) {
                OCRelease(check);
                return stringValue;
            }
        }
        OCRelease(stringValue);
        return NULL;
    }
    return stringValue;
}
static OCComparisonResult compareOnlyTheStrings(const void *val1, const void *val2, void *context) {
    OCTypeID type1 = OCGetTypeID(val1);
    OCTypeID type2 = OCGetTypeID(val2);
    OCTypeID stringType = OCStringGetTypeID();
    if (type1 == type2) {
        if (type1 == stringType) {
            return OCStringCompare((OCStringRef)val1, (OCStringRef)val2, (OCStringCompareFlags)context);
        }
    }
    return kOCCompareLessThan;
}
OCArrayRef SIScalarCreateArrayOfConversionQuantitiesScalarsAndStringValues(SIScalarRef theScalar, OCStringRef quantity, OCStringRef *error) {
    if (error)
        if (*error) return NULL;
    OCMutableArrayRef result = OCArrayCreateMutable(0, &kOCTypeArrayCallBacks);
    SIUnitRef fahrenheit = SIUnitWithSymbol(STR("°F"));
    SIUnitRef celsius = SIUnitWithSymbol(STR("°C"));
    if (SIDimensionalityHasSameReducedDimensionality(SIDimensionalityForQuantity(kSIQuantityTemperature, NULL), SIQuantityGetUnitDimensionality((SIQuantityRef)theScalar))) {
        SIUnitRef theUnit = SIQuantityGetUnit((SIQuantityRef)theScalar);
        if (theUnit == fahrenheit) {
            OCArrayAppendValue(result, theScalar);
            double complex value = (SIScalarDoubleComplexValue(theScalar) - 32) * 5. / 9.;
            SIScalarRef scalar = SIScalarCreate(celsius, kSINumberComplex128Type, &value);
            OCArrayAppendValue(result, scalar);
            OCRelease(scalar);
            return result;
        }
        if (theUnit == celsius) {
            OCArrayAppendValue(result, theScalar);
            double complex value = SIScalarDoubleValue(theScalar) * 9. / 5. + 32;
            SIScalarRef scalar = SIScalarCreate(fahrenheit, kSINumberComplex128Type, &value);
            OCArrayAppendValue(result, scalar);
            OCRelease(scalar);
            return result;
        }
    }
    OCArrayRef units = NULL;
    OCArrayRef quantities = NULL;
    if (theScalar) {
        if (NULL == quantity) {
            quantities = SIDimensionalityCreateArrayOfQuantitiesWithSameReducedDimensionality(SIQuantityGetUnitDimensionality((SIQuantityRef)theScalar));
            if (quantities) {
                OCArrayAppendArray(result, quantities, OCRangeMake(0, OCArrayGetCount(quantities)));
                OCRelease(quantities);
            }
        }
        if (NULL == quantity)
            units = SIUnitCreateArrayOfConversionUnits(SIQuantityGetUnit((SIQuantityRef)theScalar));
        else
            units = SIUnitCreateArrayOfUnitsForQuantity(quantity);
        if (units) {
            for (uint64_t index = 0; index < OCArrayGetCount(units); index++) {
                SIUnitRef unit = (SIUnitRef)OCArrayGetValueAtIndex(units, index);
                if (unit && fahrenheit != unit && celsius != unit) {
                    SIScalarRef newScalar = SIScalarCreateByConvertingToUnit(theScalar, unit, error);
                    if (error && *error) {
                        if (newScalar) OCRelease(newScalar);
                        if (units) OCRelease(units);
                        return NULL;
                    }
                    if (newScalar) {
                        if (SIUnitIsCoherentUnit(unit)) {
                            OCArrayAppendValue(result, newScalar);
                        } else {
                            int magnitude = log10(fabs(SIScalarDoubleValue(newScalar)));
                            if (abs(magnitude) < 6) {
                                OCArrayAppendValue(result, newScalar);
                            }
                        }
                        OCRelease(newScalar);
                    }
                }
            }
            OCRelease(units);
        }
        if (SIDimensionalityHasSameReducedDimensionality(SIDimensionalityForQuantity(kSIQuantityTime, NULL), SIQuantityGetUnitDimensionality((SIQuantityRef)theScalar)) && SIScalarIsReal(theScalar)) {
            SIUnitRef unit1 = SIUnitWithSymbol(STR("yr"));
            SIUnitRef unit2 = SIUnitWithSymbol(STR("mo"));
            SIUnitRef unit3 = SIUnitWithSymbol(STR("wk"));
            SIUnitRef unit4 = SIUnitWithSymbol(STR("d"));
            SIUnitRef unit5 = SIUnitWithSymbol(STR("h"));
            SIUnitRef unit6 = SIUnitWithSymbol(STR("min"));
            SIUnitRef unit7 = SIUnitWithSymbol(STR("s"));
            SIUnitRef theUnits[7] = {unit1, unit2, unit3, unit4, unit5, unit6, unit7};
            OCArrayRef units = OCArrayCreate((const void **)theUnits, 7, &kOCTypeArrayCallBacks);
            OCStringRef stringValue = SIScalarCreateStringValueSplitByUnits(theScalar, units, true, error);
            OCRelease(units);
            if (stringValue) {
                OCArrayAppendValue(result, stringValue);
                OCRelease(stringValue);
            }
        }
        if (SIDimensionalityHasSameReducedDimensionality(SIDimensionalityForQuantity(kSIQuantityVolume, NULL), SIQuantityGetUnitDimensionality((SIQuantityRef)theScalar)) && SIScalarIsReal(theScalar)) {
            SIUnitRef unit1 = SIUnitWithSymbol(STR("gal"));
            SIUnitRef unit2 = SIUnitWithSymbol(STR("qt"));
            SIUnitRef unit3 = SIUnitWithSymbol(STR("pt"));
            SIUnitRef unit4 = SIUnitWithSymbol(STR("cup"));
            SIUnitRef unit5 = SIUnitWithSymbol(STR("floz"));
            SIUnitRef unit6 = SIUnitWithSymbol(STR("tbsp"));
            SIUnitRef unit7 = SIUnitWithSymbol(STR("tsp"));
            SIUnitRef unit8 = SIUnitWithSymbol(STR("halftsp"));
            SIUnitRef unit9 = SIUnitWithSymbol(STR("quartertsp"));
            SIUnitRef theUnits[9] = {unit1, unit2, unit3, unit4, unit5, unit6, unit7, unit8, unit9};
            OCArrayRef units = OCArrayCreate((const void **)theUnits, 9, &kOCTypeArrayCallBacks);
            OCStringRef stringValue = SIScalarCreateStringValueSplitByUnits(theScalar, units, false, error);
            OCRelease(units);
            if (stringValue) {
                OCArrayAppendValue(result, stringValue);
                OCRelease(stringValue);
            }
        }
        if (SIDimensionalityHasSameReducedDimensionality(SIDimensionalityForQuantity(kSIQuantityLength, NULL), SIQuantityGetUnitDimensionality((SIQuantityRef)theScalar)) && SIScalarIsReal(theScalar)) {
            SIUnitRef unit1 = SIUnitWithSymbol(STR("mi"));
            SIUnitRef unit2 = SIUnitWithSymbol(STR("ft"));
            SIUnitRef unit3 = SIUnitWithSymbol(STR("in"));
            SIUnitRef theUnits[3] = {unit1, unit2, unit3};
            OCArrayRef units = OCArrayCreate((const void **)theUnits, 3, &kOCTypeArrayCallBacks);
            OCStringRef stringValue = SIScalarCreateStringValueSplitByUnits(theScalar, units, false, error);
            OCRelease(units);
            if (stringValue) {
                OCArrayAppendValue(result, stringValue);
                OCRelease(stringValue);
            }
        }
        if (SIDimensionalityHasSameReducedDimensionality(SIDimensionalityForQuantity(kSIQuantityLength, NULL), SIQuantityGetUnitDimensionality((SIQuantityRef)theScalar)) && SIScalarIsReal(theScalar)) {
            SIUnitRef unit1 = SIUnitWithSymbol(STR("mi"));
            SIUnitRef unit2 = SIUnitWithSymbol(STR("yd"));
            SIUnitRef unit3 = SIUnitWithSymbol(STR("ft"));
            SIUnitRef unit4 = SIUnitWithSymbol(STR("in"));
            SIUnitRef theUnits[4] = {unit1, unit2, unit3, unit4};
            OCArrayRef units = OCArrayCreate((const void **)theUnits, 4, &kOCTypeArrayCallBacks);
            OCStringRef stringValue = SIScalarCreateStringValueSplitByUnits(theScalar, units, false, error);
            OCRelease(units);
            if (stringValue) {
                if (OCArrayBSearchValues(result, OCRangeMake(0, OCArrayGetCount(result)), stringValue,
                                         (OCComparatorFunction)compareOnlyTheStrings, NULL) >= OCArrayGetCount(result)) {
                    OCArrayAppendValue(result, stringValue);
                    OCRelease(stringValue);
                }
            }
        }
        if (SIDimensionalityHasSameReducedDimensionality(SIDimensionalityForQuantity(kSIQuantityMass, NULL), SIQuantityGetUnitDimensionality((SIQuantityRef)theScalar)) && SIScalarIsReal(theScalar)) {
            SIUnitRef unit1 = SIUnitWithSymbol(STR("ton"));
            SIUnitRef unit2 = SIUnitWithSymbol(STR("lb"));
            SIUnitRef unit3 = SIUnitWithSymbol(STR("oz"));
            SIUnitRef theUnits[3] = {unit1, unit2, unit3};
            OCArrayRef units = OCArrayCreate((const void **)theUnits, 3, &kOCTypeArrayCallBacks);
            OCStringRef stringValue = SIScalarCreateStringValueSplitByUnits(theScalar, units, false, error);
            OCRelease(units);
            if (stringValue) {
                OCArrayAppendValue(result, stringValue);
                OCRelease(stringValue);
            }
        }
        if (SIDimensionalityHasSameReducedDimensionality(SIDimensionalityForQuantity(kSIQuantityMass, NULL), SIQuantityGetUnitDimensionality((SIQuantityRef)theScalar)) && SIScalarIsReal(theScalar)) {
            SIUnitRef unit1 = SIUnitWithSymbol(STR("ton"));
            SIUnitRef unit2 = SIUnitWithSymbol(STR("st"));
            SIUnitRef unit3 = SIUnitWithSymbol(STR("lb"));
            SIUnitRef unit4 = SIUnitWithSymbol(STR("oz"));
            SIUnitRef theUnits[4] = {unit1, unit2, unit3, unit4};
            OCArrayRef units = OCArrayCreate((const void **)theUnits, 4, &kOCTypeArrayCallBacks);
            OCStringRef stringValue = SIScalarCreateStringValueSplitByUnits(theScalar, units, false, error);
            OCRelease(units);
            if (stringValue) {
                if (OCArrayBSearchValues(result, OCRangeMake(0, OCArrayGetCount(result)), stringValue,
                                         (OCComparatorFunction)compareOnlyTheStrings, NULL) >= OCArrayGetCount(result)) {
                    OCArrayAppendValue(result, stringValue);
                    OCRelease(stringValue);
                }
            }
        }
    }
    return NULL;
}
OCArrayRef SIScalarCreateArrayOfConversionQuantitiesAndUnits(SIScalarRef theScalar, OCStringRef quantity, OCStringRef *error) {
    if (error)
        if (*error) return NULL;
    OCArrayRef units = NULL;
    OCArrayRef quantities = NULL;
    OCMutableArrayRef result = OCArrayCreateMutable(0, &kOCTypeArrayCallBacks);
    if (theScalar) {
        if (NULL == quantity) {
            quantities = SIDimensionalityCreateArrayOfQuantitiesWithSameReducedDimensionality(SIQuantityGetUnitDimensionality((SIQuantityRef)theScalar));
            if (quantities) {
                OCArrayAppendArray(result, quantities, OCRangeMake(0, OCArrayGetCount(quantities)));
                OCRelease(quantities);
            }
        }
        if (NULL == quantity)
            units = SIUnitCreateArrayOfConversionUnits(SIQuantityGetUnit((SIQuantityRef)theScalar));
        else
            units = SIUnitCreateArrayOfUnitsForQuantity(quantity);
        if (units) {
            for (uint64_t index = 0; index < OCArrayGetCount(units); index++) {
                SIUnitRef unit = (SIUnitRef)OCArrayGetValueAtIndex(units, index);
                if (unit) {
                    SIScalarRef newScalar = SIScalarCreateByConvertingToUnit(theScalar, unit, error);
                    if (error) {
                        if (*error) {
                            if (newScalar) OCRelease(newScalar);
                            if (units) OCRelease(units);
                            return NULL;
                        }
                    }
                    if (newScalar) {
                        if (SIUnitIsCoherentUnit(unit))
                            OCArrayAppendValue(result, unit);
                        else {
                            int magnitude = log10(fabs(SIScalarDoubleValue(newScalar)));
                            if (abs(magnitude) < 6) OCArrayAppendValue(result, unit);
                        }
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
void SIScalarShow(SIScalarRef theScalar) {
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, );
    OCStringRef cf_string = SIScalarCreateStringValue(theScalar);
    if (cf_string) {
        OCStringShow(cf_string);
        OCStringShow(STR("\n"));
        OCRelease(cf_string);
    } else
        fprintf(stdout, "invalid value.");
}
bool SIScalarValidateProposedStringValue(SIScalarRef theScalar, OCStringRef proposedStringValue, OCStringRef *error) {
    if (error)
        if (*error) return false;
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, false);
    IF_NO_OBJECT_EXISTS_RETURN(proposedStringValue, false);
    SIScalarRef proposedValue = SIScalarCreateFromExpression(proposedStringValue, error);
    if (error)
        if (*error) OCRelease(*error);
    if (proposedValue == NULL) {
        if (error) {
            SIDimensionalityRef dimensionality = SIQuantityGetUnitDimensionality((SIQuantityRef)theScalar);
            OCStringRef dimensionalitySymbol = SIDimensionalityCopySymbol(dimensionality);
            *error = OCStringCreateWithFormat(STR("Unrecognized input. Value must have dimensionality: %@"), dimensionalitySymbol);
            OCRelease(dimensionalitySymbol);
        }
        return false;
    } else if (!SIQuantityHasSameReducedDimensionality((SIQuantityRef)proposedValue, (SIQuantityRef)theScalar)) {
        if (error) {
            SIDimensionalityRef dimensionality = SIQuantityGetUnitDimensionality((SIQuantityRef)theScalar);
            OCStringRef dimensionalitySymbol = SIDimensionalityCopySymbol(dimensionality);
            *error = OCStringCreateWithFormat(STR("Unrecognized input. Value must have dimensionality: %@"), dimensionalitySymbol);
            OCRelease(dimensionalitySymbol);
        }
        OCRelease(proposedValue);
        return false;
    }
    OCRelease(proposedValue);
    return true;
}
OCStringRef SIScalarCreateNumericStringValue(SIScalarRef theScalar) {
    if (theScalar == NULL) return STR("");
    OCStringRef stringValue = NULL;
    switch (theScalar->type) {
        case kSINumberFloat32Type:
        case kSINumberComplex64Type:
            stringValue = SIScalarCreateNumericStringValueWithFormat(theScalar, STR("%.7g"));
            break;
        case kSINumberFloat64Type:
        case kSINumberComplex128Type:
            //            stringValue = SIScalarCreateNumericStringValueWithFormat(theScalar, STR("%.16lg"));
            stringValue = SIScalarCreateNumericStringValueWithFormat(theScalar, STR("%.14lg"));
            break;
    }
    return stringValue;
}
OCStringRef SIScalarCreateStringValue(SIScalarRef theScalar) {
    OCStringRef stringValue = NULL;
    if (theScalar == NULL) return stringValue;
    if (OCGetTypeID(theScalar) == OCNumberGetTypeID()) {
        return OCNumberCreateStringValue((OCNumberRef)((OCTypeRef)theScalar));
    }
    switch (theScalar->type) {
        case kSINumberFloat32Type:
        case kSINumberComplex64Type:
            stringValue = SIScalarCreateStringValueWithFormat(theScalar, STR("%.7g"));
            break;
        case kSINumberFloat64Type:
        case kSINumberComplex128Type:
            stringValue = SIScalarCreateStringValueWithFormat(theScalar, STR("%.16lg"));
            break;
    }
    return stringValue;
}
OCStringRef SIScalarCreateStringValueForPart(SIScalarRef theScalar, complexPart thePart) {
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, NULL);
    SIScalarRef temp = SIScalarCreateByTakingComplexPart(theScalar, thePart);
    OCStringRef string = SIScalarCreateStringValue(temp);
    OCRelease(temp);
    return string;
}
OCStringRef SIScalarCreateNumericStringValueWithFormat(SIScalarRef theScalar, OCStringRef format) {
    if (theScalar == NULL) return STR("");
    switch (theScalar->type) {
        case kSINumberFloat32Type: {
            float value = SIScalarFloatValue(theScalar);
            if (OCCompareFloatValues(value, 0.0) == kOCCompareEqualTo) value = 0.0;
            OCStringRef numericString = OCFloatCreateStringValue(value);
            return numericString;
        }
        case kSINumberFloat64Type: {
            double value = SIScalarDoubleValue(theScalar);
            if (OCCompareDoubleValues(value, 0.0) == kOCCompareEqualTo) value = 0.0;
            OCStringRef numericString = OCDoubleComplexCreateStringValue(value, format);
            return numericString;
        }
        case kSINumberComplex64Type: {
            float complex value = SIScalarFloatComplexValue(theScalar);
            OCMutableStringRef cf_string = OCStringCreateMutable(0);
            if (crealf(value) != 0.0 && cimagf(value) != 0.0) OCStringAppend(cf_string, STR("("));
            OCStringRef numericString = OCFloatComplexCreateStringValue(value, format);
            OCStringAppend(cf_string, numericString);
            OCRelease(numericString);
            if (crealf(value) != 0.0 && cimagf(value) != 0.0) OCStringAppend(cf_string, STR(")"));
            return cf_string;
        }
        case kSINumberComplex128Type: {
            double complex value = SIScalarDoubleComplexValue(theScalar);
            OCMutableStringRef cf_string = OCStringCreateMutable(0);
            if (creal(value) != 0.0 && cimag(value) != 0.0) OCStringAppend(cf_string, STR("("));
            OCStringRef numericString = OCDoubleComplexCreateStringValue(value, format);
            OCStringAppend(cf_string, numericString);
            OCRelease(numericString);
            if (creal(value) != 0.0 && cimag(value) != 0.0) OCStringAppend(cf_string, STR(")"));
            return cf_string;
        }
    }
    return NULL;
}
OCStringRef SIScalarCopyUnitSymbol(SIScalarRef theScalar) {
    return SIUnitCopySymbol(theScalar->unit);
}
OCStringRef SIScalarCreateStringValueWithFormat(SIScalarRef theScalar, OCStringRef format) {
    if (theScalar == NULL) return STR("");

    // Check if the scalar is infinite and return infinity symbol with unit
    if (SIScalarIsInfinite(theScalar)) {
        OCStringRef unit_symbol = SIScalarCopyUnitSymbol(theScalar);
        OCMutableStringRef cf_string = OCStringCreateMutable(0);
        OCStringAppend(cf_string, STR("∞"));
        if (theScalar->unit) {
            OCStringAppend(cf_string, STR(" "));
            OCStringAppend(cf_string, unit_symbol);
        }
        OCStringTrimWhitespace(cf_string);
        OCRelease(unit_symbol);
        return cf_string;
    }

    OCStringRef unit_symbol = SIScalarCopyUnitSymbol(theScalar);
    switch (theScalar->type) {
        case kSINumberFloat32Type: {
            float value = SIScalarFloatValue(theScalar);
            if (OCCompareFloatValues(value, 0.0) == kOCCompareEqualTo) value = 0.0;
            OCStringRef numericString = OCFloatCreateStringValue(value);
            OCMutableStringRef cf_string = OCStringCreateMutableCopy(numericString);
            OCRelease(numericString);
            if (theScalar->unit) {
                OCStringAppend(cf_string, STR(" "));
                OCStringAppend(cf_string, unit_symbol);
            }
            OCStringTrimWhitespace(cf_string);
            OCRelease(unit_symbol);
            return cf_string;
        }
        case kSINumberFloat64Type: {
            double value = SIScalarDoubleValue(theScalar);
            if (OCCompareDoubleValues(value, 0.0) == kOCCompareEqualTo) value = 0.0;
            OCStringRef numericString = OCDoubleCreateStringValue(value);
            OCMutableStringRef cf_string = OCStringCreateMutableCopy(numericString);
            OCRelease(numericString);
            if (theScalar->unit) {
                OCStringAppend(cf_string, STR(" "));
                OCStringAppend(cf_string, unit_symbol);
            }
            OCStringTrimWhitespace(cf_string);
            OCRelease(unit_symbol);
            return cf_string;
        }
        case kSINumberComplex64Type: {
            float complex value = SIScalarFloatComplexValue(theScalar);
            OCMutableStringRef cf_string = OCStringCreateMutable(0);
            if (crealf(value) != 0.0 && cimagf(value) != 0.0) OCStringAppend(cf_string, STR("("));
            OCStringRef numericString = OCFloatComplexCreateStringValue(value, format);
            OCStringAppend(cf_string, numericString);
            OCRelease(numericString);
            if (crealf(value) != 0.0 && cimagf(value) != 0.0) OCStringAppend(cf_string, STR(")"));
            if (theScalar->unit) {
                OCStringAppend(cf_string, STR(" "));
                OCStringAppend(cf_string, unit_symbol);
            }
            OCStringTrimWhitespace(cf_string);
            OCRelease(unit_symbol);
            return cf_string;
        }
        case kSINumberComplex128Type: {
            double complex value = SIScalarDoubleComplexValue(theScalar);
            OCMutableStringRef cf_string = OCStringCreateMutable(0);
            if (creal(value) != 0.0 && cimag(value) != 0.0) OCStringAppend(cf_string, STR("("));
            OCStringRef numericString = OCDoubleComplexCreateStringValue(value, format);
            OCStringAppend(cf_string, numericString);
            OCRelease(numericString);
            if (creal(value) != 0.0 && cimag(value) != 0.0) OCStringAppend(cf_string, STR(")"));
            if (theScalar->unit) {
                OCStringAppend(cf_string, STR(" "));
                OCStringAppend(cf_string, unit_symbol);
            }
            OCStringTrimWhitespace(cf_string);
            OCRelease(unit_symbol);
            return cf_string;
        }
    }
    return NULL;
}
void SIScalarAddToArrayAsStringValue(SIScalarRef theScalar, OCMutableArrayRef array) {
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, );
    IF_NO_OBJECT_EXISTS_RETURN(array, );
    OCStringRef stringValue = SIScalarCreateStringValue(theScalar);
    OCArrayAppendValue(array, stringValue);
    OCRelease(stringValue);
}
#pragma mark Tests
bool SIScalarIsReal(SIScalarRef theScalar) {
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, false);
    double complex value = SIScalarDoubleComplexValue(theScalar);
    if (cimag(value) == 0.0) return true;
    return false;
}
bool SIScalarIsImaginary(SIScalarRef theScalar) {
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, false);
    double complex value = SIScalarDoubleComplexValue(theScalar);
    if (creal(value) == 0.0 && cabs(value) != 0.0) return true;
    return false;
}
bool SIScalarIsComplex(SIScalarRef theScalar) {
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, false);
    double complex value = SIScalarDoubleComplexValue(theScalar);
    if (cimag(value) == 0.0) return false;
    return true;
}
bool SIScalarIsZero(SIScalarRef theScalar) {
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, false);
    double complex value = SIScalarDoubleComplexValue(theScalar);
    if (cimag(value) != 0.0) return false;
    if (creal(value) != 0.0) return false;
    return true;
}
bool SIScalarIsInfinite(SIScalarRef theScalar) {
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, false);
    switch (SIQuantityGetNumericType((SIQuantityRef)theScalar)) {
        case kSINumberFloat32Type:
            if (isinf(theScalar->value.floatValue)) return true;
        case kSINumberFloat64Type:
            if (isinf(theScalar->value.doubleValue)) return true;
        case kSINumberComplex64Type:
            if (isinf(crealf(theScalar->value.floatComplexValue))) return true;
            if (isinf(cimagf(theScalar->value.floatComplexValue))) return true;
        case kSINumberComplex128Type:
            if (isinf(creal(theScalar->value.doubleComplexValue))) return true;
            if (isinf(cimag(theScalar->value.doubleComplexValue))) return true;
    }
    return false;
}
bool SIScalarIsRealNonNegativeInteger(SIScalarRef theScalar) {
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, false);
    if (SIScalarIsComplex(theScalar)) return false;
    double value = SIScalarDoubleValue(theScalar);
    double integerPart;
    double fractionalPart = modf(value, &integerPart);
    if (fractionalPart != 0.0) return false;
    if (integerPart >= 0) return true;
    return false;
}
bool SIScalarEqual(SIScalarRef input1, SIScalarRef input2) {
    IF_NO_OBJECT_EXISTS_RETURN(input1, false);
    IF_NO_OBJECT_EXISTS_RETURN(input2, false);
    if (input1 == input2) return true;
    if (input1->type != input2->type) return false;
    if (!SIUnitEqual(input1->unit, input2->unit)) return false;
    switch (input1->type) {
        case kSINumberFloat32Type: {
            if (input1->value.floatValue != input2->value.floatValue) return false;
            break;
        }
        case kSINumberFloat64Type: {
            if (input1->value.doubleValue != input2->value.doubleValue) return false;
            break;
        }
        case kSINumberComplex64Type: {
            if (input1->value.floatComplexValue != input2->value.floatComplexValue) return false;
            break;
        }
        case kSINumberComplex128Type: {
            if (input1->value.doubleComplexValue != input2->value.doubleComplexValue) return false;
            break;
        }
    }
    return true;
}
OCComparisonResult SIScalarCompare(SIScalarRef theScalar, SIScalarRef theOtherScalar) {
    if (NULL == theScalar) {
        IF_NO_OBJECT_EXISTS_RETURN(theScalar, kOCCompareError);
    }
    if (NULL == theOtherScalar) {
        IF_NO_OBJECT_EXISTS_RETURN(theOtherScalar, kOCCompareError);
    }
    if (!SIDimensionalityHasSameReducedDimensionality(SIQuantityGetUnitDimensionality((SIQuantityRef)theScalar),
                                                      SIQuantityGetUnitDimensionality((SIQuantityRef)theOtherScalar))) return kOCCompareUnequalDimensionalities;
    SIMutableScalarRef theOtherConverted = SIScalarCreateMutableCopy(theOtherScalar);
    if (NULL == theOtherConverted) {
        IF_NO_OBJECT_EXISTS_RETURN(theOtherConverted, kOCCompareError);
    }
    SIScalarConvertToUnit(theOtherConverted, SIQuantityGetUnit((SIQuantityRef)theScalar), NULL);
    OCComparisonResult result = kOCCompareError;
    switch (theScalar->type) {
        case kSINumberFloat32Type: {
            switch (theOtherConverted->type) {
                case kSINumberFloat32Type: {
                    result = OCCompareFloatValues((float)theScalar->value.floatValue, (float)theOtherConverted->value.floatValue);
                    break;
                }
                case kSINumberFloat64Type: {
                    result = OCCompareFloatValues((float)theScalar->value.floatValue, (float)theOtherConverted->value.doubleValue);
                    break;
                }
                case kSINumberComplex64Type: {
                    if (!SIScalarIsReal(theOtherConverted)) {
                        result = kOCCompareNoSingleValue;
                        break;
                    }
                    result = OCCompareFloatValues((float)theScalar->value.floatValue, (float)crealf(theOtherConverted->value.floatComplexValue));
                    break;
                }
                case kSINumberComplex128Type: {
                    if (!SIScalarIsReal(theOtherConverted)) {
                        result = kOCCompareNoSingleValue;
                        break;
                    }
                    result = OCCompareFloatValues((float)theScalar->value.floatValue, (float)creal(theOtherConverted->value.doubleComplexValue));
                    break;
                }
            }
            break;
        }
        case kSINumberFloat64Type: {
            switch (theOtherConverted->type) {
                case kSINumberFloat32Type: {
                    result = OCCompareFloatValues((float)theScalar->value.doubleValue, (float)theOtherConverted->value.floatValue);
                    break;
                }
                case kSINumberFloat64Type: {
                    result = OCCompareDoubleValues((double)theScalar->value.doubleValue, (double)theOtherConverted->value.doubleValue);
                    break;
                }
                case kSINumberComplex64Type: {
                    if (!SIScalarIsReal(theOtherConverted)) {
                        result = kOCCompareNoSingleValue;
                        break;
                    }
                    result = OCCompareFloatValues((float)theScalar->value.doubleValue, (float)crealf(theOtherConverted->value.floatComplexValue));
                    break;
                }
                case kSINumberComplex128Type: {
                    if (!SIScalarIsReal(theOtherConverted)) {
                        result = kOCCompareNoSingleValue;
                        break;
                    }
                    result = OCCompareDoubleValues((double)theScalar->value.doubleValue, creal(theOtherConverted->value.doubleComplexValue));
                    break;
                }
            }
            break;
        }
        case kSINumberComplex64Type: {
            switch (theOtherConverted->type) {
                case kSINumberFloat32Type: {
                    if (!SIScalarIsReal(theScalar)) {
                        result = kOCCompareNoSingleValue;
                        break;
                    }
                    result = OCCompareFloatValues((float)creal(theScalar->value.floatComplexValue), (float)theOtherConverted->value.floatValue);
                    break;
                }
                case kSINumberFloat64Type: {
                    if (!SIScalarIsReal(theScalar)) {
                        result = kOCCompareNoSingleValue;
                        break;
                    }
                    result = OCCompareFloatValues((float)crealf(theScalar->value.floatComplexValue), (float)theOtherConverted->value.doubleValue);
                    break;
                }
                case kSINumberComplex64Type: {
                    OCComparisonResult realResult = OCCompareFloatValues((float)crealf(theScalar->value.floatComplexValue), (float)crealf(theOtherConverted->value.floatComplexValue));
                    OCComparisonResult imagResult = OCCompareFloatValues((float)cimagf(theScalar->value.floatComplexValue), (float)cimagf(theOtherConverted->value.floatComplexValue));
                    if (realResult == kOCCompareEqualTo && imagResult == kOCCompareEqualTo)
                        result = kOCCompareEqualTo;
                    else
                        result = kOCCompareNoSingleValue;
                    break;
                }
                case kSINumberComplex128Type: {
                    OCComparisonResult realResult = OCCompareFloatValues((float)crealf(theScalar->value.floatComplexValue), (float)creal(theOtherConverted->value.doubleComplexValue));
                    OCComparisonResult imagResult = OCCompareFloatValues((float)cimagf(theScalar->value.floatComplexValue), (float)cimag(theOtherConverted->value.doubleComplexValue));
                    if (realResult == kOCCompareEqualTo && imagResult == kOCCompareEqualTo)
                        result = kOCCompareEqualTo;
                    else
                        result = kOCCompareNoSingleValue;
                    break;
                }
            }
            break;
        }
        case kSINumberComplex128Type: {
            switch (theOtherConverted->type) {
                case kSINumberFloat32Type: {
                    if (!SIScalarIsReal(theScalar)) {
                        result = kOCCompareNoSingleValue;
                        break;
                    }
                    result = OCCompareFloatValues((float)creal(theScalar->value.doubleComplexValue), (float)theOtherConverted->value.floatValue);
                    break;
                }
                case kSINumberFloat64Type: {
                    if (!SIScalarIsReal(theScalar)) {
                        result = kOCCompareNoSingleValue;
                        break;
                    }
                    result = OCCompareDoubleValues((double)creal(theScalar->value.doubleComplexValue), (double)theOtherConverted->value.doubleValue);
                    break;
                }
                case kSINumberComplex64Type: {
                    OCComparisonResult realResult = OCCompareFloatValues((float)creal(theScalar->value.doubleComplexValue), (float)crealf(theOtherConverted->value.floatComplexValue));
                    OCComparisonResult imagResult = OCCompareFloatValues((float)cimag(theScalar->value.doubleComplexValue), (float)cimagf(theOtherConverted->value.floatComplexValue));
                    if (realResult == kOCCompareEqualTo && imagResult == kOCCompareEqualTo)
                        result = kOCCompareEqualTo;
                    else
                        result = kOCCompareNoSingleValue;
                    break;
                }
                case kSINumberComplex128Type: {
                    OCComparisonResult realResult = OCCompareDoubleValues((double)creal(theScalar->value.doubleComplexValue), (double)creal(theOtherConverted->value.doubleComplexValue));
                    OCComparisonResult imagResult = OCCompareDoubleValues((double)cimag(theScalar->value.doubleComplexValue), (double)cimag(theOtherConverted->value.doubleComplexValue));
                    if (realResult == kOCCompareEqualTo && imagResult == kOCCompareEqualTo)
                        result = kOCCompareEqualTo;
                    else
                        result = kOCCompareNoSingleValue;
                    break;
                }
            }
            break;
        }
    }
    OCRelease(theOtherConverted);
    return result;
}
OCComparisonResult SIScalarCompareReduced(SIScalarRef theScalar, SIScalarRef theOtherScalar) {
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, kOCCompareError);
    IF_NO_OBJECT_EXISTS_RETURN(theOtherScalar, kOCCompareError);
    SIScalarRef theScalarReduced = SIScalarCreateByReducingUnit(theScalar);
    SIScalarRef theOtherScalarReduced = SIScalarCreateByReducingUnit(theOtherScalar);
    OCComparisonResult result = SIScalarCompare(theScalarReduced, theOtherScalarReduced);
    OCRelease(theScalarReduced);
    OCRelease(theOtherScalarReduced);
    return result;
}
OCComparisonResult SIScalarCompareLoose(SIScalarRef theScalar, SIScalarRef theOtherScalar) {
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, kOCCompareError);
    IF_NO_OBJECT_EXISTS_RETURN(theOtherScalar, kOCCompareError);
    if (!SIDimensionalityEqual(SIQuantityGetUnitDimensionality((SIQuantityRef)theScalar),
                               SIQuantityGetUnitDimensionality((SIQuantityRef)theOtherScalar))) return kOCCompareUnequalDimensionalities;
    SIMutableScalarRef theOtherConverted = SIScalarCreateMutableCopy(theOtherScalar);
    SIScalarConvertToUnit(theOtherConverted, SIQuantityGetUnit((SIQuantityRef)theScalar), NULL);
    OCComparisonResult result = kOCCompareError;
    switch (theScalar->type) {
        case kSINumberFloat32Type: {
            switch (theOtherConverted->type) {
                case kSINumberFloat32Type: {
                    result = OCCompareFloatValuesLoose((float)theScalar->value.floatValue, (float)theOtherConverted->value.floatValue);
                    break;
                }
                case kSINumberFloat64Type: {
                    result = OCCompareFloatValuesLoose((float)theScalar->value.floatValue, (float)theOtherConverted->value.doubleValue);
                    break;
                }
                case kSINumberComplex64Type: {
                    if (!SIScalarIsReal(theOtherConverted)) {
                        result = kOCCompareNoSingleValue;
                        break;
                    }
                    result = OCCompareFloatValuesLoose((float)theScalar->value.floatValue, (float)crealf(theOtherConverted->value.floatComplexValue));
                    break;
                }
                case kSINumberComplex128Type: {
                    if (!SIScalarIsReal(theOtherConverted)) {
                        result = kOCCompareNoSingleValue;
                        break;
                    }
                    result = OCCompareFloatValuesLoose((float)theScalar->value.floatValue, (float)creal(theOtherConverted->value.doubleComplexValue));
                    break;
                }
            }
            break;
        }
        case kSINumberFloat64Type: {
            switch (theOtherConverted->type) {
                case kSINumberFloat32Type: {
                    result = OCCompareFloatValuesLoose((float)theScalar->value.doubleValue, (float)theOtherConverted->value.floatValue);
                    break;
                }
                case kSINumberFloat64Type: {
                    result = OCCompareDoubleValuesLoose((double)theScalar->value.doubleValue, (double)theOtherConverted->value.doubleValue);
                    break;
                }
                case kSINumberComplex64Type: {
                    if (!SIScalarIsReal(theOtherConverted)) {
                        result = kOCCompareNoSingleValue;
                        break;
                    }
                    result = OCCompareFloatValuesLoose((float)theScalar->value.doubleValue, (float)crealf(theOtherConverted->value.floatComplexValue));
                    break;
                }
                case kSINumberComplex128Type: {
                    if (!SIScalarIsReal(theOtherConverted)) {
                        result = kOCCompareNoSingleValue;
                        break;
                    }
                    result = OCCompareDoubleValuesLoose((double)theScalar->value.doubleValue, creal(theOtherConverted->value.doubleComplexValue));
                    break;
                }
            }
            break;
        }
        case kSINumberComplex64Type: {
            switch (theOtherConverted->type) {
                case kSINumberFloat32Type: {
                    if (!SIScalarIsReal(theScalar)) {
                        result = kOCCompareNoSingleValue;
                        break;
                    }
                    result = OCCompareFloatValuesLoose((float)creal(theScalar->value.floatComplexValue), (float)theOtherConverted->value.floatValue);
                    break;
                }
                case kSINumberFloat64Type: {
                    if (!SIScalarIsReal(theScalar)) {
                        result = kOCCompareNoSingleValue;
                        break;
                    }
                    result = OCCompareFloatValuesLoose((float)crealf(theScalar->value.floatComplexValue), (float)theOtherConverted->value.doubleValue);
                    break;
                }
                case kSINumberComplex64Type: {
                    OCComparisonResult realResult = OCCompareFloatValuesLoose((float)crealf(theScalar->value.floatComplexValue), (float)crealf(theOtherConverted->value.floatComplexValue));
                    OCComparisonResult imagResult = OCCompareFloatValuesLoose((float)cimagf(theScalar->value.floatComplexValue), (float)cimagf(theOtherConverted->value.floatComplexValue));
                    if (realResult == kOCCompareEqualTo && imagResult == kOCCompareEqualTo)
                        result = kOCCompareEqualTo;
                    else
                        result = kOCCompareNoSingleValue;
                    break;
                }
                case kSINumberComplex128Type: {
                    OCComparisonResult realResult = OCCompareFloatValuesLoose((float)crealf(theScalar->value.floatComplexValue), (float)creal(theOtherConverted->value.doubleComplexValue));
                    OCComparisonResult imagResult = OCCompareFloatValuesLoose((float)cimagf(theScalar->value.floatComplexValue), (float)cimag(theOtherConverted->value.doubleComplexValue));
                    if (realResult == kOCCompareEqualTo && imagResult == kOCCompareEqualTo)
                        result = kOCCompareEqualTo;
                    else
                        result = kOCCompareNoSingleValue;
                    break;
                }
            }
            break;
        }
        case kSINumberComplex128Type: {
            switch (theOtherConverted->type) {
                case kSINumberFloat32Type: {
                    if (!SIScalarIsReal(theScalar)) {
                        result = kOCCompareNoSingleValue;
                        break;
                    }
                    result = OCCompareFloatValuesLoose((float)creal(theScalar->value.doubleComplexValue), (float)theOtherConverted->value.floatValue);
                    break;
                }
                case kSINumberFloat64Type: {
                    if (!SIScalarIsReal(theScalar)) {
                        result = kOCCompareNoSingleValue;
                        break;
                    }
                    result = OCCompareDoubleValuesLoose((double)creal(theScalar->value.doubleComplexValue), (double)theOtherConverted->value.doubleValue);
                    break;
                }
                case kSINumberComplex64Type: {
                    OCComparisonResult realResult = OCCompareFloatValuesLoose((float)creal(theScalar->value.doubleComplexValue), (float)crealf(theOtherConverted->value.floatComplexValue));
                    OCComparisonResult imagResult = OCCompareFloatValuesLoose((float)cimag(theScalar->value.doubleComplexValue), (float)cimag(theOtherConverted->value.floatComplexValue));
                    if (realResult == kOCCompareEqualTo && imagResult == kOCCompareEqualTo)
                        result = kOCCompareEqualTo;
                    else
                        result = kOCCompareNoSingleValue;
                    break;
                }
                case kSINumberComplex128Type: {
                    OCComparisonResult realResult = OCCompareDoubleValuesLoose((double)creal(theScalar->value.doubleComplexValue), (double)creal(theOtherConverted->value.doubleComplexValue));
                    OCComparisonResult imagResult = OCCompareDoubleValuesLoose((double)cimag(theScalar->value.doubleComplexValue), (double)cimag(theOtherConverted->value.doubleComplexValue));
                    if (realResult == kOCCompareEqualTo && imagResult == kOCCompareEqualTo)
                        result = kOCCompareEqualTo;
                    else
                        result = kOCCompareNoSingleValue;
                    break;
                }
            }
            break;
        }
    }
    OCRelease(theOtherConverted);
    return result;
}
OCComparisonResult SIScalarCompareLooseReduced(SIScalarRef theScalar, SIScalarRef theOtherScalar) {
    IF_NO_OBJECT_EXISTS_RETURN(theScalar, kOCCompareError);
    IF_NO_OBJECT_EXISTS_RETURN(theOtherScalar, kOCCompareError);
    SIScalarRef theScalarReduced = SIScalarCreateByReducingUnit(theScalar);
    SIScalarRef theOtherScalarReduced = SIScalarCreateByReducingUnit(theOtherScalar);
    OCComparisonResult result = SIScalarCompareLoose(theScalarReduced, theOtherScalarReduced);
    OCRelease(theScalarReduced);
    OCRelease(theOtherScalarReduced);
    return result;
}
