//
//  PSScalarConstants
//
//  Created by PhySy Ltd on 5/5/13.
//  Copyright (c) 2008-2014 PhySy Ltd. All rights reserved.
//

/*!
 @header PSScalarConstants
 
 @copyright PhySy Ltd
 @unsorted
 */

#ifndef PSScalarConstants_h
#define PSScalarConstants_h

/*!
 @function PSPeriodicTableCreateElementSymbols
 @abstract Returns an array with the element symbols
 @result an integer.
 */
OCArrayRef PSPeriodicTableCreateElementSymbols(OCStringRef *errorString);

/*!
 @function PSPeriodicTableCreateIsotopeSymbols
 @abstract Returns an array with the isotope symbols
 @result an integer.
 */
OCArrayRef PSPeriodicTableCreateIsotopeSymbols(OCStringRef *errorString);

/*!
 @function PSPeriodicTableCreateStableIsotopeSymbols
 @abstract Returns an array with the NMR active isotope symbols
 @result an integer.
 */
OCArrayRef PSPeriodicTableCreateStableIsotopeSymbols(OCStringRef *errorString);

/*!
 @function PSPeriodicTableCreateNMRActiveIsotopeSymbols
 @abstract Returns an array with the NMR active isotope symbols
 @result an integer.
 */
OCArrayRef PSPeriodicTableCreateNMRActiveIsotopeSymbols(OCStringRef *errorString);

/*!
 @function PSPeriodicTableGetStableNMRActiveIsotopeSymbols
 @abstract Returns an array with the stable NMR active isotope symbols
 @result an integer.
 */
OCArrayRef PSPeriodicTableGetStableNMRActiveIsotopeSymbols(OCStringRef *errorString);

/*!
 @function PSPeriodicTableGetAtomicNumber
 @abstract Returns the atomic number of an element
 @param elementSymbol a OCString containing the element symbol.
 @result an integer.
 */
int64_t PSPeriodicTableGetAtomicNumber(OCStringRef elementSymbol, OCStringRef *errorString);

/*!
 @function PSPeriodicTableCreateMolarMass
 @abstract Creates a scalar with the molar mass of an element
 @param elementSymbol a OCString containing the element symbol.
 @result a scalar.
 */
PSScalarRef PSPeriodicTableCreateMolarMass(OCStringRef elementSymbol, OCStringRef *errorString);

/*!
 @function PSPeriodicTableCreateIsotopeStable
 @abstract Creates a bool for whether isotope is stable or not.
 @param isotopeSymbol  a OCString containing the isotope symbol.
 @result a bool.
 */
bool PSPeriodicTableCreateIsotopeStable(OCStringRef isotopeSymbol, OCStringRef *errorString);

/*!
 @function PSPeriodicTableCreateIsotopeMagneticDipoleMoment
 @abstract Creates a scalar with the magnetic dipole moment of an isotope
 @param isotopeSymbol  a OCString containing the isotope symbol.
 @result a scalar.
 */
PSScalarRef PSPeriodicTableCreateIsotopeMagneticDipoleMoment(OCStringRef isotopeSymbol, OCStringRef *errorString);

/*!
 @function PSPeriodicTableCreateIsotopeElectricQuadrupoleMoment
 @abstract Creates a scalar with the electric quadrupole moment of an isotope
 @param isotopeSymbol  a OCString containing the isotope symbol.
 @result a scalar.
 */
PSScalarRef PSPeriodicTableCreateIsotopeElectricQuadrupoleMoment(OCStringRef isotopeSymbol, OCStringRef *errorString);

/*!
 @function PSPeriodicTableCreateIsotopeGyromagneticRatio
 @abstract Creates a scalar with the gyromagnetic ratio of an isotope
 @param isotopeSymbol  a OCString containing the isotope symbol.
 @result a scalar.
 */
PSScalarRef PSPeriodicTableCreateIsotopeGyromagneticRatio(OCStringRef isotopeSymbol, OCStringRef *errorString);

/*!
 @function PSPeriodicTableCreateIsotopeAbundance
 @abstract Creates a scalar with the natural abundance of an isotope
 @param isotopeSymbol a OCString containing the isotope symbol.
 @result a scalar.
 */
PSScalarRef PSPeriodicTableCreateIsotopeAbundance(OCStringRef isotopeSymbol, OCStringRef *errorString);

/*!
 @function PSPeriodicTableCreateIsotopeLifetime
 @abstract Creates a scalar with the lifetime of an isotope
 @param isotopeSymbol a OCString containing the isotope symbol.
 @result a scalar.
 */
PSScalarRef PSPeriodicTableCreateIsotopeLifetime(OCStringRef isotopeSymbol, OCStringRef *errorString);

/*!
 @function PSPeriodicTableCreateIsotopeHalfLife
 @abstract Creates a scalar with the half life of an isotope
 @param isotopeSymbol a OCString containing the isotope symbol.
 @result a scalar.
 */
PSScalarRef PSPeriodicTableCreateIsotopeHalfLife(OCStringRef isotopeSymbol, OCStringRef *errorString);

/*!
 @function PSPeriodicTableCreateIsotopeSpin
 @abstract Creates a scalar with the nuclear spin of an isotope
 @param isotopeSymbol a OCString containing the isotope symbol.
 @result a scalar.
 */
PSScalarRef PSPeriodicTableCreateIsotopeSpin(OCStringRef isotopeSymbol, OCStringRef *errorString);

/*!
 @function PSPeriodicTableCreateNMRFrequency
 @abstract Creates a scalar with the NMR frequency/magnetic flux density of an isotope
 @param isotopeSymbol a OCString containing the isotope symbol.
 @result a scalar.
 */
PSScalarRef PSPeriodicTableCreateNMRFrequency(OCStringRef isotopeSymbol, OCStringRef *errorString);

PSScalarRef PSPeriodicTableCreateFormulaMass(OCStringRef formula, OCStringRef *errorString);


#endif
