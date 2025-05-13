//
//  SIScalarConstants
//
//  Created by PhySy Ltd on 5/5/13.
//  Copyright (c) 2008-2014 PhySy Ltd. All rights reserved.
//

/*!
 @header SIScalarConstants
 
 @copyright PhySy Ltd
 @unsorted
 */

#ifndef SIScalarConstants_h
#define SIScalarConstants_h

/*!
 @function SIPeriodicTableCreateElementSymbols
 @abstract Returns an array with the element symbols
 @result an integer.
 */
OCArrayRef SIPeriodicTableCreateElementSymbols(OCStringRef *errorString);

/*!
 @function SIPeriodicTableCreateIsotopeSymbols
 @abstract Returns an array with the isotope symbols
 @result an integer.
 */
OCArrayRef SIPeriodicTableCreateIsotopeSymbols(OCStringRef *errorString);

/*!
 @function SIPeriodicTableCreateStableIsotopeSymbols
 @abstract Returns an array with the NMR active isotope symbols
 @result an integer.
 */
OCArrayRef SIPeriodicTableCreateStableIsotopeSymbols(OCStringRef *errorString);

/*!
 @function SIPeriodicTableCreateNMRActiveIsotopeSymbols
 @abstract Returns an array with the NMR active isotope symbols
 @result an integer.
 */
OCArrayRef SIPeriodicTableCreateNMRActiveIsotopeSymbols(OCStringRef *errorString);

/*!
 @function SIPeriodicTableGetStableNMRActiveIsotopeSymbols
 @abstract Returns an array with the stable NMR active isotope symbols
 @result an integer.
 */
OCArrayRef SIPeriodicTableGetStableNMRActiveIsotopeSymbols(OCStringRef *errorString);

/*!
 @function SIPeriodicTableGetAtomicNumber
 @abstract Returns the atomic number of an element
 @param elementSymbol a OCString containing the element symbol.
 @result an integer.
 */
int64_t SIPeriodicTableGetAtomicNumber(OCStringRef elementSymbol, OCStringRef *errorString);

/*!
 @function SIPeriodicTableCreateMolarMass
 @abstract Creates a scalar with the molar mass of an element
 @param elementSymbol a OCString containing the element symbol.
 @result a scalar.
 */
SIScalarRef SIPeriodicTableCreateMolarMass(OCStringRef elementSymbol, OCStringRef *errorString);

/*!
 @function SIPeriodicTableCreateIsotopeStable
 @abstract Creates a bool for whether isotope is stable or not.
 @param isotopeSymbol  a OCString containing the isotope symbol.
 @result a bool.
 */
bool SIPeriodicTableCreateIsotopeStable(OCStringRef isotopeSymbol, OCStringRef *errorString);

/*!
 @function SIPeriodicTableCreateIsotopeMagneticDipoleMoment
 @abstract Creates a scalar with the magnetic dipole moment of an isotope
 @param isotopeSymbol  a OCString containing the isotope symbol.
 @result a scalar.
 */
SIScalarRef SIPeriodicTableCreateIsotopeMagneticDipoleMoment(OCStringRef isotopeSymbol, OCStringRef *errorString);

/*!
 @function SIPeriodicTableCreateIsotopeElectricQuadrupoleMoment
 @abstract Creates a scalar with the electric quadrupole moment of an isotope
 @param isotopeSymbol  a OCString containing the isotope symbol.
 @result a scalar.
 */
SIScalarRef SIPeriodicTableCreateIsotopeElectricQuadrupoleMoment(OCStringRef isotopeSymbol, OCStringRef *errorString);

/*!
 @function SIPeriodicTableCreateIsotopeGyromagneticRatio
 @abstract Creates a scalar with the gyromagnetic ratio of an isotope
 @param isotopeSymbol  a OCString containing the isotope symbol.
 @result a scalar.
 */
SIScalarRef SIPeriodicTableCreateIsotopeGyromagneticRatio(OCStringRef isotopeSymbol, OCStringRef *errorString);

/*!
 @function SIPeriodicTableCreateIsotopeAbundance
 @abstract Creates a scalar with the natural abundance of an isotope
 @param isotopeSymbol a OCString containing the isotope symbol.
 @result a scalar.
 */
SIScalarRef SIPeriodicTableCreateIsotopeAbundance(OCStringRef isotopeSymbol, OCStringRef *errorString);

/*!
 @function SIPeriodicTableCreateIsotopeLifetime
 @abstract Creates a scalar with the lifetime of an isotope
 @param isotopeSymbol a OCString containing the isotope symbol.
 @result a scalar.
 */
SIScalarRef SIPeriodicTableCreateIsotopeLifetime(OCStringRef isotopeSymbol, OCStringRef *errorString);

/*!
 @function SIPeriodicTableCreateIsotopeHalfLife
 @abstract Creates a scalar with the half life of an isotope
 @param isotopeSymbol a OCString containing the isotope symbol.
 @result a scalar.
 */
SIScalarRef SIPeriodicTableCreateIsotopeHalfLife(OCStringRef isotopeSymbol, OCStringRef *errorString);

/*!
 @function SIPeriodicTableCreateIsotopeSpin
 @abstract Creates a scalar with the nuclear spin of an isotope
 @param isotopeSymbol a OCString containing the isotope symbol.
 @result a scalar.
 */
SIScalarRef SIPeriodicTableCreateIsotopeSpin(OCStringRef isotopeSymbol, OCStringRef *errorString);

/*!
 @function SIPeriodicTableCreateNMRFrequency
 @abstract Creates a scalar with the NMR frequency/magnetic flux density of an isotope
 @param isotopeSymbol a OCString containing the isotope symbol.
 @result a scalar.
 */
SIScalarRef SIPeriodicTableCreateNMRFrequency(OCStringRef isotopeSymbol, OCStringRef *errorString);

SIScalarRef SIPeriodicTableCreateFormulaMass(OCStringRef formula, OCStringRef *errorString);


#endif
