
%code top {
#include <stdio.h>
#include <stdlib.h>
#include "generic/ModelIO.hpp"
#include "common.hpp"
#define YYERROR_VERBOSE 1
    int mat_lex(void);
    void mat_error(const char *);
    extern char *mat_text;
    extern int mat_leng;
    extern cs354::ModelParserState cs354::model_parser_state;
}

%code requires {
#include "generic/ModelIO.hpp"
#include "common.hpp"
    extern int mat_lineno;
}

%union {
    GLfloat float_triplet[3];
    GLfloat fval;
    int ival;
    const char *str;
}

%defines
%token KA KD KS TR MAP_KA MAP_KD MAP_KS MAP_TR MAP_BUMP MAP_DECAL NEWMTL
%token TYPE_FLOAT TYPE_INT TYPE_STRING
%type <float_triplet> float_triple
%type <fval> floatval floatv
%type <ival> intv
%type <str> strval

%name-prefix "mat_"

%start exp

%%

exp:
     /* empty */
| exp statement
;

statement:
  KA float_triple  { }
| KD float_triple  { }
| KS float_triple  { }
| TR floatval      { }
| MAP_KA strval    { }
| MAP_KD strval    { }
| MAP_KS strval    { }
| MAP_TR strval    { }
| MAP_BUMP strval  { }
| MAP_DECAL strval { }
| NEWMTL strval    { }
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
TYPE_FLOAT { $$ = strtod(mat_text, NULL); }
;

intv: 
TYPE_INT { $$ = strtoll(mat_text, NULL, 10); }
;

strval:
TYPE_STRING { $$ = mat_text; }
;

%%

void mat_error(const char *str) {
    fprintf(stderr, "Error near line %d: %s [%s]\n", mat_lineno, str, mat_text);
}
