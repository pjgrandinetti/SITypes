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
| INTEGER {
    // Handle standalone integer (dimensionless unit)
    if ($1 == 1) {
        // Create dimensionless term with space symbol
        SIUnitTerm* dimensionless = siueCreateTerm(STR(" "), 1);
        OCMutableArrayRef termArray = OCArrayCreateMutable(1, NULL);
        OCArrayAppendValue(termArray, dimensionless);
        $$ = siueCreateExpression(termArray, NULL);
        OCRelease(termArray);
    } else {
        siueError = STR("Only '1' is allowed as a standalone number");
        YYERROR;
    }
}
| term_list '/' term_list {
    $$ = siueCreateExpression($1, $3);
}
| expression '/' unit_term {
    // For chained division like a/b/c, add c to denominator
    OCMutableArrayRef newDen = $1->denominator ? OCArrayCreateMutableCopy($1->denominator) : OCArrayCreateMutable(1, NULL);
    OCArrayAppendValue(newDen, $3);
    $$ = siueCreateExpression($1->numerator, newDen);
    OCRelease(newDen);
}
| expression '/' '(' term_list ')' {
    // For division by parenthetical expression like a/(b*c)
    OCMutableArrayRef newDen = $1->denominator ? OCArrayCreateMutableCopy($1->denominator) : OCArrayCreateMutable(1, NULL);
    OCIndex count = OCArrayGetCount($4);
    for (OCIndex i = 0; i < count; i++) {
        OCArrayAppendValue(newDen, OCArrayGetValueAtIndex($4, i));
    }
    $$ = siueCreateExpression($1->numerator, newDen);
    OCRelease(newDen);
}
| '(' expression ')' '^' INTEGER {
    // Handle parenthetical expression raised to power: (a/b)^n
    $$ = siueApplyPowerToExpression($2, $5);
}
| '(' expression ')' '^' '(' INTEGER ')' {
    // Handle parenthetical expression raised to parenthetical power: (a/b)^(n)
    $$ = siueApplyPowerToExpression($2, $6);
}
| '(' expression ')' {
    // Handle parenthetical expression without power: (a/b)
    $$ = $2;
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
| term_list '*' '(' term_list ')' {
    // Handle multiplication with parenthetical expression: term_list * (term_list)
    OCIndex count = OCArrayGetCount($4);
    for (OCIndex i = 0; i < count; i++) {
        OCArrayAppendValue((OCMutableArrayRef)$1, OCArrayGetValueAtIndex($4, i));
    }
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
| UNIT_SYMBOL '^' '(' INTEGER ')' {
    $$ = siueCreateTerm($1, $4);
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
