#include "SITypes.h"
#include "SIDimensionalityPrivate.h"
#include "SIScalarConstants.h"

// GCC compatibility: __has_feature is Clang-specific
#ifndef __has_feature
#define __has_feature(x) 0
#endif

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
