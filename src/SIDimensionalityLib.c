#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "SIDimensionalityPrivate.h"
#include "SILibrary.h"
// Local comparator for OCStringRef values (case-insensitive sort)
static int OCStringSort(const void *val1, const void *val2, void *context) {
    (void)context;
    OCStringRef str1 = *(const OCStringRef *)val1;
    OCStringRef str2 = *(const OCStringRef *)val2;
    return OCStringCompare(str1, str2, kOCCompareCaseInsensitive);
}
#pragma mark Library
// dimLibrary is a Singleton
OCMutableDictionaryRef dimLibrary = NULL;
OCMutableDictionaryRef dimQuantitiesLibrary = NULL;
static void DimensionalityLibraryBuild();
/// Interns a freshly created dimensionality, returning an existing one if already present.
static SIDimensionalityRef InternDim(SIDimensionalityRef dim) {
    if (!dim) return NULL;
    if (!dimLibrary) DimensionalityLibraryBuild();
    // Lookup by symbol
    SIDimensionalityRef existing = (SIDimensionalityRef)OCDictionaryGetValue(dimLibrary, dim->symbol);
    if (existing) {
        OCRelease(dim);
        return existing;
    }
    // Not found → insert as a static interned object
    OCTypeSetStaticInstance(dim, true);
    OCDictionaryAddValue(dimLibrary, dim->symbol, dim);
    OCRelease(dim);
    return dim;
}
SIDimensionalityRef SIDimensionalityFromDictionary(OCDictionaryRef dict, OCStringRef *error) {
    if (!dict) {
        if (error) *error = STR("SIDimensionalityFromDictionary: missing dictionary");
        return NULL;
    }
    // grab the two arrays
    OCIndexArrayRef numArr = (OCIndexArrayRef)OCDictionaryGetValue(dict, STR("numerators"));
    OCIndexArrayRef denArr = (OCIndexArrayRef)OCDictionaryGetValue(dict, STR("denominators"));
    if (!numArr || !denArr ||
        OCIndexArrayGetCount(numArr) != BASE_DIMENSION_COUNT ||
        OCIndexArrayGetCount(denArr) != BASE_DIMENSION_COUNT) {
        if (error) *error = STR("SIDimensionalityFromDictionary: invalid exponent arrays");
        return NULL;
    }
    // extract into locals
    uint8_t n[BASE_DIMENSION_COUNT], d[BASE_DIMENSION_COUNT];
    for (uint8_t i = 0; i < BASE_DIMENSION_COUNT; ++i) {
        OCIndex ni = OCIndexArrayGetValueAtIndex(numArr, i);
        OCIndex di = OCIndexArrayGetValueAtIndex(denArr, i);
        if (ni > 255 || di > 255) {
            if (error) *error = STR("SIDimensionalityFromDictionary: exponent out of range");
            return NULL;
        }
        n[i] = (uint8_t)ni;
        d[i] = (uint8_t)di;
    }
    // re‐create (and intern) the SIDimensionality
    return SIDimensionalityWithExponents(
        n[kSILengthIndex], d[kSILengthIndex],
        n[kSIMassIndex], d[kSIMassIndex],
        n[kSITimeIndex], d[kSITimeIndex],
        n[kSICurrentIndex], d[kSICurrentIndex],
        n[kSITemperatureIndex], d[kSITemperatureIndex],
        n[kSIAmountIndex], d[kSIAmountIndex],
        n[kSILuminousIntensityIndex], d[kSILuminousIntensityIndex]);
}
static SIDimensionalityRef AddDimensionalityToLibrary(uint8_t length_num_exp, uint8_t length_den_exp,
                                                      uint8_t mass_num_exp, uint8_t mass_den_exp,
                                                      uint8_t time_num_exp, uint8_t time_den_exp,
                                                      uint8_t current_num_exp, uint8_t current_den_exp,
                                                      uint8_t temperature_num_exp, uint8_t temperature_den_exp,
                                                      uint8_t amount_num_exp, uint8_t amount_den_exp,
                                                      uint8_t luminous_intensity_num_exp, uint8_t luminous_intensity_den_exp) {
    uint8_t num_exp[BASE_DIMENSION_COUNT] = {length_num_exp, mass_num_exp, time_num_exp, current_num_exp, temperature_num_exp, amount_num_exp, luminous_intensity_num_exp};
    uint8_t den_exp[BASE_DIMENSION_COUNT] = {length_den_exp, mass_den_exp, time_den_exp, current_den_exp, temperature_den_exp, amount_den_exp, luminous_intensity_den_exp};
    SIDimensionalityRef dim = SIDimensionalityCreate(num_exp, den_exp);
    // Set dim to a static instance.
    // This sets the reference count to 1 and prevents it from being released or retained
    // by OCRelease an OCRetain.   This ensures that only one instance of the dimensionality
    // exists in the library.
    if (!OCDictionaryContainsKey(dimLibrary, dim->symbol)) {
        OCTypeSetStaticInstance(dim, true);
        OCDictionaryAddValue(dimLibrary, dim->symbol, dim);
        OCRelease(dim);
        return dim;
    }
    SIDimensionalityRef existingDim = OCDictionaryGetValue(dimLibrary, dim->symbol);
    OCRelease(dim);
    return existingDim;
}
SIDimensionalityRef SIDimensionalityForSymbol(OCStringRef symbol, OCStringRef *error) {
    if (symbol == NULL)
        return NULL;
    if (OCDictionaryContainsKey(dimLibrary, symbol)) {
        return (SIDimensionalityRef)OCDictionaryGetValue(dimLibrary, symbol);
    }
    return SIDimensionalityParseExpression(symbol, error);
}
SIDimensionalityRef SIDimensionalityForQuantity(OCStringRef quantity, OCStringRef *error) {
    if (quantity == NULL)
        return NULL;
    if (NULL == dimQuantitiesLibrary)
        DimensionalityLibraryBuild();
    SIDimensionalityRef dimensionality = NULL;
    if (OCDictionaryContainsKey(dimQuantitiesLibrary, quantity)) {
        dimensionality = OCDictionaryGetValue(dimQuantitiesLibrary, quantity);
        return dimensionality;
    }
    OCMutableStringRef lowerCaseQuantity = OCStringCreateMutableCopy(quantity);
    OCStringLowercase(lowerCaseQuantity);
    if (OCDictionaryContainsKey(dimQuantitiesLibrary, lowerCaseQuantity)) {
        dimensionality = OCDictionaryGetValue(dimQuantitiesLibrary, lowerCaseQuantity);
    }
    OCRelease(lowerCaseQuantity);
    if (dimensionality == NULL) {
        dimensionality = SIDimensionalityParseExpression(quantity, error);
    }
    return dimensionality;
}
SIDimensionalityRef SIDimensionalityByReducing(SIDimensionalityRef theDim) {
    IF_NO_OBJECT_EXISTS_RETURN(theDim, NULL);
    uint8_t num_exp[BASE_DIMENSION_COUNT];
    uint8_t den_exp[BASE_DIMENSION_COUNT];
    for (size_t i = 0; i < BASE_DIMENSION_COUNT; ++i) {
        int8_t diff = (int8_t)theDim->num_exp[i] - (int8_t)theDim->den_exp[i];
        num_exp[i] = (diff > 0) ? diff : 0;
        den_exp[i] = (diff < 0) ? -diff : 0;
    }
    // Delegate to the array‐based creator + interner
    return SIDimensionalityWithExponentArrays(num_exp, den_exp);
}
SIDimensionalityRef SIDimensionalityByTakingNthRoot(SIDimensionalityRef theDim,
                                                    uint8_t root,
                                                    OCStringRef *error) {
    // Propagate any existing error
    if (error && *error) return NULL;
    IF_NO_OBJECT_EXISTS_RETURN(theDim, NULL);
    // First reduce so we only track net exponents
    SIDimensionalityRef reduced = SIDimensionalityByReducing(theDim);
    uint8_t num_exp[BASE_DIMENSION_COUNT];
    uint8_t den_exp[BASE_DIMENSION_COUNT];
    for (size_t i = 0; i < BASE_DIMENSION_COUNT; ++i) {
        int8_t nr = reduced->num_exp[i];
        int8_t dr = reduced->den_exp[i];
        // Must be divisible by root
        if ((nr % root) != 0 || (dr % root) != 0) {
            if (error) {
                *error = STR("Can't raise physical dimensionality to a non-integer power.");
            }
            return NULL;
        }
        // Divide the original exponents
        num_exp[i] = theDim->num_exp[i] / root;
        den_exp[i] = theDim->den_exp[i] / root;
    }
    // Delegate to array-based creator + intern
    return SIDimensionalityWithExponentArrays(num_exp, den_exp);
}
SIDimensionalityRef SIDimensionalityByDividingWithoutReducing(SIDimensionalityRef theDim1,
                                                              SIDimensionalityRef theDim2) {
    IF_NO_OBJECT_EXISTS_RETURN(theDim1, NULL);
    IF_NO_OBJECT_EXISTS_RETURN(theDim2, NULL);
    uint8_t num_exp[BASE_DIMENSION_COUNT];
    uint8_t den_exp[BASE_DIMENSION_COUNT];
    for (size_t i = 0; i < BASE_DIMENSION_COUNT; ++i) {
        num_exp[i] = theDim1->num_exp[i] + theDim2->den_exp[i];
        den_exp[i] = theDim1->den_exp[i] + theDim2->num_exp[i];
    }
    // SIDimensionalityWithExponentArrays handles both creation and interning
    return SIDimensionalityWithExponentArrays(num_exp, den_exp);
}
SIDimensionalityRef SIDimensionalityByDividing(SIDimensionalityRef theDim1, SIDimensionalityRef theDim2) {
    return SIDimensionalityByReducing(SIDimensionalityByDividingWithoutReducing(theDim1, theDim2));
}
SIDimensionalityRef SIDimensionalityByRaisingToPowerWithoutReducing(SIDimensionalityRef theDim,
                                                                    int power,
                                                                    OCStringRef *error) {
    // 1) Propagate any preexisting error
    if (error && *error) return NULL;
    // 2) Validate input
    IF_NO_OBJECT_EXISTS_RETURN(theDim, NULL);
    // 3) Compute new exponents in two small arrays
    uint8_t num_exp[BASE_DIMENSION_COUNT];
    uint8_t den_exp[BASE_DIMENSION_COUNT];
    for (size_t i = 0; i < BASE_DIMENSION_COUNT; ++i) {
        if (power > 0) {
            num_exp[i] = theDim->num_exp[i] * (uint8_t)power;
            den_exp[i] = theDim->den_exp[i] * (uint8_t)power;
        } else {
            // negative power → swap roles
            uint8_t magnitude = (uint8_t)(-power);
            num_exp[i] = theDim->den_exp[i] * magnitude;
            den_exp[i] = theDim->num_exp[i] * magnitude;
        }
    }
    // 6) Delegate to the array-based constructor + intern
    return SIDimensionalityWithExponentArrays(num_exp, den_exp);
}
SIDimensionalityRef SIDimensionalityByRaisingToPower(SIDimensionalityRef theDim, int power, OCStringRef *error) {
    if (error)
        if (*error) return NULL;
    return SIDimensionalityByReducing(SIDimensionalityByRaisingToPowerWithoutReducing(theDim, power, error));
}
SIDimensionalityRef SIDimensionalityByMultiplyingWithoutReducing(SIDimensionalityRef theDim1,
                                                                 SIDimensionalityRef theDim2,
                                                                 OCStringRef *error) {
    // Propagate existing error
    if (error && *error) return NULL;
    IF_NO_OBJECT_EXISTS_RETURN(theDim1, NULL);
    IF_NO_OBJECT_EXISTS_RETURN(theDim2, NULL);
    // If both refer to the same dimensionality, square it
    if (theDim1 == theDim2)
        return SIDimensionalityByRaisingToPowerWithoutReducing(theDim1, 2, error);
    // Build numerator and denominator exponent arrays
    uint8_t num_exp[BASE_DIMENSION_COUNT];
    uint8_t den_exp[BASE_DIMENSION_COUNT];
    for (size_t i = 0; i < BASE_DIMENSION_COUNT; ++i) {
        num_exp[i] = theDim1->num_exp[i] + theDim2->num_exp[i];
        den_exp[i] = theDim1->den_exp[i] + theDim2->den_exp[i];
    }
    // Delegate to the array-based creator + interner
    return SIDimensionalityWithExponentArrays(num_exp, den_exp);
}
SIDimensionalityRef SIDimensionalityByMultiplying(SIDimensionalityRef theDim1, SIDimensionalityRef theDim2, OCStringRef *error) {
    if (error)
        if (*error) return NULL;
    return SIDimensionalityByReducing(SIDimensionalityByMultiplyingWithoutReducing(theDim1, theDim2, error));
}
OCArrayRef SIDimensionalityCreateArrayOfQuantities(SIDimensionalityRef theDim) {
    IF_NO_OBJECT_EXISTS_RETURN(theDim, NULL);
    if (NULL == dimQuantitiesLibrary) DimensionalityLibraryBuild();
    uint64_t count = OCDictionaryGetCountOfValue(dimQuantitiesLibrary, theDim);
    if (0 == count) return NULL;
    OCStringRef quantities[count];
    uint64_t totalCount = OCDictionaryGetCount(dimQuantitiesLibrary);
    OCStringRef keys[totalCount];
    SIDimensionalityRef dimens[totalCount];
    OCDictionaryGetKeysAndValues(dimQuantitiesLibrary, (const void **)keys, (const void **)dimens);
    uint64_t i = 0;
    for (uint64_t index = 0; index < totalCount; index++) {
        if (SIDimensionalityEqual(dimens[index], theDim)) {
            quantities[i++] = keys[index];
        }
    }
    OCArrayRef result = NULL;
    if (i == count) result = OCArrayCreate((const void **)quantities, count, &kOCTypeArrayCallBacks);
    return result;
}
// Helper: recursively build all dimensionalities with the same reduced exponents
static void collectSameReduced(
    const SIDimensionalityRef theDim,
    const int8_t reducedExp[BASE_DIMENSION_COUNT],
    const int8_t largestExp[BASE_DIMENSION_COUNT],
    size_t idx,
    uint8_t num_exp[BASE_DIMENSION_COUNT],
    uint8_t den_exp[BASE_DIMENSION_COUNT],
    OCMutableArrayRef result) {
    if (idx == BASE_DIMENSION_COUNT) {
        // Create-and-intern
        SIDimensionalityRef dim = SIDimensionalityWithExponentArrays(num_exp, den_exp);
        OCArrayAppendValue(result, dim);
        return;
    }
    int8_t r = reducedExp[idx];
    int8_t l = largestExp[idx];
    for (int8_t i = l; i >= abs(r); --i) {
        if (r >= 0) {
            num_exp[idx] = i;
            den_exp[idx] = i - r;
        } else {
            num_exp[idx] = i - (-r);
            den_exp[idx] = i;
        }
        collectSameReduced(theDim, reducedExp, largestExp, idx + 1, num_exp, den_exp, result);
    }
}
OCArrayRef
SIDimensionalityCreateArrayWithSameReducedDimensionality(SIDimensionalityRef theDim) {
    IF_NO_OBJECT_EXISTS_RETURN(theDim, NULL);
    if (!dimQuantitiesLibrary) DimensionalityLibraryBuild();
    // Compute reduced and max exponents
    int8_t reducedExp[BASE_DIMENSION_COUNT];
    int8_t largestExp[BASE_DIMENSION_COUNT];
    for (size_t i = 0; i < BASE_DIMENSION_COUNT; ++i) {
        int8_t diff = (int8_t)theDim->num_exp[i] - (int8_t)theDim->den_exp[i];
        reducedExp[i] = diff;
        largestExp[i] = (diff >= 0) ? theDim->num_exp[i] : theDim->den_exp[i];
    }
    // Prepare result container
    OCMutableArrayRef result = OCArrayCreateMutable(0, &kOCTypeArrayCallBacks);
    if (!result) return NULL;
    // Temporary exponent arrays
    uint8_t num_exp[BASE_DIMENSION_COUNT];
    uint8_t den_exp[BASE_DIMENSION_COUNT];
    // Recursively fill result
    collectSameReduced(theDim, reducedExp, largestExp, 0, num_exp, den_exp, result);
    return result;
}
OCArrayRef SIDimensionalityCreateArrayOfQuantitiesWithSameReducedDimensionality(SIDimensionalityRef theDim) {
    OCArrayRef reducedDims = SIDimensionalityCreateArrayWithSameReducedDimensionality(theDim);
    OCMutableArrayRef result = OCArrayCreateMutable(0, &kOCTypeArrayCallBacks);
    for (uint64_t dimIndex = 0; dimIndex < OCArrayGetCount(reducedDims); dimIndex++) {
        SIDimensionalityRef dim = OCArrayGetValueAtIndex(reducedDims, dimIndex);
        OCArrayRef quantities = SIDimensionalityCreateArrayOfQuantities(dim);
        if (quantities) {
            OCArrayAppendArray(result, quantities, OCRangeMake(0, OCArrayGetCount(quantities)));
            OCRelease(quantities);
        }
    }
    OCRelease(reducedDims);
    return result;
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
OCArrayRef SIDimensionalityCreateArrayOfQuantityNames(SIDimensionalityRef dim) {
    if (!dim) return NULL;
    // Lazy initialization of the dimensionality-to-quantity map
    if (!dimQuantitiesLibrary) {
        DimensionalityLibraryBuild();
    }
    OCIndex totalCount = OCDictionaryGetCount(dimQuantitiesLibrary);
    if (totalCount == 0) return NULL;
    // Retrieve all keys and values from the dictionary
    OCStringRef keys[totalCount];
    SIDimensionalityRef values[totalCount];
    OCDictionaryGetKeysAndValues(dimQuantitiesLibrary,
                                 (const void **)keys,
                                 (const void **)values);
    // Collect quantity names associated with the given dimensionality
    OCStringRef matchingNames[totalCount];
    OCIndex matchCount = 0;
    for (OCIndex i = 0; i < totalCount; ++i) {
        if (SIDimensionalityEqual(values[i], dim)) {
            matchingNames[matchCount++] = keys[i];
        }
    }
    if (matchCount == 0) return NULL;
    // Build mutable result array
    OCMutableArrayRef result = OCArrayCreateMutable(matchCount, &kOCTypeArrayCallBacks);
    if (!result) return NULL;
    for (OCIndex i = 0; i < matchCount; ++i) {
        OCArrayAppendValue(result, matchingNames[i]);
    }
    // Sort alphabetically (case-insensitive)
    OCArraySortValues(result,
                      OCRangeMake(0, matchCount),
                      OCStringSort,
                      NULL);
    return result;
}
OCArrayRef SIDimensionalityCreateArrayOfQuantityNamesWithSameReducedDimensionality(SIDimensionalityRef dim) {
    if (!dim) return NULL;
    OCArrayRef equivalents = SIDimensionalityCreateArrayWithSameReducedDimensionality(dim);
    if (!equivalents) return NULL;
    OCMutableArrayRef result = OCArrayCreateMutable(0, &kOCTypeArrayCallBacks);
    if (!result) {
        OCRelease(equivalents);
        return NULL;
    }
    for (OCIndex i = 0; i < OCArrayGetCount(equivalents); ++i) {
        SIDimensionalityRef equivDim = OCArrayGetValueAtIndex(equivalents, i);
        OCArrayRef quantityNames = SIDimensionalityCreateArrayOfQuantityNames(equivDim);
        if (quantityNames && OCArrayGetCount(quantityNames) > 0) {
            OCArrayAppendArray(result, quantityNames, OCRangeMake(0, OCArrayGetCount(quantityNames)));
            OCRelease(quantityNames);
        } else {
            OCStringRef symbol = SIDimensionalityGetSymbol(equivDim);
            if (symbol) {
                OCMutableStringRef fallback = OCStringCreateMutable(0);
                OCStringAppendCString(fallback, "Dimensionality: ");
                OCStringAppend(fallback, symbol);
                OCArrayAppendValue(result, fallback);
                OCRelease(fallback);
            }
        }
    }
    OCRelease(equivalents);
    return result;
}
OCMutableDictionaryRef SIDimensionalityGetLibrary() {
    if (NULL == dimLibrary)
        DimensionalityLibraryBuild();
    return dimLibrary;
}
void SIDimensionalitySetLibrary(OCMutableDictionaryRef newDimLibrary) {
    if (newDimLibrary == dimLibrary)
        return;
    if (newDimLibrary) {
        if (dimLibrary)
            OCRelease(dimLibrary);
        dimLibrary = (OCMutableDictionaryRef)OCRetain(newDimLibrary);
    }
}
#pragma mark Operations
SIDimensionalityRef SIDimensionalityWithExponentArrays(const uint8_t *num_exp, const uint8_t *den_exp) {
    if (!num_exp || !den_exp) return NULL;
    // Create the raw object
    SIDimensionalityRef dim = SIDimensionalityCreate(num_exp, den_exp);
    if (!dim) return NULL;
    // Intern (or reuse) and return
    return InternDim(dim);
}
static SIDimensionalityRef SIDimensionalityWithExponents(uint8_t length_num_exp, uint8_t length_den_exp,
                                                         uint8_t mass_num_exp, uint8_t mass_den_exp,
                                                         uint8_t time_num_exp, uint8_t time_den_exp,
                                                         uint8_t current_num_exp, uint8_t current_den_exp,
                                                         uint8_t temperature_num_exp, uint8_t temperature_den_exp,
                                                         uint8_t amount_num_exp, uint8_t amount_den_exp,
                                                         uint8_t luminous_num_exp, uint8_t luminous_den_exp) {
    const uint8_t num_exp[BASE_DIMENSION_COUNT] = {
        length_num_exp, mass_num_exp, time_num_exp, current_num_exp,
        temperature_num_exp, amount_num_exp, luminous_num_exp};
    const uint8_t den_exp[BASE_DIMENSION_COUNT] = {
        length_den_exp, mass_den_exp, time_den_exp, current_den_exp,
        temperature_den_exp, amount_den_exp, luminous_den_exp};
    return SIDimensionalityWithExponentArrays(num_exp, den_exp);
}
SIDimensionalityRef SIDimensionalityDimensionless() {
    return SIDimensionalityWithExponents(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}
SIDimensionalityRef SIDimensionalityForBaseDimensionIndex(SIBaseDimensionIndex index) {
    switch (index) {
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
/*
 @function SIDimensionalityCopyDimensionalityLibrary
 @abstract Gets a copy of the dimLibrary of dimensionalities
 @result a CFSet containing the dimensionalities.
 */
static OCDictionaryRef SIDimensionalityCopyDimensionalityLibrary(void) {
    if (NULL == dimLibrary)
        DimensionalityLibraryBuild();
    return OCDictionaryCreateCopy(dimLibrary);
}
// Add a cleanup function for static dictionaries
void cleanupDimensionalityLibraries(void) {
    if (!dimLibrary) return;
    // 1) First tear down dimQuantitiesLibrary if you haven’t already
    if (dimQuantitiesLibrary) {
        OCRelease(dimQuantitiesLibrary);
        dimQuantitiesLibrary = NULL;
    }
    // 2) Grab all of the *keys* (the symbols) out of dimLibrary
    OCArrayRef keys = OCDictionaryCreateArrayWithAllKeys(dimLibrary);
    if (keys) {
        size_t count = OCArrayGetCount(keys);
        for (size_t i = 0; i < count; ++i) {
            OCStringRef key = (OCStringRef)OCArrayGetValueAtIndex(keys, i);
            SIDimensionalityRef dim = (SIDimensionalityRef)OCDictionaryGetValue(dimLibrary, key);
            OCDictionaryRemoveValue(dimLibrary, key);
            OCTypeSetStaticInstance(dim, false);
            OCRelease(dim);
        }
        OCRelease(keys);
    }
    // 3) Finally release the dictionary itself
    OCRelease(dimLibrary);
    dimLibrary = NULL;
}
void DimensionalityLibraryBuild() {
    dimLibrary = OCDictionaryCreateMutable(0);
    dimQuantitiesLibrary = OCDictionaryCreateMutable(0);
    SIDimensionalityRef dim;
#pragma mark kSIQuantityDimensionless
    dim = AddDimensionalityToLibrary(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityDimensionless, dim);
#pragma mark kSIQuantityLength
    // Length
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
#pragma mark kSIQuantityMass
    // Mass
    dim = AddDimensionalityToLibrary(0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityMass, dim);
    // Inverse Mass
    dim = AddDimensionalityToLibrary(0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityInverseMass, dim);
    // Mass Ratio
    dim = AddDimensionalityToLibrary(0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityMassRatio, dim);
#pragma mark kSIQuantityTime
    // Time
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
#pragma mark kSIQuantityCurrent
    // Current
    dim = AddDimensionalityToLibrary(0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityCurrent, dim);
    // Inverse Current
    dim = AddDimensionalityToLibrary(0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityInverseCurrent, dim);
    // Current Ratio
    dim = AddDimensionalityToLibrary(0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityCurrentRatio, dim);
#pragma mark kSIQuantityTemperature
    // Temperature
    dim = AddDimensionalityToLibrary(0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityTemperature, dim);
    // Inverse Temperature
    dim = AddDimensionalityToLibrary(0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityInverseTemperature, dim);
    // Temperature Ratio
    dim = AddDimensionalityToLibrary(0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityTemperatureRatio, dim);
#pragma mark kSIQuantityAmount
    // Amount
    dim = AddDimensionalityToLibrary(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityAmount, dim);
    // inverse amount
    dim = AddDimensionalityToLibrary(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityInverseAmount, dim);
    // amount ratio
    dim = AddDimensionalityToLibrary(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityAmountRatio, dim);
#pragma mark kSIQuantityLuminousIntensity
    // Luminous Intensity
    dim = AddDimensionalityToLibrary(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityLuminousIntensity, dim);
    dim = AddDimensionalityToLibrary(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityInverseLuminousIntensity, dim);
    dim = AddDimensionalityToLibrary(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityLuminousIntensityRatio, dim);
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
    // Temperature Gradient
    dim = AddDimensionalityToLibrary(0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityTemperatureGradient, dim);
    // Coherent Units with no Unit name - Table 2
    // Speed, Velocity
    dim = AddDimensionalityToLibrary(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantitySpeed, dim);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityVelocity, dim);
    // Linear Momentum
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
    // Surface Tension
    dim = AddDimensionalityToLibrary(0, 0, 1, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantitySurfaceTension, dim);
    // Surface Energy
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
    // Thermal Conductance
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
    //  Electric Flux
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
    //  Magnetizability
    dim = AddDimensionalityToLibrary(2, 0, 1, 2, 4, 2, 2, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityMagnetizability, dim);
    // Permittivity
    dim = AddDimensionalityToLibrary(0, 3, 0, 1, 4, 0, 2, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityPermittivity, dim);
    //  Permeability
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
    // Pressure Gradient
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
    // Moment of Force
    dim = AddDimensionalityToLibrary(2, 0, 2, 1, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityMomentOfForce, dim);
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
    //  Electric Dipole Moment
    dim = AddDimensionalityToLibrary(1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityElectricDipoleMoment, dim);
    //  Gyromagnetic Ratio
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
    //  Magnetic Dipole Moment
    dim = AddDimensionalityToLibrary(2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityMagneticDipoleMoment, dim);
    //  Magnetic Dipole Moment Ratio
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
    // Mass to Charge Ratio
    dim = AddDimensionalityToLibrary(0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityMassToChargeRatio, dim);
    // Magnetic Field Gradient
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
    // amount concentration
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
    // Heat Transfer Coefficient
    dim = AddDimensionalityToLibrary(0, 0, 1, 0, 0, 3, 0, 0, 0, 1, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityHeatTransferCoefficient, dim);
    // Stefan Boltzman constant dimensionality
    dim = AddDimensionalityToLibrary(2, 2, 1, 0, 0, 3, 0, 0, 0, 4, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityPowerPerAreaPerTemperatureToFourthPower, dim);
    // kSIQuantityGasPermeance
    dim = AddDimensionalityToLibrary(0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityGasPermeance, dim);
    // kSIQuantityFirstHyperPolarizability
    dim = AddDimensionalityToLibrary(3, 4, 0, 2, 7, 0, 3, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityFirstHyperPolarizability, dim);
    // kSIQuantitySecondHyperPolarizability
    dim = AddDimensionalityToLibrary(4, 6, 0, 3, 10, 0, 4, 0, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantitySecondHyperPolarizability, dim);
    // Second Radiation Constant
    dim = AddDimensionalityToLibrary(3, 2, 1, 1, 2, 2, 0, 0, 1, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantitySecondRadiationConstant, dim);
    // Wien Wavelength Displacement Constant
    dim = AddDimensionalityToLibrary(1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityWavelengthDisplacementConstant, dim);
    // Fine Structure Constant
    dim = AddDimensionalityToLibrary(5, 5, 1, 1, 4, 4, 2, 2, 0, 0, 0, 0, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityFineStructureConstant, dim);
    // 1/(N•T)  kSIQuantityRatePerAmountConcentrationPerTime
    dim = AddDimensionalityToLibrary(3, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0);
    OCDictionaryAddValue(dimQuantitiesLibrary, kSIQuantityRatePerAmountConcentrationPerTime, dim);
}
bool DimensionalityLibraryDumpCSV(const char *csvPath) {
    FILE *fp = fopen(csvPath, "w");
    if (!fp) {
        perror("fopen");
        return false;
    }
    // 1) write header
    fprintf(fp,
            "Quantity,"
            "Length,Mass,Time,Current,Temperature,Amount,LuminousIntensity\n");
    // 2) grab all keys
    OCArrayRef keys = OCDictionaryCreateArrayWithAllKeys(dimQuantitiesLibrary);
    if (!keys) {
        fprintf(stderr, "ERROR: could not enumerate keys\n");
        fclose(fp);
        return false;
    }
    uint64_t count = OCArrayGetCount(keys);
    for (uint64_t i = 0; i < count; i++) {
        OCStringRef key = (OCStringRef)OCArrayGetValueAtIndex(keys, i);
        const void *vdim = OCDictionaryGetValue(dimQuantitiesLibrary, key);
        if (!vdim) continue;

        // quantity name – use OCStringGetCString, not the non‐existent GetCharactersPtr
        const char *quantity = OCStringGetCString(key);

        // cast to our internal struct
        struct impl_SIDimensionality *impl =
            (struct impl_SIDimensionality *)vdim;

        // 3) write one CSV row
        fprintf(fp,
                "\"%s\","         // quantity (quoted in case the string contains commas)
                "{%u,%u},"    // Length
                "{%u,%u},"    // Mass
                "{%u,%u},"    // Time
                "{%u,%u},"    // Current
                "{%u,%u},"    // Temperature
                "{%u,%u},"    // Amount
                "{%u,%u}\n",  // LuminousIntensity
                quantity,
                impl->num_exp[kSILengthIndex],         impl->den_exp[kSILengthIndex],
                impl->num_exp[kSIMassIndex],           impl->den_exp[kSIMassIndex],
                impl->num_exp[kSITimeIndex],           impl->den_exp[kSITimeIndex],
                impl->num_exp[kSICurrentIndex],        impl->den_exp[kSICurrentIndex],
                impl->num_exp[kSITemperatureIndex],    impl->den_exp[kSITemperatureIndex],
                impl->num_exp[kSIAmountIndex],         impl->den_exp[kSIAmountIndex],
                impl->num_exp[kSILuminousIntensityIndex], impl->den_exp[kSILuminousIntensityIndex]);
    }
    OCRelease(keys);
    fclose(fp);
    return true;
}