
%code top {
#include <stdio.h>
#include <stdlib.h>
#include "generic/ModelIO.hpp"
#include "common.hpp"
#define YYERROR_VERBOSE 1
    int wf_lex(void);
    void wf_error(const char *);
    extern char *wf_text;
    extern int wf_leng;
    extern cs354::ModelParserState cs354::model_parser_state;
}

%code requires {
#include "generic/ModelIO.hpp"
#include "common.hpp"
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
  "v" float_triple  { cs354::model_parser_state.vertex($2); }
| "vn" float_triple { cs354::model_parser_state.normal($2); }
| "vt" float_triple { cs354::model_parser_state.texture($2); }
| "f" face_arg_list { cs354::model_parser_state.face(); }
| "g" strval        { cs354::model_parser_state.group($2); }
| "mtllib" strval   { cs354::model_parser_state.mtllib($2); }
| "usemtl" strval   { cs354::model_parser_state.usemtl($2); }
;

face_arg_list:
  face_arg               { cs354::model_parser_state.face_arg($1); }
| face_arg_list face_arg { cs354::model_parser_state.face_arg($2); }
;

face_arg:
  intv                   { $$[0] = $1; $$[1] = $1; $$[2] = $1; }
| intv "/" intv          { $$[0] = $1; $$[1] = $3; $$[2] = $3; }
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

void yyerror(const char *str) {
    fputs(str, stderr);
    fprintf(stderr, "\nToken: %s\n", wf_text);
}
