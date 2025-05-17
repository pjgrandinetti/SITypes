#include "../src/SILibrary.h" // Updated include path to resolve missing header issue
#include <assert.h>
#include <stdio.h>

void test_unit_0(void) {
    printf("Running %s...\n", __func__);
    OCStringRef errorString = NULL;

    SIUnitRef unit = SIUnitForParsedSymbol(STR("m•kg^2•s^3•A^4•K^5•mol^6•cd^7/(m^2•kg^3•s^4•A^5•K^6•mol^7•cd^8)"), NULL, &errorString);
    OCStringRef plist = SIUnitCopySymbol(unit);
    SIUnitRef unit2 = SIUnitForParsedSymbol(plist, NULL, &errorString);
    assert(SIUnitEqual(unit, unit2));
    printf("%s passed\n", __func__);
}

void test_unit_1(void) {
    printf("Running %s...\n", __func__);
    OCStringRef errorString = NULL;

    SIDimensionalityRef dimensionality = SIDimensionalityForSymbol(STR("M/(L*T^2)"), &errorString);
    assert(dimensionality != NULL);

    SIUnitRef unit = SIUnitForUnderivedSymbol(STR("bar"));
    assert(unit != NULL);
    printf("%s passed\n", __func__);
}

void test_unit_3(void) {
    printf("Running %s...\n", __func__);
    SIUnitRef unit;
    OCStringRef root_name;

    unit = SIUnitForUnderivedSymbol(STR("N"));
    assert(unit != NULL);

    unit = SIUnitForUnderivedSymbol(STR("m"));
    assert(unit != NULL);
    root_name = SIUnitCopyRootSymbol(unit);
    assert(OCStringCompare(root_name, STR("m"), 0) == kOCCompareEqualTo);
    printf("%s passed\n", __func__);
}

void test_unit_4(void) {
    printf("Running %s...\n", __func__);
    OCStringRef errorString = NULL;
    double multiplier = 1.0; // needs to be initialized to 1.0

    SIUnitRef unit = SIUnitForParsedSymbol(STR("km"), &multiplier, &errorString);
    assert(unit != NULL);
    assert(multiplier == 1.0);
    assert(SIUnitGetNumeratorPrefixAtIndex(unit, kSILengthIndex) == kSIPrefixKilo);
    assert(SIUnitGetDenominatorPrefixAtIndex(unit, kSILengthIndex) == kSIPrefixNone);

    OCStringRef rootName = SIUnitCopyRootName(unit);
    assert(OCStringCompare(rootName, STR("meter"), 0) == kOCCompareEqualTo);
    printf("%s passed\n", __func__);
}

void test_unit_5(void) {
    printf("Running %s...\n", __func__);
    SIUnitRef unit = SIUnitForUnderivedSymbol(STR("g"));
    assert(unit != NULL);
    OCStringRef rootPlural = SIUnitCopyRootPluralName(unit);
    assert(OCStringCompare(rootPlural, STR("grams"), 0) == kOCCompareEqualTo);
    assert(SIUnitAllowsSIPrefix(unit));
    printf("%s passed\n", __func__);
}

void test_unit_6(void) {
    printf("Running %s...\n", __func__);
    OCStringRef errorString = NULL;
    double multiplier = 0.0;
    SIUnitRef unit = SIUnitForUnderivedSymbol(STR("N"));
    assert(unit != NULL);
    OCStringRef symbol = SIUnitCopySymbol(unit);
    SIUnitRef unit2 = SIUnitForParsedSymbol(symbol, &multiplier, &errorString);
    assert(unit2 != NULL);
    assert(SIUnitEqual(unit, unit2));
    printf("%s passed\n", __func__);
}
