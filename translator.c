#include "translator.h"
#include "mal.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

int parse_expression(FILE *fd, struct ast_node *node, storage_t res, bool can_allocate_ident);

int translate_expression(FILE *fd, struct ast_node *node, storage_t dest, bool can_allocate_ident) {
    storage_t src1, src2;
    allocate_storage(dest);
    src1 = get_storage(fd);
    if(parse_expression(fd, node->value.expression.left, src1, node->value.expression.operation != NEGATION_OP) == -1)
        return -1;
    if(node->value.expression.operation != NEGATION_OP) {
        allocate_storage(dest);
        allocate_storage(src1);
        src2 = get_storage(fd);
        if(parse_expression(fd, node->value.expression.right, src2, false) == -1) {
            return -1;
        }
    }
    switch(node->value.expression.operation) {
        case PLUS_OP:
            add_oper_backend(fd, dest, src1, src2);
            break;
        case ASSIGN_OP:
            addi_oper_backend(fd, src1, src2, 0);
            addi_oper_backend(fd, dest, src1, 0);
            break;
        default:
            fprintf(stderr,"not implemented expression");
            return -1;
            break;
    }
    if(node->value.expression.operation != NEGATION_OP) {
        free_storage(fd, src2);
    }
    free_storage(fd, src1);
}

int parse_expression(FILE *fd, struct ast_node *node, storage_t res, bool can_allocate_ident) {
    switch(node->type) {
    case INTEGER_T:
        li_oper_backend(fd, res, node->value.i);
        break;
    case IDENT_T:
        return load_ident(fd, res, node->value.str, can_allocate_ident);
        break;
    case EXPRESSION_T:
        return translate_expression(fd, node, res, true);
        break;
    default:
        fprintf(stderr, "not implemented operation!");
        return -1;
        break;
    }
    return 0;
}

int parse_loop(FILE *fd, struct ast_node *node) {
    return -1;
}

int parse_branch(FILE *fd, struct ast_node *node) {
    return -1;
}

int parse_function_call(FILE *fd, struct ast_node *node) {
    return -1;
}

int parse_statement(FILE *fd, struct ast_node *node) {
    switch(node->type) {
    case EXPRESSION_T:
    case IDENT_T:
    case STRING_T:
    case REAL_T:
    case CAST_T:
        return parse_expression(fd, node, dummy, false);
        break;
    case FOR_T:
    case WHILE_T:
        return parse_loop(fd, node);
        break;
    case FUNCTION_CALL_T:
        return parse_function_call(fd, node);
        break;
    case BRANCH_T:
        return parse_branch(fd, node);
        break;
    default:
        return -1;
        break;
    }
}

int parse_statement_list(FILE *fd, struct ast_node *node) {
    switch (node->type) {
    case AST_LIST_ELEMENT_T:
        for(struct ast_node *cur = node; cur != NULL; cur = cur->value.ast_list_element.next) {
            if(parse_statement(fd, cur->value.ast_list_element.node) == -1) {
                return -1;
            }
        }
        break;
    case EXPRESSION_T:
    case IDENT_T:
    case INTEGER_T:
    case REAL_T:
    case RETURN_T:
    case CAST_T:
    case FOR_T:
    case WHILE_T:
    case BRANCH_T:
    case FUNCTION_CALL_T:
        return parse_statement(fd, node);
        break;
    default:
        fprintf(stderr, "unsupported statement");
        return -1;
        break;
    }
    return 0;
}

int parse_function_def(FILE *fd, struct ast_node *node) {
    if(node->type != FUNCTION_T) {
        return -1;
    }
    fprintf(fd, "%s:\n", node->value.function.function_name->value.str);
    return parse_statement_list(fd, node->value.function.stmt);
}

int parse_function_def_list(FILE *fd, struct ast_node *node) {
    switch(node->type) {
    case AST_LIST_ELEMENT_T:
        for(struct ast_node *cur = node; cur != NULL; cur = cur->value.ast_list_element.next) {
            if(parse_function_def(fd, cur->value.ast_list_element.node) == -1) {
                return -1;
            }
        }
        break;
    case FUNCTION_T:
        return parse_function_def(fd, node);
        break;
    default:
        fprintf(stderr, "something else in function definition space!");
        return -1;
        break;
    }
    return 0;
}

int translate(FILE *fd, struct ast_node *root){
    return parse_function_def_list(fd, root);
}
