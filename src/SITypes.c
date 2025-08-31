#include "SITypes.h"
#include "SIDimensionalityPrivate.h"
#include "SIScalarConstants.h"
#include <stdlib.h>

// GCC compatibility: __has_feature is Clang-specific
#ifndef __has_feature
#define __has_feature(x) 0
#endif


OCStringRef SITypesCreateStringRepresentation(OCTypeRef obj) {
    if (!obj) return NULL;
    OCTypeID type = OCGetTypeID(obj);
    if (type == SIScalarGetTypeID()) {
        return SIScalarCreateStringValue((SIScalarRef)obj);
    }
    else if (type == SIUnitGetTypeID()) {
        return SIUnitCopySymbol((SIUnitRef)obj);
    }
    else if (type == SIDimensionalityGetTypeID()) {
        return SIDimensionalityCopySymbol((SIDimensionalityRef)obj);
    }
    return NULL;
}
OCTypeRef SITypesCreateByReducing(OCTypeRef a)
{
    OCTypeID typeA = OCGetTypeID(a);

    // Perform the reduction operation
    OCTypeRef result = NULL;
    if (typeA == SIScalarGetTypeID()) {
        result = (OCTypeRef)SIScalarCreateByReducingUnit((SIScalarRef)a);
    }
    else if (typeA == SIUnitGetTypeID()) {
        double unit_multiplier = 1.0;
        result = (OCTypeRef)SIUnitByReducing((SIUnitRef)a, &unit_multiplier);
    }
    else if (typeA == SIDimensionalityGetTypeID()) {
        result = (OCTypeRef)SIDimensionalityByReducing((SIDimensionalityRef)a);
    }

    return result;
}

OCTypeRef SITypesCreateByTakingNthRoot(OCTypeRef a, int root, OCStringRef *outError)
{
    if (!a) {
        if (outError) *outError = STR("Invalid argument");
        return NULL;
    }

    OCTypeID typeA = OCGetTypeID(a);

    // Perform the nth root operation
    OCTypeRef result = NULL;
    if (typeA == SIScalarGetTypeID()) {
        result = (OCTypeRef)SIScalarCreateByTakingNthRoot((SIScalarRef)a, root, outError);
    }
    else if (typeA == SIUnitGetTypeID()) {
        double unit_multiplier = 1.0;
        result = (OCTypeRef)SIUnitByTakingNthRoot((SIUnitRef)a, root, &unit_multiplier, outError);
    }
    else if (typeA == SIDimensionalityGetTypeID()) {
        result = (OCTypeRef)SIDimensionalityByTakingNthRoot((SIDimensionalityRef)a, root, outError);
    }
    else {
        if (outError) *outError = STR("Unsupported type for nth root operation");
        return NULL;
    }

    return result;
}
OCTypeRef SITypesCreateByRaisingToPower(OCTypeRef a, int power, OCStringRef *outError)
{
    if (!a) {
        if (outError) *outError = STR("Invalid argument");
        return NULL;
    }

    OCTypeID typeA = OCGetTypeID(a);

    // Perform the power arithmetic operation
    OCTypeRef result = NULL;
    if (typeA == SIScalarGetTypeID()) {
        result = (OCTypeRef)SIScalarCreateByRaisingToPower((SIScalarRef)a, power, outError);
    }
    else if (typeA == SIUnitGetTypeID()) {
        double unit_multiplier = 1.0;
        result = (OCTypeRef)SIUnitByRaisingToPower((SIUnitRef)a, power, &unit_multiplier, outError);
    }
    else if (typeA == SIDimensionalityGetTypeID()) {
        result = (OCTypeRef)SIDimensionalityByRaisingToPower((SIDimensionalityRef)a, power);
    }
    else {
        if (outError) *outError = STR("Unsupported type for power operation");
        return NULL;
    }

    return result;
}

OCTypeRef SITypesCreateWithBinaryArithmeticOperation(OCTypeRef a, OCTypeRef b, char op, OCStringRef *outError)
{
    if (!a || !b) {
        if (outError) *outError = STR("Invalid arguments");
        return NULL;
    }

    OCTypeID typeA = OCGetTypeID(a);
    OCTypeID typeB = OCGetTypeID(b);
    if(typeA != typeB) {
        if (outError) *outError = STR("Type mismatch");
        return NULL;
    }

    // Perform the binary arithmetic operation
    OCTypeRef result = NULL;
    if (op == '+') {
        if(typeA == SIScalarGetTypeID()) {
            result = (OCTypeRef)SIScalarCreateByAdding((SIScalarRef)a, (SIScalarRef)b, outError);
        }
        else if(typeA == SIUnitGetTypeID()) {
            // Units don't add - this would be an error
            if (outError) *outError = STR("Cannot add units");
            return NULL;
        }
        else if(typeA == SIDimensionalityGetTypeID()) {
            // Dimensionalities don't add - this would be an error
            if (outError) *outError = STR("Cannot add dimensionalities");
            return NULL;
        }
    } else if (op == '-') {
        if(typeA == SIScalarGetTypeID()) {
            result = (OCTypeRef)SIScalarCreateBySubtracting((SIScalarRef)a, (SIScalarRef)b, outError);
        }
        else if(typeA == SIUnitGetTypeID()) {
            // Units don't subtract - this would be an error
            if (outError) *outError = STR("Cannot subtract units");
            return NULL;
        }
        else if(typeA == SIDimensionalityGetTypeID()) {
            // Dimensionalities don't subtract - this would be an error
            if (outError) *outError = STR("Cannot subtract dimensionalities");
            return NULL;
        }
    } else if (op == '*') {
        if(typeA == SIScalarGetTypeID()) {
            result = (OCTypeRef)SIScalarCreateByMultiplying((SIScalarRef)a, (SIScalarRef)b, outError);
        }
        else if(typeA == SIUnitGetTypeID()) {
            double unit_multiplier = 1;
            result = (OCTypeRef)SIUnitByMultiplying((SIUnitRef)a, (SIUnitRef)b, &unit_multiplier, outError);
        }
        else if(typeA == SIDimensionalityGetTypeID()) {
            result = (OCTypeRef)SIDimensionalityByMultiplying((SIDimensionalityRef)a, (SIDimensionalityRef)b);
        }
    } else if (op == '/') {
        if(typeA == SIScalarGetTypeID()) {
            result = (OCTypeRef)SIScalarCreateByDividing((SIScalarRef)a, (SIScalarRef)b, outError);
        }
        else if(typeA == SIUnitGetTypeID()) {
            double unit_multiplier = 1;
            result = (OCTypeRef)SIUnitByDividing((SIUnitRef)a, (SIUnitRef)b, &unit_multiplier, outError);
        }
        else if(typeA == SIDimensionalityGetTypeID()) {
            result = (OCTypeRef)SIDimensionalityByDividing((SIDimensionalityRef)a, (SIDimensionalityRef)b);
        }
    } else {
        if (outError) *outError = STR("Unknown operation");
        return NULL;
    }

    return result;
}

static bool siTypesShutdownCalled = false;
void SITypesShutdown(void) {
    if (siTypesShutdownCalled) return;
    siTypesShutdownCalled = true;
    cleanupScalarConstantsLibraries();
#if !defined(__SANITIZE_ADDRESS__) && !__has_feature(address_sanitizer)
    OCReportLeaksForTypeDetailed(SIScalarGetTypeID());
#endif
    SIUnitLibrariesShutdown();
#if !defined(__SANITIZE_ADDRESS__) && !__has_feature(address_sanitizer)
    OCReportLeaksForTypeDetailed(SIUnitGetTypeID());
#endif
    cleanupDimensionalityLibraries();
#if !defined(__SANITIZE_ADDRESS__) && !__has_feature(address_sanitizer)
    OCReportLeaksForTypeDetailed(SIDimensionalityGetTypeID());
#endif
    // Clean up the underlying OCTypes layer that SITypes depends on
    OCTypesShutdown();
}
// // Automatically run at program exit or library unload
// __attribute__((destructor(500)))
// static void SITypes_AutoCleanup(void) {
//     SITypesShutdown();
//     OCTypesShutdown();  // if you have a symmetric OCTypes layer
// }
