%{
#include <stdio.h>
#include <string.h>
#include "ast.h"
#include "ast_list.h"
%}

%token NUMBER
%token LBRACKET RBRACKET
%token ADD MUL SUB DIV
%token INT REAL STRING

%token IDENT
%union {
    struct ast_node *node;
}

%type <str> IDENT
%type <i> NUMBER

%token ASSIGN

%{
    struct ast_node *root;
%}

%token FUNCTION_PARAM_DELIMITER
%token STATEMENT_DELIMITER

%%

root:
    stmt_list { root = $<node>1; }
    ;

stmt_list
    : { $<node>$ = NULL; }
	| stmt STATEMENT_DELIMITER stmt_list { $<node>$ = new_ast_list_element($<node>1, $<node>3); }
	;

stmt:
	assign_or_expr
	;

assign_or_expr
    : IDENT ASSIGN assign_or_expr { $<node>$ = new_node(EXPRESSION_T, (union value_t){.expression = (struct expression_t){.operation = ASSIGN_OP, .left = $<node>1, .right = $<node>3 }}); }
    | expr
    ;


expr
	: expr ADD term { $<node>$ = new_node(EXPRESSION_T, (union value_t){.expression = (struct expression_t){.operation = PLUS_OP, .left = $<node>1, .right = $<node>3 }}); }
	| expr SUB term { $<node>$ = new_node(EXPRESSION_T, (union value_t){.expression = (struct expression_t){.operation = MINUS_OP, .left = $<node>1, .right = $<node>3 }}); }
	| term
    | function_call
	;

parenthesized_expr:
    expr
    ;

term
	: term MUL factor { $<node>$ = new_node(EXPRESSION_T, (union value_t){.expression = (struct expression_t){.operation = MULTIPLICATION_OP, .left = $<node>1, .right = $<node>3 }}); }
	| term DIV factor { $<node>$ = new_node(EXPRESSION_T, (union value_t){.expression = (struct expression_t){.operation = DIVISION_OP, .left = $<node>1, .right = $<node>3 }}); }
	| factor
	;

factor
	: LBRACKET parenthesized_expr RBRACKET { $<node>$ = $<node>2; }
	| NUMBER
	| IDENT {
            $<node>$ = new_node(IDENT_T, (union value_t){.str = $<node>1->value.str });
        }
	;


 expr_list
    : expr FUNCTION_PARAM_DELIMITER expr_list { $<node>$ = new_ast_list_element($<node>1, $<node>3); }
    | expr { $<node>$ = new_ast_list_element($<node>1, NULL); }
    ;

function_call
    : IDENT LBRACKET expr_list RBRACKET { $<node>$ = new_node(FUNCTION_CALL_T, (union value_t){.function_call = {.function_name = $<node>1, .param = $<node>3}}); }
    ;

%%
