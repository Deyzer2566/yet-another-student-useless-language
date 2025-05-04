%{
#include <stdio.h>
#include <string.h>
#include "ast.h"
#include "ast_list.h"
%}

%token NUMBER
%token LBRACKET RBRACKET
%token EOL 
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
#include <stdbool.h>
#include <stdlib.h>

struct idents_list_t {
    struct list_header_t list_header;
    const char *key;
    struct ast_node *value;
};

struct idents_list_t *ident = NULL;

void update_ident(const char* key, struct ast_node *value) {
    struct idents_list_t *cur = ident;
    while(cur != NULL) {
        if(strcmp(cur->key, key) == 0) {
            cur->value = value;
            return;
        }
        if(cur->list_header.next == NULL)
            break;
        else 
            cur = (struct idents_list_t*)cur->list_header.next;
    }
    if(cur == NULL) {
        cur = malloc(sizeof(struct idents_list_t));
		ident = cur;
    } else {
        cur->list_header.next = malloc(sizeof(struct idents_list_t));
        cur = (struct idents_list_t*)cur->list_header.next;
    }
    cur->key = key;
    cur->value = value;
	cur->list_header.next = NULL;
}

struct yystype_or_err {
    struct ast_node *ret_value;
    bool valid;
};

struct yystype_or_err get_ident(const char* key) {
    struct idents_list_t *cur = ident;
    while(cur != NULL) {
        if(strcmp(cur->key, key) == 0) {
            return (struct yystype_or_err){.valid = true, .ret_value = cur->value};
        }
        cur = (struct idents_list_t*)cur->list_header.next;
    }
    return (struct yystype_or_err){.valid = false};
}
%}

%{
    struct ast_node *root;
%}

%%

root:
    stmt_list { root = $<node>1; }
    ;

stmt_list
    : { $<node>$ = NULL; }
	| stmt EOL stmt_list {
            struct ast_node *element = malloc(sizeof(struct ast_node));
            element->type = AST_LIST_ELEMENT_T;
            element->value.ast_list_element.next = $<node>3;
            element->value.ast_list_element.node = $<node>1;
            $<node>$ = element;
        }
	;

stmt:
	assign_or_expr
	;

assign_or_expr
    : IDENT ASSIGN expr { $<node>$ = $<node>3; update_ident($<node>1->value.str, $<node>3); }
    | expr
    ;


expr
	: expr ADD term { $<node>$ = new_node(EXPRESSION_T, (union value_t){.expression = (struct expression_t){.operation = PLUS_OP, .left = $<node>1, .right = $<node>3 }}); }
	| expr SUB term { $<node>$ = new_node(EXPRESSION_T, (union value_t){.expression = (struct expression_t){.operation = MINUS_OP, .left = $<node>1, .right = $<node>3 }}); }
	| term
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

%%
