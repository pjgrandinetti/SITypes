%{
    #include <OCTypes/OCLibrary.h>
    #include "SILibrary.h"
    #include "SIScalarParser.h"
    #include <ctype.h>
    void siserror(char *s, ...);
    SIScalarRef result;
    OCStringRef scalarError;
    int  sislex(void);
    int  sisparse(void);
%}

%name-prefix="sis"

%union {
    ScalarNodeRef            a;
    SIScalarRef              d;
    struct symbol           *s;
    builtInMathFunctions     math_fn;
    builtInConstantFunctions const_fn;
    OCMutableStringRef       const_string;
}

/* declare tokens */
%token <d>            SCALAR
%token <math_fn>      MATH_FUNC
%token <const_fn>     CONST_FUNC CO      /* accept both modern and legacy names */
%token <const_string> CONST_STRING
%token                EOL

%left   '='
%left   '+' '-'
%left   '*' '/'
%right  '^'
%right  '!'
%nonassoc '|' UMINUS

%type <a> exp explist

%%

calclist:
      /* empty */
    | calclist exp
      {
          result = ScalarNodeEvaluate($2, &scalarError);
          if (!ScalarNodeisLeaf($2))
              ScalarNodeFree($2);
      }
    ;

exp:
      exp '+' exp     { $$ = ScalarNodeCreateInnerNode('+', $1, $3); }
    | exp '-' exp     { $$ = ScalarNodeCreateInnerNode('-', $1, $3); }
    | exp '*' exp     { $$ = ScalarNodeCreateInnerNode('*', $1, $3); }
    | exp '/' exp     { $$ = ScalarNodeCreateInnerNode('/', $1, $3); }
    | exp '^' exp     { $$ = ScalarNodeCreateInnerNode('^', $1, $3); }
    | '|' exp '|'     { $$ = ScalarNodeCreateInnerNode('|', $2, NULL); }
    | '(' exp ')'     { $$ = $2; }
    | '-' exp %prec UMINUS
                      { $$ = ScalarNodeCreateInnerNode('M', $2, NULL); }
    | exp '!'         { $$ = ScalarNodeCreateInnerNode('!', $1, NULL); }
    | SCALAR          { if ($1 == NULL) YYERROR; $$ = ScalarNodeCreateNumberLeaf($1); }
    | MATH_FUNC '(' explist ')' 
                      { $$ = ScalarNodeCreateMathFunction($1, $3); }
    | CONST_FUNC CONST_STRING
                      { $$ = ScalarNodeCreateConstantFunction($1, $2); }
    | CO CONST_STRING
                      { $$ = ScalarNodeCreateConstantFunction($1, $2); }
    ;

explist:
      exp
    | exp ',' explist
      { $$ = ScalarNodeCreateInnerNode('L', $1, $3); }
    ;

%%

extern int  sis_scan_string(const char *);
extern void sislex_destroy(void);
bool       sis_syntax_error;

SIScalarRef
SIScalarCreateWithOCString(OCStringRef string, OCStringRef *error)
{
    fprintf(stderr,"Just entered SIScalarCreateWithOCString\n");

    if (OCStringCompare(string, kSIQuantityDimensionless, kOCCompareCaseInsensitive)
        == kOCCompareEqualTo)
    {
        return NULL;
    }

    OCMutableStringRef mutString = OCStringCreateMutableCopy(string);
    fprintf(stderr,"Just OCStringCreateMutableCopy\n");

    /* perform all Unicode-aware replacements */
    OCStringFindAndReplace(mutString, STR("*"), STR("•"),
        OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("•"), STR("*"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("×"), STR("*"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("÷"), STR("/"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("−"), STR("-"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("\n"), STR(""), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("+"), STR("+"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("μ"), STR("µ"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("γ"), STR("𝛾"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("º"), STR("°"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("h_p"), STR("h_P"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("ɣ"), STR("𝛾"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("√"), STR("sqrt"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("∛"), STR("cbrt"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("∜"), STR("qtrt"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR(" "), STR(""), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    // Quick fix for quartertsp
    OCStringFindAndReplace(mutString, STR("qtertsp"), STR("quartertsp"), OCRangeMake(0, OCStringGetLength(mutString)), kOCCompareCaseInsensitive);

    fprintf(stderr,"All OCStringFindAndReplace\n");


    // Insert '*' for implicit multiplication
    {
        OCMutableStringRef tmp = OCStringCreateMutableCopy(mutString);
        OCRelease(mutString);
        mutString = tmp;

        OCStringShow(mutString);

        // Now drive *both* the lookups and inserts on `mutString`
        size_t len = OCStringGetLength(mutString);
        for (int idx = (int)len - 2; idx >= 0; --idx) {
            char prev = OCStringGetCharacterAtIndex(mutString, idx);
            char next = OCStringGetCharacterAtIndex(mutString, idx + 1);
            if (characterIsDigitOrDecimalPoint(prev) && isalpha((unsigned char)next)) {
                OCStringInsert(mutString, idx + 1, STR("*"));
            }
        }

        OCStringShow(mutString);
    }

    OCStringFindAndReplace(mutString, STR(")("), STR(")*("),
                           OCRangeMake(0, OCStringGetLength(mutString)), 0);
    OCStringFindAndReplace(mutString, STR("Ɑ"), STR("α"),
                           OCRangeMake(0, OCStringGetLength(mutString)), 0);

    // Handle implicit multiplication around parentheses
    OCArrayRef openParentheses = OCStringCreateArrayWithFindResults(
        mutString, STR("("), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    if (openParentheses) {
        OCMutableStringRef mutStringNew = OCStringCreateMutableCopy(mutString);
        for (int index = OCArrayGetCount(openParentheses) - 1; index >= 0; index--) {
            OCRange *range = (OCRange *)OCArrayGetValueAtIndex(openParentheses, index);
            if (range->location > 0 && range->location < OCStringGetLength(mutString)) {
                char prev = OCStringGetCharacterAtIndex(mutString, range->location - 1);
                bool closeBr = false, skip = false;
                for (int j = range->location - 1; j >= 0; j--) {
                    char c = OCStringGetCharacterAtIndex(mutString, j);
                    if (c == '[') { if (!closeBr) skip = true; }
                    if (c == ']') closeBr = true;
                }
                if (!skip && characterIsDigitOrDecimalPoint(prev)) {
                    OCStringInsert(mutStringNew, range->location, STR("*"));
                }
            }
        }
        OCRelease(mutString);
        mutString = mutStringNew;
        OCRelease(openParentheses);
    }

    OCArrayRef closeParentheses = OCStringCreateArrayWithFindResults(
        mutString, STR(")"), OCRangeMake(0, OCStringGetLength(mutString)), 0);
    if (closeParentheses) {
        OCMutableStringRef mutStringNew = OCStringCreateMutableCopy(mutString);
        for (int index = OCArrayGetCount(closeParentheses) - 1; index >= 0; index--) {
            OCRange *range = (OCRange *)OCArrayGetValueAtIndex(closeParentheses, index);
            if (range->location < OCStringGetLength(mutString) - 1) {
                unsigned char next = (unsigned char)OCStringGetCharacterAtIndex(mutString, range->location + 1);
                bool openBr = false, skip = false;
                for (int j = range->location + 1; j < OCStringGetLength(mutString); j++) {
                    unsigned char c = (unsigned char)OCStringGetCharacterAtIndex(mutString, j);
                    if (c == ']') { if (!openBr) skip = true; }
                    if (c == '[') openBr = true;
                }
                if (!skip) {
                    if (next != '+' && next != '-' && next != '*' && next != '/' && next != '^' && next != ')' && next != 0xE2) {
                        OCStringInsert(mutStringNew, range->location + 1, STR("*"));
                    }
                }
            }
        }
        OCRelease(mutString);
        mutString = mutStringNew;
        OCRelease(closeParentheses);
    }

// Ready to Parse    /* check for and get the final conversion unit */
    double unit_multiplier = 1.0;
    SIUnitRef finalUnit = ConversionWithDefinedUnit(mutString, &unit_multiplier, &scalarError);

    sis_syntax_error = false;
    const char *cString = OCStringGetCString(mutString);
    if (cString) {
        sis_scan_string(cString);
        sisparse();
        sislex_destroy();
        OCRelease(mutString);
    }
    if (error) {
        if (scalarError) *error = scalarError;
        if (*error) {
            if (result) OCRelease(result);
            return NULL;
        }
    }

    if (result) {
        /* unit conversion and real-part extraction logic */
        if (finalUnit) {
            if (!SIScalarConvertToUnit((SIMutableScalarRef)result, finalUnit, error)) {
                OCRelease(result);
                return NULL;
            }
        }
        // If the result is real-only, extract its real component
        if (SIScalarIsReal(result)) {
            SIScalarRef realResult = SIScalarCreateByTakingComplexPart(result, kSIRealPart);
            OCRelease(result);
            return realResult;
        }
    } else {
        if (error) *error = STR("Syntax Error");
    }
    return result;
}

void
siserror(char *s, ...)
{
    scalarError = STR("Syntax Error");
    sis_syntax_error = true;
}

