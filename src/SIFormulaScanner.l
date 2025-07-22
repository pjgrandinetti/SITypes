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

    OCMutableStringRef mutString = OCStringCreateMutableCopy(formula);
    const char *cString = OCStringGetCString(mutString);

    sifw_scan_string(cString);
    SIScalarRef result = NULL;
    int tok;
    
    // Initialize total mass to zero
    SIUnitRef massUnit = SIUnitFromExpression(STR("g/mol"), NULL, error);
    SIScalarRef totalMass = SIScalarCreateWithDouble(0.0, massUnit);
    OCRelease(massUnit);
    
    SIScalarRef currentElementMass = NULL;
    int multiplier = 1;

    while ((tok = sifwlex())) {
        switch (tok) {
            case ELEMENT:
            {
                // Add previous element if any
                if (currentElementMass) {
                    SIScalarRef contribution = SIScalarCreateWithDouble(multiplier, NULL);
                    SIScalarRef scaled = SIScalarCreateByMultiplying(currentElementMass, contribution, error);
                    SIScalarRef newTotal = SIScalarCreateByAdding(totalMass, scaled, error);
                    OCRelease(contribution);
                    OCRelease(scaled);
                    OCRelease(totalMass);
                    OCRelease(currentElementMass);
                    totalMass = newTotal;
                    currentElementMass = NULL;
                    multiplier = 1;
                }
                
                // Get atomic weight for this element
                OCStringRef elementSymbol = OCStringCreateWithCString(yytext);
                currentElementMass = SIPeriodicTableCreateMolarMass(elementSymbol, error);
                OCRelease(elementSymbol);
                if (!currentElementMass) {
                    if (error && !*error) *error = STR("Unknown element");
                    OCRelease(totalMass);
                    OCRelease(mutString);
                    sifwlex_destroy();
                    return NULL;
                }
                break;
            }
            case NUMBER:
                multiplier = yylval;
                break;
                
            case MULTIPLIER:
                multiplier = atoi(yytext+1); // skip the * at beginning
                break;
                
            case OPENPARENTHESIS:
            case CLOSEPARENTHESIS:
                // For now, ignore parentheses (could be enhanced later)
                break;
                
            case ERROR:
                if (error && !*error) *error = STR("Invalid character in formula");
                if (currentElementMass) OCRelease(currentElementMass);
                OCRelease(totalMass);
                OCRelease(mutString);
                sifwlex_destroy();
                return NULL;
        }
    }

    // Add the last element if any
    if (currentElementMass) {
        SIScalarRef contribution = SIScalarCreateWithDouble(multiplier, NULL);
        SIScalarRef scaled = SIScalarCreateByMultiplying(currentElementMass, contribution, error);
        SIScalarRef newTotal = SIScalarCreateByAdding(totalMass, scaled, error);
        OCRelease(contribution);
        OCRelease(scaled);
        OCRelease(totalMass);
        OCRelease(currentElementMass);
        totalMass = newTotal;
    }

    result = totalMass;
    OCRelease(mutString);
    sifwlex_destroy();

    return result;
}
