%option noyywrap nodefault yylineno
%option prefix = "sid"
%{
    #include "SILibrary.h"
    #include "SIDimensionality.tab.h"
    #include "SIDimensionalityParser.h"
%}
SYMBOL (L|M|T|I|@|N|J)
%%
[+-]?[0-9]+ {sidlval.iVal = atoi(yytext); return INTEGER;}
{SYMBOL} {
    OCStringRef string = OCStringCreateWithCString(yytext);
    sidlval.dimensionality = SIDimensionalityWithBaseDimensionSymbol(string, &dimensionalityError);
    OCRelease(string);
    return DIMENSIONALITY;
}
[\t ]+  { /* ignore whitespace */}
[a-zA-Z]+ {
    dimensionalityError = STR("Unknown dimensionality symbol");
}
.      {return yytext[0];}
%%


