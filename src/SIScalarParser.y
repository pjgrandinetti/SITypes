%{
    #include <OCTypes/OCLibrary.h>
    #include "SILibrary.h"
    #include "SIScalarParser.h"
    #include "SIScalarParserHelpers.c"
    #include <ctype.h>
    void siserror(char *s, ...);

    extern int  sis_scan_string(const char *);
    extern void sislex_destroy(void);
    bool       sis_syntax_error;

    OCStringRef scalarError;

    SIScalarRef result;
%}

%name-prefix="sis"

%union {
    ScalarNodeRef            a;
    SIScalarRef              d;
    struct symbol           *s;
    builtInMathFunctions     math_fn;
    builtInConstantFunctions const_fn;
    OCMutableStringRef       const_string;
}

/* declare tokens */
%token <d>            SCALAR
%token <math_fn>      MATH_FUNC
%token <const_fn>     CONST_FUNC CO      /* accept both modern and legacy names */
%token <const_string> CONST_STRING
%token                EOL

%left   '='
%left   '+' '-'
%left   '*' '/'
%right  '^'
%right  '!'
%nonassoc '|' UMINUS

%type <a> exp explist

%%


calclist:
      /* empty */
    | calclist exp
      {
          printf("Evaluating expression\n");
          result = ScalarNodeEvaluate($2, &scalarError);
          if (!ScalarNodeisLeaf($2))
              ScalarNodeFree($2);
      }
    ;

exp:
      exp '+' exp     { $$ = ScalarNodeCreateInnerNode('+', $1, $3); }
    | exp '-' exp     { $$ = ScalarNodeCreateInnerNode('-', $1, $3); }
    | exp '*' exp     { $$ = ScalarNodeCreateInnerNode('*', $1, $3); }
    | exp '/' exp     { $$ = ScalarNodeCreateInnerNode('/', $1, $3); }
    | exp '^' exp     { $$ = ScalarNodeCreateInnerNode('^', $1, $3); }
    | '|' exp '|'     { $$ = ScalarNodeCreateInnerNode('|', $2, NULL); }
    | '(' exp ')'     { $$ = $2; }
    | '-' exp %prec UMINUS
                      { $$ = ScalarNodeCreateInnerNode('M', $2, NULL); }
    | exp '!'         { $$ = ScalarNodeCreateInnerNode('!', $1, NULL); }
    | SCALAR          { if ($1 == NULL) YYERROR; $$ = ScalarNodeCreateNumberLeaf($1); }
    | MATH_FUNC '(' explist ')' 
                      { $$ = ScalarNodeCreateMathFunction($1, $3); }
    | CONST_FUNC CONST_STRING
                      { $$ = ScalarNodeCreateConstantFunction($1, $2); }
    | CO CONST_STRING
                      { $$ = ScalarNodeCreateConstantFunction($1, $2); }
    ;

explist:
      exp
    | exp ',' explist
      { $$ = ScalarNodeCreateInnerNode('L', $1, $3); }
    ;

%%


