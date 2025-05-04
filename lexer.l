%{
#include <stdio.h>
#include "syntaxer.tab.h"
#include "ast.h"
%}

%%
[0-9]+					{ int val = atoi(yytext); yylval.node = new_node(INTEGER_T, (union value_t){.i = val}); return NUMBER; }
"("						return LBRACKET;
")"						return RBRACKET;
"+"						return ADD;
"*"						return MUL;
\n                      return STATEMENT_DELIMITER;
"-"						return SUB;
"/"						return DIV;
"int"                   return INT;
"real"                  return REAL;
"string"                return STRING;
"if"                    return IF;
"else"                  return ELSE;
"{"                     return LBRACE;
"}"                     return RBRACE;
[a-zA-Z_][a-zA-Z0-9]*   { char *str = malloc(strlen(yytext)); memcpy(str, yytext, strlen(yytext)); yylval.node = new_node(IDENT_T, (union value_t){.str = str}); return IDENT; }
"="                     return ASSIGN;
","                     return FUNCTION_PARAM_DELIMITER;
%%
