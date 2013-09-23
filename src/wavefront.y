
%code top {
#include <stdio.h>
#include <stdlib.h>
#include "generic/WavefrontLoader.hpp"
#include "common.hpp"
#define YYERROR_VERBOSE 1
    int wf_lex(void);
    void wf_error(const char *);
    extern char *wf_text;
    extern int wf_leng;
}

%code requires {
#include "generic/WavefrontLoader.hpp"
#include "common.hpp"
    extern int wf_lineno;
}

%union {
    GLfloat float_triplet[3];
    GLfloat fval;
    int int_triplet[3];
    int ival;
    const char *str;
}

%defines
%token GROUP "g"
%token MTLLIB "mtllib"
%token USEMTL "usemtl"
%token ID_V "v"
%token ID_VT "vt"
%token ID_VN "vn"
%token ID_F "f"
%token ID_SEP "/"
%token ID_S "s"
%token TYPE_FLOAT TYPE_INT TYPE_STRING
%type <float_triplet> float_triple
%type <int_triplet> face_arg
%type <fval> floatval floatv
%type <ival> intv
%type <str> strval

%name-prefix "wf_"

%start exp

%%

exp:
     /* empty */
| exp statement
;

statement:
  "v" float_triple  { cs354::loader->v($2); }
| "vn" float_triple { cs354::loader->vn($2); }
| "vt" float_triple { cs354::loader->vt($2); }
| "f" face_arg_list { cs354::loader->f(); }
| "g" strval        { cs354::loader->g($2); }
| "s" intv          { fprintf(stderr, "Unsupported Function: 's %d'\n", $2); }
| "s" strval        { fprintf(stderr, "Unsupported Function: 's %s'\n", $2); }
| "mtllib" strval   { cs354::loader->mtllib($2); }
| "usemtl" strval   { cs354::loader->usemtl($2); }
;

face_arg_list:
  face_arg               { cs354::loader->fArg($1); }
| face_arg_list face_arg { cs354::loader->fArg($2); }
;

face_arg:
  intv                   { $$[0] = $1; $$[1] =  0; $$[2] =  0; }
| intv "/" intv          { $$[0] = $1; $$[1] = $3; $$[2] =  0; }
| intv "/" "/" intv      { $$[0] = $1; $$[1] =  0; $$[2] = $4; }
| intv "/" intv "/" intv { $$[0] = $1; $$[1] = $3; $$[2] = $5; }
;

float_triple:
  floatval { $$[0] = $1; $$[1] = 0.0; $$[2] = 0.0; }
| floatval floatval { $$[0] = $1; $$[1] = $2; $$[2] = 0.0; }
| floatval floatval floatval { $$[0] = $1; $$[1] = $2; $$[2] = $3; }
;

floatval:
  floatv { $$ = (GLfloat)($1); }
| intv { $$ = $1; }
;

floatv:
TYPE_FLOAT { $$ = strtod(wf_text, NULL); }
;

intv: 
TYPE_INT { $$ = strtoll(wf_text, NULL, 10); }
;

strval:
TYPE_STRING { $$ = wf_text; }
;

%%

void wf_error(const char *str) {
    fprintf(stderr, "Error near line %d: %s [%s]\n", wf_lineno, str, wf_text);
}
