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
    siueReleaseTermArray($1);  // Release the term array and its terms since siueCreateExpression copied them
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
        siueReleaseTerm(dimensionless);  // Release the term after copying into expression
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
        siueReleaseTermArray($3);  // Release the term_list and its terms since siueCreateExpression copied them
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
        siueReleaseTermArray($4);  // Release the term_list and its terms since siueCreateExpression copied them
    } else {
        siueError = STR("Only '1' is allowed as a numeric coefficient");
        YYERROR;
    }
}
| term_list '/' term_list {
    $$ = siueCreateExpression($1, $3);
    siueReleaseTermArray($1);  // Release since siueCreateExpression copied it
    siueReleaseTermArray($3);  // Release since siueCreateExpression copied it
}
| expression '/' unit_term {
    // For chained division like a/b/c, add c to denominator
    OCMutableArrayRef newDen = $1->denominator ? OCArrayCreateMutableCopy($1->denominator) : OCArrayCreateMutable(1, NULL);
    OCArrayAppendValue(newDen, $3);
    $$ = siueCreateExpression($1->numerator, newDen);
    OCRelease(newDen);  // Just release the array, not the terms (they're copied by siueCreateExpression)
    siueReleaseTerm($3);  // Release the unit_term since it was copied into the expression
    siueRelease($1);  // Free the intermediate expression
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
    siueRelease($1);  // Free the intermediate expression
    siueReleaseTermArray($4);    // Release the term_list and its terms since they were copied
}
| '(' expression ')' '^' INTEGER {
    // Handle parenthetical expression raised to power: (a/b)^n
    $$ = siueApplyPowerToExpression($2, $5);
}
| '(' expression ')' '^' DECIMAL {
    // Handle parenthetical expression raised to decimal power: (a/b)^0.5
    $$ = siueApplyFractionalPowerToExpression($2, $5);
    if (!$$) {
        // If fractional power was rejected, we need to clean up the expression
        siueRelease($2);
    }
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
    // We need to copy the terms from $4 to $1, then release $4 properly
    OCIndex count = OCArrayGetCount($4);
    for (OCIndex i = 0; i < count; i++) {
        SIUnitTerm* term = (SIUnitTerm*)OCArrayGetValueAtIndex($4, i);
        SIUnitTerm* termCopy = siueCopyTerm(term);  // Make a copy for $1
        OCArrayAppendValue((OCMutableArrayRef)$1, termCopy);
    }
    $$ = $1;
    siueReleaseTermArray($4);  // Release the source array and its terms
}
| '(' term_list ')' {
    $$ = $2;
}
| '(' term_list ')' '^' INTEGER {
    $$ = siueApplyPowerToTermList($2, $5);
}
| '(' term_list ')' '^' DECIMAL {
    // Handle decimal power for term lists - convert to expression, apply power, extract numerator
    SIUnitExpression* temp_expr = siueCreateExpression($2, NULL);
    if (temp_expr && siueApplyFractionalPowerToExpression(temp_expr, $5)) {
        // Extract the numerator from the modified expression
        $$ = temp_expr->numerator;
        OCRetain($$);  // Keep the numerator
        // Clean up the expression wrapper without freeing the numerator
        if (temp_expr->denominator) OCRelease(temp_expr->denominator);
        free(temp_expr);
    } else {
        $$ = NULL;
        if (temp_expr) siueRelease(temp_expr);
    }
    // $2 was already copied by siueCreateExpression, so we still need to clean it up
    siueReleaseTermArray($2);
}
;

unit_term: UNIT_SYMBOL {
    $$ = siueCreateTerm($1, 1);
    OCRelease($1);  // Release the string from lexer since siueCreateTerm copied it
}
| UNIT_SYMBOL '^' INTEGER {
    $$ = siueCreateTerm($1, $3);
    OCRelease($1);  // Release the string from lexer since siueCreateTerm copied it
}
| UNIT_SYMBOL '^' '(' INTEGER ')' {
    $$ = siueCreateTerm($1, $4);
    OCRelease($1);  // Release the string from lexer since siueCreateTerm copied it
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
