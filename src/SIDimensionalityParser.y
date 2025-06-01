%{
    #include <stdio.h>
    #include "SILibrary.h"
    #include "SIDimensionalityParser.h"

    void yyerror(char *s, ...);
    static SIDimensionalityRef final_dimensionality;
    OCStringRef dimensionalityError;
    int sidlex(void);
    
    %}

%name-prefix="sid"

%union {
    SIDimensionalityRef dimensionality;
    int    iVal;
}

%token <dimensionality> DIMENSIONALITY
%token <iVal> INTEGER
%type <dimensionality> exp calclist
%left '*' '/'
%left '^'
%%
calclist: /* do nothing */ { $$ = NULL; }
| calclist exp {final_dimensionality = $2;}
;

exp: '(' exp ')' {$$ = $2;}
| exp '*' exp {$$ = SIDimensionalityByMultiplyingWithoutReducing($1,$3,&dimensionalityError);}
| exp '/' exp {$$ = SIDimensionalityByDividingWithoutReducing($1,$3);}
| exp '^' INTEGER {$$ = SIDimensionalityByRaisingToAPowerWithoutReducing($1,$3,&dimensionalityError);}
| INTEGER '/' exp {
    if($1 == 1) {$$ = SIDimensionalityByRaisingToAPowerWithoutReducing($3,-1,&dimensionalityError);}
    else  {
        dimensionalityError = STR("Unknown dimensionality symbol");
        yyerror("Unknown unit symbol");
    }
}
| DIMENSIONALITY
;

%%

extern int sid_scan_string(const char *);
extern void sidlex_destroy(void);

bool sid_syntax_error;
SIDimensionalityRef SIDimensionalityForSymbol(OCStringRef string, OCStringRef *error)
{
    if(error) if(*error) return NULL;

    OCMutableStringRef  mutString = OCStringCreateMutableCopy(string);
    OCStringTrimWhitespace(mutString);
    
    if(OCStringGetLength(mutString) == 1) {
        SIDimensionalityRef result = SIDimensionalityWithBaseDimensionSymbol(mutString,error);
        OCRelease(mutString);
        return result;
    }
    
    OCStringFindAndReplace2(mutString,STR("•"),STR("*"));
    OCStringFindAndReplace2(mutString,STR("ϴ"), STR("@"));

    final_dimensionality = NULL;
    dimensionalityError = NULL;
    sid_syntax_error = false;
    uint64_t length = OCStringGetLength(mutString);
    if(length) {
        const char *cString = OCStringGetCString(mutString);
        
        sid_scan_string(cString);
        sidparse();
        sidlex_destroy();
        OCRelease(mutString);
    }
    if(dimensionalityError) *error = dimensionalityError;
    
    return final_dimensionality;
}

void yyerror(char *s, ...)
{
    fprintf(stderr, "error: %s\n",s);
    sid_syntax_error = true;
}


