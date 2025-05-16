#include "../src/SILibrary.h" // Updated include path to resolve missing header issue
#include <stdio.h>

#ifndef PRINTERROR
#define PRINTERROR printf("failure: line %d, %s\n", __LINE__, __FUNCTION__)
#endif

bool unitTest0(void) {
    fprintf(stderr, "%s begin...\n", __func__);
    OCStringRef errorString = NULL;

    SIUnitRef unit = SIUnitForParsedSymbol(STR("m•kg^2•s^3•A^4•K^5•mol^6•cd^7/(m^2•kg^3•s^4•A^5•K^6•mol^7•cd^8)"), NULL, &errorString);
    OCStringRef plist = SIUnitCopySymbol(unit);
    SIUnitRef unit2 = SIUnitForParsedSymbol(plist, NULL, &errorString);
    if (!SIUnitEqual(unit, unit2)) PRINTERROR;

    fprintf(stderr, "%s end...without problems\n", __func__);
    return true;
}

bool unitTest1(void) {
    OCStringRef errorString = NULL;
    fprintf(stderr, "%s begin...\n", __func__);

    SIDimensionalityRef dimensionality = SIDimensionalityForSymbol(STR("M/(L*T^2)"), &errorString);
    if (dimensionality == NULL) PRINTERROR;

    SIUnitRef unit = SIUnitForUnderivedSymbol(STR("bar"));
    if (unit == NULL) PRINTERROR;

    fprintf(stderr, "%s end...without problems\n", __func__);
    return true;
}

bool unitTest3(void) {
    fprintf(stderr, "%s begin...\n", __func__);

    SIUnitRef unit;
    OCStringRef root_name;

    unit = SIUnitForUnderivedSymbol(STR("N"));
    if (unit == NULL) PRINTERROR;

    unit = SIUnitForUnderivedSymbol(STR("m"));
    if (unit == NULL) PRINTERROR;
    root_name = SIUnitCopyRootSymbol(unit);
    if (OCStringCompare(root_name, STR("m"), 0) != 0) PRINTERROR;

    fprintf(stderr, "%s end...without problems\n", __func__);
    return true;
}

bool unitTest4(void) {
    fprintf(stderr, "%s begin...\n", __func__);
    OCStringRef errorString = NULL;
    double multiplier = 0.0;

    // Test SIUnitGetNumeratorPrefixAtIndex and denominator for "km"
    SIUnitRef unit = SIUnitForParsedSymbol(STR("km"), &multiplier, &errorString);
    if (!unit) PRINTERROR;
    if (multiplier != 1.0) PRINTERROR;
    if (SIUnitGetNumeratorPrefixAtIndex(unit, kSILengthIndex) != kSIPrefixKilo) PRINTERROR;
    if (SIUnitGetDenominatorPrefixAtIndex(unit, kSILengthIndex) != kSIPrefixNone) PRINTERROR;

    // Test SIUnitCopyRootName
    OCStringRef rootName = SIUnitCopyRootName(unit);
    if (OCStringCompare(rootName, STR("meter"), 0) != 0) PRINTERROR;
    OCRelease(rootName);
    OCRelease(unit);

    fprintf(stderr, "%s end...without problems\n", __func__);
    return true;
}

bool unitTest5(void) {
    fprintf(stderr, "%s begin...\n", __func__);
    // Test root plural name and prefix allowance for gram
    SIUnitRef unit = SIUnitForUnderivedSymbol(STR("g"));
    if (!unit) PRINTERROR;
    OCStringRef rootPlural = SIUnitCopyRootPluralName(unit);
    if (OCStringCompare(rootPlural, STR("grams"), 0) != 0) PRINTERROR;
    OCRelease(rootPlural);
    if (!SIUnitAllowsSIPrefix(unit)) PRINTERROR;
    OCRelease(unit);
    fprintf(stderr, "%s end...without problems\n", __func__);
    return true;
}

bool unitTest6(void) {
    fprintf(stderr, "%s begin...\n", __func__);
    OCStringRef errorString = NULL;
    double multiplier = 0.0;
    // Round-trip parsing and conversion for Newton
    SIUnitRef unit = SIUnitForUnderivedSymbol(STR("N"));
    if (!unit) PRINTERROR;
    OCStringRef symbol = SIUnitCopySymbol(unit);
    SIUnitRef unit2 = SIUnitForParsedSymbol(symbol, &multiplier, &errorString);
    if (!unit2) PRINTERROR;
    if (!SIUnitEqual(unit, unit2)) PRINTERROR;
    OCRelease(symbol);
    OCRelease(unit);
    OCRelease(unit2);
    fprintf(stderr, "%s end...without problems\n", __func__);
    return true;
}
