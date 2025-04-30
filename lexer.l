%{
#include <stdio.h>
#include "syntaxer.tab.h"
%}

%%
[0-9]+					{ yylval = atoi(yytext); return NUMBER; }
"("						return LBRACKET;
")"						return RBRACKET;
"+"						return ADD;
"*"						return MUL;
\n						return EOL;
"-"						return SUB;
"/"						return DIV;
%%
