#include "SITypes.h"
#include "SIDimensionalityPrivate.h"
#include "SIScalarConstants.h"
void SITypesShutdown(void) {
    cleanupScalarConstantsLibraries();
#if !defined(__SANITIZE_ADDRESS__) && !__has_feature(address_sanitizer)
    OCReportLeaksForType(SIScalarGetTypeID());
#endif
    SIUnitLibrariesShutdown();
#if !defined(__SANITIZE_ADDRESS__) && !__has_feature(address_sanitizer)
    OCReportLeaksForType(SIUnitGetTypeID());
#endif
    cleanupDimensionalityLibraries();
#if !defined(__SANITIZE_ADDRESS__) && !__has_feature(address_sanitizer)
    OCReportLeaksForType(SIDimensionalityGetTypeID());
#endif
}
// // Automatically run at program exit or library unload
// __attribute__((destructor(500)))
// static void SITypes_AutoCleanup(void) {
//     SITypesShutdown();
//     OCTypesShutdown();  // if you have a symmetric OCTypes layer
// }
