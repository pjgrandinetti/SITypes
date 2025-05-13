//
//  main.c
//  SILibTests
//
//  Created by Philip Grandinetti on 6/13/17.
//  Copyright © 2017 PhySy Ltd. All rights reserved.
//

#define PRINTERROR printf("failure: line %d, %s\n",__LINE__,__FUNCTION__)

#include "SILibrary.h"

bool dimensionalityTest0(void);
bool dimensionalityTest1(void);
bool dimensionalityTest2(void);
bool unitTest0(void);
bool unitTest1(void);
bool unitTest3(void);
bool unitTest4(void);
bool unitTest5(void);
bool scalarTest1(void);
bool scalarTest2(void);

int main(int argc, const char * argv[]) {
    dimensionalityTest0();
    dimensionalityTest1();
    dimensionalityTest2();

    unitTest0();
    unitTest1();
    unitTest3();
    unitTest4();
    unitTest5();

    scalarTest1();
    scalarTest2();
}

bool dimensionalityTest0(void)
{
    OCStringRef errorString = NULL;
    fprintf(stderr,"%s begin...\n",__func__);

    SIDimensionalityRef dimensionality1 = SIDimensionalityForSymbol(STR("L"), &errorString);
    OCStringRef symbol = SIDimensionalityGetSymbol(dimensionality1);
    if(OCStringCompare(symbol, STR("L"), 0)!=kOCCompareEqualTo) PRINTERROR;
    
    SIDimensionalityRef dimensionality2 = SIDimensionalityWithBaseDimensionSymbol(STR("L"), &errorString);
    if(OCStringCompare(SIDimensionalityGetSymbol(dimensionality1), SIDimensionalityGetSymbol(dimensionality2), 0)!=kOCCompareEqualTo) PRINTERROR;
    
    if(SIDimensionalityGetNumeratorExponentAtIndex(dimensionality1, kSILengthIndex)!=1) PRINTERROR;
    if(SIDimensionalityGetNumeratorExponentAtIndex(dimensionality1, kSIMassIndex)!=0) PRINTERROR;
    if(SIDimensionalityGetNumeratorExponentAtIndex(dimensionality1, kSITimeIndex)!=0) PRINTERROR;
    if(SIDimensionalityGetNumeratorExponentAtIndex(dimensionality1, kSICurrentIndex)!=0) PRINTERROR;
    if(SIDimensionalityGetNumeratorExponentAtIndex(dimensionality1, kSITemperatureIndex)!=0) PRINTERROR;
    if(SIDimensionalityGetNumeratorExponentAtIndex(dimensionality1, kSIAmountIndex)!=0) PRINTERROR;
    if(SIDimensionalityGetNumeratorExponentAtIndex(dimensionality1, kSILuminousIntensityIndex)!=0) PRINTERROR;
    
    if(SIDimensionalityGetDenominatorExponentAtIndex(dimensionality1, kSILengthIndex)!=0) PRINTERROR;
    if(SIDimensionalityGetDenominatorExponentAtIndex(dimensionality1, kSIMassIndex)!=0) PRINTERROR;
    if(SIDimensionalityGetDenominatorExponentAtIndex(dimensionality1, kSITimeIndex)!=0) PRINTERROR;
    if(SIDimensionalityGetDenominatorExponentAtIndex(dimensionality1, kSICurrentIndex)!=0) PRINTERROR;
    if(SIDimensionalityGetDenominatorExponentAtIndex(dimensionality1, kSITemperatureIndex)!=0) PRINTERROR;
    if(SIDimensionalityGetDenominatorExponentAtIndex(dimensionality1, kSIAmountIndex)!=0) PRINTERROR;
    if(SIDimensionalityGetDenominatorExponentAtIndex(dimensionality1, kSILuminousIntensityIndex)!=0) PRINTERROR;
    
    if(SIDimensionalityReducedExponentAtIndex(dimensionality1, kSILengthIndex)!=1) PRINTERROR;
    if(SIDimensionalityReducedExponentAtIndex(dimensionality1, kSIMassIndex)!=0) PRINTERROR;
    if(SIDimensionalityReducedExponentAtIndex(dimensionality1, kSITimeIndex)!=0) PRINTERROR;
    if(SIDimensionalityReducedExponentAtIndex(dimensionality1, kSICurrentIndex)!=0) PRINTERROR;
    if(SIDimensionalityReducedExponentAtIndex(dimensionality1, kSITemperatureIndex)!=0) PRINTERROR;
    if(SIDimensionalityReducedExponentAtIndex(dimensionality1, kSIAmountIndex)!=0) PRINTERROR;
    if(SIDimensionalityReducedExponentAtIndex(dimensionality1, kSILuminousIntensityIndex)!=0) PRINTERROR;
    
    SIDimensionalityRef length = SIDimensionalityForBaseDimensionIndex(kSILengthIndex);
    if(OCStringCompare(SIDimensionalityGetSymbol(length), STR("L"), 0)!=kOCCompareEqualTo) PRINTERROR;
    
    SIDimensionalityRef mass = SIDimensionalityForBaseDimensionIndex(kSIMassIndex);
    if(OCStringCompare(SIDimensionalityGetSymbol(mass), STR("M"), 0)!=kOCCompareEqualTo) PRINTERROR;
    
    SIDimensionalityRef time = SIDimensionalityForBaseDimensionIndex(kSITimeIndex);
    if(OCStringCompare(SIDimensionalityGetSymbol(time), STR("T"), 0)!=kOCCompareEqualTo) PRINTERROR;
    
    SIDimensionalityRef current = SIDimensionalityForBaseDimensionIndex(kSICurrentIndex);
    if(OCStringCompare(SIDimensionalityGetSymbol(current), STR("I"), 0)!=kOCCompareEqualTo) PRINTERROR;
    
    SIDimensionalityRef temperature = SIDimensionalityForBaseDimensionIndex(kSITemperatureIndex);
    if(OCStringCompare(SIDimensionalityGetSymbol(temperature), STR("ϴ"), 0)!=kOCCompareEqualTo) PRINTERROR;
    
    SIDimensionalityRef amount = SIDimensionalityForBaseDimensionIndex(kSIAmountIndex);
    if(OCStringCompare(SIDimensionalityGetSymbol(amount), STR("N"), 0)!=kOCCompareEqualTo) PRINTERROR;
    
    SIDimensionalityRef luminousIntensity = SIDimensionalityForBaseDimensionIndex(kSILuminousIntensityIndex);
    if(OCStringCompare(SIDimensionalityGetSymbol(luminousIntensity), STR("J"), 0)!=kOCCompareEqualTo) PRINTERROR;
    
    if(SIDimensionalityIsDimensionless(length)) PRINTERROR;
    if(SIDimensionalityIsDimensionless(mass)) PRINTERROR;
    if(SIDimensionalityIsDimensionless(time)) PRINTERROR;
    if(SIDimensionalityIsDimensionless(current)) PRINTERROR;
    if(SIDimensionalityIsDimensionless(temperature)) PRINTERROR;
    if(SIDimensionalityIsDimensionless(amount)) PRINTERROR;
    if(SIDimensionalityIsDimensionless(luminousIntensity)) PRINTERROR;
    
    SIDimensionalityRef dimensionless = SIDimensionalityDimensionless();
    if(!SIDimensionalityIsDimensionless(dimensionless)) PRINTERROR;
    
    SIDimensionalityRef radians = SIDimensionalityForSymbol(STR("L/L"), &errorString);
    if(!SIDimensionalityIsDimensionless(radians)) PRINTERROR;
    
    SIDimensionalityRef steradians = SIDimensionalityForSymbol(STR("L^2/L^2"), &errorString);
    if(!SIDimensionalityIsDimensionless(steradians)) PRINTERROR;
    
    if(SIDimensionalityIsDerived(length)) PRINTERROR;
    if(SIDimensionalityIsDerived(mass)) PRINTERROR;
    if(SIDimensionalityIsDerived(time)) PRINTERROR;
    if(SIDimensionalityIsDerived(current)) PRINTERROR;
    if(SIDimensionalityIsDerived(temperature)) PRINTERROR;
    if(SIDimensionalityIsDerived(amount)) PRINTERROR;
    if(SIDimensionalityIsDerived(luminousIntensity)) PRINTERROR;
    
    if(!SIDimensionalityIsDerived(radians)) PRINTERROR;
    if(!SIDimensionalityIsDerived(steradians)) PRINTERROR;
    if(SIDimensionalityIsDerived(dimensionless)) PRINTERROR;
    
    SIDimensionalityRef force = SIDimensionalityForSymbol(STR("M*L/T^2"), &errorString);
    if(!SIDimensionalityIsDerived(force)) PRINTERROR;
    
    if(!SIDimensionalityIsDimensionlessAndDerived(radians)) PRINTERROR;
    if(SIDimensionalityIsDimensionlessAndNotDerived(radians)) PRINTERROR;
    
    if(!SIDimensionalityIsDimensionlessAndDerived(steradians)) PRINTERROR;
    if(SIDimensionalityIsDimensionlessAndNotDerived(steradians)) PRINTERROR;
    
    if(SIDimensionalityIsDimensionlessAndDerived(dimensionless)) PRINTERROR;
    if(!SIDimensionalityIsDimensionlessAndNotDerived(dimensionless)) PRINTERROR;
    
    if(!SIDimensionalityIsBaseDimensionality(length)) PRINTERROR;
    if(!SIDimensionalityIsBaseDimensionality(mass)) PRINTERROR;
    if(!SIDimensionalityIsBaseDimensionality(time)) PRINTERROR;
    if(!SIDimensionalityIsBaseDimensionality(current)) PRINTERROR;
    if(!SIDimensionalityIsBaseDimensionality(temperature)) PRINTERROR;
    if(!SIDimensionalityIsBaseDimensionality(amount)) PRINTERROR;
    if(!SIDimensionalityIsBaseDimensionality(luminousIntensity)) PRINTERROR;
    
    if(SIDimensionalityIsBaseDimensionality(dimensionless)) PRINTERROR;
    if(SIDimensionalityIsBaseDimensionality(steradians)) PRINTERROR;
    if(SIDimensionalityIsBaseDimensionality(radians)) PRINTERROR;
    if(SIDimensionalityIsBaseDimensionality(force)) PRINTERROR;
    
    if(!SIDimensionalityHasSameReducedDimensionality(steradians,radians)) PRINTERROR;
    if(!SIDimensionalityHasSameReducedDimensionality(steradians,dimensionless)) PRINTERROR;
    
    SIDimensionalityRef lengthQuantity = SIDimensionalityForQuantity(kSIQuantityLength, &errorString);
    if(!OCTypeEqual(length, lengthQuantity)) PRINTERROR;
    
    SIDimensionalityRef massQuantity = SIDimensionalityForQuantity(kSIQuantityMass, &errorString);
    if(!OCTypeEqual(mass, massQuantity)) PRINTERROR;
    
    SIDimensionalityRef timeQuantity = SIDimensionalityForQuantity(kSIQuantityTime, &errorString);
    if(!OCTypeEqual(time, timeQuantity)) PRINTERROR;
    
    SIDimensionalityRef currentQuantity = SIDimensionalityForQuantity(kSIQuantityCurrent, &errorString);
    if(!OCTypeEqual(current, currentQuantity)) PRINTERROR;
    
    SIDimensionalityRef temperatureQuantity = SIDimensionalityForQuantity(kSIQuantityTemperature, &errorString);
    if(!OCTypeEqual(temperature, temperatureQuantity)) PRINTERROR;
    
    SIDimensionalityRef amountQuantity = SIDimensionalityForQuantity(kSIQuantityAmount, &errorString);
    if(!OCTypeEqual(amount, amountQuantity)) PRINTERROR;
    
    SIDimensionalityRef luminousIntensityQuantity = SIDimensionalityForQuantity(kSIQuantityLuminousIntensity, &errorString);
    if(!OCTypeEqual(luminousIntensity, luminousIntensityQuantity)) PRINTERROR;
    
    // Oh boy!  Someone needs to write tests to make sure all quantity strings return the correct dimensionality.
    SIDimensionalityRef dimensionlessQuantity = SIDimensionalityForQuantity(kSIQuantityDimensionless, &errorString);
    if(!OCTypeEqual(dimensionless, dimensionlessQuantity)) PRINTERROR;
    
    SIDimensionalityRef forceQuantity = SIDimensionalityForQuantity(kSIQuantityForce, &errorString);
    if(!OCTypeEqual(force, forceQuantity)) PRINTERROR;
    
    SIDimensionalityRef radiansQuantity = SIDimensionalityForQuantity(kSIQuantityPlaneAngle, &errorString);
    if(!OCTypeEqual(radians, radiansQuantity)) PRINTERROR;
    
    SIDimensionalityRef steradiansQuantity = SIDimensionalityForQuantity(kSIQuantitySolidAngle, &errorString);
    if(!OCTypeEqual(steradians, steradiansQuantity)) PRINTERROR;
    
    SIDimensionalityRef inverseLength = SIDimensionalityForSymbol(STR("1/L"), &errorString);
    SIDimensionalityRef inverseLengthQuantity = SIDimensionalityForQuantity(kSIQuantityInverseLength, &errorString);
    if(!OCTypeEqual(inverseLength, inverseLengthQuantity)) PRINTERROR;
    
    SIDimensionalityRef inverseMass = SIDimensionalityForSymbol(STR("1/M"), &errorString);
    SIDimensionalityRef inverseMassQuantity = SIDimensionalityForQuantity(kSIQuantityInverseMass, &errorString);
    if(!OCTypeEqual(inverseMass, inverseMassQuantity)) PRINTERROR;

    fprintf(stderr,"%s end...without problems\n",__func__);
    return true;
}

bool dimensionalityTest1(void)
{
    OCStringRef errorString = NULL;
    
    fprintf(stderr,"%s begin...\n",__func__);
    
    // These are older tests included for fun.
    
    OCStringRef symbol = STR("L•M^2•T^3•I^4•ϴ^5•N^6•J^7/(L^2•M^3•T^4•I^5•ϴ^6•N^7•J^8)");
    SIDimensionalityRef dimensionality = SIDimensionalityForSymbol(symbol, &errorString);
    if(dimensionality==NULL) PRINTERROR;
    if(SIDimensionalityGetNumeratorExponentAtIndex(dimensionality, kSILengthIndex) != 1) PRINTERROR;
    if(SIDimensionalityGetDenominatorExponentAtIndex(dimensionality, kSILengthIndex) != 2) PRINTERROR;
    if(SIDimensionalityReducedExponentAtIndex(dimensionality, kSILengthIndex) != -1) PRINTERROR;
    
    if(SIDimensionalityGetNumeratorExponentAtIndex(dimensionality, kSIMassIndex) != 2) PRINTERROR;
    if(SIDimensionalityGetDenominatorExponentAtIndex(dimensionality, kSIMassIndex) != 3) PRINTERROR;
    if(SIDimensionalityReducedExponentAtIndex(dimensionality, kSIMassIndex) != -1) PRINTERROR;
    
    if(SIDimensionalityGetNumeratorExponentAtIndex(dimensionality, kSITimeIndex) != 3) PRINTERROR;
    if(SIDimensionalityGetDenominatorExponentAtIndex(dimensionality, kSITimeIndex) != 4) PRINTERROR;
    if(SIDimensionalityReducedExponentAtIndex(dimensionality, kSITimeIndex) != -1) PRINTERROR;
    
    if(SIDimensionalityGetNumeratorExponentAtIndex(dimensionality, kSICurrentIndex) != 4) PRINTERROR;
    if(SIDimensionalityGetDenominatorExponentAtIndex(dimensionality, kSICurrentIndex) != 5) PRINTERROR;
    if(SIDimensionalityReducedExponentAtIndex(dimensionality, kSICurrentIndex) != -1) PRINTERROR;
    
    if(SIDimensionalityGetNumeratorExponentAtIndex(dimensionality, kSITemperatureIndex) != 5) PRINTERROR;
    if(SIDimensionalityGetDenominatorExponentAtIndex(dimensionality, kSITemperatureIndex) != 6) PRINTERROR;
    if(SIDimensionalityReducedExponentAtIndex(dimensionality, kSITemperatureIndex) != -1) PRINTERROR;
    
    if(SIDimensionalityGetNumeratorExponentAtIndex(dimensionality, kSIAmountIndex) != 6) PRINTERROR;
    if(SIDimensionalityGetDenominatorExponentAtIndex(dimensionality, kSIAmountIndex) != 7) PRINTERROR;
    if(SIDimensionalityReducedExponentAtIndex(dimensionality, kSIAmountIndex) != -1) PRINTERROR;
    
    if(SIDimensionalityGetNumeratorExponentAtIndex(dimensionality, kSILuminousIntensityIndex) != 7) PRINTERROR;
    if(SIDimensionalityGetDenominatorExponentAtIndex(dimensionality, kSILuminousIntensityIndex) != 8) PRINTERROR;
    if(SIDimensionalityReducedExponentAtIndex(dimensionality, kSILuminousIntensityIndex) != -1) PRINTERROR;
    
    OCStringRef derivedSymbol = SIDimensionalityGetSymbol(dimensionality);
    if(OCStringCompare(symbol, derivedSymbol, 0) != kOCCompareEqualTo) PRINTERROR;
    
    if(!SIDimensionalityEqual(dimensionality,SIDimensionalityForSymbol(derivedSymbol, &errorString))) PRINTERROR;
    
    if(SIDimensionalityIsDimensionless(dimensionality)) PRINTERROR;
    if(!SIDimensionalityIsDerived(dimensionality)) PRINTERROR;
    if(SIDimensionalityIsDimensionlessAndNotDerived(dimensionality)) PRINTERROR;
    if(SIDimensionalityIsDimensionlessAndDerived(dimensionality)) PRINTERROR;
    if(SIDimensionalityIsBaseDimensionality(dimensionality)) PRINTERROR;
    
    SIDimensionalityRef reducedDimensionality = SIDimensionalityForSymbol(STR("1/(L•M•T•I•ϴ•N•J)"), &errorString);
    if(!SIDimensionalityHasSameReducedDimensionality(dimensionality,reducedDimensionality)) PRINTERROR;
    if(!SIDimensionalityHasReducedExponents(dimensionality,-1,-1,-1,-1,-1,-1,-1)) PRINTERROR;
    
    fprintf(stderr,"%s end...without problems\n",__func__);
    return true;
}

bool dimensionalityTest2(void)
{
    // These are older tests included for fun.

    fprintf(stderr,"%s begin...\n",__func__);
    OCStringRef errorString = NULL;
    
    SIDimensionalityRef force = SIDimensionalityForQuantity(kSIQuantityForce, &errorString);
    SIDimensionalityRef acceleration = SIDimensionalityForQuantity(kSIQuantityAcceleration, &errorString);
    SIDimensionalityRef dimensionality = SIDimensionalityByDividing(force,acceleration);
    SIDimensionalityRef mass = SIDimensionalityForQuantity(kSIQuantityMass, &errorString);
    
    if(!SIDimensionalityEqual(dimensionality,mass)) PRINTERROR;
    
    SIDimensionalityRef distance = SIDimensionalityForQuantity(kSIQuantityLength, &errorString);
    SIDimensionalityRef work = SIDimensionalityForQuantity(kSIQuantityEnergy, &errorString);
    dimensionality = SIDimensionalityByMultiplying(force, distance, &errorString);
    if(!SIDimensionalityEqual(dimensionality,work)) PRINTERROR;
    
    SIDimensionalityRef area = SIDimensionalityForQuantity(kSIQuantityArea, &errorString);
    dimensionality = SIDimensionalityByRaisingToAPower(distance, 2,  &errorString);
    if(!SIDimensionalityEqual(dimensionality,area)) PRINTERROR;
    
    SIDimensionalityRef angle = SIDimensionalityForQuantity(kSIQuantityPlaneAngle, &errorString);
    dimensionality = SIDimensionalityByDividingWithoutReducing(distance,distance);
    if(!SIDimensionalityEqual(dimensionality,angle)) PRINTERROR;
    
    SIDimensionalityRef solidAngle = SIDimensionalityForQuantity(kSIQuantitySolidAngle, &errorString);
    dimensionality = SIDimensionalityByDividingWithoutReducing(area,area);
    if(!SIDimensionalityEqual(dimensionality,solidAngle)) PRINTERROR;
    
    SIDimensionalityRef dimensionless = SIDimensionalityForQuantity(kSIQuantityDimensionless, &errorString);
    dimensionality = SIDimensionalityByDividing(distance,distance);
    if(!SIDimensionalityEqual(dimensionality,dimensionless)) PRINTERROR;
    
    dimensionality = SIDimensionalityByDividing(area,area);
    if(!SIDimensionalityEqual(dimensionality,dimensionless)) PRINTERROR;
    
    OCStringRef symbol = STR("L^2*M^2*T^2*I/(M*T^4*I)");
    dimensionality = SIDimensionalityForSymbol(symbol, &errorString);
    
    OCArrayRef array  = SIDimensionalityCreateArrayWithSameReducedDimensionality(dimensionality);
    for(uint64_t index=0;index<OCArrayGetCount(array); index++) {
        SIDimensionalityRef same_dimensionality = OCArrayGetValueAtIndex(array, index);
        if(!SIDimensionalityHasSameReducedDimensionality(same_dimensionality, dimensionality)) PRINTERROR;
    }
    
    fprintf(stderr,"%s end...without problems\n",__func__);
    return true;
}


bool unitTest0(void)
{
    fprintf(stderr,"%s begin...\n",__func__);
    OCStringRef errorString = NULL;
    
    SIUnitRef unit = SIUnitForParsedSymbol(STR("m•kg^2•s^3•A^4•K^5•mol^6•cd^7/(m^2•kg^3•s^4•A^5•K^6•mol^7•cd^8)"), NULL, &errorString);
    OCStringRef plist = SIUnitCopySymbol(unit);
    SIUnitRef unit2 = SIUnitForParsedSymbol(plist, NULL, &errorString);
    if(!OCTypeEqual(unit, unit2)) PRINTERROR;
    
    fprintf(stderr,"%s end...without problems\n",__func__);
    return true;
}

bool unitTest1(void)
{
    OCStringRef errorString = NULL;
    
    fprintf(stderr,"%s begin...\n",__func__);
    
    
    SIDimensionalityRef dimensionality = SIDimensionalityForSymbol(STR("M/(L*T^2)"), &errorString);
    if(dimensionality==NULL) PRINTERROR;
    
    SIUnitRef unit = SIUnitForUnderivedSymbol(STR("bar"));
    
    if(unit==NULL) PRINTERROR;
    
    double multiplier = 1.0;

    SIUnitRef foundUnit = SIUnitForParsedSymbol(STR("R_∞"),&multiplier, &errorString);

    fprintf(stderr,"%s end...without problems\n",__func__);
    return true;
}

bool unitTest3(void)
{
    fprintf(stderr,"%s begin...\n",__func__);
    
    SIUnitRef unit;
    OCStringRef root_name;
    
    unit = SIUnitForUnderivedSymbol(STR("N"));
    if(unit==NULL) PRINTERROR;
    
    unit = SIUnitForUnderivedSymbol(STR("m"));
    if(unit==NULL) PRINTERROR;
    root_name = SIUnitCopyRootSymbol(unit);
    if(OCStringCompare(root_name, STR("m"), 0)!=0) PRINTERROR;
    
    unit = SIUnitForUnderivedSymbol(STR("g"));
    if(unit==NULL) PRINTERROR;
    root_name = SIUnitCopyRootSymbol(unit);
    if(OCStringCompare(root_name, STR("g"), 0)!=0) PRINTERROR;
    
    unit = SIUnitForUnderivedSymbol(STR("s"));
    if(unit==NULL) PRINTERROR;
    root_name = SIUnitCopyRootSymbol(unit);
    if(OCStringCompare(root_name, STR("s"), 0)!=0) PRINTERROR;
    
    unit = SIUnitForUnderivedSymbol(STR("A"));
    if(unit==NULL) PRINTERROR;
    root_name = SIUnitCopyRootSymbol(unit);
    if(OCStringCompare(root_name, STR("A"), 0)!=0) PRINTERROR;
    
    unit = SIUnitForUnderivedSymbol(STR("K"));
    if(unit==NULL) PRINTERROR;
    root_name = SIUnitCopyRootSymbol(unit);
    if(OCStringCompare(root_name, STR("K"), 0)!=0) PRINTERROR;
    
    unit = SIUnitForUnderivedSymbol(STR("mol"));
    if(unit==NULL) PRINTERROR;
    root_name = SIUnitCopyRootSymbol(unit);
    if(OCStringCompare(root_name, STR("mol"), 0)!=0) PRINTERROR;
    
    unit = SIUnitForUnderivedSymbol(STR("cd"));
    if(unit==NULL) PRINTERROR;
    root_name = SIUnitCopyRootSymbol(unit);
    if(OCStringCompare(root_name, STR("cd"), 0)!=0) PRINTERROR;
    
    fprintf(stderr,"%s end...without problems\n",__func__);
    return true;
}

bool unitTest4(void)
{
    fprintf(stderr,"%s begin...\n",__func__);
    OCStringRef errorString = NULL;
    
    SIUnitRef unit1 = SIUnitForUnderivedSymbol(STR("cm"));
    SIUnitRef unit2 = SIUnitForUnderivedSymbol(STR("ms"));
    SIUnitRef unit3 = SIUnitForParsedSymbol(STR("cm*ms"),NULL, &errorString);
    SIUnitRef unit4 = SIUnitByMultiplying(unit1, unit2, NULL,&errorString);
    if(!OCTypeEqual(unit3, unit4)) PRINTERROR;
    
    fprintf(stderr,"%s end...without problems\n",__func__);
    return true;
}

bool unitTest5(void)
{
    fprintf(stderr,"%s begin...\n",__func__);
    OCStringRef errorString = NULL;
    
    OCDictionaryRef unitsDictionary = SIUnitGetLibrary();
    OCArrayRef values = OCDictionaryCreateArrayWithAllValues(unitsDictionary);
    
    for(uint64_t i=0;i<OCArrayGetCount(values);i++) {
        SIUnitRef libUnit = (SIUnitRef) OCArrayGetValueAtIndex(values, i);
        OCStringRef derivedSymbol = SIUnitCopySymbol(libUnit);
        double multiplier = 1.0;
        SIUnitRef foundUnit = SIUnitForSymbol(derivedSymbol,&multiplier, &errorString);
        OCRelease(derivedSymbol);
        if(!SIUnitAreEquivalentUnits(foundUnit, libUnit)) {
            fprintf(stderr,"\n");
            SIUnitShow(libUnit);
            fprintf(stderr,"\n");
            OCStringRef derivedSymbol = SIUnitCopySymbol(libUnit);
            OCStringShow(derivedSymbol);
            fprintf(stderr,"\n");
            
            double multiplier = 1.0;
            SIUnitRef foundUnit = SIUnitForSymbol(derivedSymbol,&multiplier, &errorString);
            SIUnitShow(foundUnit);
            OCRelease(derivedSymbol);
            PRINTERROR;
        }
    }
    fprintf(stderr,"%s end...without problems\n",__func__);
    
    return true;
}

bool scalarTest1(void)
{
    fprintf(stderr,"%s begin...\n",__func__);
    OCStringRef errorString = NULL;
    
    // Valid
    errorString = NULL;
    SIScalarRef scalar = SIScalarCreateWithOCString(STR("4.3 eV"),&errorString);
    if(!scalar) return false;
    SIScalarRef temp = SIScalarCreateWithFloatComplex(4.3, SIUnitForUnderivedSymbol(STR("eV")));
    if(SIScalarCompare(scalar, temp)!=kOCCompareEqualTo) PRINTERROR;
    if(scalar) OCRelease(scalar);
    
    // Valid
    errorString = NULL;
    scalar = SIScalarCreateWithOCString(STR("4.3 eV^2"),&errorString);
    if(!scalar) PRINTERROR;
    
    double unit_multiplier = 1;
    errorString = NULL;
    SIUnitRef unit = SIUnitForSymbol(STR("eV^2"),&unit_multiplier, &errorString);
    temp = SIScalarCreateWithFloatComplex(4.3*unit_multiplier, unit);
    if(SIScalarCompare(scalar, temp)!=kOCCompareEqualTo) PRINTERROR;
    if(scalar) OCRelease(scalar);
    
    // Valid
    unit_multiplier = 1;
    errorString = NULL;
    unit = SIUnitForSymbol(STR("eV/N"),&unit_multiplier, &errorString);
    
    errorString = NULL;
    scalar = SIScalarCreateWithOCString(STR("4.3 Å"),&errorString);
    if(!scalar) PRINTERROR;
    
    unit_multiplier = 1;
    errorString = NULL;
    unit = SIUnitForSymbol(STR("Å"),&unit_multiplier, &errorString);
    temp = SIScalarCreateWithFloatComplex(4.3*unit_multiplier, unit);
    if(SIScalarCompare(scalar, temp)!=kOCCompareEqualTo) PRINTERROR;
    if(scalar) OCRelease(scalar);
    
    // Invalid, returns NULL
    errorString = NULL;
    scalar = SIScalarCreateWithOCString(STR("4.3e"),&errorString);
    if(scalar) PRINTERROR;
    if(scalar) OCRelease(scalar);
    
    // Invalid, returns NULL
    errorString = NULL;
    scalar = SIScalarCreateWithOCString(STR("4.3e+"),&errorString);
    if(scalar) PRINTERROR;
    if(scalar) OCRelease(scalar);
    
    // Valid
    errorString = NULL;
    scalar = SIScalarCreateWithOCString(STR("4.3e+3"),&errorString);
    if(!scalar) PRINTERROR;
    temp = SIScalarCreateWithFloatComplex(4.3e3, NULL);
    if(SIScalarCompare(scalar, temp)!=kOCCompareEqualTo) PRINTERROR;
    
    if(scalar) OCRelease(scalar);
    
    // Invalid, returns NULL
    errorString = NULL;
    scalar = SIScalarCreateWithOCString(STR("4.3e+mV"),&errorString);
    if(scalar) PRINTERROR;
    if(scalar) OCRelease(scalar);
    
    // Invalid, returns NULL
    errorString = NULL;
    scalar = SIScalarCreateWithOCString(STR("4.3eV+"),&errorString);
    if(scalar) PRINTERROR;
    if(scalar) OCRelease(scalar);
    
    // Invalid, returns NULL
    errorString = NULL;
    scalar = SIScalarCreateWithOCString(STR("4.3e-3*"),&errorString);
    if(scalar) PRINTERROR;
    if(scalar) OCRelease(scalar);
    
    // Valid
    errorString = NULL;
    scalar = SIScalarCreateWithOCString(STR("4.3e-3*I"),&errorString);
    if(!scalar) PRINTERROR;
    
    if(SIScalarIsReal(scalar)) PRINTERROR;
    temp = SIScalarCreateWithFloatComplex(4.3e-3*I, NULL);
    if(SIScalarCompare(scalar, temp)!=kOCCompareEqualTo) PRINTERROR;
    if(scalar) OCRelease(scalar);
    
    // Valid
    errorString = NULL;
    scalar = SIScalarCreateWithOCString(STR("4.3e-3 + 3.4e-3*I"),&errorString);
    if(!scalar) PRINTERROR;
    
    temp = SIScalarCreateWithFloatComplex(4.3e-3 + 3.4e-3*I, NULL);
    if(SIScalarCompare(scalar, temp)!=kOCCompareEqualTo) PRINTERROR;
    if(scalar) OCRelease(scalar);
    
    // Valid
    errorString = NULL;
    scalar = SIScalarCreateWithOCString(STR("4.3e-3*I + 3.4e-3"),&errorString);
    if(!scalar) PRINTERROR;
    
    temp = SIScalarCreateWithFloatComplex(4.3e-3*I + 3.4e-3, NULL);
    if(SIScalarCompare(scalar, temp)!=kOCCompareEqualTo) PRINTERROR;
    if(scalar) OCRelease(scalar);
    
    unit_multiplier = 1;
    errorString = NULL;
    temp = SIScalarCreateWithFloatComplex((4.3e-3*I + 3.4e-3)*unit_multiplier, SIUnitForSymbol(STR("eV"),&unit_multiplier, &errorString));
    if(SIScalarCompare(scalar, temp)!=kOCCompareEqualTo) PRINTERROR;
    if(scalar) OCRelease(scalar);
    
    errorString = NULL;
    SIScalarRef mass = SIScalarCreateWithOCString(STR("42.01 kg"),&errorString);
    
    errorString = NULL;
    SIScalarRef acceleration = SIScalarCreateWithOCString(STR("9.8 m/s^2"),&errorString);
    SIScalarRef force = SIScalarCreateByMultiplying(mass, acceleration,&errorString);
    
    unit_multiplier = 1;
    errorString = NULL;
    unit = SIUnitForSymbol(STR("N"),&unit_multiplier, &errorString);
    SIScalarRef newtons = SIScalarCreateByConvertingToUnit(force, unit,NULL);
    if(!SIScalarEqual(force, newtons)) PRINTERROR;
    
    fprintf(stderr,"%s end...without problems\n",__func__);
    return true;
}

bool scalarTest2(void)
{
    fprintf(stderr,"%s begin...\n",__func__);
    OCStringRef errorString = NULL;
    
    SIScalarRef force = SIScalarCreateWithOCString(STR("500 N"),NULL);
    SIScalarRef acceleration = SIScalarCreateWithOCString(STR("9.8 m/s^2"),NULL);
    SIScalarRef mass = SIScalarCreateByDividing(force, acceleration,NULL);
    SIScalarRef mass2 = SIScalarCreateWithOCString(STR("500 N * 9.8 m/s^2"),NULL);
    if(!SIScalarCompareReduced(mass, mass2)) PRINTERROR;
    
    SIScalarRef n = SIScalarCreateWithOCString(STR("1 mol"),NULL);
    SIScalarRef T = SIScalarCreateWithOCString(STR("298.15 K"),NULL);
    SIScalarRef V = SIScalarCreateWithOCString(STR("22.4 L"),NULL);
    SIScalarRef R = SIScalarCreateWithOCString(STR("8.3144621 J/(K*mol)"),NULL);
    
    double multiplier = 1.;
    SIUnitRef unit = SIUnitForSymbol(STR("L*atm/(K*mol)"), &multiplier, &errorString);
    unit = SIUnitByReducing(unit, &multiplier);
    SIScalarRef Ratm = SIScalarCreateByConvertingToUnit(R, unit,NULL);
    
    SIMutableScalarRef p = (SIMutableScalarRef) SIScalarCreateByMultiplying(n, R,&errorString);
    SIScalarMultiply(p, T,&errorString);
    SIScalarDivide(p, V,NULL);
    
    p = (SIMutableScalarRef) SIScalarCreateByMultiplying(n, Ratm,&errorString);
    SIScalarMultiply(p, T,&errorString);
    SIScalarDivide(p, V,NULL);
    
    SIUnitRef atmospheres = SIUnitFindWithName(STR("atmospheres"));
    SIScalarConvertToUnit((SIMutableScalarRef) p, atmospheres,NULL);
    
    OCRelease(n);
    OCRelease(T);
    OCRelease(V);
    OCRelease(R);
    OCRelease(p);
    
    
    SIScalarRef Joule = SIScalarCreateWithOCString(STR("1J"),NULL);
    SIScalarRef eV = SIScalarCreateWithOCString(STR("1eV"),NULL);
    
    SIScalarRef temp = SIScalarCreateByDividingWithoutReducingUnit(eV,Joule,NULL);
    SIScalarRef temp2 = SIScalarCreateByReducingUnit(temp);
    if(SIScalarCompareReduced(temp, temp2)!= kOCCompareEqualTo) return false;
    
    SIScalarRef mm = SIScalarCreateWithOCString(STR("0.01 mm"),NULL);
    SIScalarRef m = SIScalarCreateWithOCString(STR("1m"),NULL);
    
    SIScalarRef ratio = SIScalarCreateByDividingWithoutReducingUnit(mm,m,NULL);
    SIScalarRef ratio2 = SIScalarCreateByReducingUnit(ratio);
    if(SIScalarCompareReduced(ratio, ratio2)!= kOCCompareEqualTo) PRINTERROR;
    
    
    SIScalarRef speed = SIScalarCreateWithOCString(STR("88 km/h"),NULL);
    double unit_multiplier = 1;
    SIUnitRef kph = SIUnitForSymbol(STR("km/h"), &unit_multiplier, NULL);
    SIScalarRef newSpeed = SIScalarCreateByConvertingToUnit(speed, kph, NULL);
    if(OCCompareDoubleValues(88, SIScalarDoubleValue(newSpeed))!= kOCCompareEqualTo) PRINTERROR;
    OCRelease(speed);
    OCRelease(newSpeed);
    
    fprintf(stderr,"%s end...without problems\n",__func__);
    return true;
}
