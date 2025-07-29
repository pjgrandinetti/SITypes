#ifndef SIUNIT2_H
#define SIUNIT2_H

#include <OCLibrary.h>
#include "SIDimensionality.h"

// Forward declarations (SIUnit2Ref is already defined in SILibrary.h)

// Type functions
OCTypeID SIUnit2GetTypeID(void);

// Constructor functions
SIUnit2Ref SIUnit2CoherentUnitFromDimensionality(SIDimensionalityRef dimensionality);
SIUnit2Ref SIUnit2DimensionlessAndUnderived(void);

// Accessor functions
SIDimensionalityRef SIUnit2GetDimensionality(SIUnit2Ref theUnit);
OCStringRef SIUnit2GetSymbol(SIUnit2Ref theUnit);
OCStringRef SIUnit2GetName(SIUnit2Ref theUnit);
OCStringRef SIUnit2GetPluralName(SIUnit2Ref theUnit);
double SIUnit2GetScaleToCoherentSI(SIUnit2Ref theUnit);
double SIUnit2ScaleToCoherentSIUnit(SIUnit2Ref theUnit);
OCStringRef SIUnit2CopySymbol(SIUnit2Ref theUnit);
OCStringRef SIUnit2CopyName(SIUnit2Ref theUnit);
OCStringRef SIUnit2CopyPluralName(SIUnit2Ref theUnit);

bool SIUnit2Equal(SIUnit2Ref theUnit1, SIUnit2Ref theUnit2);
bool SIUnit2AreEquivalentUnits(SIUnit2Ref theUnit1, SIUnit2Ref theUnit2);
bool SIUnit2IsCoherentUnit(SIUnit2Ref theUnit);
bool SIUnit2IsDimensionless(SIUnit2Ref theUnit);

// Unit conversion
double SIUnit2Conversion(SIUnit2Ref initialUnit, SIUnit2Ref finalUnit);

// Unit library management
void SIUnit2LibrarySetImperialVolumes(bool value);
bool SIUnit2LibraryGetImperialVolumes(void);
void SIUnit2LibrariesRelease(void);  // do not call, called by SITypesShutdown()

// Array creation functions
OCArrayRef SIUnit2CreateArrayOfUnitsForQuantity(OCStringRef quantity);
OCArrayRef SIUnit2CreateArrayOfUnitsForDimensionality(SIDimensionalityRef theDim);
OCArrayRef SIUnit2CreateArrayOfUnitsForSameReducedDimensionality(SIDimensionalityRef theDim);
OCArrayRef SIUnit2CreateArrayOfConversionUnits(SIUnit2Ref theUnit);
OCArrayRef SIUnit2CreateArrayOfEquivalentUnits(SIUnit2Ref theUnit);
SIUnit2Ref SIUnit2WithSymbol(OCStringRef symbol);
SIUnit2Ref SIUnit2FindWithName(OCStringRef input);
SIUnit2Ref SIUnit2FindEquivalentUnitWithShortestSymbol(SIUnit2Ref theUnit);

// Unit reduction operations
SIUnit2Ref SIUnit2ByReducing(SIUnit2Ref theUnit, double *unit_multiplier);

// Utility functions
OCStringRef SIUnit2GuessQuantityName(SIUnit2Ref theUnit);

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
SIUnit2Ref SIUnit2ByTakingNthRoot(SIUnit2Ref theUnit,
                                  int root,
                                  double *unit_multiplier,
                                  OCStringRef *error);

#endif // SIUNIT2_H
