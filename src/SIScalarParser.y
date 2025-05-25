%{
    #include <OCTypes/OCLibrary.h>
    #include "SILibrary.h"
    #include "SIScalarParser.h"
    #include <ctype.h>
    void siserror(char *s, ...);

    ScalarNodeRef sis_root = NULL;

    SIScalarRef result;
    OCStringRef scalarError;
    int sislex(void);    
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

/* whenever Bison discards an 'exp' or 'explist', free its node */
%destructor { ScalarNodeFree($$); } exp explist

/* declare tokens */
%token <d> SCALAR
%token <math_fn> MATH_FUNC
%token <const_fn> CONST_FUNC
%token <const_string> CONST_STRING
%token EOL
%left '='
%left  '+' '-'
%left '*' '/'
%right '^'
%right '!'
%nonassoc '|' UMINUS

%type <a> exp explist

%%
calclist:
      /* empty */
    | calclist exp
      {
        sis_root = $2;
        result = ScalarNodeEvaluate($2, &scalarError);
      }
    ;

exp:
      exp '+' exp   { $$ = ScalarNodeCreateInnerNode('+',$1,$3); }
    | exp '-' exp   { $$ = ScalarNodeCreateInnerNode('-',$1,$3); }
    | exp '*' exp   { $$ = ScalarNodeCreateInnerNode('*',$1,$3); }
    | exp '/' exp   { $$ = ScalarNodeCreateInnerNode('/',$1,$3); }
    | exp '^' exp   { $$ = ScalarNodeCreateInnerNode('^',$1,$3); }
    | '|' exp '|'   { $$ = ScalarNodeCreateInnerNode('|',$2,NULL); }
    | '(' exp ')'   { $$ = $2; }
    | '-' exp %prec UMINUS { $$ = ScalarNodeCreateInnerNode('M',$2,NULL); }
    | exp '!'       { $$ = ScalarNodeCreateInnerNode('!',$1,NULL); }
    | SCALAR        { if ($1==NULL) YYERROR; $$ = ScalarNodeCreateNumberLeaf($1); }
    | MATH_FUNC '(' explist ')' { $$ = ScalarNodeCreateMathFunction($1,$3); }
    | CONST_FUNC CONST_STRING   { $$ = ScalarNodeCreateConstantFunction($1,$2); }
    ;

explist:
      exp
    | exp ',' explist { $$ = ScalarNodeCreateInnerNode('L',$1,$3); }
    ;

%%

extern int sis_scan_string(const char *);
extern void sislex_destroy(void);
bool sis_syntax_error;
