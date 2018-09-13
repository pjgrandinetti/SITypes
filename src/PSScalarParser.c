//
//  PSScalarParser.c
//
//  Created by PhySy Ltd on 5/3/13.
//  Copyright (c) 2008-2014 PhySy Ltd. All rights reserved.
//

#include "SILibrary.h"
#import "PSScalarConstants.h"
#import "PSScalarParser.h"

struct __symbol {
    char *name;
    PSScalarRef value;
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
    PSScalarRef number;
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


PSScalarRef ScalarNodeEvaluate(ScalarNodeRef node, OCStringRef *errorString)
{
    if(errorString) if(*errorString) return NULL;
    
    switch(node->nodeType) {
        case 'K': {
            NumberRef leaf = (NumberRef) node;
            return leaf->number;
        }
        case '+': {
            PSScalarRef left = ScalarNodeEvaluate(node->left, errorString);
            if(errorString) if(*errorString) return NULL;
            PSScalarRef right = ScalarNodeEvaluate(node->right, errorString);
            if(errorString) if(*errorString) return NULL;
            PSScalarRef temp = PSScalarCreateByAdding(left, right, errorString);
            if(errorString) if(*errorString) return NULL;
            if(temp) return temp;
            return NULL;
        }
        case '-': {
            PSScalarRef left = ScalarNodeEvaluate(node->left,errorString);
            if(errorString) if(*errorString) return NULL;
            PSScalarRef right = ScalarNodeEvaluate(node->right,errorString);
            if(errorString) if(*errorString) return NULL;
            PSScalarRef temp = PSScalarCreateBySubtracting(left, right, errorString);
            if(errorString) if(*errorString) return NULL;
            if(temp) return temp;
            return NULL;
        }
        case '*': {
            PSScalarRef left = ScalarNodeEvaluate(node->left,errorString);
            if(errorString) if(*errorString) return NULL;
            PSScalarRef right = ScalarNodeEvaluate(node->right,errorString);
            if(errorString) if(*errorString) return NULL;
            PSScalarRef temp = PSScalarCreateByMultiplyingWithoutReducingUnit(left, right, errorString);
            if(temp) return temp;
            return NULL;
        }
        case '/': {
            PSScalarRef left = ScalarNodeEvaluate(node->left,errorString);
            if(errorString) if(*errorString) return NULL;
            PSScalarRef right = ScalarNodeEvaluate(node->right,errorString);
            if(errorString) if(*errorString) return NULL;
            PSScalarRef temp = PSScalarCreateByDividingWithoutReducingUnit(left, right, errorString);
            if(temp) return temp;
            return NULL;
        }
        case '!': {
            PSScalarRef left = ScalarNodeEvaluate(node->left,errorString);
            if(errorString) if(*errorString) return NULL;
            PSScalarRef temp = PSScalarCreateByGammaFunctionWithoutReducingUnit(left, errorString);
            if(temp) return temp;
            return NULL;
        }
        case '^': {
            PSScalarRef left = ScalarNodeEvaluate(node->left,errorString);
            if(errorString) if(*errorString) return NULL;
            PSScalarRef right = ScalarNodeEvaluate(node->right,errorString);
            if(errorString) if(*errorString) return NULL;
            if(SIUnitIsDimensionless(PSQuantityGetUnit((PSQuantityRef) right))) {
                PSScalarReduceUnit((PSMutableScalarRef) right);
                double complex power = PSScalarDoubleComplexValue(right);
                SIUnitRef argumentUnit = PSQuantityGetUnit((PSQuantityRef) left);
                if(SIUnitIsDimensionless(argumentUnit) && SIUnitGetScaleNonSIToCoherentSI(argumentUnit) == 1.0) {
                    PSScalarReduceUnit((PSMutableScalarRef) left);
                    double complex x = PSScalarDoubleComplexValue(left);
                    if(cimag(power) == 0 && power == floor(creal(power))) {
                        double complex result = raise_to_integer_power(x,  (long) creal(power));
                        if(isnan(result)) {
                            if(errorString) {
                                *errorString = STR("Overflow.");
                            }
                            
                            return NULL;
                        }
                        return PSScalarCreateWithDoubleComplex(result, NULL);
                    }
                    else {
                        double complex result = cpow(x, power);
                        if(isnan(result)) {
                            if(errorString) {
                                *errorString = STR("Overflow.");
                            }
                            
                            return NULL;
                        }
                        return PSScalarCreateWithDoubleComplex(result, NULL);
                    }
                }
                else {
                    if(PSScalarIsReal(right)) {
                        return  PSScalarCreateByRaisingToAPowerWithoutReducingUnit(left, power, errorString);
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
            PSScalarRef left = ScalarNodeEvaluate(node->left,errorString);
            if(errorString) if(*errorString) return NULL;
            return PSScalarCreateByTakingComplexPart(left, kPSMagnitudePart);
        }
        case 'M': {
            PSScalarRef left = ScalarNodeEvaluate(node->left,errorString);
            if(errorString) if(*errorString) return NULL;
            return PSScalarCreateByMultiplyingByDimensionlessRealConstant(left, -1.);
        }
        case 'F': {
            PSScalarRef result = builtInMathFunction((ScalarNodeMathFunctionRef) node, errorString);
            if(errorString) if(*errorString) return NULL;
            return result;
        }
        case 'C': {
            PSScalarRef result = builtInConstantFunction((ScalarNodeConstantFunctionRef) node, errorString);
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

ScalarNodeRef ScalarNodeCreateNumberLeaf(PSScalarRef number)
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

PSScalarRef builtInConstantFunction(ScalarNodeConstantFunctionRef func, OCStringRef *errorString)
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
            return PSPeriodicTableCreateMolarMass(func->string, errorString);
            break;
        }
        case BC_FW: {
            return PSPeriodicTableCreateFormulaMass(func->string, errorString);
            break;
        }
        case BC_Isotope_Abundance:
            return PSPeriodicTableCreateIsotopeAbundance(func->string, errorString);
            break;
        case BC_Isotope_Spin:
            return PSPeriodicTableCreateIsotopeSpin(func->string, errorString);
            break;
        case BC_Isotope_HalfLife:
            return PSPeriodicTableCreateIsotopeHalfLife(func->string, errorString);
            break;
        case BC_Isotope_Gyromag:
            return PSPeriodicTableCreateIsotopeGyromagneticRatio(func->string, errorString);
            break;
        case BC_Isotope_MagneticDipole:
            return PSPeriodicTableCreateIsotopeMagneticDipoleMoment(func->string, errorString);
            break;
        case BC_Isotope_ElectricQuadrupole:
            return PSPeriodicTableCreateIsotopeElectricQuadrupoleMoment(func->string, errorString);
            break;
        case BC_nmr:
            return PSPeriodicTableCreateNMRFrequency(func->string, errorString);
            break;
        default:
            return NULL;
    }
    return NULL;
}

PSScalarRef builtInMathFunction(ScalarNodeMathFunctionRef func, OCStringRef *errorString)
{
    if(errorString) if(*errorString) return NULL;
    
    builtInMathFunctions funcType = func->funcType;
    PSScalarRef scalar = ScalarNodeEvaluate(func->left,errorString);
    if(NULL==scalar) return NULL;
    
    switch(funcType) {
        case BM_reduce: {
            return PSScalarCreateByReducingUnit(scalar);
        }
        case BM_sqrt: {
            return PSScalarCreateByTakingNthRoot(scalar,2,errorString);
        }
        case BM_cbrt: {
            return PSScalarCreateByTakingNthRoot(scalar,3,errorString);
        }
        case BM_qtrt: {
            return PSScalarCreateByTakingNthRoot(scalar,4,errorString);
        }
        case BM_exp: {
            if(SIDimensionalityIsDimensionless(PSQuantityGetUnitDimensionality((PSQuantityRef) scalar))) {
                PSScalarConvertToCoherentUnit((PSMutableScalarRef) scalar, errorString);
                double complex value = cexp(PSScalarDoubleComplexValue(scalar));
                return PSScalarCreateWithDoubleComplex(value, NULL);
            }
            if(errorString) {
                *errorString = STR("exp requires dimensionless unit.");
            }
            return NULL;
        }
        case BM_erf: {
            if(SIDimensionalityIsDimensionless(PSQuantityGetUnitDimensionality((PSQuantityRef) scalar))) {
                if(!PSScalarIsComplex(scalar)) {
                    PSScalarConvertToCoherentUnit((PSMutableScalarRef) scalar, errorString);
                    double complex value = erf(PSScalarDoubleValue(scalar));
                    return PSScalarCreateWithDoubleComplex(value, NULL);
                }
            }
            if(errorString) {
                *errorString = STR("erf requires dimensionless real quantity.");
            }
            return NULL;
        }
        case BM_erfc: {
            if(SIDimensionalityIsDimensionless(PSQuantityGetUnitDimensionality((PSQuantityRef) scalar))) {
                if(!PSScalarIsComplex(scalar)) {
                    PSScalarConvertToCoherentUnit((PSMutableScalarRef) scalar, errorString);
                    double complex value = erfc(PSScalarDoubleValue(scalar));
                    return PSScalarCreateWithDoubleComplex(value, NULL);
                }
            }
            if(errorString) {
                *errorString = STR("erfc requires dimensionless real quantity.");
                
            }
            return NULL;
        }
        case BM_ln: {
            if(SIDimensionalityIsDimensionless(PSQuantityGetUnitDimensionality((PSQuantityRef) scalar))) {
                PSScalarConvertToCoherentUnit((PSMutableScalarRef) scalar, errorString);
                double complex value = clog(PSScalarDoubleComplexValue(scalar));
                return PSScalarCreateWithDoubleComplex(value, NULL);
            }
            if(errorString) {
                *errorString = STR("ln requires dimensionless unit.");
            }
            
            return NULL;
        }
        case BM_log: {
            if(SIDimensionalityIsDimensionless(PSQuantityGetUnitDimensionality((PSQuantityRef) scalar))) {
                PSScalarConvertToCoherentUnit((PSMutableScalarRef) scalar, errorString);
                double complex value = clog(PSScalarDoubleComplexValue(scalar))/log(10);
                return PSScalarCreateWithDoubleComplex(value, NULL);
            }
            if(errorString) {
                *errorString = STR("ln requires dimensionless unit.");
            }
            
            return NULL;
        }
        case BM_acos: {
            if(SIDimensionalityIsDimensionless(PSQuantityGetUnitDimensionality((PSQuantityRef) scalar))) {
                PSScalarConvertToCoherentUnit((PSMutableScalarRef) scalar, errorString);
                double complex value = cacos(PSScalarDoubleComplexValue(scalar));
                SIUnitRef unit = SIUnitForUnderivedSymbol(STR("rad"));
                return PSScalarCreateWithDoubleComplex(value, unit);
            }
            if(errorString) {
                *errorString = STR("acos requires dimensionless unit.");
                
                return NULL;
            }
        case BM_acosh: {
            if(SIDimensionalityIsDimensionless(PSQuantityGetUnitDimensionality((PSQuantityRef) scalar))) {
                PSScalarConvertToCoherentUnit((PSMutableScalarRef) scalar, errorString);
                double complex value = cacosh(PSScalarDoubleComplexValue(scalar));
                SIUnitRef unit = SIUnitForUnderivedSymbol(STR("rad"));
                return PSScalarCreateWithDoubleComplex(value, unit);
            }
            if(errorString) {
                *errorString = STR("acosh requires dimensionless unit.");
            }
            
            return NULL;
        }
        case BM_asin: {
            if(SIDimensionalityIsDimensionless(PSQuantityGetUnitDimensionality((PSQuantityRef) scalar))) {
                PSScalarConvertToCoherentUnit((PSMutableScalarRef) scalar, errorString);
                double complex value = casin(PSScalarDoubleComplexValue(scalar));
                SIUnitRef unit = SIUnitForUnderivedSymbol(STR("rad"));
                return PSScalarCreateWithDoubleComplex(value, unit);
            }
            if(errorString) {
                *errorString = STR("asin requires dimensionless unit.");
            }
            
            return NULL;
        }
        case BM_asinh: {
            if(SIDimensionalityIsDimensionless(PSQuantityGetUnitDimensionality((PSQuantityRef) scalar))) {
                PSScalarConvertToCoherentUnit((PSMutableScalarRef) scalar, errorString);
                SIUnitRef unit = SIUnitForUnderivedSymbol(STR("rad"));
                double complex value = casinh(PSScalarDoubleComplexValue(scalar));
                return PSScalarCreateWithDoubleComplex(value, unit);
            }
            if(errorString) {
                *errorString = STR("asinh requires dimensionless unit.");
            }
            
            return NULL;
        }
        case BM_atan: {
            if(SIDimensionalityIsDimensionless(PSQuantityGetUnitDimensionality((PSQuantityRef) scalar))) {
                PSScalarConvertToCoherentUnit((PSMutableScalarRef) scalar, errorString);
                SIUnitRef unit = SIUnitForUnderivedSymbol(STR("rad"));
                double complex value = catan(PSScalarDoubleComplexValue(scalar));
                return PSScalarCreateWithDoubleComplex(value, unit);
            }
            if(errorString) {
                *errorString = STR("atan requires dimensionless unit.");
            }
            
            return NULL;
        }
        case BM_atanh: {
            if(SIDimensionalityIsDimensionless(PSQuantityGetUnitDimensionality((PSQuantityRef) scalar))) {
                PSScalarConvertToCoherentUnit((PSMutableScalarRef) scalar, errorString);
                SIUnitRef unit = SIUnitForUnderivedSymbol(STR("rad"));
                double complex value = catanh(PSScalarDoubleComplexValue(scalar));
                return PSScalarCreateWithDoubleComplex(value, unit);
            }
            if(errorString) {
                *errorString = STR("atanh requires dimensionless unit.");
            }
            
            return NULL;
        }
        case BM_cos: {
            if(SIDimensionalityIsDimensionless(PSQuantityGetUnitDimensionality((PSQuantityRef) scalar))) {
                PSScalarConvertToCoherentUnit((PSMutableScalarRef) scalar, errorString);
                double complex value = complex_cosine(PSScalarDoubleComplexValue(scalar));
                return PSScalarCreateWithDoubleComplex(value, NULL);
            }
            if(errorString) {
                *errorString = STR("cos requires dimensionless unit.");
            }
            
            return NULL;
        }
        case BM_cosh: {
            if(SIDimensionalityIsDimensionless(PSQuantityGetUnitDimensionality((PSQuantityRef) scalar))) {
                PSScalarConvertToCoherentUnit((PSMutableScalarRef) scalar, errorString);
                double complex value = ccosh(PSScalarDoubleComplexValue(scalar));
                return PSScalarCreateWithDoubleComplex(value, NULL);
            }
            if(errorString) {
                *errorString = STR("cosh requires dimensionless unit.");
            }
            
            return NULL;
        }
        case BM_sin: {
            if(SIDimensionalityIsDimensionless(PSQuantityGetUnitDimensionality((PSQuantityRef) scalar))) {
                PSScalarConvertToCoherentUnit((PSMutableScalarRef) scalar, errorString);
                double complex value = complex_sine(PSScalarDoubleComplexValue(scalar));
                return PSScalarCreateWithDoubleComplex(value, NULL);
            }
            if(errorString) {
                *errorString = STR("sin requires dimensionless unit.");
            }
            
            return NULL;
        }
        case BM_sinh: {
            if(SIDimensionalityIsDimensionless(PSQuantityGetUnitDimensionality((PSQuantityRef) scalar))) {
                PSScalarConvertToCoherentUnit((PSMutableScalarRef) scalar, errorString);
                double complex value = csinh(PSScalarDoubleComplexValue(scalar));
                return PSScalarCreateWithDoubleComplex(value, NULL);
            }
            if(errorString) {
                *errorString = STR("sinh requires dimensionless unit.");
            }
            
            return NULL;
        }
        case BM_tan: {
            if(SIDimensionalityIsDimensionless(PSQuantityGetUnitDimensionality((PSQuantityRef) scalar))) {
                PSScalarConvertToCoherentUnit((PSMutableScalarRef) scalar, errorString);
                double complex value = complex_tangent(PSScalarDoubleComplexValue(scalar));
                return PSScalarCreateWithDoubleComplex(value, NULL);
            }
            if(errorString) {
                *errorString = STR("tan requires dimensionless unit.");
            }
            
            return NULL;
        }
        case BM_tanh: {
            if(SIDimensionalityIsDimensionless(PSQuantityGetUnitDimensionality((PSQuantityRef) scalar))) {
                PSScalarConvertToCoherentUnit((PSMutableScalarRef) scalar, errorString);
                double complex value = ctanh(PSScalarDoubleComplexValue(scalar));
                return PSScalarCreateWithDoubleComplex(value, NULL);
            }
            if(errorString) {
                *errorString = STR("tanh requires dimensionless unit.");
            }
            
            return NULL;
        }
        case BM_conj:
            return PSScalarCreateByConjugation(scalar);
        case BM_creal:
            return PSScalarCreateByTakingComplexPart(scalar,kPSRealPart);
        case BM_cimag:
            return PSScalarCreateByTakingComplexPart(scalar,kPSImaginaryPart);
        case BM_carg:
            return PSScalarCreateByTakingComplexPart(scalar,kPSArgumentPart);
        case BM_cabs:
            return PSScalarCreateByTakingComplexPart(scalar,kPSMagnitudePart);
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
    OCArrayRef conversions = OCStringCreateArrayBySeparatingStrings(mutString,STR(".."));
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

