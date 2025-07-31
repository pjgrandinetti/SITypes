%option noyywrap nodefault
%option prefix="siue"
%option nounput
%option noinput
%{
    #include "SILibrary.h"
    #include "SIUnitExpression.h"
    #include "SIUnitExpressionParser.tab.h"
    
    extern void siueerror(char *s, ...);
    extern OCStringRef siueError;
%}

/* Generic patterns for potential unit symbols */
IDENTIFIER [a-zA-Z][a-zA-Z0-9_]*
SYMBOL {IDENTIFIER}

%%

"*"     { return '*'; }

{SYMBOL} {
    // Create string from matched text
    OCStringRef string = OCStringCreateWithCString(yytext);
    
    // Validate against the dynamic token library
    if (siueValidateSymbol(string)) {
        siuelval.unit_symbol = string;
        return UNIT_SYMBOL;
    } else {
        // Invalid symbol - set error and return error token
        OCRelease(string);
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Unknown unit symbol: %s", yytext);
        siueError = OCStringCreateWithCString(error_msg);
        return UNKNOWN_SYMBOL;
    }
}

[+\-]?[0-9]+ {
    siuelval.iVal = atoi(yytext);
    return INTEGER;
}

[0-9]+\.[0-9]+ {
    char error_msg[256];
    snprintf(error_msg, sizeof(error_msg), "Fractional powers are not allowed in unit expressions");
    siueError = OCStringCreateWithCString(error_msg);
    return DECIMAL;
}

[\t ]+  { /* ignore whitespace */ }

.      { return yytext[0]; }

%%
