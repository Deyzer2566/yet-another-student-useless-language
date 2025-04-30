%{
#include <stdio.h>
#include "syntaxer.tab.h"
%}

%%
[0-9]+					{ yylval.i = atoi(yytext); return NUMBER; }
"("						return LBRACKET;
")"						return RBRACKET;
"+"						return ADD;
"*"						return MUL;
\n						return EOL;
"-"						return SUB;
"/"						return DIV;
[a-zA-Z_]\w*            { yylval.str = malloc(strlen(yytext)); memcpy(yylval.str, yytext, strlen(yytext)); return IDENT; }
"="                     return ASSIGN;
%%
