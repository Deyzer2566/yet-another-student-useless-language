%{
#include <stdio.h>
#include "syntaxer.tab.h"
#include "ast.h"
%}

%x STR
%x MULTILINE_COMMENT
%option yylineno


%%
[0-9]+					{ int val = atoi(yytext); yylval.node = new_node(INTEGER_T, (union value_t){.i = val}); return NUMBER; }
[0-9]+\.[0-9]*          { float val = atof(yytext); yylval.node = new_node(REAL_T, (union value_t){.f = val}); return REAL; }
"("						return LBRACKET;
")"						return RBRACKET;
"+"						return ADD;
"*"						return MUL;
";"                     return STATEMENT_DELIMITER;
"-"						return MINUS;
"/"						return DIV;
"=="                    return EQ;
"!="                    return NEQ;
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
"%"                     return MOD;
"int"                   { yylval.type = malloc(sizeof(enum type_t)); *yylval.type = INTEGER_T; return TYPE_INT; }
"real"                  { yylval.type = malloc(sizeof(enum type_t)); *yylval.type = REAL_T; return TYPE_REAL; }
"string"                { yylval.type = malloc(sizeof(enum type_t)); *yylval.type = STRING_T; return TYPE_STRING; }
"if"                    return IF;
"else"                  return ELSE;
"{"                     return LBRACE;
"}"                     return RBRACE;
"while"                 return WHILE;
"for"                   return FOR;
"return"                return RETURN;
[a-zA-Z_][a-zA-Z0-9]*   { char *str = malloc(strlen(yytext)+1); strcpy(str, yytext); yylval.node = new_node(IDENT_T, (union value_t){.str = str}); return IDENT; }
"="                     return ASSIGN;
","                     return PARAM_DELIMITER;
\"                      { yylval.str = malloc(1); yylval.str[0] = 0; BEGIN(STR); }
<STR>\\n                { char *str = malloc(strlen(yylval.str) + 1+1); str[0] = 0; strcpy(str, yylval.str); strcat(str, "\n"); free(yylval.str); yylval.str = str; }
<STR>\\r                { char *str = malloc(strlen(yylval.str) + 1+1); str[0] = 0; strcpy(str, yylval.str); strcat(str, "\r"); free(yylval.str); yylval.str = str; }
<STR>\\\"               { char *str = malloc(strlen(yylval.str) + 1+1); str[0] = 0; strcpy(str, yylval.str); strcat(str, "\""); free(yylval.str); yylval.str = str; }
<STR>\\\\               { char *str = malloc(strlen(yylval.str) + 1+1); str[0] = 0; strcpy(str, yylval.str); strcat(str, "\\"); free(yylval.str); yylval.str = str; }
<STR>[^\\\"]*           { char *str = malloc(strlen(yylval.str) + strlen(yytext)+1); str[0] = 0; strcpy(str, yylval.str); strcat(str, yytext); free(yylval.str); yylval.str = str; }
<STR>\"                 { yylval.node = new_node(STRING_T, (union value_t){.str = yylval.str}); BEGIN(INITIAL); return STRING; }
[ \t\n]                     
\/\*                    BEGIN(MULTILINE_COMMENT);
<MULTILINE_COMMENT>[^\*]
<MULTILINE_COMMENT>\*[^\/]
<MULTILINE_COMMENT>\*\/ BEGIN(INITIAL);
\/\/[^\n]*
%%
