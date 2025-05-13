%{
    #include <stdio.h>
    #include "SILibrary.h"
    #include "SIScalarParser.h"
    void siserror(char *s, ...);
    SIScalarRef result;
    OCStringRef scalarErrorString;
    int sislex(void);
    %}

%name-prefix="sis"

%union {
    ScalarNodeRef a;
    SIScalarRef d;
    struct symbol *s;
    builtInMathFunctions math_fn;
    builtInConstantFunctions const_fn;
    OCMutableStringRef const_string;
}

/* declare tokens */
%token <d> SCALAR
%token <math_fn> MATH_FUNC
%token <const_fn> CONST_FUNC
%token <const_string> CONST_STRING
%token EOL
%left '='
%left '+' '-'
%left '*' '/'
%right '^'
%right '!'
%nonassoc '|' UMINUS

%type <a> exp explist

/* may 19th, 2013 removed line below from exp: (was after exp '*' exp) */

%%
calclist:   /* do nothing */
| calclist exp EOL {
    result = ScalarNodeEvaluate($2, &scalarErrorString);
    if(!ScalarNodeisLeaf($2)) ScalarNodeFree($2);
}
;

exp: exp '+' exp {$$ = ScalarNodeCreateInnerNode('+',$1, $3);}
| exp '-' exp {$$ = ScalarNodeCreateInnerNode('-',$1, $3);}
| exp '*' exp {$$ = ScalarNodeCreateInnerNode('*',$1, $3);}
| exp '/' exp {$$ = ScalarNodeCreateInnerNode('/',$1, $3);}
| exp '^' exp {$$ = ScalarNodeCreateInnerNode('^',$1, $3);}
| '|' exp '|' {$$ = ScalarNodeCreateInnerNode('|',$2, NULL);}
| '(' exp ')' {$$ = $2;}
| '-' exp %prec UMINUS{$$ = ScalarNodeCreateInnerNode('M',$2, NULL);}
| exp '!' {$$ = ScalarNodeCreateInnerNode('!',$1, NULL);}
| SCALAR    {if($1==NULL) {YYERROR;} $$ = ScalarNodeCreateNumberLeaf($1);}
| MATH_FUNC '(' explist ')' {$$ = ScalarNodeCreateMathFunction($1,$3);}
| CONST_FUNC CONST_STRING {$$ = ScalarNodeCreateConstantFunction($1,$2);}
;

explist: exp
| exp ',' explist   {$$ = ScalarNodeCreateInnerNode('L',$1,$3);}

%%

extern int sis_scan_string(const char *);
extern void sislex_destroy(void);
bool sis_syntax_error;

SIScalarRef SIScalarCreateWithOCString(OCStringRef string, OCStringRef *errorString)
{
    if(errorString) if(*errorString) return NULL;
    if(OCStringCompare(string,kSIQuantityDimensionless,kOCCompareCaseInsensitive) == kOCCompareEqualTo) return NULL;
    
    OCMutableStringRef  mutString = OCStringCreateMutableCopy ( string);
    
    OCStringFindAndReplace (mutString,STR("*"), STR("•"),OCRangeMake(0,OCStringGetLength(mutString)),0);
    
    result = SIScalarCreateWithStringContainingSingleUnitFromLibrary(mutString);
    if(result) return result;
    
    
    result = NULL;
    scalarErrorString = NULL;
    // check for and get the final conversion unit
    double unit_multiplier = 1.0;
    SIUnitRef finalUnit = ConversionWithDefinedUnit(mutString, &unit_multiplier, errorString);
    
    OCStringFindAndReplace (mutString,STR("•"), STR("*"),OCRangeMake(0,OCStringGetLength(mutString)),0);
    OCStringFindAndReplace (mutString,STR("×"), STR("*"),OCRangeMake(0,OCStringGetLength(mutString)),0);
    OCStringFindAndReplace (mutString,STR("÷"), STR("/"),OCRangeMake(0,OCStringGetLength(mutString)),0);
    OCStringFindAndReplace (mutString,STR("−"), STR("-"),OCRangeMake(0,OCStringGetLength(mutString)),0);
    OCStringFindAndReplace (mutString,STR("\n"), STR(""),OCRangeMake(0,OCStringGetLength(mutString)),0);
    OCStringFindAndReplace (mutString,STR("+"), STR("+"),OCRangeMake(0,OCStringGetLength(mutString)),0);
    OCStringFindAndReplace (mutString,STR("μ"), STR("µ"),OCRangeMake(0,OCStringGetLength(mutString)),0);
    OCStringFindAndReplace (mutString,STR("γ"), STR("𝛾"),OCRangeMake(0,OCStringGetLength(mutString)),0);
    OCStringFindAndReplace (mutString,STR("º"), STR("°"),OCRangeMake(0,OCStringGetLength(mutString)),0);
    OCStringFindAndReplace (mutString,STR("h_p"), STR("h_P"),OCRangeMake(0,OCStringGetLength(mutString)),0);
    OCStringFindAndReplace (mutString,STR("ɣ"), STR("𝛾"),OCRangeMake(0,OCStringGetLength(mutString)),0);
    OCStringFindAndReplace (mutString,STR("√"), STR("sqrt"),OCRangeMake(0,OCStringGetLength(mutString)),0);
    OCStringFindAndReplace (mutString,STR("∛"), STR("cbrt"),OCRangeMake(0,OCStringGetLength(mutString)),0);
    OCStringFindAndReplace (mutString,STR("∜"), STR("qtrt"),OCRangeMake(0,OCStringGetLength(mutString)),0);
    OCStringFindAndReplace (mutString,STR(" "), STR(""),OCRangeMake(0,OCStringGetLength(mutString)),0);
    OCStringFindAndReplace (mutString,STR(")("), STR(")*("),OCRangeMake(0,OCStringGetLength(mutString)),0);
    OCStringFindAndReplace (mutString,STR("Ɑ"), STR("α"),OCRangeMake(0,OCStringGetLength(mutString)),0);
    
    // Replace unit name with unit symbol
    
    
    OCArrayRef sortedUnits = SIUnitGetUnitsSortedByNameLength();
    for(int64_t index=0;index<OCArrayGetCount(sortedUnits);index++) {
        SIUnitRef unit = OCArrayGetValueAtIndex(sortedUnits,index);
        OCStringRef symbol = SIUnitCopySymbol(unit);
        OCStringRef pluralName = SIUnitCreatePluralName(unit);
        if(pluralName) {
            OCStringFindAndReplace(mutString, pluralName, symbol, OCRangeMake(0, OCStringGetLength(mutString)), kOCCompareCaseInsensitive);
            OCRelease(pluralName);
        }
        OCStringRef name = SIUnitCreateName(unit);
        if(name) {
            OCStringFindAndReplace (mutString,name, symbol,OCRangeMake(0,OCStringGetLength(mutString)),kOCCompareCaseInsensitive);
            OCRelease(name);
        }
        OCRelease(symbol);
    }
    
    // Quick fix for quartertsp
    OCStringFindAndReplace (mutString,STR("qtertsp"), STR("quartertsp"),OCRangeMake(0,OCStringGetLength(mutString)),kOCCompareCaseInsensitive);
    
    OCArrayRef openParentheses = OCStringCreateArrayWithFindResults(mutString,STR("("),OCRangeMake(0,OCStringGetLength(mutString)),0);
    if(openParentheses) {
        OCMutableStringRef  mutStringNew = OCStringCreateMutableCopy ( mutString);
        for(int64_t index = OCArrayGetCount(openParentheses)-1; index>=0;index--) {
            OCRange *range = (OCRange *) OCArrayGetValueAtIndex(openParentheses,index);
            if(range->location>0 && range->location<OCStringGetLength(mutString)) {
                char previousCharacter = OCStringGetCharacterAtIndex(mutString,range->location-1);
                // Don't insert asterisk if it's a string inside [ ]
                bool closeSquareBracket = false;
                bool skipThis = false;
                for(int64_t j=range->location-1; j>=0;j--) {
                    char scanChar = OCStringGetCharacterAtIndex(mutString,j);
                    if(scanChar=='[') {
                        if(!closeSquareBracket) skipThis = true;
                    }
                    if(scanChar==']') closeSquareBracket = true;
                }
                
                if(!skipThis && characterIsDigitOrDecimalPoint(previousCharacter)) OCStringInsert(mutStringNew, range->location, STR("*"));
            }
        }
        OCRelease(mutString);
        mutString = mutStringNew;
        OCRelease(openParentheses);
    }
    
    OCArrayRef closeParentheses = OCStringCreateArrayWithFindResults(mutString,STR(")"),OCRangeMake(0,OCStringGetLength(mutString)),0);
    if(closeParentheses) {
        OCMutableStringRef  mutStringNew = OCStringCreateMutableCopy (mutString);
        for(int64_t index = OCArrayGetCount(closeParentheses)-1; index>=0;index--) {
            OCRange *range = (OCRange *) OCArrayGetValueAtIndex(closeParentheses,index);
            if(range->location<OCStringGetLength(mutString)-1) {
                char nextCharacter = OCStringGetCharacterAtIndex(mutString,range->location+1);
                // Don't insert asterisk if it's a string inside [ ]
                bool openSquareBracket = false;
                bool skipThis = false;
                for(int64_t j=range->location+1; j<OCStringGetLength(mutString);j++) {
                    char scanChar = OCStringGetCharacterAtIndex(mutString,j);
                    if(scanChar==']') {
                        if(!openSquareBracket) skipThis = true;
                    }
                    if(scanChar=='[') openSquareBracket = true;
                }
                if(!skipThis) {
                    if((int)nextCharacter !='+' && (int)nextCharacter !='-'
                    && (int)nextCharacter !='*' && (int)nextCharacter !='/'
                    && (int)nextCharacter !='^'  && (int)nextCharacter !=')'
                    && (int)nextCharacter !=8226) OCStringInsert(mutStringNew, range->location+1, STR("*"));
                }
                
            }
        }
        OCRelease(mutString);
        mutString = mutStringNew;
        OCRelease(closeParentheses);
    }
    
    // Ready to Parse
    
    sis_syntax_error = false;
    const char *cString = OCStringGetCString(mutString);
    if(cString) {
        sis_scan_string(cString);
        sisparse();
        sislex_destroy();
        OCRelease(mutString);
    }
    if(errorString) {
        if(scalarErrorString) *errorString = scalarErrorString;
        if(*errorString) {
            if(result) OCRelease(result);
            return NULL;
        }
    }
    
    if(result) {
        if(finalUnit) {
            if(!SIScalarConvertToUnit( (SIMutableScalarRef) result, finalUnit, errorString)) {
                OCRelease(result);
                return NULL;
            }
        }
        
        if(SIScalarIsReal(result)) {
            SIScalarRef realResult = SIScalarCreateByTakingComplexPart(result,kSIRealPart);
            OCRelease(result);
            return realResult;
        }
    }
    else {
        if(errorString) {
            *errorString = STR("Syntax Error");
        }
    }
    
    return result;
}

void siserror(char *s, ...)
{
    scalarErrorString = STR("Syntax Error");
    sis_syntax_error = true;
}
