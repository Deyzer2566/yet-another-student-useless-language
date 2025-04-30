%{
#include <stdio.h>
#include <string.h>
%}

%token NUMBER
%token LBRACKET RBRACKET
%token EOL 
%token ADD MUL SUB DIV

%%

stmt_list:
	| stmt_list stmt EOL
	;

stmt:
	| expr { $$ = $1; printf("=%d\n", $1); }
	;

expr
	: expr ADD term { $$ = $1 + $3; }
	| expr SUB term { $$ = $1 - $3; }
	| term
	;

term
	: term MUL factor { $$ = $1 * $3; }
	| term DIV factor { $$ = $1 / $3; }
	| factor
	;

factor
	: LBRACKET expr RBRACKET { $$ = $2; }
	| NUMBER
	;

%%

void yyerror(const char *str)
{
		fprintf(stderr,"ошибка: %s\n",str);
} 
  
int main()
{
		yyparse();
}