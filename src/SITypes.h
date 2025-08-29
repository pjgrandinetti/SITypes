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

// Arithmetic operations
/**
 * @brief Perform binary arithmetic operation on SITypes objects.
 * @param a First operand (SIScalar, SIUnit, or SIDimensionality).
 * @param b Second operand (must be same type as a).
 * @param op Operation character: '+', '-', '*', or '/'.
 * @param outError Optional pointer to receive error message if operation fails.
 * @return Result of the operation, or NULL on failure.
 * @note Addition and subtraction are only supported for SIScalar objects.
 *       Multiplication and division work for all SITypes.
 */
OCTypeRef SITypesCreateWithBinaryArithmeticOperation(OCTypeRef a, OCTypeRef b, char op, OCStringRef *outError);

/**
 * @brief Raise SITypes object to an integer power.
 * @param a The base operand (SIScalar, SIUnit, or SIDimensionality).
 * @param power The integer exponent to raise a to.
 * @param outError Optional pointer to receive error message if operation fails.
 * @return Result of the power operation, or NULL on failure.
 * @note All SITypes support power operations with integer exponents.
 */
OCTypeRef SITypesCreateByRaisingToPower(OCTypeRef a, int power, OCStringRef *outError);

/**
 * @brief Take the nth root of an SITypes object.
 * @param a The operand (SIScalar, SIUnit, or SIDimensionality).
 * @param root The integer root to take (e.g., 2 for square root, 3 for cube root).
 * @param outError Optional pointer to receive error message if operation fails.
 * @return Result of the nth root operation, or NULL on failure.
 * @note All SITypes support nth root operations with integer roots.
 */
OCTypeRef SITypesCreateByTakingNthRoot(OCTypeRef a, int root, OCStringRef *outError);

/**
 * @brief Reduce an SITypes object to its simplest form.
 * @param a The operand (SIScalar, SIUnit, or SIDimensionality).
 * @return Result of the reduction operation, or NULL on failure.
 * @note For scalars, reduces the unit; for units, reduces to lowest terms; 
 *       for dimensionalities, reduces exponents to lowest terms.
 */
OCTypeRef SITypesCreateByReducing(OCTypeRef a);

/**
 * @brief Create a string representation of an SITypes object.
 * @param obj The SITypes object (SIScalar, SIUnit, or SIDimensionality).
 * @return String representation of the object, or NULL if obj is NULL or unsupported type.
 * @note For scalars, returns the full value with unit; for units, returns the symbol;
 *       for dimensionalities, returns the symbolic representation.
 *       The returned string must be released with OCRelease().
 */
OCStringRef SITypesCreateStringRepresentation(OCTypeRef obj);


// Enhanced JSON serialization for OCDictionary containing SITypes objects
/**
 * @brief Create JSON representation of a dictionary that may contain SITypes objects.
 * @param dict The dictionary to serialize.
 * @return A cJSON object representing the dictionary, or cJSON null if dict is NULL.
 * @note This function handles SIScalar and SIUnit types in addition to standard OCTypes.
 */
cJSON *SITypesMetadataCopyJSON(OCDictionaryRef dict);

/**
 * @brief Create a dictionary from JSON that may contain SITypes objects.
 * @param json The JSON object to deserialize.
 * @param outError Optional pointer to receive error message if parsing fails.
 * @return A new OCDictionaryRef, or NULL on failure.
 * @note This function can reconstruct SIScalar and SIUnit types from JSON.
 */
OCDictionaryRef SITypesMetadataCreateFromJSON(cJSON *json, OCStringRef *outError);

#endif /* SITypes_h */
