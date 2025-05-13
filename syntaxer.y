%{
#include <stdio.h>
#include <string.h>
#include "ast.h"
#include "list.h"
%}

%{
struct ident_with_typename_t {
    enum type_t *type;
    char *name;
};
struct idents_with_typename_list_t {
    struct list_header_t *next;
    struct ident_with_typename_t ident_with_typename;
};
%}

%token NUMBER
%token LBRACKET RBRACKET
%token ADD MUL MINUS DIV
%token TYPE_INT TYPE_REAL TYPE_STRING

%token IDENT
%union {
    struct ast_node *node;
    enum type_t *type;
    char *str;
}

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

%token STRING

%token REAL

%token NEQ

%token RETURN

%token MOD

%%

root:
    function_definition_list { root = $<node>1; }
    ;

stmt_block
    : LBRACE stmt_list RBRACE { $<node>$ = $<node>2; }
    ;

stmt_list
    : { $<node>$ = NULL; }
	| splitable_stmt STATEMENT_DELIMITER stmt_list { $<node>$ = new_ast_list_element($<node>1, $<node>3); }
    | unsplitable_stmt stmt_list { $<node>$ = new_ast_list_element($<node>1, $<node>2); }
	;

splitable_stmt
    : assign_or_expr
    | return
    ;

unsplitable_stmt
	: branch
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
    | expr EQ term { $<node>$ = new_node(EXPRESSION_T, (union value_t){.expression = (struct expression_t){.operation = EQ_OP, .left = $<node>1, .right = $<node>3 }}); }
    | expr NEQ term { $<node>$ = new_node(EXPRESSION_T, (union value_t){.expression = (struct expression_t){.operation = NEQ_OP, .left = $<node>1, .right = $<node>3 }}); }
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
    | term MOD factor { $<node>$ = new_node(EXPRESSION_T, (union value_t){.expression = (struct expression_t){.operation = MOD_OP, .left = $<node>1, .right = $<node>3 }}); }
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
    | LBRACKET typename RBRACKET factor { $<node>$ = new_node(CAST_T, (union value_t){.cast = {.cast_type = $<type>2, .fact = $<node>4 }}); }
    | STRING
    | REAL
    | function_call
    | deref_pointer
	;

typename
    : TYPE_INT
    | TYPE_REAL
    | TYPE_STRING
    ;

expr_list
    : expr PARAM_DELIMITER expr_list { $<node>$ = new_ast_list_element($<node>1, $<node>3); }
    | expr { $<node>$ = new_ast_list_element($<node>1, NULL); }
    ;

function_call
    : IDENT LBRACKET expr_list RBRACKET { $<node>$ = new_node(FUNCTION_CALL_T, (union value_t){.function_call = {.function_name = $<node>1, .param = $<node>3}}); }
    | IDENT LBRACKET RBRACKET { $<node>$ = new_node(FUNCTION_CALL_T, (union value_t){.function_call = {.function_name = $<node>1, .param = NULL}}); }
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

ident_with_typename
    : typename IDENT { $<node>$ = new_node(IDENT_DESCRIPTION_T, (union value_t){.ident_description = {.type = $<type>1, .ident = $<node>2 }}); }
    ;

ident_with_typename_list
    : ident_with_typename PARAM_DELIMITER ident_with_typename_list { $<node>$ = new_ast_list_element($<node>1, $<node>3); }
    | ident_with_typename { $<node>$ = new_ast_list_element($<node>1, NULL); }
    ;

function_definition
    : typename IDENT LBRACKET ident_with_typename_list RBRACKET stmt_block { $<node>$ = new_node(FUNCTION_T, (union value_t){.function={.type=$<type>1, .function_name=$<node>2, .params = $<node>4, .stmt = $<node>6}}); }
    | typename IDENT LBRACKET RBRACKET stmt_block { $<node>$ = new_node(FUNCTION_T, (union value_t){.function={.type=$<type>1, .function_name=$<node>2, .params = NULL, .stmt = $<node>5}}); }
    ;

function_definition_list
    : function_definition function_definition_list { $<node>$ = new_ast_list_element($<node>1, $<node>2); }
    | function_definition { $<node>$ = new_ast_list_element($<node>1, NULL); }
    ;

return
    : RETURN assign_or_expr { $<node>$ = new_node(RETURN_T, (union value_t){.return_ = {.return_value = $<node>2}}); }
    ;

deref_pointer
    : MUL factor { $<node>$ = new_node(EXPRESSION_T, (union value_t){.expression = (struct expression_t){.operation = DEREF_POINTER_OP, .left = $<node>2, .right = NULL}}); }
    ;

%%
