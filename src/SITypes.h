/**
 * @file SITypes.h
 * @brief Core definitions, macros, and public interfaces for the SITypes library.
 *
 * This header centralizes the core types, forward declarations, and helper macros
 * used throughout the SITypes framework. It also includes all public-facing APIs
 * from the SITypes modules.
 *
 * @note This header should be included wherever SITypes functionality is required.
 */
#ifndef SITypes_h
#define SITypes_h
// C Standard Library Headers
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>          // Needed for memset, strcmp, etc.
#define SILIB_TYPES_COUNT 3  // Total number of types in SITypes
/** @cond INTERNAL */
// Centralized Ref typedefs
typedef const struct impl_SIDimensionality *SIDimensionalityRef;
typedef const struct impl_SIUnit *SIUnitRef;
typedef const struct impl_SIUnit *SIUnitRef;
typedef const struct impl_SIQuantity *SIQuantityRef;
typedef const struct impl_SIScalar *SIScalarRef;
typedef struct impl_SIScalar *SIMutableScalarRef;
/** @endcond */
// Include OCTypes base framework
#include <OCTypes.h>
// Public SITypes API headers
#include "SIDimensionality.h"
#include "SIDimensionalityParser.h"
#include "SIQuantity.h"
#include "SIScalar.h"
#include "SIScalarConstants.h"
#include "SIUnit.h"
#include "SIUnitParser.h"

// Library management functions
void SITypesShutdown(void);

// Enhanced JSON serialization for OCDictionary containing SITypes objects
/**
 * @brief Create JSON representation of a dictionary that may contain SITypes objects.
 * @param dict The dictionary to serialize.
 * @return A cJSON object representing the dictionary, or cJSON null if dict is NULL.
 * @note This function handles SIScalar and SIUnit types in addition to standard OCTypes.
 */
cJSON *SIMetadataCopyJSON(OCDictionaryRef dict);

/**
 * @brief Create a dictionary from JSON that may contain SITypes objects.
 * @param json The JSON object to deserialize.
 * @param outError Optional pointer to receive error message if parsing fails.
 * @return A new OCDictionaryRef, or NULL on failure.
 * @note This function can reconstruct SIScalar and SIUnit types from JSON.
 */
OCDictionaryRef SIMetadataCreateFromJSON(cJSON *json, OCStringRef *outError);

#endif /* SITypes_h */
