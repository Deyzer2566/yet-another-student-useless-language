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
"-"						return MINUS;
"/"						return DIV;
"=="                    return EQ;
">"                     return LARGER;
">="                    return LARGER_OR_EQ;
"<"                     return SMALLER;
"<="                    return SMALLER_OR_EQ;
"&&"                    return LOGICAL_AND;
"||"                    return LOGICAL_OR;
"!"                     return LOGICAL_NOT;
"&"                     return BITWISE_AND;
"|"                     return BITWISE_OR;
"~"                     return BITWISE_NOT;
"^"                     return XOR;
"int"                   return INT;
"real"                  return REAL;
"string"                return STRING;
"if"                    return IF;
"else"                  return ELSE;
"{"                     return LBRACE;
"}"                     return RBRACE;
"while"                 return WHILE;
"for"                   return FOR;
[a-zA-Z_][a-zA-Z0-9]*   { char *str = malloc(strlen(yytext)); memcpy(str, yytext, strlen(yytext)); yylval.node = new_node(IDENT_T, (union value_t){.str = str}); return IDENT; }
"="                     return ASSIGN;
","                     return PARAM_DELIMITER;
%%
