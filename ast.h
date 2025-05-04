#pragma once
#include <stddef.h>
enum type_t;
union value_t;
struct expression_t;
struct ast_node;
enum type_t {
    EXPRESSION_T,
    INTEGER_T,
    REAL_T,
    STRING_T,
    IDENT_T,
    AST_LIST_ELEMENT_T
};
struct expression_t {
    enum operation_t{
        PLUS_OP,
        MINUS_OP,
        MULTIPLICATION_OP,
        DIVISION_OP
    } operation;
    struct ast_node *left;
    struct ast_node *right;
};
union value_t {
    struct expression_t expression;
    int i;
    float f;
    char *str;
    struct {
        struct ast_node *next;
        struct ast_node *node;
    } ast_list_element;
};
struct ast_node {
    enum type_t type;
    union value_t value;
};
struct ast_node *new_node(enum type_t type, union value_t value);
void free_node(struct ast_node *node);
