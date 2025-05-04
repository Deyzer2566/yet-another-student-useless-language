#include "ast.h"
#include <stdlib.h>

struct ast_node *new_node(enum type_t type, union value_t value) {
    struct ast_node *node = malloc(sizeof(struct ast_node));
    node->type = type;
    node->value = value;
    return node;
}

void free_node(struct ast_node *node) {
    if(node == NULL)
        return;
    if(node->type == EXPRESSION_T) {
        if(node->value.expression.left != NULL)
            free(node->value.expression.left);
        if(node->value.expression.right != NULL)
            free(node->value.expression.right);
    } else if(node->type == AST_LIST_ELEMENT_T) {
        for(struct ast_node *cur = node; cur != NULL; cur = cur->value.ast_list_element.next)
            free_node(cur->value.ast_list_element.node);
    } else if(node->type == IDENT_T) {
        free(node->value.str);
    } else if(node->type == STRING_T) {
        free(node->value.str);
    } else if(node->type == BRANCH_T) {
        free_node(node->value.branch.expression);
        free_node(node->value.branch.then_stmt);
        free_node(node->value.branch.else_stmt);
    } else if(node->type == WHILE_T) {
        free_node(node->value.while_loop.expression);
        free_node(node->value.while_loop.stmt);
    } else if(node->type == FOR_T) {
        free_node(node->value.for_loop.init);
        free_node(node->value.for_loop.limit);
        free_node(node->value.for_loop.step);
        free_node(node->value.for_loop.stmt);
    } else if(node->type == CAST_T) {
        free_node(node->value.cast.fact);
        free(node->value.cast.cast_type);
    }
    free(node);
}

struct ast_node *new_ast_list_element(struct ast_node *node, struct ast_node *next) {
    return new_node(AST_LIST_ELEMENT_T, (union value_t){
        .ast_list_element.node = node,
        .ast_list_element.next = next
    });
}
