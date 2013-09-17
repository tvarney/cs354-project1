
%{
#include <stdio.h>
#include "material.tab.h"
%}

%option noyywrap nounput yylineno
%option prefix="mat_"

%%

"\n" { }
"#"[^\n]+"\n" { }
[[:space:]]+ { }

[+-]?[0-9]+ {
    return TYPE_INT;
}
[+-]?[0-9]+\.[0-9]+([eE][+-]?[0-9]+)? {
    return TYPE_FLOAT;
}

"ka"        return KA;
"kd"        return KD;
"ks"        return KS;
"tr"        return TR;
"d"         return TR;
"map_ka"    return MAP_KA;
"map_kd"    return MAP_KD;
"map_ks"    return MAP_KS;
"map_tr"    return MAP_TR;
"map_d"     return MAP_TR;
"bump"      return MAP_BUMP;
"map_bump"  return MAP_BUMP;
"decal"     return MAP_DECAL;
"map_decal" return MAP_DECAL;
"newmtl"    return NEWMTL;

[a-zA-Z0-9'_''.']+ { return TYPE_STRING; }

. { fprintf(stderr, "'%s'\n", yytext); }

%%
