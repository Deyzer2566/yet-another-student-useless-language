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
    }
    free(node);
}
