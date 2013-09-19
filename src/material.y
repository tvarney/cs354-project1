
%code top {
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "generic/WavefrontLoader.hpp"
#include "common.hpp"
#define YYERROR_VERBOSE 1
    int mat_lex(void);
    void mat_error(const char *);
    extern char *mat_text;
    extern int mat_leng;
    void mat_unsupported(const char *msg, ...);
    extern int mat_lineno;
    using namespace cs354;
}

%code requires {
#include "generic/WavefrontLoader.hpp"
#include "common.hpp"
}

%union {
    GLfloat float_triplet[3];
    GLfloat fval;
    int ival;
    const char *str;
}

%defines
%token NEWMTL ILLUM
%token KA KD KS KE TR NS NI TF
%token MAP_KA MAP_KD MAP_KS MAP_TR MAP_BUMP MAP_DECAL
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
  KA float_triple  { cs354::loader->ka($2); }
| KD float_triple  { cs354::loader->kd($2); }
| KS float_triple  { cs354::loader->ks($2); }
| KE float_triple  { mat_unsupported("ke %f %f %f", $2[0], $2[1], $2[2]); }
| NS floatval      { cs354::loader->ns($2); }
| TR floatval      { mat_unsupported("tr %f", $2); }
| MAP_KA strval    { mat_unsupported("map_ka %s", $2); }
| MAP_KD strval    { mat_unsupported("map_kd %s", $2); }
| MAP_KS strval    { mat_unsupported("map_ks %s", $2); }
| MAP_TR strval    { mat_unsupported("map_tr %s", $2); }
| MAP_BUMP strval  { mat_unsupported("map_bump %s", $2); }
| MAP_DECAL strval { mat_unsupported("map_decal %s", $2); }
| NEWMTL strval    { cs354::loader->newmtl($2); }
| ILLUM intv       { mat_unsupported("illum %d", $2); }
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
    fprintf(stderr, "Error near line %d: %s [%s]\n", mat_lineno, str,
            mat_text);
}

void mat_unsupported(const char *str, ...) {
    va_list vargs;
    
    fprintf(stderr, "Unsupported Feature: ");
    va_start(vargs, str);
    vfprintf(stderr, str, vargs);
    va_end(vargs);
    fputc('\n', stderr);
}
