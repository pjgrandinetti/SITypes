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
    double dVal;
    SIUnitTerm* term;
    OCArrayRef term_list;
    SIUnitExpression* expression;
}

%token <unit_symbol> UNIT_SYMBOL
%token <iVal> INTEGER
%token <dVal> DECIMAL
%token UNKNOWN_SYMBOL

%type <term> unit_term
%type <term_list> term_list
%type <expression> expression

%left '*' '/'
%right '^'

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
| INTEGER '/' term_list {
    // Handle expressions like "1/m" - numerator is empty, denominator has the terms
    if ($1 == 1) {
        // Create empty numerator, put terms in denominator
        OCMutableArrayRef emptyNum = OCArrayCreateMutable(0, NULL);
        $$ = siueCreateExpression(emptyNum, $3);
        OCRelease(emptyNum);
    } else {
        siueError = STR("Only '1' is allowed as a numeric coefficient");
        YYERROR;
    }
}
| INTEGER '/' '(' term_list ')' {
    // Handle expressions like "1/(m*s)" - numerator is empty, denominator has the terms
    if ($1 == 1) {
        // Create empty numerator, put terms in denominator
        OCMutableArrayRef emptyNum = OCArrayCreateMutable(0, NULL);
        $$ = siueCreateExpression(emptyNum, $4);
        OCRelease(emptyNum);
    } else {
        siueError = STR("Only '1' is allowed as a numeric coefficient");
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
| '(' expression ')' '^' DECIMAL {
    // Handle parenthetical expression raised to decimal power: (a/b)^0.5
    $$ = siueApplyFractionalPowerToExpression($2, $5);
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
| '(' term_list ')' '^' DECIMAL {
    // Handle decimal power for term lists too
    // Convert term_list to expression first, then apply fractional power
    SIUnitExpression* temp_expr = siueCreateExpression($2, NULL);
    if (temp_expr) {
        SIUnitExpression* result = siueApplyFractionalPowerToExpression(temp_expr, $5);
        if (result && result->numerator) {
            $$ = result->numerator;
            OCRetain($$); // Keep the numerator
        } else {
            $$ = NULL;
        }
        OCRelease(temp_expr);
        if (result) OCRelease(result);
    } else {
        $$ = NULL;
    }
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
| UNKNOWN_SYMBOL {
    siueError = STR("Unknown unit symbol");
    YYERROR;
}
;

%%

void siueerror(char *s, ...) {
    if (!siueError) {
        siueError = OCStringCreateWithCString(s);
    }
}
