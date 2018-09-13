%option noyywrap nodefault
%option prefix = "psfw"
%{
#include "SILibrary.h"

enum yytokentype {
    NUMBER = 258,
    ELEMENT = 259,
    OPENPARENTHESIS = 260,
    CLOSEPARENTHESIS = 261,
    PLUS = 262,
    EOL = 263,
    ERROR = 264
};
int yylval;
%}
ELEMENT (H|He|Li|Be|B|C|N|O|F|Ne|Na|Mg|Al|Si|P|S|Cl|Ar|K|Ca|Sc|Ti|V|Cr|Mn|Fe|Co|Ni|Cu|Zn|Ga|Ge|As|Se|Br|Kr|Rb|Sr|Y|Zr|Nb|Mo|Tc|Ru|Rh|Pd|Ag|Cd|In|Sn|Sb|Te|I|Xe|Cs|Ba|La|Ce|Pr|Nd|Pm|Sm|Eu|Gd|Tb|Dy|Ho|Er|Tm|Yb|Lu|Hf|Ta|W|Re|Os|Ir|Pt|Au|Hg|Tl|Pb|Bi|Po|At|Rn|Fr|Ra|Ac|Th|Pa|U|Np|Pu|Am|Cm|Bk|Cf|Es|Fm|Md|No|Lr|Rf|Db|Sg|Bh|Hs|Mt|Ds|Rg|Cn|Uut|Fl|Uup|Lv|Uus|Uuo)
%%

{ELEMENT} { return ELEMENT; }
"(" {return OPENPARENTHESIS;}
")" {return CLOSEPARENTHESIS;}
\*[0-9]+ {return PLUS; }
[0-9]+ { yylval=atoi(yytext); return NUMBER; }
\n {return EOL;}

. {return ERROR;}
%%

PSScalarRef PSPeriodicTableCreateFormulaMass(OCStringRef formula, OCStringRef *errorString)
{
    if(errorString) if(*errorString) return NULL;
    int tok;
    OCMutableStringRef calculation = OCStringCreateMutable(0);
    OCMutableStringRef  mutString = OCStringCreateMutableCopy(formula);
    
    const char *cString = OCStringGetCString(mutString);
    psfw_scan_string(cString);
    
    PSScalarRef result = NULL;
    
    while((tok = psfwlex())) {
        if(tok==NUMBER) {
            OCStringRef calc = OCStringCreateWithFormat(STR("(%d)"),yylval);
            OCStringAppend(calculation,calc);
            OCRelease(calc);
        }
        else if(tok==ELEMENT) {
            OCStringRef calc = OCStringCreateWithFormat(STR("+(aw[%s])"),yytext);
            OCStringAppend(calculation,calc);
            OCRelease(calc);
        }
        else if(tok==PLUS){
            OCStringRef calc = OCStringCreateWithFormat(STR("+%s!"),yytext);
            OCStringAppend(calculation,calc);
            OCRelease(calc);
        }
        else if(tok==OPENPARENTHESIS) {
            OCStringAppend(calculation,STR("+("));
        }
        else if(tok==CLOSEPARENTHESIS) {
            OCStringAppend(calculation,STR(")"));
        }
        else if(tok==ERROR) {
            OCRelease(calculation);
            OCRelease(mutString);
            
            psfwlex_destroy();
            return result;
        }
    }
    
    int64_t length = OCStringGetLength(calculation);
    if(length>0) {
        OCStringDelete(calculation, OCRangeMake(0, 1));
        OCStringFindAndReplace (calculation,STR("(+"), STR("("),OCRangeMake(0,OCStringGetLength(calculation)),0);
        OCStringFindAndReplace (calculation,STR("+*"), STR("+"),OCRangeMake(0,OCStringGetLength(calculation)),0);
        int64_t number = OCStringFindAndReplace (calculation,STR("!+"), STR("("),OCRangeMake(0,OCStringGetLength(calculation)),0);
        if(number==1) {
            OCStringAppend(calculation,STR(")"));
        }
        
        result = PSScalarCreateWithOCString(calculation, errorString);
    }
    
    OCRelease(calculation);
    OCRelease(mutString);
    
    psfwlex_destroy();
    return result;
}


