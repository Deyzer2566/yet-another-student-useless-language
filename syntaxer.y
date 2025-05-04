%{
#include <stdio.h>
#include <string.h>
#include "ast.h"
#include "ast_list.h"
%}

%token NUMBER
%token LBRACKET RBRACKET
%token ADD MUL MINUS DIV
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

%token PARAM_DELIMITER
%token STATEMENT_DELIMITER

%token IF ELSE

%token LBRACE RBRACE

%token WHILE

%token FOR

%token EQ LARGER LARGER_OR_EQ SMALLER SMALLER_OR_EQ
%token LOGICAL_AND LOGICAL_OR LOGICAL_NOT
%token BITWISE_AND BITWISE_OR BITWISE_NOT
%token NEGATION XOR

%%

root:
    stmt_list { root = $<node>1; }
    ;

stmt_block
    : LBRACE stmt_list RBRACE { $<node>$ = $<node>2; }
    ;

stmt_list
    : { $<node>$ = NULL; }
	| stmt STATEMENT_DELIMITER stmt_list { $<node>$ = new_ast_list_element($<node>1, $<node>3); }
    | stmt { $<node>$ = new_ast_list_element($<node>1, NULL); }
	;

stmt:
	assign_or_expr
    | branch
    | loop
	;

assign_or_expr
    : IDENT ASSIGN assign_or_expr { $<node>$ = new_node(EXPRESSION_T, (union value_t){.expression = (struct expression_t){.operation = ASSIGN_OP, .left = $<node>1, .right = $<node>3 }}); }
    | expr
    ;


expr
	: expr ADD term { $<node>$ = new_node(EXPRESSION_T, (union value_t){.expression = (struct expression_t){.operation = PLUS_OP, .left = $<node>1, .right = $<node>3 }}); }
	| expr MINUS term { $<node>$ = new_node(EXPRESSION_T, (union value_t){.expression = (struct expression_t){.operation = MINUS_OP, .left = $<node>1, .right = $<node>3 }}); }
    | expr LOGICAL_OR term { $<node>$ = new_node(EXPRESSION_T, (union value_t){.expression = (struct expression_t){.operation = LOGICAL_OR_OP, .left = $<node>1, .right = $<node>3 }}); }
    | expr BITWISE_OR term { $<node>$ = new_node(EXPRESSION_T, (union value_t){.expression = (struct expression_t){.operation = BITWISE_OR_OP, .left = $<node>1, .right = $<node>3 }}); }
	| term
    | function_call
    | expr EQ term { $<node>$ = new_node(EXPRESSION_T, (union value_t){.expression = (struct expression_t){.operation = EQ_OP, .left = $<node>1, .right = $<node>3 }}); }
    | expr LARGER term { $<node>$ = new_node(EXPRESSION_T, (union value_t){.expression = (struct expression_t){.operation = LARGER_OP, .left = $<node>1, .right = $<node>3 }}); }
    | expr LARGER_OR_EQ term { $<node>$ = new_node(EXPRESSION_T, (union value_t){.expression = (struct expression_t){.operation = LARGER_OR_EQ_OP, .left = $<node>1, .right = $<node>3 }}); }
    | expr SMALLER term { $<node>$ = new_node(EXPRESSION_T, (union value_t){.expression = (struct expression_t){.operation = SMALLER_OP, .left = $<node>1, .right = $<node>3 }}); }
    | expr SMALLER_OR_EQ term { $<node>$ = new_node(EXPRESSION_T, (union value_t){.expression = (struct expression_t){.operation = SMALLER_OR_EQ_OP, .left = $<node>1, .right = $<node>3 }}); }
	;

parenthesized_expr:
    expr
    ;

term
	: term MUL factor { $<node>$ = new_node(EXPRESSION_T, (union value_t){.expression = (struct expression_t){.operation = MULTIPLICATION_OP, .left = $<node>1, .right = $<node>3 }}); }
	| term DIV factor { $<node>$ = new_node(EXPRESSION_T, (union value_t){.expression = (struct expression_t){.operation = DIVISION_OP, .left = $<node>1, .right = $<node>3 }}); }
    | term LOGICAL_AND factor { $<node>$ = new_node(EXPRESSION_T, (union value_t){.expression = (struct expression_t){.operation = LOGICAL_AND_OP, .left = $<node>1, .right = $<node>3 }}); }
    | term BITWISE_AND factor { $<node>$ = new_node(EXPRESSION_T, (union value_t){.expression = (struct expression_t){.operation = BITWISE_AND_OP, .left = $<node>1, .right = $<node>3 }}); }
    | term XOR factor { $<node>$ = new_node(EXPRESSION_T, (union value_t){.expression = (struct expression_t){.operation = XOR_OP, .left = $<node>1, .right = $<node>3 }}); }
	| factor
	;

factor
	: LBRACKET parenthesized_expr RBRACKET { $<node>$ = $<node>2; }
	| NUMBER
	| IDENT {
            $<node>$ = new_node(IDENT_T, (union value_t){.str = $<node>1->value.str });
        }
    | MINUS factor { $<node>$ = new_node(EXPRESSION_T, (union value_t){.expression = (struct expression_t){.operation = NEGATION_OP, .left = $<node>2, .right = NULL }}); }
    | LOGICAL_NOT factor { $<node>$ = new_node(EXPRESSION_T, (union value_t){.expression = (struct expression_t){.operation = LOGICAL_NOT_OP, .left = $<node>2, .right = NULL }}); }
    | BITWISE_NOT factor { $<node>$ = new_node(EXPRESSION_T, (union value_t){.expression = (struct expression_t){.operation = BITWISE_NOT_OP, .left = $<node>2, .right = NULL }}); }
	;


expr_list
    : expr PARAM_DELIMITER expr_list { $<node>$ = new_ast_list_element($<node>1, $<node>3); }
    | expr { $<node>$ = new_ast_list_element($<node>1, NULL); }
    ;

function_call
    : IDENT LBRACKET expr_list RBRACKET { $<node>$ = new_node(FUNCTION_CALL_T, (union value_t){.function_call = {.function_name = $<node>1, .param = $<node>3}}); }
    ;

branch
    : IF LBRACKET assign_or_expr RBRACKET stmt_block { $<node>$ = new_node(BRANCH_T, (union value_t){.branch = {.expression = $<node>3, .then_stmt=$<node>5}}); }
    | IF LBRACKET assign_or_expr RBRACKET stmt_block ELSE stmt_block { $<node>$ = new_node(BRANCH_T, (union value_t){.branch = {.expression = $<node>3, .then_stmt=$<node>5, .else_stmt=$<node>7}}); }
    ;

loop
    : while_loop
    | for_loop
    ;

while_loop
    : WHILE LBRACKET assign_or_expr RBRACKET stmt_block { $<node>$ = new_node(WHILE_T, (union value_t){.while_loop = {.expression = $<node>3, .stmt=$<node>5}}); }
    ;

assing_expr_or_nil
    : { $<node>$ = NULL; }
    | assign_or_expr
    ;

for_loop
    : FOR LBRACKET assing_expr_or_nil PARAM_DELIMITER assing_expr_or_nil PARAM_DELIMITER assing_expr_or_nil RBRACKET stmt_block { 
            $<node>$ = new_node(FOR_T, (union value_t){.for_loop = {.init = $<node>3, .limit=$<node>5, .step = $<node>7, .stmt = $<node>9}}); 
        }
    ;

%%
