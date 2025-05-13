//
//  SILibrary.h
//  SITypes
//
//  Created by Philip Grandinetti on 6/13/17.
//  Copyright © 2017 PhySy Ltd. All rights reserved.
//

#ifndef SILibrary_h
#define SILibrary_h

#include <stdio.h> // For printf used in macros
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
        printf("*** WARNING - %s - object doesn't exist.\n", __func__); \
        return X; \
    }
/** \endcond */


#endif /* SILibrary_h */
