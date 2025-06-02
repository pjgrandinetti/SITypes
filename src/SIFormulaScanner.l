%option noyywrap nodefault
%option prefix="sifw"
%option nounput
%option noinput
%{
#import "SILibrary.h"

enum yytokentype {
    NUMBER = 258,
    ELEMENT = 259,
    OPENPARENTHESIS = 260,
    CLOSEPARENTHESIS = 261,
    MULTIPLIER = 262,
    EOL = 263,
    ERROR = 264
};

int yylval;
%}

/* Token patterns */
WHITESPACE   [ \t]+
ELEMENT      (H|He|Li|Be|B|C|N|O|F|Ne|Na|Mg|Al|Si|P|S|Cl|Ar|K|Ca|Sc|Ti|V|Cr|Mn|Fe|Co|Ni|Cu|Zn|Ga|Ge|As|Se|Br|Kr|Rb|Sr|Y|Zr|Nb|Mo|Tc|Ru|Rh|Pd|Ag|Cd|In|Sn|Sb|Te|I|Xe|Cs|Ba|La|Ce|Pr|Nd|Pm|Sm|Eu|Gd|Tb|Dy|Ho|Er|Tm|Yb|Lu|Hf|Ta|W|Re|Os|Ir|Pt|Au|Hg|Tl|Pb|Bi|Po|At|Rn|Fr|Ra|Ac|Th|Pa|U|Np|Pu|Am|Cm|Bk|Cf|Es|Fm|Md|No|Lr|Rf|Db|Sg|Bh|Hs|Mt|Ds|Rg|Cn|Uut|Fl|Uup|Lv|Uus|Uuo)
NUMBER       [0-9]+
MULTIPLIER   \*[0-9]+

%%

{WHITESPACE}          { /* skip */ }
{ELEMENT}             { return ELEMENT; }
"("                   { return OPENPARENTHESIS; }
")"                   { return CLOSEPARENTHESIS; }
{MULTIPLIER}          { return MULTIPLIER; }
{NUMBER}              { yylval = atoi(yytext); return NUMBER; }
\n                    { return EOL; }
.                     { return ERROR; }

%%

SIScalarRef SIPeriodicTableCreateFormulaMass(OCStringRef formula, OCStringRef *error)
{
    if (error && *error) return NULL;

    OCMutableStringRef calculation = OCStringCreateMutable(0);
    OCMutableStringRef mutString = OCStringCreateMutableCopy(formula);
    const char *cString = OCStringGetCString(mutString);

    sifw_scan_string(cString);
    SIScalarRef result = NULL;
    int tok;

    while ((tok = sifwlex())) {
        OCStringRef fragment = NULL;

        switch (tok) {
            case NUMBER:
                fragment = OCStringCreateWithFormat(NULL, NULL, STR("(%d)"), yylval);
                break;

            case ELEMENT:
                fragment = OCStringCreateWithFormat(NULL, NULL, STR("+aw[%s]"), yytext);
                break;

            case MULTIPLIER:
                fragment = OCStringCreateWithFormat(NULL, NULL, STR("+%s!"), yytext);
                break;

            case OPENPARENTHESIS:
                fragment = STR("+(");
                OCRetain(fragment);
                break;

            case CLOSEPARENTHESIS:
                fragment = STR(")");
                OCRetain(fragment);
                break;

            case ERROR:
                OCRelease(calculation);
                OCRelease(mutString);
                sifwlex_destroy();
                return NULL;
        }

        if (fragment) {
            OCStringAppend(calculation, fragment);
            OCRelease(fragment);
        }
    }

    if (OCStringGetLength(calculation) > 0) {
        OCStringDelete(calculation, OCRangeMake(0, 1)); // remove leading '+'

        OCStringFindAndReplace(calculation, STR("(+"), STR("("), OCRangeMake(0, OCStringGetLength(calculation)), 0);
        OCStringFindAndReplace(calculation, STR("+*"), STR("+"), OCRangeMake(0, OCStringGetLength(calculation)), 0);

        int64_t replacements = OCStringFindAndReplace(calculation, STR("!+"), STR("("), OCRangeMake(0, OCStringGetLength(calculation)), 0);
        if (replacements == 1) {
            OCStringAppend(calculation, STR(")"));
        }

        result = SIScalarCreateWithOCString(calculation, error);
    }

    OCRelease(calculation);
    OCRelease(mutString);

    sifwlex_destroy();
    return result;
}
