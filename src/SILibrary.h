/**
 * @file SILibrary.h
 * @brief Core definitions, macros, and public interfaces for the SITypes library.
 *
 * This header centralizes the core types, forward declarations, and helper macros
 * used throughout the SITypes framework. It also includes all public-facing APIs
 * from the SITypes modules.
 *
 * @note This header should be included wherever SITypes functionality is required.
 */

#ifndef SILibrary_h
#define SILibrary_h

// C Standard Library Headers
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h> // Needed for memset, strcmp, etc.

// Forward declarations for all opaque types used across SITypes
struct __SIDimensionality;
struct __SIUnit;
struct __SIQuantity;
struct __SIScalar;

// Include OCTypes base framework
#include <OCLibrary.h>

// Public SITypes API headers
#include "SIDimensionality.h"
#include "SIDimensionalityParser.h"
#include "SIUnit.h"
#include "SIUnitParser.h"
#include "SIQuantity.h"
#include "SIScalar.h"
#include "SIScalarConstants.h"


#endif /* SILibrary_h */
