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
    uint8_t numerator_exponent[BASE_DIMENSION_COUNT];
    uint8_t denominator_exponent[BASE_DIMENSION_COUNT];
    OCStringRef symbol;
};

bool __SIDimensionalityEqual(const void *theType1, const void *theType2)
{
    SIDimensionalityRef theDimensionality1 = (SIDimensionalityRef)theType1;
    SIDimensionalityRef theDimensionality2 = (SIDimensionalityRef)theType2;
    if (theDimensionality1->_base.typeID != theDimensionality2->_base.typeID)
        return false;

    if (NULL == theDimensionality1 || NULL == theDimensionality2)
        return false;
    if (theDimensionality1 == theDimensionality2)
        return true;
    for (int i = 0; i < BASE_DIMENSION_COUNT; i++)
    {
        if (theDimensionality1->numerator_exponent[i] != theDimensionality2->numerator_exponent[i])
            return false;
        if (theDimensionality1->denominator_exponent[i] != theDimensionality2->denominator_exponent[i])
            return false;
    }

    return true;
}

void __SIDimensionalityFinalize(const void *theType)
{
    if (NULL == theType)
        return;
    SIDimensionalityRef theDimensionality = (SIDimensionalityRef)theType;
    free((void *)theDimensionality);
}

static OCStringRef __SIDimensionalityCopyFormattingDescription(OCTypeRef theType)
{
    SIDimensionalityRef theDimensionality = (SIDimensionalityRef)theType;
    return (OCStringRef)OCStringCreateCopy(theDimensionality->symbol);
}

OCTypeID SIDimensionalityGetTypeID(void)
{
    if (kSIDimensionalityID == _kOCNotATypeID)
        kSIDimensionalityID = OCRegisterType("SIDimensionality");
    return kSIDimensionalityID;
}

static struct __SIDimensionality *SIDimensionalityAllocate()
{
    struct __SIDimensionality *theDimensionality = malloc(sizeof(struct __SIDimensionality));
    if (NULL == theDimensionality)
        return NULL;
    theDimensionality->_base.typeID = SIDimensionalityGetTypeID();
    theDimensionality->_base.retainCount = 1;
    theDimensionality->_base.finalize = __SIDimensionalityFinalize;
    theDimensionality->_base.equal = __SIDimensionalityEqual;
    theDimensionality->_base.copyFormattingDesc = __SIDimensionalityCopyFormattingDescription;
    theDimensionality->symbol = NULL;
    return theDimensionality;
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

static uint8_t getExponentAtIndex(SIDimensionalityRef dimensionality, SIBaseDimensionIndex index, bool isNumerator)
{
    if (dimensionality == NULL)
        return 0;
    return isNumerator ? dimensionality->numerator_exponent[index] : dimensionality->denominator_exponent[index];
}

uint8_t SIDimensionalityGetNumeratorExponentAtIndex(SIDimensionalityRef theDimensionality, SIBaseDimensionIndex index)
{
    return getExponentAtIndex(theDimensionality, index, true);
}

uint8_t SIDimensionalityGetDenominatorExponentAtIndex(SIDimensionalityRef theDimensionality, SIBaseDimensionIndex index)
{
    return getExponentAtIndex(theDimensionality, index, false);
}

// dimensionalityLibrary is a Singleton
OCMutableDictionaryRef dimensionalityLibrary = NULL;
OCMutableDictionaryRef dimensionalityQuantitiesLibrary = NULL;
static void DimensionalityLibraryBuild();

#pragma mark Designated Creator

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

static OCStringRef SIDimensionalityCreateSymbol(SIDimensionalityRef theDimensionality)
{
    if (NULL == theDimensionality)
        return NULL;

    OCMutableStringRef numerator = OCStringCreateMutable(0);
    OCMutableStringRef denominator = OCStringCreateMutable(0);
    bool denominator_multiple_dimensions = false;

    for (uint8_t index = 0; index < BASE_DIMENSION_COUNT; index++)
    {
        appendDimensionSymbol(numerator, theDimensionality->numerator_exponent[index], baseDimensionSymbol(index));
        appendDimensionSymbol(denominator, theDimensionality->denominator_exponent[index], baseDimensionSymbol(index));
    }

    if (OCStringGetLength(numerator) != 0)
    {
        if (OCStringGetLength(denominator) != 0)
        {
            OCStringRef symbol;
            if (denominator_multiple_dimensions)
            {
                symbol = OCStringCreateWithFormat(STR("%@/(%@)"), numerator, denominator);
            }
            else
            {
                symbol = OCStringCreateWithFormat(STR("%@/%@"), numerator, denominator);
            }
            OCRelease(numerator);
            OCRelease(denominator);
            return symbol;
        }
        else
        {
            OCRelease(denominator);
            return numerator;
        }
    }
    else
    {
        if (OCStringGetLength(denominator) != 0)
        {
            OCStringRef symbol;
            if (denominator_multiple_dimensions)
            {
                symbol = OCStringCreateWithFormat(STR("1/(%@)"), denominator);
            }
            else
            {
                symbol = OCStringCreateWithFormat(STR("1/%@"), denominator);
            }
            OCRelease(numerator);
            OCRelease(denominator);
            return symbol;
        }
        else
        {
            return STR("1");
        }
    }
}

/*
 @function SIDimensionalityCreate
 @abstract Creates a SIDimensionality.
 @param length_numerator_exponent integer numerator exponent for length dimension
 @param length_denominator_exponent integer denominator exponent for length dimension
 @param mass_numerator_exponent integer numerator exponent for mass dimension
 @param mass_denominator_exponent integer denominator exponent for mass dimension
 @param time_numerator_exponent integer numerator exponent for time dimension
 @param time_denominator_exponent integer denominator exponent for time dimension
 @param current_numerator_exponent integer numerator exponent for current dimension
 @param current_denominator_exponent integer denominator exponent for current dimension
 @param temperature_numerator_exponent integer numerator exponent for temperature dimension
 @param temperature_denominator_exponent integer denominator exponent for temperature dimension
 @param amount_numerator_exponent integer numerator exponent for amount dimension
 @param amount_denominator_exponent integer denominator exponent for amount dimension
 @param luminous_intensity_numerator_exponent integer numerator exponent for luminous intensity dimension
 @param luminous_intensity_denominator_exponent integer denominator exponent for luminous intensity dimension
 @result SIDimensionality object
 */
static SIDimensionalityRef SIDimensionalityCreate(uint8_t length_numerator_exponent, uint8_t length_denominator_exponent,
                                                  uint8_t mass_numerator_exponent, uint8_t mass_denominator_exponent,
                                                  uint8_t time_numerator_exponent, uint8_t time_denominator_exponent,
                                                  uint8_t current_numerator_exponent, uint8_t current_denominator_exponent,
                                                  uint8_t temperature_numerator_exponent, uint8_t temperature_denominator_exponent,
                                                  uint8_t amount_numerator_exponent, uint8_t amount_denominator_exponent,
                                                  uint8_t luminous_intensity_numerator_exponent, uint8_t luminous_intensity_denominator_exponent)
{
    // Initialize object
    struct __SIDimensionality *theDimensionality = SIDimensionalityAllocate();
    if (NULL == theDimensionality)
        return NULL;

    //  setup attributes
    theDimensionality->numerator_exponent[kSILengthIndex] = length_numerator_exponent;
    theDimensionality->denominator_exponent[kSILengthIndex] = length_denominator_exponent;
    theDimensionality->numerator_exponent[kSIMassIndex] = mass_numerator_exponent;
    theDimensionality->denominator_exponent[kSIMassIndex] = mass_denominator_exponent;
    theDimensionality->numerator_exponent[kSITimeIndex] = time_numerator_exponent;
    theDimensionality->denominator_exponent[kSITimeIndex] = time_denominator_exponent;
    theDimensionality->numerator_exponent[kSICurrentIndex] = current_numerator_exponent;
    theDimensionality->denominator_exponent[kSICurrentIndex] = current_denominator_exponent;
    theDimensionality->numerator_exponent[kSITemperatureIndex] = temperature_numerator_exponent;
    theDimensionality->denominator_exponent[kSITemperatureIndex] = temperature_denominator_exponent;
    theDimensionality->numerator_exponent[kSIAmountIndex] = amount_numerator_exponent;
    theDimensionality->denominator_exponent[kSIAmountIndex] = amount_denominator_exponent;
    theDimensionality->numerator_exponent[kSILuminousIntensityIndex] = luminous_intensity_numerator_exponent;
    theDimensionality->denominator_exponent[kSILuminousIntensityIndex] = luminous_intensity_denominator_exponent;

    theDimensionality->symbol = SIDimensionalityCreateSymbol(theDimensionality);

    return (SIDimensionalityRef)theDimensionality;
}

#pragma mark Accessors

OCStringRef SIDimensionalityGetSymbol(SIDimensionalityRef theDimensionality)
{
    return theDimensionality->symbol;
}

/*
 @function SIDimensionalityReducedExponentAtIndex
 @abstract Returns the exponent for the dimension at Index.
 @param theDimensionality The Dimensionality.
 @result the integer exponent (numerator-denominator).
 @discussion base units length, mass, time, current, temperature, amount, and luminous intensity are
 assigned to index constants kSILengthIndex, kSIMassIndex, kSITimeIndex, kSICurrentIndex,  kSITemperatureIndex,
 kSIAmountIndex, kSILuminousIntensityIndex, respectively.
 */
int8_t SIDimensionalityReducedExponentAtIndex(SIDimensionalityRef theDimensionality, SIBaseDimensionIndex index)
{
    IF_NO_OBJECT_EXISTS_RETURN(theDimensionality, 0)

    if (theDimensionality)
        return theDimensionality->numerator_exponent[index] - theDimensionality->denominator_exponent[index];
    return 0;
}

#pragma mark Operations

static SIDimensionalityRef SIDimensionalityWithExponents(uint8_t length_numerator_exponent, uint8_t length_denominator_exponent,
                                                         uint8_t mass_numerator_exponent, uint8_t mass_denominator_exponent,
                                                         uint8_t time_numerator_exponent, uint8_t time_denominator_exponent,
                                                         uint8_t current_numerator_exponent, uint8_t current_denominator_exponent,
                                                         uint8_t temperature_numerator_exponent, uint8_t temperature_denominator_exponent,
                                                         uint8_t amount_numerator_exponent, uint8_t amount_denominator_exponent,
                                                         uint8_t luminous_intensity_numerator_exponent, uint8_t luminous_intensity_denominator_exponent)
{
    SIDimensionalityRef newDimensionality = SIDimensionalityCreate(length_numerator_exponent, length_denominator_exponent,
                                                                   mass_numerator_exponent, mass_denominator_exponent,
                                                                   time_numerator_exponent, time_denominator_exponent,
                                                                   current_numerator_exponent, current_denominator_exponent,
                                                                   temperature_numerator_exponent, temperature_denominator_exponent,
                                                                   amount_numerator_exponent, amount_denominator_exponent,
                                                                   luminous_intensity_numerator_exponent, luminous_intensity_denominator_exponent);


    if (NULL == newDimensionality)
        return NULL;

    if (NULL == dimensionalityLibrary)
        DimensionalityLibraryBuild();

    if (OCDictionaryContainsKey(dimensionalityLibrary, newDimensionality->symbol))
    {
        SIDimensionalityRef existingDimensionality = OCDictionaryGetValue(dimensionalityLibrary, newDimensionality->symbol);

        OCRelease(newDimensionality);
        return existingDimensionality;
    }

    struct __SIDimensionality *temp = (struct __SIDimensionality *)newDimensionality;
    temp->_base.retainCount = 0;
    OCDictionaryAddValue(dimensionalityLibrary, newDimensionality->symbol, newDimensionality);
    OCRelease(newDimensionality);
    return newDimensionality;
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

SIDimensionalityRef SIDimensionalityByReducing(SIDimensionalityRef theDimensionality)
{
    IF_NO_OBJECT_EXISTS_RETURN(theDimensionality, NULL)

    uint8_t numerator_exponent[BASE_DIMENSION_COUNT];
    uint8_t denominator_exponent[BASE_DIMENSION_COUNT];

    for (int8_t i = 0; i < BASE_DIMENSION_COUNT; i++)
    {
        int power = theDimensionality->numerator_exponent[i] - theDimensionality->denominator_exponent[i];
        if (power > 0)
        {
            numerator_exponent[i] = power;
            denominator_exponent[i] = 0;
        }
        else if (power < 0)
        {
            denominator_exponent[i] = -power;
            numerator_exponent[i] = 0;
        }
        else
            denominator_exponent[i] = numerator_exponent[i] = 0;
    }
    return SIDimensionalityWithExponents(numerator_exponent[kSILengthIndex], denominator_exponent[kSILengthIndex],
                                         numerator_exponent[kSIMassIndex], denominator_exponent[kSIMassIndex],
                                         numerator_exponent[kSITimeIndex], denominator_exponent[kSITimeIndex],
                                         numerator_exponent[kSICurrentIndex], denominator_exponent[kSICurrentIndex],
                                         numerator_exponent[kSITemperatureIndex], denominator_exponent[kSITemperatureIndex],
                                         numerator_exponent[kSIAmountIndex], denominator_exponent[kSIAmountIndex],
                                         numerator_exponent[kSILuminousIntensityIndex], denominator_exponent[kSILuminousIntensityIndex]);
}

SIDimensionalityRef SIDimensionalityByTakingNthRoot(SIDimensionalityRef theDimensionality, uint8_t root, OCStringRef *error)
{
    if (error)
        if (*error)
            return NULL;

    IF_NO_OBJECT_EXISTS_RETURN(theDimensionality, NULL)

    uint8_t numerator_exponent[BASE_DIMENSION_COUNT];
    uint8_t denominator_exponent[BASE_DIMENSION_COUNT];

    SIDimensionalityRef reducedDimensionality = SIDimensionalityByReducing(theDimensionality);

    for (int8_t i = 0; i < BASE_DIMENSION_COUNT; i++)
    {
        if (reducedDimensionality->numerator_exponent[i] % root != 0 || reducedDimensionality->denominator_exponent[i] % root != 0)
        {
            if (error)
            {
                *error = STR("Can't raise physical dimensionality to a non-integer power.");
            }
            return NULL;
        }
        numerator_exponent[i] = theDimensionality->numerator_exponent[i] / root;
        denominator_exponent[i] = theDimensionality->denominator_exponent[i] / root;
    }

    return SIDimensionalityWithExponents(numerator_exponent[kSILengthIndex], denominator_exponent[kSILengthIndex],
                                         numerator_exponent[kSIMassIndex], denominator_exponent[kSIMassIndex],
                                         numerator_exponent[kSITimeIndex], denominator_exponent[kSITimeIndex],
                                         numerator_exponent[kSICurrentIndex], denominator_exponent[kSICurrentIndex],
                                         numerator_exponent[kSITemperatureIndex], denominator_exponent[kSITemperatureIndex],
                                         numerator_exponent[kSIAmountIndex], denominator_exponent[kSIAmountIndex],
                                         numerator_exponent[kSILuminousIntensityIndex], denominator_exponent[kSILuminousIntensityIndex]);
}

SIDimensionalityRef SIDimensionalityByDividingWithoutReducing(SIDimensionalityRef theDimensionality1, SIDimensionalityRef theDimensionality2)
{
    /*
     *	This routine will create an derived SI Dimensionality formed by the division of theDimensionality1 by theDimensionality2.
     *	It will additionally return a multiplier for the numerical part of the quantity product
     *
     */
    IF_NO_OBJECT_EXISTS_RETURN(theDimensionality1, NULL)
    IF_NO_OBJECT_EXISTS_RETURN(theDimensionality2, NULL)

    uint8_t numerator_exponent[BASE_DIMENSION_COUNT];
    uint8_t denominator_exponent[BASE_DIMENSION_COUNT];
    for (uint8_t i = 0; i < BASE_DIMENSION_COUNT; i++)
    {
        numerator_exponent[i] = theDimensionality1->numerator_exponent[i] + theDimensionality2->denominator_exponent[i];
        denominator_exponent[i] = theDimensionality1->denominator_exponent[i] + theDimensionality2->numerator_exponent[i];
    }
    return SIDimensionalityWithExponents(numerator_exponent[kSILengthIndex], denominator_exponent[kSILengthIndex],
                                         numerator_exponent[kSIMassIndex], denominator_exponent[kSIMassIndex],
                                         numerator_exponent[kSITimeIndex], denominator_exponent[kSITimeIndex],
                                         numerator_exponent[kSICurrentIndex], denominator_exponent[kSICurrentIndex],
                                         numerator_exponent[kSITemperatureIndex], denominator_exponent[kSITemperatureIndex],
                                         numerator_exponent[kSIAmountIndex], denominator_exponent[kSIAmountIndex],
                                         numerator_exponent[kSILuminousIntensityIndex], denominator_exponent[kSILuminousIntensityIndex]);
}

SIDimensionalityRef SIDimensionalityByDividing(SIDimensionalityRef theDimensionality1, SIDimensionalityRef theDimensionality2)
{
    return SIDimensionalityByReducing(SIDimensionalityByDividingWithoutReducing(theDimensionality1, theDimensionality2));
}

SIDimensionalityRef SIDimensionalityByRaisingToAPowerWithoutReducing(SIDimensionalityRef theDimensionality, double power, OCStringRef *error)
{
    /*
     *	This routine will create an derived SI Dimensionality formed by the raising theDimensionality to a power.
     *	It will additionally return a multiplier for the numerical part of the quantity product
     *
     */

    if (error)
        if (*error)
            return NULL;
    IF_NO_OBJECT_EXISTS_RETURN(theDimensionality, NULL)

    int32_t pow = (int32_t)floor(power);
    double fraction = power - pow;
    if (OCCompareDoubleValues(fraction, 0.0) != kOCCompareEqualTo)
    {
        int32_t root = (int32_t)floor(1. / power);
        fraction = 1. / power - root;
        if (OCCompareDoubleValues(fraction, 0.0) == kOCCompareEqualTo)
            return SIDimensionalityByTakingNthRoot(theDimensionality, root, error);
        else
        {
            if (error)
            {
                *error = STR("Can't raise physical dimensionality to a non-integer power.");
            }
            return NULL;
        }
    }

    uint8_t numerator_exponent[BASE_DIMENSION_COUNT];
    uint8_t denominator_exponent[BASE_DIMENSION_COUNT];
    for (uint8_t i = 0; i < BASE_DIMENSION_COUNT; i++)
    {
        if (pow > 0)
        {
            numerator_exponent[i] = theDimensionality->numerator_exponent[i] * pow;
            denominator_exponent[i] = theDimensionality->denominator_exponent[i] * pow;
        }
        else
        {
            numerator_exponent[i] = theDimensionality->denominator_exponent[i] * (-pow);
            denominator_exponent[i] = theDimensionality->numerator_exponent[i] * (-pow);
        }
    }
    return SIDimensionalityWithExponents(numerator_exponent[kSILengthIndex], denominator_exponent[kSILengthIndex],
                                         numerator_exponent[kSIMassIndex], denominator_exponent[kSIMassIndex],
                                         numerator_exponent[kSITimeIndex], denominator_exponent[kSITimeIndex],
                                         numerator_exponent[kSICurrentIndex], denominator_exponent[kSICurrentIndex],
                                         numerator_exponent[kSITemperatureIndex], denominator_exponent[kSITemperatureIndex],
                                         numerator_exponent[kSIAmountIndex], denominator_exponent[kSIAmountIndex],
                                         numerator_exponent[kSILuminousIntensityIndex], denominator_exponent[kSILuminousIntensityIndex]);
}

SIDimensionalityRef SIDimensionalityByRaisingToAPower(SIDimensionalityRef theDimensionality, double power, OCStringRef *error)
{
    if (error)
        if (*error)
            return NULL;
    return SIDimensionalityByReducing(SIDimensionalityByRaisingToAPowerWithoutReducing(theDimensionality, power, error));
}

SIDimensionalityRef SIDimensionalityByMultiplyingWithoutReducing(SIDimensionalityRef theDimensionality1, SIDimensionalityRef theDimensionality2, OCStringRef *error)
{
    /*
     *	This routine will create an derived SI Dimensionality formed by the product of two Dimensionalities.
     *	It will additionally return a multiplier for the numerical part of the quantity product
     *
     */
    if (error)
        if (*error)
            return NULL;
    IF_NO_OBJECT_EXISTS_RETURN(theDimensionality1, NULL)
    IF_NO_OBJECT_EXISTS_RETURN(theDimensionality2, NULL)
    if (theDimensionality1 == theDimensionality2)
        return SIDimensionalityByRaisingToAPowerWithoutReducing(theDimensionality1, 2, error);

    uint8_t numerator_exponent[BASE_DIMENSION_COUNT];
    uint8_t denominator_exponent[BASE_DIMENSION_COUNT];
    for (uint8_t i = 0; i < BASE_DIMENSION_COUNT; i++)
    {
        numerator_exponent[i] = theDimensionality1->numerator_exponent[i] + theDimensionality2->numerator_exponent[i];
        denominator_exponent[i] = theDimensionality1->denominator_exponent[i] + theDimensionality2->denominator_exponent[i];
    }
    return SIDimensionalityWithExponents(numerator_exponent[kSILengthIndex], denominator_exponent[kSILengthIndex],
                                         numerator_exponent[kSIMassIndex], denominator_exponent[kSIMassIndex],
                                         numerator_exponent[kSITimeIndex], denominator_exponent[kSITimeIndex],
                                         numerator_exponent[kSICurrentIndex], denominator_exponent[kSICurrentIndex],
                                         numerator_exponent[kSITemperatureIndex], denominator_exponent[kSITemperatureIndex],
                                         numerator_exponent[kSIAmountIndex], denominator_exponent[kSIAmountIndex],
                                         numerator_exponent[kSILuminousIntensityIndex], denominator_exponent[kSILuminousIntensityIndex]);
}

SIDimensionalityRef SIDimensionalityByMultiplying(SIDimensionalityRef theDimensionality1, SIDimensionalityRef theDimensionality2, OCStringRef *error)
{
    if (error)
        if (*error)
            return NULL;
    return SIDimensionalityByReducing(SIDimensionalityByMultiplyingWithoutReducing(theDimensionality1, theDimensionality2, error));
}

OCArrayRef SIDimensionalityCreateArrayOfQuantities(SIDimensionalityRef theDimensionality)
{
    IF_NO_OBJECT_EXISTS_RETURN(theDimensionality, NULL)

    if (NULL == dimensionalityQuantitiesLibrary)
        DimensionalityLibraryBuild();
    uint64_t count = OCDictionaryGetCountOfValue(dimensionalityQuantitiesLibrary, theDimensionality);
    if (0 == count)
        return NULL;
    OCStringRef quantities[count];

    uint64_t totalCount = OCDictionaryGetCount(dimensionalityQuantitiesLibrary);
    OCStringRef keys[totalCount];
    SIDimensionalityRef dimensionalities[totalCount];

    OCDictionaryGetKeysAndValues(dimensionalityQuantitiesLibrary, (const void **)keys, (const void **)dimensionalities);
    uint64_t i = 0;
    for (uint64_t index = 0; index < totalCount; index++)
    {
        if (__SIDimensionalityEqual(dimensionalities[index], theDimensionality))
        {
            quantities[i++] = keys[index];
        }
    }

    OCArrayRef result = NULL;
    if (i == count)
        result = OCArrayCreate((const void **)quantities, count, &kOCTypeArrayCallBacks);
    return result;
}

OCArrayRef SIDimensionalityCreateArrayWithSameReducedDimensionality(SIDimensionalityRef theDimensionality)
{
    IF_NO_OBJECT_EXISTS_RETURN(theDimensionality, NULL)
    if (NULL == dimensionalityQuantitiesLibrary)
        DimensionalityLibraryBuild();

    int8_t reducedLength = theDimensionality->numerator_exponent[kSILengthIndex] - theDimensionality->denominator_exponent[kSILengthIndex];
    int8_t largestLength = theDimensionality->numerator_exponent[kSILengthIndex];
    if (reducedLength < 0)
        largestLength = theDimensionality->denominator_exponent[kSILengthIndex];

    int8_t reducedMass = theDimensionality->numerator_exponent[kSIMassIndex] - theDimensionality->denominator_exponent[kSIMassIndex];
    int8_t largestMass = theDimensionality->numerator_exponent[kSIMassIndex];
    if (reducedMass < 0)
        largestMass = theDimensionality->denominator_exponent[kSIMassIndex];

    int8_t reducedTime = theDimensionality->numerator_exponent[kSITimeIndex] - theDimensionality->denominator_exponent[kSITimeIndex];
    int8_t largestTime = theDimensionality->numerator_exponent[kSITimeIndex];
    if (reducedTime < 0)
        largestTime = theDimensionality->denominator_exponent[kSITimeIndex];

    int8_t reducedCurrent = theDimensionality->numerator_exponent[kSICurrentIndex] - theDimensionality->denominator_exponent[kSICurrentIndex];
    int8_t largestCurrent = theDimensionality->numerator_exponent[kSICurrentIndex];
    if (reducedCurrent < 0)
        largestCurrent = theDimensionality->denominator_exponent[kSICurrentIndex];

    int8_t reducedTemperature = theDimensionality->numerator_exponent[kSITemperatureIndex] - theDimensionality->denominator_exponent[kSITemperatureIndex];
    int8_t largestTemperature = theDimensionality->numerator_exponent[kSITemperatureIndex];
    if (reducedTemperature < 0)
        largestTemperature = theDimensionality->denominator_exponent[kSITemperatureIndex];

    int8_t reducedAmount = theDimensionality->numerator_exponent[kSIAmountIndex] - theDimensionality->denominator_exponent[kSIAmountIndex];
    int8_t largestAmount = theDimensionality->numerator_exponent[kSIAmountIndex];
    if (reducedAmount < 0)
        largestAmount = theDimensionality->denominator_exponent[kSIAmountIndex];

    int8_t reducedLuminous = theDimensionality->numerator_exponent[kSILuminousIntensityIndex] - theDimensionality->denominator_exponent[kSILuminousIntensityIndex];
    int8_t largestLuminous = theDimensionality->numerator_exponent[kSILuminousIntensityIndex];
    if (reducedLuminous < 0)
        largestLuminous = theDimensionality->denominator_exponent[kSILuminousIntensityIndex];

    OCMutableArrayRef result = OCArrayCreateMutable(0, &kOCTypeArrayCallBacks);
    if (result)
    {
        for (int8_t iLength = largestLength; iLength >= abs(reducedLength); iLength--)
        {
            int8_t length_numerator = iLength;
            int8_t length_denominator = iLength - abs(reducedLength);
            if (reducedLength < 0)
            {
                length_numerator = iLength - abs(reducedLength);
                length_denominator = iLength;
            }
            for (int8_t iMass = largestMass; iMass >= abs(reducedMass); iMass--)
            {
                int8_t mass_numerator = iMass;
                int8_t mass_denominator = iMass - abs(reducedMass);
                if (reducedMass < 0)
                {
                    mass_numerator = iMass - abs(reducedMass);
                    mass_denominator = iMass;
                }
                for (int8_t iTime = largestTime; iTime >= abs(reducedTime); iTime--)
                {
                    int8_t time_numerator = iTime;
                    int8_t time_denominator = iTime - abs(reducedTime);
                    if (reducedTime < 0)
                    {
                        time_numerator = iTime - abs(reducedTime);
                        time_denominator = iTime;
                    }
                    for (int8_t iCurrent = largestCurrent; iCurrent >= abs(reducedCurrent); iCurrent--)
                    {
                        int8_t current_numerator = iCurrent;
                        int8_t current_denominator = iCurrent - abs(reducedCurrent);
                        if (reducedCurrent < 0)
                        {
                            current_numerator = iCurrent - abs(reducedCurrent);
                            current_denominator = iCurrent;
                        }
                        for (int8_t iTemp = largestTemperature; iTemp >= abs(reducedTemperature); iTemp--)
                        {
                            int8_t temperature_numerator = iTemp;
                            int8_t temperature_denominator = iTemp - abs(reducedTemperature);
                            if (reducedTemperature < 0)
                            {
                                temperature_numerator = iTemp - abs(reducedTemperature);
                                temperature_denominator = iTemp;
                            }
                            for (int8_t iAmount = largestAmount; iAmount >= abs(reducedAmount); iAmount--)
                            {
                                int8_t amount_numerator = iAmount;
                                int8_t amount_denominator = iAmount - abs(reducedAmount);
                                if (reducedAmount < 0)
                                {
                                    amount_numerator = iAmount - abs(reducedAmount);
                                    amount_denominator = iAmount;
                                }
                                for (int8_t iLuminous = largestLuminous; iLuminous >= abs(reducedLuminous); iLuminous--)
                                {
                                    int8_t luminous_numerator = iLuminous;
                                    int8_t luminous_denominator = iLuminous - abs(reducedLuminous);
                                    if (reducedLuminous < 0)
                                    {
                                        luminous_numerator = iLuminous - abs(reducedLuminous);
                                        luminous_denominator = iLuminous;
                                    }
                                    SIDimensionalityRef dimensionality = SIDimensionalityWithExponents(length_numerator, length_denominator,
                                                                                                       mass_numerator, mass_denominator,
                                                                                                       time_numerator, time_denominator,
                                                                                                       current_numerator, current_denominator,
                                                                                                       temperature_numerator, temperature_denominator,
                                                                                                       amount_numerator, amount_denominator,
                                                                                                       luminous_numerator, luminous_denominator);
                                    OCArrayAppendValue(result, dimensionality);
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

OCArrayRef SIDimensionalityCreateArrayOfQuantitiesWithSameReducedDimensionality(SIDimensionalityRef theDimensionality)
{
    OCArrayRef reducedDimensionalities = SIDimensionalityCreateArrayWithSameReducedDimensionality(theDimensionality);
    OCMutableArrayRef result = OCArrayCreateMutable(0, &kOCTypeArrayCallBacks);
    for (uint64_t dimensionalityIndex = 0; dimensionalityIndex < OCArrayGetCount(reducedDimensionalities); dimensionalityIndex++)
    {
        SIDimensionalityRef dimensionality = OCArrayGetValueAtIndex(reducedDimensionalities, dimensionalityIndex);
        OCArrayRef quantities = SIDimensionalityCreateArrayOfQuantities(dimensionality);
        if (quantities)
        {
            OCArrayAppendArray(result, quantities, OCRangeMake(0, OCArrayGetCount(quantities)));
            OCRelease(quantities);
        }
    }
    OCRelease(reducedDimensionalities);
    return result;
}

#pragma mark Strings and Archiving

void SIDimensionalityShow(SIDimensionalityRef theDimensionality)
{
    IF_NO_OBJECT_EXISTS_RETURN(theDimensionality, )
    OCStringShow((theDimensionality)->symbol);
    fprintf(stdout, "\n");
}

void SIDimensionalityShowFull(SIDimensionalityRef theDimensionality)
{
    IF_NO_OBJECT_EXISTS_RETURN(theDimensionality, )

    OCStringShow(STR("============================================================================================================="));

    OCStringRef cf_string;

    OCStringShow(STR("                                            m  kg   s   A   K  mol cd"));
    OCStringShow(STR("-------------------------------------------------------------------------------------------------------------"));

    cf_string = OCStringCreateWithFormat(STR("SI base dimension numerator exponents:    %3d %3d %3d %3d %3d %3d %3d"),
                                         theDimensionality->numerator_exponent[kSILengthIndex],
                                         theDimensionality->numerator_exponent[kSIMassIndex],
                                         theDimensionality->numerator_exponent[kSITimeIndex],
                                         theDimensionality->numerator_exponent[kSICurrentIndex],
                                         theDimensionality->numerator_exponent[kSITemperatureIndex],
                                         theDimensionality->numerator_exponent[kSIAmountIndex],
                                         theDimensionality->numerator_exponent[kSILuminousIntensityIndex]);
    OCStringShow(cf_string);
    OCRelease(cf_string);

    cf_string = OCStringCreateWithFormat(STR("SI base dimension denominator exponents:  %3d %3d %3d %3d %3d %3d %3d"),
                                         theDimensionality->denominator_exponent[kSILengthIndex],
                                         theDimensionality->denominator_exponent[kSIMassIndex],
                                         theDimensionality->denominator_exponent[kSITimeIndex],
                                         theDimensionality->denominator_exponent[kSICurrentIndex],
                                         theDimensionality->denominator_exponent[kSITemperatureIndex],
                                         theDimensionality->denominator_exponent[kSIAmountIndex],
                                         theDimensionality->denominator_exponent[kSILuminousIntensityIndex]);
    OCStringShow(cf_string);
    OCRelease(cf_string);

    OCStringShow(STR("-------------------------------------------------------------------------------------------------------------"));

    OCStringShow(theDimensionality->symbol);

    OCArrayRef quantities = SIDimensionalityCreateArrayOfQuantities(theDimensionality);
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

static SIDimensionalityRef AddDimensionalityToLibrary(uint8_t length_numerator_exponent, uint8_t length_denominator_exponent,
                                                      uint8_t mass_numerator_exponent, uint8_t mass_denominator_exponent,
                                                      uint8_t time_numerator_exponent, uint8_t time_denominator_exponent,
                                                      uint8_t current_numerator_exponent, uint8_t current_denominator_exponent,
                                                      uint8_t temperature_numerator_exponent, uint8_t temperature_denominator_exponent,
                                                      uint8_t amount_numerator_exponent, uint8_t amount_denominator_exponent,
                                                      uint8_t luminous_intensity_numerator_exponent, uint8_t luminous_intensity_denominator_exponent)
{
    SIDimensionalityRef dimensionality = SIDimensionalityCreate(length_numerator_exponent, length_denominator_exponent,
                                                                mass_numerator_exponent, mass_denominator_exponent,
                                                                time_numerator_exponent, time_denominator_exponent,
                                                                current_numerator_exponent, current_denominator_exponent,
                                                                temperature_numerator_exponent, temperature_denominator_exponent,
                                                                amount_numerator_exponent, amount_denominator_exponent,
                                                                luminous_intensity_numerator_exponent, luminous_intensity_denominator_exponent);

    struct __SIDimensionality *temp = (struct __SIDimensionality *)dimensionality;
    temp->_base.retainCount = 0;
    OCDictionaryAddValue(dimensionalityLibrary, dimensionality->symbol, dimensionality);
    OCRelease(dimensionality);
    return dimensionality;
}

static void DimensionalityLibraryBuild()
{

    dimensionalityLibrary = OCDictionaryCreateMutable(0);
    dimensionalityQuantitiesLibrary = OCDictionaryCreateMutable(0);

    SIDimensionalityRef dimensionality;

    // Base Root Name and Root Symbol Units - Table 1

    // ***** Dimensionless **********************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityDimensionless
    // Dimensionless                           length     mass        time        current     temperature     amount      luminous intensity
    dimensionality = AddDimensionalityToLibrary(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityDimensionless, dimensionality);

    // ***** Length *****************************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityLength
    // Length                                   length     mass        time        current     temperature     amount      luminous intensity
    dimensionality = AddDimensionalityToLibrary(1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityLength, dimensionality);

    // Wavenumber, Inverse Length
    dimensionality = AddDimensionalityToLibrary(0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityWavenumber, dimensionality);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityInverseLength, dimensionality);

    // Plane Angle, Length Ratio
    dimensionality = AddDimensionalityToLibrary(1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityPlaneAngle, dimensionality);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityLengthRatio, dimensionality);

    // ***** Mass *******************************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityMass
    // Mass                                     length      mass        time        current     temperature     amount      luminous intensity
    dimensionality = AddDimensionalityToLibrary(0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityMass, dimensionality);

    // Inverse Mass                                     length      mass        time        current     temperature     amount      luminous intensity
    dimensionality = AddDimensionalityToLibrary(0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityInverseMass, dimensionality);

    // Mass Ratio                               length      mass        time        current     temperature     amount      luminous intensity
    dimensionality = AddDimensionalityToLibrary(0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityMassRatio, dimensionality);

    // ***** Time *******************************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityTime
    // Time                                     length      mass        time        current     temperature     amount      luminous intensity
    dimensionality = AddDimensionalityToLibrary(0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityTime, dimensionality);

    // Inverse Time, Frequency, Radioactivity
    dimensionality = AddDimensionalityToLibrary(0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityFrequency, dimensionality);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityRadioactivity, dimensionality);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityInverseTime, dimensionality);

    // Time, Frequency Ratio
    dimensionality = AddDimensionalityToLibrary(0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityTimeRatio, dimensionality);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityFrequencyRatio, dimensionality);

    // Inverse Time Squared
    dimensionality = AddDimensionalityToLibrary(0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityInverseTimeSquared, dimensionality);

    // ***** Current ****************************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityCurrent
    // Current                                  length     mass        time        current     temperature     amount      luminous intensity
    dimensionality = AddDimensionalityToLibrary(0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityCurrent, dimensionality);

    // Inverse Current                          length     mass        time        current     temperature     amount      luminous intensity
    dimensionality = AddDimensionalityToLibrary(0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityInverseCurrent, dimensionality);

    // Current Ratio                            length     mass        time        current     temperature     amount      luminous intensity
    dimensionality = AddDimensionalityToLibrary(0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityCurrentRatio, dimensionality);

    // ***** Thermodynamic Temperature **********************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityTemperature
    // Temperature                              length     mass        time        current     temperature     amount      luminous intensity
    dimensionality = AddDimensionalityToLibrary(0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityTemperature, dimensionality);

    // Inverse Temperature                      length     mass        time        current     temperature     amount      luminous intensity
    dimensionality = AddDimensionalityToLibrary(0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityInverseTemperature, dimensionality);

    // Temperature Ratio                        length     mass        time        current     temperature     amount      luminous intensity
    dimensionality = AddDimensionalityToLibrary(0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityTemperatureRatio, dimensionality);

    // ***** Amount *****************************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityAmount
    // Amount                                   length     mass        time        current     temperature     amount      luminous intensity
    dimensionality = AddDimensionalityToLibrary(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityAmount, dimensionality);

    // inverse amount
    dimensionality = AddDimensionalityToLibrary(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityInverseAmount, dimensionality);

    // amount ratio
    dimensionality = AddDimensionalityToLibrary(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityAmountRatio, dimensionality);

    // ***** Luminous Intensity *****************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityLuminousIntensity
    // Luminous Intensity                       length     mass        time        current     temperature     amount      luminous intensity
    dimensionality = AddDimensionalityToLibrary(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityLuminousIntensity, dimensionality);

    dimensionality = AddDimensionalityToLibrary(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityInverseLuminousIntensity, dimensionality);

    dimensionality = AddDimensionalityToLibrary(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityLuminousIntensityRatio, dimensionality);

    // ***** Area *******************************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityArea
    // Area
    dimensionality = AddDimensionalityToLibrary(2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityArea, dimensionality);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityRockPermeability, dimensionality);

    // Inverse Area
    dimensionality = AddDimensionalityToLibrary(0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityInverseArea, dimensionality);

    // Area Ratio, Solid Angle
    dimensionality = AddDimensionalityToLibrary(2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityAreaRatio, dimensionality);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantitySolidAngle, dimensionality);

    // ***** Volume *******************************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityVolume
    // Volume
    dimensionality = AddDimensionalityToLibrary(3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityVolume, dimensionality);

    // Inverse Volume
    dimensionality = AddDimensionalityToLibrary(0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityInverseVolume, dimensionality);

    // Volume Ratio
    dimensionality = AddDimensionalityToLibrary(3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityVolumeRatio, dimensionality);

    // Temperature Gradient                    length     mass        time        current     temperature     amount      luminous intensity
    dimensionality = AddDimensionalityToLibrary(0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityTemperatureGradient, dimensionality);

    // Coherent Units with no Unit name - Table 2

    // Speed, Velocity
    dimensionality = AddDimensionalityToLibrary(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantitySpeed, dimensionality);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityVelocity, dimensionality);

    // Linear Momentum                          length     mass        time        current     temperature     amount      luminous intensity
    dimensionality = AddDimensionalityToLibrary(1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityLinearMomentum, dimensionality);

    // Acceleration
    dimensionality = AddDimensionalityToLibrary(1, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityAcceleration, dimensionality);

    // Moment of Inertia
    dimensionality = AddDimensionalityToLibrary(2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityMomentOfInertia, dimensionality);

    // Mass Flow Rate
    dimensionality = AddDimensionalityToLibrary(0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityMassFlowRate, dimensionality);

    // Mass Flux
    dimensionality = AddDimensionalityToLibrary(0, 2, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityMassFlux, dimensionality);

    // Diffusion Flux
    dimensionality = AddDimensionalityToLibrary(0, 2, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityDiffusionFlux, dimensionality);

    // Density
    dimensionality = AddDimensionalityToLibrary(0, 3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityDensity, dimensionality);

    // Specific Gravity
    dimensionality = AddDimensionalityToLibrary(3, 3, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantitySpecificGravity, dimensionality);

    // Surface Density
    dimensionality = AddDimensionalityToLibrary(0, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantitySurfaceDensity, dimensionality);

    // Specific Surface Area
    dimensionality = AddDimensionalityToLibrary(2, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantitySpecificSurfaceArea, dimensionality);

    // Surface Area to Volume Ratio
    dimensionality = AddDimensionalityToLibrary(2, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantitySurfaceAreaToVolumeRatio, dimensionality);

    // Specific Volume
    dimensionality = AddDimensionalityToLibrary(3, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantitySpecificVolume, dimensionality);

    // Current Density
    dimensionality = AddDimensionalityToLibrary(0, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityCurrentDensity, dimensionality);

    // Magnetic Field Strength
    dimensionality = AddDimensionalityToLibrary(0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityMagneticFieldStrength, dimensionality);

    // Luminance
    dimensionality = AddDimensionalityToLibrary(0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityLuminance, dimensionality);

    // Refractive Index
    dimensionality = AddDimensionalityToLibrary(1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityRefractiveIndex, dimensionality);

    // More Coherent Units with no Symbols - Table 4

    // Dynamic Viscosity
    dimensionality = AddDimensionalityToLibrary(0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityDynamicViscosity, dimensionality);

    // Fluidity (inverse dynamic viscosity)
    dimensionality = AddDimensionalityToLibrary(1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityFluidity, dimensionality);

    // Moment of Force
    dimensionality = AddDimensionalityToLibrary(2, 0, 2, 1, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityMomentOfForce, dimensionality);

    // Surface Tension
    dimensionality = AddDimensionalityToLibrary(0, 0, 1, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantitySurfaceTension, dimensionality);

    // Surface Energy                           length     mass        time        current     temperature     amount      luminous intensity
    dimensionality = AddDimensionalityToLibrary(2, 2, 1, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantitySurfaceEnergy, dimensionality);

    // Angular Velocity
    dimensionality = AddDimensionalityToLibrary(1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityAngularVelocity, dimensionality);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityAngularSpeed, dimensionality);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityAngularFrequency, dimensionality);

    // Angular Acceleration
    dimensionality = AddDimensionalityToLibrary(1, 1, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityAngularAcceleration, dimensionality);

    // Heat Flux Density, Irradiance
    dimensionality = AddDimensionalityToLibrary(2, 2, 1, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityHeatFluxDensity, dimensionality);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityIrradiance, dimensionality);

    // Spectral Radiant Flux Density
    dimensionality = AddDimensionalityToLibrary(0, 1, 1, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantitySpectralRadiantFluxDensity, dimensionality);

    // Heat Capacity, Entropy
    dimensionality = AddDimensionalityToLibrary(2, 0, 1, 0, 0, 2, 0, 0, 0, 1, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityHeatCapacity, dimensionality);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityEntropy, dimensionality);

    // Specific Heat Capacity, Specific Entropy
    dimensionality = AddDimensionalityToLibrary(2, 0, 0, 0, 0, 2, 0, 0, 0, 1, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantitySpecificHeatCapacity, dimensionality);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantitySpecificEntropy, dimensionality);

    // Specific Energy
    dimensionality = AddDimensionalityToLibrary(2, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantitySpecificEnergy, dimensionality);

    // Thermal Conductance                      length      mass        time        current     temperature     amount      luminous intensity
    dimensionality = AddDimensionalityToLibrary(2, 0, 1, 0, 0, 3, 0, 0, 0, 1, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityThermalConductance, dimensionality);

    // Thermal Conductivity
    dimensionality = AddDimensionalityToLibrary(1, 0, 1, 0, 0, 3, 0, 0, 0, 1, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityThermalConductivity, dimensionality);

    // Electric Field Strength
    dimensionality = AddDimensionalityToLibrary(1, 0, 1, 0, 0, 3, 0, 1, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityElectricFieldStrength, dimensionality);

    // Electric Charge Density
    dimensionality = AddDimensionalityToLibrary(0, 3, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityElectricChargeDensity, dimensionality);

    //  Electric Flux                           length     mass        time        current   temperature  amount    luminous intensity
    dimensionality = AddDimensionalityToLibrary(3, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityElectricFlux, dimensionality);

    // Surface Charge Density, Electric Flux Density, Electric Displacement
    dimensionality = AddDimensionalityToLibrary(0, 2, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantitySurfaceChargeDensity, dimensionality);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityElectricFluxDensity, dimensionality);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityElectricDisplacement, dimensionality);

    // Electric Polarizability
    dimensionality = AddDimensionalityToLibrary(2, 2, 0, 1, 4, 0, 2, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityElectricPolarizability, dimensionality);

    // Electric Quadrupole Moment
    dimensionality = AddDimensionalityToLibrary(2, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityElectricQuadrupoleMoment, dimensionality);

    //  Magnetizability                        length     mass        time        current   temperature  amount    luminous intensity
    dimensionality = AddDimensionalityToLibrary(2, 0, 1, 2, 4, 2, 2, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityMagnetizability, dimensionality);

    // Permittivity
    dimensionality = AddDimensionalityToLibrary(0, 3, 0, 1, 4, 0, 2, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityPermittivity, dimensionality);

    //  Permeability                    length     mass        time        current   temperature  amount    luminous intensity
    dimensionality = AddDimensionalityToLibrary(1, 0, 1, 0, 0, 2, 0, 2, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityPermeability, dimensionality);

    // Molar Energy
    dimensionality = AddDimensionalityToLibrary(2, 0, 1, 0, 0, 2, 0, 0, 0, 0, 0, 1, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityMolarEnergy, dimensionality);

    // Molar Entropy, Molar Heat Capacity
    dimensionality = AddDimensionalityToLibrary(2, 0, 1, 0, 0, 2, 0, 0, 0, 1, 0, 1, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityMolarEntropy, dimensionality);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityMolarHeatCapacity, dimensionality);

    // Absorbed Dose Rate
    dimensionality = AddDimensionalityToLibrary(2, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityAbsorbedDoseRate, dimensionality);

    // Radiant Intensity
    dimensionality = AddDimensionalityToLibrary(4, 2, 1, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityRadiantIntensity, dimensionality);

    // Spectral Radiant Intensity
    dimensionality = AddDimensionalityToLibrary(4, 3, 1, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantitySpectralRadiantIntensity, dimensionality);

    // Radiance
    dimensionality = AddDimensionalityToLibrary(4, 4, 1, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0);

    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityRadiance, dimensionality);

    // Spectral Radiance
    dimensionality = AddDimensionalityToLibrary(4, 5, 1, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantitySpectralRadiance, dimensionality);

    // Special Names and Symbols for Coherent Derived Units - Table 3

    // Porosity
    dimensionality = AddDimensionalityToLibrary(3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityPorosity, dimensionality);

    // Force
    dimensionality = AddDimensionalityToLibrary(1, 0, 1, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityForce, dimensionality);

    // Pressure, Stress, Energy Density
    dimensionality = AddDimensionalityToLibrary(0, 1, 1, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityPressure, dimensionality);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityStress, dimensionality);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityEnergyDensity, dimensionality);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityElasticModulus, dimensionality);

    // Compressibility : Inverse Pressure
    dimensionality = AddDimensionalityToLibrary(1, 0, 0, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityCompressibility, dimensionality);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityStressOpticCoefficient, dimensionality);

    // Pressure Gradient                       length     mass        time        current   temperature  amount    luminous intensity
    dimensionality = AddDimensionalityToLibrary(0, 2, 1, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityPressureGradient, dimensionality);

    // Energy, Work, Heat
    dimensionality = AddDimensionalityToLibrary(2, 0, 1, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityEnergy, dimensionality);

    // Spectral radiant energy
    dimensionality = AddDimensionalityToLibrary(2, 1, 1, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantitySpectralRadiantEnergy, dimensionality);

    // Torque
    dimensionality = AddDimensionalityToLibrary(3, 1, 1, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityTorque, dimensionality);

    // Power, Radiant Flux
    dimensionality = AddDimensionalityToLibrary(2, 0, 1, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityPower, dimensionality);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityRadiantFlux, dimensionality);

    // Spectral Power
    dimensionality = AddDimensionalityToLibrary(2, 1, 1, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantitySpectralPower, dimensionality);

    // Volume Power Density
    dimensionality = AddDimensionalityToLibrary(2, 3, 1, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityVolumePowerDensity, dimensionality);

    // Specific Power
    dimensionality = AddDimensionalityToLibrary(2, 0, 1, 1, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantitySpecificPower, dimensionality);

    // Electric Charge, Amount of Electricity
    dimensionality = AddDimensionalityToLibrary(0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityElectricCharge, dimensionality);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityAmountOfElectricity, dimensionality);

    //  Electric Dipole Moment                  length     mass        time        current   temperature  amount    luminous intensity
    dimensionality = AddDimensionalityToLibrary(1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityElectricDipoleMoment, dimensionality);

    //  Gyromagnetic Ratio                      length     mass        time        current   temperature  amount    luminous intensity
    dimensionality = AddDimensionalityToLibrary(1, 1, 0, 1, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityGyromagneticRatio, dimensionality);

    // Electric Potential Difference, Electromotive Force
    dimensionality = AddDimensionalityToLibrary(2, 0, 1, 0, 0, 3, 0, 1, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityElectricPotentialDifference, dimensionality);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityElectromotiveForce, dimensionality);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityVoltage, dimensionality);

    // Electrical Mobility
    dimensionality = AddDimensionalityToLibrary(2, 2, 0, 1, 3, 1, 1, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityElectricalMobility, dimensionality);

    // Electric Field Gradient
    dimensionality = AddDimensionalityToLibrary(2, 2, 1, 0, 0, 3, 0, 1, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityElectricFieldGradient, dimensionality);

    // Capacitance
    dimensionality = AddDimensionalityToLibrary(0, 2, 0, 1, 4, 0, 2, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityCapacitance, dimensionality);

    // Electric Resistance
    dimensionality = AddDimensionalityToLibrary(2, 0, 1, 0, 0, 3, 0, 2, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityElectricResistance, dimensionality);

    // Electric Resistance per length
    dimensionality = AddDimensionalityToLibrary(2, 1, 1, 0, 0, 3, 0, 2, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityElectricResistancePerLength, dimensionality);

    // Electric Resistivity
    dimensionality = AddDimensionalityToLibrary(3, 0, 1, 0, 0, 3, 0, 2, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityElectricResistivity, dimensionality);

    // Electric Conductance
    dimensionality = AddDimensionalityToLibrary(0, 2, 0, 1, 3, 0, 2, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityElectricConductance, dimensionality);

    // Electric Conductivity
    dimensionality = AddDimensionalityToLibrary(0, 3, 0, 1, 3, 0, 2, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityElectricConductivity, dimensionality);

    // Molar Conductivity
    dimensionality = AddDimensionalityToLibrary(2, 2, 0, 1, 3, 0, 2, 0, 0, 0, 0, 1, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityMolarConductivity, dimensionality);

    //  Magnetic Dipole Moment                  length     mass        time        current   temperature  amount    luminous intensity
    dimensionality = AddDimensionalityToLibrary(2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityMagneticDipoleMoment, dimensionality);

    //  Magnetic Dipole Moment Ratio             length     mass        time        current   temperature  amount    luminous intensity
    dimensionality = AddDimensionalityToLibrary(2, 2, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityMagneticDipoleMomentRatio, dimensionality);

    // Magnetic Flux
    dimensionality = AddDimensionalityToLibrary(2, 0, 1, 0, 0, 2, 0, 1, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityMagneticFlux, dimensionality);

    // Magnetic Flux Density
    dimensionality = AddDimensionalityToLibrary(0, 0, 1, 0, 0, 2, 0, 1, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityMagneticFluxDensity, dimensionality);

    // Inverse Magnetic Flux Density
    dimensionality = AddDimensionalityToLibrary(0, 0, 0, 1, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityInverseMagneticFluxDensity, dimensionality);

    // Frequency per Magnetic Flux Density, Charge to Mass Ratio, Radiation Exposure (x- and gamma-rays)
    dimensionality = AddDimensionalityToLibrary(0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityRadiationExposure, dimensionality);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityChargeToMassRatio, dimensionality);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityFrequencyPerMagneticFluxDensity, dimensionality);

    // Mass to Charge Ratio                     length     mass       time      current   temperature  amount    luminous intensity
    dimensionality = AddDimensionalityToLibrary(0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityMassToChargeRatio, dimensionality);

    // Magnetic Field Gradient                  length     mass        time        current   temperature  amount    luminous intensity
    dimensionality = AddDimensionalityToLibrary(0, 1, 1, 0, 0, 2, 0, 1, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityMagneticFieldGradient, dimensionality);

    // Inductance
    dimensionality = AddDimensionalityToLibrary(2, 0, 1, 0, 0, 2, 0, 2, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityInductance, dimensionality);

    // Luminous Flux
    dimensionality = AddDimensionalityToLibrary(2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityLuminousFlux, dimensionality);

    // Luminous Flux Density
    dimensionality = AddDimensionalityToLibrary(2, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityLuminousFluxDensity, dimensionality);

    // Luminous Energy
    dimensionality = AddDimensionalityToLibrary(2, 2, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityLuminousEnergy, dimensionality);

    // Illuminance
    dimensionality = AddDimensionalityToLibrary(2, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityIlluminance, dimensionality);

    // Absorbed dose, Dose equivalent
    dimensionality = AddDimensionalityToLibrary(2, 0, 1, 1, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityAbsorbedDose, dimensionality);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityDoseEquivalent, dimensionality);

    // Catalytic Activity
    dimensionality = AddDimensionalityToLibrary(0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityCatalyticActivity, dimensionality);

    // Catalytic Activity Concentration
    dimensionality = AddDimensionalityToLibrary(0, 3, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityCatalyticActivityConcentration, dimensionality);

    // Catalytic Activity Content
    dimensionality = AddDimensionalityToLibrary(0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityCatalyticActivityContent, dimensionality);

    // Table 6 - Non-SI units but SI accepted

    // Reduced Action
    dimensionality = AddDimensionalityToLibrary(3, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityReducedAction, dimensionality);

    // Action, Angular Momentum
    dimensionality = AddDimensionalityToLibrary(2, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityAction, dimensionality);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityAngularMomentum, dimensionality);

    // Kinematic Viscosity
    dimensionality = AddDimensionalityToLibrary(2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityKinematicViscosity, dimensionality);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityDiffusionCoefficient, dimensionality);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityCirculation, dimensionality);

    // amount concentration                     length     mass       time      current   temperature  amount    luminous intensity
    dimensionality = AddDimensionalityToLibrary(0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityAmountConcentration, dimensionality);

    // mass concentration
    dimensionality = AddDimensionalityToLibrary(0, 3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityMassConcentration, dimensionality);

    // molar mass
    dimensionality = AddDimensionalityToLibrary(0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityMolarMass, dimensionality);

    // molality
    dimensionality = AddDimensionalityToLibrary(0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityMolality, dimensionality);

    // molar magnetic susceptibility
    dimensionality = AddDimensionalityToLibrary(3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityMolarMagneticSusceptibility, dimensionality);

    // coulomb per mole
    dimensionality = AddDimensionalityToLibrary(0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityChargePerAmount, dimensionality);

    // cubic meters per kilogram second (Gravitational Constant)
    dimensionality = AddDimensionalityToLibrary(3, 0, 0, 1, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityGravitationalConstant, dimensionality);

    // distance per volume
    dimensionality = AddDimensionalityToLibrary(1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityLengthPerVolume, dimensionality);

    // volume per distance
    dimensionality = AddDimensionalityToLibrary(3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityVolumePerLength, dimensionality);

    // volume per time
    dimensionality = AddDimensionalityToLibrary(3, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityVolumetricFlowRate, dimensionality);

    // power per luminous flux
    dimensionality = AddDimensionalityToLibrary(3, 1, 1, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 1);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityPowerPerLuminousFlux, dimensionality);

    // luminous efficacy
    dimensionality = AddDimensionalityToLibrary(0, 2, 0, 1, 3, 0, 0, 0, 0, 0, 0, 0, 1, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityLuminousEfficacy, dimensionality);

    // Heat Transfer Coefficient                length     mass       time      current   temperature  amount    luminous intensity
    dimensionality = AddDimensionalityToLibrary(0, 0, 1, 0, 0, 3, 0, 0, 0, 1, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityHeatTransferCoefficient, dimensionality);

    // Stefan Boltzman constant dimensionality  length     mass       time      current   temperature  amount    luminous intensity
    dimensionality = AddDimensionalityToLibrary(2, 2, 1, 0, 0, 3, 0, 0, 0, 4, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityPowerPerAreaPerTemperatureToFourthPower, dimensionality);

    // Gas Permeance
    dimensionality = AddDimensionalityToLibrary(1, 2, 0, 1, 2, 1, 0, 0, 0, 0, 1, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityGasPermeance, dimensionality);

    // kSIQuantityFirstHyperPolarizability
    dimensionality = AddDimensionalityToLibrary(3, 4, 0, 2, 7, 0, 3, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityFirstHyperPolarizability, dimensionality);

    // kSIQuantitySecondHyperPolarizability
    dimensionality = AddDimensionalityToLibrary(4, 6, 0, 3, 10, 0, 4, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantitySecondHyperPolarizability, dimensionality);

    // Second Radiation Constant                length     mass       time      current   temperature  amount    luminous intensity
    dimensionality = AddDimensionalityToLibrary(3, 2, 1, 1, 2, 2, 0, 0, 1, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantitySecondRadiationConstant, dimensionality);

    // Wien Wavelength Displacement Constant    length     mass       time      current   temperature  amount    luminous intensity
    dimensionality = AddDimensionalityToLibrary(1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityWavelengthDisplacementConstant, dimensionality);

    // Fine Structure Constant                  length     mass       time      current   temperature  amount    luminous intensity
    dimensionality = AddDimensionalityToLibrary(5, 5, 1, 1, 4, 4, 2, 2, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityFineStructureConstant, dimensionality);

    // 1/(N•T)  kSIQuantityRatePerAmountConcentrationPerTime
    dimensionality = AddDimensionalityToLibrary(3, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0);
    OCDictionaryAddValue(dimensionalityQuantitiesLibrary, kSIQuantityRatePerAmountConcentrationPerTime, dimensionality);

}

/*
 @function SIDimensionalityCopyDimensionalityLibrary
 @abstract Gets a copy of the dimensionalityLibrary of dimensionalities
 @result a CFSet containing the dimensionalities.
 */
static OCDictionaryRef SIDimensionalityCopyDimensionalityLibrary(void)
{
    if (NULL == dimensionalityLibrary)
        DimensionalityLibraryBuild();
    return OCDictionaryCreateCopy(dimensionalityLibrary);
}

SIDimensionalityRef SIDimensionalityForQuantity(OCStringRef quantity, OCStringRef *error)
{
    if (quantity == NULL)
        return NULL;
    if (NULL == dimensionalityQuantitiesLibrary)
        DimensionalityLibraryBuild();
    SIDimensionalityRef dimensionality = NULL;

    if (OCDictionaryContainsKey(dimensionalityQuantitiesLibrary, quantity))
    {
        dimensionality = OCDictionaryGetValue(dimensionalityQuantitiesLibrary, quantity);
        return dimensionality;
    }

    OCMutableStringRef lowerCaseQuantity = OCStringCreateMutableCopy(quantity);
    OCStringLowercase(lowerCaseQuantity);

    if (OCDictionaryContainsKey(dimensionalityQuantitiesLibrary, lowerCaseQuantity))
    {
        dimensionality = OCDictionaryGetValue(dimensionalityQuantitiesLibrary, lowerCaseQuantity);
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
 @abstract Shows every dimensionality in the dimensionalityLibrary
 */
// static void SIDimensionalityLibraryShowFull(void)
//{
//     if(NULL==dimensionalityLibrary) DimensionalityLibraryBuild();
//     OCDictionaryApplyFunction(dimensionalityLibrary,(OCDictionaryApplierFunction) SIDimensionalityShowFull,NULL);
// }

OCMutableDictionaryRef SIDimensionalityGetLibrary()
{
    if (NULL == dimensionalityLibrary)
        DimensionalityLibraryBuild();
    return dimensionalityLibrary;
}

void SIDimensionalitySetLibrary(OCMutableDictionaryRef newDimensionalityLibrary)
{
    if (newDimensionalityLibrary == dimensionalityLibrary)
        return;
    if (newDimensionalityLibrary)
    {
        if (dimensionalityLibrary)
            OCRelease(dimensionalityLibrary);
        dimensionalityLibrary = (OCMutableDictionaryRef)OCRetain(newDimensionalityLibrary);
    }
}

// OCArrayRef SIDimensionalityLibraryCreateArrayOfAllQuantities()
//{
//     if(NULL==dimensionalityQuantitiesLibrary) DimensionalityLibraryBuild();
//     uint64_t totalCount = OCDictionaryGetCount(dimensionalityQuantitiesLibrary);
//     OCStringRef keys[totalCount];
//     SIDimensionalityRef dimensionalities[totalCount];
//     OCDictionaryGetKeysAndValues(dimensionalityQuantitiesLibrary, (const void **) keys, (const void **)  dimensionalities);
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
        if (input1->numerator_exponent[i] != input2->numerator_exponent[i])
            return false;
        if (input1->denominator_exponent[i] != input2->denominator_exponent[i])
            return false;
    }

    return true;
}

bool SIDimensionalityIsDimensionless(SIDimensionalityRef theDimensionality)
{
    IF_NO_OBJECT_EXISTS_RETURN(theDimensionality, false)

    for (SIBaseDimensionIndex index = 0; index < BASE_DIMENSION_COUNT; index++)
    {
        int theDimensionality_exponent = theDimensionality->numerator_exponent[index] - theDimensionality->denominator_exponent[index];
        if (theDimensionality_exponent != 0)
            return false;
    }
    return true;
}

bool SIDimensionalityIsDimensionlessAndNotDerived(SIDimensionalityRef theDimensionality)
{
    // To be dimensionless and not derived all the numerator and denominator exponents must be 0
    IF_NO_OBJECT_EXISTS_RETURN(theDimensionality, false)
    for (SIBaseDimensionIndex index = 0; index < BASE_DIMENSION_COUNT; index++)
    {
        if (theDimensionality->numerator_exponent[index] != 0)
            return false;
        if (theDimensionality->denominator_exponent[index] != 0)
            return false;
    }
    return true;
}

bool SIDimensionalityIsDimensionlessAndDerived(SIDimensionalityRef theDimensionality)
{
    if (SIDimensionalityIsDimensionlessAndNotDerived(theDimensionality))
        return false;
    if (SIDimensionalityIsDimensionless(theDimensionality))
        return true;
    return false;
}

bool SIDimensionalityIsBaseDimensionality(SIDimensionalityRef theDimensionality)
{
    IF_NO_OBJECT_EXISTS_RETURN(theDimensionality, false)
    if (SIDimensionalityIsDimensionlessAndNotDerived(theDimensionality))
        return false;
    // If it is base dimensionality, then all the denominator exponents must be 0
    // and all numerator exponents are zero except one, which is 1
    for (SIBaseDimensionIndex index = 0; index < BASE_DIMENSION_COUNT; index++)
        if (SIDimensionalityGetDenominatorExponentAtIndex(theDimensionality, index) != 0)
            return false;
    int count = 0;
    for (SIBaseDimensionIndex index = 0; index < BASE_DIMENSION_COUNT; index++)
    {
        if (theDimensionality->numerator_exponent[index] > 1)
            return false;
        if (theDimensionality->numerator_exponent[index] == 1)
            count++;
    }
    if (count == 1)
        return true;
    return false;
}

bool SIDimensionalityIsDerived(SIDimensionalityRef theDimensionality)
{
    IF_NO_OBJECT_EXISTS_RETURN(theDimensionality, false)
    if (SIDimensionalityIsDimensionlessAndNotDerived(theDimensionality))
        return false;
    if (SIDimensionalityIsBaseDimensionality(theDimensionality))
        return false;
    return true;
}

bool SIDimensionalityHasSameReducedDimensionality(SIDimensionalityRef theDimensionality1, SIDimensionalityRef theDimensionality2)
{
    IF_NO_OBJECT_EXISTS_RETURN(theDimensionality1, NULL)
    IF_NO_OBJECT_EXISTS_RETURN(theDimensionality2, NULL)
    if (theDimensionality1 == theDimensionality2)
        return true;

    for (int i = 0; i < BASE_DIMENSION_COUNT; i++)
    {
        int theDimensionality1_exponent = theDimensionality1->numerator_exponent[i] - theDimensionality1->denominator_exponent[i];
        int theDimensionality2_exponent = theDimensionality2->numerator_exponent[i] - theDimensionality2->denominator_exponent[i];
        if (theDimensionality1_exponent != theDimensionality2_exponent)
            return false;
    }
    return true;
}

static bool SIDimensionalityHasExponents(SIDimensionalityRef theDimensionality,
                                         uint8_t length_numerator_exponent, uint8_t length_denominator_exponent,
                                         uint8_t mass_numerator_exponent, uint8_t mass_denominator_exponent,
                                         uint8_t time_numerator_exponent, uint8_t time_denominator_exponent,
                                         uint8_t current_numerator_exponent, uint8_t current_denominator_exponent,
                                         uint8_t temperature_numerator_exponent, uint8_t temperature_denominator_exponent,
                                         uint8_t amount_numerator_exponent, uint8_t amount_denominator_exponent,
                                         uint8_t luminous_intensity_numerator_exponent, uint8_t luminous_intensity_denominator_exponent)
{
    if (theDimensionality == NULL)
        theDimensionality = SIDimensionalityDimensionless();

    if (SIDimensionalityGetNumeratorExponentAtIndex(theDimensionality, kSILengthIndex) != length_numerator_exponent)
        return false;
    if (SIDimensionalityGetNumeratorExponentAtIndex(theDimensionality, kSIMassIndex) != mass_numerator_exponent)
        return false;
    if (SIDimensionalityGetNumeratorExponentAtIndex(theDimensionality, kSITimeIndex) != time_numerator_exponent)
        return false;
    if (SIDimensionalityGetNumeratorExponentAtIndex(theDimensionality, kSICurrentIndex) != current_numerator_exponent)
        return false;
    if (SIDimensionalityGetNumeratorExponentAtIndex(theDimensionality, kSITemperatureIndex) != temperature_numerator_exponent)
        return false;
    if (SIDimensionalityGetNumeratorExponentAtIndex(theDimensionality, kSIAmountIndex) != amount_numerator_exponent)
        return false;
    if (SIDimensionalityGetNumeratorExponentAtIndex(theDimensionality, kSILuminousIntensityIndex) != luminous_intensity_numerator_exponent)
        return false;

    if (SIDimensionalityGetDenominatorExponentAtIndex(theDimensionality, kSILengthIndex) != length_denominator_exponent)
        return false;
    if (SIDimensionalityGetDenominatorExponentAtIndex(theDimensionality, kSIMassIndex) != mass_denominator_exponent)
        return false;
    if (SIDimensionalityGetDenominatorExponentAtIndex(theDimensionality, kSITimeIndex) != time_denominator_exponent)
        return false;
    if (SIDimensionalityGetDenominatorExponentAtIndex(theDimensionality, kSICurrentIndex) != current_denominator_exponent)
        return false;
    if (SIDimensionalityGetDenominatorExponentAtIndex(theDimensionality, kSITemperatureIndex) != temperature_denominator_exponent)
        return false;
    if (SIDimensionalityGetDenominatorExponentAtIndex(theDimensionality, kSIAmountIndex) != amount_denominator_exponent)
        return false;
    if (SIDimensionalityGetDenominatorExponentAtIndex(theDimensionality, kSILuminousIntensityIndex) != luminous_intensity_denominator_exponent)
        return false;
    return true;
}

bool SIDimensionalityHasReducedExponents(SIDimensionalityRef theDimensionality,
                                         int8_t length_exponent,
                                         int8_t mass_exponent,
                                         int8_t time_exponent,
                                         int8_t current_exponent,
                                         int8_t temperature_exponent,
                                         int8_t amount_exponent,
                                         int8_t luminous_intensity_exponent)
{
    IF_NO_OBJECT_EXISTS_RETURN(theDimensionality, NULL)

    if (SIDimensionalityReducedExponentAtIndex(theDimensionality, kSILengthIndex) != length_exponent)
        return false;
    if (SIDimensionalityReducedExponentAtIndex(theDimensionality, kSIMassIndex) != mass_exponent)
        return false;
    if (SIDimensionalityReducedExponentAtIndex(theDimensionality, kSITimeIndex) != time_exponent)
        return false;
    if (SIDimensionalityReducedExponentAtIndex(theDimensionality, kSICurrentIndex) != current_exponent)
        return false;
    if (SIDimensionalityReducedExponentAtIndex(theDimensionality, kSITemperatureIndex) != temperature_exponent)
        return false;
    if (SIDimensionalityReducedExponentAtIndex(theDimensionality, kSIAmountIndex) != amount_exponent)
        return false;
    if (SIDimensionalityReducedExponentAtIndex(theDimensionality, kSILuminousIntensityIndex) != luminous_intensity_exponent)
        return false;
    return true;
}

/*
 @function SIDimensionalityHasSameDimensionlessAndDerivedDimensionalities
 @abstract Determines if the two Dimensionalities have the same dimensionless exponents,
 @param theDimensionality1 The first Dimensionality.
 @param theDimensionality2 The second Dimensionality.
 @result true or false.
 */
static bool SIDimensionalityHasSameDimensionlessAndDerivedDimensionalities(SIDimensionalityRef theDimensionality1, SIDimensionalityRef theDimensionality2)
{
    IF_NO_OBJECT_EXISTS_RETURN(theDimensionality1, NULL)
    IF_NO_OBJECT_EXISTS_RETURN(theDimensionality2, NULL)
    if (!SIDimensionalityIsDimensionlessAndDerived(theDimensionality1))
        return false;
    if (!SIDimensionalityIsDimensionlessAndDerived(theDimensionality2))
        return false;

    if (!SIDimensionalityEqual(theDimensionality1, theDimensionality2))
        return false;
    return true;
}
