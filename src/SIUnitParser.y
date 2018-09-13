%{
    #include "SILibrary.h"
    
    void yyerror(char *s, ...);
    static SIUnitRef final_unit;
    OCStringRef unitError;
    static double *unit_multiplier_ref;
    int psulex(void);
    %}

%name-prefix="psu"

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
calclist: /* do nothing */
| calclist exp {final_unit = $2;}
;

exp: '(' exp ')' {$$ = $2;}
| exp '*' exp {$$ = SIUnitByMultiplyingWithoutReducing($1,$3,unit_multiplier_ref, &unitError);}
| exp '/' exp {$$ = SIUnitByDividingWithoutReducing($1,$3,unit_multiplier_ref);}
| exp '^' INTEGER {$$ = SIUnitByRaisingToAPowerWithoutReducing($1,$3,unit_multiplier_ref, &unitError);}
| INTEGER '/' exp {
    if($1 == 1) {$$ = SIUnitByRaisingToAPowerWithoutReducing($3,-1,unit_multiplier_ref, &unitError);}
    else  {
        unitError = STR("Unknown unit symbol");
        yyerror("Unknown unit symbol");
    }
}
| UNIT
;

%%

extern int psu_scan_string(const char *);
extern void psulex_destroy(void);
bool psu_syntax_error;

SIUnitRef SIUnitForParsedSymbol(OCStringRef string, double *unit_multiplier, OCStringRef *error)
{
    if(error) if(*error) return NULL;
    
    OCMutableStringRef  mutString = OCStringCreateMutableCopy(string);
    OCStringTrimWhitespace (mutString);
    if(OCStringGetLength(string) == 1 && (
    (OCStringCompare(string,STR("1"),0) == kOCCompareEqualTo) ||(OCStringCompare(string,STR(" "),0) == kOCCompareEqualTo)
    )) {
        OCRelease(mutString);
        return SIUnitDimensionlessAndUnderived();
    }
    
    OCStringFindAndReplace2(mutString,STR("•"),STR("*"));
    
    
    
    final_unit = NULL;
    unitError = NULL;
    unit_multiplier_ref = unit_multiplier;
    psu_syntax_error = false;
    uint64_t length = OCStringGetLength(mutString);
    if(length) {
        const char *cString = OCStringGetCString(mutString);
        psu_scan_string(cString);
        psuparse();
        psulex_destroy();
        OCRelease(mutString);
    }
    if(unitError) *error = unitError;
    return final_unit;
}

void yyerror(char *s, ...)
{
    fprintf(stderr, "error: %s\n",s);
    psu_syntax_error = true;
}


