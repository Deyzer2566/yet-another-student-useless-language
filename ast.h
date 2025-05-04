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
    AST_LIST_ELEMENT_T,
    FUNCTION_CALL_T,
    BRANCH_T
};
struct expression_t {
    enum operation_t{
        PLUS_OP,
        MINUS_OP,
        MULTIPLICATION_OP,
        DIVISION_OP,
        ASSIGN_OP
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
    struct {
        struct ast_node *function_name;
        struct ast_node *param;
    } function_call;
    struct {
        struct ast_node *expression;
        struct ast_node *then_stmt;
        struct ast_node *else_stmt;
    } branch;
};
struct ast_node {
    enum type_t type;
    union value_t value;
};
struct ast_node *new_node(enum type_t type, union value_t value);
void free_node(struct ast_node *node);
/*
 * Создает элемент списка узлов
 * node - ast-узел не указатель на создаваемый массив
 * next - ast-узел с типом AST_LIST_ELEMENT_T
 */
struct ast_node *new_ast_list_element(struct ast_node *node, struct ast_node *next);
