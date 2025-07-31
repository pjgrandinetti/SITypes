%{
    #include <stdio.h>
    #include "SILibrary.h"
    #include "SIUnitExpression.h"
    
    void siueerror(char *s, ...);
    
    // External variables and functions defined in SIUnitExpression.c
    extern OCStringRef siueError;
    
    // External lexer function
    int siuelex(void);
%}

%name-prefix="siue"

%union {
    OCStringRef unit_symbol;
    int iVal;
    SIUnitTerm* term;
    OCArrayRef term_list;
    SIUnitExpression* expression;
}

%token <unit_symbol> UNIT_SYMBOL
%token <iVal> INTEGER
%token DECIMAL UNKNOWN_SYMBOL

%type <term> unit_term
%type <term_list> term_list
%type <expression> expression

%left '*' '/'
%left '^'

%%

/* Grammar rules */
input: expression { siueSetParsedExpression($1); }
     ;

expression: term_list { 
    $$ = siueCreateExpression($1, NULL);
}
| term_list '/' term_list {
    $$ = siueCreateExpression($1, $3);
}
;

term_list: unit_term {
    $$ = (OCArrayRef)OCArrayCreateMutable(0, NULL);
    OCArrayAppendValue((OCMutableArrayRef)$$, $1);
}
| term_list '*' unit_term {
    OCArrayAppendValue((OCMutableArrayRef)$1, $3);
    $$ = $1;
}
| '(' term_list ')' {
    $$ = $2;
}
| '(' term_list ')' '^' INTEGER {
    $$ = siueApplyPowerToTermList($2, $5);
}
;

unit_term: UNIT_SYMBOL {
    $$ = siueCreateTerm($1, 1);
}
| UNIT_SYMBOL '^' INTEGER {
    $$ = siueCreateTerm($1, $3);
}
| UNIT_SYMBOL '^' DECIMAL {
    siueError = STR("Fractional powers are not allowed");
    YYERROR;
}
| UNKNOWN_SYMBOL {
    siueError = STR("Unknown unit symbol");
    YYERROR;
}
| INTEGER '/' term_list {
    if ($1 == 1) {
        // Handle 1/expression case
        $$ = NULL; // This will be handled specially
    } else {
        siueError = STR("Only '1' is allowed as a numeric coefficient");
        YYERROR;
    }
}
;

%%

void siueerror(char *s, ...) {
    if (!siueError) {
        siueError = OCStringCreateWithCString(s);
    }
}
