//
//  SIScalarConstants.c
//
//  Created by PhySy Ltd on 5/5/13.
//  Copyright (c) 2008-2014 PhySy Ltd. All rights reserved.
//

#include "SILibrary.h"
#include <math.h> // For INFINITY
#include "SIScalarConstants.h"
#include "SIScalarConstantsData.h"
#include "SIUnitParser.h"

OCMutableDictionaryRef molarMassLibrary = NULL;
OCMutableDictionaryRef isotopeAbundanceLibrary = NULL;
OCMutableDictionaryRef isotopeStableLibrary = NULL;
OCMutableDictionaryRef isotopeSpinLibrary = NULL;
OCMutableDictionaryRef isotopeLifetimeLibrary = NULL;
OCMutableDictionaryRef isotopeHalfLifeLibrary = NULL;
OCMutableDictionaryRef nuclearMagneticMomentLibrary = NULL;
OCMutableDictionaryRef nuclearElectricQuadrupoleMomentLibrary = NULL;
OCMutableDictionaryRef nuclearGyromagneticRatioLibrary = NULL;

static bool SIPeriodicTableCreateMolarMassLibrary(OCStringRef *errorString)
{
    if(errorString) if(*errorString) return false;
    
    molarMassLibrary  = OCDictionaryCreateMutable(0);
    
    double multiplier = 1.0;
    SIUnitRef unit = SIUnitForParsedSymbol(STR("g/mol"), &multiplier, errorString);
    
    for(int64_t index=0;index<118;index++) {
        SIScalarRef value = SIScalarCreateWithDouble(atomicMass[index]*multiplier,unit);
        OCStringRef key = OCStringCreateWithCString(atomicSymbol[index]);
        OCMutableStringRef lowerCaseKey = OCStringCreateMutableCopy(key);
        OCStringLowercase(lowerCaseKey);
        OCDictionaryAddValue(molarMassLibrary, lowerCaseKey, value);
        OCRelease(value);
        OCRelease(key);
        OCRelease(lowerCaseKey);
    }
    
    for(int64_t index=0;index<3181;index++) {
        if(isotopeMass[index]) {
            SIScalarRef value = SIScalarCreateWithDouble(isotopeMass[index],unit);
            OCStringRef key = OCStringCreateWithCString(isotopeSymbol[index]);
            OCMutableStringRef lowerCaseKey = OCStringCreateMutableCopy(key);
            OCStringLowercase(lowerCaseKey);
            OCDictionaryAddValue(molarMassLibrary, lowerCaseKey, value);
            OCRelease(value);
            OCRelease(key);
            OCRelease(lowerCaseKey);
        }
    }
    return true;
}

OCArrayRef SIPeriodicTableCreateElementSymbols(OCStringRef *errorString)
{
    if(errorString) if(*errorString) return NULL;
    OCMutableArrayRef symbols = OCArrayCreateMutable(0, &kOCTypeArrayCallBacks);
    
    for(int64_t index=0;index<118;index++) {
        OCStringRef symbol = OCStringCreateWithCString(atomicSymbol[index]);
        OCArrayAppendValue(symbols, symbol);
        OCRelease(symbol);
    }
    return symbols;
}

OCArrayRef SIPeriodicTableCreateIsotopeSymbols(OCStringRef *errorString)
{
    if(errorString) if(*errorString) return NULL;
    OCMutableArrayRef symbols = OCArrayCreateMutable(0, &kOCTypeArrayCallBacks);
    
    for(int64_t index=0;index<3181;index++) {
        if(isotopeSpin[index]>=0) {
            OCStringRef symbol = OCStringCreateWithCString(isotopeSymbol[index]);
            OCArrayAppendValue(symbols, symbol);
            OCRelease(symbol);
        }
    }
    return symbols;
}

OCArrayRef SIPeriodicTableCreateNMRActiveIsotopeSymbols(OCStringRef *errorString)
{
    if(errorString) if(*errorString) return NULL;
    OCMutableArrayRef symbols = OCArrayCreateMutable(0, &kOCTypeArrayCallBacks);
    
    for(int64_t index=0;index<3181;index++) {
        if(isotopeSpin[index]>0) {
            OCStringRef symbol = OCStringCreateWithCString(isotopeSymbol[index]);
            OCArrayAppendValue(symbols, symbol);
            OCRelease(symbol);
        }
    }
    return symbols;
}

OCArrayRef SIPeriodicTableCreateStableIsotopeSymbols(OCStringRef *errorString)
{
    if(errorString) if(*errorString) return NULL;
    OCMutableArrayRef symbols = OCArrayCreateMutable(0, &kOCTypeArrayCallBacks);
    
    for(int64_t index=0;index<3181;index++) {
        if(isotopeStable[index]) {
            OCStringRef symbol = OCStringCreateWithCString(isotopeSymbol[index]);
            OCArrayAppendValue(symbols, symbol);
            OCRelease(symbol);
        }
    }
    return symbols;
}

OCArrayRef SIPeriodicTableCreateStableNMRActiveIsotopeSymbols(OCStringRef *errorString)
{
    if(errorString) if(*errorString) return NULL;
    OCMutableArrayRef symbols = OCArrayCreateMutable(0, &kOCTypeArrayCallBacks);
    
    for(int64_t index=0;index<3181;index++) {
        if(isotopeSpin[index]>0 && isotopeStable[index]) {
            OCStringRef symbol = OCStringCreateWithCString(isotopeSymbol[index]);
            OCArrayAppendValue(symbols, symbol);
            OCRelease(symbol);
        }
    }
    return symbols;
}

int64_t SIPeriodicTableGetAtomicNumber(OCStringRef elementSymbol, OCStringRef *errorString)
{
    if(errorString) if(*errorString) return kOCNotFound;
    for(int64_t index=0;index<118;index++) {
        OCStringRef symbol = OCStringCreateWithCString(atomicSymbol[index]);
        
        if(OCStringCompare(symbol, elementSymbol, kOCCompareCaseInsensitive) == kOCCompareEqualTo) {
            OCRelease(symbol);
            return index+1;
        }
        OCRelease(symbol);
    }
    return kOCNotFound;
}

SIScalarRef SIPeriodicTableCreateMolarMass(OCStringRef elementSymbol, OCStringRef *errorString)
{
    if(errorString) if(*errorString) return NULL;
    
    if(NULL==molarMassLibrary) SIPeriodicTableCreateMolarMassLibrary(errorString);
    OCMutableStringRef lowerCaseKey = OCStringCreateMutableCopy(elementSymbol);
    OCStringLowercase(lowerCaseKey);
    SIScalarRef molarMass = (SIScalarRef) OCDictionaryGetValue(molarMassLibrary, lowerCaseKey);
    OCRelease(lowerCaseKey);
    if(NULL == molarMass && errorString !=NULL) {
        *errorString = STR("Symbol not found");
    }
    return SIScalarCreateCopy(molarMass);
}

static void SIPeriodicTableIsotopeStableibrary(void)
{
    isotopeStableLibrary  = OCDictionaryCreateMutable(0);
    
    for(int64_t index=0;index<3181;index++) {
        OCBooleanRef value = kOCBooleanFalse;
        if(isotopeStable[index]) value = kOCBooleanTrue;
        OCStringRef key = OCStringCreateWithCString(isotopeSymbol[index]);
        OCMutableStringRef lowerCaseKey = OCStringCreateMutableCopy(key);
        OCStringLowercase(lowerCaseKey);
        OCDictionaryAddValue(isotopeStableLibrary, lowerCaseKey, value);
        OCRelease(value);
        OCRelease(key);
        OCRelease(lowerCaseKey);
    }
}

bool SIPeriodicTableCreateIsotopeStable(OCStringRef isotopeSymbol, OCStringRef *errorString)
{
    if(errorString) if(*errorString) return NULL;
    
    if(NULL==isotopeStableLibrary) SIPeriodicTableIsotopeStableibrary();
    OCMutableStringRef lowerCaseKey = OCStringCreateMutableCopy(isotopeSymbol);
    OCStringLowercase(lowerCaseKey);
    OCBooleanRef stable = (OCBooleanRef) OCDictionaryGetValue(isotopeStableLibrary, lowerCaseKey);
    OCRelease(lowerCaseKey);
    if(NULL == stable && errorString!=NULL) {
        *errorString = STR("Symbol not found");
    }
    return OCBooleanGetValue(stable);
}


static void SIPeriodicTableIsotopeHalfLifeLibrary(void)
{
    isotopeHalfLifeLibrary  = OCDictionaryCreateMutable(0);
    
    SIUnitRef unit = SIUnitForUnderivedSymbol(STR("s"));
    
    for(int64_t index=0;index<3181;index++) {
        SIScalarRef value = SIScalarCreateWithDouble(isotopeHalfLife[index],unit);
        OCStringRef key = OCStringCreateWithCString(isotopeSymbol[index]);
        OCMutableStringRef lowerCaseKey = OCStringCreateMutableCopy(key);
        OCStringLowercase(lowerCaseKey);
        OCDictionaryAddValue(isotopeHalfLifeLibrary, lowerCaseKey, value);
        OCRelease(value);
        OCRelease(key);
        OCRelease(lowerCaseKey);
    }
}

SIScalarRef SIPeriodicTableCreateIsotopeHalfLife(OCStringRef isotopeSymbol, OCStringRef *errorString)
{
    if(errorString) if(*errorString) return NULL;
    
    if(NULL==isotopeHalfLifeLibrary) SIPeriodicTableIsotopeHalfLifeLibrary();
    OCMutableStringRef lowerCaseKey = OCStringCreateMutableCopy(isotopeSymbol);
    OCStringLowercase(lowerCaseKey);
    SIScalarRef halfLife = (SIScalarRef) OCDictionaryGetValue(isotopeHalfLifeLibrary, lowerCaseKey);
    OCRelease(lowerCaseKey);
    if(NULL == halfLife && errorString!=NULL) {
        *errorString = STR("Symbol not found");
    }
    return SIScalarCreateCopy(halfLife);
}



static void SIPeriodicTableIsotopeLifetimeLibrary(void)
{
    isotopeLifetimeLibrary  = OCDictionaryCreateMutable(0);
    
    SIUnitRef unit = SIUnitForUnderivedSymbol(STR("s"));
    
    for(int64_t index=0;index<3181;index++) {
        SIScalarRef value = SIScalarCreateWithDouble(isotopeLifeTime[index],unit);
        OCStringRef key = OCStringCreateWithCString(isotopeSymbol[index]);
        OCMutableStringRef lowerCaseKey = OCStringCreateMutableCopy(key);
        OCStringLowercase(lowerCaseKey);
        OCDictionaryAddValue(isotopeLifetimeLibrary, lowerCaseKey, value);
        OCRelease(value);
        OCRelease(key);
        OCRelease(lowerCaseKey);
    }
}

SIScalarRef SIPeriodicTableCreateIsotopeLifetime(OCStringRef isotopeSymbol, OCStringRef *errorString)
{
    if(errorString) if(*errorString) return NULL;
    
    if(NULL==isotopeLifetimeLibrary) SIPeriodicTableIsotopeLifetimeLibrary();
    OCMutableStringRef lowerCaseKey = OCStringCreateMutableCopy(isotopeSymbol);
    OCStringLowercase(lowerCaseKey);
    SIScalarRef lifetime = (SIScalarRef) OCDictionaryGetValue(isotopeLifetimeLibrary, lowerCaseKey);
    OCRelease(lowerCaseKey);
    if(NULL == lifetime && errorString!=NULL) {
        *errorString = STR("Symbol not found");
    }
    return SIScalarCreateCopy(lifetime);
}


static void SIPeriodicTableIsotopeAbundanceLibrary(void)
{
    isotopeAbundanceLibrary  = OCDictionaryCreateMutable(0);
    
    SIUnitRef unit = SIUnitDimensionlessAndUnderived();
    
    for(int64_t index=0;index<3181;index++) {
        SIScalarRef value = SIScalarCreateWithDouble(isotopeAbundance[index],unit);
        OCStringRef key = OCStringCreateWithCString(isotopeSymbol[index]);
        OCMutableStringRef lowerCaseKey = OCStringCreateMutableCopy(key);
        OCStringLowercase(lowerCaseKey);
        OCDictionaryAddValue(isotopeAbundanceLibrary, lowerCaseKey, value);
        OCRelease(value);
        OCRelease(key);
        OCRelease(lowerCaseKey);
    }
}


SIScalarRef SIPeriodicTableCreateIsotopeAbundance(OCStringRef isotopeSymbol, OCStringRef *errorString)
{
    if(errorString) if(*errorString) return NULL;
    
    if(NULL==isotopeAbundanceLibrary) SIPeriodicTableIsotopeAbundanceLibrary();
    OCMutableStringRef lowerCaseKey = OCStringCreateMutableCopy(isotopeSymbol);
    OCStringLowercase(lowerCaseKey);
    SIScalarRef abundance = (SIScalarRef) OCDictionaryGetValue(isotopeAbundanceLibrary, lowerCaseKey);
    OCRelease(lowerCaseKey);
    if(NULL == abundance && errorString!=NULL) {
        *errorString = STR("Symbol not found");
    }
    return SIScalarCreateCopy(abundance);
}

static bool SIPeriodicTableNuclearElectricQuadrupoleMomentLibrary(OCStringRef *errorString)
{
    if(errorString) if(*errorString) return false;
    nuclearElectricQuadrupoleMomentLibrary = OCDictionaryCreateMutable(0);
    
    double multiplier = 1.0;
    SIUnitRef unit = SIUnitForParsedSymbol(STR("b"), &multiplier, errorString);
    
    for(int64_t index=0;index<3181;index++) {
        if(quadMoment[index] != -99) {
            double moment = quadMoment[index];
            SIScalarRef value = SIScalarCreateWithDouble(moment,unit);
            OCStringRef key = OCStringCreateWithCString( isotopeSymbol[index]);
            OCMutableStringRef lowerCaseKey = OCStringCreateMutableCopy(key);
            OCStringLowercase(lowerCaseKey);
            OCDictionaryAddValue(nuclearElectricQuadrupoleMomentLibrary, lowerCaseKey, value);
            OCRelease(value);
            OCRelease(key);
            OCRelease(lowerCaseKey);
        }
    }
    return true;
}

SIScalarRef SIPeriodicTableCreateIsotopeElectricQuadrupoleMoment(OCStringRef isotopeSymbol, OCStringRef *errorString)
{
    if(errorString) if(*errorString) return NULL;
    
    if(NULL==nuclearElectricQuadrupoleMomentLibrary) SIPeriodicTableNuclearElectricQuadrupoleMomentLibrary(errorString);
    OCMutableStringRef lowerCaseKey = OCStringCreateMutableCopy(isotopeSymbol);
    OCStringLowercase(lowerCaseKey);
    SIScalarRef electricQuadrupoleMoment = (SIScalarRef) OCDictionaryGetValue(nuclearElectricQuadrupoleMomentLibrary, lowerCaseKey);
    OCRelease(lowerCaseKey);
    if(NULL == electricQuadrupoleMoment && errorString!=NULL) {
        *errorString = STR("Symbol not found");
    }
    return SIScalarCreateCopy(electricQuadrupoleMoment);
}


static bool SIPeriodicTableNuclearMagneticDipoleMomentLibrary(OCStringRef *errorString)
{
    if(errorString) if(*errorString) return false;
    nuclearMagneticMomentLibrary  = OCDictionaryCreateMutable(0);
    
    double multiplier = 1.0;
    SIUnitRef unit = SIUnitForParsedSymbol(STR("µ_N"), &multiplier, errorString);
    
    for(int64_t index=0;index<3181;index++) {
        if(isotopeSpin[index] != -99) {
            double moment = isotopeMagneticMoment[index];
            SIScalarRef value = SIScalarCreateWithDouble(moment,unit);
            OCStringRef key = OCStringCreateWithCString(isotopeSymbol[index]);
            OCMutableStringRef lowerCaseKey = OCStringCreateMutableCopy(key);
            OCStringLowercase(lowerCaseKey);
            OCDictionaryAddValue(nuclearMagneticMomentLibrary, lowerCaseKey, value);
            OCRelease(value);
            OCRelease(key);
            OCRelease(lowerCaseKey);
        }
    }
    return true;
}

SIScalarRef SIPeriodicTableCreateIsotopeMagneticDipoleMoment(OCStringRef isotopeSymbol, OCStringRef *errorString)
{
    if(errorString) if(*errorString) return NULL;
    
    if(NULL==nuclearMagneticMomentLibrary) SIPeriodicTableNuclearMagneticDipoleMomentLibrary(errorString);
    OCMutableStringRef lowerCaseKey = OCStringCreateMutableCopy(isotopeSymbol);
    OCStringLowercase(lowerCaseKey);
    SIScalarRef magneticDipoleMoment = (SIScalarRef) OCDictionaryGetValue(nuclearMagneticMomentLibrary, lowerCaseKey);
    OCRelease(lowerCaseKey);
    if(NULL == magneticDipoleMoment && errorString!=NULL) {
        *errorString = STR("Symbol not found");
    }
    return SIScalarCreateCopy(magneticDipoleMoment);
}

static void SIPeriodicTableIsotopeSpinLibrary(void)
{
    isotopeSpinLibrary  = OCDictionaryCreateMutable(0);
    
    SIUnitRef unit = SIUnitDimensionlessAndUnderived();
    
    for(int64_t index=0;index<3181;index++) {
        if(isotopeSpin[index] != -99) {
            SIScalarRef value = SIScalarCreateWithDouble(isotopeSpin[index],unit);
            OCStringRef key = OCStringCreateWithCString(isotopeSymbol[index]);
            OCMutableStringRef lowerCaseKey = OCStringCreateMutableCopy(key);
            OCStringLowercase(lowerCaseKey);
            OCDictionaryAddValue(isotopeSpinLibrary, lowerCaseKey, value);
            OCRelease(value);
            OCRelease(key);
            OCRelease(lowerCaseKey);
        }
    }
}

SIScalarRef SIPeriodicTableCreateIsotopeSpin(OCStringRef isotopeSymbol, OCStringRef *errorString)
{
    if(errorString) if(*errorString) return NULL;
    
    if(NULL==isotopeSpinLibrary) SIPeriodicTableIsotopeSpinLibrary();
    
    OCMutableStringRef lowerCaseKey = OCStringCreateMutableCopy(isotopeSymbol);
    OCStringLowercase(lowerCaseKey);
    SIScalarRef spin = (SIScalarRef) OCDictionaryGetValue(isotopeSpinLibrary, lowerCaseKey);
    OCRelease(lowerCaseKey);
    if(NULL == spin && errorString!=NULL) {
        *errorString = STR("Symbol not found");
    }
    return SIScalarCreateCopy(spin);
}

static bool SIPeriodicTableNuclearGyromagneticRatioLibrary(OCStringRef *errorString)
{
    if(errorString) if(*errorString) return false;
    nuclearGyromagneticRatioLibrary  = OCDictionaryCreateMutable(0);
    
    double multiplier = 1.0;
    SIUnitRef unit = SIUnitForParsedSymbol(STR("rad/(s•T)"), &multiplier, errorString);
    
    for(int64_t index=0;index<3181;index++) {
        if(isotopeMagneticMoment[index] && isotopeSpin[index] != -99) {
            double spin =isotopeSpin[index];
            double moment = isotopeMagneticMoment[index];
            SIScalarRef value = SIScalarCreateWithDouble(moment*5.0507832413e-27*multiplier/spin,unit);
            OCStringRef key = OCStringCreateWithCString(isotopeSymbol[index]);
            OCMutableStringRef lowerCaseKey = OCStringCreateMutableCopy(key);
            OCStringLowercase(lowerCaseKey);
            OCDictionaryAddValue(nuclearGyromagneticRatioLibrary, lowerCaseKey, value);
            OCRelease(value);
            OCRelease(key);
            OCRelease(lowerCaseKey);
        }
    }
    return true;
}

SIScalarRef SIPeriodicTableCreateIsotopeGyromagneticRatio(OCStringRef isotopeSymbol, OCStringRef *errorString)
{
    if(errorString) if(*errorString) return NULL;
    
    if(NULL==nuclearMagneticMomentLibrary) SIPeriodicTableNuclearMagneticDipoleMomentLibrary(errorString);
    if(NULL==isotopeSpinLibrary) SIPeriodicTableIsotopeSpinLibrary();

    OCMutableStringRef lowerCaseKey = OCStringCreateMutableCopy( isotopeSymbol);
    OCStringLowercase(lowerCaseKey);
    SIScalarRef magneticMoment = (SIScalarRef) OCDictionaryGetValue(nuclearMagneticMomentLibrary, lowerCaseKey);
    SIScalarRef spin = (SIScalarRef) OCDictionaryGetValue(isotopeSpinLibrary, lowerCaseKey);
    SIScalarRef hbar = SIScalarCreateWithOCString(STR("ℏ"), errorString);
    
    OCRelease(lowerCaseKey);
    if((NULL == magneticMoment || NULL == spin || NULL == hbar ) && errorString!=NULL) {
        *errorString = STR("Symbol not found");
    }
    
    
    SIMutableScalarRef gyromagneticRatio = SIScalarCreateMutableCopy(magneticMoment);
    SIScalarDivide(gyromagneticRatio, hbar, errorString);
    if(hbar) OCRelease(hbar);
    
    if(SIScalarDoubleValue(spin)>0) SIScalarMultiplyByDimensionlessRealConstant(gyromagneticRatio, 1./SIScalarDoubleValue(spin));
    
    double multiplier = 1;
    SIUnitRef unit = SIUnitForSymbol(STR("rad/(s•T)"), &multiplier, errorString);
    SIScalarConvertToUnit(gyromagneticRatio, unit, errorString);
    
    return gyromagneticRatio;
}

SIScalarRef SIPeriodicTableCreateNMRFrequency(OCStringRef isotopeSymbol, OCStringRef *errorString)
{
    if(errorString) if(*errorString) return NULL;
    
    SIScalarRef gyro = SIPeriodicTableCreateIsotopeGyromagneticRatio(isotopeSymbol, errorString);
    if(NULL == gyro  && errorString!=NULL) {
        *errorString = STR("Symbol not found");
        return NULL;
    }
    SIMutableScalarRef nmr = SIScalarCreateMutableCopy(gyro);
    SIScalarMultiplyByDimensionlessRealConstant(nmr, 1./6.283185307179586);
    
    double multiplier = 1;
    SIUnitRef unit = SIUnitForSymbol(STR("MHz/T"), &multiplier, errorString);
    SIScalarConvertToUnit(nmr, unit, errorString);
    return nmr;
}

bool SIPeriodicTableIsElement(OCStringRef elementSymbol)
{
    for(int64_t index=0;index<118;index++) {
        OCStringRef key = OCStringCreateWithCString(atomicSymbol[index]);
        if(OCStringCompare(elementSymbol, key, 0)==kOCCompareEqualTo) {
            OCRelease(key);
            return true;
        }
        OCRelease(key);
    }
    return false;
}

