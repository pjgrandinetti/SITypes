//
//  SIDimensionality.c
//  SITypes
//
//  Created by philip on 6/7/17.
//  Copyright © 2017 PhySy Ltd. All rights reserved.
//

#include "SILibrary.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define BASE_DIMENSION_COUNT 7

static OCTypeID kSIDimensionalityID = _kOCNotATypeID;

// SIDimensionality Opaque Type
struct __SIDimensionality
{
    OCBase _base;

    // SIDimensionality Type attributes  - order of declaration is essential
    uint8_t num_exp[BASE_DIMENSION_COUNT];
    uint8_t den_exp[BASE_DIMENSION_COUNT];
    OCStringRef symbol;
};

bool __SIDimensionalityEqual(const void *theType1, const void *theType2)
{
    IF_NO_OBJECT_EXISTS_RETURN(theType1, false)
    IF_NO_OBJECT_EXISTS_RETURN(theType2, false)

    SIDimensionalityRef theDim1 = (SIDimensionalityRef)theType1;
    SIDimensionalityRef theDim2 = (SIDimensionalityRef)theType2;
    if (theDim1->_base.typeID != theDim2->_base.typeID)
        return false;

    if (NULL == theDim1 || NULL == theDim2)
        return false;
    if (theDim1 == theDim2)
        return true;
    for (int i = 0; i < BASE_DIMENSION_COUNT; i++)
    {
        if (theDim1->num_exp[i] != theDim2->num_exp[i])
            return false;
        if (theDim1->den_exp[i] != theDim2->den_exp[i])
            return false;
    }

    return true;
}

void __SIDimensionalityFinalize(const void *theType)
{
    if (NULL == theType)
        return;
    SIDimensionalityRef theDim = (SIDimensionalityRef)theType;
    free((void *)theDim);
}

static OCStringRef __SIDimensionalityCopyFormattingDescription(OCTypeRef theType)
{
    SIDimensionalityRef theDim = (SIDimensionalityRef)theType;
    return (OCStringRef)OCStringCreateCopy(theDim->symbol);
}

OCTypeID SIDimensionalityGetTypeID(void)
{
    if (kSIDimensionalityID == _kOCNotATypeID)
        kSIDimensionalityID = OCRegisterType("SIDimensionality");
    return kSIDimensionalityID;
}

static struct __SIDimensionality *SIDimensionalityAllocate()
{
    struct __SIDimensionality *theDim = malloc(sizeof(struct __SIDimensionality));
    if (NULL == theDim)
        return NULL;
    theDim->_base.typeID = SIDimensionalityGetTypeID();
    theDim->_base.static_instance = false;
    theDim->_base.finalize = __SIDimensionalityFinalize;
    theDim->_base.equal = __SIDimensionalityEqual;
    theDim->_base.copyFormattingDesc = __SIDimensionalityCopyFormattingDescription;
    theDim->symbol = NULL;
    theDim->_base.retainCount = 0;
    OCRetain(theDim);
    return theDim;
}

#pragma mark Static Utility Functions

static OCStringRef baseDimensionSymbol(SIBaseDimensionIndex index)
{
    switch (index)
    {
    case kSILengthIndex:
        return STR("L");
    case kSIMassIndex:
        return STR("M");
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

static uint8_t getExpAtIndex(SIDimensionalityRef dimensionality, SIBaseDimensionIndex index, bool isNumerator)
{
    if (dimensionality == NULL)
        return 0;
    return isNumerator ? dimensionality->num_exp[index] : dimensionality->den_exp[index];
}

uint8_t SIDimensionalityGetNumExpAtIndex(SIDimensionalityRef theDim, SIBaseDimensionIndex index)
{
    return getExpAtIndex(theDim, index, true);
}

uint8_t SIDimensionalityGetDenExpAtIndex(SIDimensionalityRef theDim, SIBaseDimensionIndex index)
{
    return getExpAtIndex(theDim, index, false);
}

// dimLibrary is a Singleton
OCMutableDictionaryRef dimLibrary = NULL;
OCMutableDictionaryRef dimQuantitiesLibrary = NULL;
static void DimensionalityLibraryBuild();

static void appendDimensionSymbol(OCMutableStringRef str, uint8_t exponent, OCStringRef symbol)
{
    if (exponent > 0)
    {
        if (OCStringGetLength(str) > 0)
        {
            OCStringAppendFormat(str, STR("•"));
        }
        if (exponent != 1)
        {
            OCStringAppendFormat(str, STR("%@^%d"), symbol, exponent);
        }
        else
        {
            OCStringAppendFormat(str, STR("%@"), symbol);
        }
    }
}

static OCStringRef
SIDimensionalityCreateSymbol(SIDimensionalityRef theDim)
{
    if (NULL == theDim) {
        return NULL;
    }

    // build up numerator and denominator
    OCMutableStringRef num = OCStringCreateMutable(0);
    OCMutableStringRef den = OCStringCreateMutable(0);
    bool multiDen = false;

    for (uint8_t i = 0; i < BASE_DIMENSION_COUNT; i++) {
        OCStringRef base = baseDimensionSymbol(i);

        // numerator
        if (theDim->num_exp[i] > 0) {
            appendDimensionSymbol(num, theDim->num_exp[i], base);
        }

        // denominator (track if we append more than one)
        if (theDim->den_exp[i] > 0) {
            if (OCStringGetLength(den) > 0) {
                multiDen = true;
            }
            appendDimensionSymbol(den, theDim->den_exp[i], base);
        }
    }

    // pick the right format
    size_t lenN = OCStringGetLength(num);
    size_t lenD = OCStringGetLength(den);
    OCStringRef sym = NULL;

    if (lenN && lenD) {
        // e.g. "kg/(m·s2)" vs "kg/m"
        sym = OCStringCreateWithFormat(
            multiDen ? STR("%@/(%@)") : STR("%@/%@"),
            num, den
        );
    }
    else if (lenN) {
        // pure numerator, e.g. "m2·kg"
        sym = num;            // transfer ownership
    }
    else if (lenD) {
        // pure denominator, e.g. "1/(s·A)" vs "1/s"
        sym = OCStringCreateWithFormat(
            multiDen ? STR("1/(%@)") : STR("1/%@"),
            den
        );
    }
    else {
        // dimensionless
        sym = STR("1");       // constant, no release needed
    }

    // balance out the two mutable temporaries
    if (sym != num) OCRelease(num);
    if (sym != den) OCRelease(den);

    return sym;
}


#pragma mark Designated Creator

/*
 @function SIDimensionalityCreate
 @abstract Creates a SIDimensionality.
 @param length_num_exp integer numerator exponent for length dimension
 @param length_den_exp integer denominator exponent for length dimension
 @param mass_num_exp integer numerator exponent for mass dimension
 @param mass_den_exp integer denominator exponent for mass dimension
 @param time_num_exp integer numerator exponent for time dimension
 @param time_den_exp integer denominator exponent for time dimension
 @param current_num_exp integer numerator exponent for current dimension
 @param current_den_exp integer denominator exponent for current dimension
 @param temperature_num_exp integer numerator exponent for temperature dimension
 @param temperature_den_exp integer denominator exponent for temperature dimension
 @param amount_num_exp integer numerator exponent for amount dimension
 @param amount_den_exp integer denominator exponent for amount dimension
 @param luminous_intensity_num_exp integer numerator exponent for luminous intensity dimension
 @param luminous_intensity_den_exp integer denominator exponent for luminous intensity dimension
 @result SIDimensionality object
 */
static SIDimensionalityRef SIDimensionalityCreate(
    uint8_t length_num_exp, uint8_t length_den_exp,
    uint8_t mass_num_exp, uint8_t mass_den_exp,
    uint8_t time_num_exp, uint8_t time_den_exp,
    uint8_t current_num_exp, uint8_t current_den_exp,
    uint8_t temperature_num_exp, uint8_t temperature_den_exp,
    uint8_t amount_num_exp, uint8_t amount_den_exp,
    uint8_t luminous_intensity_num_exp, uint8_t luminous_intensity_den_exp)
{
    // Initialize object
    struct __SIDimensionality *theDim = SIDimensionalityAllocate();
    if (NULL == theDim)
        return NULL;

    //  setup attributes
    theDim->num_exp[kSILengthIndex] = length_num_exp;
    theDim->den_exp[kSILengthIndex] = length_den_exp;
    theDim->num_exp[kSIMassIndex] = mass_num_exp;
    theDim->den_exp[kSIMassIndex] = mass_den_exp;
    theDim->num_exp[kSITimeIndex] = time_num_exp;
    theDim->den_exp[kSITimeIndex] = time_den_exp;
    theDim->num_exp[kSICurrentIndex] = current_num_exp;
    theDim->den_exp[kSICurrentIndex] = current_den_exp;
    theDim->num_exp[kSITemperatureIndex] = temperature_num_exp;
    theDim->den_exp[kSITemperatureIndex] = temperature_den_exp;
    theDim->num_exp[kSIAmountIndex] = amount_num_exp;
    theDim->den_exp[kSIAmountIndex] = amount_den_exp;
    theDim->num_exp[kSILuminousIntensityIndex] = luminous_intensity_num_exp;
    theDim->den_exp[kSILuminousIntensityIndex] = luminous_intensity_den_exp;

    theDim->symbol = SIDimensionalityCreateSymbol(theDim);

    return (SIDimensionalityRef)theDim;
}

#pragma mark Accessors

OCStringRef SIDimensionalityGetSymbol(SIDimensionalityRef theDim)
{
    return theDim->symbol;
}

/*
 @function SIDimensionalityReducedExponentAtIndex
 @abstract Returns the exponent for the dimension at Index.
 @param theDim The Dimensionality.
 @result the integer exponent (numerator-denominator).
 @discussion base units length, mass, time, current, temperature, amount, and luminous intensity are
 assigned to index constants kSILengthIndex, kSIMassIndex, kSITimeIndex, kSICurrentIndex,  kSITemperatureIndex,
 kSIAmountIndex, kSILuminousIntensityIndex, respectively.
 */
int8_t SIDimensionalityReducedExponentAtIndex(SIDimensionalityRef theDim, SIBaseDimensionIndex index)
{
    IF_NO_OBJECT_EXISTS_RETURN(theDim, 0)

    if (theDim)
        return theDim->num_exp[index] - theDim->den_exp[index];
    return 0;
}

#pragma mark Operations

static SIDimensionalityRef SIDimensionalityWithExponents(
    uint8_t length_num_exp, uint8_t length_den_exp,
    uint8_t mass_num_exp, uint8_t mass_den_exp,
    uint8_t time_num_exp, uint8_t time_den_exp,
    uint8_t current_num_exp, uint8_t current_den_exp,
    uint8_t temperature_num_exp, uint8_t temperature_den_exp,
    uint8_t amount_num_exp, uint8_t amount_den_exp,
    uint8_t luminous_intensity_num_exp, uint8_t luminous_intensity_den_exp)
{
    SIDimensionalityRef newDim = SIDimensionalityCreate(length_num_exp, length_den_exp,
                                                        mass_num_exp, mass_den_exp,
                                                        time_num_exp, time_den_exp,
                                                        current_num_exp, current_den_exp,
                                                        temperature_num_exp, temperature_den_exp,
                                                        amount_num_exp, amount_den_exp,
                                                        luminous_intensity_num_exp, luminous_intensity_den_exp);

    if (NULL == newDim)
        return NULL;

    if (NULL == dimLibrary)
        DimensionalityLibraryBuild();

    if (OCDictionaryContainsKey(dimLibrary, newDim->symbol))
    {
        SIDimensionalityRef existingDim = OCDictionaryGetValue(dimLibrary, newDim->symbol);

        OCRelease(newDim);
        return existingDim;
    }

    OCTypeSetStaticInstance(newDim, true);
    OCDictionaryAddValue(dimLibrary, newDim->symbol, newDim);
    OCRelease(newDim);
    return newDim;
}

SIDimensionalityRef SIDimensionalityDimensionless()
{
    return SIDimensionalityWithExponents(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}

SIDimensionalityRef SIDimensionalityForBaseDimensionIndex(SIBaseDimensionIndex index)
{

    switch (index)
    {
    case kSILengthIndex:
        return SIDimensionalityWithExponents(1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    case kSIMassIndex:
        return SIDimensionalityWithExponents(0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    case kSITimeIndex:
        return SIDimensionalityWithExponents(0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    case kSICurrentIndex:
        return SIDimensionalityWithExponents(0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0);
    case kSITemperatureIndex:
        return SIDimensionalityWithExponents(0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0);
    case kSIAmountIndex:
        return SIDimensionalityWithExponents(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0);
    case kSILuminousIntensityIndex:
        return SIDimensionalityWithExponents(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0);
    }
    return NULL;
}

SIDimensionalityRef SIDimensionalityWithBaseDimensionSymbol(OCStringRef theString, OCStringRef *error)
{
    if (error)
        if (*error)
            return NULL;

    if (OCStringGetLength(theString) > 1)
    {
        if (error)
        {
            *error = STR("Invalid Dimensionality symbol.");
        }
        return NULL;
    }

    SIBaseDimensionIndex index;
    if (OCStringCompare(theString, STR("L"), 0) == kOCCompareEqualTo)
        index = kSILengthIndex;
    else if (OCStringCompare(theString, STR("M"), 0) == kOCCompareEqualTo)
        index = kSIMassIndex;
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

SIDimensionalityRef SIDimensionalityByReducing(SIDimensionalityRef theDim)
{
    IF_NO_OBJECT_EXISTS_RETURN(theDim, NULL)

    uint8_t num_exp[BASE_DIMENSION_COUNT];
    uint8_t den_exp[BASE_DIMENSION_COUNT];

    for (int8_t i = 0; i < BASE_DIMENSION_COUNT; i++)
    {
        int power = theDim->num_exp[i] - theDim->den_exp[i];
        if (power > 0)
        {
            num_exp[i] = power;
            den_exp[i] = 0;
        }
        else if (power < 0)
        {
            den_exp[i] = -power;
            num_exp[i] = 0;
        }
        else
            den_exp[i] = num_exp[i] = 0;
    }
    return SIDimensionalityWithExponents(num_exp[kSILengthIndex], den_exp[kSILengthIndex],
                                         num_exp[kSIMassIndex], den_exp[kSIMassIndex],
                                         num_exp[kSITimeIndex], den_exp[kSITimeIndex],
                                         num_exp[kSICurrentIndex], den_exp[kSICurrentIndex],
                                         num_exp[kSITemperatureIndex], den_exp[kSITemperatureIndex],
                                         num_exp[kSIAmountIndex], den_exp[kSIAmountIndex],
                                         num_exp[kSILuminousIntensityIndex], den_exp[kSILuminousIntensityIndex]);
}

SIDimensionalityRef SIDimensionalityByTakingNthRoot(SIDimensionalityRef theDim, uint8_t root, OCStringRef *error)
{
    if (error)
        if (*error)
            return NULL;

    IF_NO_OBJECT_EXISTS_RETURN(theDim, NULL)

    uint8_t num_exp[BASE_DIMENSION_COUNT];
    uint8_t den_exp[BASE_DIMENSION_COUNT];

    SIDimensionalityRef reducedDim = SIDimensionalityByReducing(theDim);

    for (int8_t i = 0; i < BASE_DIMENSION_COUNT; i++)
    {
        if (reducedDim->num_exp[i] % root != 0 || reducedDim->den_exp[i] % root != 0)
        {
            if (error)
            {
                *error = STR("Can't raise physical dimensionality to a non-integer power.");
            }
            return NULL;
        }
        num_exp[i] = theDim->num_exp[i] / root;
        den_exp[i] = theDim->den_exp[i] / root;
    }

    return SIDimensionalityWithExponents(num_exp[kSILengthIndex], den_exp[kSILengthIndex],
                                         num_exp[kSIMassIndex], den_exp[kSIMassIndex],
                                         num_exp[kSITimeIndex], den_exp[kSITimeIndex],
                                         num_exp[kSICurrentIndex], den_exp[kSICurrentIndex],
                                         num_exp[kSITemperatureIndex], den_exp[kSITemperatureIndex],
                                         num_exp[kSIAmountIndex], den_exp[kSIAmountIndex],
                                         num_exp[kSILuminousIntensityIndex], den_exp[kSILuminousIntensityIndex]);
}

SIDimensionalityRef SIDimensionalityByDividingWithoutReducing(SIDimensionalityRef theDim1, SIDimensionalityRef theDim2)
{
    /*
     *	This routine will create an derived SI Dimensionality formed by the division of theDim1 by theDim2.
     *	It will additionally return a multiplier for the numerical part of the quantity product
     *
     */
    IF_NO_OBJECT_EXISTS_RETURN(theDim1, NULL)
    IF_NO_OBJECT_EXISTS_RETURN(theDim2, NULL)

    uint8_t num_exp[BASE_DIMENSION_COUNT];
    uint8_t den_exp[BASE_DIMENSION_COUNT];
    for (uint8_t i = 0; i < BASE_DIMENSION_COUNT; i++)
    {
        num_exp[i] = theDim1->num_exp[i] + theDim2->den_exp[i];
        den_exp[i] = theDim1->den_exp[i] + theDim2->num_exp[i];
    }
    return SIDimensionalityWithExponents(num_exp[kSILengthIndex], den_exp[kSILengthIndex],
                                         num_exp[kSIMassIndex], den_exp[kSIMassIndex],
                                         num_exp[kSITimeIndex], den_exp[kSITimeIndex],
                                         num_exp[kSICurrentIndex], den_exp[kSICurrentIndex],
                                         num_exp[kSITemperatureIndex], den_exp[kSITemperatureIndex],
                                         num_exp[kSIAmountIndex], den_exp[kSIAmountIndex],
                                         num_exp[kSILuminousIntensityIndex], den_exp[kSILuminousIntensityIndex]);
}

SIDimensionalityRef SIDimensionalityByDividing(SIDimensionalityRef theDim1, SIDimensionalityRef theDim2)
{
    return SIDimensionalityByReducing(SIDimensionalityByDividingWithoutReducing(theDim1, theDim2));
}

SIDimensionalityRef SIDimensionalityByRaisingToAPowerWithoutReducing(SIDimensionalityRef theDim, double power, OCStringRef *error)
{
    /*
     *	This routine will create an derived SI Dimensionality formed by the raising theDim to a power.
     *	It will additionally return a multiplier for the numerical part of the quantity product
     *
     */

    if (error)
        if (*error)
            return NULL;
    IF_NO_OBJECT_EXISTS_RETURN(theDim, NULL)

    int32_t pow = (int32_t)floor(power);
    double fraction = power - pow;
    if (OCCompareDoubleValues(fraction, 0.0) != kOCCompareEqualTo)
    {
        int32_t root = (int32_t)floor(1. / power);
        fraction = 1. / power - root;
        if (OCCompareDoubleValues(fraction, 0.0) == kOCCompareEqualTo)
            return SIDimensionalityByTakingNthRoot(theDim, root, error);
        else
        {
            if (error)
            {
                *error = STR("Can't raise physical dimensionality to a non-integer power.");
            }
            return NULL;
        }
    }

    uint8_t num_exp[BASE_DIMENSION_COUNT];
    uint8_t den_exp[BASE_DIMENSION_COUNT];
    for (uint8_t i = 0; i < BASE_DIMENSION_COUNT; i++)
    {
        if (pow > 0)
        {
            num_exp[i] = theDim->num_exp[i] * pow;
            den_exp[i] = theDim->den_exp[i] * pow;
        }
        else
        {
            num_exp[i] = theDim->den_exp[i] * (-pow);
            den_exp[i] = theDim->num_exp[i] * (-pow);
        }
    }
    return SIDimensionalityWithExponents(num_exp[kSILengthIndex], den_exp[kSILengthIndex],
                                         num_exp[kSIMassIndex], den_exp[kSIMassIndex],
                                         num_exp[kSITimeIndex], den_exp[kSITimeIndex],
                                         num_exp[kSICurrentIndex], den_exp[kSICurrentIndex],
                                         num_exp[kSITemperatureIndex], den_exp[kSITemperatureIndex],
                                         num_exp[kSIAmountIndex], den_exp[kSIAmountIndex],
                                         num_exp[kSILuminousIntensityIndex], den_exp[kSILuminousIntensityIndex]);
}

SIDimensionalityRef SIDimensionalityByRaisingToAPower(SIDimensionalityRef theDim, double power, OCStringRef *error)
{
    if (error)
        if (*error)
            return NULL;
    return SIDimensionalityByReducing(SIDimensionalityByRaisingToAPowerWithoutReducing(theDim, power, error));
}

SIDimensionalityRef SIDimensionalityByMultiplyingWithoutReducing(SIDimensionalityRef theDim1, SIDimensionalityRef theDim2, OCStringRef *error)
{
    /*
     *	This routine will create an derived SI Dimensionality formed by the product of two Dimensionalities.
     *	It will additionally return a multiplier for the numerical part of the quantity product
     *
     */
    if (error)
        if (*error)
            return NULL;
    IF_NO_OBJECT_EXISTS_RETURN(theDim1, NULL)
    IF_NO_OBJECT_EXISTS_RETURN(theDim2, NULL)
    if (theDim1 == theDim2)
        return SIDimensionalityByRaisingToAPowerWithoutReducing(theDim1, 2, error);

    uint8_t num_exp[BASE_DIMENSION_COUNT];
    uint8_t den_exp[BASE_DIMENSION_COUNT];
    for (uint8_t i = 0; i < BASE_DIMENSION_COUNT; i++)
    {
        num_exp[i] = theDim1->num_exp[i] + theDim2->num_exp[i];
        den_exp[i] = theDim1->den_exp[i] + theDim2->den_exp[i];
    }
    return SIDimensionalityWithExponents(num_exp[kSILengthIndex], den_exp[kSILengthIndex],
                                         num_exp[kSIMassIndex], den_exp[kSIMassIndex],
                                         num_exp[kSITimeIndex], den_exp[kSITimeIndex],
                                         num_exp[kSICurrentIndex], den_exp[kSICurrentIndex],
                                         num_exp[kSITemperatureIndex], den_exp[kSITemperatureIndex],
                                         num_exp[kSIAmountIndex], den_exp[kSIAmountIndex],
                                         num_exp[kSILuminousIntensityIndex], den_exp[kSILuminousIntensityIndex]);
}

SIDimensionalityRef SIDimensionalityByMultiplying(SIDimensionalityRef theDim1, SIDimensionalityRef theDim2, OCStringRef *error)
{
    if (error)
        if (*error)
            return NULL;
    return SIDimensionalityByReducing(SIDimensionalityByMultiplyingWithoutReducing(theDim1, theDim2, error));
}

OCArrayRef SIDimensionalityCreateArrayOfQuantities(SIDimensionalityRef theDim)
{
    IF_NO_OBJECT_EXISTS_RETURN(theDim, NULL)

    if (NULL == dimQuantitiesLibrary)
        DimensionalityLibraryBuild();
    uint64_t count = OCDictionaryGetCountOfValue(dimQuantitiesLibrary, theDim);
    if (0 == count)
        return NULL;
    OCStringRef quantities[count];

    uint64_t totalCount = OCDictionaryGetCount(dimQuantitiesLibrary);
    OCStringRef keys[totalCount];
    SIDimensionalityRef dimens[totalCount];

    OCDictionaryGetKeysAndValues(dimQuantitiesLibrary, (const void **)keys, (const void **)dimens);
    uint64_t i = 0;
    for (uint64_t index = 0; index < totalCount; index++)
    {
        if (__SIDimensionalityEqual(dimens[index], theDim))
        {
            quantities[i++] = keys[index];
        }
    }

    OCArrayRef result = NULL;
    if (i == count)
        result = OCArrayCreate((const void **)quantities, count, &kOCTypeArrayCallBacks);
    return result;
}

OCArrayRef SIDimensionalityCreateArrayWithSameReducedDimensionality(SIDimensionalityRef theDim)
{
    IF_NO_OBJECT_EXISTS_RETURN(theDim, NULL)
    if (NULL == dimQuantitiesLibrary)
        DimensionalityLibraryBuild();

    int8_t reducedLength = theDim->num_exp[kSILengthIndex] - theDim->den_exp[kSILengthIndex];
    int8_t largestLength = theDim->num_exp[kSILengthIndex];
    if (reducedLength < 0)
        largestLength = theDim->den_exp[kSILengthIndex];

    int8_t reducedMass = theDim->num_exp[kSIMassIndex] - theDim->den_exp[kSIMassIndex];
    int8_t largestMass = theDim->num_exp[kSIMassIndex];
    if (reducedMass < 0)
        largestMass = theDim->den_exp[kSIMassIndex];

    int8_t reducedTime = theDim->num_exp[kSITimeIndex] - theDim->den_exp[kSITimeIndex];
    int8_t largestTime = theDim->num_exp[kSITimeIndex];
    if (reducedTime < 0)
        largestTime = theDim->den_exp[kSITimeIndex];

    int8_t reducedCurrent = theDim->num_exp[kSICurrentIndex] - theDim->den_exp[kSICurrentIndex];
    int8_t largestCurrent = theDim->num_exp[kSICurrentIndex];
    if (reducedCurrent < 0)
        largestCurrent = theDim->den_exp[kSICurrentIndex];

    int8_t reducedTemperature = theDim->num_exp[kSITemperatureIndex] - theDim->den_exp[kSITemperatureIndex];
    int8_t largestTemperature = theDim->num_exp[kSITemperatureIndex];
    if (reducedTemperature < 0)
        largestTemperature = theDim->den_exp[kSITemperatureIndex];

    int8_t reducedAmount = theDim->num_exp[kSIAmountIndex] - theDim->den_exp[kSIAmountIndex];
    int8_t largestAmount = theDim->num_exp[kSIAmountIndex];
    if (reducedAmount < 0)
        largestAmount = theDim->den_exp[kSIAmountIndex];

    int8_t reducedLuminous = theDim->num_exp[kSILuminousIntensityIndex] - theDim->den_exp[kSILuminousIntensityIndex];
    int8_t largestLuminous = theDim->num_exp[kSILuminousIntensityIndex];
    if (reducedLuminous < 0)
        largestLuminous = theDim->den_exp[kSILuminousIntensityIndex];

    OCMutableArrayRef result = OCArrayCreateMutable(0, &kOCTypeArrayCallBacks);
    if (result)
    {
        for (int8_t iLength = largestLength; iLength >= abs(reducedLength); iLength--)
        {
            int8_t length_num = iLength;
            int8_t length_den = iLength - abs(reducedLength);
            if (reducedLength < 0)
            {
                length_num = iLength - abs(reducedLength);
                length_den = iLength;
            }
            for (int8_t iMass = largestMass; iMass >= abs(reducedMass); iMass--)
            {
                int8_t mass_num = iMass;
                int8_t mass_den = iMass - abs(reducedMass);
                if (reducedMass < 0)
                {
                    mass_num = iMass - abs(reducedMass);
                    mass_den = iMass;
                }
                for (int8_t iTime = largestTime; iTime >= abs(reducedTime); iTime--)
                {
                    int8_t time_num = iTime;
                    int8_t time_den = iTime - abs(reducedTime);
                    if (reducedTime < 0)
                    {
                        time_num = iTime - abs(reducedTime);
                        time_den = iTime;
                    }
                    for (int8_t iCurrent = largestCurrent; iCurrent >= abs(reducedCurrent); iCurrent--)
                    {
                        int8_t current_num = iCurrent;
                        int8_t current_den = iCurrent - abs(reducedCurrent);
                        if (reducedCurrent < 0)
                        {
                            current_num = iCurrent - abs(reducedCurrent);
                            current_den = iCurrent;
                        }
                        for (int8_t iTemp = largestTemperature; iTemp >= abs(reducedTemperature); iTemp--)
                        {
                            int8_t temperature_num = iTemp;
                            int8_t temperature_den = iTemp - abs(reducedTemperature);
                            if (reducedTemperature < 0)
                            {
                                temperature_num = iTemp - abs(reducedTemperature);
                                temperature_den = iTemp;
                            }
                            for (int8_t iAmount = largestAmount; iAmount >= abs(reducedAmount); iAmount--)
                            {
                                int8_t amount_num = iAmount;
                                int8_t amount_den = iAmount - abs(reducedAmount);
                                if (reducedAmount < 0)
                                {
                                    amount_num = iAmount - abs(reducedAmount);
                                    amount_den = iAmount;
                                }
                                for (int8_t iLuminous = largestLuminous; iLuminous >= abs(reducedLuminous); iLuminous--)
                                {
                                    int8_t luminous_num = iLuminous;
                                    int8_t luminous_den = iLuminous - abs(reducedLuminous);
                                    if (reducedLuminous < 0)
                                    {
                                        luminous_num = iLuminous - abs(reducedLuminous);
                                        luminous_den = iLuminous;
                                    }
                                    SIDimensionalityRef dim = SIDimensionalityWithExponents(length_num, length_den,
                                                                                            mass_num, mass_den,
                                                                                            time_num, time_den,
                                                                                            current_num, current_den,
                                                                                            temperature_num, temperature_den,
                                                                                            amount_num, amount_den,
                                                                                            luminous_num, luminous_den);
                                    OCArrayAppendValue(result, dim);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return result;
}

OCArrayRef SIDimensionalityCreateArrayOfQuantitiesWithSameReducedDimensionality(SIDimensionalityRef theDim)
{
    OCArrayRef reducedDims = SIDimensionalityCreateArrayWithSameReducedDimensionality(theDim);
    OCMutableArrayRef result = OCArrayCreateMutable(0, &kOCTypeArrayCallBacks);
    for (uint64_t dimIndex = 0; dimIndex < OCArrayGetCount(reducedDims); dimIndex++)
    {
        SIDimensionalityRef dim = OCArrayGetValueAtIndex(reducedDims, dimIndex);
        OCArrayRef quantities = SIDimensionalityCreateArrayOfQuantities(dim);
        if (quantities)
        {
            OCArrayAppendArray(result, quantities, OCRangeMake(0, OCArrayGetCount(quantities)));
            OCRelease(quantities);
        }
    }
    OCRelease(reducedDims);
    return result;
}

#pragma mark Strings and Archiving

void SIDimensionalityShow(SIDimensionalityRef theDim)
{
    IF_NO_OBJECT_EXISTS_RETURN(theDim, )
    OCStringShow((theDim)->symbol);
    fprintf(stdout, "\n");
}

void SIDimensionalityShowFull(SIDimensionalityRef theDim)
{
    IF_NO_OBJECT_EXISTS_RETURN(theDim, )

    OCStringShow(STR("============================================================================================================="));

    OCStringRef cf_string;

    OCStringShow(STR("                                            m  kg   s   A   K  mol cd"));
    OCStringShow(STR("-------------------------------------------------------------------------------------------------------------"));

    cf_string = OCStringCreateWithFormat(STR("SI base dimension numerator exponents:    %3d %3d %3d %3d %3d %3d %3d"),
                                         theDim->num_exp[kSILengthIndex],
                                         theDim->num_exp[kSIMassIndex],
                                         theDim->num_exp[kSITimeIndex],
                                         theDim->num_exp[kSICurrentIndex],
                                         theDim->num_exp[kSITemperatureIndex],
                                         theDim->num_exp[kSIAmountIndex],
                                         theDim->num_exp[kSILuminousIntensityIndex]);
    OCStringShow(cf_string);
    OCRelease(cf_string);

    cf_string = OCStringCreateWithFormat(STR("SI base dimension denominator exponents:  %3d %3d %3d %3d %3d %3d %3d"),
                                         theDim->den_exp[kSILengthIndex],
                                         theDim->den_exp[kSIMassIndex],
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
    if (quantities)
    {
        for (uint64_t index = 0; index < OCArrayGetCount(quantities); index++)
        {
            OCStringRef quantity = OCArrayGetValueAtIndex(quantities, index);
            OCStringShow(quantity);
        }
        OCRelease(quantities);
    }
    OCStringShow(STR("\n============================================================================================================="));

    fprintf(stderr, "\n\n");
}

#pragma mark Library

static SIDimensionalityRef AddDimensionalityToLibrary(uint8_t length_num_exp, uint8_t length_den_exp,
                                                      uint8_t mass_num_exp, uint8_t mass_den_exp,
                                                      uint8_t time_num_exp, uint8_t time_den_exp,
                                                      uint8_t current_num_exp, uint8_t current_den_exp,
                                                      uint8_t temperature_num_exp, uint8_t temperature_den_exp,
                                                      uint8_t amount_num_exp, uint8_t amount_den_exp,
                                                      uint8_t luminous_intensity_num_exp, uint8_t luminous_intensity_den_exp)
{
    SIDimensionalityRef dim = SIDimensionalityCreate(length_num_exp, length_den_exp,
                                                     mass_num_exp, mass_den_exp,
                                                     time_num_exp, time_den_exp,
                                                     current_num_exp, current_den_exp,
                                                     temperature_num_exp, temperature_den_exp,
                                                     amount_num_exp, amount_den_exp,
                                                     luminous_intensity_num_exp, luminous_intensity_den_exp);

    OCTypeSetStaticInstance(dim, true);
    OCDictionaryAddValue(dimLibrary, dim->symbol, dim);
    OCRelease(dim);
    return dim;
}

static void DimensionalityLibraryBuild()
{

    dimLibrary = OCDictionaryCreateMutable(0);
    dimQuantitiesLibrary = OCDictionaryCreateMutable(0);

    SIDimensionalityRef dim;

    // Base Root Name and Root Symbol Units - Table 1

    // ***** Dimensionless **********************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityDimensionless
    // Dimensionless                           length     mass        time        current     temperature     amount      luminous intensity
    dim = AddDimensionalityToLibrary(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityDimensionless, dim);

    // ***** Length *****************************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityLength
    // Length                                   length     mass        time        current     temperature     amount      luminous intensity
    dim = AddDimensionalityToLibrary(1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityLength, dim);

    // Wavenumber, Inverse Length
    dim = AddDimensionalityToLibrary(0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityWavenumber, dim);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityInverseLength, dim);

    // Plane Angle, Length Ratio
    dim = AddDimensionalityToLibrary(1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityPlaneAngle, dim);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityLengthRatio, dim);

    // ***** Mass *******************************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityMass
    // Mass                                     length      mass        time        current     temperature     amount      luminous intensity
    dim = AddDimensionalityToLibrary(0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityMass, dim);

    // Inverse Mass                                     length      mass        time        current     temperature     amount      luminous intensity
    dim = AddDimensionalityToLibrary(0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityInverseMass, dim);

    // Mass Ratio                               length      mass        time        current     temperature     amount      luminous intensity
    dim = AddDimensionalityToLibrary(0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityMassRatio, dim);

    // ***** Time *******************************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityTime
    // Time                                     length      mass        time        current     temperature     amount      luminous intensity
    dim = AddDimensionalityToLibrary(0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityTime, dim);

    // Inverse Time, Frequency, Radioactivity
    dim = AddDimensionalityToLibrary(0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityFrequency, dim);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityRadioactivity, dim);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityInverseTime, dim);

    // Time, Frequency Ratio
    dim = AddDimensionalityToLibrary(0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityTimeRatio, dim);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityFrequencyRatio, dim);

    // Inverse Time Squared
    dim = AddDimensionalityToLibrary(0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityInverseTimeSquared, dim);

    // ***** Current ****************************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityCurrent
    // Current                                  length     mass        time        current     temperature     amount      luminous intensity
    dim = AddDimensionalityToLibrary(0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityCurrent, dim);

    // Inverse Current                          length     mass        time        current     temperature     amount      luminous intensity
    dim = AddDimensionalityToLibrary(0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityInverseCurrent, dim);

    // Current Ratio                            length     mass        time        current     temperature     amount      luminous intensity
    dim = AddDimensionalityToLibrary(0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityCurrentRatio, dim);

    // ***** Thermodynamic Temperature **********************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityTemperature
    // Temperature                              length     mass        time        current     temperature     amount      luminous intensity
    dim = AddDimensionalityToLibrary(0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityTemperature, dim);

    // Inverse Temperature                      length     mass        time        current     temperature     amount      luminous intensity
    dim = AddDimensionalityToLibrary(0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityInverseTemperature, dim);

    // Temperature Ratio                        length     mass        time        current     temperature     amount      luminous intensity
    dim = AddDimensionalityToLibrary(0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityTemperatureRatio, dim);

    // ***** Amount *****************************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityAmount
    // Amount                                   length     mass        time        current     temperature     amount      luminous intensity
    dim = AddDimensionalityToLibrary(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityAmount, dim);

    // inverse amount
    dim = AddDimensionalityToLibrary(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityInverseAmount, dim);

    // amount ratio
    dim = AddDimensionalityToLibrary(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityAmountRatio, dim);

    // ***** Luminous Intensity *****************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityLuminousIntensity
    // Luminous Intensity                       length     mass        time        current     temperature     amount      luminous intensity
    dim = AddDimensionalityToLibrary(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityLuminousIntensity, dim);

    dim = AddDimensionalityToLibrary(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityInverseLuminousIntensity, dim);

    dim = AddDimensionalityToLibrary(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityLuminousIntensityRatio, dim);

    // ***** Area *******************************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityArea
    // Area
    dim = AddDimensionalityToLibrary(2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityArea, dim);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityRockPermeability, dim);

    // Inverse Area
    dim = AddDimensionalityToLibrary(0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityInverseArea, dim);

    // Area Ratio, Solid Angle
    dim = AddDimensionalityToLibrary(2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityAreaRatio, dim);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantitySolidAngle, dim);

    // ***** Volume *******************************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityVolume
    // Volume
    dim = AddDimensionalityToLibrary(3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityVolume, dim);

    // Inverse Volume
    dim = AddDimensionalityToLibrary(0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityInverseVolume, dim);

    // Volume Ratio
    dim = AddDimensionalityToLibrary(3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityVolumeRatio, dim);

    // Temperature Gradient                    length     mass        time        current     temperature     amount      luminous intensity
    dim = AddDimensionalityToLibrary(0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityTemperatureGradient, dim);

    // Coherent Units with no Unit name - Table 2

    // Speed, Velocity
    dim = AddDimensionalityToLibrary(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantitySpeed, dim);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityVelocity, dim);

    // Linear Momentum                          length     mass        time        current     temperature     amount      luminous intensity
    dim = AddDimensionalityToLibrary(1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityLinearMomentum, dim);

    // Acceleration
    dim = AddDimensionalityToLibrary(1, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityAcceleration, dim);

    // Moment of Inertia
    dim = AddDimensionalityToLibrary(2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityMomentOfInertia, dim);

    // Mass Flow Rate
    dim = AddDimensionalityToLibrary(0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityMassFlowRate, dim);

    // Mass Flux
    dim = AddDimensionalityToLibrary(0, 2, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityMassFlux, dim);

    // Diffusion Flux
    dim = AddDimensionalityToLibrary(0, 2, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityDiffusionFlux, dim);

    // Density
    dim = AddDimensionalityToLibrary(0, 3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityDensity, dim);

    // Specific Gravity
    dim = AddDimensionalityToLibrary(3, 3, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantitySpecificGravity, dim);

    // Surface Density
    dim = AddDimensionalityToLibrary(0, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantitySurfaceDensity, dim);

    // Specific Surface Area
    dim = AddDimensionalityToLibrary(2, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantitySpecificSurfaceArea, dim);

    // Surface Area to Volume Ratio
    dim = AddDimensionalityToLibrary(2, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantitySurfaceAreaToVolumeRatio, dim);

    // Specific Volume
    dim = AddDimensionalityToLibrary(3, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantitySpecificVolume, dim);

    // Current Density
    dim = AddDimensionalityToLibrary(0, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityCurrentDensity, dim);

    // Magnetic Field Strength
    dim = AddDimensionalityToLibrary(0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityMagneticFieldStrength, dim);

    // Luminance
    dim = AddDimensionalityToLibrary(0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityLuminance, dim);

    // Refractive Index
    dim = AddDimensionalityToLibrary(1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityRefractiveIndex, dim);

    // More Coherent Units with no Symbols - Table 4

    // Dynamic Viscosity
    dim = AddDimensionalityToLibrary(0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityDynamicViscosity, dim);

    // Fluidity (inverse dynamic viscosity)
    dim = AddDimensionalityToLibrary(1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityFluidity, dim);

    // Moment of Force
    dim = AddDimensionalityToLibrary(2, 0, 2, 1, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityMomentOfForce, dim);

    // Surface Tension
    dim = AddDimensionalityToLibrary(0, 0, 1, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantitySurfaceTension, dim);

    // Surface Energy                           length     mass        time        current     temperature     amount      luminous intensity
    dim = AddDimensionalityToLibrary(2, 2, 1, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantitySurfaceEnergy, dim);

    // Angular Velocity
    dim = AddDimensionalityToLibrary(1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityAngularVelocity, dim);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityAngularSpeed, dim);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityAngularFrequency, dim);

    // Angular Acceleration
    dim = AddDimensionalityToLibrary(1, 1, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityAngularAcceleration, dim);

    // Heat Flux Density, Irradiance
    dim = AddDimensionalityToLibrary(2, 2, 1, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityHeatFluxDensity, dim);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityIrradiance, dim);

    // Spectral Radiant Flux Density
    dim = AddDimensionalityToLibrary(0, 1, 1, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantitySpectralRadiantFluxDensity, dim);

    // Heat Capacity, Entropy
    dim = AddDimensionalityToLibrary(2, 0, 1, 0, 0, 2, 0, 0, 0, 1, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityHeatCapacity, dim);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityEntropy, dim);

    // Specific Heat Capacity, Specific Entropy
    dim = AddDimensionalityToLibrary(2, 0, 0, 0, 0, 2, 0, 0, 0, 1, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantitySpecificHeatCapacity, dim);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantitySpecificEntropy, dim);

    // Specific Energy
    dim = AddDimensionalityToLibrary(2, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantitySpecificEnergy, dim);

    // Thermal Conductance                      length      mass        time        current     temperature     amount      luminous intensity
    dim = AddDimensionalityToLibrary(2, 0, 1, 0, 0, 3, 0, 0, 0, 1, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityThermalConductance, dim);

    // Thermal Conductivity
    dim = AddDimensionalityToLibrary(1, 0, 1, 0, 0, 3, 0, 0, 0, 1, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityThermalConductivity, dim);

    // Electric Field Strength
    dim = AddDimensionalityToLibrary(1, 0, 1, 0, 0, 3, 0, 1, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityElectricFieldStrength, dim);

    // Electric Charge Density
    dim = AddDimensionalityToLibrary(0, 3, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityElectricChargeDensity, dim);

    //  Electric Flux                           length     mass        time        current   temperature  amount    luminous intensity
    dim = AddDimensionalityToLibrary(3, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityElectricFlux, dim);

    // Surface Charge Density, Electric Flux Density, Electric Displacement
    dim = AddDimensionalityToLibrary(0, 2, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantitySurfaceChargeDensity, dim);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityElectricFluxDensity, dim);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityElectricDisplacement, dim);

    // Electric Polarizability
    dim = AddDimensionalityToLibrary(2, 2, 0, 1, 4, 0, 2, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityElectricPolarizability, dim);

    // Electric Quadrupole Moment
    dim = AddDimensionalityToLibrary(2, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityElectricQuadrupoleMoment, dim);

    //  Magnetizability                        length     mass        time        current   temperature  amount    luminous intensity
    dim = AddDimensionalityToLibrary(2, 0, 1, 2, 4, 2, 2, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityMagnetizability, dim);

    // Permittivity
    dim = AddDimensionalityToLibrary(0, 3, 0, 1, 4, 0, 2, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityPermittivity, dim);

    //  Permeability                    length     mass        time        current   temperature  amount    luminous intensity
    dim = AddDimensionalityToLibrary(1, 0, 1, 0, 0, 2, 0, 2, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityPermeability, dim);

    // Molar Energy
    dim = AddDimensionalityToLibrary(2, 0, 1, 0, 0, 2, 0, 0, 0, 0, 0, 1, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityMolarEnergy, dim);

    // Molar Entropy, Molar Heat Capacity
    dim = AddDimensionalityToLibrary(2, 0, 1, 0, 0, 2, 0, 0, 0, 1, 0, 1, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityMolarEntropy, dim);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityMolarHeatCapacity, dim);

    // Absorbed Dose Rate
    dim = AddDimensionalityToLibrary(2, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityAbsorbedDoseRate, dim);

    // Radiant Intensity
    dim = AddDimensionalityToLibrary(4, 2, 1, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityRadiantIntensity, dim);

    // Spectral Radiant Intensity
    dim = AddDimensionalityToLibrary(4, 3, 1, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantitySpectralRadiantIntensity, dim);

    // Radiance
    dim = AddDimensionalityToLibrary(4, 4, 1, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0);

    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityRadiance, dim);

    // Spectral Radiance
    dim = AddDimensionalityToLibrary(4, 5, 1, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantitySpectralRadiance, dim);

    // Special Names and Symbols for Coherent Derived Units - Table 3

    // Porosity
    dim = AddDimensionalityToLibrary(3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityPorosity, dim);

    // Force
    dim = AddDimensionalityToLibrary(1, 0, 1, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityForce, dim);

    // Pressure, Stress, Energy Density
    dim = AddDimensionalityToLibrary(0, 1, 1, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityPressure, dim);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityStress, dim);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityEnergyDensity, dim);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityElasticModulus, dim);

    // Compressibility : Inverse Pressure
    dim = AddDimensionalityToLibrary(1, 0, 0, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityCompressibility, dim);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityStressOpticCoefficient, dim);

    // Pressure Gradient                       length     mass        time        current   temperature  amount    luminous intensity
    dim = AddDimensionalityToLibrary(0, 2, 1, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityPressureGradient, dim);

    // Energy, Work, Heat
    dim = AddDimensionalityToLibrary(2, 0, 1, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityEnergy, dim);

    // Spectral radiant energy
    dim = AddDimensionalityToLibrary(2, 1, 1, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantitySpectralRadiantEnergy, dim);

    // Torque
    dim = AddDimensionalityToLibrary(3, 1, 1, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityTorque, dim);

    // Power, Radiant Flux
    dim = AddDimensionalityToLibrary(2, 0, 1, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityPower, dim);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityRadiantFlux, dim);

    // Spectral Power
    dim = AddDimensionalityToLibrary(2, 1, 1, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantitySpectralPower, dim);

    // Volume Power Density
    dim = AddDimensionalityToLibrary(2, 3, 1, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityVolumePowerDensity, dim);

    // Specific Power
    dim = AddDimensionalityToLibrary(2, 0, 1, 1, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantitySpecificPower, dim);

    // Electric Charge, Amount of Electricity
    dim = AddDimensionalityToLibrary(0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityElectricCharge, dim);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityAmountOfElectricity, dim);

    //  Electric Dipole Moment                  length     mass        time        current   temperature  amount    luminous intensity
    dim = AddDimensionalityToLibrary(1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityElectricDipoleMoment, dim);

    //  Gyromagnetic Ratio                      length     mass        time        current   temperature  amount    luminous intensity
    dim = AddDimensionalityToLibrary(1, 1, 0, 1, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityGyromagneticRatio, dim);

    // Electric Potential Difference, Electromotive Force
    dim = AddDimensionalityToLibrary(2, 0, 1, 0, 0, 3, 0, 1, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityElectricPotentialDifference, dim);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityElectromotiveForce, dim);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityVoltage, dim);

    // Electrical Mobility
    dim = AddDimensionalityToLibrary(2, 2, 0, 1, 3, 1, 1, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityElectricalMobility, dim);

    // Electric Field Gradient
    dim = AddDimensionalityToLibrary(2, 2, 1, 0, 0, 3, 0, 1, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityElectricFieldGradient, dim);

    // Capacitance
    dim = AddDimensionalityToLibrary(0, 2, 0, 1, 4, 0, 2, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityCapacitance, dim);

    // Electric Resistance
    dim = AddDimensionalityToLibrary(2, 0, 1, 0, 0, 3, 0, 2, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityElectricResistance, dim);

    // Electric Resistance per length
    dim = AddDimensionalityToLibrary(2, 1, 1, 0, 0, 3, 0, 2, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityElectricResistancePerLength, dim);

    // Electric Resistivity
    dim = AddDimensionalityToLibrary(3, 0, 1, 0, 0, 3, 0, 2, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityElectricResistivity, dim);

    // Electric Conductance
    dim = AddDimensionalityToLibrary(0, 2, 0, 1, 3, 0, 2, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityElectricConductance, dim);

    // Electric Conductivity
    dim = AddDimensionalityToLibrary(0, 3, 0, 1, 3, 0, 2, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityElectricConductivity, dim);

    // Molar Conductivity
    dim = AddDimensionalityToLibrary(2, 2, 0, 1, 3, 0, 2, 0, 0, 0, 0, 1, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityMolarConductivity, dim);

    //  Magnetic Dipole Moment                  length     mass        time        current   temperature  amount    luminous intensity
    dim = AddDimensionalityToLibrary(2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityMagneticDipoleMoment, dim);

    //  Magnetic Dipole Moment Ratio             length     mass        time        current   temperature  amount    luminous intensity
    dim = AddDimensionalityToLibrary(2, 2, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityMagneticDipoleMomentRatio, dim);

    // Magnetic Flux
    dim = AddDimensionalityToLibrary(2, 0, 1, 0, 0, 2, 0, 1, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityMagneticFlux, dim);

    // Magnetic Flux Density
    dim = AddDimensionalityToLibrary(0, 0, 1, 0, 0, 2, 0, 1, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityMagneticFluxDensity, dim);

    // Inverse Magnetic Flux Density
    dim = AddDimensionalityToLibrary(0, 0, 0, 1, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityInverseMagneticFluxDensity, dim);

    // Frequency per Magnetic Flux Density, Charge to Mass Ratio, Radiation Exposure (x- and gamma-rays)
    dim = AddDimensionalityToLibrary(0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityRadiationExposure, dim);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityChargeToMassRatio, dim);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityFrequencyPerMagneticFluxDensity, dim);

    // Mass to Charge Ratio                     length     mass       time      current   temperature  amount    luminous intensity
    dim = AddDimensionalityToLibrary(0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityMassToChargeRatio, dim);

    // Magnetic Field Gradient                  length     mass        time        current   temperature  amount    luminous intensity
    dim = AddDimensionalityToLibrary(0, 1, 1, 0, 0, 2, 0, 1, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityMagneticFieldGradient, dim);

    // Inductance
    dim = AddDimensionalityToLibrary(2, 0, 1, 0, 0, 2, 0, 2, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityInductance, dim);

    // Luminous Flux
    dim = AddDimensionalityToLibrary(2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityLuminousFlux, dim);

    // Luminous Flux Density
    dim = AddDimensionalityToLibrary(2, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityLuminousFluxDensity, dim);

    // Luminous Energy
    dim = AddDimensionalityToLibrary(2, 2, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityLuminousEnergy, dim);

    // Illuminance
    dim = AddDimensionalityToLibrary(2, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityIlluminance, dim);

    // Absorbed dose, Dose equivalent
    dim = AddDimensionalityToLibrary(2, 0, 1, 1, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityAbsorbedDose, dim);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityDoseEquivalent, dim);

    // Catalytic Activity
    dim = AddDimensionalityToLibrary(0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityCatalyticActivity, dim);

    // Catalytic Activity Concentration
    dim = AddDimensionalityToLibrary(0, 3, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityCatalyticActivityConcentration, dim);

    // Catalytic Activity Content
    dim = AddDimensionalityToLibrary(0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityCatalyticActivityContent, dim);

    // Table 6 - Non-SI units but SI accepted

    // Reduced Action
    dim = AddDimensionalityToLibrary(3, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityReducedAction, dim);

    // Action, Angular Momentum
    dim = AddDimensionalityToLibrary(2, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityAction, dim);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityAngularMomentum, dim);

    // Kinematic Viscosity
    dim = AddDimensionalityToLibrary(2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityKinematicViscosity, dim);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityDiffusionCoefficient, dim);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityCirculation, dim);

    // amount concentration                     length     mass       time      current   temperature  amount    luminous intensity
    dim = AddDimensionalityToLibrary(0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityAmountConcentration, dim);

    // mass concentration
    dim = AddDimensionalityToLibrary(0, 3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityMassConcentration, dim);

    // molar mass
    dim = AddDimensionalityToLibrary(0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityMolarMass, dim);

    // molality
    dim = AddDimensionalityToLibrary(0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityMolality, dim);

    // molar magnetic susceptibility
    dim = AddDimensionalityToLibrary(3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityMolarMagneticSusceptibility, dim);

    // coulomb per mole
    dim = AddDimensionalityToLibrary(0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityChargePerAmount, dim);

    // cubic meters per kilogram second (Gravitational Constant)
    dim = AddDimensionalityToLibrary(3, 0, 0, 1, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityGravitationalConstant, dim);

    // distance per volume
    dim = AddDimensionalityToLibrary(1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityLengthPerVolume, dim);

    // volume per distance
    dim = AddDimensionalityToLibrary(3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityVolumePerLength, dim);

    // volume per time
    dim = AddDimensionalityToLibrary(3, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityVolumetricFlowRate, dim);

    // power per luminous flux
    dim = AddDimensionalityToLibrary(3, 1, 1, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 1);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityPowerPerLuminousFlux, dim);

    // luminous efficacy
    dim = AddDimensionalityToLibrary(0, 2, 0, 1, 3, 0, 0, 0, 0, 0, 0, 0, 1, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityLuminousEfficacy, dim);

    // Heat Transfer Coefficient                length     mass       time      current   temperature  amount    luminous intensity
    dim = AddDimensionalityToLibrary(0, 0, 1, 0, 0, 3, 0, 0, 0, 1, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityHeatTransferCoefficient, dim);

    // Stefan Boltzman constant dimensionality  length     mass       time      current   temperature  amount    luminous intensity
    dim = AddDimensionalityToLibrary(2, 2, 1, 0, 0, 3, 0, 0, 0, 4, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityPowerPerAreaPerTemperatureToFourthPower, dim);

    // Gas Permeance
    dim = AddDimensionalityToLibrary(1, 2, 0, 1, 2, 1, 0, 0, 0, 0, 1, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityGasPermeance, dim);

    // kSIQuantityFirstHyperPolarizability
    dim = AddDimensionalityToLibrary(3, 4, 0, 2, 7, 0, 3, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityFirstHyperPolarizability, dim);

    // kSIQuantitySecondHyperPolarizability
    dim = AddDimensionalityToLibrary(4, 6, 0, 3, 10, 0, 4, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantitySecondHyperPolarizability, dim);

    // Second Radiation Constant                length     mass       time      current   temperature  amount    luminous intensity
    dim = AddDimensionalityToLibrary(3, 2, 1, 1, 2, 2, 0, 0, 1, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantitySecondRadiationConstant, dim);

    // Wien Wavelength Displacement Constant    length     mass       time      current   temperature  amount    luminous intensity
    dim = AddDimensionalityToLibrary(1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityWavelengthDisplacementConstant, dim);

    // Fine Structure Constant                  length     mass       time      current   temperature  amount    luminous intensity
    dim = AddDimensionalityToLibrary(5, 5, 1, 1, 4, 4, 2, 2, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityFineStructureConstant, dim);

    // 1/(N•T)  kSIQuantityRatePerAmountConcentrationPerTime
    dim = AddDimensionalityToLibrary(3, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityRatePerAmountConcentrationPerTime, dim);
}

/*
 @function SIDimensionalityCopyDimensionalityLibrary
 @abstract Gets a copy of the dimLibrary of dimensionalities
 @result a CFSet containing the dimensionalities.
 */
static OCDictionaryRef SIDimensionalityCopyDimensionalityLibrary(void)
{
    if (NULL == dimLibrary)
        DimensionalityLibraryBuild();
    return OCDictionaryCreateCopy(dimLibrary);
}

SIDimensionalityRef SIDimensionalityForQuantity(OCStringRef quantity, OCStringRef *error)
{
    if (quantity == NULL)
        return NULL;
    if (NULL == dimQuantitiesLibrary)
        DimensionalityLibraryBuild();
    SIDimensionalityRef dimensionality = NULL;

    if (OCDictionaryContainsKey(dimQuantitiesLibrary, quantity))
    {
        dimensionality = OCDictionaryGetValue(dimQuantitiesLibrary, quantity);
        return dimensionality;
    }

    OCMutableStringRef lowerCaseQuantity = OCStringCreateMutableCopy(quantity);
    OCStringLowercase(lowerCaseQuantity);

    if (OCDictionaryContainsKey(dimQuantitiesLibrary, lowerCaseQuantity))
    {
        dimensionality = OCDictionaryGetValue(dimQuantitiesLibrary, lowerCaseQuantity);
    }
    OCRelease(lowerCaseQuantity);
    if (dimensionality == NULL)
    {
        dimensionality = SIDimensionalityForSymbol(quantity, error);
    }
    return dimensionality;
}

/*
 @function SIDimensionalityLibraryShowFull
 @abstract Shows every dimensionality in the dimLibrary
 */
// static void SIDimensionalityLibraryShowFull(void)
//{
//     if(NULL==dimLibrary) DimensionalityLibraryBuild();
//     OCDictionaryApplyFunction(dimLibrary,(OCDictionaryApplierFunction) SIDimensionalityShowFull,NULL);
// }

OCMutableDictionaryRef SIDimensionalityGetLibrary()
{
    if (NULL == dimLibrary)
        DimensionalityLibraryBuild();
    return dimLibrary;
}

void SIDimensionalitySetLibrary(OCMutableDictionaryRef newDimLibrary)
{
    if (newDimLibrary == dimLibrary)
        return;
    if (newDimLibrary)
    {
        if (dimLibrary)
            OCRelease(dimLibrary);
        dimLibrary = (OCMutableDictionaryRef)OCRetain(newDimLibrary);
    }
}

// OCArrayRef SIDimensionalityLibraryCreateArrayOfAllQuantities()
//{
//     if(NULL==dimQuantitiesLibrary) DimensionalityLibraryBuild();
//     uint64_t totalCount = OCDictionaryGetCount(dimQuantitiesLibrary);
//     OCStringRef keys[totalCount];
//     SIDimensionalityRef dimensionalities[totalCount];
//     OCDictionaryGetKeysAndValues(dimQuantitiesLibrary, (const void **) keys, (const void **)  dimensionalities);
//     OCArrayRef quantities = OCArrayCreate((void *) keys, totalCount);
//     OCMutableArrayRef sorted = OCArrayCreateMutableCopy(quantities);
//     OCArraySortValues(sorted, OCRangeMake(0, totalCount), (CFComparatorFunction)OCStringCompare, NULL);
//     OCRelease(quantities);
//     return sorted;
// }
//

#pragma mark Tests

bool SIDimensionalityEqual(SIDimensionalityRef input1, SIDimensionalityRef input2)
{
    IF_NO_OBJECT_EXISTS_RETURN(input1, false)
    IF_NO_OBJECT_EXISTS_RETURN(input2, false)

    if (NULL == input1)
        return false;
    if (NULL == input2)
        return false;

    if (input1 == input2)
        return true;

    for (int i = 0; i < BASE_DIMENSION_COUNT; i++)
    {
        if (input1->num_exp[i] != input2->num_exp[i])
            return false;
        if (input1->den_exp[i] != input2->den_exp[i])
            return false;
    }

    return true;
}

bool SIDimensionalityIsDimensionless(SIDimensionalityRef theDim)
{
    IF_NO_OBJECT_EXISTS_RETURN(theDim, false)

    for (SIBaseDimensionIndex index = 0; index < BASE_DIMENSION_COUNT; index++)
    {
        int theDim_exponent = theDim->num_exp[index] - theDim->den_exp[index];
        if (theDim_exponent != 0)
            return false;
    }
    return true;
}

bool SIDimensionalityIsDimensionlessAndNotDerived(SIDimensionalityRef theDim)
{
    // To be dimensionless and not derived all the numerator and denominator exponents must be 0
    IF_NO_OBJECT_EXISTS_RETURN(theDim, false)
    for (SIBaseDimensionIndex index = 0; index < BASE_DIMENSION_COUNT; index++)
    {
        if (theDim->num_exp[index] != 0)
            return false;
        if (theDim->den_exp[index] != 0)
            return false;
    }
    return true;
}

bool SIDimensionalityIsDimensionlessAndDerived(SIDimensionalityRef theDim)
{
    if (SIDimensionalityIsDimensionlessAndNotDerived(theDim))
        return false;
    if (SIDimensionalityIsDimensionless(theDim))
        return true;
    return false;
}

bool SIDimensionalityIsBaseDimensionality(SIDimensionalityRef theDim)
{
    IF_NO_OBJECT_EXISTS_RETURN(theDim, false)
    if (SIDimensionalityIsDimensionlessAndNotDerived(theDim))
        return false;
    // If it is base dimensionality, then all the denominator exponents must be 0
    // and all numerator exponents are zero except one, which is 1
    for (SIBaseDimensionIndex index = 0; index < BASE_DIMENSION_COUNT; index++)
        if (SIDimensionalityGetDenExpAtIndex(theDim, index) != 0)
            return false;
    int count = 0;
    for (SIBaseDimensionIndex index = 0; index < BASE_DIMENSION_COUNT; index++)
    {
        if (theDim->num_exp[index] > 1)
            return false;
        if (theDim->num_exp[index] == 1)
            count++;
    }
    if (count == 1)
        return true;
    return false;
}

bool SIDimensionalityIsDerived(SIDimensionalityRef theDim)
{
    IF_NO_OBJECT_EXISTS_RETURN(theDim, false)
    if (SIDimensionalityIsDimensionlessAndNotDerived(theDim))
        return false;
    if (SIDimensionalityIsBaseDimensionality(theDim))
        return false;
    return true;
}

bool SIDimensionalityHasSameReducedDimensionality(SIDimensionalityRef theDim1, SIDimensionalityRef theDim2)
{
    IF_NO_OBJECT_EXISTS_RETURN(theDim1, NULL)
    IF_NO_OBJECT_EXISTS_RETURN(theDim2, NULL)
    if (theDim1 == theDim2)
        return true;

    for (int i = 0; i < BASE_DIMENSION_COUNT; i++)
    {
        int theDim1_exponent = theDim1->num_exp[i] - theDim1->den_exp[i];
        int theDim2_exponent = theDim2->num_exp[i] - theDim2->den_exp[i];
        if (theDim1_exponent != theDim2_exponent)
            return false;
    }
    return true;
}

static bool SIDimensionalityHasExponents(SIDimensionalityRef theDim,
                                         uint8_t length_num_exp, uint8_t length_den_exp,
                                         uint8_t mass_num_exp, uint8_t mass_den_exp,
                                         uint8_t time_num_exp, uint8_t time_den_exp,
                                         uint8_t current_num_exp, uint8_t current_den_exp,
                                         uint8_t temperature_num_exp, uint8_t temperature_den_exp,
                                         uint8_t amount_num_exp, uint8_t amount_den_exp,
                                         uint8_t luminous_intensity_num_exp, uint8_t luminous_intensity_den_exp)
{
    if (theDim == NULL)
        theDim = SIDimensionalityDimensionless();

    if (SIDimensionalityGetNumExpAtIndex(theDim, kSILengthIndex) != length_num_exp)
        return false;
    if (SIDimensionalityGetNumExpAtIndex(theDim, kSIMassIndex) != mass_num_exp)
        return false;
    if (SIDimensionalityGetNumExpAtIndex(theDim, kSITimeIndex) != time_num_exp)
        return false;
    if (SIDimensionalityGetNumExpAtIndex(theDim, kSICurrentIndex) != current_num_exp)
        return false;
    if (SIDimensionalityGetNumExpAtIndex(theDim, kSITemperatureIndex) != temperature_num_exp)
        return false;
    if (SIDimensionalityGetNumExpAtIndex(theDim, kSIAmountIndex) != amount_num_exp)
        return false;
    if (SIDimensionalityGetNumExpAtIndex(theDim, kSILuminousIntensityIndex) != luminous_intensity_num_exp)
        return false;

    if (SIDimensionalityGetDenExpAtIndex(theDim, kSILengthIndex) != length_den_exp)
        return false;
    if (SIDimensionalityGetDenExpAtIndex(theDim, kSIMassIndex) != mass_den_exp)
        return false;
    if (SIDimensionalityGetDenExpAtIndex(theDim, kSITimeIndex) != time_den_exp)
        return false;
    if (SIDimensionalityGetDenExpAtIndex(theDim, kSICurrentIndex) != current_den_exp)
        return false;
    if (SIDimensionalityGetDenExpAtIndex(theDim, kSITemperatureIndex) != temperature_den_exp)
        return false;
    if (SIDimensionalityGetDenExpAtIndex(theDim, kSIAmountIndex) != amount_den_exp)
        return false;
    if (SIDimensionalityGetDenExpAtIndex(theDim, kSILuminousIntensityIndex) != luminous_intensity_den_exp)
        return false;
    return true;
}

bool SIDimensionalityHasReducedExponents(SIDimensionalityRef theDim,
                                         int8_t length_exponent,
                                         int8_t mass_exponent,
                                         int8_t time_exponent,
                                         int8_t current_exponent,
                                         int8_t temperature_exponent,
                                         int8_t amount_exponent,
                                         int8_t luminous_intensity_exponent)
{
    IF_NO_OBJECT_EXISTS_RETURN(theDim, NULL)

    if (SIDimensionalityReducedExponentAtIndex(theDim, kSILengthIndex) != length_exponent)
        return false;
    if (SIDimensionalityReducedExponentAtIndex(theDim, kSIMassIndex) != mass_exponent)
        return false;
    if (SIDimensionalityReducedExponentAtIndex(theDim, kSITimeIndex) != time_exponent)
        return false;
    if (SIDimensionalityReducedExponentAtIndex(theDim, kSICurrentIndex) != current_exponent)
        return false;
    if (SIDimensionalityReducedExponentAtIndex(theDim, kSITemperatureIndex) != temperature_exponent)
        return false;
    if (SIDimensionalityReducedExponentAtIndex(theDim, kSIAmountIndex) != amount_exponent)
        return false;
    if (SIDimensionalityReducedExponentAtIndex(theDim, kSILuminousIntensityIndex) != luminous_intensity_exponent)
        return false;
    return true;
}

/*
 @function SIDimensionalityHasSameDimensionlessAndDerivedDimensionalities
 @abstract Determines if the two Dimensionalities have the same dimensionless exponents,
 @param theDim1 The first Dimensionality.
 @param theDim2 The second Dimensionality.
 @result true or false.
 */
static bool SIDimensionalityHasSameDimensionlessAndDerivedDimensionalities(SIDimensionalityRef theDim1, SIDimensionalityRef theDim2)
{
    IF_NO_OBJECT_EXISTS_RETURN(theDim1, NULL)
    IF_NO_OBJECT_EXISTS_RETURN(theDim2, NULL)
    if (!SIDimensionalityIsDimensionlessAndDerived(theDim1))
        return false;
    if (!SIDimensionalityIsDimensionlessAndDerived(theDim2))
        return false;

    if (!SIDimensionalityEqual(theDim1, theDim2))
        return false;
    return true;
}

// Add a cleanup function for static dictionaries
void cleanupDimensionalityLibraries(void) {
    if (dimQuantitiesLibrary) {
        OCRelease(dimQuantitiesLibrary);
        dimQuantitiesLibrary = NULL;
    }
    // Release the dimensionality library after dimQuantitiesLibrary.
    if (dimLibrary) {
        OCArrayRef values = OCDictionaryCreateArrayWithAllValues((OCDictionaryRef)dimLibrary);
        if (values) {
            for (int i = 0; i < OCArrayGetCount(values); i++) {
                SIDimensionalityRef dim = OCArrayGetValueAtIndex(values, i);
                OCTypeSetStaticInstance(dim, false);
                OCRetain(dim);
            }
            OCRelease(values);
        }
        OCRelease(dimLibrary);
        dimLibrary = NULL;
    }

}

// Register cleanupDimensionalityLibraries with atexit
__attribute__((constructor))
static void registerDimensionalityCleanup(void) {
    atexit(cleanupDimensionalityLibraries);
}
