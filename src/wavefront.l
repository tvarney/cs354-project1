
%{
#include <stdio.h>
#include "wavefront.tab.h"
%}

%option noyywrap nounput
%option prefix="wf_"

%%

"#"[^\n]+"\n" { }
[[:space:]]+ { }

[+-]?[0-9]+ {
    return TYPE_INT;
}
[+-]?[0-9]+\.[0-9]+([eE][+-]?[0-9]+)? {
    return TYPE_FLOAT;
}

"vn"     { return ID_VN; }
"vt"     { return ID_VT; }
"v"      { return ID_V; }
"f"      { return ID_F; }
"/"      { return ID_SEP; }
"g"      { return GROUP; }
"usemtl" { return USEMTL; }
"mtllib" { return MTLLIB; }

[_a-zA-Z][_a-zA-Z0-9]+ { return TYPE_STRING; }

. { fprintf(stderr, "'%s'\n", yytext); }

%%
