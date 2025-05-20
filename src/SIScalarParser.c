//
//  SIScalarParser.c
//
//  Created by PhySy Ltd on 5/3/13.
//  Copyright (c) 2008-2014 PhySy Ltd. All rights reserved.
//

#include "SILibrary.h"
#include <stdio.h> // For yyerror, fprintf, stderr
#include <math.h> // For floor, isnan, erf, erfc, log
#include <stdlib.h> // For malloc, free
#include "SIScalarConstants.h"
#include "SIScalarParser.h"

struct __symbol {
    char *name;
    SIScalarRef value;
} __symbol;

#define NHASH 9997
struct __symbol symbolTable[NHASH];

struct __scalarNode {
    int nodeType;
    ScalarNodeRef left;
    ScalarNodeRef right;
}  __scalarNode;

struct __scalarValue {
    int nodeType;
    SIScalarRef number;
} __scalarValue;

struct __scalarNodeMathFunction {
    int nodeType;
    ScalarNodeRef left;
    builtInMathFunctions funcType;
} __scalarNodeMathFunction;

struct __scalarNodeConstantFunction {
    int nodeType;
    builtInConstantFunctions funcType;
    OCMutableStringRef string;
} __scalarNodeConstantFunction;


SIScalarRef ScalarNodeEvaluate(ScalarNodeRef node, OCStringRef *errorString)
{
    if(errorString) if(*errorString) return NULL;
    
    switch(node->nodeType) {
        case 'K': {
            NumberRef leaf = (NumberRef) node;
            return leaf->number;
        }
        case '+': {
            SIScalarRef left = ScalarNodeEvaluate(node->left, errorString);
            if(errorString) if(*errorString) return NULL;
            SIScalarRef right = ScalarNodeEvaluate(node->right, errorString);
            if(errorString) if(*errorString) return NULL;
            SIScalarRef temp = SIScalarCreateByAdding(left, right, errorString);
            if(errorString) if(*errorString) return NULL;
            if(temp) return temp;
            return NULL;
        }
        case '-': {
            SIScalarRef left = ScalarNodeEvaluate(node->left,errorString);
            if(errorString) if(*errorString) return NULL;
            SIScalarRef right = ScalarNodeEvaluate(node->right,errorString);
            if(errorString) if(*errorString) return NULL;
            SIScalarRef temp = SIScalarCreateBySubtracting(left, right, errorString);
            if(errorString) if(*errorString) return NULL;
            if(temp) return temp;
            return NULL;
        }
        case '*': {
            SIScalarRef left = ScalarNodeEvaluate(node->left,errorString);
            if(errorString) if(*errorString) return NULL;
            SIScalarRef right = ScalarNodeEvaluate(node->right,errorString);
            if(errorString) if(*errorString) return NULL;
            SIScalarRef temp = SIScalarCreateByMultiplyingWithoutReducingUnit(left, right, errorString);
            if(temp) return temp;
            return NULL;
        }
        case '/': {
            SIScalarRef left = ScalarNodeEvaluate(node->left,errorString);
            if(errorString) if(*errorString) return NULL;
            SIScalarRef right = ScalarNodeEvaluate(node->right,errorString);
            if(errorString) if(*errorString) return NULL;
            SIScalarRef temp = SIScalarCreateByDividingWithoutReducingUnit(left, right, errorString);
            if(temp) return temp;
            return NULL;
        }
        case '!': {
            SIScalarRef left = ScalarNodeEvaluate(node->left,errorString);
            if(errorString) if(*errorString) return NULL;
            SIScalarRef temp = SIScalarCreateByGammaFunctionWithoutReducingUnit(left, errorString);
            if(temp) return temp;
            return NULL;
        }
        case '^': {
            SIScalarRef left = ScalarNodeEvaluate(node->left,errorString);
            if(errorString) if(*errorString) return NULL;
            SIScalarRef right = ScalarNodeEvaluate(node->right,errorString);
            if(errorString) if(*errorString) return NULL;
            if(SIUnitIsDimensionless(SIQuantityGetUnit((SIQuantityRef) right))) {
                SIScalarReduceUnit((SIMutableScalarRef) right);
                double complex power = SIScalarDoubleComplexValue(right);
                SIUnitRef argumentUnit = SIQuantityGetUnit((SIQuantityRef) left);
                if(SIUnitIsDimensionless(argumentUnit) && SIUnitGetScaleNonSIToCoherentSI(argumentUnit) == 1.0) {
                    SIScalarReduceUnit((SIMutableScalarRef) left);
                    double complex x = SIScalarDoubleComplexValue(left);
                    if(cimag(power) == 0 && creal(power) == floor(creal(power))) { // ensure power is an integer
                        double complex result = raise_to_integer_power(x,  (long) creal(power)); // Reverted to raise_to_integer_power
                        if(isnan(creal(result)) && isnan(cimag(result))) { // Check both parts for NaN
                            if(errorString) {
                                *errorString = STR("Overflow.");
                            }
                            
                            return NULL;
                        }
                        return SIScalarCreateWithDoubleComplex(result, NULL);
                    }
                    else {
                        double complex result = cpow(x, power);
                        if(isnan(creal(result)) && isnan(cimag(result))) { // Check both parts for NaN
                            if(errorString) {
                                *errorString = STR("Overflow.");
                            }
                            
                            return NULL;
                        }
                        return SIScalarCreateWithDoubleComplex(result, NULL);
                    }
                }
                else {
                    if(SIScalarIsReal(right)) {
                        return  SIScalarCreateByRaisingToAPowerWithoutReducingUnit(left, power, errorString);
                    }
                    else {
                        if(errorString) {
                            *errorString = STR("Powers must be real.");
                        }
                        return NULL;
                        
                    }
                }
            }
            if(errorString) {
                *errorString = STR("Powers must be dimensionless.");
            }
            return NULL;
        }
        case '|': {
            SIScalarRef left = ScalarNodeEvaluate(node->left,errorString);
            if(errorString) if(*errorString) return NULL;
            return SIScalarCreateByTakingComplexPart(left, kSIMagnitudePart);
        }
        case 'M': {
            SIScalarRef left = ScalarNodeEvaluate(node->left,errorString);
            if(errorString) if(*errorString) return NULL;
            return SIScalarCreateByMultiplyingByDimensionlessRealConstant(left, -1.);
        }
        case 'F': {
            SIScalarRef result = builtInMathFunction((ScalarNodeMathFunctionRef) node, errorString);
            if(errorString) if(*errorString) return NULL;
            return result;
        }
        case 'C': {
            SIScalarRef result = builtInConstantFunction((ScalarNodeConstantFunctionRef) node, errorString);
            if(errorString) if(*errorString) return NULL;
            return result;
        }
        default:
            return NULL;
    }
    return 0;
}

ScalarNodeRef ScalarNodeCreateInnerNode(int nodeType, ScalarNodeRef left, ScalarNodeRef right)
{
    struct __scalarNode *node = malloc(sizeof(struct __scalarNode));
    node->nodeType = nodeType;
    node->left = left;
    node->right = right;
    return node;
}

ScalarNodeRef ScalarNodeCreateMathFunction(builtInMathFunctions funcType, ScalarNodeRef left)
{
    struct __scalarNodeMathFunction *node = malloc(sizeof(struct __scalarNodeMathFunction));
    node->nodeType = 'F';
    node->left = left;
    node->funcType = funcType;
    return (ScalarNodeRef) node;
}

ScalarNodeRef ScalarNodeCreateConstantFunction(builtInConstantFunctions funcType, OCMutableStringRef string)
{
    struct __scalarNodeConstantFunction *node = malloc(sizeof(struct __scalarNodeConstantFunction));
    node->nodeType = 'C';
    node->string = string;
    node->funcType = funcType;
    return (ScalarNodeRef) node;
}

ScalarNodeRef ScalarNodeCreateNumberLeaf(SIScalarRef number)
{
    struct __scalarValue *leaf = malloc(sizeof(struct __scalarValue));
    leaf->nodeType = 'K';
    leaf->number = number;
    return (ScalarNodeRef) leaf;
}

char ScalarNodeGetType(ScalarNodeRef node)
{
    return node->nodeType;
}

bool ScalarNodeisLeaf(ScalarNodeRef node)
{
    return (node->nodeType =='K');
}

void ScalarNodeFree(ScalarNodeRef node)
{
    if(node==NULL) return;
    
    switch(node->nodeType) {
        case '+':
        case '-':
        case '*':
        case '/':
        case '^':
            ScalarNodeFree(node->right);
        case '|':
        case 'M':
        case 'F':
        case '!':
            ScalarNodeFree(node->left);
            break;
        case 'K': {
            struct __scalarValue *leaf = (struct __scalarValue *) node;
            OCRelease(leaf->number);
            free((void *) node);
        }
    }
}

SIScalarRef builtInConstantFunction(ScalarNodeConstantFunctionRef func, OCStringRef *errorString)
{
    if(errorString) if(*errorString) return NULL;
    
    builtInConstantFunctions funcType = func->funcType;
    
    // Remove square brackets
    OCRange range;
    range.location = OCStringGetLength(func->string)-1;
    range.length = 1;
    OCStringDelete (func->string, range);
    range.location = 0;
    range.length = 1;
    OCStringDelete (func->string, range);
    
    switch (funcType) {
        case BC_AW: {
            return SIPeriodicTableCreateMolarMass(func->string, errorString);
            break;
        }
        case BC_FW: {
            return SIPeriodicTableCreateFormulaMass(func->string, errorString);
            break;
        }
        case BC_Isotope_Abundance:
            return SIPeriodicTableCreateIsotopeAbundance(func->string, errorString);
            break;
        case BC_Isotope_Spin:
            return SIPeriodicTableCreateIsotopeSpin(func->string, errorString);
            break;
        case BC_Isotope_HalfLife:
            return SIPeriodicTableCreateIsotopeHalfLife(func->string, errorString);
            break;
        case BC_Isotope_Gyromag:
            return SIPeriodicTableCreateIsotopeGyromagneticRatio(func->string, errorString);
            break;
        case BC_Isotope_MagneticDipole:
            return SIPeriodicTableCreateIsotopeMagneticDipoleMoment(func->string, errorString);
            break;
        case BC_Isotope_ElectricQuadrupole:
            return SIPeriodicTableCreateIsotopeElectricQuadrupoleMoment(func->string, errorString);
            break;
        case BC_nmr:
            return SIPeriodicTableCreateNMRFrequency(func->string, errorString);
            break;
        default:
            return NULL;
    }
    return NULL;
}

SIScalarRef builtInMathFunction(ScalarNodeMathFunctionRef func, OCStringRef *errorString)
{
    if(errorString) if(*errorString) return NULL;
    
    builtInMathFunctions funcType = func->funcType;
    SIScalarRef scalar = ScalarNodeEvaluate(func->left,errorString);
    if(NULL==scalar) return NULL;
    
    switch(funcType) {
        case BM_reduce: {
            return SIScalarCreateByReducingUnit(scalar);
        }
        case BM_sqrt: {
            return SIScalarCreateByTakingNthRoot(scalar,2,errorString);
        }
        case BM_cbrt: {
            return SIScalarCreateByTakingNthRoot(scalar,3,errorString);
        }
        case BM_qtrt: {
            return SIScalarCreateByTakingNthRoot(scalar,4,errorString);
        }
        case BM_exp: {
            if(SIDimensionalityIsDimensionless(SIQuantityGetUnitDimensionality((SIQuantityRef) scalar))) {
                SIScalarConvertToCoherentUnit((SIMutableScalarRef) scalar, errorString);
                double complex value = cexp(SIScalarDoubleComplexValue(scalar));
                return SIScalarCreateWithDoubleComplex(value, NULL);
            }
            if(errorString) {
                *errorString = STR("exp requires dimensionless unit.");
            }
            return NULL;
        }
        case BM_erf: {
            if(SIDimensionalityIsDimensionless(SIQuantityGetUnitDimensionality((SIQuantityRef) scalar))) {
                if(!SIScalarIsComplex(scalar)) {
                    SIScalarConvertToCoherentUnit((SIMutableScalarRef) scalar, errorString);
                    double complex value = erf(SIScalarDoubleValue(scalar));
                    return SIScalarCreateWithDoubleComplex(value, NULL);
                }
            }
            if(errorString) {
                *errorString = STR("erf requires dimensionless real quantity.");
            }
            return NULL;
        }
        case BM_erfc: {
            if(SIDimensionalityIsDimensionless(SIQuantityGetUnitDimensionality((SIQuantityRef) scalar))) {
                if(!SIScalarIsComplex(scalar)) {
                    SIScalarConvertToCoherentUnit((SIMutableScalarRef) scalar, errorString);
                    double complex value = erfc(SIScalarDoubleValue(scalar));
                    return SIScalarCreateWithDoubleComplex(value, NULL);
                }
            }
            if(errorString) {
                *errorString = STR("erfc requires dimensionless real quantity.");
                
            }
            return NULL;
        }
        case BM_ln: {
            if(SIDimensionalityIsDimensionless(SIQuantityGetUnitDimensionality((SIQuantityRef) scalar))) {
                SIScalarConvertToCoherentUnit((SIMutableScalarRef) scalar, errorString);
                double complex value = clog(SIScalarDoubleComplexValue(scalar));
                return SIScalarCreateWithDoubleComplex(value, NULL);
            }
            if(errorString) {
                *errorString = STR("ln requires dimensionless unit.");
            }
            
            return NULL;
        }
        case BM_log: {
            if(SIDimensionalityIsDimensionless(SIQuantityGetUnitDimensionality((SIQuantityRef) scalar))) {
                SIScalarConvertToCoherentUnit((SIMutableScalarRef) scalar, errorString);
                double complex value = clog(SIScalarDoubleComplexValue(scalar))/log(10);
                return SIScalarCreateWithDoubleComplex(value, NULL);
            }
            if(errorString) {
                *errorString = STR("ln requires dimensionless unit.");
            }
            
            return NULL;
        }
        case BM_acos: {
            if(SIDimensionalityIsDimensionless(SIQuantityGetUnitDimensionality((SIQuantityRef) scalar))) {
                SIScalarConvertToCoherentUnit((SIMutableScalarRef) scalar, errorString);
                double complex value = cacos(SIScalarDoubleComplexValue(scalar));
                SIUnitRef unit = SIUnitForUnderivedSymbol(STR("rad"));
                return SIScalarCreateWithDoubleComplex(value, unit);
            }
            if(errorString) {
                *errorString = STR("acos requires dimensionless unit.");
                
                return NULL;
            }
        case BM_acosh: {
            if(SIDimensionalityIsDimensionless(SIQuantityGetUnitDimensionality((SIQuantityRef) scalar))) {
                SIScalarConvertToCoherentUnit((SIMutableScalarRef) scalar, errorString);
                double complex value = cacosh(SIScalarDoubleComplexValue(scalar));
                SIUnitRef unit = SIUnitForUnderivedSymbol(STR("rad"));
                return SIScalarCreateWithDoubleComplex(value, unit);
            }
            if(errorString) {
                *errorString = STR("acosh requires dimensionless unit.");
            }
            
            return NULL;
        }
        case BM_asin: {
            if(SIDimensionalityIsDimensionless(SIQuantityGetUnitDimensionality((SIQuantityRef) scalar))) {
                SIScalarConvertToCoherentUnit((SIMutableScalarRef) scalar, errorString);
                double complex value = casin(SIScalarDoubleComplexValue(scalar));
                SIUnitRef unit = SIUnitForUnderivedSymbol(STR("rad"));
                return SIScalarCreateWithDoubleComplex(value, unit);
            }
            if(errorString) {
                *errorString = STR("asin requires dimensionless unit.");
            }
            
            return NULL;
        }
        case BM_asinh: {
            if(SIDimensionalityIsDimensionless(SIQuantityGetUnitDimensionality((SIQuantityRef) scalar))) {
                SIScalarConvertToCoherentUnit((SIMutableScalarRef) scalar, errorString);
                SIUnitRef unit = SIUnitForUnderivedSymbol(STR("rad"));
                double complex value = casinh(SIScalarDoubleComplexValue(scalar));
                return SIScalarCreateWithDoubleComplex(value, unit);
            }
            if(errorString) {
                *errorString = STR("asinh requires dimensionless unit.");
            }
            
            return NULL;
        }
        case BM_atan: {
            if(SIDimensionalityIsDimensionless(SIQuantityGetUnitDimensionality((SIQuantityRef) scalar))) {
                SIScalarConvertToCoherentUnit((SIMutableScalarRef) scalar, errorString);
                SIUnitRef unit = SIUnitForUnderivedSymbol(STR("rad"));
                double complex value = catan(SIScalarDoubleComplexValue(scalar));
                return SIScalarCreateWithDoubleComplex(value, unit);
            }
            if(errorString) {
                *errorString = STR("atan requires dimensionless unit.");
            }
            
            return NULL;
        }
        case BM_atanh: {
            if(SIDimensionalityIsDimensionless(SIQuantityGetUnitDimensionality((SIQuantityRef) scalar))) {
                SIScalarConvertToCoherentUnit((SIMutableScalarRef) scalar, errorString);
                SIUnitRef unit = SIUnitForUnderivedSymbol(STR("rad"));
                double complex value = catanh(SIScalarDoubleComplexValue(scalar));
                return SIScalarCreateWithDoubleComplex(value, unit);
            }
            if(errorString) {
                *errorString = STR("atanh requires dimensionless unit.");
            }
            
            return NULL;
        }
        case BM_cos: {
            if(SIDimensionalityIsDimensionless(SIQuantityGetUnitDimensionality((SIQuantityRef) scalar))) {
                SIScalarConvertToCoherentUnit((SIMutableScalarRef) scalar, errorString);
                double complex value = complex_cosine(SIScalarDoubleComplexValue(scalar));
                return SIScalarCreateWithDoubleComplex(value, NULL);
            }
            if(errorString) {
                *errorString = STR("cos requires dimensionless unit.");
            }
            
            return NULL;
        }
        case BM_cosh: {
            if(SIDimensionalityIsDimensionless(SIQuantityGetUnitDimensionality((SIQuantityRef) scalar))) {
                SIScalarConvertToCoherentUnit((SIMutableScalarRef) scalar, errorString);
                double complex value = ccosh(SIScalarDoubleComplexValue(scalar));
                return SIScalarCreateWithDoubleComplex(value, NULL);
            }
            if(errorString) {
                *errorString = STR("cosh requires dimensionless unit.");
            }
            
            return NULL;
        }
        case BM_sin: {
            if(SIDimensionalityIsDimensionless(SIQuantityGetUnitDimensionality((SIQuantityRef) scalar))) {
                SIScalarConvertToCoherentUnit((SIMutableScalarRef) scalar, errorString);
                double complex value = complex_sine(SIScalarDoubleComplexValue(scalar));
                return SIScalarCreateWithDoubleComplex(value, NULL);
            }
            if(errorString) {
                *errorString = STR("sin requires dimensionless unit.");
            }
            
            return NULL;
        }
        case BM_sinh: {
            if(SIDimensionalityIsDimensionless(SIQuantityGetUnitDimensionality((SIQuantityRef) scalar))) {
                SIScalarConvertToCoherentUnit((SIMutableScalarRef) scalar, errorString);
                double complex value = csinh(SIScalarDoubleComplexValue(scalar));
                return SIScalarCreateWithDoubleComplex(value, NULL);
            }
            if(errorString) {
                *errorString = STR("sinh requires dimensionless unit.");
            }
            
            return NULL;
        }
        case BM_tan: {
            if(SIDimensionalityIsDimensionless(SIQuantityGetUnitDimensionality((SIQuantityRef) scalar))) {
                SIScalarConvertToCoherentUnit((SIMutableScalarRef) scalar, errorString);
                double complex value = complex_tangent(SIScalarDoubleComplexValue(scalar));
                return SIScalarCreateWithDoubleComplex(value, NULL);
            }
            if(errorString) {
                *errorString = STR("tan requires dimensionless unit.");
            }
            
            return NULL;
        }
        case BM_tanh: {
            if(SIDimensionalityIsDimensionless(SIQuantityGetUnitDimensionality((SIQuantityRef) scalar))) {
                SIScalarConvertToCoherentUnit((SIMutableScalarRef) scalar, errorString);
                double complex value = ctanh(SIScalarDoubleComplexValue(scalar));
                return SIScalarCreateWithDoubleComplex(value, NULL);
            }
            if(errorString) {
                *errorString = STR("tanh requires dimensionless unit.");
            }
            
            return NULL;
        }
        case BM_conj:
            return SIScalarCreateByConjugation(scalar);
        case BM_creal:
            return SIScalarCreateByTakingComplexPart(scalar,kSIRealPart);
        case BM_cimag:
            return SIScalarCreateByTakingComplexPart(scalar,kSIImaginaryPart);
        case BM_carg:
            return SIScalarCreateByTakingComplexPart(scalar,kSIArgumentPart);
        case BM_cabs:
            return SIScalarCreateByTakingComplexPart(scalar,kSIMagnitudePart);
        default:
            if(errorString) {
                *errorString = STR("unknown function.");
            }
            
            return NULL;
        }
    }
}

SIUnitRef ConversionWithDefinedUnit(OCMutableStringRef mutString, double *unit_multiplier, OCStringRef *errorString)
{
    if(errorString) if(*errorString) return NULL;
    // Remove appended unit conversion
    printf("ConversionWithDefinedUnit, before calling\n");
    OCArrayRef conversions = OCStringCreateArrayBySeparatingStrings(mutString,STR(".."));
    printf("ConversionWithDefinedUnit, after calling\n");

    if(conversions) {
        if(OCArrayGetCount(conversions) == 2) {
            OCStringRef firstString = OCArrayGetValueAtIndex(conversions, 0);
            OCStringRef secondString = OCArrayGetValueAtIndex(conversions, 1);
            if(OCStringGetLength(firstString)==0 || OCStringGetLength(secondString)==0) {
                OCRelease(conversions);
                return NULL;
            }
            SIUnitRef finalUnit = SIUnitForSymbol(secondString, unit_multiplier, errorString);
            if(finalUnit) {
                OCStringReplaceAll(mutString, firstString);
                OCRelease(conversions);
                return finalUnit;
            }
        }
        OCRelease(conversions);
    }
    return NULL;
}

