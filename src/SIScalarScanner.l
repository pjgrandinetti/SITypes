%option noyywrap nodefault
%option prefix = "sis"
%option nounput
%option noinput
%{
    #include "SILibrary.h"
    #include "SIScalarParser.h"
    #include "SIScalarParser.tab.h"
    static double complex temp;
    extern OCStringRef scalarError;  // forward declare parser's error string
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

"acos"          {sislval.math_fn = BM_acos; return MATH_FUNC;}
"acosh"         {sislval.math_fn = BM_acosh; return MATH_FUNC;}
"asin"          {sislval.math_fn = BM_asin; return MATH_FUNC;}
"asinh"         {sislval.math_fn = BM_asinh; return MATH_FUNC;}
"atan"          {sislval.math_fn = BM_atan; return MATH_FUNC;}
"atanh"         {sislval.math_fn = BM_atanh; return MATH_FUNC;}
"cos"           {sislval.math_fn = BM_cos; return MATH_FUNC;}
"cosh"          {sislval.math_fn = BM_cosh; return MATH_FUNC;}
"sin"           {sislval.math_fn = BM_sin; return MATH_FUNC;}
"sinh"          {sislval.math_fn = BM_sinh; return MATH_FUNC;}
"tan"           {sislval.math_fn = BM_tan; return MATH_FUNC;}
"tanh"          {sislval.math_fn = BM_tanh; return MATH_FUNC;}
"erf"           {sislval.math_fn = BM_erf; return MATH_FUNC;}
"erfc"          {sislval.math_fn = BM_erfc; return MATH_FUNC;}
"exp"           {sislval.math_fn = BM_exp; return MATH_FUNC;}
"ln"            {sislval.math_fn = BM_ln; return MATH_FUNC;}
"log"           {sislval.math_fn = BM_log; return MATH_FUNC;}

"conj"          {sislval.math_fn = BM_conj; return MATH_FUNC;}
"creal"         {sislval.math_fn = BM_creal; return MATH_FUNC;}
"cimag"         {sislval.math_fn = BM_cimag; return MATH_FUNC;}
"carg"          {sislval.math_fn = BM_carg; return MATH_FUNC;}
"cabs"          {sislval.math_fn = BM_cabs; return MATH_FUNC;}
"sqrt"          {sislval.math_fn = BM_sqrt; return MATH_FUNC;}
"cbrt"          {sislval.math_fn = BM_cbrt; return MATH_FUNC;}
"qtrt"          {sislval.math_fn = BM_qtrt; return MATH_FUNC;}
"reduce"        {sislval.math_fn = BM_reduce; return MATH_FUNC;}

"aw"           {sislval.const_fn = BC_AW; return CONST_FUNC;}
"fw"           {sislval.const_fn = BC_FW; return CONST_FUNC;}
"abundance"     {sislval.const_fn = BC_Isotope_Abundance; return CONST_FUNC;}
"𝛾_I"           {sislval.const_fn = BC_Isotope_Gyromag; return CONST_FUNC;}
"µ_I"           {sislval.const_fn = BC_Isotope_MagneticDipole; return CONST_FUNC;}
"Q_I"           {sislval.const_fn = BC_Isotope_ElectricQuadrupole; return CONST_FUNC;}
"nmr"           {sislval.const_fn = BC_nmr; return CONST_FUNC;}
"spin"         {sislval.const_fn = BC_Isotope_Spin; return CONST_FUNC;}
"t_½"         {sislval.const_fn = BC_Isotope_HalfLife; return CONST_FUNC;}

"&hbar" {double multiplier = 1;
    SIUnitRef unit = SIUnitFromExpression(STR("ℏ"),&multiplier, &scalarError);
    sislval.d = (SIScalarRef) OCAutorelease(SIScalarCreateWithDouble(multiplier,unit));
    return SCALAR;}

{REALNUMBER} {
    temp = OCComplexFromCString(yytext);
    SIScalarRef d = SIScalarCreateWithDoubleComplex(temp, NULL);
    if(d) OCAutorelease(d);
    sislval.d = d;
    return SCALAR;
}

{COMPLEXNUMBER} {
    temp = OCComplexFromCString(yytext);
    SIScalarRef d = SIScalarCreateWithDoubleComplex(temp, NULL);
    if(d) OCAutorelease(d);
    sislval.d = d;
    return SCALAR;
}

{REALNUMBER}[ ]*\*[ ]*I[ ]* {
    temp = OCComplexFromCString(yytext);
    SIScalarRef d = SIScalarCreateWithDoubleComplex(temp, NULL);
    if(d) OCAutorelease(d);
    sislval.d = d;
    return SCALAR;
}

I[ ]*\*[ ]*{REALNUMBER} {
    temp = OCComplexFromCString(yytext);
    SIScalarRef d = SIScalarCreateWithDoubleComplex(temp, NULL);
    if(d) OCAutorelease(d);
    sislval.d = d;
    return SCALAR;
}

"I" {
    temp = OCComplexFromCString(yytext);
    SIScalarRef d = SIScalarCreateWithDoubleComplex(temp, NULL);
    if(d) OCAutorelease(d);
    sislval.d = d;
    return SCALAR;
}

{UNIT} {
    OCStringRef string = OCStringCreateWithCString(yytext);
    double unit_multiplier = 1;
    SIUnitRef unit = SIUnitFromExpression(string, &unit_multiplier, &scalarError);
    if(unit == NULL) {
        if(string) OCRelease(string);
        yyterminate();
        }
    SIScalarRef d = SIScalarCreateWithDouble(unit_multiplier, unit);
    if(d) OCAutorelease(d);
    sislval.d = d;
    OCRelease(string);
    return SCALAR;
}

{REALNUMBER}[ ]*/{UNIT} {
    temp = OCComplexFromCString(yytext);
    BEGIN(together);
}

{UNIT}[ ]*/{REALNUMBER} {
    sislval.d = NULL;
    return SCALAR;
}

{UNIT}[ ]*/{COMPLEXNUMBER} {
    sislval.d = NULL;
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
    SIUnitRef unit = SIUnitFromExpression(string, &unit_multiplier,&scalarError);
    if(unit==NULL) {
        if(string) OCRelease(string);
        yyterminate();
        BEGIN(INITIAL);
        return SCALAR;
    }
    SIScalarRef d = SIScalarCreateWithDoubleComplex(temp*unit_multiplier, unit);
    if(d) OCAutorelease(d);
    sislval.d = d;
    OCRelease(string);
    BEGIN(INITIAL);
    return SCALAR;
}

{STRING} {
    OCMutableStringRef const_string = OCMutableStringCreateWithCString(yytext);
    if(const_string) OCAutorelease(const_string);
    sislval.const_string = const_string;
    return CONST_STRING;
}

{BADNUMBER} {
    sislval.d = NULL;
    return SCALAR;
}

{BADUNIT} {
    sislval.d = NULL;
    return SCALAR;
}

[ \t]+          {/* ignore whitespace */}
.               {return(yytext[0]);}

%%
