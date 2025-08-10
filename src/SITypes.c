#include "SITypes.h"
#include "SIDimensionalityPrivate.h"
#include "SIScalarConstants.h"
#include "SIUnitExpression.h"
#include "SIScalarParser.h"
#include "SIDimensionalityParser.h"
#include "SIUnitParser.h"
static bool siTypesShutdownCalled = false;
void SITypesShutdown(void) {
    if (siTypesShutdownCalled) return;
    siTypesShutdownCalled = true;
    cleanupScalarConstantsLibraries();
#if !defined(__SANITIZE_ADDRESS__) && !__has_feature(address_sanitizer)
    OCReportLeaksForType(SIScalarGetTypeID());
#endif
    // Clean up expression parser error state
    if (siueError) {
        OCRelease(siueError);
        siueError = NULL;
    }

    // Clean up scalar parser error state
    if (scalarErrorString) {
        OCRelease(scalarErrorString);
        scalarErrorString = NULL;
    }

    // Clean up dimensionality parser error state
    if (dimensionalityError) {
        OCRelease(dimensionalityError);
        dimensionalityError = NULL;
    }

    // Clean up unit parser error state
    if (unitError) {
        OCRelease(unitError);
        unitError = NULL;
    }

    // Comprehensive cleanup of parser states and internal buffers
    // (this includes clearing parsed expressions and error states)
    siueCleanupParserState();

    SIUnitLibrariesShutdown();
#if !defined(__SANITIZE_ADDRESS__) && !__has_feature(address_sanitizer)
    OCReportLeaksForType(SIUnitGetTypeID());
#endif
    cleanupDimensionalityLibraries();
#if !defined(__SANITIZE_ADDRESS__) && !__has_feature(address_sanitizer)
    OCReportLeaksForType(SIDimensionalityGetTypeID());
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
