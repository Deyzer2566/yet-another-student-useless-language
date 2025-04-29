%{
#include <stdio.h>
#include <string.h>
%}

%token NUMBER LBRACKET RBRACKET ADD EOL

%%

stmt_list:
	| stmt_list stmt EOL 	{ printf("=%d\n", $2); };

stmt:
	| expr { $$ = $1; }
	;

expr:
	NUMBER { $$ = $1;}
	| expr ADD expr {$$ = $1 + $3;}
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