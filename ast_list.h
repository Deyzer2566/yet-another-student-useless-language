#pragma once
#include "list.h"
#include "ast.h"
#include <stdlib.h>
struct ast_node_list {
    struct list_header_t *next;
    struct ast_node *node;
};