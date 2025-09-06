//
//  SIDimensionality.c
//  SITypes
//
//  Created by philip on 6/7/17.
//  Copyright © 2017 PhySy Ltd. All rights reserved.
//
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "SIDimensionalityPrivate.h"
#include "SITypes.h"
static OCTypeID kSIDimensionalityID = kOCNotATypeID;
OCTypeID SIDimensionalityGetTypeID(void) {
    if (kSIDimensionalityID == kOCNotATypeID)
        kSIDimensionalityID = OCRegisterType("SIDimensionality", (OCTypeRef (*)(cJSON *, OCStringRef *))SIDimensionalityFromJSON);
    return kSIDimensionalityID;
}
static bool impl_SIDimensionalityEqual(const void *theType1, const void *theType2) {
    if (theType1 == theType2) return true;
    if (theType1 == NULL || theType2 == NULL) return false;
    SIDimensionalityRef dim1 = (SIDimensionalityRef)theType1;
    SIDimensionalityRef dim2 = (SIDimensionalityRef)theType2;
    if (dim1->base.typeID != dim2->base.typeID)
        return false;
    for (int i = 0; i < BASE_DIMENSION_COUNT; ++i) {
        if (dim1->num_exp[i] != dim2->num_exp[i]) return false;
        if (dim1->den_exp[i] != dim2->den_exp[i]) return false;
    }
    return true;
}
static void impl_SIDimensionalityFinalize(const void *theType) {
    if (theType == NULL)
        return;
    SIDimensionalityRef theDim = (SIDimensionalityRef)theType;
    if (theDim->symbol)
        OCRelease(theDim->symbol);
}
static OCStringRef impl_SIDimensionalityCopyFormattingDescription(OCTypeRef cf) {
    if (cf == NULL) return NULL;
    SIDimensionalityRef theDim = (SIDimensionalityRef)cf;
    if (theDim->symbol != NULL) {
        return OCStringCreateCopy(theDim->symbol);
    }
    // Fallback: return a generic placeholder
    return OCStringCreateWithCString("<SIDimensionality>");
}
static void *
impl_SIDimensionalityDeepCopy(const void *obj) {
    if (obj == NULL) return NULL;
    // Raw-access to the exponent arrays
    const struct impl_SIDimensionality *src =
        (const struct impl_SIDimensionality *)obj;
    // Intern (or create) the SIDimensionality with the same numerator/denominator exponents
    SIDimensionalityRef copy = SIDimensionalityWithExponentArrays(src->num_exp, src->den_exp);
    // copy may be NULL if exponents were invalid
    return (void *)copy;
}
// mutable‐copy is the same for this “interned” type
static void *
impl_SIDimensionalityDeepCopyMutable(const void *obj) {
    return impl_SIDimensionalityDeepCopy(obj);
}
static cJSON *impl_SIDimensionalityCopyJSON(const void *obj, bool typed, OCStringRef *outError) {
    return SIDimensionalityCopyAsJSON((SIDimensionalityRef)obj, typed, outError);
}
static struct impl_SIDimensionality *SIDimensionalityAllocate() {
    struct impl_SIDimensionality *obj = OCTypeAlloc(
        struct impl_SIDimensionality,
        SIDimensionalityGetTypeID(),
        impl_SIDimensionalityFinalize,
        impl_SIDimensionalityEqual,
        impl_SIDimensionalityCopyFormattingDescription,
        impl_SIDimensionalityCopyJSON,
        impl_SIDimensionalityDeepCopy,
        impl_SIDimensionalityDeepCopyMutable
    );
    if (!obj) {
        fprintf(stderr, "SIDimensionalityAllocate: OCTypeAlloc failed.\n");
        return NULL;
    }
    // Initialize type-specific fields
    obj->symbol = NULL;
    memset(obj->num_exp, 0, sizeof(obj->num_exp));
    memset(obj->den_exp, 0, sizeof(obj->den_exp));
    return obj;
}
OCDictionaryRef SIDimensionalityCopyDictionary(SIDimensionalityRef dim) {
    if (!dim) return NULL;
    // 1) Create the mutable dictionary
    OCMutableDictionaryRef dict = OCDictionaryCreateMutable(0);
    // 2) Symbol
    OCStringRef symCopy = OCStringCreateCopy(dim->symbol);
    OCDictionaryAddValue(dict, STR("symbol"), symCopy);
    OCRelease(symCopy);
    // 3) Numerators & denominators as OCIndexArray
    OCMutableIndexArrayRef numArr = OCIndexArrayCreateMutable(0);
    OCMutableIndexArrayRef denArr = OCIndexArrayCreateMutable(0);
    for (uint8_t i = 0; i < BASE_DIMENSION_COUNT; i++) {
        OCIndexArrayAppendValue(numArr, dim->num_exp[i]);
        OCIndexArrayAppendValue(denArr, dim->den_exp[i]);
    }
    OCDictionaryAddValue(dict, STR("numerators"), numArr);
    OCDictionaryAddValue(dict, STR("denominators"), denArr);
    // 4) Clean up
    OCRelease(numArr);
    OCRelease(denArr);
    return dict;
}
cJSON *SIDimensionalityCopyAsJSON(SIDimensionalityRef dim, bool typed, OCStringRef *outError) {
    if (outError) *outError = NULL;

    if (!dim) {
        if (outError) *outError = STR("SIDimensionality input is NULL");
        return cJSON_CreateNull();
    }

    // Get symbol once for both paths
    OCStringRef symbol = SIDimensionalityCopySymbol(dim);
    if (!symbol) {
        if (outError) *outError = STR("Failed to get symbol from SIDimensionality");
        return cJSON_CreateNull();
    }

    const char *s = OCStringGetCString(symbol);
    const char *symbolStr = s ? s : "";

    // Create the appropriate JSON value
    cJSON *result;
    if (typed) {
        result = cJSON_CreateObject();
        if (result) {
            cJSON_AddStringToObject(result, "type", "SIDimensionality");
            cJSON_AddStringToObject(result, "value", symbolStr);
        } else {
            if (outError) *outError = STR("Failed to create JSON object");
        }
    } else {
        result = cJSON_CreateString(symbolStr);
        if (!result && outError) {
            *outError = STR("Failed to create JSON string");
        }
    }

    OCRelease(symbol);
    return result ? result : cJSON_CreateNull();
}

SIDimensionalityRef SIDimensionalityFromJSON(cJSON *json, OCStringRef *outError) {
    if (!json) {
        if (outError) *outError = STR("JSON input is NULL");
        return NULL;
    }

    // Handle typed format
    if (cJSON_IsObject(json)) {
        cJSON *type = cJSON_GetObjectItem(json, "type");
        cJSON *value = cJSON_GetObjectItem(json, "value");

        if (!cJSON_IsString(type) || !cJSON_IsString(value)) {
            if (outError) *outError = STR("JSON object missing valid 'type' or 'value' string fields");
            return NULL;
        }

        const char *typeName = cJSON_GetStringValue(type);
        if (!typeName || strcmp(typeName, "SIDimensionality") != 0) {
            if (outError) *outError = STR("JSON type field is not 'SIDimensionality'");
            return NULL;
        }

        const char *symbol = cJSON_GetStringValue(value);
        if (!symbol) {
            if (outError) *outError = STR("JSON value field is NULL");
            return NULL;
        }

        OCStringRef str = OCStringCreateWithCString(symbol);
        if (!str) {
            if (outError) *outError = STR("Failed to create OCString from symbol");
            return NULL;
        }

        SIDimensionalityRef dim = SIDimensionalityFromExpression(str, outError);
        OCRelease(str);

        return dim;
    }
    // Handle untyped format
    else if (cJSON_IsString(json)) {
        const char *symbol = json->valuestring;
        if (!symbol) {
            if (outError) *outError = STR("JSON string value is NULL");
            return NULL;
        }

        OCStringRef str = OCStringCreateWithCString(symbol);
        if (!str) {
            if (outError) *outError = STR("Failed to create OCString from symbol");
            return NULL;
        }

        SIDimensionalityRef dim = SIDimensionalityFromExpression(str, outError);
        OCRelease(str);

        return dim;
    }

    if (outError) *outError = STR("JSON input is neither an object nor a string");
    return NULL;
}

#pragma mark Static Utility Functions
static OCStringRef baseDimensionSymbol(SIBaseDimensionIndex index) {
    switch (index) {
        case kSIMassIndex:
            return STR("M");
        case kSILengthIndex:
            return STR("L");
        case kSITimeIndex:
            return STR("T");
        case kSICurrentIndex:
            return STR("I");
        case kSITemperatureIndex:
            return STR("ϴ");
        case kSIAmountIndex:
            return STR("N");
        case kSILuminousIntensityIndex:
            return STR("J");
        default:
            break;
    }
    return NULL;
}
static uint8_t getExpAtIndex(SIDimensionalityRef dimensionality, SIBaseDimensionIndex index, bool isNumerator) {
    if (dimensionality == NULL)
        return 0;
    return isNumerator ? dimensionality->num_exp[index] : dimensionality->den_exp[index];
}
uint8_t SIDimensionalityGetNumExpAtIndex(SIDimensionalityRef theDim, SIBaseDimensionIndex index) {
    return getExpAtIndex(theDim, index, true);
}
uint8_t SIDimensionalityGetDenExpAtIndex(SIDimensionalityRef theDim, SIBaseDimensionIndex index) {
    return getExpAtIndex(theDim, index, false);
}
static void appendDimensionSymbol(OCMutableStringRef str, uint8_t exponent, OCStringRef symbol) {
    if (exponent > 0) {
        if (OCStringGetLength(str) > 0) {
            OCStringAppendFormat(str, STR("•"));
        }
        if (exponent != 1) {
            OCStringAppendFormat(str, STR("%@^%d"), symbol, exponent);
        } else {
            OCStringAppendFormat(str, STR("%@"), symbol);
        }
    }
}
static OCStringRef
SIDimensionalityCreateSymbol(SIDimensionalityRef theDim) {
    if (!theDim) return NULL;
    // 1) Build numerator and denominator fragments
    OCMutableStringRef num = OCStringCreateMutable(0);
    OCMutableStringRef den = OCStringCreateMutable(0);
    bool multiDen = false;
    for (uint8_t i = 0; i < BASE_DIMENSION_COUNT; i++) {
        OCStringRef base = baseDimensionSymbol(i);
        uint8_t n = theDim->num_exp[i], d = theDim->den_exp[i];
        if (n) appendDimensionSymbol(num, n, base);
        if (d) {
            if (OCStringGetLength(den)) multiDen = true;
            appendDimensionSymbol(den, d, base);
        }
    }
    // 2) Build final result in one mutable buffer
    OCMutableStringRef result = OCStringCreateMutable(0);
    size_t lenN = OCStringGetLength(num), lenD = OCStringGetLength(den);
    if (lenN && lenD) {
        // numerator/denominator form
        OCStringAppendCString(result, OCStringGetCString(num));
        OCStringAppendCString(result, "/");
        if (multiDen) OCStringAppendCString(result, "(");
        OCStringAppendCString(result, OCStringGetCString(den));
        if (multiDen) OCStringAppendCString(result, ")");
    } else if (lenN) {
        // pure numerator
        OCStringAppendCString(result, OCStringGetCString(num));
    } else if (lenD) {
        // pure denominator form
        OCStringAppendCString(result, "(1/");
        if (multiDen) OCStringAppendCString(result, "(");
        OCStringAppendCString(result, OCStringGetCString(den));
        if (multiDen) OCStringAppendCString(result, ")");
        OCStringAppendCString(result, ")");
    } else {
        // dimensionless
        OCStringAppendCString(result, "1");
    }
    // 3) Clean up temporaries
    OCRelease(num);
    OCRelease(den);
    // 4) Return an immutable copy (or transfer ownership if your API allows)
    OCStringRef sym = OCStringCreateCopy(result);
    OCRelease(result);
    return sym;
}
#pragma mark Designated Creator
SIDimensionalityRef SIDimensionalityCreate(const uint8_t *num_exp, const uint8_t *den_exp) {
    // Initialize object
    struct impl_SIDimensionality *theDim = SIDimensionalityAllocate();
    if (NULL == theDim)
        return NULL;
    //  setup attributes
    for (uint8_t i = 0; i < BASE_DIMENSION_COUNT; i++) {
        theDim->num_exp[i] = num_exp[i];
        theDim->den_exp[i] = den_exp[i];
    }
    theDim->symbol = SIDimensionalityCreateSymbol(theDim);
    return (SIDimensionalityRef)theDim;
}
#pragma mark Accessors
OCStringRef SIDimensionalityCopySymbol(SIDimensionalityRef theDim) {
    IF_NO_OBJECT_EXISTS_RETURN(theDim, NULL);
    return OCStringCreateCopy(theDim->symbol);
}
int8_t SIDimensionalityReducedExponentAtIndex(SIDimensionalityRef theDim, SIBaseDimensionIndex index) {
    IF_NO_OBJECT_EXISTS_RETURN(theDim, 0);
    if (theDim)
        return theDim->num_exp[index] - theDim->den_exp[index];
    return 0;
}
SIDimensionalityRef SIDimensionalityWithBaseDimensionSymbol(OCStringRef theString, OCStringRef *error) {
    if (error)
        if (*error) return NULL;
    if (OCStringGetLength(theString) > 1) {
        if (error) {
            *error = STR("Invalid Dimensionality symbol.");
        }
        return NULL;
    }
    SIBaseDimensionIndex index;
    if (OCStringCompare(theString, STR("M"), 0) == kOCCompareEqualTo)
        index = kSIMassIndex;
    else if (OCStringCompare(theString, STR("L"), 0) == kOCCompareEqualTo)
        index = kSILengthIndex;
    else if (OCStringCompare(theString, STR("T"), 0) == kOCCompareEqualTo)
        index = kSITimeIndex;
    else if (OCStringCompare(theString, STR("I"), 0) == kOCCompareEqualTo)
        index = kSICurrentIndex;
    else if (OCStringCompare(theString, STR("ϴ"), 0) == kOCCompareEqualTo)
        index = kSITemperatureIndex;
    else if (OCStringCompare(theString, STR("@"), 0) == kOCCompareEqualTo)
        index = kSITemperatureIndex;
    else if (OCStringCompare(theString, STR("N"), 0) == kOCCompareEqualTo)
        index = kSIAmountIndex;
    else if (OCStringCompare(theString, STR("J"), 0) == kOCCompareEqualTo)
        index = kSILuminousIntensityIndex;
    else
        return NULL;
    return SIDimensionalityForBaseDimensionIndex(index);
}
#pragma mark Strings and Archiving
void SIDimensionalityShow(SIDimensionalityRef theDim) {
    IF_NO_OBJECT_EXISTS_RETURN(theDim, );
    OCStringShow((theDim)->symbol);
    fprintf(stdout, "\n");
}
void SIDimensionalityShowFull(SIDimensionalityRef theDim) {
    IF_NO_OBJECT_EXISTS_RETURN(theDim, );
    OCStringShow(STR("============================================================================================================="));
    OCStringRef cf_string;
    OCStringShow(STR("                                            m  kg   s   A   K  mol cd"));
    OCStringShow(STR("-------------------------------------------------------------------------------------------------------------"));
    cf_string = OCStringCreateWithFormat(STR("SI base dimension numerator exponents:    %3d %3d %3d %3d %3d %3d %3d"),
                                         theDim->num_exp[kSIMassIndex],
                                         theDim->num_exp[kSILengthIndex],
                                         theDim->num_exp[kSITimeIndex],
                                         theDim->num_exp[kSICurrentIndex],
                                         theDim->num_exp[kSITemperatureIndex],
                                         theDim->num_exp[kSIAmountIndex],
                                         theDim->num_exp[kSILuminousIntensityIndex]);
    OCStringShow(cf_string);
    OCRelease(cf_string);
    cf_string = OCStringCreateWithFormat(STR("SI base dimension denominator exponents:  %3d %3d %3d %3d %3d %3d %3d"),
                                         theDim->den_exp[kSIMassIndex],
                                         theDim->den_exp[kSILengthIndex],
                                         theDim->den_exp[kSITimeIndex],
                                         theDim->den_exp[kSICurrentIndex],
                                         theDim->den_exp[kSITemperatureIndex],
                                         theDim->den_exp[kSIAmountIndex],
                                         theDim->den_exp[kSILuminousIntensityIndex]);
    OCStringShow(cf_string);
    OCRelease(cf_string);
    OCStringShow(STR("-------------------------------------------------------------------------------------------------------------"));
    OCStringShow(theDim->symbol);
    OCArrayRef quantities = SIDimensionalityCreateArrayOfQuantities(theDim);
    if (quantities) {
        for (uint64_t index = 0; index < OCArrayGetCount(quantities); index++) {
            OCStringRef quantity = OCArrayGetValueAtIndex(quantities, index);
            OCStringShow(quantity);
        }
        OCRelease(quantities);
    }
    OCStringShow(STR("\n============================================================================================================="));
    fprintf(stderr, "\n\n");
}
#pragma mark Tests
bool SIDimensionalityEqual(SIDimensionalityRef input1, SIDimensionalityRef input2) {
    IF_NO_OBJECT_EXISTS_RETURN(input1, false);
    IF_NO_OBJECT_EXISTS_RETURN(input2, false);
    if (NULL == input1)
        return false;
    if (NULL == input2)
        return false;
    if (input1 == input2)
        return true;
    for (int i = 0; i < BASE_DIMENSION_COUNT; i++) {
        if (input1->num_exp[i] != input2->num_exp[i])
            return false;
        if (input1->den_exp[i] != input2->den_exp[i])
            return false;
    }
    return true;
}
bool SIDimensionalityIsDimensionless(SIDimensionalityRef theDim) {
    IF_NO_OBJECT_EXISTS_RETURN(theDim, false);
    for (SIBaseDimensionIndex index = 0; index < BASE_DIMENSION_COUNT; index++) {
        int theDim_exponent = theDim->num_exp[index] - theDim->den_exp[index];
        if (theDim_exponent != 0)
            return false;
    }
    return true;
}
bool SIDimensionalityIsDimensionlessAndNotDerived(SIDimensionalityRef theDim) {
    // To be dimensionless and not derived all the numerator and denominator exponents must be 0
    IF_NO_OBJECT_EXISTS_RETURN(theDim, false);
    for (SIBaseDimensionIndex index = 0; index < BASE_DIMENSION_COUNT; index++) {
        if (theDim->num_exp[index] != 0)
            return false;
        if (theDim->den_exp[index] != 0)
            return false;
    }
    return true;
}
bool SIDimensionalityIsDimensionlessAndDerived(SIDimensionalityRef theDim) {
    if (SIDimensionalityIsDimensionlessAndNotDerived(theDim))
        return false;
    if (SIDimensionalityIsDimensionless(theDim))
        return true;
    return false;
}
bool SIDimensionalityIsBaseDimensionality(SIDimensionalityRef theDim) {
    IF_NO_OBJECT_EXISTS_RETURN(theDim, false);
    if (SIDimensionalityIsDimensionlessAndNotDerived(theDim))
        return false;
    // If it is base dimensionality, then all the denominator exponents must be 0
    // and all numerator exponents are zero except one, which is 1
    for (SIBaseDimensionIndex index = 0; index < BASE_DIMENSION_COUNT; index++)
        if (SIDimensionalityGetDenExpAtIndex(theDim, index) != 0)
            return false;
    int count = 0;
    for (SIBaseDimensionIndex index = 0; index < BASE_DIMENSION_COUNT; index++) {
        if (theDim->num_exp[index] > 1)
            return false;
        if (theDim->num_exp[index] == 1)
            count++;
    }
    if (count == 1)
        return true;
    return false;
}
bool SIDimensionalityIsDerived(SIDimensionalityRef theDim) {
    IF_NO_OBJECT_EXISTS_RETURN(theDim, false);
    if (SIDimensionalityIsDimensionlessAndNotDerived(theDim))
        return false;
    if (SIDimensionalityIsBaseDimensionality(theDim))
        return false;
    return true;
}
bool SIDimensionalityHasSameReducedDimensionality(SIDimensionalityRef theDim1, SIDimensionalityRef theDim2) {
    IF_NO_OBJECT_EXISTS_RETURN(theDim1, NULL);
    IF_NO_OBJECT_EXISTS_RETURN(theDim2, NULL);
    if (theDim1 == theDim2)
        return true;
    for (int i = 0; i < BASE_DIMENSION_COUNT; i++) {
        int theDim1_exponent = theDim1->num_exp[i] - theDim1->den_exp[i];
        int theDim2_exponent = theDim2->num_exp[i] - theDim2->den_exp[i];
        if (theDim1_exponent != theDim2_exponent)
            return false;
    }
    return true;
}
bool SIDimensionalityCanBeReduced(SIDimensionalityRef theDim) {
    IF_NO_OBJECT_EXISTS_RETURN(theDim, false);

    // A dimensionality can be reduced if there are common factors
    // between numerator and denominator exponents for any base dimension
    for (SIBaseDimensionIndex index = 0; index < BASE_DIMENSION_COUNT; index++) {
        uint8_t num_exp = theDim->num_exp[index];
        uint8_t den_exp = theDim->den_exp[index];

        // If both numerator and denominator have non-zero exponents,
        // they can be reduced by their common factor
        if (num_exp > 0 && den_exp > 0) {
            return true;
        }
    }

    return false;
}
static bool SIDimensionalityHasExponents(SIDimensionalityRef theDim,
                                         uint8_t mass_num_exp, uint8_t mass_den_exp,
                                         uint8_t length_num_exp, uint8_t length_den_exp,
                                         uint8_t time_num_exp, uint8_t time_den_exp,
                                         uint8_t current_num_exp, uint8_t current_den_exp,
                                         uint8_t temperature_num_exp, uint8_t temperature_den_exp,
                                         uint8_t amount_num_exp, uint8_t amount_den_exp,
                                         uint8_t luminous_num_exp, uint8_t luminous_den_exp) {
    // Treat NULL as the dimensionless case
    if (!theDim) {
        theDim = SIDimensionalityDimensionless();
    }
    // Pack expected exponents into two arrays
    uint8_t expected_num[BASE_DIMENSION_COUNT] = {
        mass_num_exp,
        length_num_exp,
        time_num_exp,
        current_num_exp,
        temperature_num_exp,
        amount_num_exp,
        luminous_num_exp};
    uint8_t expected_den[BASE_DIMENSION_COUNT] = {
        mass_den_exp,
        length_den_exp,
        time_den_exp,
        current_den_exp,
        temperature_den_exp,
        amount_den_exp,
        luminous_den_exp};
    // Compare in one loop
    for (size_t i = 0; i < BASE_DIMENSION_COUNT; ++i) {
        if (theDim->num_exp[i] != expected_num[i] ||
            theDim->den_exp[i] != expected_den[i]) {
            return false;
        }
    }
    return true;
}
bool SIDimensionalityHasReducedExponents(SIDimensionalityRef theDim,
                                         int8_t mass_exponent,
                                         int8_t length_exponent,
                                         int8_t time_exponent,
                                         int8_t current_exponent,
                                         int8_t temperature_exponent,
                                         int8_t amount_exponent,
                                         int8_t luminous_intensity_exponent) {
    // Guard against NULL
    IF_NO_OBJECT_EXISTS_RETURN(theDim, false);
    // Pack the expected reduced exponents
    int8_t expected[BASE_DIMENSION_COUNT] = {
        mass_exponent,
        length_exponent,
        time_exponent,
        current_exponent,
        temperature_exponent,
        amount_exponent,
        luminous_intensity_exponent};
    // Compare all in one loop
    for (size_t i = 0; i < BASE_DIMENSION_COUNT; ++i) {
        if (SIDimensionalityReducedExponentAtIndex(theDim, (SIBaseDimensionIndex)i) != expected[i]) {
            return false;
        }
    }
    return true;
}
/*
  @function SIDimensionalityHasSameDimensionlessAndDerivedDimensionalities
  @abstract Determines if the two Dimensionalities have the same dimensionless exponents,
  @param theDim1 The first Dimensionality.
  @param theDim2 The second Dimensionality.
  @result true or false.
  */
static bool SIDimensionalityHasSameDimensionlessAndDerivedDimensionalities(SIDimensionalityRef theDim1, SIDimensionalityRef theDim2) {
    IF_NO_OBJECT_EXISTS_RETURN(theDim1, NULL);
    IF_NO_OBJECT_EXISTS_RETURN(theDim2, NULL);
    if (!SIDimensionalityIsDimensionlessAndDerived(theDim1))
        return false;
    if (!SIDimensionalityIsDimensionlessAndDerived(theDim2))
        return false;
    if (!SIDimensionalityEqual(theDim1, theDim2))
        return false;
    return true;
}
