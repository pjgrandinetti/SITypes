//
//  SIUnit.c
//  SITypes
//
//  Created by philip on 6/7/17.
//  Copyright © 2017 PhySy Ltd. All rights reserved.
//

#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <math.h> // For floor, isnan, erf, erfc, log, sqrt, pow, fabs, log10
#include "SILibrary.h"

static OCTypeID kSIUnitID = _kOCNotATypeID;

#define UNIT_NOT_FOUND -1

// unitsLibrary is a Singleton
OCMutableDictionaryRef unitsLibrary = NULL;
OCMutableDictionaryRef unitsQuantitiesLibrary = NULL;
OCMutableDictionaryRef unitsDimensionalitiesLibrary = NULL;
OCMutableArrayRef unitsNamesLibrary = NULL;
bool imperialVolumes = false;

// SIUnit Opaque Type
struct __SIUnit
{
    OCBase _base;

    // SIUnit Type attributes
    // Attributes needed to describe all Derived SI Units, including
    // Coherent SI Base Units, SI Base Units, & Derived Coherent SI Units
    SIDimensionalityRef dimensionality;
    SIPrefix num_prefix[7];
    SIPrefix den_prefix[7];
    OCStringRef symbol;

    // Attributes needed to describe Special SI Units and Non-SI units
    OCStringRef root_name;
    OCStringRef root_plural_name;
    OCStringRef root_symbol;
    SIPrefix root_symbol_prefix;
    bool is_special_si_symbol;
    bool allows_si_prefix;

    // Attributes needed to describe Non-SI units
    // unit must have a symbol for this value to have meaning.
    double scale_to_coherent_si;
};

#pragma mark Static Utility Functions

static bool isValidSIPrefix(SIPrefix input)
{
    switch (input)
    {
    case kSIPrefixYocto:
    case kSIPrefixZepto:
    case kSIPrefixAtto:
    case kSIPrefixFemto:
    case kSIPrefixPico:
    case kSIPrefixNano:
    case kSIPrefixMicro:
    case kSIPrefixMilli:
    case kSIPrefixCenti:
    case kSIPrefixDeci:
    case kSIPrefixNone:
    case kSIPrefixDeca:
    case kSIPrefixHecto:
    case kSIPrefixKilo:
    case kSIPrefixMega:
    case kSIPrefixGiga:
    case kSIPrefixTera:
    case kSIPrefixPeta:
    case kSIPrefixExa:
    case kSIPrefixZetta:
    case kSIPrefixYotta:
        return true;
    }
    return false;
}

SIPrefix findClosestPrefix(int input)
{
    if (input >= kSIPrefixYotta)
        return kSIPrefixYotta;
    if (input >= kSIPrefixZetta)
        return kSIPrefixZetta;
    if (input >= kSIPrefixExa)
        return kSIPrefixExa;
    if (input >= kSIPrefixPeta)
        return kSIPrefixPeta;
    if (input >= kSIPrefixTera)
        return kSIPrefixTera;
    if (input >= kSIPrefixGiga)
        return kSIPrefixGiga;
    if (input >= kSIPrefixMega)
        return kSIPrefixMega;
    if (input >= kSIPrefixKilo)
        return kSIPrefixKilo;
    if (input >= kSIPrefixHecto)
        return kSIPrefixHecto;
    if (input >= kSIPrefixDeca)
        return kSIPrefixDeca;
    if (input >= kSIPrefixNone)
        return kSIPrefixNone;
    if (input >= kSIPrefixDeci)
        return kSIPrefixDeci;
    if (input >= kSIPrefixCenti)
        return kSIPrefixCenti;
    if (input >= kSIPrefixMilli)
        return kSIPrefixMilli;
    if (input >= kSIPrefixMicro)
        return kSIPrefixMicro;
    if (input >= kSIPrefixNano)
        return kSIPrefixNano;
    if (input >= kSIPrefixPico)
        return kSIPrefixPico;
    if (input >= kSIPrefixFemto)
        return kSIPrefixFemto;
    if (input >= kSIPrefixAtto)
        return kSIPrefixAtto;
    if (input >= kSIPrefixZepto)
        return kSIPrefixZepto;
    return kSIPrefixYocto;
}

static OCStringRef prefixSymbolForSIPrefix(SIPrefix prefix)
{
    switch (prefix)
    {
    case kSIPrefixYocto:
        return STR("y");
    case kSIPrefixZepto:
        return STR("z");
    case kSIPrefixAtto:
        return STR("a");
    case kSIPrefixFemto:
        return STR("f");
    case kSIPrefixPico:
        return STR("p");
    case kSIPrefixNano:
        return STR("n");
    case kSIPrefixMicro:
        return STR("µ");
    case kSIPrefixMilli:
        return STR("m");
    case kSIPrefixCenti:
        return STR("c");
    case kSIPrefixDeci:
        return STR("d");
    case kSIPrefixNone:
        return STR("");
    case kSIPrefixDeca:
        return STR("da");
    case kSIPrefixHecto:
        return STR("h");
    case kSIPrefixKilo:
        return STR("k");
    case kSIPrefixMega:
        return STR("M");
    case kSIPrefixGiga:
        return STR("G");
    case kSIPrefixTera:
        return STR("T");
    case kSIPrefixPeta:
        return STR("P");
    case kSIPrefixExa:
        return STR("E");
    case kSIPrefixZetta:
        return STR("Z");
    case kSIPrefixYotta:
        return STR("Y");
    default:
        return NULL;
    }
}

static OCStringRef prefixNameForSIPrefix(SIPrefix prefix)
{
    switch (prefix)
    {
    case kSIPrefixYocto:
        return STR("yocto");
    case kSIPrefixZepto:
        return STR("zepto");
    case kSIPrefixAtto:
        return STR("atto");
    case kSIPrefixFemto:
        return STR("femto");
    case kSIPrefixPico:
        return STR("pico");
    case kSIPrefixNano:
        return STR("nano");
    case kSIPrefixMicro:
        return STR("micro");
    case kSIPrefixMilli:
        return STR("milli");
    case kSIPrefixCenti:
        return STR("centi");
    case kSIPrefixDeci:
        return STR("deci");
    case kSIPrefixNone:
        return STR("");
    case kSIPrefixDeca:
        return STR("deca");
    case kSIPrefixHecto:
        return STR("hecto");
    case kSIPrefixKilo:
        return STR("kilo");
    case kSIPrefixMega:
        return STR("mega");
    case kSIPrefixGiga:
        return STR("giga");
    case kSIPrefixTera:
        return STR("tera");
    case kSIPrefixPeta:
        return STR("peta");
    case kSIPrefixExa:
        return STR("exa");
    case kSIPrefixZetta:
        return STR("zeta");
    case kSIPrefixYotta:
        return STR("yotta");
    default:
        return NULL;
    }
}

static OCStringRef baseUnitRootName(const uint8_t index)
{
    switch (index)
    {
    case kSILengthIndex:
        return kSIUnitMeter;
    case kSIMassIndex:
        return kSIUnitGram;
    case kSITimeIndex:
        return kSIUnitSecond;
    case kSICurrentIndex:
        return kSIUnitAmpere;
    case kSITemperatureIndex:
        return kSIUnitKelvin;
    case kSIAmountIndex:
        return kSIUnitMole;
    case kSILuminousIntensityIndex:
        return kSIUnitCandela;
    default:
        break;
    }
    return NULL;
}

static OCStringRef baseUnitName(const uint8_t index)
{
    if (index == kSIMassIndex)
        return STR("kilogram");
    else
        return baseUnitRootName(index);
}

static OCStringRef baseUnitPluralRootName(const uint8_t index)
{

    switch (index)
    {
    case kSILengthIndex:
        return kSIUnitMeters;
    case kSIMassIndex:
        return kSIUnitGrams;
    case kSITimeIndex:
        return kSIUnitSeconds;
    case kSICurrentIndex:
        return kSIUnitAmperes;
    case kSITemperatureIndex:
        return kSIUnitKelvin;
    case kSIAmountIndex:
        return kSIUnitMoles;
    case kSILuminousIntensityIndex:
        return kSIUnitCandelas;
    default:
        break;
    }
    return NULL;
}

static OCStringRef baseUnitPluralName(const uint8_t index)
{
    if (index == kSIMassIndex)
        return STR("kilograms");
    else
        return baseUnitPluralRootName(index);
}

static OCStringRef baseUnitRootSymbol(const uint8_t index)
{
    switch (index)
    {
    case kSILengthIndex:
        return STR("m");
    case kSIMassIndex:
        return STR("g");
    case kSITimeIndex:
        return STR("s");
    case kSICurrentIndex:
        return STR("A");
    case kSITemperatureIndex:
        return STR("K");
    case kSIAmountIndex:
        return STR("mol");
    case kSILuminousIntensityIndex:
        return STR("cd");
    default:
        break;
    }
    return NULL;
}

static OCStringRef baseUnitSymbol(const uint8_t index)
{
    if (index == kSIMassIndex)
        return STR("kg");
    else
        return baseUnitRootSymbol(index);
}

bool __SIUnitEqual(const void *theType1, const void *theType2)
{
    IF_NO_OBJECT_EXISTS_RETURN(theType1, false)
    IF_NO_OBJECT_EXISTS_RETURN(theType2, false)

    SIUnitRef theUnit1 = (SIUnitRef)theType1;
    SIUnitRef theUnit2 = (SIUnitRef)theType2;

    if (theUnit1->_base.typeID != theUnit2->_base.typeID)
        return false;
    if (theUnit1 == theUnit2)
        return true;
    if (!OCTypeEqual(theUnit1->dimensionality, theUnit2->dimensionality))
        return false;

    for (int index = 0; index < 7; index++)
    {
        if (theUnit1->num_prefix[index] != theUnit2->num_prefix[index])
            return false;
        if (theUnit1->den_prefix[index] != theUnit2->den_prefix[index])
            return false;
    }
    if (!OCTypeEqual(theUnit1->symbol, theUnit2->symbol))
        return false;
    if (!OCTypeEqual(theUnit1->root_name, theUnit2->root_name))
        return false;
    if (!OCTypeEqual(theUnit1->root_plural_name, theUnit2->root_plural_name))
        return false;
    if (!OCTypeEqual(theUnit1->root_symbol, theUnit2->root_symbol))
        return false;

    if (theUnit1->root_symbol_prefix != theUnit2->root_symbol_prefix)
        return false;
    if (theUnit1->is_special_si_symbol != theUnit2->is_special_si_symbol)
        return false;
    if (theUnit1->scale_to_coherent_si != theUnit2->scale_to_coherent_si)
        return false;
    if (theUnit1->scale_to_coherent_si != theUnit2->scale_to_coherent_si)
        return false;

    return true;
}

void __SIUnitFinalize(const void *theType)
{
    if (NULL == theType)
        return;

    SIUnitRef theUnit = (SIUnitRef)theType;

    // Release dynamically allocated members
    if (theUnit->dimensionality)
        OCRelease(theUnit->dimensionality);

    if (theUnit->symbol)
        OCRelease(theUnit->symbol);

    if (theUnit->root_name)
        OCRelease(theUnit->root_name);

    if (theUnit->root_plural_name)
        OCRelease(theUnit->root_plural_name);

    if (theUnit->root_symbol)
        OCRelease(theUnit->root_symbol);

    // Free the structure itself
    free((void *)theUnit);
}

static OCStringRef __SIUnitCopyFormattingDescription(OCTypeRef theType)
{
    SIUnitRef theUnit = (SIUnitRef)theType;
    return (OCStringRef)OCStringCreateCopy(theUnit->symbol);
}

OCTypeID SIUnitGetTypeID(void)
{
    if (kSIUnitID == _kOCNotATypeID)
        kSIUnitID = OCRegisterType("SIUnit");
    return kSIUnitID;
}

static struct __SIUnit *SIUnitAllocate()
{
    struct __SIUnit *theUnit = malloc(sizeof(struct __SIUnit));
    if (NULL == theUnit)
        return NULL;
    theUnit->_base.typeID = SIUnitGetTypeID();
    theUnit->_base.static_instance = false;
    theUnit->_base.finalize = __SIUnitFinalize;
    theUnit->_base.equal = __SIUnitEqual;
    theUnit->_base.copyFormattingDesc = __SIUnitCopyFormattingDescription;
    theUnit->_base.retainCount = 0;
    theUnit->dimensionality = NULL;
    for (int index = 0; index < 7; index++)
    {
        theUnit->num_prefix[index] = kSIPrefixNone;
        theUnit->den_prefix[index] = kSIPrefixNone;
    }
    theUnit->scale_to_coherent_si = 1.;
    theUnit->allows_si_prefix = false;
    theUnit->is_special_si_symbol = false;
    theUnit->root_symbol_prefix = kSIPrefixNone;

    theUnit->symbol = NULL;
    theUnit->root_name = NULL;
    theUnit->root_plural_name = NULL;
    theUnit->root_symbol = NULL;
    OCRetain(theUnit);
    return theUnit;
}

bool SIUnitIsSIBaseUnit(SIUnitRef theUnit)
{
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, false)

    // Non-SI units return false
    if (theUnit->scale_to_coherent_si != 1.)
        return false;

    // To be a base unit all the denominator exponents must be 0
    // and all numerator exponents are zero except one, which is 1
    if (theUnit->root_symbol == NULL)
    {
        for (uint8_t i = 0; i < 7; i++)
            if (SIDimensionalityGetDenExpAtIndex(theUnit->dimensionality, i) != 0)
                return false;
        int count = 0;
        for (uint8_t i = 0; i < 7; i++)
        {
            uint8_t numExp = SIDimensionalityGetNumExpAtIndex(theUnit->dimensionality, i);
            if (numExp > 1)
                return false;
            if (numExp < 0)
                return false;
            if (numExp == 1)
                count++;
        }
        if (count == 1)
            return true;
    }
    return false;
}

#pragma mark Accessors

SIDimensionalityRef SIUnitGetDimensionality(SIUnitRef theUnit)
{
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, NULL)
    return theUnit->dimensionality;
}

OCStringRef SIUnitCopyRootName(SIUnitRef theUnit) {
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, NULL)

    OCStringRef result = NULL;

    if (SIUnitIsSIBaseUnit(theUnit)) {
        for (uint8_t i = 0; i <= 6; i++) {
            if (SIDimensionalityGetNumExpAtIndex(theUnit->dimensionality, i)) {
                result = OCStringCreateCopy(baseUnitRootName(i));
                break;
            }
        }
    } else {
        if (theUnit->root_name) {
            result = OCStringCreateCopy(theUnit->root_name);
        }
    }

    if (!result) {
        result = STR("");
    }

    return result;
}


OCStringRef SIUnitCopyRootPluralName(SIUnitRef theUnit)
{
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, NULL)

    if (SIUnitIsSIBaseUnit(theUnit))
    {
        for (uint8_t i = 0; i < 7; i++)
            if (SIDimensionalityGetNumExpAtIndex(theUnit->dimensionality, i))
                return OCStringCreateCopy(baseUnitPluralRootName(i));
    }
    else
    {
        if (theUnit->root_plural_name)
            return OCStringCreateCopy(theUnit->root_plural_name);
    }
    return STR("");
}

OCStringRef SIUnitCopyRootSymbol(SIUnitRef theUnit)
{
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, NULL)

    if (SIUnitIsSIBaseUnit(theUnit))
    {
        for (uint8_t i = 0; i < 7; i++)
            if (SIDimensionalityGetNumExpAtIndex(theUnit->dimensionality, i))
                return OCStringCreateCopy(baseUnitRootSymbol(i));
    }
    else
    {
        if (theUnit->root_symbol)
            return OCStringCreateCopy(theUnit->root_symbol);
    }
    return NULL;
}

bool SIUnitAllowsSIPrefix(SIUnitRef theUnit)
{
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, false)

    return theUnit->allows_si_prefix;
}

SIPrefix SIUnitCopyRootSymbolPrefix(SIUnitRef theUnit)
{
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, 0)

    return theUnit->root_symbol_prefix;
}

double SIUnitGetScaleNonSIToCoherentSI(SIUnitRef theUnit)
{
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, 0)

    return theUnit->scale_to_coherent_si;
}

bool SIUnitGetIsSpecialSISymbol(SIUnitRef theUnit)
{
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, false)

    return theUnit->is_special_si_symbol;
}

SIPrefix SIUnitGetNumeratorPrefixAtIndex(SIUnitRef theUnit, const uint8_t index)
{
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, 0)

    return theUnit->num_prefix[index];
}

SIPrefix SIUnitGetDenominatorPrefixAtIndex(SIUnitRef theUnit, const uint8_t index)
{
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, 0)

    return theUnit->den_prefix[index];
}

bool SIUnitIsDimensionlessAndUnderived(SIUnitRef theUnit)
{
    if (!SIDimensionalityIsDimensionlessAndNotDerived(theUnit->dimensionality))
        return false;
    if (theUnit->root_name != NULL)
        return false;
    return true;
}

OCStringRef SIUnitCopySymbol(SIUnitRef theUnit)
{
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, NULL)

    // Construct the symbol of the unit from root_symbol and prefix.

    if (SIUnitIsDimensionlessAndUnderived(theUnit))
    {
        return OCStringCreateCopy(STR(" "));
    }

    // Symbol should be generated and saved when unit is created.
    return OCStringCreateCopy(theUnit->symbol);
}

OCStringRef SIUnitCreateDerivedSymbol(SIUnitRef theUnit)
{
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, NULL)
    /*
     *	This routine constructs a unit symbol in terms of the seven base unit symbols
     *	including their SI Prefix units.
     *
     *  This routine will not substitute any special SI symbols.
     */

    // Note:  This doesn't work for prefixed Special SI units or Non-SI units.
    // In both those cases, this routine would need to return a numerical value multiplier
    // so the quantity can be expressed correctly in the units of the derived symbol.
    // Therefore, we return the special SI or Non-Si unit prefixed symbol

    if (theUnit->root_symbol)
        return OCStringCreateCopy(theUnit->symbol);

    OCMutableStringRef numerator = OCStringCreateMutable(0);
    OCMutableStringRef denominator = OCStringCreateMutable(0);
    bool denominator_multiple_units = false;

    // Numerator
    uint8_t exponent = SIDimensionalityGetNumExpAtIndex(theUnit->dimensionality, 0);
    if (exponent > 0)
    {
        OCStringRef prefix = prefixSymbolForSIPrefix(SIUnitGetNumeratorPrefixAtIndex(theUnit, 0));
        if (exponent != 1)
            OCStringAppendFormat(numerator, STR("%@%@^%d"), prefix, baseUnitRootSymbol(0), exponent);
        else
            OCStringAppendFormat(numerator, STR("%@%@"), prefix, baseUnitRootSymbol(0));
    }
    for (uint8_t index = 1; index < 7; index++)
    {
        exponent = SIDimensionalityGetNumExpAtIndex(theUnit->dimensionality, index);
        if (exponent > 0)
        {
            OCStringRef prefix = prefixSymbolForSIPrefix(SIUnitGetNumeratorPrefixAtIndex(theUnit, index));
            if (OCStringGetLength(numerator) == 0)
            {
                if (exponent != 1)
                    OCStringAppendFormat(numerator, STR("%@%@^%d"), prefix, baseUnitRootSymbol(index), exponent);
                else
                    OCStringAppendFormat(numerator, STR("%@%@"), prefix, baseUnitRootSymbol(index));
            }
            else
            {
                if (exponent != 1)
                    OCStringAppendFormat(numerator, STR("•%@%@^%d"), prefix, baseUnitRootSymbol(index), exponent);
                else
                    OCStringAppendFormat(numerator, STR("•%@%@"), prefix, baseUnitRootSymbol(index));
            }
        }
    }

    // Denominator
    exponent = SIDimensionalityGetDenExpAtIndex(theUnit->dimensionality, 0);
    if (exponent > 0)
    {
        OCStringRef prefix = prefixSymbolForSIPrefix(SIUnitGetDenominatorPrefixAtIndex(theUnit, 0));
        if (exponent != 1)
            OCStringAppendFormat(denominator, STR("%@%@^%d"), prefix, baseUnitRootSymbol(0), exponent);
        else
            OCStringAppendFormat(denominator, STR("%@%@"), prefix, baseUnitRootSymbol(0));
    }
    for (uint8_t index = 1; index < 7; index++)
    {

        exponent = SIDimensionalityGetDenExpAtIndex(theUnit->dimensionality, index);
        if (exponent > 0)
        {
            OCStringRef prefix = prefixSymbolForSIPrefix(SIUnitGetDenominatorPrefixAtIndex(theUnit, index));
            if (OCStringGetLength(denominator) == 0)
            {
                if (exponent != 1)
                    OCStringAppendFormat(denominator, STR("%@%@^%d"), prefix, baseUnitRootSymbol(index), exponent);
                else
                    OCStringAppendFormat(denominator, STR("%@%@"), prefix, baseUnitRootSymbol(index));
            }
            else
            {
                denominator_multiple_units = true;
                if (exponent != 1)
                    OCStringAppendFormat(denominator, STR("•%@%@^%d"), prefix, baseUnitRootSymbol(index), exponent);
                else
                    OCStringAppendFormat(denominator, STR("•%@%@"), prefix, baseUnitRootSymbol(index));
            }
        }
    }

    if (OCStringGetLength(numerator) != 0)
    {
        if (OCStringGetLength(denominator) != 0)
        {
            OCStringRef symbol;
            if (denominator_multiple_units)
                symbol = OCStringCreateWithFormat(STR("%@/(%@)"), numerator, denominator);
            else
                symbol = OCStringCreateWithFormat(STR("%@/%@"), numerator, denominator);
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
            if (denominator_multiple_units)
                symbol = OCStringCreateWithFormat(NULL, NULL, STR("(1/(%@))"), denominator);
            else
                symbol = OCStringCreateWithFormat(STR("(1/%@)"), denominator);
            OCRelease(numerator);
            OCRelease(denominator);
            return symbol;
        }
        else
        {
            OCRelease(numerator);
            OCRelease(denominator);
            return STR(" ");
        }
    }
}

/*
 @function SIUnitCreate
 @abstract Creates a new SIUnit object
 @param dimensionality dimensionality of the unit
 @param length_num_prefix integer exponent associated with SI prefix of length coherent base unit
 @param length_den_prefix integer exponent associated with SI prefix of length coherent base unit
 @param mass_num_prefix integer exponent associated with SI prefix of mass coherent base unit
 @param mass_den_prefix integer exponent associated with SI prefix of mass coherent base unit
 @param time_num_prefix integer exponent associated with SI prefix of time coherent base unit
 @param time_den_prefix integer exponent associated with SI prefix of time coherent base unit
 @param current_num_prefix integer exponent associated with SI prefix of current coherent base unit
 @param current_den_prefix integer exponent associated with SI prefix of current coherent base unit
 @param temperature_num_prefix integer exponent associated with SI prefix of temperature coherent base unit
 @param temperature_den_prefix integer exponent associated with SI prefix of temperature coherent base unit
 @param amount_num_prefix integer exponent associated with SI prefix of amount coherent base unit
 @param amount_den_prefix integer exponent associated with SI prefix of amount coherent base unit
 @param luminous_intensity_num_prefix integer exponent associated with SI prefix of luminous intensity coherent base unit
 @param luminous_intensity_den_prefix integer exponent associated with SI prefix of luminous intensity coherent base unit
 @param root_name the root name for the unit, such as Kelvin or mole, which can be prefixed with prefixes such as kilo, milli, etc.
 @param root_plural_name the plural version of the root name for the unit.
 @param root_symbol the symbol for the root name for the unit.
 @param root_symbol_prefix the integer exponent associated with SI prefix for the root unit.
 @param allows_si_prefix true if SI prefix can be used with root symbol
 @param is_special_si_symbol true for valid SI base symbols, false if base_symbol is non-SI symbol or if the base_name is NULL
 @param scale_to_coherent_si scaling of the Non-SI root unit
 to the coherent SI base unit or coherent derived SI unit with the
 same dimensionality.
 @result SIUnit object
 @discussion If unit is given a root name and symbol, then prefixes are allowed for this name and symbol, and the prefixes associated
 with the 7 dimensions are ignored.
 */
static SIUnitRef SIUnitCreate(SIDimensionalityRef dimensionality,
                              SIPrefix length_num_prefix, SIPrefix length_den_prefix,
                              SIPrefix mass_num_prefix, SIPrefix mass_den_prefix,
                              SIPrefix time_num_prefix, SIPrefix time_den_prefix,
                              SIPrefix current_num_prefix, SIPrefix current_den_prefix,
                              SIPrefix temperature_num_prefix, SIPrefix temperature_den_prefix,
                              SIPrefix amount_num_prefix, SIPrefix amount_den_prefix,
                              SIPrefix luminous_intensity_num_prefix, SIPrefix luminous_intensity_den_prefix,
                              OCStringRef root_name,
                              OCStringRef root_plural_name,
                              OCStringRef root_symbol,
                              SIPrefix root_symbol_prefix,
                              bool allows_si_prefix,
                              bool is_special_si_symbol,
                              double scale_to_coherent_si)
{
    // Initialize object
    struct __SIUnit *theUnit = SIUnitAllocate();
    if (NULL == theUnit)
        return NULL;

    theUnit->allows_si_prefix = allows_si_prefix;

    //  setup attributes
    if (root_symbol == NULL)
    {
        // Only derived SI units are allowed to have no symbol
        if (is_special_si_symbol)
        {
            // Can't be valid SI symbol if there's no symbol
            OCRelease(theUnit);
            fprintf(stderr, "*** ERROR - %s %s\n", __FILE__, __func__);
            fprintf(stderr, "          - can't be valid SI symbol if there's no symbol.\n");
            fprintf(stderr, "          - is_special_si_symbol = %d instead of 0\n", is_special_si_symbol);
            return NULL;
        }
        if (scale_to_coherent_si != 1.)
        {
            // non-SI units are not allowed to have no symbol
            OCRelease(theUnit);
            //            SIUnitRelease(kOCAllocatorDefault,theUnit);
            fprintf(stderr, "*** ERROR - %s %s\n", __FILE__, __func__);
            fprintf(stderr, "          - Only derived SI units are allowed to have no symbol.\n");
            fprintf(stderr, "          - scale_to_coherent_si = %g instead of 1.\n", scale_to_coherent_si);
            return NULL;
        }
        if (root_symbol_prefix)
        {
            // no prefix possible if no symbol
            OCRelease(theUnit);
            //            SIUnitRelease(kOCAllocatorDefault,theUnit);
            fprintf(stderr, "*** ERROR - %s %s\n", __FILE__, __func__);
            fprintf(stderr, "          - Trying to use SI prefix with no unit symbol.\n");
            return NULL;
        }
        theUnit->is_special_si_symbol = false;
        theUnit->root_symbol_prefix = 0;
        theUnit->scale_to_coherent_si = 1.0;
    }
    else
    {
        if (!isValidSIPrefix(root_symbol_prefix))
        {
            OCRelease(theUnit);
            //            SIUnitRelease(kOCAllocatorDefault,theUnit);
            fprintf(stderr, "*** ERROR - %s %s\n", __FILE__, __func__);
            fprintf(stderr, "          - SI prefix request invalid: symbol_prefix = %d\n", root_symbol_prefix);
            return NULL;
        }
        if (is_special_si_symbol)
        {
            if (scale_to_coherent_si != 1.)
            {
                OCRelease(theUnit);
                //            SIUnitRelease(kOCAllocatorDefault,theUnit);
                fprintf(stderr, "*** ERROR - %s %s\n", __FILE__, __func__);
                fprintf(stderr, "          - can't be special SI symbol if scale_to_coherent_si = %g instead of 1.\n", scale_to_coherent_si);
                return NULL;
            }
            theUnit->is_special_si_symbol = true;
            theUnit->root_symbol_prefix = root_symbol_prefix;
            theUnit->scale_to_coherent_si = 1.0;
        }
        else
        {
            theUnit->is_special_si_symbol = false;
            theUnit->root_symbol_prefix = root_symbol_prefix;
            theUnit->scale_to_coherent_si = scale_to_coherent_si;
        }
    }

    if (!isValidSIPrefix(length_num_prefix))
    {
        OCRelease(theUnit);
        //            SIUnitRelease(kOCAllocatorDefault,theUnit);
        fprintf(stderr, "*** ERROR - %s %s\n", __FILE__, __func__);
        fprintf(stderr, "          - SI prefix request invalid: length_num_prefix = %d\n", length_num_prefix);
        return NULL;
    }

    if (!isValidSIPrefix(length_den_prefix))
    {
        OCRelease(theUnit);
        //            SIUnitRelease(kOCAllocatorDefault,theUnit);
        fprintf(stderr, "*** ERROR - %s %s\n", __FILE__, __func__);
        fprintf(stderr, "          - SI prefix request invalid: length_den_prefix = %d\n", length_den_prefix);
        return NULL;
    }

    if (!isValidSIPrefix(mass_num_prefix))
    {
        OCRelease(theUnit);
        //            SIUnitRelease(kOCAllocatorDefault,theUnit);
        fprintf(stderr, "*** ERROR - %s %s\n", __FILE__, __func__);
        fprintf(stderr, "          - SI prefix request invalid: mass_num_prefix = %d\n", mass_num_prefix);
        return NULL;
    }

    if (!isValidSIPrefix(mass_den_prefix))
    {
        OCRelease(theUnit);
        //            SIUnitRelease(kOCAllocatorDefault,theUnit);
        fprintf(stderr, "*** ERROR - %s %s\n", __FILE__, __func__);
        fprintf(stderr, "          - SI prefix request invalid: mass_denominator__prefix = %d\n", mass_den_prefix);
        return NULL;
    }

    if (!isValidSIPrefix(time_num_prefix))
    {
        OCRelease(theUnit);
        //            SIUnitRelease(kOCAllocatorDefault,theUnit);
        fprintf(stderr, "*** ERROR - %s %s\n", __FILE__, __func__);
        fprintf(stderr, "          - SI prefix request invalid: time_num_prefix = %d\n", time_num_prefix);
        return NULL;
    }

    if (!isValidSIPrefix(time_den_prefix))
    {
        OCRelease(theUnit);
        //            SIUnitRelease(kOCAllocatorDefault,theUnit);
        fprintf(stderr, "*** ERROR - %s %s\n", __FILE__, __func__);
        fprintf(stderr, "          - SI prefix request invalid: time_den_prefix = %d\n", time_den_prefix);
        return NULL;
    }

    if (!isValidSIPrefix(current_num_prefix))
    {
        OCRelease(theUnit);
        //            SIUnitRelease(kOCAllocatorDefault,theUnit);
        fprintf(stderr, "*** ERROR - %s %s\n", __FILE__, __func__);
        fprintf(stderr, "          - SI prefix request invalid: current_num_prefix = %d\n", current_num_prefix);
        return NULL;
    }

    if (!isValidSIPrefix(current_den_prefix))
    {
        OCRelease(theUnit);
        //            SIUnitRelease(kOCAllocatorDefault,theUnit);
        fprintf(stderr, "*** ERROR - %s %s\n", __FILE__, __func__);
        fprintf(stderr, "          - SI prefix request invalid: current_den_prefix = %d\n", current_den_prefix);
        return NULL;
    }

    if (!isValidSIPrefix(temperature_num_prefix))
    {
        OCRelease(theUnit);
        //            SIUnitRelease(kOCAllocatorDefault,theUnit);
        fprintf(stderr, "*** ERROR - %s %s\n", __FILE__, __func__);
        fprintf(stderr, "          - SI prefix request invalid: temperature_num_prefix = %d\n", temperature_num_prefix);
        return NULL;
    }

    if (!isValidSIPrefix(temperature_den_prefix))
    {
        OCRelease(theUnit);
        //            SIUnitRelease(kOCAllocatorDefault,theUnit);
        fprintf(stderr, "*** ERROR - %s %s\n", __FILE__, __func__);
        fprintf(stderr, "          - SI prefix request invalid: temperature_den_prefix = %d\n", temperature_den_prefix);
        return NULL;
    }

    if (!isValidSIPrefix(luminous_intensity_num_prefix))
    {
        OCRelease(theUnit);
        //            SIUnitRelease(kOCAllocatorDefault,theUnit);
        fprintf(stderr, "*** ERROR - %s %s\n", __FILE__, __func__);
        fprintf(stderr, "          - SI prefix request invalid: luminous_intensity_num_prefix = %d\n", luminous_intensity_num_prefix);
        return NULL;
    }

    if (!isValidSIPrefix(luminous_intensity_den_prefix))
    {
        OCRelease(theUnit);
        //            SIUnitRelease(kOCAllocatorDefault,theUnit);
        fprintf(stderr, "*** ERROR - %s %s\n", __FILE__, __func__);
        fprintf(stderr, "          - SI prefix request invalid: luminous_intensity_den_prefix = %d\n", luminous_intensity_den_prefix);
        return NULL;
    }

    if (root_name)
        theUnit->root_name = OCStringCreateCopy(root_name);
    if (root_plural_name)
        theUnit->root_plural_name = OCStringCreateCopy(root_plural_name);
    if (root_symbol)
        theUnit->root_symbol = OCStringCreateCopy(root_symbol);
    theUnit->dimensionality = dimensionality;
    theUnit->num_prefix[kSILengthIndex] = length_num_prefix;
    theUnit->num_prefix[kSIMassIndex] = mass_num_prefix;
    theUnit->num_prefix[kSITimeIndex] = time_num_prefix;
    theUnit->num_prefix[kSICurrentIndex] = current_num_prefix;
    theUnit->num_prefix[kSITemperatureIndex] = temperature_num_prefix;
    theUnit->num_prefix[kSIAmountIndex] = amount_num_prefix;
    theUnit->num_prefix[kSILuminousIntensityIndex] = luminous_intensity_num_prefix;

    theUnit->den_prefix[kSILengthIndex] = length_den_prefix;
    theUnit->den_prefix[kSIMassIndex] = mass_den_prefix;
    theUnit->den_prefix[kSITimeIndex] = time_den_prefix;
    theUnit->den_prefix[kSICurrentIndex] = current_den_prefix;
    theUnit->den_prefix[kSITemperatureIndex] = temperature_den_prefix;
    theUnit->den_prefix[kSIAmountIndex] = amount_den_prefix;
    theUnit->den_prefix[kSILuminousIntensityIndex] = luminous_intensity_den_prefix;

    {
        if (SIUnitIsSIBaseUnit(theUnit))
        {
            // The root_symbol attribute is empty for the seven base units, so we need to ask
            // baseUnitRootSymbol for its root_symbol
            for (int i = 0; i < 7; i++)
            {
                if (SIDimensionalityGetNumExpAtIndex(theUnit->dimensionality, i))
                {
                    // Only one numerator_exponent will be non-zero (and 1).
                    OCMutableStringRef name = OCStringCreateMutable(0);
                    OCStringRef prefix_string = prefixSymbolForSIPrefix(theUnit->num_prefix[i]);
                    OCStringRef name_string = baseUnitRootSymbol(i);
                    OCStringAppend(name, prefix_string);
                    OCStringAppend(name, name_string);
                    theUnit->symbol = name;
                }
            }
        }
        else
        {
            if (theUnit->root_symbol)
            {
                OCMutableStringRef name = OCStringCreateMutable(0);
                OCStringRef prefix_string = prefixSymbolForSIPrefix(theUnit->root_symbol_prefix);
                OCStringAppend(name, prefix_string);
                OCStringAppend(name, theUnit->root_symbol);

                theUnit->symbol = name;
            }
            else
            {
                OCStringRef symbol = SIUnitCreateDerivedSymbol(theUnit);
                theUnit->symbol = symbol;
            }
        }
    }
    return (SIUnitRef)theUnit;
}

static bool AddAllSIPrefixedUnitsToLibrary(SIUnitRef rootUnit, OCStringRef quantity);

static SIUnitRef AddUnitForQuantityToLibrary(OCStringRef quantity,
                                             SIPrefix length_num_prefix, SIPrefix length_den_prefix,
                                             SIPrefix mass_num_prefix, SIPrefix mass_den_prefix,
                                             SIPrefix time_num_prefix, SIPrefix time_den_prefix,
                                             SIPrefix current_num_prefix, SIPrefix current_den_prefix,
                                             SIPrefix temperature_num_prefix, SIPrefix temperature_den_prefix,
                                             SIPrefix amount_num_prefix, SIPrefix amount_den_prefix,
                                             SIPrefix luminous_intensity_num_prefix, SIPrefix luminous_intensity_den_prefix,
                                             OCStringRef root_name,
                                             OCStringRef root_plural_name,
                                             OCStringRef root_symbol,
                                             SIPrefix root_symbol_prefix,
                                             bool is_special_si_symbol,
                                             double scale_to_coherent_si,
                                             bool allows_si_prefix)
{

    SIDimensionalityRef theDim = SIDimensionalityForQuantity(quantity, NULL);
    SIUnitRef unit = SIUnitCreate(theDim,
                                  length_num_prefix, length_den_prefix,
                                  mass_num_prefix, mass_den_prefix,
                                  time_num_prefix, time_den_prefix,
                                  current_num_prefix, current_den_prefix,
                                  temperature_num_prefix, temperature_den_prefix,
                                  amount_num_prefix, amount_den_prefix,
                                  luminous_intensity_num_prefix, luminous_intensity_den_prefix,
                                  root_name,
                                  root_plural_name,
                                  root_symbol,
                                  root_symbol_prefix,
                                  allows_si_prefix,
                                  is_special_si_symbol,
                                  scale_to_coherent_si);

    // Add unit to units library dictionary
    if (OCDictionaryContainsKey(unitsLibrary, unit->symbol))
    {
        fprintf(stderr, "WARNING - Cannot add unit to library because symbol already is present\n");
        OCStringShow(unit->symbol);
        OCRelease(unit);
        return OCDictionaryGetValue(unitsLibrary, unit->symbol);
    }
    OCTypeSetStaticInstance(unit, true);
    OCDictionaryAddValue(unitsLibrary, unit->symbol, unit);
    OCRelease(unit);

    // Append unit to mutable array value associated with quantity key inside quanity library dictionary
    {
        OCMutableArrayRef units = (OCMutableArrayRef)OCDictionaryGetValue(unitsQuantitiesLibrary, quantity);
        if (units)
            OCArrayAppendValue(units, unit);
        else
        {
            units = OCArrayCreateMutable(0, &kOCTypeArrayCallBacks);
            OCArrayAppendValue(units, unit);
            OCDictionaryAddValue(unitsQuantitiesLibrary, quantity, units);
            OCRelease(units);
        }
    }

    // Append unit to mutable array value associated with dimensionality key inside dimensionality library dictionary
    {
        OCStringRef dimensionalitySymbol = SIDimensionalityGetSymbol(theDim);
        OCMutableArrayRef units = (OCMutableArrayRef)OCDictionaryGetValue(unitsDimensionalitiesLibrary, dimensionalitySymbol);
        if (units)
            OCArrayAppendValue(units, unit);
        else
        {
            units = OCArrayCreateMutable(0, &kOCTypeArrayCallBacks);
            OCArrayAppendValue(units, unit);
            OCDictionaryAddValue(unitsDimensionalitiesLibrary, dimensionalitySymbol, units);
            OCRelease(units);
        }
    }

    if (allows_si_prefix)
        AddAllSIPrefixedUnitsToLibrary(unit, quantity);
    return unit;
}

static bool AddAllSIPrefixedUnitsToLibrary(SIUnitRef rootUnit, OCStringRef quantity)
{
    IF_NO_OBJECT_EXISTS_RETURN(rootUnit, false)

    // Table 5 - SI Prefixes
    SIPrefix prefix[21] = {
        kSIPrefixYocto,
        kSIPrefixZepto,
        kSIPrefixAtto,
        kSIPrefixFemto,
        kSIPrefixPico,
        kSIPrefixNano,
        kSIPrefixMicro,
        kSIPrefixMilli,
        kSIPrefixCenti,
        kSIPrefixDeci,
        kSIPrefixNone,
        kSIPrefixDeca,
        kSIPrefixHecto,
        kSIPrefixKilo,
        kSIPrefixMega,
        kSIPrefixGiga,
        kSIPrefixTera,
        kSIPrefixPeta,
        kSIPrefixExa,
        kSIPrefixZetta,
        kSIPrefixYotta};

    OCStringRef root_name = SIUnitCopyRootName(rootUnit);
    if (root_name)
    {
        for (int iPrefix = 0; iPrefix < 21; iPrefix++)
        {
            if (prefix[iPrefix] != kSIPrefixNone)
            {
                if (SIUnitIsSIBaseUnit(rootUnit))
                {
                    SIDimensionalityRef dimensionality = SIUnitGetDimensionality(rootUnit);
                    SIPrefix num_prefixes[7];
                    SIPrefix den_prefixes[7];
                    for (int8_t i = 0; i < 7; i++)
                    {
                        den_prefixes[i] = kSIPrefixNone;
                        if (SIDimensionalityGetNumExpAtIndex(dimensionality, i) == 1)
                            num_prefixes[i] = prefix[iPrefix];
                        else
                            num_prefixes[i] = kSIPrefixNone;
                    }
                    AddUnitForQuantityToLibrary(quantity,
                                                num_prefixes[kSILengthIndex],
                                                den_prefixes[kSILengthIndex],
                                                num_prefixes[kSIMassIndex],
                                                den_prefixes[kSIMassIndex],
                                                num_prefixes[kSITimeIndex],
                                                den_prefixes[kSITimeIndex],
                                                num_prefixes[kSICurrentIndex],
                                                den_prefixes[kSICurrentIndex],
                                                num_prefixes[kSITemperatureIndex],
                                                den_prefixes[kSITemperatureIndex],
                                                num_prefixes[kSIAmountIndex],
                                                den_prefixes[kSIAmountIndex],
                                                num_prefixes[kSILuminousIntensityIndex],
                                                den_prefixes[kSILuminousIntensityIndex],
                                                rootUnit->root_name,
                                                rootUnit->root_plural_name,
                                                rootUnit->root_symbol,
                                                rootUnit->root_symbol_prefix,
                                                rootUnit->is_special_si_symbol,
                                                rootUnit->scale_to_coherent_si, false);
                }
                else if (rootUnit->root_symbol)
                {
                    AddUnitForQuantityToLibrary(quantity,
                                                rootUnit->num_prefix[kSILengthIndex],
                                                rootUnit->den_prefix[kSILengthIndex],
                                                rootUnit->num_prefix[kSIMassIndex],
                                                rootUnit->den_prefix[kSIMassIndex],
                                                rootUnit->num_prefix[kSITimeIndex],
                                                rootUnit->den_prefix[kSITimeIndex],
                                                rootUnit->num_prefix[kSICurrentIndex],
                                                rootUnit->den_prefix[kSICurrentIndex],
                                                rootUnit->num_prefix[kSITemperatureIndex],
                                                rootUnit->den_prefix[kSITemperatureIndex],
                                                rootUnit->num_prefix[kSIAmountIndex],
                                                rootUnit->den_prefix[kSIAmountIndex],
                                                rootUnit->num_prefix[kSILuminousIntensityIndex],
                                                rootUnit->den_prefix[kSILuminousIntensityIndex],
                                                rootUnit->root_name,
                                                rootUnit->root_plural_name,
                                                rootUnit->root_symbol,
                                                prefix[iPrefix],
                                                rootUnit->is_special_si_symbol,
                                                rootUnit->scale_to_coherent_si, false);
                }
            }
        }
        OCRelease(root_name); // Release the dynamically allocated root_name
    }

    return true;
}



static void AddNonSIUnitToLibrary(OCStringRef quantity, OCStringRef name, OCStringRef pluralName, OCStringRef symbol, double scale_to_coherent_si)
{
    AddUnitForQuantityToLibrary(quantity,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                name, pluralName, symbol, kSIPrefixNone, false, scale_to_coherent_si, false);
}

void UnitsLibraryCreate();

SIUnitRef SIUnitForUnderivedSymbol(OCStringRef symbol)
{
    if (NULL == symbol)
    {
        return NULL;
    }
    if (NULL == unitsLibrary)
        UnitsLibraryCreate();
    IF_NO_OBJECT_EXISTS_RETURN(unitsLibrary, NULL)

    SIUnitRef unit = OCDictionaryGetValue(unitsLibrary, symbol);
    return unit;
}

bool SIUnitsLibraryRemoveUnitWithSymbol(OCStringRef symbol)
{
    if (NULL == unitsLibrary)
        UnitsLibraryCreate();

    if (OCDictionaryContainsKey(unitsLibrary, symbol))
    {
        SIUnitRef unit = (SIUnitRef)OCDictionaryGetValue(unitsLibrary, symbol);
        OCDictionaryRemoveValue(unitsLibrary, symbol);
        OCTypeSetStaticInstance(unit, false);
        OCRelease(unit);
        return true;
    }
    return false;
}

void SIUnitsLibrarySetImperialVolumes(bool value)
{
    if (imperialVolumes == value)
        return;

    SIUnitsLibraryRemoveUnitWithSymbol(STR("gal"));
    SIUnitsLibraryRemoveUnitWithSymbol(STR("qt"));
    SIUnitsLibraryRemoveUnitWithSymbol(STR("pt"));
    SIUnitsLibraryRemoveUnitWithSymbol(STR("cup"));
    SIUnitsLibraryRemoveUnitWithSymbol(STR("gi"));
    SIUnitsLibraryRemoveUnitWithSymbol(STR("floz"));
    SIUnitsLibraryRemoveUnitWithSymbol(STR("tbsp"));
    SIUnitsLibraryRemoveUnitWithSymbol(STR("tsp"));
    SIUnitsLibraryRemoveUnitWithSymbol(STR("halftsp"));
    SIUnitsLibraryRemoveUnitWithSymbol(STR("quartertsp"));

    SIUnitsLibraryRemoveUnitWithSymbol(STR("(1/gal)"));
    SIUnitsLibraryRemoveUnitWithSymbol(STR("(1/qt)"));
    SIUnitsLibraryRemoveUnitWithSymbol(STR("(1/pt)"));
    SIUnitsLibraryRemoveUnitWithSymbol(STR("(1/cup)"));
    SIUnitsLibraryRemoveUnitWithSymbol(STR("(1/gi)"));
    SIUnitsLibraryRemoveUnitWithSymbol(STR("(1/floz)"));
    SIUnitsLibraryRemoveUnitWithSymbol(STR("(1/tbsp)"));
    SIUnitsLibraryRemoveUnitWithSymbol(STR("(1/tsp)"));

    SIUnitsLibraryRemoveUnitWithSymbol(STR("mi/gal"));
    SIUnitsLibraryRemoveUnitWithSymbol(STR("gal/h"));
    SIUnitsLibraryRemoveUnitWithSymbol(STR("gal/min"));
    SIUnitsLibraryRemoveUnitWithSymbol(STR("gal/s"));

    if (value)
    {
        // Remove Imperial Volumes
        SIUnitsLibraryRemoveUnitWithSymbol(STR("galUK"));
        SIUnitsLibraryRemoveUnitWithSymbol(STR("qtUK"));
        SIUnitsLibraryRemoveUnitWithSymbol(STR("ptUK"));
        SIUnitsLibraryRemoveUnitWithSymbol(STR("cupUK"));
        SIUnitsLibraryRemoveUnitWithSymbol(STR("giUK"));
        SIUnitsLibraryRemoveUnitWithSymbol(STR("flozUK"));
        SIUnitsLibraryRemoveUnitWithSymbol(STR("tbspUK"));
        SIUnitsLibraryRemoveUnitWithSymbol(STR("tspUK"));
        SIUnitsLibraryRemoveUnitWithSymbol(STR("halftspUK"));
        SIUnitsLibraryRemoveUnitWithSymbol(STR("quartertspUK"));

        SIUnitsLibraryRemoveUnitWithSymbol(STR("(1/galUK)"));
        SIUnitsLibraryRemoveUnitWithSymbol(STR("(1/qtUK)"));
        SIUnitsLibraryRemoveUnitWithSymbol(STR("(1/ptUK)"));
        SIUnitsLibraryRemoveUnitWithSymbol(STR("(1/cupUK)"));
        SIUnitsLibraryRemoveUnitWithSymbol(STR("(1/giUK)"));
        SIUnitsLibraryRemoveUnitWithSymbol(STR("(1/flozUK)"));
        SIUnitsLibraryRemoveUnitWithSymbol(STR("(1/tbspUK)"));
        SIUnitsLibraryRemoveUnitWithSymbol(STR("(1/tspUK)"));

        SIUnitsLibraryRemoveUnitWithSymbol(STR("mi/galUK"));
        SIUnitsLibraryRemoveUnitWithSymbol(STR("galUK/h"));
        SIUnitsLibraryRemoveUnitWithSymbol(STR("galUK/min"));
        SIUnitsLibraryRemoveUnitWithSymbol(STR("galUK/s"));

        // Define US Volume units
        // Volume
        AddNonSIUnitToLibrary(kSIQuantityVolume, STR("US gallon"), STR("US gallons"), STR("galUS"), 0.003785411784);
        AddNonSIUnitToLibrary(kSIQuantityVolume, STR("US quart"), STR("US quarts"), STR("qtUS"), 0.003785411784 / 4);
        AddNonSIUnitToLibrary(kSIQuantityVolume, STR("US pint"), STR("US pints"), STR("ptUS"), 0.003785411784 / 8);
        AddNonSIUnitToLibrary(kSIQuantityVolume, STR("US cup"), STR("US cups"), STR("cupUS"), 0.003785411784 / 16);
        AddNonSIUnitToLibrary(kSIQuantityVolume, STR("US gill"), STR("US gills"), STR("giUS"), 0.003785411784 / 32);
        AddNonSIUnitToLibrary(kSIQuantityVolume, STR("US fluid ounce"), STR("US fluid ounces"), STR("flozUS"), 0.003785411784 / 128);
        AddNonSIUnitToLibrary(kSIQuantityVolume, STR("US tablespoon"), STR("US tablespoons"), STR("tbspUS"), 0.003785411784 / 256);
        AddNonSIUnitToLibrary(kSIQuantityVolume, STR("US teaspoon"), STR("US teaspoons"), STR("tspUS"), 0.003785411784 / 768);
        AddNonSIUnitToLibrary(kSIQuantityVolume, STR("US half teaspoon"), STR("US half teaspoons"), STR("halftspUS"), 0.003785411784 / 768 / 2.);
        AddNonSIUnitToLibrary(kSIQuantityVolume, STR("US quarter teaspoon"), STR("US quarter teaspoons"), STR("quartertspUS"), 0.003785411784 / 768 / 4.);

        // Inverse Volume
        AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse US gallon"), STR("inverse US gallons"), STR("(1/galUS)"), 1. / 0.003785411784);
        AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse US quart"), STR("inverse US quarts"), STR("(1/qtUS)"), 1. / (0.003785411784 / 4));
        AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse US pint"), STR("inverse US pints"), STR("(1/ptUS)"), 1. / (0.003785411784 / 8));
        AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse US cup"), STR("inverse US cups"), STR("(1/cupUS)"), 1. / (0.003785411784 / 16));
        AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse US gill"), STR("inverse US gills"), STR("(1/giUS)"), 1. / (0.003785411784 / 32));
        AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse US fluid ounce"), STR("inverse US fluid ounces"), STR("(1/flozUS)"), 1. / (0.003785411784 / 128));
        AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse US tablespoon"), STR("inverse US tablespoons"), STR("(1/tbspUS)"), 1. / (0.003785411784 / 256));
        AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse US teaspoon"), STR("inverse US teaspoon"), STR("(1/tspUS)"), 1. / (0.003785411784 / 768));

        // Distance / Volume
        AddNonSIUnitToLibrary(kSIQuantityLengthPerVolume, STR("mile per US gallon"), STR("miles per US gallon"), STR("mi/galUS"), 1609.344 / 0.003785411784);

        AddNonSIUnitToLibrary(kSIQuantityVolumetricFlowRate, STR("US gallon per hour"), STR("US gallons per hour"), STR("galUS/h"), 0.003785411784 / 3600.);
        AddNonSIUnitToLibrary(kSIQuantityVolumetricFlowRate, STR("US gallon per minute"), STR("US gallons per minute"), STR("galUS/min"), 0.003785411784 / 60.);
        AddNonSIUnitToLibrary(kSIQuantityVolumetricFlowRate, STR("US gallon per second"), STR("US gallons per second"), STR("galUS/s"), 0.003785411784);

        // Define UK Volume units
        // Volume
        AddNonSIUnitToLibrary(kSIQuantityVolume, STR("gallon"), STR("gallons"), STR("gal"), 0.00454609);
        AddNonSIUnitToLibrary(kSIQuantityVolume, STR("quart"), STR("quarts"), STR("qt"), 0.00454609 / 4);
        AddNonSIUnitToLibrary(kSIQuantityVolume, STR("pint"), STR("pints"), STR("pt"), 0.00454609 / 8);
        AddNonSIUnitToLibrary(kSIQuantityVolume, STR("cup"), STR("cups"), STR("cup"), 0.00454609 / 16);
        AddNonSIUnitToLibrary(kSIQuantityVolume, STR("gill"), STR("gill"), STR("gi"), 0.00454609 / 32);
        AddNonSIUnitToLibrary(kSIQuantityVolume, STR("fluid ounce"), STR("fluid ounces"), STR("floz"), 0.00454609 / 160);
        AddNonSIUnitToLibrary(kSIQuantityVolume, STR("tablespoon"), STR("tablespoons"), STR("tbsp"), 0.00454609 / 256);
        AddNonSIUnitToLibrary(kSIQuantityVolume, STR("teaspoon"), STR("teaspoons"), STR("tsp"), 0.00454609 / 768);
        AddNonSIUnitToLibrary(kSIQuantityVolume, STR("half teaspoon"), STR("half teaspoons"), STR("halftsp"), 0.00454609 / 768 / 2.);
        AddNonSIUnitToLibrary(kSIQuantityVolume, STR("quarter teaspoon"), STR("quarter teaspoons"), STR("quartertsp"), 0.00454609 / 768 / 4.);

        // Inverse Volume
        AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse gallon"), STR("inverse gallons"), STR("(1/gal)"), 1. / 0.00454609);
        AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse quart"), STR("inverse quarts"), STR("(1/qt)"), 1. / (0.00454609 / 4));
        AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse pint"), STR("inverse pints"), STR("(1/pt)"), 1. / (0.00454609 / 8));
        AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse cup"), STR("inverse cups"), STR("(1/cup)"), 1. / (0.00454609 / 16));
        AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse gill"), STR("inverse gills"), STR("(1/gi)"), 1. / (0.00454609 / 32));
        AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse fluid ounce"), STR("inverse fluid ounces"), STR("(1/floz)"), 1. / (0.00454609 / 160));
        AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse tablespoon"), STR("inverse tablespoons"), STR("(1/tbsp)"), 1. / (0.00454609 / 256));
        AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse teaspoon"), STR("inverse teaspoon"), STR("(1/tsp)"), 1. / (0.00454609 / 768));

        // Distance / Volume
        AddNonSIUnitToLibrary(kSIQuantityLengthPerVolume, STR("mile per gallon"), STR("miles per gallon"), STR("mi/gal"), 1609.344 / 0.00454609);

        AddNonSIUnitToLibrary(kSIQuantityVolumetricFlowRate, STR("gallon per hour"), STR("gallons per hour"), STR("gal/h"), 0.00454609 / 3600.);
        AddNonSIUnitToLibrary(kSIQuantityVolumetricFlowRate, STR("gallon per minute"), STR("gallons per minute"), STR("gal/min"), 0.00454609 / 60.);
        AddNonSIUnitToLibrary(kSIQuantityVolumetricFlowRate, STR("gallon per second"), STR("gallons per second"), STR("gal/s"), 0.00454609);
    }
    else
    {
        // Remove US Volumes
        SIUnitsLibraryRemoveUnitWithSymbol(STR("galUS"));
        SIUnitsLibraryRemoveUnitWithSymbol(STR("qtUS"));
        SIUnitsLibraryRemoveUnitWithSymbol(STR("ptUS"));
        SIUnitsLibraryRemoveUnitWithSymbol(STR("cupUS"));
        SIUnitsLibraryRemoveUnitWithSymbol(STR("giUS"));
        SIUnitsLibraryRemoveUnitWithSymbol(STR("flozUS"));
        SIUnitsLibraryRemoveUnitWithSymbol(STR("tbspUS"));
        SIUnitsLibraryRemoveUnitWithSymbol(STR("tspUS"));
        SIUnitsLibraryRemoveUnitWithSymbol(STR("halftspUS"));
        SIUnitsLibraryRemoveUnitWithSymbol(STR("quartertspUS"));

        SIUnitsLibraryRemoveUnitWithSymbol(STR("(1/galUS)"));
        SIUnitsLibraryRemoveUnitWithSymbol(STR("(1/qtUS)"));
        SIUnitsLibraryRemoveUnitWithSymbol(STR("(1/ptUS)"));
        SIUnitsLibraryRemoveUnitWithSymbol(STR("(1/cupUS)"));
        SIUnitsLibraryRemoveUnitWithSymbol(STR("(1/giUS)"));
        SIUnitsLibraryRemoveUnitWithSymbol(STR("(1/flozUS)"));
        SIUnitsLibraryRemoveUnitWithSymbol(STR("(1/tbspUS)"));
        SIUnitsLibraryRemoveUnitWithSymbol(STR("(1/tspUS)"));

        SIUnitsLibraryRemoveUnitWithSymbol(STR("mi/galUS"));
        SIUnitsLibraryRemoveUnitWithSymbol(STR("galUS/h"));
        SIUnitsLibraryRemoveUnitWithSymbol(STR("galUS/min"));
        SIUnitsLibraryRemoveUnitWithSymbol(STR("galUS/s"));

        // Define US Volume units
        // Volume
        AddNonSIUnitToLibrary(kSIQuantityVolume, STR("gallon"), STR("gallons"), STR("gal"), 0.003785411784);
        AddNonSIUnitToLibrary(kSIQuantityVolume, STR("quart"), STR("quarts"), STR("qt"), 0.003785411784 / 4);
        AddNonSIUnitToLibrary(kSIQuantityVolume, STR("pint"), STR("pints"), STR("pt"), 0.003785411784 / 8);
        AddNonSIUnitToLibrary(kSIQuantityVolume, STR("cup"), STR("cups"), STR("cup"), 0.003785411784 / 16);
        AddNonSIUnitToLibrary(kSIQuantityVolume, STR("gill"), STR("gill"), STR("gi"), 0.003785411784 / 32);
        AddNonSIUnitToLibrary(kSIQuantityVolume, STR("fluid ounce"), STR("fluid ounces"), STR("floz"), 0.003785411784 / 128);
        AddNonSIUnitToLibrary(kSIQuantityVolume, STR("tablespoon"), STR("tablespoons"), STR("tbsp"), 0.003785411784 / 256);
        AddNonSIUnitToLibrary(kSIQuantityVolume, STR("teaspoon"), STR("teaspoons"), STR("tsp"), 0.003785411784 / 768);
        AddNonSIUnitToLibrary(kSIQuantityVolume, STR("half teaspoon"), STR("half teaspoons"), STR("halftsp"), 0.003785411784 / 768 / 2);
        AddNonSIUnitToLibrary(kSIQuantityVolume, STR("quarter teaspoon"), STR("quarter teaspoons"), STR("quartertsp"), 0.003785411784 / 768 / 4);

        // Inverse Volume
        AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse gallon"), STR("inverse gallons"), STR("(1/gal)"), 1. / 0.003785411784);
        AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse quart"), STR("inverse quarts"), STR("(1/qt)"), 1. / (0.003785411784 / 4));
        AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse pint"), STR("inverse pints"), STR("(1/pt)"), 1. / (0.003785411784 / 8));
        AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse cup"), STR("inverse cups"), STR("(1/cup)"), 1. / (0.003785411784 / 16));
        AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse gill"), STR("inverse gill"), STR("(1/gi)"), 1. / (0.003785411784 / 32));
        AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse fluid ounce"), STR("inverse fluid ounces"), STR("(1/floz)"), 1. / (0.003785411784 / 128));
        AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse tablespoon"), STR("inverse tablespoons"), STR("(1/tbsp)"), 1. / (0.003785411784 / 256));
        AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse teaspoon"), STR("inverse teaspoon"), STR("(1/tsp)"), 1. / (0.003785411784 / 768));

        // Distance / Volume
        AddNonSIUnitToLibrary(kSIQuantityLengthPerVolume, STR("mile per gallon"), STR("miles per gallon"), STR("mi/gal"), 1609.344 / 0.003785411784);

        AddNonSIUnitToLibrary(kSIQuantityVolumetricFlowRate, STR("gallon per hour"), STR("gallons per hour"), STR("gal/h"), 0.003785411784 / 3600.);
        AddNonSIUnitToLibrary(kSIQuantityVolumetricFlowRate, STR("gallon per minute"), STR("gallons per minute"), STR("gal/min"), 0.003785411784 / 60.);
        AddNonSIUnitToLibrary(kSIQuantityVolumetricFlowRate, STR("gallon per second"), STR("gallons per second"), STR("gal/s"), 0.003785411784);

        // Define UK Volume units
        // Volume
        AddNonSIUnitToLibrary(kSIQuantityVolume, STR("imperial gallon"), STR("imperial gallons"), STR("galUK"), 0.00454609);
        AddNonSIUnitToLibrary(kSIQuantityVolume, STR("imperial quart"), STR("imperial quarts"), STR("qtUK"), 0.00454609 / 4);
        AddNonSIUnitToLibrary(kSIQuantityVolume, STR("imperial pint"), STR("imperial pints"), STR("ptUK"), 0.00454609 / 8);
        AddNonSIUnitToLibrary(kSIQuantityVolume, STR("imperial cup"), STR("imperial cups"), STR("cupUK"), 0.00454609 / 16);
        AddNonSIUnitToLibrary(kSIQuantityVolume, STR("imperial gill"), STR("imperial gill"), STR("giUK"), 0.00454609 / 32);
        AddNonSIUnitToLibrary(kSIQuantityVolume, STR("imperial fluid ounce"), STR("imperial fluid ounces"), STR("flozUK"), 0.00454609 / 160);
        AddNonSIUnitToLibrary(kSIQuantityVolume, STR("imperial tablespoon"), STR("imperial tablespoons"), STR("tbspUK"), 0.00454609 / 256);
        AddNonSIUnitToLibrary(kSIQuantityVolume, STR("imperial teaspoon"), STR("imperial teaspoons"), STR("tspUK"), 0.00454609 / 768);
        AddNonSIUnitToLibrary(kSIQuantityVolume, STR("imperial half teaspoon"), STR("imperial half teaspoons"), STR("halftspUK"), 0.00454609 / 768 / 2);
        AddNonSIUnitToLibrary(kSIQuantityVolume, STR("imperial quarter teaspoon"), STR("imperial quarter teaspoons"), STR("quartertspUK"), 0.00454609 / 768 / 4);

        // Inverse Volume
        AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse imperial gallon"), STR("inverse imperial gallons"), STR("(1/galUK)"), 1. / 0.00454609);
        AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse imperial quart"), STR("inverse imperial quarts"), STR("(1/qtUK)"), 1. / (0.00454609 / 4));
        AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse imperial pint"), STR("inverse imperial pints"), STR("(1/ptUK)"), 1. / (0.00454609 / 8));
        AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse imperial cup"), STR("inverse imperial cups"), STR("(1/cupUK)"), 1. / (0.00454609 / 16));
        AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse imperial gill"), STR("inverse imperial gills"), STR("(1/giUK)"), 1. / (0.00454609 / 32));
        AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse imperial fluid ounce"), STR("inverse imperial fluid ounces"), STR("(1/flozUK)"), 1. / (0.00454609 / 160));
        AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse imperial tablespoon"), STR("inverse imperial tablespoons"), STR("(1/tbspUK)"), 1. / (0.00454609 / 256));
        AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse imperial teaspoon"), STR("inverse imperial teaspoon"), STR("(1/tspUK)"), 1. / (0.00454609 / 768));

        // Distance / Volume
        AddNonSIUnitToLibrary(kSIQuantityLengthPerVolume, STR("mile per imperial gallon"), STR("miles per imperial gallon"), STR("mi/galUK"), 1609.344 / 0.00454609);
        AddNonSIUnitToLibrary(kSIQuantityVolumetricFlowRate, STR("imperial gallon per hour"), STR("imperial gallons per hour"), STR("galUK/h"), 0.00454609 / 3600.);
        AddNonSIUnitToLibrary(kSIQuantityVolumetricFlowRate, STR("imperial gallon per minute"), STR("imperial gallons per minute"), STR("galUK/min"), 0.00454609 / 60.);
        AddNonSIUnitToLibrary(kSIQuantityVolumetricFlowRate, STR("imperial gallon per second"), STR("imperial gallons per second"), STR("galUK/s"), 0.00454609);
    }
    imperialVolumes = value;
}

// Add a cleanup function for static dictionaries and array
void cleanupUnitsLibraries(void) {
    printf("Cleaning up units libraries...\n");
    if (unitsQuantitiesLibrary) {
        OCRelease(unitsQuantitiesLibrary);
        unitsQuantitiesLibrary = NULL;
    }

    if (unitsDimensionalitiesLibrary) {
        OCRelease(unitsDimensionalitiesLibrary);
        unitsDimensionalitiesLibrary = NULL;
    }

    if (unitsNamesLibrary) {
        OCRelease(unitsNamesLibrary);
        unitsNamesLibrary = NULL;
    }

    // This dictionary must be released last, as it converts all units from static instances
    // to non-static instances.  If this dictionary is released first, then the static instances
    // will have been released and would be invalid in the other dictionaries above.
    if (unitsLibrary) {
        OCArrayRef values = OCDictionaryCreateArrayWithAllValues((OCDictionaryRef)unitsLibrary);
        if (values) {
            for (uint64_t i = 0; i < OCArrayGetCount(values); i++) {
                SIUnitRef unit = (SIUnitRef)OCArrayGetValueAtIndex(values, i);
                OCTypeSetStaticInstance(unit, false);
                // Increase the retain count to 2 since it will be released with values and
                // then again when the unitsLibrary is released.
                OCRetain(unit);
            }
            OCRelease(values);
        }
        OCRelease(unitsLibrary);
        unitsLibrary = NULL;
    }

}

void UnitsLibraryCreate()
{
    setlocale(LC_ALL, "");

    const struct lconv *const currentlocale = localeconv();

    printf("In the current locale, the default currency symbol is: %s\n",
           currentlocale->currency_symbol);

    unitsLibrary = OCDictionaryCreateMutable(0);
    IF_NO_OBJECT_EXISTS_RETURN(unitsLibrary, )
    unitsQuantitiesLibrary = OCDictionaryCreateMutable(0);
    IF_NO_OBJECT_EXISTS_RETURN(unitsQuantitiesLibrary, )
    unitsDimensionalitiesLibrary = OCDictionaryCreateMutable(0);
    IF_NO_OBJECT_EXISTS_RETURN(unitsDimensionalitiesLibrary, )

    // Derived Constants
#pragma mark Derived Constants
    double hbar = kSIPlanckConstant / (2 * kSIPi);
    double alpha = (1. / (4. * kSIPi * kSIElectricConstant)) * kSIElementaryCharge * kSIElementaryCharge / (kSISpeedOfLight * kSIPlanckConstant / (2 * kSIPi));
    double lightYear = (double)kSIYear * (double)kSISpeedOfLight;
    double E_h = kSIElectronMass * (kSIElementaryCharge * kSIElementaryCharge / (2 * kSIElectricConstant * kSIPlanckConstant)) * (kSIElementaryCharge * kSIElementaryCharge / (2 * kSIElectricConstant * kSIPlanckConstant));
    double a_0 = kSIElectricConstant * kSIPlanckConstant * kSIPlanckConstant / (kSIPi * kSIElectronMass * kSIElementaryCharge * kSIElementaryCharge);
    double R_H = kSIElectronMass * kSIElementaryCharge * kSIElementaryCharge * kSIElementaryCharge * kSIElementaryCharge / (8 * kSIElectricConstant * kSIElectricConstant * kSIPlanckConstant * kSIPlanckConstant * kSIPlanckConstant * kSISpeedOfLight);
    double Ry = kSIPlanckConstant * kSISpeedOfLight * R_H;
    double Λ_0 = E_h / (kSIElementaryCharge * a_0 * a_0);
    double G_0 = 2 * kSIElementaryCharge * kSIElementaryCharge / kSIPlanckConstant;
    double mu_N = kSIElementaryCharge * hbar / (2 * kSIProtonMass);
    double mu_e = kSIElementaryCharge * hbar / (2 * kSIElectronMass);

    double planckTime = sqrt(hbar * (double)kSIGravitaionalConstant / ((double)kSISpeedOfLight * kSISpeedOfLight * kSISpeedOfLight * kSISpeedOfLight * kSISpeedOfLight));
    double planckLength = sqrt(hbar * (double)kSIGravitaionalConstant / ((double)kSISpeedOfLight * kSISpeedOfLight * kSISpeedOfLight));
    double planckMass = sqrt(hbar * kSISpeedOfLight / kSIGravitaionalConstant);
    double planckTemperature = planckMass * kSISpeedOfLight * kSISpeedOfLight / kSIBoltmannConstant;

    // Base Root Name and Root Symbol Units - Table 1

    // ***** Dimensionless **********************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityDimensionless
    AddUnitForQuantityToLibrary(kSIQuantityDimensionless,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                NULL, NULL, NULL, kSIPrefixNone, false, 1., false);

    // Dimensionless - Percent
    AddNonSIUnitToLibrary(kSIQuantityDimensionless, STR("percent"), STR("percent"), STR("%"), 0.01);
    AddNonSIUnitToLibrary(kSIQuantityDimensionless, STR("part per thousand"), STR("parts per thousand"), STR("‰"), 0.001);
    AddNonSIUnitToLibrary(kSIQuantityDimensionless, STR("part per ten thousand"), STR("parts per ten thousand"), STR("‱"), 0.0001);

    // Dimensionless - ppm
    AddNonSIUnitToLibrary(kSIQuantityDimensionless, STR("part per million"), STR("parts per million"), STR("ppm"), 1.e-6);

    // Dimensionless - ppb
    AddNonSIUnitToLibrary(kSIQuantityDimensionless, STR("part per billion"), STR("parts per billion"), STR("ppb"), 1.e-9);

    // Dimensionless - ppt
    AddNonSIUnitToLibrary(kSIQuantityDimensionless, STR("part per trillion"), STR("parts per trillion"), STR("ppt"), 1.e-12);

    // Dimensionless - ppq
    AddNonSIUnitToLibrary(kSIQuantityDimensionless, STR("part per quadrillion"), STR("parts per quadrillion"), STR("ppq"), 1.e-15);

    // Derived Dimensionless - fine structure constant
    // (1/(4•π•ε_0))•q_e^2/(c_0•h_P/(2•π))
    AddNonSIUnitToLibrary(kSIQuantityFineStructureConstant, STR("fine structure constant"), STR("fine structure constant"), STR("α"), alpha);

    // Derived Dimensionless - inverse fine structure constant
    AddNonSIUnitToLibrary(kSIQuantityFineStructureConstant, STR("inverse fine structure constant"), STR("inverse fine structure constant"), STR("(1/α)"), 1 / alpha);

    // ***** Length *****************************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityLength
    AddUnitForQuantityToLibrary(kSIQuantityLength,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                NULL, NULL, NULL, kSIPrefixNone, false, 1., true);

    OCMutableArrayRef units = NULL;

    // Length - astronomical units
    AddNonSIUnitToLibrary(kSIQuantityLength, STR("astronomical unit"), STR("astronomical units"), STR("ua"), 1.49597870691e11);

    AddNonSIUnitToLibrary(kSIQuantityLength, STR("light year"), STR("light years"), STR("ly"), lightYear);

    // Length - Angstrom
    AddNonSIUnitToLibrary(kSIQuantityLength, STR("ångström"), STR("ångströms"), STR("Å"), 1.e-10);

    // atomic unit of length
    // a_0 = ε_0•h_P^2/(π*m_e•q_e^2)
    AddNonSIUnitToLibrary(kSIQuantityLength, STR("atomic unit of length"), STR("atomic unit of length"), STR("a_0"), a_0);

    // Length - nautical mile - !!!!!! Disabled in favor of Molarity !!!!!!!
    //  AddNonSIUnitToLibrary(kSIQuantityLength, STR("nautical mile"), STR("nautical miles"), STR("M"),1852.);

    // Length - fathom
    AddNonSIUnitToLibrary(kSIQuantityLength, STR("fathom"), STR("fathoms"), STR("ftm"), 2 * 1609.344 / 1760);

    // Length
    AddNonSIUnitToLibrary(kSIQuantityLength, STR("inch"), STR("inches"), STR("in"), 1609.344 / 63360);
    AddNonSIUnitToLibrary(kSIQuantityLength, STR("foot"), STR("feet"), STR("ft"), 1609.344 / 5280);
    AddNonSIUnitToLibrary(kSIQuantityLength, STR("yard"), STR("yards"), STR("yd"), 1609.344 / 1760);
    AddNonSIUnitToLibrary(kSIQuantityLength, STR("mile"), STR("miles"), STR("mi"), 1609.344);

    AddNonSIUnitToLibrary(kSIQuantityLength, STR("link"), STR("links"), STR("li"), 1609.344 / 5280 * 33 / 50);
    AddNonSIUnitToLibrary(kSIQuantityLength, STR("rod"), STR("rods"), STR("rod"), 1609.344 / 5280 * 16.5);
    AddNonSIUnitToLibrary(kSIQuantityLength, STR("chain"), STR("chains"), STR("ch"), 1609.344 / 5280 * 16.5 * 4);
    AddNonSIUnitToLibrary(kSIQuantityLength, STR("furlong"), STR("furlongs"), STR("fur"), 1609.344 / 5280 * 16.5 * 4 * 10);
    AddNonSIUnitToLibrary(kSIQuantityLength, STR("league"), STR("leagues"), STR("lea"), 1609.344 * 3);

    // Compton Wavelength
    AddNonSIUnitToLibrary(kSIQuantityLength, STR("compton wavelength"), STR("compton wavelengths"), STR("λ_C"), kSIPlanckConstant / (kSIElectronMass * kSISpeedOfLight));

    // ***** Inverse Length, Wave Number ********************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityWavenumber
    AddNonSIUnitToLibrary(kSIQuantityWavenumber, STR("inverse meter"), STR("inverse meters"), STR("(1/m)"), 1);
    AddNonSIUnitToLibrary(kSIQuantityWavenumber, STR("inverse centimeter"), STR("inverse centimeters"), STR("(1/cm)"), 100);
    AddNonSIUnitToLibrary(kSIQuantityWavenumber, STR("inverse ångström"), STR("inverse ångströms"), STR("(1/Å)"), 1.e10);

    AddNonSIUnitToLibrary(kSIQuantityWavenumber, STR("inverse mile"), STR("inverse miles"), STR("(1/mi)"), 1. / 1609.344);
    AddNonSIUnitToLibrary(kSIQuantityWavenumber, STR("inverse yard"), STR("inverse yards"), STR("(1/yd)"), 1. / (1609.344 / 1760));
    AddNonSIUnitToLibrary(kSIQuantityWavenumber, STR("inverse foot"), STR("inverse feet"), STR("(1/ft)"), 1. / (1609.344 / 5280));
    AddNonSIUnitToLibrary(kSIQuantityWavenumber, STR("inverse inch"), STR("inverse inches"), STR("(1/in)"), 1. / (1609.344 / 63360));

    // Inverse Length - Rydberg constant
    // R_H = m_e•q_e^4/(8•ε_0^2•h_P^3•c_0)
    AddNonSIUnitToLibrary(kSIQuantityWavenumber, STR("rydberg constant"), STR("rydberg constant"), STR("R_∞"), R_H);

    // UnitsQuantitiesLibrary contains an array of valid units for each quantity.
    units = (OCMutableArrayRef)OCDictionaryGetValue(unitsQuantitiesLibrary, kSIQuantityWavenumber);
    OCDictionaryAddValue(unitsQuantitiesLibrary, kSIQuantityInverseLength, units);

    // ***** Length Ratio ***********************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityLengthRatio
    AddUnitForQuantityToLibrary(kSIQuantityLengthRatio,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("meter per meter"), STR("meters per meter"), STR("m/m"), kSIPrefixNone, true, 1., true);

    // ***** Plane Angle ************************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityPlaneAngle
    AddUnitForQuantityToLibrary(kSIQuantityPlaneAngle,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("radian"), STR("radians"), STR("rad"), kSIPrefixNone, true, 1., true);

    // ***** Mass *******************************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityMass
    AddUnitForQuantityToLibrary(kSIQuantityMass,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                NULL, NULL, NULL, kSIPrefixNone, false, 1., true);

    AddNonSIUnitToLibrary(kSIQuantityMass, STR("microgram"), STR("micrograms"), STR("mcg"), 1e-9);
    AddNonSIUnitToLibrary(kSIQuantityMass, STR("tonne"), STR("tonnes"), STR("t"), 1e3);

    // Mass - Dalton
    AddUnitForQuantityToLibrary(kSIQuantityMass,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("dalton"), STR("daltons"), STR("Da"), kSIPrefixNone, false, kSIAtomicMassConstant, true);

    // Mass - unified atomic mass unit
    AddNonSIUnitToLibrary(kSIQuantityMass, STR("atomic mass unit"), STR("atomic mass units"), STR("u"), kSIAtomicMassConstant);

    // Mass - atomic mass constant
    AddNonSIUnitToLibrary(kSIQuantityMass, STR("atomic mass constant"), STR("atomic mass constant"), STR("m_u"), kSIAtomicMassConstant);

    // electron mass or atomic unit of mass
    AddNonSIUnitToLibrary(kSIQuantityMass, STR("electron mass"), STR("electron mass"), STR("m_e"), kSIElectronMass);

    // proton mass
    AddNonSIUnitToLibrary(kSIQuantityMass, STR("proton mass"), STR("proton mass"), STR("m_p"), kSIProtonMass);

    // neutron mass
    AddNonSIUnitToLibrary(kSIQuantityMass, STR("neutron mass"), STR("neutron mass"), STR("m_n"), kSINeutronMass);
    // alpha particle mass
    AddNonSIUnitToLibrary(kSIQuantityMass, STR("alpha particle mass"), STR("alpha particle mass"), STR("m_a"), kSIAlphaParticleMass);

    // muon mass
    AddNonSIUnitToLibrary(kSIQuantityMass, STR("muon mass"), STR("myon mass"), STR("m_µ"), kSIMuonMass);

    // Mass
    AddNonSIUnitToLibrary(kSIQuantityMass, STR("ton"), STR("tons"), STR("ton"), 0.45359237 * 2000);
    AddNonSIUnitToLibrary(kSIQuantityMass, STR("hundredweight"), STR("hundredweight"), STR("cwt"), 0.45359237 * 100);
    AddNonSIUnitToLibrary(kSIQuantityMass, STR("pound"), STR("pounds"), STR("lb"), 0.45359237);
    AddNonSIUnitToLibrary(kSIQuantityMass, STR("stone"), STR("stones"), STR("st"), 6.35029318);
    AddNonSIUnitToLibrary(kSIQuantityMass, STR("ounce"), STR("ounces"), STR("oz"), 0.028349523125);
    AddNonSIUnitToLibrary(kSIQuantityMass, STR("grain"), STR("grains"), STR("gr"), 0.45359237 / 7000);
    AddNonSIUnitToLibrary(kSIQuantityMass, STR("dram"), STR("drams"), STR("dr"), 0.45359237 / 256);

    AddNonSIUnitToLibrary(kSIQuantityMass, STR("tonUK"), STR("tonsUK"), STR("tonUK"), 0.45359237 * 2240);
    AddNonSIUnitToLibrary(kSIQuantityMass, STR("hundredweightUK"), STR("hundredweightUK"), STR("cwtUK"), 0.45359237 * 112);

    // ***** Inverse Mass ***********************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityInverseMass
    // Inverse Mass
    AddNonSIUnitToLibrary(kSIQuantityInverseMass, STR("inverse kilogram"), STR("inverse kilograms"), STR("(1/kg)"), 1.);
    AddNonSIUnitToLibrary(kSIQuantityInverseMass, STR("inverse tonne"), STR("inverse tonnes"), STR("(1/t)"), 1. / 1e3);
    AddNonSIUnitToLibrary(kSIQuantityInverseMass, STR("inverse ton"), STR("inverse tons"), STR("(1/ton)"), 1. / (0.45359237 * 2000));
    AddNonSIUnitToLibrary(kSIQuantityInverseMass, STR("inverse stone"), STR("inverse stones"), STR("(1/st)"), 1. / 6.35029318);
    AddNonSIUnitToLibrary(kSIQuantityInverseMass, STR("inverse pound"), STR("inverse pounds"), STR("(1/lb)"), 1. / 0.45359237);
    AddNonSIUnitToLibrary(kSIQuantityInverseMass, STR("inverse ounce"), STR("inverse ounces"), STR("(1/oz)"), 1. / 0.028349523125);

    // ***** Mass Ratio *************************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityMassRatio
    AddUnitForQuantityToLibrary(kSIQuantityMassRatio,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("gram per kilogram"), STR("grams per kilogram"), STR("g/kg"), kSIPrefixNone, false, 0.001, true);

    // ***** Time *******************************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityTime
    AddUnitForQuantityToLibrary(kSIQuantityTime,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                NULL, NULL, NULL, kSIPrefixNone, false, 1., true);

    // Time
    AddNonSIUnitToLibrary(kSIQuantityTime, STR("minute"), STR("minutes"), STR("min"), kSIMinute);
    AddNonSIUnitToLibrary(kSIQuantityTime, STR("hour"), STR("hours"), STR("h"), kSIHour);
    AddNonSIUnitToLibrary(kSIQuantityTime, STR("day"), STR("days"), STR("d"), kSIDay);
    AddNonSIUnitToLibrary(kSIQuantityTime, STR("week"), STR("weeks"), STR("wk"), kSIWeek);
    AddNonSIUnitToLibrary(kSIQuantityTime, STR("month"), STR("months"), STR("month"), kSIMonth);
    AddNonSIUnitToLibrary(kSIQuantityTime, STR("year"), STR("years"), STR("yr"), kSIYear);
    AddNonSIUnitToLibrary(kSIQuantityTime, STR("decade"), STR("decades"), STR("dayr"), kSIDecade);
    AddNonSIUnitToLibrary(kSIQuantityTime, STR("century"), STR("centuries"), STR("hyr"), kSICentury);
    AddNonSIUnitToLibrary(kSIQuantityTime, STR("millennium"), STR("millennia"), STR("kyr"), kSIMillennium);

    // atomic unit of time
    AddNonSIUnitToLibrary(kSIQuantityTime, STR("atomic unit of time"), STR("atomic units of time"), STR("ℏ/E_h"), hbar / E_h);

    // natural unit of length
    AddNonSIUnitToLibrary(kSIQuantityLength, STR("natural unit of length"), STR("natural units of length"), STR("ƛ_C"), 386.15926764e-15);

    // natural unit of time
    AddNonSIUnitToLibrary(kSIQuantityTime, STR("natural unit of time"), STR("natural units of time"), STR("ℏ/(m_e•c_0^2)"), hbar / (kSIElectronMass * kSISpeedOfLight * kSISpeedOfLight));

    // natural unit of momentum
    AddNonSIUnitToLibrary(kSIQuantityLinearMomentum, STR("natural unit of momentum"), STR("natural units of momentum"), STR("m_e•c_0"), kSIElectronMass * kSISpeedOfLight);

    // natural unit of energy
    AddNonSIUnitToLibrary(kSIQuantityEnergy, STR("natural unit of energy"), STR("natural units of energy"), STR("m_e•c_0^2"), kSIElectronMass * kSISpeedOfLight * kSISpeedOfLight);

    // planck length
    // sqrt(h_P * &G/(2*π*c_0^3))
    AddNonSIUnitToLibrary(kSIQuantityLength, STR("planck length"), STR("planck length"), STR("l_P"), planckLength);

    // planck mass
    AddNonSIUnitToLibrary(kSIQuantityMass, STR("planck mass"), STR("planck mass"), STR("m_P"), planckMass);

    // planck time
    AddNonSIUnitToLibrary(kSIQuantityTime, STR("planck time"), STR("planck time"), STR("t_P"), planckTime);

    // planck temperature
    AddNonSIUnitToLibrary(kSIQuantityTemperature, STR("planck temperature"), STR("planck temperature"), STR("T_P"), planckTemperature);

    // ***** Inverse Time ***********************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityInverseTime
    AddNonSIUnitToLibrary(kSIQuantityInverseTime, STR("inverse seconds"), STR("inverse seconds"), STR("(1/s)"), 1.);
    AddNonSIUnitToLibrary(kSIQuantityInverseTime, STR("inverse minute"), STR("inverse minutes"), STR("(1/min)"), 1. / 60.);
    AddNonSIUnitToLibrary(kSIQuantityInverseTime, STR("inverse hour"), STR("inverse hours"), STR("(1/h)"), 1. / (60. * 60.));
    AddNonSIUnitToLibrary(kSIQuantityInverseTime, STR("inverse day"), STR("inverse days"), STR("(1/d)"), 1. / (60. * 60 * 24.));
    AddNonSIUnitToLibrary(kSIQuantityInverseTime, STR("inverse week"), STR("inverse weeks"), STR("(1/wk)"), 1. / (60. * 60 * 24. * 7.));
    AddNonSIUnitToLibrary(kSIQuantityInverseTime, STR("inverse month"), STR("inverse months"), STR("(1/month)"), 1. / (365.25 * 86400 / 12.));
    AddNonSIUnitToLibrary(kSIQuantityInverseTime, STR("inverse year"), STR("inverse years"), STR("(1/yr)"), 1. / (365.25 * 86400));

    // ***** Time Ratio **************************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityTimeRatio
    AddUnitForQuantityToLibrary(kSIQuantityTimeRatio,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("second per second"), STR("seconds per second"), STR("s/s"), kSIPrefixNone, true, 1., true);

    // ***** Frequency **************************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityFrequency
    AddUnitForQuantityToLibrary(kSIQuantityFrequency,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("hertz"), STR("hertz"), STR("Hz"), kSIPrefixNone, true, 1., true);

    // ***** Frequency Ratio *******************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityFrequencyRatio
    AddUnitForQuantityToLibrary(kSIQuantityFrequencyRatio,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("hertz per hertz"), STR("hertz per hertz"), STR("Hz/Hz"), kSIPrefixNone, true, 1., true);

    // Include ppm as a kSIQuantityFrequencyRatio to make NMR people happy.
    units = (OCMutableArrayRef)OCDictionaryGetValue(unitsQuantitiesLibrary, kSIQuantityFrequencyRatio);
    OCArrayAppendValue(units, SIUnitForUnderivedSymbol(STR("ppm")));

    // ***** Radioactivity **********************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityRadioactivity
    AddUnitForQuantityToLibrary(kSIQuantityRadioactivity,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("becquerel"), STR("becquerels"), STR("Bq"), kSIPrefixNone, true, 1., true);

    AddUnitForQuantityToLibrary(kSIQuantityRadioactivity,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("curie"), STR("curies"), STR("Ci"), kSIPrefixNone, false, 3.7e10, true);

    // ***** inverse seconds ********************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityInverseTimeSquared
    AddNonSIUnitToLibrary(kSIQuantityInverseTimeSquared, STR("inverse millisecond squared"), STR("inverse milliseconds squared"), STR("(1/ms^2)"), 1000000.);
    AddNonSIUnitToLibrary(kSIQuantityInverseTimeSquared, STR("inverse second squared"), STR("inverse seconds squared"), STR("(1/s^2)"), 1.);
    AddNonSIUnitToLibrary(kSIQuantityInverseTimeSquared, STR("inverse hour inverse second"), STR("inverse hour inverse seconds"), STR("(1/(h•s))"), 1. / 3600.);
    AddNonSIUnitToLibrary(kSIQuantityInverseTimeSquared, STR("inverse hour inverse minute"), STR("inverse hour inverse minutes"), STR("(1/(h•min))"), 1. / 3600. / 60.);

    // ***** Current ****************************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityCurrent
    AddUnitForQuantityToLibrary(kSIQuantityCurrent,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                NULL, NULL, NULL, kSIPrefixNone, false, 1., true);

    AddNonSIUnitToLibrary(kSIQuantityCurrent, STR("atomic unit of current"), STR("atomic unit of current"), STR("q_e•E_h/ℏ"), kSIElementaryCharge * E_h / hbar);

#pragma mark kSIQuantityInverseCurrent
    AddNonSIUnitToLibrary(kSIQuantityInverseCurrent, STR("inverse ampere"), STR("inverse amperes"), STR("(1/A)"), 1.);

#pragma mark kSIQuantityCurrentRatio
    AddUnitForQuantityToLibrary(kSIQuantityCurrentRatio,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("ampere per ampere"), STR("amperes per ampere"), STR("A/A"), kSIPrefixNone, true, 1., true);

    // ***** Thermodynamic Temperature **********************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityTemperature
    AddUnitForQuantityToLibrary(kSIQuantityTemperature,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                NULL, NULL, NULL, kSIPrefixNone, false, 1., true);
    AddNonSIUnitToLibrary(kSIQuantityTemperature, STR("rankine"), STR("rankines"), STR("°R"), 0.555555555555556);
    AddNonSIUnitToLibrary(kSIQuantityTemperature, STR("fahrenheit"), STR("fahrenheit"), STR("°F"), 0.555555555555556);
    AddNonSIUnitToLibrary(kSIQuantityTemperature, STR("celsius"), STR("celsius"), STR("°C"), 1);

#pragma mark kSIQuantityInverseTemperature
    AddNonSIUnitToLibrary(kSIQuantityInverseTemperature, STR("inverse kelvin"), STR("inverse kelvin"), STR("(1/K)"), 1.);

#pragma mark kSIQuantityTemperatureRatio
    AddUnitForQuantityToLibrary(kSIQuantityTemperatureRatio,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("kelvin per kelvin"), STR("kelvin per kelvin"), STR("K/K"), kSIPrefixNone, true, 1., true);

    // ***** Thermodynamic Temperature Gradient *************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityTemperatureGradient
    AddNonSIUnitToLibrary(kSIQuantityTemperatureGradient, STR("kelvin per meter"), STR("kelvin per meter"), STR("K/m"), 1);
    AddNonSIUnitToLibrary(kSIQuantityTemperatureGradient, STR("celsius per meter"), STR("celsius per meter"), STR("°C/m"), 1);
    AddNonSIUnitToLibrary(kSIQuantityTemperatureGradient, STR("fahrenheit per foot"), STR("fahrenheit per foot"), STR("°F/ft"), 0.555555555555556 / (1609.344 / 5280));
    AddNonSIUnitToLibrary(kSIQuantityTemperatureGradient, STR("rankine per foot"), STR("rankines per foot"), STR("°R/ft"), 0.555555555555556 / (1609.344 / 5280));

    // ***** Amount *****************************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityAmount
    AddUnitForQuantityToLibrary(kSIQuantityAmount,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                NULL, NULL, NULL, kSIPrefixNone, false, 1., true);

#pragma mark kSIQuantityInverseAmount
    AddNonSIUnitToLibrary(kSIQuantityInverseAmount, STR("inverse mole"), STR("inverse moles"), STR("(1/mol)"), 1.);

#pragma mark kSIQuantityAmountRatio
    AddUnitForQuantityToLibrary(kSIQuantityAmountRatio,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("mole per mole"), STR("moles per mole"), STR("mol/mol"), kSIPrefixNone, true, 1., true);

    // ***** Luminous Intensity *****************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityLuminousIntensity
    AddUnitForQuantityToLibrary(kSIQuantityLuminousIntensity,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                NULL, NULL, NULL, kSIPrefixNone, false, 1., true);

#pragma mark kSIQuantityInverseLuminousIntensity
    AddNonSIUnitToLibrary(kSIQuantityInverseLuminousIntensity, STR("inverse candela"), STR("inverse candelas"), STR("(1/cd)"), 1.);

#pragma mark kSIQuantityLuminousIntensityRatio
    AddUnitForQuantityToLibrary(kSIQuantityLuminousIntensityRatio,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("candela per candela"), STR("candelas per candela"), STR("cd/cd"), kSIPrefixNone, true, 1., true);

    // ***** Area *******************************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityArea
    AddNonSIUnitToLibrary(kSIQuantityArea, STR("hectare"), STR("hectares"), STR("ha"), 1e4);
    AddNonSIUnitToLibrary(kSIQuantityArea, STR("barn"), STR("barns"), STR("b"), 1.e-28);
    AddNonSIUnitToLibrary(kSIQuantityArea, STR("square meter"), STR("square meters"), STR("m^2"), 1);
    AddNonSIUnitToLibrary(kSIQuantityArea, STR("square centimeter"), STR("square centimeters"), STR("cm^2"), 0.0001);
    AddNonSIUnitToLibrary(kSIQuantityArea, STR("square kilometer"), STR("square kilometers"), STR("km^2"), 1000000);

    // Area
    AddNonSIUnitToLibrary(kSIQuantityArea, STR("square inch"), STR("square inches"), STR("in^2"), 0.00064516);
    AddNonSIUnitToLibrary(kSIQuantityArea, STR("square foot"), STR("square feet"), STR("ft^2"), 0.09290304);
    AddNonSIUnitToLibrary(kSIQuantityArea, STR("square yard"), STR("square yards"), STR("yd^2"), 0.83612736);
    AddNonSIUnitToLibrary(kSIQuantityArea, STR("square mile"), STR("square miles"), STR("mi^2"), 2589988.110336);

    AddNonSIUnitToLibrary(kSIQuantityArea, STR("square rod"), STR("square rods"), STR("rod^2"), 5.029210 * 5.029210);
    AddNonSIUnitToLibrary(kSIQuantityArea, STR("square chain"), STR("square chains"), STR("ch^2"), 5.029210 * 5.029210 * 16);
    AddNonSIUnitToLibrary(kSIQuantityArea, STR("acre"), STR("acres"), STR("ac"), 4046.8564224);
    AddNonSIUnitToLibrary(kSIQuantityArea, STR("township"), STR("townships"), STR("twp"), 2589988.110336 * 36.);

    // ***** Inverse Area ***********************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityInverseArea
    AddNonSIUnitToLibrary(kSIQuantityInverseArea, STR("inverse hectare"), STR("inverse hectares"), STR("(1/ha)"), 1e-4);
    AddNonSIUnitToLibrary(kSIQuantityInverseArea, STR("inverse barn"), STR("inverse barns"), STR("(1/b)"), 1.e28);
    AddNonSIUnitToLibrary(kSIQuantityInverseArea, STR("inverse square kilometer"), STR("inverse square kilometer"), STR("(1/km^2)"), 1. / 1000000);
    AddNonSIUnitToLibrary(kSIQuantityInverseArea, STR("inverse square centimeter"), STR("inverse square centimeters"), STR("(1/cm^2)"), 1. / 0.0001);

    AddNonSIUnitToLibrary(kSIQuantityInverseArea, STR("inverse square inch"), STR("inverse square inches"), STR("(1/in^2)"), 1. / 0.00064516);
    AddNonSIUnitToLibrary(kSIQuantityInverseArea, STR("inverse square foot"), STR("inverse square feet"), STR("(1/ft^2)"), 1. / 0.09290304);
    AddNonSIUnitToLibrary(kSIQuantityInverseArea, STR("inverse square yard"), STR("inverse square yards"), STR("(1/yd^2)"), 1. / 0.83612736);
    AddNonSIUnitToLibrary(kSIQuantityInverseArea, STR("inverse acre"), STR("inverse acres"), STR("(1/ac)"), 1. / 4046.8564224);
    AddNonSIUnitToLibrary(kSIQuantityInverseArea, STR("inverse square mile"), STR("inverse square miles"), STR("(1/mi^2)"), 1. / 2589988.110336);

    // ***** Area - rock permeability ***********************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityRockPermeability
    AddNonSIUnitToLibrary(kSIQuantityRockPermeability, STR("darcy"), STR("darcys"), STR("Dc"), 9.869233e-13);
    AddNonSIUnitToLibrary(kSIQuantityRockPermeability, STR("millidarcy"), STR("millidarcys"), STR("mDc"), 9.869233e-16);
    AddNonSIUnitToLibrary(kSIQuantityRockPermeability, STR("microdarcy"), STR("microdarcys"), STR("µDc"), 9.869233e-19);
    AddNonSIUnitToLibrary(kSIQuantityRockPermeability, STR("nanodarcy"), STR("nanodarcys"), STR("nDc"), 9.869233e-21);

    // ***** Solid Angle ************************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantitySolidAngle
    AddUnitForQuantityToLibrary(kSIQuantitySolidAngle,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("steradian"), STR("steradians"), STR("sr"), kSIPrefixNone, true, 1., true);

    // ***** Area Ratio ************************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityAreaRatio
    AddNonSIUnitToLibrary(kSIQuantityAreaRatio, STR("square meter per square meter"), STR("square meters per square meter"), STR("m^2/m^2"), 1);

    // ***** Volume *****************************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityVolume
    AddUnitForQuantityToLibrary(kSIQuantityVolume,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("liter"), STR("liters"), STR("L"), kSIPrefixNone, false, 1e-3, true);

    // Volume
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("cubic inch"), STR("cubic inches"), STR("in^3"), (1609.344 / 63360) * (1609.344 / 63360) * (1609.344 / 63360));
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("cubic foot"), STR("cubic feet"), STR("ft^3"), (1609.344 / 5280) * (1609.344 / 5280) * (1609.344 / 5280));
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("cubic yard"), STR("cubic yards"), STR("yd^3"), (1609.344 / 1760) * (1609.344 / 1760) * (1609.344 / 1760));

    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("acre foot"), STR("acre feet"), STR("ac•ft"), 1609.344 / 5280 * 4046.8564224);
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("oil barrel"), STR("oil barrels"), STR("bbl"), 0.158987295);
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("thousand oil barrels"), STR("thousand oil barrels"), STR("Mbbl"), 0.158987295e3);
    AddNonSIUnitToLibrary(kSIQuantityVolume, STR("million oil barrels"), STR("million oil barrels"), STR("MMbbl"), 0.158987295e6);

    // ***** Inverse Volume *********************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityInverseVolume
    AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse liter"), STR("inverse liters"), STR("(1/L)"), 1. / 1e-3);
    AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse milliliter"), STR("inverse milliliters"), STR("(1/mL)"), 1. / 1e-6);
    AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse cubic inch"), STR("inverse cubic inches"), STR("(1/in^3)"), 1. / (1609.344 / 63360) * (1609.344 / 63360) * (1609.344 / 63360));
    AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse cubic foot"), STR("inverse cubic feet"), STR("(1/ft^3)"), 1. / (1609.344 / 5280) * (1609.344 / 5280) * (1609.344 / 5280));
    AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse cubic yard"), STR("inverse cubic yards"), STR("(1/yd^3)"), 1. / (1609.344 / 1760) * (1609.344 / 1760) * (1609.344 / 1760));

    AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse cubic centimeter"), STR("inverse cubic centimeters"), STR("(1/cm^3)"), 1000000.);

    AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse cubic millimeter"), STR("inverse cubic millimeters"), STR("(1/mm^3)"), 1000000000.);

    AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse cubic micrometer"), STR("inverse cubic micrometers"), STR("(1/µm^3)"), 1e+18);

    AddNonSIUnitToLibrary(kSIQuantityInverseVolume, STR("inverse cubic ångström"), STR("inverse cubic ångströms"), STR("(1/Å^3)"), 1e+30);

    // ***** Volume Ratio ***********************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityVolumeRatio
    AddNonSIUnitToLibrary(kSIQuantityVolumeRatio, STR("cubic meter per cubic meter"), STR("cubic meters per cubic meter"), STR("m^3/m^3"), 1);

    // ***** Surface Area to Volume Ratio *******************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantitySurfaceAreaToVolumeRatio
    AddNonSIUnitToLibrary(kSIQuantitySurfaceAreaToVolumeRatio, STR("square meter per cubic meter"), STR("square meters per cubic meter"), STR("m^2/m^3"), 1);
    AddNonSIUnitToLibrary(kSIQuantitySurfaceAreaToVolumeRatio, STR("square meter per liter"), STR("square meters per liter"), STR("m^2/L"), 1000);

    // ***** Speed ******************************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantitySpeed
    AddUnitForQuantityToLibrary(kSIQuantitySpeed,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("meter per second"), STR("meters per second"), STR("m/s"), kSIPrefixNone, false, 1, true);
    // UnitsQuantitiesLibrary contains an array of valid units for each quantity.
    units = (OCMutableArrayRef)OCDictionaryGetValue(unitsQuantitiesLibrary, kSIQuantitySpeed);
    OCDictionaryAddValue(unitsQuantitiesLibrary, kSIQuantityVelocity, units);

    AddUnitForQuantityToLibrary(kSIQuantitySpeed,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("meter per minute"), STR("meters per minute"), STR("m/min"), kSIPrefixNone, false, 1. / 60., true);

    AddUnitForQuantityToLibrary(kSIQuantitySpeed,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("meter per hour"), STR("meters per hour"), STR("m/h"), kSIPrefixNone, false, 1. / 3600., true);

    AddNonSIUnitToLibrary(kSIQuantitySpeed, STR("knot"), STR("knots"), STR("kn"), 0.514444444444444);
    // speed of light
    AddNonSIUnitToLibrary(kSIQuantitySpeed, STR("speed of light"), STR("speed of light"), STR("c_0"), kSISpeedOfLight);

    AddNonSIUnitToLibrary(kSIQuantitySpeed, STR("inch per second"), STR("inches per second"), STR("in/s"), 1609.344 / 63360);
    AddNonSIUnitToLibrary(kSIQuantitySpeed, STR("inch per minute"), STR("inches per minute"), STR("in/min"), 1609.344 / 63360 / 60.);
    AddNonSIUnitToLibrary(kSIQuantitySpeed, STR("inch per hour"), STR("inches per hour"), STR("in/h"), 1609.344 / 63360 / 3600.);

    AddNonSIUnitToLibrary(kSIQuantitySpeed, STR("foot per second"), STR("feet per second"), STR("ft/s"), 1609.344 / 5280);
    AddNonSIUnitToLibrary(kSIQuantitySpeed, STR("foot per minute"), STR("feet per minute"), STR("ft/min"), 1609.344 / 5280 / 60.);
    AddNonSIUnitToLibrary(kSIQuantitySpeed, STR("foot per hour"), STR("feet per hour"), STR("ft/h"), 1609.344 / 5280 / 3600.);

    AddNonSIUnitToLibrary(kSIQuantitySpeed, STR("mile per second"), STR("miles per second"), STR("mi/s"), 1609.344);
    AddNonSIUnitToLibrary(kSIQuantitySpeed, STR("mile per minute"), STR("miles per minute"), STR("mi/min"), 1609.344 / 60.);
    AddNonSIUnitToLibrary(kSIQuantitySpeed, STR("mile per hour"), STR("miles per hour"), STR("mi/h"), 1609.344 / 3600.);

#pragma mark kSIQuantityVelocity
    // Atomic Unit of Velocity
    AddNonSIUnitToLibrary(kSIQuantityVelocity, STR("atomic unit of velocity"), STR("atomic units of velocity"), STR("a_0•E_h/ℏ"), a_0 * E_h / hbar);

    // ***** Linear Momentum ********************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityLinearMomentum
    AddUnitForQuantityToLibrary(kSIQuantityLinearMomentum,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("gram meter per second"), STR("gram meters per second"), STR("m•g/s"), kSIPrefixNone, false, 0.001, true);

    AddUnitForQuantityToLibrary(kSIQuantityLinearMomentum,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("newton second"), STR("newton seconds"), STR("N•s"), kSIPrefixNone, false, 1, true);

    // ***** Angular Momentum, Action ***********************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityAngularMomentum
    AddUnitForQuantityToLibrary(kSIQuantityAngularMomentum,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("joule second"), STR("joules second"), STR("J•s"), kSIPrefixNone, false, 1, true);

    AddUnitForQuantityToLibrary(kSIQuantityAngularMomentum,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("gram square meter per second"), STR("gram square meters per second"), STR("g•m^2/s"), kSIPrefixNone, false, 0.001, true);

    // Action - planck constant
    AddNonSIUnitToLibrary(kSIQuantityAction, STR("planck constant"), STR("planck constant"), STR("h_P"), kSIPlanckConstant);

    // Reduced Action - planck constant/2π
    AddNonSIUnitToLibrary(kSIQuantityReducedAction, STR("reduced planck constant"), STR("reduced planck constant"), STR("ℏ"), hbar);

    // quantum of circulation
    AddNonSIUnitToLibrary(kSIQuantityCirculation, STR("quantum of circulation"), STR("quantum of circulation"), STR("h_P/(2•m_e)"), hbar);

    // second radiation constant
    AddNonSIUnitToLibrary(kSIQuantitySecondRadiationConstant, STR("second radiation constant"), STR("second radiation constant"), STR("h_P•c_0/k_B"), kSIPlanckConstant * kSISpeedOfLight / kSIBoltmannConstant);

    // von Klitzing constant
    AddNonSIUnitToLibrary(kSIQuantityElectricResistance, STR("von klitzing constant"), STR("von klitzing constant"), STR("h_P/(q_e^2)"), kSIPlanckConstant / (kSIElementaryCharge * kSIElementaryCharge));

    // ***** Acceleration ***********************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityAcceleration
    AddUnitForQuantityToLibrary(kSIQuantityAcceleration,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("meter per square second"), STR("meters per square second"), STR("m/s^2"), kSIPrefixNone, false, 1, true);

    AddUnitForQuantityToLibrary(kSIQuantityAcceleration,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("meter per hour per second"), STR("meters per hour per second"), STR("m/(h•s)"), kSIPrefixNone, false, 1. / 3600., true);

    // acceleration due to gravity at sea level
    AddNonSIUnitToLibrary(kSIQuantityAcceleration, STR("gravity acceleration"), STR("gravity acceleration"), STR("g_0"), kSIGravityAcceleration);

    /******** American System of Units not accepted in SI System ********/
    // Acceleration
    AddNonSIUnitToLibrary(kSIQuantityAcceleration, STR("mile per square second"), STR("miles per square second"), STR("mi/s^2"), 1609.344);
    AddNonSIUnitToLibrary(kSIQuantityAcceleration, STR("foot per square second"), STR("feet per square second"), STR("ft/s^2"), 1609.344 / 5280);
    AddNonSIUnitToLibrary(kSIQuantityAcceleration, STR("inch per square second"), STR("inches per square second"), STR("in/s^2"), 1609.344 / 63360);

    AddNonSIUnitToLibrary(kSIQuantityAcceleration, STR("mile per square minute"), STR("miles per square minute"), STR("mi/min^2"), 1609.344 / 60. / 60.);
    AddNonSIUnitToLibrary(kSIQuantityAcceleration, STR("foot per square minute"), STR("feet per square minute"), STR("ft/min^2"), 1609.344 / 5280 / 60. / 60.);
    AddNonSIUnitToLibrary(kSIQuantityAcceleration, STR("inch per square minute"), STR("inches per square minute"), STR("in/min^2"), 1609.344 / 63360 / 60. / 60.);

    AddNonSIUnitToLibrary(kSIQuantityAcceleration, STR("mile per hour per second"), STR("miles per hour per second"), STR("mi/(h•s)"), 1609.344 / 60. / 60.);

    AddNonSIUnitToLibrary(kSIQuantityAcceleration, STR("knot per second"), STR("knots per second"), STR("kn/s"), 0.514444444444444);

    // ***** Density ****************************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityDensity
    // Because kilogram is the base unit, this one is a little tricky to define.
    // Need to be false for is_special_si_symbol so scaling to coherent derived SI unit uses special symbol prefix and scale_to_coherent_si
    // In this case scale_to_coherent_si is 1e-3
    AddUnitForQuantityToLibrary(kSIQuantityDensity,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("gram per cubic meter"), STR("grams per cubic meter"), STR("g/m^3"), kSIPrefixNone, false, 1e-3, true);

    // ***** Mass Flow Rate ****************************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityMassFlowRate
    AddUnitForQuantityToLibrary(kSIQuantityMassFlowRate,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("gram per second"), STR("grams per second"), STR("g/s"), kSIPrefixNone, false, 1e-3, true);

    // ***** Mass Flux ****************************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityMassFlux
    AddUnitForQuantityToLibrary(kSIQuantityMassFlux,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("gram per square meter per second"), STR("grams per square meter per second"), STR("g/(m^2•s)"), kSIPrefixNone, false, 1e-3, true);

    // ***** Surface Density ********************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantitySurfaceDensity
    // Because kilogram is the base unit, this one is a little tricky to define.
    // Need to be false for is_special_si_symbol so scaling to coherent derived SI unit uses special symbol prefix and scale_to_coherent_si
    // In this case scale_to_coherent_si is 1e-3
    AddUnitForQuantityToLibrary(kSIQuantitySurfaceDensity,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("gram per square meter"), STR("grams per square meter"), STR("g/m^2"), kSIPrefixNone, false, 1e-3, true);

    // ***** Current Density ********************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityCurrentDensity
    AddUnitForQuantityToLibrary(kSIQuantityCurrentDensity,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("ampere per square meter"), STR("amperes per square meter"), STR("A/m^2"), kSIPrefixNone, true, 1, true);

    // ***** Amount Concentration ***************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityAmountConcentration
    AddUnitForQuantityToLibrary(kSIQuantityAmountConcentration,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("mole per cubic meter"), STR("moles per cubic meter"), STR("mol/m^3"), kSIPrefixNone, true, 1, true);

    AddUnitForQuantityToLibrary(kSIQuantityAmountConcentration,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("mole per liter"), STR("moles per liter"), STR("mol/L"), kSIPrefixNone, false, 1000, true);

    AddUnitForQuantityToLibrary(kSIQuantityAmountConcentration,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("mole per milliliter"), STR("moles per milliliter"), STR("mol/mL"), kSIPrefixNone, false, 1000000., true);

    AddUnitForQuantityToLibrary(kSIQuantityAmountConcentration,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("mole per microliter"), STR("moles per microliter"), STR("mol/µL"), kSIPrefixNone, false, 1000000000., true);

    AddUnitForQuantityToLibrary(kSIQuantityAmountConcentration,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("mole per liter"), STR("moles per liter"), STR("M"), kSIPrefixNone, false, 1000, true);

    // ***** Mass Concentration *****************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityMassConcentration
    // Because kilogram is the base unit, this one is a really tricky to define.
    // Need to be false for is_special_si_symbol so scaling to coherent derived SI unit uses special symbol prefix and scale_to_coherent_si
    // In this case scale_to_coherent_si is 1e-3 (for grams) times 1e3 (for liters) to give scale_to_coherent_si = 1
    AddUnitForQuantityToLibrary(kSIQuantityMassConcentration,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("gram per liter"), STR("grams per liter"), STR("g/L"), kSIPrefixNone, false, 1, true);

    // Because kilogram is the base unit, this one is a really tricky to define.
    // Need to be false for is_special_si_symbol so scaling to coherent derived SI unit uses special symbol prefix and scale_to_coherent_si
    // In this case scale_to_coherent_si is 1e-3 (for grams) times 1e6 (for liters) to give scale_to_coherent_si = 1e3
    AddUnitForQuantityToLibrary(kSIQuantityMassConcentration,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("gram per milliliter"), STR("grams per milliliter"), STR("g/mL"), kSIPrefixNone, false, 1e3, true);

    // Because kilogram is the base unit, this one is a really tricky to define.
    // Need to be false for is_special_si_symbol so scaling to coherent derived SI unit uses special symbol prefix and scale_to_coherent_si
    // In this case scale_to_coherent_si is 1e-3 (for grams) times 1e9 (for liters) to give scale_to_coherent_si = 1e6
    AddUnitForQuantityToLibrary(kSIQuantityMassConcentration,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("gram per microliter"), STR("grams per microliter"), STR("g/µL"), kSIPrefixNone, false, 1e6, true);

    // Special Names and Symbols for Coherent Derived Units - Table 3

    // ***** Force ******************************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityForce
    AddUnitForQuantityToLibrary(kSIQuantityForce,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("newton"), STR("newtons"), STR("N"), kSIPrefixNone, true, 1., true);

    // ***** Torque ******************************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityTorque
    AddUnitForQuantityToLibrary(kSIQuantityTorque,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("newton meter per radian"), STR("newton meters per radian"), STR("N•m/rad"), kSIPrefixNone, true, 1., true);

    AddUnitForQuantityToLibrary(kSIQuantityTorque,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("joule per radian"), STR("joules per radian"), STR("J/rad"), kSIPrefixNone, true, 1., true);

    AddNonSIUnitToLibrary(kSIQuantityTorque, STR("pound force foot per radian"), STR("pound force feet per radian"), STR("lbf•ft/rad"), 1.3558179483314);
    AddNonSIUnitToLibrary(kSIQuantityTorque, STR("pound force inch per radian"), STR("pound force inches per radian"), STR("lbf•in/rad"), 1.3558179483314 / 12.);
    AddNonSIUnitToLibrary(kSIQuantityTorque, STR("kilogram force meter per radian"), STR("kilogram force meters per radian"), STR("kgf•m/rad"), 9.80665);
    AddNonSIUnitToLibrary(kSIQuantityTorque, STR("kilogram force centimeter per radian"), STR("kilogram force centimeters per radian"), STR("kgf•cm/rad"), 0.0980665);

    // ***** Moment of Inertia ******************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityMomentOfInertia
    AddNonSIUnitToLibrary(kSIQuantityMomentOfInertia, STR("meter squared kilogram"), STR("meters squared kilogram"), STR("m^2•kg"), 1);
    AddNonSIUnitToLibrary(kSIQuantityMomentOfInertia, STR("meter squared gram"), STR("meters squared gram"), STR("m^2•g"), 1.e-3);
    AddNonSIUnitToLibrary(kSIQuantityMomentOfInertia, STR("centimeter squared kilogram"), STR("centimeter squared kilogram"), STR("cm^2•kg"), 0.0001);
    AddNonSIUnitToLibrary(kSIQuantityMomentOfInertia, STR("centimeter squared gram"), STR("centimeter squared gram"), STR("cm^2•g"), 1.e-7);

    // ***** Pressure, Stress *******************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityPressure, kSIQuantityStress
    // Pressure, Stress
    AddUnitForQuantityToLibrary(kSIQuantityPressure,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("pascal"), STR("pascals"), STR("Pa"), kSIPrefixNone, true, 1., true);
    // UnitsQuantitiesLibrary contains an array of valid units for each quantity.
    units = (OCMutableArrayRef)OCDictionaryGetValue(unitsQuantitiesLibrary, kSIQuantityPressure);
    OCDictionaryAddValue(unitsQuantitiesLibrary, kSIQuantityStress, units);

    AddNonSIUnitToLibrary(kSIQuantityPressure, STR("pound force per square inch"), STR("pounds force per square inch"), STR("lbf/in^2"), 6894.75729);
    AddNonSIUnitToLibrary(kSIQuantityPressure, STR("pound force per square inch"), STR("pounds force per square inch"), STR("psi"), 6894.75729);
    AddNonSIUnitToLibrary(kSIQuantityPressure, STR("pound force per square foot"), STR("pounds force per square feet"), STR("lbf/ft^2"), 47.880259);
    AddNonSIUnitToLibrary(kSIQuantityPressure, STR("torr"), STR("torrs"), STR("Torr"), 1.01325e5 / 760);

    // ***** Inverse Pressure : Compressibility *******************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityCompressibility
    AddNonSIUnitToLibrary(kSIQuantityCompressibility, STR("inverse pascal"), STR("inverse pascals"), STR("1/Pa"), 1);
    AddNonSIUnitToLibrary(kSIQuantityStressOpticCoefficient, STR("brewster"), STR("brewsters"), STR("B"), 1.e-12);

    // ***** Pressure Gradient ******************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityPressureGradient
    // Pressure, Stress
    AddUnitForQuantityToLibrary(kSIQuantityPressureGradient,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("pascal per meter"), STR("pascals per meter"), STR("Pa/m"), kSIPrefixNone, true, 1., true);

    AddNonSIUnitToLibrary(kSIQuantityPressureGradient, STR("pound force per square inch per foot"), STR("pounds force per square inch per foot"), STR("psi/ft"), 6894.75729 / (1609.344 / 5280));

    // ***** Energy, Work, Heat *****************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityEnergy
    AddUnitForQuantityToLibrary(kSIQuantityEnergy,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("joule"), STR("joules"), STR("J"), kSIPrefixNone, true, 1., true);

    AddUnitForQuantityToLibrary(kSIQuantityEnergy,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("watt hour"), STR("watt hour"), STR("W•h"), kSIPrefixNone, false, 3.6e3, true);

    AddNonSIUnitToLibrary(kSIQuantityEnergy, STR("rydberg"), STR("rydbergs"), STR("Ry"), Ry);

    // alpha particle mass energy
    AddNonSIUnitToLibrary(kSIQuantityMass, STR("alpha particle mass energy"), STR("alpha particle mass energy"), STR("m_a•c_0^2"), kSIAlphaParticleMass * kSISpeedOfLight * kSISpeedOfLight);

    // ***** Spectral Radiant Energy ****************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantitySpectralRadiantEnergy
    AddUnitForQuantityToLibrary(kSIQuantitySpectralRadiantEnergy,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixKilo,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("joule per nanometer"), STR("joules per nanometer"), STR("J/nm"), kSIPrefixNone, false, 1.e9, true);

    // ***** Power, Radiant Flux ****************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityPower, kSIQuantityRadiantFlux
    AddUnitForQuantityToLibrary(kSIQuantityPower,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("watt"), STR("watts"), STR("W"), kSIPrefixNone, true, 1., true);
    // UnitsQuantitiesLibrary contains an array of valid units for each quantity.
    units = (OCMutableArrayRef)OCDictionaryGetValue(unitsQuantitiesLibrary, kSIQuantityPower);
    OCDictionaryAddValue(unitsQuantitiesLibrary, kSIQuantityRadiantFlux, units);

    AddUnitForQuantityToLibrary(kSIQuantityPower,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("Joule per second"), STR("Joules per second"), STR("J/s"), kSIPrefixNone, true, 1., true);

    // ***** Spectral Power ****************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantitySpectralPower
    AddUnitForQuantityToLibrary(kSIQuantitySpectralPower,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixKilo,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("watt per nanometer"), STR("watts per nanometer"), STR("W/nm"), kSIPrefixNone, false, 1.e9, true);

    // ***** Volume Power Density ****************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityVolumePowerDensity
    AddUnitForQuantityToLibrary(kSIQuantityVolumePowerDensity,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("watt per cubic meter"), STR("watts per cubic meter"), STR("W/m^3"), kSIPrefixNone, true, 1., true);

    AddUnitForQuantityToLibrary(kSIQuantityVolumePowerDensity,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("watt per cubic centimeter"), STR("watts per cubic centimeter"), STR("W/cm^3"), kSIPrefixNone, false, 100000., true);

    // ***** Specific Power ****************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantitySpecificPower
    AddUnitForQuantityToLibrary(kSIQuantitySpecificPower,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixKilo,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("watt per kilogram"), STR("watts per kilogram"), STR("W/kg"), kSIPrefixNone, true, 1., true);

    AddNonSIUnitToLibrary(kSIQuantitySpecificPower, STR("horse power per pound"), STR("horse power per pound"), STR("hp/lb"), 1643.986806920936);
    AddNonSIUnitToLibrary(kSIQuantitySpecificPower, STR("horse power per ounce"), STR("horse power per ounce"), STR("hp/oz"), 26303.78891073498);

    // ***** Electric Charge, Amount of Electricity *********************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityElectricCharge, kSIQuantityAmountOfElectricity
    AddUnitForQuantityToLibrary(kSIQuantityElectricCharge,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("coulomb"), STR("coulombs"), STR("C"), kSIPrefixNone, true, 1., true);
    // UnitsQuantitiesLibrary contains an array of valid units for each quantity.
    units = (OCMutableArrayRef)OCDictionaryGetValue(unitsQuantitiesLibrary, kSIQuantityElectricCharge);
    OCDictionaryAddValue(unitsQuantitiesLibrary, kSIQuantityAmountOfElectricity, units);

    // ***** Electric Potential Difference, Electromotive Force, Voltage ************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityElectricPotentialDifference, kSIQuantityElectromotiveForce, kSIQuantityVoltage
    AddUnitForQuantityToLibrary(kSIQuantityElectricPotentialDifference,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("volt"), STR("volts"), STR("V"), kSIPrefixNone, true, 1., true);
    // UnitsQuantitiesLibrary contains an array of valid units for each quantity.
    units = (OCMutableArrayRef)OCDictionaryGetValue(unitsQuantitiesLibrary, kSIQuantityElectricPotentialDifference);
    OCDictionaryAddValue(unitsQuantitiesLibrary, kSIQuantityElectromotiveForce, units);
    OCDictionaryAddValue(unitsQuantitiesLibrary, kSIQuantityVoltage, units);

#pragma mark kSIQuantityElectricFieldGradient
    AddUnitForQuantityToLibrary(kSIQuantityElectricFieldGradient,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("volt per square meter"), STR("volts per square meter"), STR("V/m^2"), kSIPrefixNone, true, 1., true);

    AddNonSIUnitToLibrary(kSIQuantityElectricFieldGradient, STR("atomic unit of electric field gradient"), STR("atomic unit of electric field gradient"), STR("Λ_0"), Λ_0);
    AddNonSIUnitToLibrary(kSIQuantityElectricFieldGradient, STR("atomic unit of electric field gradient"), STR("atomic unit of electric field gradient"), STR("E_h/(q_e•a_0^2)"), Λ_0);

    // ***** Capacitance ************************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityCapacitance
    AddUnitForQuantityToLibrary(kSIQuantityCapacitance,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("farad"), STR("farads"), STR("F"), kSIPrefixNone, true, 1., true);

    // ***** Electric Resistance ****************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityElectricResistance
    AddUnitForQuantityToLibrary(kSIQuantityElectricResistance,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("ohm"), STR("ohms"), STR("Ω"), kSIPrefixNone, true, 1., true);

    // ***** Electric Resistance per length ****************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityElectricResistancePerLength
    AddUnitForQuantityToLibrary(kSIQuantityElectricResistancePerLength,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("ohm per meter"), STR("ohms per meter"), STR("Ω/m"), kSIPrefixNone, true, 1., true);

    AddUnitForQuantityToLibrary(kSIQuantityElectricResistancePerLength,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("ohm per feet"), STR("ohms per feet"), STR("Ω/ft"), kSIPrefixNone, false, 1. / (1609.344 / 5280), true);

    // ***** Electric Resistivity ****************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityElectricResistivity
    AddUnitForQuantityToLibrary(kSIQuantityElectricResistivity,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("ohm meter"), STR("ohms meter"), STR("Ω•m"), kSIPrefixNone, true, 1., true);

    AddUnitForQuantityToLibrary(kSIQuantityElectricResistivity,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("ohm centimeter"), STR("ohms centimeter"), STR("Ω•cm"), kSIPrefixNone, false, 0.01, true);

    // ***** Electric Conductance ***************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityElectricConductance
    AddUnitForQuantityToLibrary(kSIQuantityElectricConductance,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("siemen"), STR("siemens"), STR("S"), kSIPrefixNone, true, 1., true);

    // Conductance Quantum
    AddNonSIUnitToLibrary(kSIQuantityElectricConductance, STR("conductance quantum"), STR("conductance quantum"), STR("G_0"), G_0);

    // Inverse Conductance Quantum
    AddNonSIUnitToLibrary(kSIQuantityElectricResistance, STR("inverse conductance quantum"), STR("inverse conductance quantum"), STR("(1/G_0)"), 1 / G_0);

    // ***** Electric Conductivity ***************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityElectricConductivity
    AddUnitForQuantityToLibrary(kSIQuantityElectricConductivity,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("siemen per meter"), STR("siemens per meter"), STR("S/m"), kSIPrefixNone, true, 1., true);

    AddUnitForQuantityToLibrary(kSIQuantityElectricConductivity,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("siemen per centimeter"), STR("siemens per centimeter"), STR("S/cm"), kSIPrefixNone, false, 100., true);

    // ***** Molar Conductivity ***************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityMolarConductivity
    AddUnitForQuantityToLibrary(kSIQuantityMolarConductivity,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("siemen meter squared per mole"), STR("siemens meter squared per mole"), STR("S•m^2/mol"), kSIPrefixNone, true, 1., true);

    AddUnitForQuantityToLibrary(kSIQuantityMolarConductivity,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("siemen centimeter squared per mole"), STR("siemens centimeter squared per mole"), STR("S•cm^2/mol"), kSIPrefixNone, false, 0.0001, true);

    // ***** Gyromagnetic Ratio *****************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityGyromagneticRatio
    AddUnitForQuantityToLibrary(kSIQuantityGyromagneticRatio,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("radian per second per tesla"), STR("radians per second per tesla"), STR("rad/(s•T)"), kSIPrefixNone, true, 1., true);

    // ***** Magnetic Dipole Moment *************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityMagneticDipoleMoment
    AddUnitForQuantityToLibrary(kSIQuantityMagneticDipoleMoment,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("ampere square meter"), STR("ampere square meters"), STR("A•m^2"), kSIPrefixNone, true, 1., true);

    AddUnitForQuantityToLibrary(kSIQuantityMagneticDipoleMoment,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("joule per tesla"), STR("joules per tesla"), STR("J/T"), kSIPrefixNone, true, 1., true);

    AddNonSIUnitToLibrary(kSIQuantityMagneticDipoleMoment, STR("nuclear magneton"), STR("nuclear magnetons"), STR("µ_N"), mu_N);
    AddNonSIUnitToLibrary(kSIQuantityMagneticDipoleMoment, STR("bohr magneton"), STR("bohr magnetons"), STR("µ_B"), mu_e);

    AddNonSIUnitToLibrary(kSIQuantityMagneticDipoleMoment, STR("proton magnetic moment"), STR("proton magnetic moment"), STR("µ_p"), kSIProtonMagneticMoment);
    AddNonSIUnitToLibrary(kSIQuantityMagneticDipoleMoment, STR("neutron magnetic moment"), STR("neutron magnetic moment"), STR("µ_n"), kSINeutronMagneticMoment);
    AddNonSIUnitToLibrary(kSIQuantityMagneticDipoleMoment, STR("electron magnetic moment"), STR("electron magnetic moment"), STR("µ_e"), kSIElectronMagneticMoment);
    AddNonSIUnitToLibrary(kSIQuantityMagneticDipoleMoment, STR("muon magnetic moment"), STR("muon magnetic moment"), STR("µ_µ"), kSIMuonMagneticMoment);

    AddNonSIUnitToLibrary(kSIQuantityMagneticDipoleMomentRatio, STR("proton g factor"), STR("proton g factor"), STR("g_p"), kSIProtonGFactor);
    AddNonSIUnitToLibrary(kSIQuantityMagneticDipoleMomentRatio, STR("neutron g factor"), STR("neutron g factor"), STR("g_n"), kSINeutronGFactor);
    AddNonSIUnitToLibrary(kSIQuantityMagneticDipoleMomentRatio, STR("electron g factor"), STR("electron g factor"), STR("g_e"), kSIElectronGFactor);
    AddNonSIUnitToLibrary(kSIQuantityMagneticDipoleMomentRatio, STR("muon g factor"), STR("muon g factor"), STR("g_µ"), kSIMuonGFactor);

    // ***** Magnetic Flux **********************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityMagneticFlux
    AddUnitForQuantityToLibrary(kSIQuantityMagneticFlux,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("weber"), STR("webers"), STR("Wb"), kSIPrefixNone, true, 1., true);

    AddNonSIUnitToLibrary(kSIQuantityMagneticFlux, STR("magnetic flux quantum"), STR("magnetic flux quantum"), STR("Φ_0"), kSIPlanckConstant / (2 * kSIElementaryCharge));

    // ***** Magnetic Flux Density **************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityMagneticFluxDensity
    AddUnitForQuantityToLibrary(kSIQuantityMagneticFluxDensity,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("tesla"), STR("tesla"), STR("T"), kSIPrefixNone, true, 1., true);

    // ***** Magnetic Field Gradient ************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityMagneticFieldGradient
    AddUnitForQuantityToLibrary(kSIQuantityMagneticFieldGradient,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("tesla per meter"), STR("tesla per meter"), STR("T/m"), kSIPrefixNone, true, 1., true);

    AddNonSIUnitToLibrary(kSIQuantityMagneticFieldGradient, STR("tesla per centimeter"), STR("tesla per centimeter"), STR("T/cm"), 100.);
    AddNonSIUnitToLibrary(kSIQuantityMagneticFieldGradient, STR("gauss per centimeter"), STR("gauss per centimeter"), STR("G/cm"), 0.01);

#pragma mark kSIQuantityMolarMagneticSusceptibility
    AddNonSIUnitToLibrary(kSIQuantityMolarMagneticSusceptibility, STR("cubic meter per mole"), STR("cubic meters per mole"), STR("m^3/mol"), 1.);
    AddNonSIUnitToLibrary(kSIQuantityMolarMagneticSusceptibility, STR("cubic centimeter per mole"), STR("cubic centimeters per mole"), STR("cm^3/mol"), 1e-06);

    // ***** Inductance *************************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityInductance
    AddUnitForQuantityToLibrary(kSIQuantityInductance,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("henry"), STR("henries"), STR("H"), kSIPrefixNone, true, 1., true);

    // ***** Luminous Flux **********************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityLuminousFlux
    AddUnitForQuantityToLibrary(kSIQuantityLuminousFlux,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("lumen"), STR("lumens"), STR("lm"), kSIPrefixNone, false, 1., true);

#pragma mark kSIQuantityLuminousFluxDensity
    AddUnitForQuantityToLibrary(kSIQuantityLuminousFluxDensity,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("lumen per square meter"), STR("lumens per square meter"), STR("lm/m^2"), kSIPrefixNone, false, 1., true);

    AddNonSIUnitToLibrary(kSIQuantityLuminousFluxDensity, STR("lumen per square foot"), STR("lumens per square foot"), STR("lm/ft^2"), 10.76391041670972);

#pragma mark kSIQuantityLuminousEnergy
    AddUnitForQuantityToLibrary(kSIQuantityLuminousEnergy,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("lumen second"), STR("lumen seconds"), STR("lm•s"), kSIPrefixNone, false, 1., true);

    AddUnitForQuantityToLibrary(kSIQuantityLuminousFlux,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("candela steradian"), STR("candela steradians"), STR("cd•sr"), kSIPrefixNone, true, 1., true);

    // ***** Illuminance ************************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityIlluminance
    AddUnitForQuantityToLibrary(kSIQuantityIlluminance,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("lux"), STR("lux"), STR("lx"), kSIPrefixNone, true, 1., true);

    // ***** Absorbed dose **********************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityAbsorbedDose
    AddUnitForQuantityToLibrary(kSIQuantityAbsorbedDose,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("gray"), STR("grays"), STR("Gy"), kSIPrefixNone, true, 1., true);

    // ***** Dose equivalent ********************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityDoseEquivalent
    AddUnitForQuantityToLibrary(kSIQuantityDoseEquivalent,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("sievert"), STR("sieverts"), STR("Sv"), kSIPrefixNone, true, 1., true);

    // ***** Catalytic Activity *****************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityCatalyticActivity
    AddUnitForQuantityToLibrary(kSIQuantityCatalyticActivity,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("mole per second"), STR("moles per second"), STR("mol/s"), kSIPrefixNone, true, 1., true);

    AddUnitForQuantityToLibrary(kSIQuantityCatalyticActivity,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("mole per minute"), STR("moles per minute"), STR("mol/min"), kSIPrefixNone, false, 1. / 60., true);

    AddUnitForQuantityToLibrary(kSIQuantityCatalyticActivity,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixKilo,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("katal"), STR("katals"), STR("kat"), kSIPrefixNone, true, 1., true);

    AddUnitForQuantityToLibrary(kSIQuantityCatalyticActivityConcentration,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixKilo,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("katal per cubic meter"), STR("katals per cubic meter"), STR("kat/m^3"), kSIPrefixNone, true, 1., true);

    AddUnitForQuantityToLibrary(kSIQuantityCatalyticActivityContent,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixKilo,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("katal per kilogram"), STR("katals per kilogram"), STR("kat/kg"), kSIPrefixNone, true, 1., true);

    AddUnitForQuantityToLibrary(kSIQuantityCatalyticActivityConcentration,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixKilo,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("katal per liter"), STR("katals per liter"), STR("kat/L"), kSIPrefixNone, false, 1000., true);

    // Rate Per Amount Concentration Per Time Unit
    AddNonSIUnitToLibrary(kSIQuantityRatePerAmountConcentrationPerTime, STR("liter per mole per second"), STR("liter per mole per second"), STR("L/(mol•s)"), 0.001);

    // ***** Refractive Index *******************************************************************************************************************************
    // ******************************************************************************************************************************************************
#pragma mark kSIQuantityRefractiveIndex
    AddNonSIUnitToLibrary(kSIQuantityRefractiveIndex, STR("meter second per meter second"), STR("meter seconds per meter second"), STR("m•s/(m•s)"), 1.);

#pragma mark kSIQuantityVoltage
    // Atomic Unit of Electric Potential
    AddNonSIUnitToLibrary(kSIQuantityVoltage, STR("atomic unit of electric potential"), STR("atomic units of electric potential"), STR("E_h/q_e"), E_h / kSIElementaryCharge);

#pragma mark kSIQuantityElectricQuadrupoleMoment
    // Atomic Unit of Electric Quadrupole Moment
    AddNonSIUnitToLibrary(kSIQuantityElectricQuadrupoleMoment, STR("atomic unit of electric quadrupole moment"), STR("atomic units of electric quadrupole moment"), STR("q_e•a_0^2"), kSIElementaryCharge * a_0 * a_0);

#pragma mark kSIQuantityForce
    // Atomic Unit of Force
    AddNonSIUnitToLibrary(kSIQuantityForce, STR("atomic unit of force"), STR("atomic units of force"), STR("E_h/a_0"), E_h / a_0);

#pragma mark kSIQuantityMagneticDipoleMoment
    // Atomic Unit of Magnetic Dipole Moment
    AddNonSIUnitToLibrary(kSIQuantityMagneticDipoleMoment, STR("atomic unit of magnetic dipole moment"), STR("atomic units of magnetic dipole moment"), STR("ℏ•q_e/m_e"), hbar * kSIElementaryCharge / kSIElectronMass);

#pragma mark kSIQuantityMagneticFluxDensity
    // Atomic Unit of Magnetic Flux Density
    AddNonSIUnitToLibrary(kSIQuantityMagneticFluxDensity, STR("atomic unit of magnetic flux density"), STR("atomic units of magnetic flux density"), STR("ℏ/(q_e•a_0^2)"), hbar / (kSIElementaryCharge * a_0 * a_0));

#pragma mark kSIQuantityMagnetizability
    // Atomic Unit of Magnetizability
    AddNonSIUnitToLibrary(kSIQuantityMagnetizability, STR("atomic unit of magnetizability"), STR("atomic units of magnetizability"), STR("q_e•a_0^2/m_e"), kSIElementaryCharge * a_0 * a_0 / kSIElectronMass);

#pragma mark kSIQuantityLinearMomentum
    // Atomic Unit of Momentum
    AddNonSIUnitToLibrary(kSIQuantityLinearMomentum, STR("atomic unit of momentum"), STR("atomic units of momentum"), STR("ℏ/a_0"), hbar / a_0);

#pragma mark kSIQuantityPermittivity
    // Atomic Unit of Permittivity
    AddNonSIUnitToLibrary(kSIQuantityPermittivity, STR("atomic unit of permittivity"), STR("atomic units of permittivity"), STR("q_e^2/(a_0•E_h)"), kSIElementaryCharge * kSIElementaryCharge / (a_0 * E_h));

#pragma mark kSIQuantityElectricResistance
    // Characteristic Impedance of Vacuum
    AddNonSIUnitToLibrary(kSIQuantityElectricResistance, STR("characteristic impedance of vacuum"), STR("characteristic impedance of vacuum"), STR("Z_0"), 4 * kSIPi * 1.e-7 * kSISpeedOfLight);

#pragma mark kSIQuantityElectricPolarizability
    // Atomic Unit of Electric Polarizability
    AddNonSIUnitToLibrary(kSIQuantityElectricPolarizability, STR("atomic unit of electric polarizability"), STR("atomic units of electric polarizability"), STR("q_e^2•a_0^2/E_h"), kSIElementaryCharge * kSIElementaryCharge * a_0 * a_0 / (E_h));

    // First Hyperpolarizability
    AddNonSIUnitToLibrary(kSIQuantityFirstHyperPolarizability, STR("atomic unit of 1st polarizability"), STR("atomic units of 1st polarizability"), STR("q_e^3•a_0^3/E_h^2"), kSIElementaryCharge * kSIElementaryCharge * kSIElementaryCharge * a_0 * a_0 * a_0 / (E_h * E_h));

    // Second Hyperpolarizability
    AddNonSIUnitToLibrary(kSIQuantitySecondHyperPolarizability, STR("atomic unit of 2nd polarizability"), STR("atomic units of 2nd polarizability"), STR("q_e^4•a_0^4/E_h^3"), kSIElementaryCharge * kSIElementaryCharge * kSIElementaryCharge * kSIElementaryCharge * a_0 * a_0 * a_0 * a_0 / (E_h * E_h * E_h));

    // Specific Volume
    AddNonSIUnitToLibrary(kSIQuantitySpecificVolume, STR("cubic meter per kilogram"), STR("cubic meters per kilogram"), STR("m^3/kg"), 1.);

    // Table 6 - Non-SI units but SI accepted

    // Plane Angle
    AddNonSIUnitToLibrary(kSIQuantityPlaneAngle, STR("degree"), STR("degrees"), STR("°"), kSIPi / 180.);

    // Non-SI units whose values in SI Units must be obtained experimentally - Table 7

    // Units accepted for use with the SI
    // Energy
    AddUnitForQuantityToLibrary(kSIQuantityEnergy,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("electronvolt"), STR("electronvolts"), STR("eV"), kSIPrefixNone, false, kSIElementaryCharge, true);

    // Mass to Charge Ratio = Thompson
    AddNonSIUnitToLibrary(kSIQuantityMassToChargeRatio, STR("thomson"), STR("thomson"), STR("Th"), kSIAtomicMassConstant / kSIElementaryCharge);

    // Charge to Mass Ratio = Inverse Thompson
    AddNonSIUnitToLibrary(kSIQuantityChargeToMassRatio, STR("inverse thomson"), STR("inverse thomson"), STR("(1/Th)"), kSIElementaryCharge / kSIAtomicMassConstant);

    // Table 8 - Other Non-SI units

    // Pressure, Stress - bar
    AddUnitForQuantityToLibrary(kSIQuantityPressure,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("bar"), STR("bars"), STR("bar"), kSIPrefixNone, false, 1e5, true);

    // Pressure - millimeters of mercury
    AddNonSIUnitToLibrary(kSIQuantityPressure, STR("millimeter of Hg"), STR("millimeters of Hg"), STR("mmHg"), 133.322);

    // Pressure - atmospheres
    AddNonSIUnitToLibrary(kSIQuantityPressure, STR("atmosphere"), STR("atmospheres"), STR("atm"), 1.01325e5);

    // Table 9 - Non-SI units associated with the CGS and the CGS-Gaussian system
    // Energy - Erg
    AddUnitForQuantityToLibrary(kSIQuantityEnergy,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("erg"), STR("ergs"), STR("erg"), kSIPrefixNone, false, 1e-7, true);

    // Force - Dyne
    AddUnitForQuantityToLibrary(kSIQuantityForce,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("dyne"), STR("dynes"), STR("dyn"), kSIPrefixNone, false, 1e-5, true);

    // Dynamic Viscosity
    AddUnitForQuantityToLibrary(kSIQuantityDynamicViscosity,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("poise"), STR("poises"), STR("P"), kSIPrefixNone, false, 0.1, true);

    // Kinematic Viscosity
    AddUnitForQuantityToLibrary(kSIQuantityKinematicViscosity,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("stokes"), STR("stokes"), STR("St"), kSIPrefixNone, false, 1e-4, true);

    AddNonSIUnitToLibrary(kSIQuantityDiffusionCoefficient, STR("square meter per second"), STR("square meters per second"), STR("m^2/s"), 1);
    AddNonSIUnitToLibrary(kSIQuantityDiffusionCoefficient, STR("square centimeter per second"), STR("square centimeters per second"), STR("cm^2/s"), 0.0001);
    AddNonSIUnitToLibrary(kSIQuantityDiffusionCoefficient, STR("square millimeter per second"), STR("square millimeters per second"), STR("mm^2/s"), 1e-6);

    // Luminance
    AddUnitForQuantityToLibrary(kSIQuantityLuminance,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("stilb"), STR("stilbs"), STR("sb"), kSIPrefixNone, false, 1e4, true);

    AddUnitForQuantityToLibrary(kSIQuantityLuminance,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("lumen per square meter per steradian"), STR("lumens per square meter per steradian"), STR("lm/(m^2•sr)"), kSIPrefixNone, false, 1, true);

    // Illuminance
    AddUnitForQuantityToLibrary(kSIQuantityIlluminance,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("phot"), STR("phots"), STR("ph"), kSIPrefixNone, false, 1e4, true);

    /*  Sorry Galileo - too close to Gallons
     // Acceleration - gal
     AddUnitForQuantityToLibrary(kSIQuantityAcceleration,
     kSIPrefixNone,kSIPrefixNone,
     kSIPrefixKilo,kSIPrefixNone,
     kSIPrefixNone,kSIPrefixNone,
     kSIPrefixNone,kSIPrefixNone,
     kSIPrefixNone,kSIPrefixNone,
     kSIPrefixNone,kSIPrefixNone,
     kSIPrefixNone,kSIPrefixNone,
     STR("galileo"),STR("galileo"),STR("Gal"), kSIPrefixNone, false, 1e-2,true);
     */

    // Magnetic Flux - maxwell
    AddUnitForQuantityToLibrary(kSIQuantityMagneticFlux,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("maxwell"), STR("maxwells"), STR("Mx"), kSIPrefixNone, false, 1e-8, true);

    // Magnetic Flux Density - gauss
    AddUnitForQuantityToLibrary(kSIQuantityMagneticFluxDensity,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("gauss"), STR("gauss"), STR("G"), kSIPrefixNone, false, 1e-4, true);

    AddNonSIUnitToLibrary(kSIQuantityInverseMagneticFluxDensity, STR("inverse gauss"), STR("inverse gauss"), STR("(1/G)"), 1.);

    // Magnetic Field Strength - œrsted
    AddUnitForQuantityToLibrary(kSIQuantityMagneticFieldStrength,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("œrsted"), STR("œrsteds"), STR("Oe"), kSIPrefixNone, false, 79.577471545947674, true);

    AddUnitForQuantityToLibrary(kSIQuantityMagneticFieldStrength,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("ampere per meter"), STR("ampere per meter"), STR("A/m"), kSIPrefixNone, true, 1., true);

    // Table 4

    AddUnitForQuantityToLibrary(kSIQuantityDynamicViscosity,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("pascal second"), STR("pascal seconds"), STR("Pa•s"), kSIPrefixNone, true, 1., true);

    AddUnitForQuantityToLibrary(kSIQuantityDynamicViscosity,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("newton second per square meter"), STR("newton seconds per square meter"), STR("N•s/m^2"), kSIPrefixNone, true, 1., true);

    AddUnitForQuantityToLibrary(kSIQuantityMomentOfForce,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("newton meter"), STR("newton meters"), STR("N•m"), kSIPrefixNone, true, 1., true);

    AddUnitForQuantityToLibrary(kSIQuantitySurfaceTension,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("netwon per meter"), STR("newtons per meter"), STR("N/m"), kSIPrefixNone, true, 1., true);

    AddUnitForQuantityToLibrary(kSIQuantityElasticModulus,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("netwon per square meter"), STR("newtons per square meter"), STR("N/m^2"), kSIPrefixNone, true, 1., true);

    AddNonSIUnitToLibrary(kSIQuantitySurfaceTension, STR("dyne per centimeter"), STR("dynes per centimeter"), STR("dyn/cm"), 0.001);

    AddUnitForQuantityToLibrary(kSIQuantitySurfaceEnergy,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("joule per square meter"), STR("joules per square meter"), STR("J/m^2"), kSIPrefixNone, true, 1., true);

    AddNonSIUnitToLibrary(kSIQuantitySurfaceEnergy, STR("dyne per square centimeter"), STR("dynes per square centimeter"), STR("dyn/cm^2"), 0.1);

    AddUnitForQuantityToLibrary(kSIQuantityAngularFrequency,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("radian per second"), STR("radians per second"), STR("rad/s"), kSIPrefixNone, true, 1., true);
    // UnitsQuantitiesLibrary contains an array of valid units for each quantity.
    units = (OCMutableArrayRef)OCDictionaryGetValue(unitsQuantitiesLibrary, kSIQuantityAngularFrequency);
    OCDictionaryAddValue(unitsQuantitiesLibrary, kSIQuantityAngularSpeed, units);
    OCDictionaryAddValue(unitsQuantitiesLibrary, kSIQuantityAngularVelocity, units);

    AddUnitForQuantityToLibrary(kSIQuantityAngularAcceleration,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("radian per square second"), STR("radians per square second"), STR("rad/s^2"), kSIPrefixNone, true, 1., true);

    AddUnitForQuantityToLibrary(kSIQuantityHeatFluxDensity,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("watt per square meter"), STR("watts per square meter"), STR("W/m^2"), kSIPrefixNone, true, 1., true);
    // UnitsQuantitiesLibrary contains an array of valid units for each quantity.
    units = (OCMutableArrayRef)OCDictionaryGetValue(unitsQuantitiesLibrary, kSIQuantityHeatFluxDensity);
    OCDictionaryAddValue(unitsQuantitiesLibrary, kSIQuantityIrradiance, units);

    AddUnitForQuantityToLibrary(kSIQuantityHeatFluxDensity,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("watt per square centimeter"), STR("watts per square centimeter"), STR("W/cm^2"), kSIPrefixNone, false, 10000., true);

    AddNonSIUnitToLibrary(kSIQuantityHeatFluxDensity, STR("watt per square foot"), STR("watts per square foot"), STR("W/ft^2"), 10.76391041670972);

    AddNonSIUnitToLibrary(kSIQuantityHeatFluxDensity, STR("watt per square inch"), STR("watts per square inch"), STR("W/in^2"), 10.76391041670972 / 12.);

    AddUnitForQuantityToLibrary(kSIQuantitySpectralRadiantFluxDensity,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixKilo,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("watt per square meter per nanometer"), STR("watts per square meter per nanometer"), STR("W/(m^2•nm)"), kSIPrefixNone, false, 1.e9, true);

    AddUnitForQuantityToLibrary(kSIQuantityEntropy,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("joule per kelvin"), STR("joules per kelvin"), STR("J/K"), kSIPrefixNone, true, 1., true);
    // UnitsQuantitiesLibrary contains an array of valid units for each quantity.
    units = (OCMutableArrayRef)OCDictionaryGetValue(unitsQuantitiesLibrary, kSIQuantityEntropy);
    OCDictionaryAddValue(unitsQuantitiesLibrary, kSIQuantityHeatCapacity, units);

    AddUnitForQuantityToLibrary(kSIQuantitySpecificHeatCapacity,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("joule per kilogram kelvin"), STR("joules per kilogram kelvin"), STR("J/(kg•K)"), kSIPrefixNone, true, 1., true);

    AddUnitForQuantityToLibrary(kSIQuantitySpecificHeatCapacity,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("joule per gram kelvin"), STR("joules per gram kelvin"), STR("J/(g•K)"), kSIPrefixNone, false, 1000., true);

    // UnitsQuantitiesLibrary contains an array of valid units for each quantity.
    units = (OCMutableArrayRef)OCDictionaryGetValue(unitsQuantitiesLibrary, kSIQuantitySpecificHeatCapacity);
    OCDictionaryAddValue(unitsQuantitiesLibrary, kSIQuantitySpecificEntropy, units);

    AddNonSIUnitToLibrary(kSIQuantitySpecificHeatCapacity, STR("calorie per gram per kelvin"), STR("calories per gram per kelvin"), STR("cal/(g•K)"), 4186.8);

    AddUnitForQuantityToLibrary(kSIQuantityMolarMass,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("gram per mole"), STR("grams per mole"), STR("g/mol"), kSIPrefixNone, false, 1e-3, true);

    AddUnitForQuantityToLibrary(kSIQuantityMolality,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixKilo,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("mole per kilogram"), STR("moles per kilogram"), STR("mol/kg"), kSIPrefixNone, true, 1., true);

    AddUnitForQuantityToLibrary(kSIQuantitySpecificEnergy,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixKilo,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("joule per kilogram"), STR("joules per kilogram"), STR("J/kg"), kSIPrefixNone, true, 1, true);
    AddUnitForQuantityToLibrary(kSIQuantitySpecificEnergy,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixKilo,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("joule per gram"), STR("joules per gram"), STR("J/g"), kSIPrefixNone, false, 1e3, true);

    AddUnitForQuantityToLibrary(kSIQuantityThermalConductance,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("watt per kelvin"), STR("watts per kelvin"), STR("W/K"), kSIPrefixNone, true, 1., true);
    AddNonSIUnitToLibrary(kSIQuantityThermalConductance, STR("Btu per hour per rankine"),
                          STR("Btus per hour per rankine"),
                          STR("Btu/(h•°R)"), 0.5275279262867396);
    AddNonSIUnitToLibrary(kSIQuantityThermalConductance, STR("calorie per hour per kelvin"),
                          STR("calories per hour per kelvin"),
                          STR("cal/(h•K)"), 1.163e-3);
    AddNonSIUnitToLibrary(kSIQuantityThermalConductance, STR("kilocalorie per hour per kelvin"),
                          STR("kilocalories per hour per kelvin"),
                          STR("kcal/(h•K)"), 1.163);

    AddUnitForQuantityToLibrary(kSIQuantityThermalConductivity,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("watt per meter kelvin"), STR("watts per meter kelvin"), STR("W/(m•K)"), kSIPrefixNone, true, 1., true);

    AddNonSIUnitToLibrary(kSIQuantityThermalConductivity, STR("Btu per hour per foot per rankine"),
                          STR("Btus per hour per foot per rankine"),
                          STR("Btu/(h•ft•°R)"), 1.730734666295077);
    AddNonSIUnitToLibrary(kSIQuantityThermalConductivity, STR("calorie per hour per meter per kelvin"),
                          STR("calories per hour per meter per kelvin"),
                          STR("cal/(h•m•K)"), 1.163e-3);
    AddNonSIUnitToLibrary(kSIQuantityThermalConductivity, STR("kilocalorie per hour per meter per kelvin"),
                          STR("kilocalories per hour per meter per kelvin"),
                          STR("kcal/(h•m•K)"), 1.163);

    AddUnitForQuantityToLibrary(kSIQuantityEnergyDensity,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("joule per cubic meter"), STR("joules per cubic meter"), STR("J/m^3"), kSIPrefixNone, true, 1., true);

    AddUnitForQuantityToLibrary(kSIQuantityEnergyDensity,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("joule per liter"), STR("joules per liter"), STR("J/L"), kSIPrefixNone, false, 1000., true);

    AddUnitForQuantityToLibrary(kSIQuantityElectricDipoleMoment,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("coulomb meter"), STR("coulomb meters"), STR("C•m"), kSIPrefixNone, true, 1., true);

    AddNonSIUnitToLibrary(kSIQuantityElectricDipoleMoment, STR("debye"), STR("debyes"), STR("D"), 3.335640951816991e-30);
    AddNonSIUnitToLibrary(kSIQuantityElectricDipoleMoment, STR("atomic unit of electric dipole moment"), STR("atomic unit of electric dipole moment"), STR("q_e•a_0"), kSIElementaryCharge * a_0);

    AddUnitForQuantityToLibrary(kSIQuantityElectricFieldStrength,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("volt per meter"), STR("volts per meter"), STR("V/m"), kSIPrefixNone, true, 1., true);

    AddNonSIUnitToLibrary(kSIQuantityElectricFieldStrength, STR("atomic unit of electric field"), STR("atomic unit of electric field"), STR("E_h/(q_e•a_0)"), E_h / (kSIElementaryCharge * a_0));

    AddUnitForQuantityToLibrary(kSIQuantityElectricFieldStrength,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("volt per centimeter"), STR("volts per centimeter"), STR("V/cm"), kSIPrefixNone, false, 100., true);

    AddUnitForQuantityToLibrary(kSIQuantityElectricFieldStrength,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("newton per coulomb"), STR("newtons per coulomb"), STR("N/C"), kSIPrefixNone, true, 1., true);

    AddUnitForQuantityToLibrary(kSIQuantityElectricFlux,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("volt meter"), STR("volts meter"), STR("V•m"), kSIPrefixNone, true, 1., true);

    AddUnitForQuantityToLibrary(kSIQuantityElectricChargeDensity,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("coulomb per cubic meter"), STR("coulombs per cubic meter"), STR("C/m^3"), kSIPrefixNone, true, 1., true);

    AddNonSIUnitToLibrary(kSIQuantityElectricChargeDensity, STR("charge density"),
                          STR("charge density"), STR("A•h/L"), 3600000);

    AddNonSIUnitToLibrary(kSIQuantityElectricChargeDensity, STR("atomic unit of charge density"),
                          STR("atomic unit of charge density"), STR("q_e/a_0^3"), kSIElementaryCharge / (a_0 * a_0 * a_0));

    AddUnitForQuantityToLibrary(kSIQuantitySurfaceChargeDensity,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("coulomb per square meter"), STR("coulombs per square meter"), STR("C/m^2"), kSIPrefixNone, true, 1., true);
    // UnitsQuantitiesLibrary contains an array of valid units for each quantity.
    units = (OCMutableArrayRef)OCDictionaryGetValue(unitsQuantitiesLibrary, kSIQuantitySurfaceChargeDensity);
    OCDictionaryAddValue(unitsQuantitiesLibrary, kSIQuantityElectricFluxDensity, units);
    OCDictionaryAddValue(unitsQuantitiesLibrary, kSIQuantityElectricDisplacement, units);

    AddUnitForQuantityToLibrary(kSIQuantityPermittivity,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("farad per meter"), STR("farads per meter"), STR("F/m"), kSIPrefixNone, true, 1., true);

    AddUnitForQuantityToLibrary(kSIQuantityPermittivity,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("coulomb per volt meter"), STR("coulombs per volt meter"), STR("C/(V•m)"), kSIPrefixNone, true, 1., true);

    AddUnitForQuantityToLibrary(kSIQuantityPermeability,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("henry per meter"), STR("henries per meter"), STR("H/m"), kSIPrefixNone, true, 1., true);

    AddUnitForQuantityToLibrary(kSIQuantityPermeability,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("newton per square ampere"), STR("newtons per square ampere"), STR("N/A^2"), kSIPrefixNone, true, 1., true);

    AddUnitForQuantityToLibrary(kSIQuantityPermeability,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("tesla meter per ampere"), STR("tesla meter per ampere"), STR("T•m/A"), kSIPrefixNone, true, 1., true);

    AddUnitForQuantityToLibrary(kSIQuantityPermeability,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("weber per ampere meter"), STR("webers per ampere meter"), STR("Wb/(A•m)"), kSIPrefixNone, true, 1., true);

    AddUnitForQuantityToLibrary(kSIQuantityMolarEntropy,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("joule per mole kelvin)"), STR("joules per mole kelvin"), STR("J/(mol•K)"), kSIPrefixNone, true, 1., true);
    // UnitsQuantitiesLibrary contains an array of valid units for each quantity.
    units = (OCMutableArrayRef)OCDictionaryGetValue(unitsQuantitiesLibrary, kSIQuantityMolarEntropy);
    OCDictionaryAddValue(unitsQuantitiesLibrary, kSIQuantityMolarHeatCapacity, units);

    AddUnitForQuantityToLibrary(kSIQuantityMolarEnergy,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("joule per mole"), STR("joules per mole"), STR("J/mol"), kSIPrefixNone, true, 1., true);

    AddUnitForQuantityToLibrary(kSIQuantityRadiationExposure,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("coulomb per kilogram"), STR("coulombs per kilogram"), STR("C/kg"), kSIPrefixNone, true, 1., true);

    AddUnitForQuantityToLibrary(kSIQuantityAbsorbedDoseRate,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("gray per second"), STR("grays per second"), STR("Gy/s"), kSIPrefixNone, true, 1., true);

    AddUnitForQuantityToLibrary(kSIQuantityRadiantIntensity,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("watt per steradian"), STR("watts per steradian"), STR("W/sr"), kSIPrefixNone, true, 1., true);

    AddUnitForQuantityToLibrary(kSIQuantitySpectralRadiantIntensity,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("watt per steradian per nanometer"), STR("watts per steradian per nanometer"), STR("W/(sr•nm)"), kSIPrefixNone, false, 1.e9, true);

    AddUnitForQuantityToLibrary(kSIQuantityRadiance,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("watt per square meter per steradian"), STR("watts per square meter per steradian"), STR("W/(m^2•sr)"), kSIPrefixNone, true, 1., true);

    AddUnitForQuantityToLibrary(kSIQuantitySpectralRadiance,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("watt per square meter per steradian per nanometer"), STR("watts per square meter steradian per nanometer"), STR("W/(m^2•sr•nm)"), kSIPrefixNone, false, 1.e9, true);

    AddUnitForQuantityToLibrary(kSIQuantityFrequencyPerMagneticFluxDensity,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("hertz per tesla"), STR("hertz per tesla"), STR("Hz/T"), kSIPrefixNone, true, 1., true);

    AddUnitForQuantityToLibrary(kSIQuantityLengthPerVolume,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("meter per liter"), STR("meters per liter"), STR("m/L"), kSIPrefixNone, false, 1. / 1.e-3, true);

    AddUnitForQuantityToLibrary(kSIQuantityPowerPerLuminousFlux,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("watt per lumens"), STR("watts per lumen"), STR("W/lm"), kSIPrefixNone, true, 1., true);

    AddUnitForQuantityToLibrary(kSIQuantityLuminousEfficacy,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("lumen per watt"), STR("lumens per watt"), STR("lm/W"), kSIPrefixNone, false, 1., true);

    AddUnitForQuantityToLibrary(kSIQuantityHeatTransferCoefficient,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixKilo, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                kSIPrefixNone, kSIPrefixNone,
                                STR("watt per square meter per kelvin"), STR("watts per square meter per kelvin"), STR("W/(m^2•K)"), kSIPrefixNone, true, 1., true);

    AddNonSIUnitToLibrary(kSIQuantityHeatTransferCoefficient, STR("Btu per hour per square foot per rankine"), STR("Btus per hour per square foot per rankine"), STR("Btu/(h•ft^2•°R)"), 5.678263340863113);
    AddNonSIUnitToLibrary(kSIQuantityHeatTransferCoefficient, STR("calorie per hour per square meter per kelvin"), STR("calories per hour per square meter per kelvin"), STR("cal/(h•m^2•K)"), 1.163e-3);
    AddNonSIUnitToLibrary(kSIQuantityHeatTransferCoefficient, STR("kilocalorie per hour per square meter per kelvin"), STR("kilocalories per hour per square meter per kelvin"), STR("kcal/(h•m^2•K)"), 1.163);

    // Energy, Work, Heat
    AddNonSIUnitToLibrary(kSIQuantityEnergy, STR("calorie"), STR("calories"), STR("cal"), 4.1868);
    AddNonSIUnitToLibrary(kSIQuantityEnergy, STR("kilocalorie"), STR("kilocalories"), STR("kcal"), 4.1868 * 1000.);

    AddNonSIUnitToLibrary(kSIQuantityMolarEnergy, STR("calorie per mole"), STR("calories per mole"), STR("cal/mol"), 4.1868);
    AddNonSIUnitToLibrary(kSIQuantityMolarEnergy, STR("kilocalorie per mole"), STR("kilocalories per mole"), STR("kcal/mol"), 4.1868 * 1000.);

    /******** Math and Scientific Constants ********/
    // pi
    AddNonSIUnitToLibrary(kSIQuantityPlaneAngle, STR("pi"), STR("pi"), STR("π"), kSIPi);
    // Euler's number
    AddNonSIUnitToLibrary(kSIQuantityDimensionless, STR("euler constant"), STR("euler constant"), STR("exp(1)"), kSIEulersNumber);

    // boltzmann constant
    AddNonSIUnitToLibrary(kSIQuantityHeatCapacity, STR("boltzmann constant"), STR("boltzmann constant"), STR("k_B"), kSIBoltmannConstant);

    // Gas constant
    AddNonSIUnitToLibrary(kSIQuantityMolarHeatCapacity, STR("gas constant"), STR("gas constant"), STR("R"), kSIBoltmannConstant * kSIAvogadroConstant);

    // elementary charge
    AddNonSIUnitToLibrary(kSIQuantityElectricCharge, STR("elementary charge"), STR("elementary charge"), STR("q_e"), kSIElementaryCharge);

    AddNonSIUnitToLibrary(kSIQuantityWavenumber, STR("inverse atomic unit of length"), STR("inverse atomic unit of length"), STR("(1/a_0)"), 1 / a_0);

    // atomic unit of energy
    // E_h = m_e • (q_e^2/(2•ε_0•h_P))^2
    AddNonSIUnitToLibrary(kSIQuantityEnergy, STR("atomic unit of energy"), STR("atomic unit of energy"), STR("E_h"), E_h);

    // Permittivity
    AddNonSIUnitToLibrary(kSIQuantityPermittivity, STR("electric constant"), STR("electric constant"), STR("ε_0"), kSIElectricConstant);

    // Permeability
    AddNonSIUnitToLibrary(kSIQuantityPermeability, STR("magnetic constant"), STR("magnetic constant"), STR("µ_0"), 4 * kSIPi * 1.e-7);

    // avogadro constant
    AddNonSIUnitToLibrary(kSIQuantityInverseAmount, STR("avogadro constant"), STR("avogadro constant"), STR("N_A"), kSIAvogadroConstant);

    // faraday constant
    AddNonSIUnitToLibrary(kSIQuantityChargePerAmount, STR("faraday constant"), STR("faraday constant"), STR("&F"), kSIElementaryCharge * kSIAvogadroConstant);
    AddNonSIUnitToLibrary(kSIQuantityChargePerAmount, STR("coulomb per mole"), STR("coulombs per mole"), STR("C/mol"), 1.0);

    // gravitational constant
    AddNonSIUnitToLibrary(kSIQuantityGravitationalConstant, STR("gravitational constant"), STR("gravitational constant"), STR("&G"), kSIGravitaionalConstant);

    // Heat, Energy
    AddNonSIUnitToLibrary(kSIQuantityEnergy, STR("british thermal unit"), STR("british thermal units"), STR("Btu"), 1055.05585257348);
    AddNonSIUnitToLibrary(kSIQuantityMomentOfForce, STR("foot pound force"), STR("feet pound force"), STR("ft•lbf"), 1.3558179483314);
    AddNonSIUnitToLibrary(kSIQuantityMomentOfForce, STR("inch pound force"), STR("inch pound force"), STR("in•lbf"), 1.3558179483314 / 12.);
    AddNonSIUnitToLibrary(kSIQuantityMomentOfForce, STR("inch ounce force"), STR("inch ounce force"), STR("in•ozf"), 1.3558179483314 / 12. / 16.);

    AddNonSIUnitToLibrary(kSIQuantityMomentOfForce, STR("pound force foot"), STR("pound force feet"), STR("lbf•ft"), 1.3558179483314);
    AddNonSIUnitToLibrary(kSIQuantityMomentOfForce, STR("pound force inch"), STR("pound force inches"), STR("lbf•in"), 1.3558179483314 / 12.);
    AddNonSIUnitToLibrary(kSIQuantityMomentOfForce, STR("ounce force inch"), STR("ounce force inches"), STR("ozf•in"), 1.3558179483314 / 12. / 16.);

    // Power
    AddNonSIUnitToLibrary(kSIQuantityPower, STR("calorie per second"), STR("calories per second"), STR("cal/s"), 4.1868);
    AddNonSIUnitToLibrary(kSIQuantityPower, STR("calorie per minute"), STR("calories per minute"), STR("cal/min"), 4.1868 / 60.);
    AddNonSIUnitToLibrary(kSIQuantityPower, STR("calorie per hour"), STR("calories per hour"), STR("cal/h"), 4.1868 / 3600.);

    AddNonSIUnitToLibrary(kSIQuantityPower, STR("horsepower"), STR("horsepower"), STR("hp"), 745.699872);
    AddNonSIUnitToLibrary(kSIQuantityPower, STR("british thermal unit per hour"), STR("british thermal unit per hour"), STR("Btu/h"), 1055.05585257348 / 3600.);
    AddNonSIUnitToLibrary(kSIQuantityPower, STR("british thermal unit per minute"), STR("british thermal unit per minute"), STR("Btu/min"), 1055.05585257348 / 60);
    AddNonSIUnitToLibrary(kSIQuantityPower, STR("british thermal unit per second"), STR("british thermal unit per second"), STR("Btu/s"), 1055.05585257348);
    AddNonSIUnitToLibrary(kSIQuantityPower, STR("erg per second"), STR("ergs per second"), STR("erg/s"), 1e-7);

    AddNonSIUnitToLibrary(kSIQuantityPower, STR("foot pound force per hour"), STR("feet pound force per hour"), STR("ft•lbf/h"), (1609.344 / 5280) * 4.4482216152605 / 3600.);
    AddNonSIUnitToLibrary(kSIQuantityPower, STR("foot pound force per minute"), STR("feet pound force per minute"), STR("ft•lbf/min"), (1609.344 / 5280) * 4.4482216152605 / 60.);
    AddNonSIUnitToLibrary(kSIQuantityPower, STR("foot pound force per second"), STR("feet pound force per second"), STR("ft•lbf/s"), (1609.344 / 5280) * 4.4482216152605);

    AddNonSIUnitToLibrary(kSIQuantityPower, STR("inch pound force per hour"), STR("inches pound force per hour"), STR("in•lbf/h"), 1.3558179483314 / 12. / 3600.);
    AddNonSIUnitToLibrary(kSIQuantityPower, STR("inch pound force per minute"), STR("inches pound force per minute"), STR("in•lbf/min"), 1.3558179483314 / 12. / 60.);
    AddNonSIUnitToLibrary(kSIQuantityPower, STR("inch pound force per second"), STR("inches pound force per second"), STR("in•lbf/s"), 1.3558179483314 / 12.);

    // Force
    AddNonSIUnitToLibrary(kSIQuantityForce, STR("pound force"), STR("pounds force"), STR("lbf"), 4.4482216152605);
    AddNonSIUnitToLibrary(kSIQuantityForce, STR("ounce force"), STR("ounces force"), STR("ozf"), 4.4482216152605 / 16.);
    AddNonSIUnitToLibrary(kSIQuantityForce, STR("kilogram force"), STR("kilograms force"), STR("kgf"), 9.80665);

    // Volume / Distance
    AddNonSIUnitToLibrary(kSIQuantityVolumePerLength, STR("liter per 100 kilometers"), STR("liters per 100 kilometers"), STR("L/(100 km)"), 1e-3 / 100000.);

    // Volumetric Flow Rate
    AddNonSIUnitToLibrary(kSIQuantityVolumetricFlowRate, STR("cubic meter per hour"), STR("cubic meters per hour"), STR("m^3/h"), 1. / 3600.);
    AddNonSIUnitToLibrary(kSIQuantityVolumetricFlowRate, STR("cubic meter per minute"), STR("cubic meters per minute"), STR("m^3/min"), 1. / 60.);
    AddNonSIUnitToLibrary(kSIQuantityVolumetricFlowRate, STR("cubic meter per second"), STR("cubic meters per second"), STR("m^3/s"), 1.);
    AddNonSIUnitToLibrary(kSIQuantityVolumetricFlowRate, STR("cubic centimeter per hour"), STR("cubic centimeters per hour"), STR("cm^3/h"), 1e-6 / 3600.);
    AddNonSIUnitToLibrary(kSIQuantityVolumetricFlowRate, STR("cubic centimeter per minute"), STR("cubic centimeters per minute"), STR("cm^3/min"), 1e-6 / 60.);
    AddNonSIUnitToLibrary(kSIQuantityVolumetricFlowRate, STR("cubic centimeter per second"), STR("cubic centimeters per second"), STR("cm^3/s"), 1e-6);
    AddNonSIUnitToLibrary(kSIQuantityVolumetricFlowRate, STR("cubic foot per hour"), STR("cubic feet per hour"), STR("ft^3/h"), (1609.344 / 5280) * (1609.344 / 5280) * (1609.344 / 5280) / 3600.);
    AddNonSIUnitToLibrary(kSIQuantityVolumetricFlowRate, STR("cubic foot per minute"), STR("cubic feet per minute"), STR("ft^3/min"), (1609.344 / 5280) * (1609.344 / 5280) * (1609.344 / 5280) / 60.);
    AddNonSIUnitToLibrary(kSIQuantityVolumetricFlowRate, STR("cubic foot per second"), STR("cubic feet per second"), STR("ft^3/s"), (1609.344 / 5280) * (1609.344 / 5280) * (1609.344 / 5280));

    // Stefan-Boltzmann Constant
    AddNonSIUnitToLibrary(kSIQuantityPowerPerAreaPerTemperatureToFourthPower, STR("stefan-boltzmann constant"), STR("stefan-boltzmann constant"), STR("σ"), kSIStefanBoltzmannConstant);

    AddNonSIUnitToLibrary(kSIQuantityWavelengthDisplacementConstant, STR("wien wavelength displacement constant"), STR("wien wavelength displacement constant"), STR("b_λ"), kSIWeinDisplacementConstant);

    // Gas Permeance Unit
    AddNonSIUnitToLibrary(kSIQuantityGasPermeance, STR("gas permeance unit"), STR("gas permeance unit"), STR("GPU"), 0.33);

    imperialVolumes = true;
    SIUnitsLibrarySetImperialVolumes(false);

    if (currentlocale->currency_symbol)
    {
        if (strcmp(currentlocale->currency_symbol, "£") == 0)
        {
            imperialVolumes = false;
            SIUnitsLibrarySetImperialVolumes(true);
        }
    }
}

static SIUnitRef SIUnitWithParameters(SIDimensionalityRef dimensionality,
                                      SIPrefix length_num_prefix,
                                      SIPrefix length_den_prefix,
                                      SIPrefix mass_num_prefix,
                                      SIPrefix mass_den_prefix,
                                      SIPrefix time_num_prefix,
                                      SIPrefix time_den_prefix,
                                      SIPrefix current_num_prefix,
                                      SIPrefix current_den_prefix,
                                      SIPrefix temperature_num_prefix,
                                      SIPrefix temperature_den_prefix,
                                      SIPrefix amount_num_prefix,
                                      SIPrefix amount_den_prefix,
                                      SIPrefix luminous_intensity_num_prefix,
                                      SIPrefix luminous_intensity_den_prefix,
                                      OCStringRef root_name,
                                      OCStringRef root_plural_name,
                                      OCStringRef root_symbol,
                                      SIPrefix root_symbol_prefix,
                                      bool allows_si_prefix,
                                      bool is_special_si_symbol,
                                      double scale_to_coherent_si)
{
    SIUnitRef theUnit = SIUnitCreate(dimensionality,
                                     length_num_prefix,
                                     length_den_prefix,
                                     mass_num_prefix,
                                     mass_den_prefix,
                                     time_num_prefix,
                                     time_den_prefix,
                                     current_num_prefix,
                                     current_den_prefix,
                                     temperature_num_prefix,
                                     temperature_den_prefix,
                                     amount_num_prefix,
                                     amount_den_prefix,
                                     luminous_intensity_num_prefix,
                                     luminous_intensity_den_prefix,
                                     root_name,
                                     root_plural_name,
                                     root_symbol,
                                     root_symbol_prefix,
                                     allows_si_prefix,
                                     is_special_si_symbol,
                                     scale_to_coherent_si);

    if (NULL == theUnit)
        return NULL;

    if (NULL == unitsLibrary)
        UnitsLibraryCreate();
    if (OCDictionaryContainsKey(unitsLibrary, theUnit->symbol))
    {
        SIUnitRef existingUnit = OCDictionaryGetValue(unitsLibrary, theUnit->symbol);
        OCRelease(theUnit);
        return existingUnit;
    }
    OCTypeSetStaticInstance(theUnit, true);
    OCDictionaryAddValue(unitsLibrary, theUnit->symbol, theUnit);
    OCRelease(theUnit);
    return theUnit;
}

SIUnitRef SIUnitDimensionlessAndUnderived(void)
{
    return SIUnitWithParameters(SIDimensionalityDimensionless(),
                                kSIPrefixNone,
                                kSIPrefixNone,
                                kSIPrefixKilo,
                                kSIPrefixNone,
                                kSIPrefixNone,
                                kSIPrefixNone,
                                kSIPrefixNone,
                                kSIPrefixNone,
                                kSIPrefixNone,
                                kSIPrefixNone,
                                kSIPrefixNone,
                                kSIPrefixNone,
                                kSIPrefixNone,
                                kSIPrefixNone,
                                NULL,
                                NULL,
                                NULL,
                                kSIPrefixNone,
                                false,
                                false,
                                1.0);
}

#pragma mark Operations

double SIUnitScaleToCoherentSIUnit(SIUnitRef theUnit)
{
    /*
     *	This method calculates the scaling factor needed to transform a number with this unit
     *	into a number with the coherent si unit of the same dimensionality
     */

    IF_NO_OBJECT_EXISTS_RETURN(theUnit, 0)

    // If this is one of the 7 SI base unit - or -
    // if the symbol is NULL then this must be a derived SI Unit
    // Either way calculate scale that returns to coherent derived unit
    // using dimension exponents and prefixes.
    if (theUnit->root_symbol == NULL)
    {
        // This method calculates the scaling back to a coherent derived unit
        // based solely on prefix and exponent for each of the seven dimensions.
        double scaling = 1.0;
        for (int i = 0; i < 7; i++)
        {
            double numerator_power = SIDimensionalityGetNumExpAtIndex(theUnit->dimensionality, i);
            double denominator_power = SIDimensionalityGetDenExpAtIndex(theUnit->dimensionality, i);
            SIPrefix num_prefix = theUnit->num_prefix[i];
            SIPrefix den_prefix = theUnit->den_prefix[i];
            if (i == 1)
            {
                num_prefix -= kSIPrefixKilo; // Since kilogram is the base unit
                den_prefix -= kSIPrefixKilo; // Since kilogram is the base unit
            }
            scaling *= pow(10., (num_prefix * numerator_power - den_prefix * denominator_power));
        }
        return scaling;
    }

    // If symbol exists and it is is_special_si_symbol then return scale using symbol prefix
    // to return to base special SI symbol unit.
    if (theUnit->is_special_si_symbol)
        return pow(10., theUnit->root_symbol_prefix);

    // If symbol exists but is not is_special_si_symbol, then
    // scale_to_coherent_si is scale from base non-SI symbol to coherent SI base unit with same dimensionality
    // symbol prefix gives scale from prefixed non-SI unit to non-SI root unit.
    return theUnit->scale_to_coherent_si * pow(10., theUnit->root_symbol_prefix);
}

bool SIUnitAreEquivalentUnits(SIUnitRef theUnit1, SIUnitRef theUnit2)
{
    IF_NO_OBJECT_EXISTS_RETURN(theUnit1, false)
    IF_NO_OBJECT_EXISTS_RETURN(theUnit2, false)
    if (theUnit1 == theUnit2)
        return true;

    // If true, these two units can be substituted for each other without modifying
    // the quantity's numerical value.
    if (!SIDimensionalityEqual(theUnit1->dimensionality, theUnit2->dimensionality))
        return false;
    if (OCCompareDoubleValues(SIUnitScaleToCoherentSIUnit(theUnit1), SIUnitScaleToCoherentSIUnit(theUnit2)) != kOCCompareEqualTo)
        return false;
    return true;
}

bool SIUnitIsCoherentDerivedUnit(SIUnitRef theUnit)
{
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, false)

    // Non-SI units are not coherent SI units.
    if (theUnit->scale_to_coherent_si != 1.)
        return false;
    if (theUnit->is_special_si_symbol)
        return false;

    // Prefixed Special SI units are no coherent SI Units.
    if (theUnit->root_symbol_prefix)
        return false;

    // All the dimension prefixes must be kSIPrefixNone,
    // except mass which is kSIPrefixKilo
    if (theUnit->num_prefix[kSILengthIndex] != kSIPrefixNone)
        return false;
    if (theUnit->num_prefix[kSIMassIndex] != kSIPrefixKilo)
        return false;
    if (theUnit->num_prefix[kSITimeIndex] != kSIPrefixNone)
        return false;
    if (theUnit->num_prefix[kSICurrentIndex] != kSIPrefixNone)
        return false;
    if (theUnit->num_prefix[kSITemperatureIndex] != kSIPrefixNone)
        return false;
    if (theUnit->num_prefix[kSIAmountIndex] != kSIPrefixNone)
        return false;
    if (theUnit->num_prefix[kSILuminousIntensityIndex] != kSIPrefixNone)
        return false;
    if (theUnit->den_prefix[kSILengthIndex] != kSIPrefixNone)
        return false;
    if (theUnit->den_prefix[kSIMassIndex] != kSIPrefixNone)
        return false;
    if (theUnit->den_prefix[kSITimeIndex] != kSIPrefixNone)
        return false;
    if (theUnit->den_prefix[kSICurrentIndex] != kSIPrefixNone)
        return false;
    if (theUnit->den_prefix[kSITemperatureIndex] != kSIPrefixNone)
        return false;
    if (theUnit->den_prefix[kSIAmountIndex] != kSIPrefixNone)
        return false;
    if (theUnit->den_prefix[kSILuminousIntensityIndex] != kSIPrefixNone)
        return false;
    return true;
}

OCArrayRef SIUnitCreateArrayOfEquivalentUnits(SIUnitRef theUnit)
{
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, NULL)
    OCArrayRef candidates = SIUnitCreateArrayOfUnitsForDimensionality(SIUnitGetDimensionality(theUnit));
    if (candidates)
    {
        OCMutableArrayRef result = OCArrayCreateMutable(0, &kOCTypeArrayCallBacks);

        for (uint64_t index = 0; index < OCArrayGetCount(candidates); index++)
        {
            SIUnitRef unit = OCArrayGetValueAtIndex(candidates, index);
            if (SIUnitAreEquivalentUnits(unit, theUnit))
                OCArrayAppendValue(result, unit);
        }

        OCRelease(candidates);
        return result;
    }
    return NULL;
}

OCArrayRef SIUnitCreateArrayOfUnitsForQuantity(OCStringRef quantity)
{
    IF_NO_OBJECT_EXISTS_RETURN(quantity, NULL)
    if (NULL == unitsLibrary)
        UnitsLibraryCreate();
    if (OCDictionaryContainsKey(unitsQuantitiesLibrary, quantity))
    {
        OCMutableArrayRef array = (OCMutableArrayRef)OCDictionaryGetValue(unitsQuantitiesLibrary, quantity);
        return OCArrayCreateCopy(array);
    }
    return NULL;
}

OCArrayRef SIUnitCreateArrayOfUnitsForDimensionality(SIDimensionalityRef theDim)
{
    IF_NO_OBJECT_EXISTS_RETURN(theDim, NULL)
    if (NULL == unitsLibrary)
        UnitsLibraryCreate();
    OCStringRef symbol = SIDimensionalityGetSymbol(theDim);
    if (OCDictionaryContainsKey(unitsDimensionalitiesLibrary, symbol))
    {
        OCMutableArrayRef array = (OCMutableArrayRef)OCDictionaryGetValue(unitsDimensionalitiesLibrary, symbol);
        return OCArrayCreateCopy(array);
    }
    return NULL;
}

OCArrayRef SIUnitCreateArrayOfUnitsForSameReducedDimensionality(SIDimensionalityRef theDim)
{
    IF_NO_OBJECT_EXISTS_RETURN(theDim, NULL)
    if (NULL == unitsLibrary)
        UnitsLibraryCreate();
    OCArrayRef dimensionalities = SIDimensionalityCreateArrayWithSameReducedDimensionality(theDim);

    OCMutableArrayRef result = OCArrayCreateMutable(0, &kOCTypeArrayCallBacks);
    for (uint64_t index = 0; index < OCArrayGetCount(dimensionalities); index++)
    {
        SIDimensionalityRef dimensionality = OCArrayGetValueAtIndex(dimensionalities, index);
        OCStringRef symbol = SIDimensionalityGetSymbol(dimensionality);
        if (OCDictionaryContainsKey(unitsDimensionalitiesLibrary, symbol))
        {
            OCMutableArrayRef array = (OCMutableArrayRef)OCDictionaryGetValue(unitsDimensionalitiesLibrary, symbol);
            OCArrayAppendArray(result, array, OCRangeMake(0, OCArrayGetCount(array)));
        }
    }
    OCRelease(dimensionalities);
    return result;
}

double SIUnitConversion(SIUnitRef initialUnit, SIUnitRef finalUnit)
{
    IF_NO_OBJECT_EXISTS_RETURN(initialUnit, 0)
    IF_NO_OBJECT_EXISTS_RETURN(finalUnit, 0)

    if (SIDimensionalityHasSameReducedDimensionality(initialUnit->dimensionality, finalUnit->dimensionality))
        return SIUnitScaleToCoherentSIUnit(initialUnit) / SIUnitScaleToCoherentSIUnit(finalUnit);
    return 0;
}

OCStringRef SIUnitCreateName(SIUnitRef theUnit)
{
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, NULL)

    // Construct the name of the unit from root_name and prefix.
    if (SIUnitIsDimensionlessAndUnderived(theUnit))
        return kSIQuantityDimensionless;

    if (SIUnitIsSIBaseUnit(theUnit))
    {
        // The root_name attribute is empty for the seven base units, so we need to ask
        // baseUnitRootName for its root_name
        for (int i = 0; i < 7; i++)
        {
            // Only one numerator_exponent will be non-zero (and 1).
            if (SIDimensionalityGetNumExpAtIndex(theUnit->dimensionality, i))
            {
                return OCStringCreateWithFormat(STR("%@%@"), prefixNameForSIPrefix(theUnit->num_prefix[i]), baseUnitRootName(i));
            }
        }
    }
    else
    {
        if (theUnit->root_name == NULL)
            return NULL;
        OCStringRef rootName = SIUnitCopyRootName(theUnit);
        OCStringRef name = OCStringCreateWithFormat(STR("%@%@"), prefixNameForSIPrefix(theUnit->root_symbol_prefix), rootName);
        OCRelease(rootName);
        return name;
    }
    return NULL;
}

OCStringRef SIUnitCreatePluralName(SIUnitRef theUnit)
{
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, NULL)

    // Construct the plural name of the unit from root_plural_name and prefix.

    if (SIUnitIsSIBaseUnit(theUnit))
    {
        // The root_plural_name attribute is empty for the seven base units, so we need to ask
        // baseUnitPluralRootName for its root_plural_name
        for (int i = 0; i < 7; i++)
        {
            // Only one numerator_exponent will be non-zero (and 1).
            if (SIDimensionalityGetNumExpAtIndex(theUnit->dimensionality, i))
            {

                OCMutableStringRef name = OCStringCreateMutable(64);

                OCStringRef prefix_string = prefixNameForSIPrefix(theUnit->num_prefix[i]);
                OCStringRef name_string = baseUnitPluralRootName(i);

                OCStringAppend(name, prefix_string);
                OCStringAppend(name, name_string);
                return name;
            }
        }
    }
    else
    {
        if (theUnit->root_plural_name == NULL)
            return NULL;
        OCMutableStringRef name = OCStringCreateMutable(64);
        OCStringRef prefix_string = prefixNameForSIPrefix(theUnit->root_symbol_prefix);

        OCStringAppend(name, prefix_string);
        OCStringAppend(name, theUnit->root_plural_name);

        return name;
    }
    return NULL;
}

static OCComparisonResult unitSort(const void *val1, const void *val2, void *context)
{
    SIUnitRef unit1 = (SIUnitRef)val1;
    SIUnitRef unit2 = (SIUnitRef)val2;
    double scale1 = SIUnitScaleToCoherentSIUnit(unit1);
    double scale2 = SIUnitScaleToCoherentSIUnit(unit2);
    if (scale1 < scale2)
        return kOCCompareLessThan;
    else if (scale1 > scale2)
        return kOCCompareGreaterThan;
    else
    {
        OCStringRef symbol1 = SIUnitCopySymbol((SIUnitRef)val1);
        OCStringRef symbol2 = SIUnitCopySymbol((SIUnitRef)val2);
        OCComparisonResult result = OCStringCompare(symbol1, symbol2, kOCCompareCaseInsensitive);
        OCRelease(symbol1);
        OCRelease(symbol2);
        return result;
    }
}

static OCComparisonResult unitNameSort(const void *val1, const void *val2, void *context)
{
    OCStringRef name1 = SIUnitCopyRootName((SIUnitRef)val1);
    OCStringRef name2 = SIUnitCopyRootName((SIUnitRef)val2);
    OCComparisonResult result = OCStringCompare(name1, name2, kOCCompareCaseInsensitive);
    OCRelease(name1);
    OCRelease(name2);
    return result;
}

static OCComparisonResult unitNameLengthSort(const void *val1, const void *val2, void *context)
{
    OCStringRef name1 = SIUnitCreateName((SIUnitRef)val1);
    OCStringRef name2 = SIUnitCreateName((SIUnitRef)val2);
    if (name1 == NULL && name2 == NULL)
        return kOCCompareEqualTo;
    if (name1 == NULL)
    {
        OCRelease(name2);
        return kOCCompareGreaterThan;
    }
    if (name2 == NULL)
    {
        OCRelease(name1);
        return kOCCompareLessThan;
    }

    OCComparisonResult result = kOCCompareEqualTo;
    if (OCStringGetLength(name1) > OCStringGetLength(name2))
        result = kOCCompareLessThan;
    if (OCStringGetLength(name1) < OCStringGetLength(name2))
        result = kOCCompareGreaterThan;
    OCRelease(name1);
    OCRelease(name2);
    return result;
}

OCArrayRef SIUnitCreateArrayOfConversionUnits(SIUnitRef theUnit)
{
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, NULL)

    OCArrayRef result = SIUnitCreateArrayOfUnitsForSameReducedDimensionality(SIUnitGetDimensionality(theUnit));

    OCMutableArrayRef sorted = OCArrayCreateMutableCopy(result);
    OCArraySortValues(sorted, OCRangeMake(0, OCArrayGetCount(result)), unitSort, NULL);
    OCRelease(result);
    return sorted;
}

static SIUnitRef SIUnitFindEquivalentDerivedSIUnit(SIUnitRef input)
{
    IF_NO_OBJECT_EXISTS_RETURN(input, NULL)

    if (input->root_symbol == NULL)
        return input;

    SIDimensionalityRef theDim = SIUnitGetDimensionality(input);
    OCArrayRef candidates = SIUnitCreateArrayOfUnitsForDimensionality(theDim);
    if (candidates)
    {
        int64_t closest = -1;
        double bestScaling = 100;
        for (int64_t index = 0; index < OCArrayGetCount(candidates); index++)
        {
            SIUnitRef unit = OCArrayGetValueAtIndex(candidates, index);
            if (SIUnitIsCoherentDerivedUnit(unit))
            { // was unit->root_symbol==NULL
                double scaling = fabs(log10(SIUnitScaleToCoherentSIUnit(unit) / SIUnitScaleToCoherentSIUnit(input)));
                if (fabs(log(SIUnitScaleToCoherentSIUnit(unit) / SIUnitScaleToCoherentSIUnit(input))) < bestScaling)
                {
                    bestScaling = scaling;
                    closest = index;
                }
            }
        }
        if (closest == -1)
            return input;
        SIUnitRef result = OCArrayGetValueAtIndex(candidates, closest);
        OCRelease(candidates);
        return result;
    }
    return input;
}

SIUnitRef SIUnitFindEquivalentUnitWithShortestSymbol(SIUnitRef theUnit)
{
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, NULL);
    if (SIUnitIsDimensionlessAndUnderived(theUnit))
        return theUnit;

    OCArrayRef candidates = SIUnitCreateArrayOfEquivalentUnits(theUnit);
    if (!candidates)
        return theUnit;

    if (OCArrayGetCount(candidates) == 0) {
        OCRelease(candidates);
        return theUnit;
    }

    SIUnitRef best = theUnit;
    OCStringRef symbol     = SIUnitCopySymbol(theUnit);
    OCStringRef rootSymbol = SIUnitCopyRootSymbol(theUnit);

    if (rootSymbol) {
        // clean up everything before returning
        OCRelease(symbol);
        OCRelease(rootSymbol);
        OCRelease(candidates);
        return theUnit;
    }

    // no rootSymbol → use symbol’s length as baseline
    int64_t length = OCStringGetLength(symbol);
    OCRelease(symbol);

    for (uint64_t i = 0, cnt = OCArrayGetCount(candidates); i < cnt; i++) {
        SIUnitRef    candidate          = OCArrayGetValueAtIndex(candidates, i);
        OCStringRef  candidateSymbol    = SIUnitCopySymbol(candidate);
        OCStringRef  candidateRootSym   = SIUnitCopyRootSymbol(candidate);

        if (candidateRootSym) {
            best = candidate;
        }
        else if (OCStringGetLength(candidateSymbol) < length) {
            best   = candidate;
            length = OCStringGetLength(candidateSymbol);
        }

        OCRelease(candidateSymbol);
        OCRelease(candidateRootSym);
    }

    OCRelease(candidates);
    return best;
}

SIUnitRef SIUnitByReducing(SIUnitRef theUnit, double *unit_multiplier)
{
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, NULL)

    SIDimensionalityRef dimensionality = SIDimensionalityByReducing(theUnit->dimensionality);
    if (SIDimensionalityEqual(dimensionality, theUnit->dimensionality))
        return SIUnitFindEquivalentUnitWithShortestSymbol(theUnit);

    SIUnitRef reducedUnit = SIUnitWithParameters(dimensionality,
                                                 theUnit->num_prefix[kSILengthIndex],
                                                 theUnit->den_prefix[kSILengthIndex],
                                                 theUnit->num_prefix[kSIMassIndex],
                                                 theUnit->den_prefix[kSIMassIndex],
                                                 theUnit->num_prefix[kSITimeIndex],
                                                 theUnit->den_prefix[kSITimeIndex],
                                                 theUnit->num_prefix[kSICurrentIndex],
                                                 theUnit->den_prefix[kSICurrentIndex],
                                                 theUnit->num_prefix[kSITemperatureIndex],
                                                 theUnit->den_prefix[kSITemperatureIndex],
                                                 theUnit->num_prefix[kSIAmountIndex],
                                                 theUnit->den_prefix[kSIAmountIndex],
                                                 theUnit->num_prefix[kSILuminousIntensityIndex],
                                                 theUnit->den_prefix[kSILuminousIntensityIndex],
                                                 NULL,
                                                 NULL,
                                                 NULL,
                                                 kSIPrefixNone,
                                                 false,
                                                 false,
                                                 1.0);
    if (unit_multiplier)
    {
        if (*unit_multiplier == 0.0)
            *unit_multiplier = 1.0;
        *unit_multiplier *= SIUnitScaleToCoherentSIUnit(theUnit) / SIUnitScaleToCoherentSIUnit(reducedUnit);
    }
    return SIUnitFindEquivalentUnitWithShortestSymbol(reducedUnit);
}

SIUnitRef SIUnitByMultiplyingWithoutReducing(SIUnitRef theUnit1, SIUnitRef theUnit2, double *unit_multiplier, OCStringRef *error)
{
    if (error)
        if (*error)
            return NULL;
    IF_NO_OBJECT_EXISTS_RETURN(theUnit1, NULL)
    IF_NO_OBJECT_EXISTS_RETURN(theUnit2, NULL)
    if (theUnit1 == theUnit2)
        return SIUnitByRaisingToAPowerWithoutReducing(theUnit1, 2, unit_multiplier, error);

    SIUnitRef dimensionlessAndUnderivedUnit = SIUnitDimensionlessAndUnderived();
    if (theUnit1 == dimensionlessAndUnderivedUnit)
    {
        return theUnit2;
    }
    if (theUnit2 == dimensionlessAndUnderivedUnit)
    {
        return theUnit1;
    }

    /*
     *	This routine will create an derived SI Unit formed by the product of two units.
     *	It will additionally return a multiplier for the numerical part of the quantity product
     *
     */

    SIUnitRef theUnit11 = SIUnitFindEquivalentDerivedSIUnit(theUnit1);
    SIUnitRef theUnit22 = SIUnitFindEquivalentDerivedSIUnit(theUnit2);

    SIDimensionalityRef dimensionality = SIDimensionalityByMultiplyingWithoutReducing(theUnit11->dimensionality, theUnit22->dimensionality, error);

    SIPrefix num_prefix[7];
    SIPrefix den_prefix[7];

    for (uint8_t i = 0; i < 7; i++)
    {
        uint8_t numerator_exponent = SIDimensionalityGetNumExpAtIndex(dimensionality, i);
        uint8_t input1_numerator_exponent = SIDimensionalityGetNumExpAtIndex(theUnit11->dimensionality, i);
        uint8_t input2_numerator_exponent = SIDimensionalityGetNumExpAtIndex(theUnit22->dimensionality, i);
        num_prefix[i] = theUnit11->num_prefix[i] * input1_numerator_exponent + theUnit22->num_prefix[i] * input2_numerator_exponent;
        if (numerator_exponent)
            num_prefix[i] /= numerator_exponent;
        else
        {
            num_prefix[i] = kSIPrefixNone;
            if (i == kSIMassIndex)
                num_prefix[i] = kSIPrefixKilo;
        }

        if (!isValidSIPrefix(num_prefix[i]))
            num_prefix[i] = findClosestPrefix(num_prefix[i]);

        uint8_t denominator_exponent = SIDimensionalityGetDenExpAtIndex(dimensionality, i);
        uint8_t input1_denominator_exponent = SIDimensionalityGetDenExpAtIndex(theUnit11->dimensionality, i);
        uint8_t input2_denominator_exponent = SIDimensionalityGetDenExpAtIndex(theUnit22->dimensionality, i);
        den_prefix[i] = theUnit11->den_prefix[i] * input1_denominator_exponent + theUnit22->den_prefix[i] * input2_denominator_exponent;
        if (denominator_exponent)
            den_prefix[i] /= denominator_exponent;
        else
            den_prefix[i] = kSIPrefixNone;

        if (!isValidSIPrefix(den_prefix[i]))
            den_prefix[i] = findClosestPrefix(den_prefix[i]);
    }

    SIUnitRef theUnit = SIUnitWithParameters(dimensionality,
                                             num_prefix[kSILengthIndex],
                                             den_prefix[kSILengthIndex],
                                             num_prefix[kSIMassIndex],
                                             den_prefix[kSIMassIndex],
                                             num_prefix[kSITimeIndex],
                                             den_prefix[kSITimeIndex],
                                             num_prefix[kSICurrentIndex],
                                             den_prefix[kSICurrentIndex],
                                             num_prefix[kSITemperatureIndex],
                                             den_prefix[kSITemperatureIndex],
                                             num_prefix[kSIAmountIndex],
                                             den_prefix[kSIAmountIndex],
                                             num_prefix[kSILuminousIntensityIndex],
                                             den_prefix[kSILuminousIntensityIndex],
                                             NULL, NULL, NULL, 0, false, false, 1.0);

    /*
     *	Calculate the multiplier for the numerical part of the new quantity.
     */

    if (unit_multiplier)
    {
        if (*unit_multiplier == 0.0)
            *unit_multiplier = 1.0;
        double unit1Scale = SIUnitScaleToCoherentSIUnit(theUnit1);
        double unit2Scale = SIUnitScaleToCoherentSIUnit(theUnit2);
        double unitScale = SIUnitScaleToCoherentSIUnit(theUnit);

        *unit_multiplier *= unit1Scale * unit2Scale / unitScale;
    }
    return theUnit;
}

SIUnitRef SIUnitByMultiplying(SIUnitRef theUnit1, SIUnitRef theUnit2, double *unit_multiplier, OCStringRef *error)
{
    if (error)
        if (*error)
            return NULL;
    SIUnitRef unit = SIUnitByMultiplyingWithoutReducing(theUnit1, theUnit2, unit_multiplier, error);
    return SIUnitByReducing(unit, unit_multiplier);
}

SIUnitRef SIUnitByDividingWithoutReducing(SIUnitRef theUnit1, SIUnitRef theUnit2, double *unit_multiplier)
{
    /*
     *	This routine will create an derived SI Unit formed by dividing two units.
     *	It will additionally return a multiplier for the numerical part of the quantity product
     *
     */

    IF_NO_OBJECT_EXISTS_RETURN(theUnit1, NULL)
    IF_NO_OBJECT_EXISTS_RETURN(theUnit2, NULL)

    SIUnitRef theUnit11 = SIUnitFindEquivalentDerivedSIUnit(theUnit1);
    SIUnitRef theUnit22 = SIUnitFindEquivalentDerivedSIUnit(theUnit2);

    SIDimensionalityRef dimensionality = SIDimensionalityByDividingWithoutReducing(theUnit1->dimensionality, theUnit2->dimensionality);

    SIPrefix num_prefix[7];
    SIPrefix den_prefix[7];

    for (uint8_t i = 0; i < 7; i++)
    {
        uint8_t numerator_exponent = SIDimensionalityGetNumExpAtIndex(dimensionality, i);
        uint8_t input1_numerator_exponent = SIDimensionalityGetNumExpAtIndex(theUnit11->dimensionality, i);
        uint8_t input2_denominator_exponent = SIDimensionalityGetDenExpAtIndex(theUnit22->dimensionality, i);
        num_prefix[i] = theUnit11->num_prefix[i] * input1_numerator_exponent + theUnit22->den_prefix[i] * input2_denominator_exponent;
        if (numerator_exponent)
            num_prefix[i] /= numerator_exponent;
        else
        {
            num_prefix[i] = kSIPrefixNone;
            if (i == kSIMassIndex)
                num_prefix[i] = kSIPrefixKilo;
        }

        if (!isValidSIPrefix(num_prefix[i]))
            num_prefix[i] = findClosestPrefix(num_prefix[i]);

        uint8_t denominator_exponent = SIDimensionalityGetDenExpAtIndex(dimensionality, i);
        uint8_t input1_denominator_exponent = SIDimensionalityGetDenExpAtIndex(theUnit1->dimensionality, i);
        uint8_t input2_numerator_exponent = SIDimensionalityGetNumExpAtIndex(theUnit2->dimensionality, i);
        den_prefix[i] = theUnit11->den_prefix[i] * input1_denominator_exponent + theUnit22->num_prefix[i] * input2_numerator_exponent;
        if (denominator_exponent)
            den_prefix[i] /= denominator_exponent;
        else
            den_prefix[i] = kSIPrefixNone;

        if (!isValidSIPrefix(den_prefix[i]))
            den_prefix[i] = findClosestPrefix(den_prefix[i]);
    }

    SIUnitRef theUnit = SIUnitWithParameters(dimensionality,
                                             num_prefix[kSILengthIndex],
                                             den_prefix[kSILengthIndex],
                                             num_prefix[kSIMassIndex],
                                             den_prefix[kSIMassIndex],
                                             num_prefix[kSITimeIndex],
                                             den_prefix[kSITimeIndex],
                                             num_prefix[kSICurrentIndex],
                                             den_prefix[kSICurrentIndex],
                                             num_prefix[kSITemperatureIndex],
                                             den_prefix[kSITemperatureIndex],
                                             num_prefix[kSIAmountIndex],
                                             den_prefix[kSIAmountIndex],
                                             num_prefix[kSILuminousIntensityIndex],
                                             den_prefix[kSILuminousIntensityIndex],
                                             NULL, NULL, NULL, 0, false, false, 1.0);

    /*
     *	Calculate the multiplier for the numerical part of the new quantity.
     */

    if (unit_multiplier)
    {
        if (*unit_multiplier == 0.0)
            *unit_multiplier = 1.0;
        double unit1Scale = SIUnitScaleToCoherentSIUnit(theUnit1);
        double unit2Scale = SIUnitScaleToCoherentSIUnit(theUnit2);
        double unitScale = SIUnitScaleToCoherentSIUnit(theUnit);
        *unit_multiplier *= unit1Scale / unit2Scale / unitScale;
    }
    return theUnit;
}

SIUnitRef SIUnitByDividing(SIUnitRef theUnit1, SIUnitRef theUnit2, double *unit_multiplier)
{
    SIUnitRef unit = SIUnitByDividingWithoutReducing(theUnit1, theUnit2, unit_multiplier);
    return SIUnitByReducing(unit, unit_multiplier);
}

SIUnitRef SIUnitByTakingNthRoot(SIUnitRef input, uint8_t root, double *unit_multiplier, OCStringRef *error)
{
    if (error)
        if (*error)
            return NULL;

    IF_NO_OBJECT_EXISTS_RETURN(input, NULL)
    SIUnitRef derivedUnit = SIUnitFindEquivalentDerivedSIUnit(input);
    SIDimensionalityRef dimensionality = SIDimensionalityByTakingNthRoot(derivedUnit->dimensionality, root, error);
    if (error)
        if (*error)
            return NULL;
    if (SIDimensionalityEqual(dimensionality, derivedUnit->dimensionality))
        return input;

    SIUnitRef theUnit = SIUnitWithParameters(dimensionality,
                                             derivedUnit->num_prefix[kSILengthIndex],
                                             derivedUnit->den_prefix[kSILengthIndex],
                                             derivedUnit->num_prefix[kSIMassIndex],
                                             derivedUnit->den_prefix[kSIMassIndex],
                                             derivedUnit->num_prefix[kSITimeIndex],
                                             derivedUnit->den_prefix[kSITimeIndex],
                                             derivedUnit->num_prefix[kSICurrentIndex],
                                             derivedUnit->den_prefix[kSICurrentIndex],
                                             derivedUnit->num_prefix[kSITemperatureIndex],
                                             derivedUnit->den_prefix[kSITemperatureIndex],
                                             derivedUnit->num_prefix[kSIAmountIndex],
                                             derivedUnit->den_prefix[kSIAmountIndex],
                                             derivedUnit->num_prefix[kSILuminousIntensityIndex],
                                             derivedUnit->den_prefix[kSILuminousIntensityIndex],
                                             NULL,
                                             NULL,
                                             NULL,
                                             kSIPrefixNone,
                                             false, false,
                                             1.0);
    /*
     *	Calculate the multiplier for the numerical part of the new quantity.
     */

    if (unit_multiplier)
    {
        if (*unit_multiplier == 0.0)
            *unit_multiplier = 1.0;
        *unit_multiplier *= pow(SIUnitScaleToCoherentSIUnit(input), 1. / root) / SIUnitScaleToCoherentSIUnit(theUnit);
    }
    return theUnit;
}

SIUnitRef SIUnitByRaisingToAPowerWithoutReducing(SIUnitRef input, double power, double *unit_multiplier, OCStringRef *error)
{
    if (error)
        if (*error)
            return NULL;
    IF_NO_OBJECT_EXISTS_RETURN(input, NULL)
    SIUnitRef derivedUnit = SIUnitFindEquivalentDerivedSIUnit(input);

    SIDimensionalityRef dimensionality = SIDimensionalityByRaisingToAPowerWithoutReducing(derivedUnit->dimensionality, power, error);
    if (error)
        if (*error)
            return NULL;
    SIUnitRef theUnit;
    if (power > 0)
        theUnit = SIUnitWithParameters(dimensionality,
                                       derivedUnit->num_prefix[kSILengthIndex],
                                       derivedUnit->den_prefix[kSILengthIndex],
                                       derivedUnit->num_prefix[kSIMassIndex],
                                       derivedUnit->den_prefix[kSIMassIndex],
                                       derivedUnit->num_prefix[kSITimeIndex],
                                       derivedUnit->den_prefix[kSITimeIndex],
                                       derivedUnit->num_prefix[kSICurrentIndex],
                                       derivedUnit->den_prefix[kSICurrentIndex],
                                       derivedUnit->num_prefix[kSITemperatureIndex],
                                       derivedUnit->den_prefix[kSITemperatureIndex],
                                       derivedUnit->num_prefix[kSIAmountIndex],
                                       derivedUnit->den_prefix[kSIAmountIndex],
                                       derivedUnit->num_prefix[kSILuminousIntensityIndex],
                                       derivedUnit->den_prefix[kSILuminousIntensityIndex],
                                       NULL, NULL, NULL, 0, false, false, 1.0);
    else
        theUnit = SIUnitWithParameters(dimensionality,
                                       derivedUnit->den_prefix[kSILengthIndex],
                                       derivedUnit->num_prefix[kSILengthIndex],
                                       derivedUnit->den_prefix[kSIMassIndex],
                                       derivedUnit->num_prefix[kSIMassIndex],
                                       derivedUnit->den_prefix[kSITimeIndex],
                                       derivedUnit->num_prefix[kSITimeIndex],
                                       derivedUnit->den_prefix[kSICurrentIndex],
                                       derivedUnit->num_prefix[kSICurrentIndex],
                                       derivedUnit->den_prefix[kSITemperatureIndex],
                                       derivedUnit->num_prefix[kSITemperatureIndex],
                                       derivedUnit->den_prefix[kSIAmountIndex],
                                       derivedUnit->num_prefix[kSIAmountIndex],
                                       derivedUnit->den_prefix[kSILuminousIntensityIndex],
                                       derivedUnit->num_prefix[kSILuminousIntensityIndex],
                                       NULL, NULL, NULL, 0, false, false, 1.0);

    /*
     *	Calculate the multiplier for the numerical part of the new quantity.
     */

    if (unit_multiplier)
    {
        if (*unit_multiplier == 0.0)
            *unit_multiplier = 1.0;
        *unit_multiplier *= pow(SIUnitScaleToCoherentSIUnit(input), power) / SIUnitScaleToCoherentSIUnit(theUnit);
    }
    return theUnit;
}

SIUnitRef SIUnitByRaisingToAPower(SIUnitRef input, double power, double *unit_multiplier, OCStringRef *error)
{
    if (error)
        if (*error)
            return NULL;
    SIUnitRef unit = SIUnitByRaisingToAPowerWithoutReducing(input, power, unit_multiplier, error);
    return SIUnitByReducing(unit, unit_multiplier);
}

void SIUnitShow(SIUnitRef theUnit)
{
    IF_NO_OBJECT_EXISTS_RETURN(theUnit, )
    OCStringShow(theUnit->symbol);
    fprintf(stdout, "\n");
    return;
}

OCMutableDictionaryRef SIUnitGetLibrary()
{
    if (NULL == unitsLibrary)
        UnitsLibraryCreate();
    return unitsLibrary;
}

SIUnitRef SIUnitForSymbol(OCStringRef symbol, double *unit_multiplier, OCStringRef *error)
{
    if (error)
        if (*error)
            return NULL;

    if (NULL == unitsLibrary)
        UnitsLibraryCreate();
    if (NULL == symbol)
    {
        if (error)
        {
            *error = STR("Unknown unit symbol");
        }
        return NULL;
    }
    IF_NO_OBJECT_EXISTS_RETURN(unitsLibrary, NULL)

    if (OCStringCompare(symbol, STR(" "), 0) == kOCCompareEqualTo)
        return SIUnitDimensionlessAndUnderived();

    OCMutableStringRef mutSymbol = OCStringCreateMutableCopy(symbol);
    OCStringTrimWhitespace(mutSymbol);
    OCStringFindAndReplace2(mutSymbol, STR("*"), STR("•"));
    SIUnitRef unit = OCDictionaryGetValue(unitsLibrary, mutSymbol);
    OCRelease(mutSymbol);

    if (unit)
        return unit;

    //    OCRange range = OCRangeMake(0, OCStringGetLength(symbol));
    //    OCRange resultRange;
    //    CFCharacterSetRef theSet = CFCharacterSetCreateWithCharactersInString (kCFAllocatorDefault,STR("*/^"));
    //    bool couldBeDerived = OCStringFindCharacterFromSet(symbol,theSet,range,kCFCompareBackwards,&resultRange);
    //    OCRelease(theSet);
    //    if(couldBeDerived) return SIUnitForParsedSymbol(symbol, unit_multiplier, error);
    //    return NULL;

    return SIUnitForParsedSymbol(symbol, unit_multiplier, error);
}

SIUnitRef SIUnitFindCoherentSIUnitWithDimensionality(SIDimensionalityRef theDim)
{
    IF_NO_OBJECT_EXISTS_RETURN(theDim, NULL)

    return SIUnitWithParameters(theDim,
                                kSIPrefixNone,
                                kSIPrefixNone,
                                kSIPrefixKilo,
                                kSIPrefixKilo,
                                kSIPrefixNone,
                                kSIPrefixNone,
                                kSIPrefixNone,
                                kSIPrefixNone,
                                kSIPrefixNone,
                                kSIPrefixNone,
                                kSIPrefixNone,
                                kSIPrefixNone,
                                kSIPrefixNone,
                                kSIPrefixNone,
                                NULL,
                                NULL,
                                NULL,
                                kSIPrefixNone,
                                false,
                                false,
                                1.0);
}

SIUnitRef SIUnitFindWithName(OCStringRef input)
{
    if (NULL == unitsLibrary)
        UnitsLibraryCreate();
    IF_NO_OBJECT_EXISTS_RETURN(unitsLibrary, NULL)

    int64_t count = OCDictionaryGetCount(unitsLibrary);
    OCStringRef keys[count];
    SIUnitRef units[count];
    OCDictionaryGetKeysAndValues(unitsLibrary, (const void **)keys, (const void **)units);

    for (int64_t index = 0; index < count; index++)
    {
        OCStringRef name = SIUnitCreateName(units[index]);
        if (name)
            if (OCStringCompare(name, input, 0) == kOCCompareEqualTo)
            {
                OCRelease(name);
                SIUnitRef theUnit = units[index];
                return theUnit;
            }
        if (name)
            OCRelease(name);
        name = SIUnitCreatePluralName(units[index]);
        if (name)
            if (OCStringCompare(name, input, 0) == kOCCompareEqualTo)
            {
                OCRelease(name);
                SIUnitRef theUnit = units[index];
                return theUnit;
            }
        if (name)
            OCRelease(name);
    }
    return NULL;
}

OCArrayRef SIUnitGetUnitsSortedByNameLength(void)
{
    if (NULL != unitsNamesLibrary)
        return unitsNamesLibrary;
    if (NULL == unitsLibrary)
        UnitsLibraryCreate();
    OCArrayRef units = OCDictionaryCreateArrayWithAllValues(unitsLibrary);
    unitsNamesLibrary = OCArrayCreateMutableCopy(units);
    OCRelease(units);
    OCArraySortValues(unitsNamesLibrary, OCRangeMake(0, OCArrayGetCount(unitsNamesLibrary)), unitNameLengthSort, NULL);
    return unitsNamesLibrary;
}

bool SIUnitIsDimensionless(SIUnitRef theUnit)
{
    if (!SIDimensionalityIsDimensionless(theUnit->dimensionality))
        return false;
    return true;
}

#pragma mark Tests

bool SIUnitEqual(SIUnitRef theUnit1, SIUnitRef theUnit2)
{
    // if true, then Units are equal in every way
    IF_NO_OBJECT_EXISTS_RETURN(theUnit1, false)
    IF_NO_OBJECT_EXISTS_RETURN(theUnit2, false)

    if (theUnit1 == theUnit2)
        return true;

    if (!SIUnitAreEquivalentUnits(theUnit1, theUnit2))
        return false;

    if (theUnit1->root_name == NULL && theUnit2->root_name != NULL)
        return false;
    if (theUnit1->root_name != NULL && theUnit2->root_name == NULL)
        return false;
    if (theUnit1->root_name != NULL && theUnit2->root_name != NULL)
    {
        if (OCStringCompare(theUnit1->root_name, theUnit2->root_name, 0) != kOCCompareEqualTo)
            return false;
    }

    if (theUnit1->root_plural_name == NULL && theUnit2->root_plural_name != NULL)
        return false;
    if (theUnit1->root_plural_name != NULL && theUnit2->root_plural_name == NULL)
        return false;
    if (theUnit1->root_plural_name != NULL && theUnit2->root_plural_name != NULL)
    {
        if (OCStringCompare(theUnit1->root_plural_name, theUnit2->root_plural_name, 0) != kOCCompareEqualTo)
            return false;
    }

    if (theUnit1->root_symbol == NULL && theUnit2->root_symbol != NULL)
        return false;
    if (theUnit1->root_symbol != NULL && theUnit2->root_symbol == NULL)
        return false;
    if (theUnit1->root_symbol != NULL && theUnit2->root_symbol != NULL)
    {
        if (OCStringCompare(theUnit1->root_symbol, theUnit2->root_symbol, 0) != kOCCompareEqualTo)
            return false;
    }

    if (theUnit1->root_symbol_prefix != theUnit2->root_symbol_prefix)
        return false;
    if (theUnit1->is_special_si_symbol != theUnit2->is_special_si_symbol)
        return false;
    if (theUnit1->scale_to_coherent_si != theUnit2->scale_to_coherent_si)
        return false;

    OCStringRef symbol1 = SIUnitCreateDerivedSymbol(theUnit1);
    OCStringRef symbol2 = SIUnitCreateDerivedSymbol(theUnit2);
    if (OCStringCompare(symbol1, symbol2, 0) != kOCCompareEqualTo)
    {
        OCRelease(symbol1);
        OCRelease(symbol2);
        return false;
    }
    OCRelease(symbol1);
    OCRelease(symbol2);
    return true;
}
