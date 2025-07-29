#ifndef SIUNIT2_H
#define SIUNIT2_H

#include <OCLibrary.h>
#include "SIDimensionality.h"

// Forward declarations (SIUnit2Ref is already defined in SILibrary.h)

// Type functions
OCTypeID SIUnit2GetTypeID(void);

// Constructor functions
SIUnit2Ref SIUnit2WithCoherentUnit(SIDimensionalityRef dimensionality);
SIUnit2Ref SIUnit2DimensionlessAndUnderived(void);

// Accessor functions
SIDimensionalityRef SIUnit2GetDimensionality(SIUnit2Ref theUnit);
OCStringRef SIUnit2GetSymbol(SIUnit2Ref theUnit);
OCStringRef SIUnit2GetName(SIUnit2Ref theUnit);
OCStringRef SIUnit2GetPluralName(SIUnit2Ref theUnit);
double SIUnit2ScaleToCoherentSIUnit(SIUnit2Ref theUnit);

// Array creation functions
OCArrayRef SIUnit2CreateArrayOfUnitsForQuantity(OCStringRef quantity);
OCArrayRef SIUnit2CreateArrayOfUnitsForDimensionality(SIDimensionalityRef theDim);
OCArrayRef SIUnit2CreateArrayOfUnitsForSameReducedDimensionality(SIDimensionalityRef theDim);
SIUnit2Ref SIUnit2WithSymbol(OCStringRef symbol);

// Power operations
SIUnit2Ref SIUnit2ByRaisingToPowerWithoutReducing(SIUnit2Ref input,
                                                   int power,
                                                   double *unit_multiplier,
                                                   OCStringRef *error);

SIUnit2Ref SIUnit2ByRaisingToPower(SIUnit2Ref input,
                                    int power,
                                    double *unit_multiplier,
                                    OCStringRef *error);

// Multiplication operations
SIUnit2Ref SIUnit2ByMultiplyingWithoutReducing(SIUnit2Ref theUnit1,
                                               SIUnit2Ref theUnit2,
                                               double *unit_multiplier,
                                               OCStringRef *error);

SIUnit2Ref SIUnit2ByMultiplying(SIUnit2Ref theUnit1,
                                SIUnit2Ref theUnit2,
                                double *unit_multiplier,
                                OCStringRef *error);

// Division operations
SIUnit2Ref SIUnit2ByDividingWithoutReducing(SIUnit2Ref theUnit1,
                                            SIUnit2Ref theUnit2,
                                            double *unit_multiplier,
                                            OCStringRef *error);

SIUnit2Ref SIUnit2ByDividing(SIUnit2Ref theUnit1,
                             SIUnit2Ref theUnit2,
                             double *unit_multiplier,
                             OCStringRef *error);

// Nth root operations
SIUnit2Ref SIUnit2ByTakingNthRootWithoutReducing(SIUnit2Ref theUnit,
                                                 int root,
                                                 double *unit_multiplier,
                                                 OCStringRef *error);

SIUnit2Ref SIUnit2ByTakingNthRoot(SIUnit2Ref theUnit,
                                  int root,
                                  double *unit_multiplier,
                                  OCStringRef *error);

#endif // SIUNIT2_H
