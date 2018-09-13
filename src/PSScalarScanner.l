%option noyywrap nodefault
%option prefix = "pss"
%{
    extern  void psserror(char *s, ...);
    #include "SILibrary.h"
    #include "PSScalarParser.h"
    #include "PSScalarParser.tab.h"
    static double complex temp;
%}

%x together

/* exponent */
DIGIT   [0-9]
EXP     ([Ee][-+]?[0-9]+)
REALNUMBER {DIGIT}+"."{DIGIT}*{EXP}?|"."?{DIGIT}+{EXP}?

OLDCOMPLEXNUMBER \({REALNUMBER}[ ]*[-+][ ]*({REALNUMBER}[ ]*\*[ ]*)?I[ ]*\)
COMPLEXNUMBER {REALNUMBER}[ ]*[-+][ ]*({REALNUMBER}[ ]*\*[ ]*)?I[ ]*
OLDUNIT (([a-zA-HJ-Z%ΩÅℏ°πµ]+)0?(\^[-+]?[0-9]+)?)([\*\/](([a-zA-HJ-Z%ΩÅℏ°πµ]+)0?(\^[-+]?[0-9]+)?))*

/* unit can be constant starting with &
 unit can be constant ending with 0
 unit can be symbol raised to arbitrary power.
 */
UNITCONSTANT ([a-zA-HJ-Z%ΩΛƛÅℏ°πσΦµεαλ∞\_]+)0?
CONSTANT (&[a-zA-HJ-Z]+)
UNITPOWER ([a-zA-HJ-Z%ΩÅℏ°πσΦµε\_]+)(\^[-+]?[0-9]+)?
UNITPOWER2 ([a-zA-HJ-Z%ΩÅℏ°πσΦµε\_]+)(\^\([-+]?[0-9]+\))?
INVERSEUNITPOWER ([(][1][/]{UNITPOWER}[)])
UNIT {CONSTANT}|{UNITCONSTANT}|{UNITPOWER}|{UNITPOWER2}|{INVERSEUNITPOWER}

BADNUMBER {DIGIT}*("."){DIGIT}*(".")|(".")(".")|(".")(" ")*(".")

BADUNIT {UNIT}{UNIT}

OLDSTRING \[[a-zA-Z][a-zA-Z0-9]*\]
STRING \[[()a-zA-Z][()a-zA-Z0-9]*\*?[()a-zA-Z0-9]*\]
%%
"+" |
"-" |
"*" |
"/" |
"^" |
"|" |
"(" |
")"     {return yytext[0];}

"acos"          {psslval.math_fn = BM_acos; return MATH_FUNC;}
"acosh"         {psslval.math_fn = BM_acosh; return MATH_FUNC;}
"asin"          {psslval.math_fn = BM_asin; return MATH_FUNC;}
"asinh"         {psslval.math_fn = BM_asinh; return MATH_FUNC;}
"atan"          {psslval.math_fn = BM_atan; return MATH_FUNC;}
"atanh"         {psslval.math_fn = BM_atanh; return MATH_FUNC;}
"cos"           {psslval.math_fn = BM_cos; return MATH_FUNC;}
"cosh"          {psslval.math_fn = BM_cosh; return MATH_FUNC;}
"sin"           {psslval.math_fn = BM_sin; return MATH_FUNC;}
"sinh"          {psslval.math_fn = BM_sinh; return MATH_FUNC;}
"tan"           {psslval.math_fn = BM_tan; return MATH_FUNC;}
"tanh"          {psslval.math_fn = BM_tanh; return MATH_FUNC;}
"erf"           {psslval.math_fn = BM_erf; return MATH_FUNC;}
"erfc"          {psslval.math_fn = BM_erfc; return MATH_FUNC;}
"exp"           {psslval.math_fn = BM_exp; return MATH_FUNC;}
"ln"            {psslval.math_fn = BM_ln; return MATH_FUNC;}
"log"           {psslval.math_fn = BM_log; return MATH_FUNC;}

"conj"          {psslval.math_fn = BM_conj; return MATH_FUNC;}
"creal"         {psslval.math_fn = BM_creal; return MATH_FUNC;}
"cimag"         {psslval.math_fn = BM_cimag; return MATH_FUNC;}
"carg"          {psslval.math_fn = BM_carg; return MATH_FUNC;}
"cabs"          {psslval.math_fn = BM_cabs; return MATH_FUNC;}
"sqrt"          {psslval.math_fn = BM_sqrt; return MATH_FUNC;}
"cbrt"          {psslval.math_fn = BM_cbrt; return MATH_FUNC;}
"qtrt"          {psslval.math_fn = BM_qtrt; return MATH_FUNC;}
"reduce"        {psslval.math_fn = BM_reduce; return MATH_FUNC;}

"aw"           {psslval.const_fn = BC_AW; return CONST_FUNC;}
"fw"           {psslval.const_fn = BC_FW; return CONST_FUNC;}
"abundance"     {psslval.const_fn = BC_Isotope_Abundance; return CONST_FUNC;}
"𝛾_I"           {psslval.const_fn = BC_Isotope_Gyromag; return CONST_FUNC;}
"µ_I"           {psslval.const_fn = BC_Isotope_MagneticDipole; return CONST_FUNC;}
"Q_I"           {psslval.const_fn = BC_Isotope_ElectricQuadrupole; return CONST_FUNC;}
"nmr"           {psslval.const_fn = BC_nmr; return CONST_FUNC;}
"spin"         {psslval.const_fn = BC_Isotope_Spin; return CONST_FUNC;}
"t_½"         {psslval.const_fn = BC_Isotope_HalfLife; return CONST_FUNC;}

"&hbar" {double multiplier = 1;
    SIUnitRef unit = SIUnitForSymbol(STR("ℏ"),&multiplier, &scalarErrorString);
    psslval.d = PSScalarCreateWithDouble(multiplier,unit);
    return SCALAR;}

{REALNUMBER} {
    temp = OCComplexFromCString(yytext);
    psslval.d = PSScalarCreateWithDoubleComplex(temp, NULL);
    return SCALAR;
}

{COMPLEXNUMBER} {
    temp = OCComplexFromCString(yytext);
    psslval.d = PSScalarCreateWithDoubleComplex(temp, NULL);
    return SCALAR;
}

{REALNUMBER}[ ]*\*[ ]*I[ ]* {
    temp = OCComplexFromCString(yytext);
    psslval.d = PSScalarCreateWithDoubleComplex(temp, NULL);
    return SCALAR;
}

I[ ]*\*[ ]*{REALNUMBER} {
    temp = OCComplexFromCString(yytext);
    psslval.d = PSScalarCreateWithDoubleComplex(temp, NULL);
    return SCALAR;
}

"I" {
    temp = OCComplexFromCString(yytext);
    psslval.d = PSScalarCreateWithDoubleComplex(temp, NULL);
    return SCALAR;
}

{UNIT} {
    OCStringRef string = OCStringCreateWithCString(yytext);
    double unit_multiplier = 1;
    SIUnitRef unit = SIUnitForSymbol(string, &unit_multiplier, &scalarErrorString);
    if(unit == NULL) {
        yyterminate();
    }
    psslval.d = PSScalarCreateWithDouble(unit_multiplier, unit);
    OCRelease(string);
    return SCALAR;
}

{REALNUMBER}[ ]*/{UNIT} {
    temp = OCComplexFromCString(yytext);
    BEGIN(together);
}

{UNIT}[ ]*/{REALNUMBER} {
    psslval.d = NULL;
    return SCALAR;
}

{UNIT}[ ]*/{COMPLEXNUMBER} {
    psslval.d = NULL;
    return SCALAR;
}

{COMPLEXNUMBER}[ ]*/{UNIT} {
    temp = OCComplexFromCString(yytext);
    BEGIN(together);
}

{REALNUMBER}[ ]*\*[ ]*I[ ]*/{UNIT} {
    temp = OCComplexFromCString(yytext);
    BEGIN(together);
}

{REALNUMBER}[ ]*\*[ ]*I[ ]*/{UNIT} {
    temp = OCComplexFromCString(yytext);
    BEGIN(together);
}
I[ ]*\*[ ]*{REALNUMBER}[ ]*/{UNIT} {
    temp = OCComplexFromCString(yytext);
    BEGIN(together);
}
I[ ]*\*[ ]*{REALNUMBER}[ ]*/{UNIT} {
    temp = OCComplexFromCString(yytext);
    BEGIN(together);
}
<together>{UNIT} {
    OCStringRef string = OCStringCreateWithCString(yytext);
    double unit_multiplier = 1;
    SIUnitRef unit = SIUnitForSymbol(string, &unit_multiplier,&scalarErrorString);
    if(unit==NULL) {
        yyterminate();
        if(string) OCRelease(string);
        BEGIN(INITIAL);
        return SCALAR;
    }
    psslval.d = PSScalarCreateWithDoubleComplex(temp*unit_multiplier, unit);
    OCRelease(string);
    BEGIN(INITIAL);
    return SCALAR;
}
{STRING} {
    psslval.const_string = OCMutableStringCreateWithCString(yytext);
    return CONST_STRING;
}
{BADNUMBER} {
    psslval.d = NULL;
    return SCALAR;
}
{BADUNIT} {
    psslval.d = NULL;
    return SCALAR;
}
[ \t]+          {/* ignore whitespace */}
.               {return(yytext[0]);}
%%
