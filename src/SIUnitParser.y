%{
    #include <stdio.h>
    #include "SILibrary.h"
    #include "SIUnitParser.h"
    
    void yyerror(char *s, ...);
    static SIUnitRef final_unit;
    OCStringRef unitError;
    static double *unit_multiplier_ref;
    int siulex(void);
    %}

%name-prefix="siu"

%union {
    SIUnitRef unit;
    int    iVal;
}

%token <unit> UNIT
%token <iVal> INTEGER
%type <unit> exp calclist
%left '*' '/'
%left '^'
%%
calclist: /* do nothing */ { $$ = NULL; }
| calclist exp {final_unit = $2;}
;

exp: '(' exp ')' {$$ = $2;}
| exp '*' exp {$$ = SIUnitByMultiplyingWithoutReducing($1,$3,unit_multiplier_ref, &unitError);}
| exp '/' exp {$$ = SIUnitByDividingWithoutReducing($1,$3,unit_multiplier_ref);}
| exp '^' INTEGER {$$ = SIUnitByRaisingToPowerWithoutReducing($1,$3,unit_multiplier_ref, &unitError);}
| INTEGER '/' exp {
    if($1 == 1) {$$ = SIUnitByRaisingToPowerWithoutReducing($3,-1,unit_multiplier_ref, &unitError);}
    else  {
        unitError = STR("Unknown unit symbol");
        yyerror("Unknown unit symbol");
    }
}
| UNIT
;

%%

extern int siu_scan_string(const char *);
extern void siulex_destroy(void);
bool siu_syntax_error;

SIUnitRef SIUnitFromExpressionInternal(OCStringRef string, double *unit_multiplier, OCStringRef *error)
{
    if(error) if(*error) return NULL;
    
    // Use the shared normalization function for parsing
    OCMutableStringRef mutString = SIUnitCreateNormalizedExpression(string, false);
    if(OCStringGetLength(string) == 1 && (
    (OCStringCompare(string,STR("1"),0) == kOCCompareEqualTo) ||(OCStringCompare(string,STR(" "),0) == kOCCompareEqualTo)
    )) {
        OCRelease(mutString);
        return SIUnitDimensionlessAndUnderived();
    }
    
    final_unit = NULL;
    unitError = NULL;
    unit_multiplier_ref = unit_multiplier;
    siu_syntax_error = false;
    uint64_t length = OCStringGetLength(mutString);
    if(length) {
        const char *cString = OCStringGetCString(mutString);
        siu_scan_string(cString);
        siuparse();
        siulex_destroy();
        OCRelease(mutString);
    }
    if(unitError) *error = unitError;
    return final_unit;
}

void yyerror(char *s, ...)
{
    fprintf(stderr, "error: %s\n",s);
    siu_syntax_error = true;
}


