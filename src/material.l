
%{
#include <stdio.h>
#include "wavefront.tab.h"
%}

%option noyywrap nounput
%option prefix="mat_"

%%

"#"[^\n]+"\n" { }
[[:space:]]+ { }

[+-]?[0-9]+ {
    return TYPE_INT;
}
[+-]?[0-9]+\.[0-9]+([eE][+-]?[0-9]+)? {
    return TYPE_FLOAT;
}

"newmtl" { return NEWMTL; }

[a-zA-Z][a-zA-Z0-9]+ { return TYPE_STRING; }

. { fprintf(stderr, "'%s'\n", yytext); }

%%
