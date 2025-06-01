/**
 * @file SILibrary.h
 * @brief Core definitions, macros, and types for the SITypes library.
 *
 * This header centralizes the core types and helper macros used throughout
 * the SITypes framework, and then includes all the public SITypes APIs.
 */

#ifndef SILibrary_h
#define SILibrary_h

#include <stdio.h> 
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// Forward declarations for all opaque struct types
struct __SIDimensionality;
struct __SIUnit;
struct __SIQuantity;
struct __SIScalar;

#include <OCTypes/OCLibrary.h>

#include "SIDimensionality.h"
#include "SIDimensionalityParser.h"
#include "SIUnit.h"
#include "SIUnitParser.h"
#include "SIQuantity.h"
#include "SIScalar.h"
#include "SIScalarConstants.h"

/** \cond INTERNAL */
#define IF_NO_OBJECT_EXISTS_RETURN(OBJECT, X) \
    if (NULL == OBJECT) { \
        fprintf(stderr, "*** WARNING - %s - object doesn't exist.\n", __func__); \
        return X; \
    }
/** \endcond */


#endif /* SILibrary_h */
