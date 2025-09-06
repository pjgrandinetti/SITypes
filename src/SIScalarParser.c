//
//  SIScalarParser.c
//
//  Created by PhySy Ltd on 5/3/13.
//  Copyright (c) 2008-2014 PhySy Ltd. All rights reserved.
//
#include "SIScalarParser.h"
#include <math.h>    // For floor, isnan, erf, erfc, log
#include <stdio.h>   // For yyerror, fprintf, stderr
#include <stdlib.h>  // For malloc, free
#include "SIScalarConstants.h"
#include "SITypes.h"
struct impl_symbol {
    char *name;
    SIScalarRef value;
} impl_symbol;
#define NHASH 9997
struct impl_symbol symbolTable[NHASH];
struct impl_scalarNode {
    int nodeType;
    ScalarNodeRef left;
    ScalarNodeRef right;
} impl_scalarNode;
struct impl_scalarValue {
    int nodeType;
    SIScalarRef number;
} impl_scalarValue;
struct impl_scalarNodeMathFunction {
    int nodeType;
    ScalarNodeRef left;
    builtInMathFunctions funcType;
} impl_scalarNodeMathFunction;
struct impl_scalarNodeConstantFunction {
    int nodeType;
    builtInConstantFunctions funcType;
    OCMutableStringRef string;
} impl_scalarNodeConstantFunction;
SIScalarRef ScalarNodeEvaluate(ScalarNodeRef node, OCStringRef *errorString) {
    if (errorString)
        if (*errorString)
            return NULL;
    switch (node->nodeType) {
        case 'K': {
            NumberRef leaf = (NumberRef)node;
            return leaf->number;
        }
        case '+': {
            SIScalarRef left = ScalarNodeEvaluate(node->left, errorString);
            if (errorString)
                if (*errorString)
                    return NULL;
            SIScalarRef right = ScalarNodeEvaluate(node->right, errorString);
            if (errorString)
                if (*errorString)
                    return NULL;
            SIScalarRef temp = SIScalarCreateByAdding(left, right, errorString);
            if (errorString)
                if (*errorString)
                    return NULL;
            if (temp) {
                OCAutorelease(temp);
                return temp;
            }
            return NULL;
        }
        case '-': {
            SIScalarRef left = ScalarNodeEvaluate(node->left, errorString);
            if (errorString)
                if (*errorString)
                    return NULL;
            SIScalarRef right = ScalarNodeEvaluate(node->right, errorString);
            if (errorString)
                if (*errorString)
                    return NULL;
            SIScalarRef temp = SIScalarCreateBySubtracting(left, right, errorString);
            if (errorString)
                if (*errorString)
                    return NULL;
            if (temp) {
                OCAutorelease(temp);
                return temp;
            }
            return NULL;
        }
        case '*': {
            SIScalarRef left = ScalarNodeEvaluate(node->left, errorString);
            if (errorString)
                if (*errorString)
                    return NULL;
            SIScalarRef right = ScalarNodeEvaluate(node->right, errorString);
            if (errorString)
                if (*errorString)
                    return NULL;
            SIScalarRef temp = SIScalarCreateByMultiplyingWithoutReducingUnit(left, right, errorString);
            if (temp) {
                OCAutorelease(temp);
                return temp;
            }
            return NULL;
        }
        case '/': {
            SIScalarRef left = ScalarNodeEvaluate(node->left, errorString);
            if (errorString)
                if (*errorString)
                    return NULL;
            SIScalarRef right = ScalarNodeEvaluate(node->right, errorString);
            if (errorString)
                if (*errorString)
                    return NULL;
            SIScalarRef temp = SIScalarCreateByDividingWithoutReducingUnit(left, right, errorString);
            if (temp) {
                OCAutorelease(temp);
                return temp;
            }
            return NULL;
        }
        case '!': {
            SIScalarRef left = ScalarNodeEvaluate(node->left, errorString);
            if (errorString)
                if (*errorString)
                    return NULL;
            SIScalarRef temp = SIScalarCreateByGammaFunctionWithoutReducingUnit(left, errorString);
            if (temp) {
                OCAutorelease(temp);
                return temp;
            }
            return NULL;
        }
        case '^': {
            SIScalarRef left = ScalarNodeEvaluate(node->left, errorString);
            if (errorString)
                if (*errorString)
                    return NULL;
            SIScalarRef right = ScalarNodeEvaluate(node->right, errorString);
            if (errorString)
                if (*errorString)
                    return NULL;
            if (SIUnitIsDimensionless(SIQuantityGetUnit((SIQuantityRef)right))) {
                SIScalarReduceUnit((SIMutableScalarRef)right);
                double complex power = SIScalarDoubleComplexValue(right);
                SIUnitRef argumentUnit = SIQuantityGetUnit((SIQuantityRef)left);
                if (SIUnitIsDimensionless(argumentUnit) && SIUnitGetScaleToCoherentSI(argumentUnit) == 1.0) {
                    SIScalarReduceUnit((SIMutableScalarRef)left);
                    double complex x = SIScalarDoubleComplexValue(left);
                    if (cimag(power) == 0 && creal(power) == floor(creal(power))) {             // ensure power is an integer
                        double complex result = raise_to_integer_power(x, (long)creal(power));  // Reverted to raise_to_integer_power
                        if (isnan(creal(result)) && isnan(cimag(result))) {                     // Check both parts for NaN
                            if (errorString) {
                                *errorString = STR("Overflow.");
                            }
                            return NULL;
                        }
                        SIScalarRef theScalar = SIScalarCreateWithDoubleComplex(result, NULL);
                        if (theScalar) {
                            OCAutorelease(theScalar);
                        }
                        return theScalar;
                    } else {
                        double complex result = cpow(x, power);
                        if (isnan(creal(result)) && isnan(cimag(result))) {  // Check both parts for NaN
                            if (errorString) {
                                *errorString = STR("Overflow.");
                            }
                            return NULL;
                        }
                        SIScalarRef theScalar = SIScalarCreateWithDoubleComplex(result, NULL);
                        if (theScalar) {
                            OCAutorelease(theScalar);
                        }
                        return theScalar;
                    }
                } else {
                    if (SIScalarIsReal(right)) {
                        SIScalarRef theScalar = SIScalarCreateByRaisingToPowerWithoutReducingUnit(left, power, errorString);
                        if (theScalar) {
                            OCAutorelease(theScalar);
                        }
                        if (errorString)
                            if (*errorString)
                                return NULL;
                        return theScalar;
                    } else {
                        if (errorString) {
                            *errorString = STR("Powers must be real.");
                        }
                        return NULL;
                    }
                }
            }
            if (errorString) {
                *errorString = STR("Powers must be dimensionless.");
            }
            return NULL;
        }
        case '|': {
            SIScalarRef left = ScalarNodeEvaluate(node->left, errorString);
            if (errorString)
                if (*errorString)
                    return NULL;
            SIScalarRef theScalar = SIScalarCreateByTakingComplexPart(left, kSIMagnitudePart);
            if (theScalar) {
                OCAutorelease(theScalar);
            }
            return theScalar;
        }
        case 'M': {
            SIScalarRef left = ScalarNodeEvaluate(node->left, errorString);
            if (errorString)
                if (*errorString)
                    return NULL;
            SIScalarRef theScalar = SIScalarCreateByMultiplyingByDimensionlessRealConstant(left, -1.);
            if (theScalar) {
                OCAutorelease(theScalar);
            }
            return theScalar;
        }
        case 'F': {
            SIScalarRef result = builtInMathFunction((ScalarNodeMathFunctionRef)node, errorString);
            if (errorString)
                if (*errorString)
                    return NULL;
            return result;
        }
        case 'C': {
            SIScalarRef result = builtInConstantFunction((ScalarNodeConstantFunctionRef)node, errorString);
            if (errorString)
                if (*errorString)
                    return NULL;
            return result;
        }
        default:
            return NULL;
    }
    return 0;
}
ScalarNodeRef ScalarNodeCreateInnerNode(int nodeType, ScalarNodeRef left, ScalarNodeRef right) {
    struct impl_scalarNode *node = malloc(sizeof(struct impl_scalarNode));
    if (NULL == node) {
        fprintf(stderr, "ScalarNodeCreateInnerNode: Memory allocation failed.\n");
        return NULL;  // Handle memory allocation failure
    }
    node->nodeType = nodeType;
    node->left = left;
    node->right = right;
    return node;
}
ScalarNodeRef ScalarNodeCreateMathFunction(builtInMathFunctions funcType, ScalarNodeRef left) {
    struct impl_scalarNodeMathFunction *node = malloc(sizeof(struct impl_scalarNodeMathFunction));
    if (NULL == node) {
        fprintf(stderr, "ScalarNodeCreateMathFunction: Memory allocation failed.\n");
        return NULL;  // Handle memory allocation failure
    }
    node->nodeType = 'F';
    node->left = left;
    node->funcType = funcType;
    return (ScalarNodeRef)node;
}
ScalarNodeRef ScalarNodeCreateConstantFunction(builtInConstantFunctions funcType, OCMutableStringRef string) {
    struct impl_scalarNodeConstantFunction *node = malloc(sizeof(struct impl_scalarNodeConstantFunction));
    if (NULL == node) {
        fprintf(stderr, "ScalarNodeCreateConstantFunction: Memory allocation failed.\n");
        return NULL;  // Handle memory allocation failure
    }
    node->nodeType = 'C';
    node->string = string;
    node->funcType = funcType;
    return (ScalarNodeRef)node;
}
ScalarNodeRef ScalarNodeCreateNumberLeaf(SIScalarRef number) {
    struct impl_scalarValue *leaf = malloc(sizeof(struct impl_scalarValue));
    if (NULL == leaf) {
        fprintf(stderr, "ScalarNodeCreateNumberLeaf: Memory allocation failed.\n");
        return NULL;  // Handle memory allocation failure
    }
    leaf->nodeType = 'K';
    leaf->number = number;
    return (ScalarNodeRef)leaf;
}
char ScalarNodeGetType(ScalarNodeRef node) {
    return node->nodeType;
}
bool ScalarNodeisLeaf(ScalarNodeRef node) {
    return (node->nodeType == 'K');
}
void ScalarNodeFree(ScalarNodeRef node) {
    if (node == NULL)
        return;
    switch (node->nodeType) {
        case '+':
        case '-':
        case '*':
        case '/':
        case '^':
        case 'L': {
            // Binary operators: free right and left
            ScalarNodeFree(node->right);
            ScalarNodeFree(node->left);
            break;
        }
        case '|':
        case 'M':
        case '!': {
            // Unary operators: free left
            ScalarNodeFree(node->left);
            break;
        }
        case 'F': {
            // Math function: free operand
            ScalarNodeMathFunctionRef funcNode = (ScalarNodeMathFunctionRef)node;
            ScalarNodeFree(funcNode->left);
            break;
        }
        case 'C':
            break;
        case 'K':
            break;
        default:
            break;
    }
    free((void *)node);  // Always free the node structure itself
}
SIScalarRef builtInConstantFunction(ScalarNodeConstantFunctionRef func, OCStringRef *errorString) {
    if (errorString)
        if (*errorString)
            return NULL;
    builtInConstantFunctions funcType = func->funcType;
    // Remove square brackets
    OCRange range;
    range.location = OCStringGetLength(func->string) - 1;
    range.length = 1;
    OCStringDelete(func->string, range);
    range.location = 0;
    range.length = 1;
    OCStringDelete(func->string, range);
    switch (funcType) {
        case BC_AW: {
            SIScalarRef theScalar = SIPeriodicTableCreateMolarMass(func->string, errorString);
            if (theScalar) OCAutorelease(theScalar);
            return theScalar;
        }
        case BC_FW: {
            SIScalarRef theScalar = SIPeriodicTableCreateFormulaMass(func->string, errorString);
            if (theScalar) OCAutorelease(theScalar);
            return theScalar;
        }
        case BC_Isotope_Abundance: {
            SIScalarRef theScalar = SIPeriodicTableCreateIsotopeAbundance(func->string, errorString);
            if (theScalar) OCAutorelease(theScalar);
            return theScalar;
        }
        case BC_Isotope_Spin: {
            SIScalarRef theScalar = SIPeriodicTableCreateIsotopeSpin(func->string, errorString);
            if (theScalar) OCAutorelease(theScalar);
            return theScalar;
        }
        case BC_Isotope_HalfLife: {
            SIScalarRef theScalar = SIPeriodicTableCreateIsotopeHalfLife(func->string, errorString);
            if (theScalar) OCAutorelease(theScalar);
            return theScalar;
        }
        case BC_Isotope_Gyromag: {
            SIScalarRef theScalar = SIPeriodicTableCreateIsotopeGyromagneticRatio(func->string, errorString);
            if (theScalar) OCAutorelease(theScalar);
            return theScalar;
        }
        case BC_Isotope_MagneticDipole: {
            SIScalarRef theScalar = SIPeriodicTableCreateIsotopeMagneticDipoleMoment(func->string, errorString);
            if (theScalar) OCAutorelease(theScalar);
            return theScalar;
        }
        case BC_Isotope_ElectricQuadrupole: {
            SIScalarRef theScalar = SIPeriodicTableCreateIsotopeElectricQuadrupoleMoment(func->string, errorString);
            if (theScalar) OCAutorelease(theScalar);
            return theScalar;
        }
        case BC_nmr: {
            SIScalarRef theScalar = SIPeriodicTableCreateNMRFrequency(func->string, errorString);
            if (theScalar) OCAutorelease(theScalar);
            return theScalar;
        }
        default: {
            return NULL;
        }
    }
    return NULL;
}
SIScalarRef builtInMathFunction(ScalarNodeMathFunctionRef func, OCStringRef *errorString) {
    if (errorString)
        if (*errorString)
            return NULL;
    builtInMathFunctions funcType = func->funcType;
    SIScalarRef scalar = ScalarNodeEvaluate(func->left, errorString);
    if (NULL == scalar) return NULL;
    switch (funcType) {
        case BM_reduce: {
            SIScalarRef result = SIScalarCreateByReducingUnit(scalar);
            if (result) OCAutorelease(result);
            return result;
        }
        case BM_sqrt: {
            SIScalarRef result = SIScalarCreateByTakingNthRoot(scalar, 2, errorString);
            if (result) OCAutorelease(result);
            return result;
        }
        case BM_cbrt: {
            SIScalarRef result = SIScalarCreateByTakingNthRoot(scalar, 3, errorString);
            if (result) OCAutorelease(result);
            return result;
        }
        case BM_qtrt: {
            SIScalarRef result = SIScalarCreateByTakingNthRoot(scalar, 4, errorString);
            if (result) OCAutorelease(result);
            return result;
        }
        case BM_exp: {
            if (SIDimensionalityIsDimensionless(SIQuantityGetUnitDimensionality((SIQuantityRef)scalar))) {
                SIScalarConvertToCoherentUnit((SIMutableScalarRef)scalar, errorString);
                double complex value = cexp(SIScalarDoubleComplexValue(scalar));
                SIScalarRef result = SIScalarCreateWithDoubleComplex(value, NULL);
                if (result) OCAutorelease(result);
                return result;
            }
            if (errorString) {
                *errorString = STR("exp requires dimensionless unit.");
            }
            return NULL;
        }
        case BM_erf: {
            if (SIDimensionalityIsDimensionless(SIQuantityGetUnitDimensionality((SIQuantityRef)scalar))) {
                if (!SIScalarIsComplex(scalar)) {
                    SIScalarConvertToCoherentUnit((SIMutableScalarRef)scalar, errorString);
                    double complex value = erf(SIScalarDoubleValue(scalar));
                    SIScalarRef result = SIScalarCreateWithDoubleComplex(value, NULL);
                    if (result) OCAutorelease(result);
                    return result;
                }
            }
            if (errorString) {
                *errorString = STR("erf requires dimensionless real quantity.");
            }
            return NULL;
        }
        case BM_erfc: {
            if (SIDimensionalityIsDimensionless(SIQuantityGetUnitDimensionality((SIQuantityRef)scalar))) {
                if (!SIScalarIsComplex(scalar)) {
                    SIScalarConvertToCoherentUnit((SIMutableScalarRef)scalar, errorString);
                    double complex value = erfc(SIScalarDoubleValue(scalar));
                    SIScalarRef result = SIScalarCreateWithDoubleComplex(value, NULL);
                    if (result) OCAutorelease(result);
                    return result;
                }
            }
            if (errorString) {
                *errorString = STR("erfc requires dimensionless real quantity.");
            }
            return NULL;
        }
        case BM_ln: {
            if (SIDimensionalityIsDimensionless(SIQuantityGetUnitDimensionality((SIQuantityRef)scalar))) {
                SIScalarConvertToCoherentUnit((SIMutableScalarRef)scalar, errorString);
                double complex value = clog(SIScalarDoubleComplexValue(scalar));
                SIScalarRef result = SIScalarCreateWithDoubleComplex(value, NULL);
                if (result) OCAutorelease(result);
                return result;
            }
            if (errorString) {
                *errorString = STR("ln requires dimensionless unit.");
            }
            return NULL;
        }
        case BM_log: {
            if (SIDimensionalityIsDimensionless(SIQuantityGetUnitDimensionality((SIQuantityRef)scalar))) {
                SIScalarConvertToCoherentUnit((SIMutableScalarRef)scalar, errorString);
                double complex value = clog(SIScalarDoubleComplexValue(scalar)) / log(10);
                SIScalarRef result = SIScalarCreateWithDoubleComplex(value, NULL);
                if (result) OCAutorelease(result);
                return result;
            }
            if (errorString) {
                *errorString = STR("ln requires dimensionless unit.");
            }
            return NULL;
        }
        case BM_acos: {
            if (SIDimensionalityIsDimensionless(SIQuantityGetUnitDimensionality((SIQuantityRef)scalar))) {
                SIScalarConvertToCoherentUnit((SIMutableScalarRef)scalar, errorString);
                double complex value = cacos(SIScalarDoubleComplexValue(scalar));
                SIUnitRef unit = SIUnitWithSymbol(STR("rad"));
                SIScalarRef result = SIScalarCreateWithDoubleComplex(value, unit);
                if (result) OCAutorelease(result);
                return result;
            }
            if (errorString) {
                *errorString = STR("acos requires dimensionless unit.");
            }
            return NULL;
        }
        case BM_acosh: {
            if (SIDimensionalityIsDimensionless(SIQuantityGetUnitDimensionality((SIQuantityRef)scalar))) {
                SIScalarConvertToCoherentUnit((SIMutableScalarRef)scalar, errorString);
                double complex value = cacosh(SIScalarDoubleComplexValue(scalar));
                SIUnitRef unit = SIUnitWithSymbol(STR("rad"));
                SIScalarRef result = SIScalarCreateWithDoubleComplex(value, unit);
                if (result) OCAutorelease(result);
                return result;
            }
            if (errorString) {
                *errorString = STR("acosh requires dimensionless unit.");
            }
            return NULL;
        }
        case BM_asin: {
            if (SIDimensionalityIsDimensionless(SIQuantityGetUnitDimensionality((SIQuantityRef)scalar))) {
                SIScalarConvertToCoherentUnit((SIMutableScalarRef)scalar, errorString);
                double complex value = casin(SIScalarDoubleComplexValue(scalar));
                SIUnitRef unit = SIUnitWithSymbol(STR("rad"));
                SIScalarRef result = SIScalarCreateWithDoubleComplex(value, unit);
                if (result) OCAutorelease(result);
                return result;
            }
            if (errorString) {
                *errorString = STR("asin requires dimensionless unit.");
            }
            return NULL;
        }
        case BM_asinh: {
            if (SIDimensionalityIsDimensionless(SIQuantityGetUnitDimensionality((SIQuantityRef)scalar))) {
                SIScalarConvertToCoherentUnit((SIMutableScalarRef)scalar, errorString);
                SIUnitRef unit = SIUnitWithSymbol(STR("rad"));
                double complex value = casinh(SIScalarDoubleComplexValue(scalar));
                SIScalarRef result = SIScalarCreateWithDoubleComplex(value, unit);
                if (result) OCAutorelease(result);
                return result;
            }
            if (errorString) {
                *errorString = STR("asinh requires dimensionless unit.");
            }
            return NULL;
        }
        case BM_atan: {
            if (SIDimensionalityIsDimensionless(SIQuantityGetUnitDimensionality((SIQuantityRef)scalar))) {
                SIScalarConvertToCoherentUnit((SIMutableScalarRef)scalar, errorString);
                SIUnitRef unit = SIUnitWithSymbol(STR("rad"));
                double complex value = catan(SIScalarDoubleComplexValue(scalar));
                SIScalarRef result = SIScalarCreateWithDoubleComplex(value, unit);
                if (result) OCAutorelease(result);
                return result;
            }
            if (errorString) {
                *errorString = STR("atan requires dimensionless unit.");
            }
            return NULL;
        }
        case BM_atanh: {
            if (SIDimensionalityIsDimensionless(SIQuantityGetUnitDimensionality((SIQuantityRef)scalar))) {
                SIScalarConvertToCoherentUnit((SIMutableScalarRef)scalar, errorString);
                SIUnitRef unit = SIUnitWithSymbol(STR("rad"));
                double complex value = catanh(SIScalarDoubleComplexValue(scalar));
                SIScalarRef result = SIScalarCreateWithDoubleComplex(value, unit);
                if (result) OCAutorelease(result);
                return result;
            }
            if (errorString) {
                *errorString = STR("atanh requires dimensionless unit.");
            }
            return NULL;
        }
        case BM_cos: {
            if (SIDimensionalityIsDimensionless(SIQuantityGetUnitDimensionality((SIQuantityRef)scalar))) {
                SIScalarConvertToCoherentUnit((SIMutableScalarRef)scalar, errorString);
                double complex value = complex_cosine(SIScalarDoubleComplexValue(scalar));
                SIScalarRef result = SIScalarCreateWithDoubleComplex(value, NULL);
                if (result) OCAutorelease(result);
                return result;
            }
            if (errorString) {
                *errorString = STR("cos requires dimensionless unit.");
            }
            return NULL;
        }
        case BM_cosh: {
            if (SIDimensionalityIsDimensionless(SIQuantityGetUnitDimensionality((SIQuantityRef)scalar))) {
                SIScalarConvertToCoherentUnit((SIMutableScalarRef)scalar, errorString);
                double complex value = ccosh(SIScalarDoubleComplexValue(scalar));
                SIScalarRef result = SIScalarCreateWithDoubleComplex(value, NULL);
                if (result) OCAutorelease(result);
                return result;
            }
            if (errorString) {
                *errorString = STR("cosh requires dimensionless unit.");
            }
            return NULL;
        }
        case BM_sin: {
            if (SIDimensionalityIsDimensionless(SIQuantityGetUnitDimensionality((SIQuantityRef)scalar))) {
                SIScalarConvertToCoherentUnit((SIMutableScalarRef)scalar, errorString);
                double complex value = complex_sine(SIScalarDoubleComplexValue(scalar));
                SIScalarRef result = SIScalarCreateWithDoubleComplex(value, NULL);
                if (result) OCAutorelease(result);
                return result;
            }
            if (errorString) {
                *errorString = STR("sin requires dimensionless unit.");
            }
            return NULL;
        }
        case BM_sinh: {
            if (SIDimensionalityIsDimensionless(SIQuantityGetUnitDimensionality((SIQuantityRef)scalar))) {
                SIScalarConvertToCoherentUnit((SIMutableScalarRef)scalar, errorString);
                double complex value = csinh(SIScalarDoubleComplexValue(scalar));
                SIScalarRef result = SIScalarCreateWithDoubleComplex(value, NULL);
                if (result) OCAutorelease(result);
                return result;
            }
            if (errorString) {
                *errorString = STR("sinh requires dimensionless unit.");
            }
            return NULL;
        }
        case BM_tan: {
            if (SIDimensionalityIsDimensionless(SIQuantityGetUnitDimensionality((SIQuantityRef)scalar))) {
                SIScalarConvertToCoherentUnit((SIMutableScalarRef)scalar, errorString);
                double complex value = complex_tangent(SIScalarDoubleComplexValue(scalar));
                SIScalarRef result = SIScalarCreateWithDoubleComplex(value, NULL);
                if (result) OCAutorelease(result);
                return result;
            }
            if (errorString) {
                *errorString = STR("tan requires dimensionless unit.");
            }
            return NULL;
        }
        case BM_tanh: {
            if (SIDimensionalityIsDimensionless(SIQuantityGetUnitDimensionality((SIQuantityRef)scalar))) {
                SIScalarConvertToCoherentUnit((SIMutableScalarRef)scalar, errorString);
                double complex value = ctanh(SIScalarDoubleComplexValue(scalar));
                SIScalarRef result = SIScalarCreateWithDoubleComplex(value, NULL);
                if (result) OCAutorelease(result);
                return result;
            }
            if (errorString) {
                *errorString = STR("tanh requires dimensionless unit.");
            }
            return NULL;
        }
        case BM_conj: {
            SIScalarRef result = SIScalarCreateByConjugation(scalar);
            if (result) OCAutorelease(result);
            return result;
        }
        case BM_creal: {
            SIScalarRef result = SIScalarCreateByTakingComplexPart(scalar, kSIRealPart);
            if (result) OCAutorelease(result);
            return result;
        }
        case BM_cimag: {
            SIScalarRef result = SIScalarCreateByTakingComplexPart(scalar, kSIImaginaryPart);
            if (result) OCAutorelease(result);
            return result;
        }
        case BM_carg: {
            SIScalarRef result = SIScalarCreateByTakingComplexPart(scalar, kSIArgumentPart);
            if (result) OCAutorelease(result);
            return result;
        }
        case BM_cabs: {
            SIScalarRef result = SIScalarCreateByTakingComplexPart(scalar, kSIMagnitudePart);
            if (result) OCAutorelease(result);
            return result;
        }
        default: {
            if (errorString) {
                *errorString = STR("unknown function.");
            }
            return NULL;
        }
    }
}
SIUnitRef ConversionWithDefinedUnit(OCMutableStringRef mutString, double *unit_multiplier, OCStringRef *errorString) {
    if (errorString)
        if (*errorString)
            return NULL;
    // Remove appended unit conversion
    OCArrayRef conversions = OCStringCreateArrayBySeparatingStrings(mutString, STR(".."));
    if (conversions) {
        if (OCArrayGetCount(conversions) == 2) {
            OCStringRef firstString = OCArrayGetValueAtIndex(conversions, 0);
            OCStringRef secondString = OCArrayGetValueAtIndex(conversions, 1);
            if (OCStringGetLength(firstString) == 0 || OCStringGetLength(secondString) == 0) {
                OCRelease(conversions);
                return NULL;
            }
            SIUnitRef finalUnit = SIUnitFromExpression(secondString, unit_multiplier, errorString);
            if (finalUnit) {
                OCStringReplaceAll(mutString, firstString);
                OCRelease(conversions);
                return finalUnit;
            }
        }
        OCRelease(conversions);
    }
    return NULL;
}
