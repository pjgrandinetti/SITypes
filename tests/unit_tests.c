#include "SILibrary.h"
#include <stdio.h>

#define PRINTERROR printf("failure: line %d, %s\n", __LINE__, __FUNCTION__)

bool unitTest0(void) {
    fprintf(stderr, "%s begin...\n", __func__);
    OCStringRef errorString = NULL;

    SIUnitRef unit = SIUnitForParsedSymbol(STR("m•kg^2•s^3•A^4•K^5•mol^6•cd^7/(m^2•kg^3•s^4•A^5•K^6•mol^7•cd^8)"), NULL, &errorString);
    OCStringRef plist = SIUnitCopySymbol(unit);
    SIUnitRef unit2 = SIUnitForParsedSymbol(plist, NULL, &errorString);
    if (!OCTypeEqual(unit, unit2)) PRINTERROR;

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
