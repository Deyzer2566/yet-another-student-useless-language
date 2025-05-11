#include "translator.h"
#include "mal.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

int parse_expression(FILE *fd, struct ast_node *node, storage_t res, bool can_allocate_ident);
int parse_statement_list(FILE *fd, struct ast_node *node);
int parse_function_call(FILE *fd, struct ast_node *node, storage_t res);
char *get_label();
char *get_label_with_counter();

int translate_expression(FILE *fd, struct ast_node *node, storage_t dest, bool can_allocate_ident) {
    storage_t src1, src2;
    allocate_storage(dest);
    src1 = get_storage(fd);
    if(parse_expression(fd, node->value.expression.left, src1, node->value.expression.operation != NEGATION_OP && can_allocate_ident) == -1)
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
        return translate_expression(fd, node, res, can_allocate_ident);
        break;
    case FUNCTION_CALL_T:
        return parse_function_call(fd, node, res);
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

struct label_waterfall_list_t {
    struct list_header_t *prev;
    struct list_header_t *next;
    char *label;
    unsigned int branch_counter;
};
struct label_waterfall_list_t *begin=NULL;
struct label_waterfall_list_t *end=NULL;
void add_label_level(char *label) {
    struct label_waterfall_list_t *new_label = malloc(sizeof(struct label_waterfall_list_t));
    new_label->next = NULL;
    new_label->prev = (struct list_header_t *)end;
    if(begin == NULL) {
        begin = new_label;
    } else {
        end->next = (struct list_header_t *)new_label;
    }
    end = new_label;
    end->label = malloc(strlen(label));
    end->branch_counter = 0;
    strcpy(end->label, label);
}
void pop_label_level() {
    free(end->label);
    struct label_waterfall_list_t *last = end;
    end = (struct label_waterfall_list_t *)end->prev;
    if(end != NULL)
        end->next = NULL;
    free(last);
}
unsigned int get_label_branch_count() {
    return end->branch_counter;
}
void inc_label_branch_count() {
    end->branch_counter++;
}
#define LABEL_SPLITTER "_"
int itoa(int num, char *buff, int radix) {
    char local_buff[26];
    int pointer = 0;
    do {
        if(num%radix > 0 && num%radix < 10)
            local_buff[25-pointer] = num % radix+'0';
        else
            local_buff[25-pointer] = num % radix+'a';
        pointer++;
        num /= radix;
    } while(num / radix > 0);
    for(int i = 0;i<pointer;i++) {
        buff[i] = local_buff[25-i];
    }
    buff[pointer] = 0;
    return pointer;
}
char *get_label() {
    size_t len = 0;
    for(struct label_waterfall_list_t *cur = begin; cur != NULL; cur = (struct label_waterfall_list_t *)cur->next) {
        len += strlen(cur->label);
        len += sizeof(LABEL_SPLITTER);
    }
    len += 1;
    char *label = malloc(len);
    label[0] = 0;
    for(struct label_waterfall_list_t *cur = begin; cur != NULL; cur = (struct label_waterfall_list_t *)cur->next) {
        strcat(label, cur->label);
        strcat(label, LABEL_SPLITTER);
    }
    return label;
}
char *get_label_with_counter() {
    size_t len = 0;
    for(struct label_waterfall_list_t *cur = begin; cur != NULL; cur = (struct label_waterfall_list_t *)cur->next) {
        len += strlen(cur->label);
        char num[26];
        itoa(cur->branch_counter, num, 10);
        len += strlen(num);
        len += sizeof(LABEL_SPLITTER);
    }
    len += 1;
    char *label = malloc(len);
    label[0] = 0;
    for(struct label_waterfall_list_t *cur = begin; cur != NULL; cur = (struct label_waterfall_list_t *)cur->next) {
        strcat(label, cur->label);
        char num[26];
        itoa(cur->branch_counter, num, 10);
        strcat(label, LABEL_SPLITTER);
        strcat(label, num);
    }
    return label;
}
int parse_branch(FILE *fd, struct ast_node *node) {
    storage_t expr_result = get_storage(fd);
    if(parse_expression(fd, node->value.branch.expression, expr_result, false) == -1) {
        return -1;
    }
    inc_label_branch_count();
    char *label_else = get_label_with_counter();
    inc_label_branch_count();
    char *label_exit = get_label_with_counter();
    beq_oper_backend_label(fd, expr_result, zero, label_else);
    if(parse_statement_list(fd, node->value.branch.then_stmt) == -1) {
        return -1;
    }
    jal_oper_backend_label(fd, zero, label_exit);
    fprintf(fd, "%s:\n", label_else);
    if(node->value.branch.else_stmt != NULL) {
        if(parse_statement_list(fd, node->value.branch.else_stmt) == -1) {
            return -1;
        }
    }
    fprintf(fd, "%s:\n", label_exit);
    free_storage(fd, expr_result);
    free(label_else);
    free(label_exit);
}

int parse_function_call(FILE *fd, struct ast_node *node, storage_t res) {
    size_t arg_count = 0;
    for(struct ast_node *arg = node->value.function_call.param; arg != NULL; arg = arg->value.ast_list_element.next) {
        arg_count ++;
    }
    if(arg_count > ABI_REGS_COUNT) {
        fprintf(stderr, "count of arguments more than space for them!");
        return -1;
    }
    struct ast_node *cur_param = node->value.function_call.param;
    for(int i = 0;i < ABI_REGS_COUNT && cur_param != NULL; i++) {
        get_specific_storage(fd, r1+i);
        parse_expression(fd, cur_param->value.ast_list_element.node, r1+i, false);
        cur_param = cur_param->value.ast_list_element.next;
    }
    get_specific_storage(fd, lr);
    jal_oper_backend_label(fd, lr, node->value.function_call.function_name->value.str);
    free_storage(fd, lr);
    for(int i = arg_count-1;i >=0; i--) {
        free_storage(fd, r1+i);
    }
    return 0;
}

int parse_statement(FILE *fd, struct ast_node *node) {
    fwrite("\n",1,1,fd);
    switch(node->type) {
    case EXPRESSION_T:
    case IDENT_T:
    case STRING_T:
    case REAL_T:
        return parse_expression(fd, node, dummy, true);
        break;
    case CAST_T:
    case FUNCTION_CALL_T:
        return parse_expression(fd, node, dummy, false);
        break;
    case FOR_T:
    case WHILE_T:
        return parse_loop(fd, node);
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
    if(node == NULL) {
        return 0;
    }
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
    add_label_level(node->value.function.function_name->value.str);
    fprintf(fd, "%s:\n", node->value.function.function_name->value.str);
    new_space();
    inc_label_branch_count();
    char *stack_size_label = get_label_with_counter();
    allocate_stack_label(fd, stack_size_label);
    if(parse_statement_list(fd, node->value.function.stmt) == -1) {
        return -1;
    }
    free_stack(fd);
    jalr_oper_backend(fd, zero, lr, 0);
    fprintf(fd, "%s:\ndata %d*1\n", stack_size_label, (int32_t)(size_space())*WORD_SIZE);
    pop_space();
    free(stack_size_label);
    pop_label_level();
    return 0;
}

int parse_function_def_list(FILE *fd, struct ast_node *node) {
    li_oper_backend(fd, sp, 0xffff+1);

    li_oper_backend(fd, r1, 87654321);//проверяем, что после выполнения main
    li_oper_backend(fd, fp, 12345678);//содержимое стека будет корректным

    jal_oper_backend_label(fd, lr, "main");
    ebreak_oper_backend(fd);
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
    fprintf(fd, "print:\newrite x1\njalr x0, x%d, 0\n", lr);
    fprintf(fd, "exit:\nebreak\n");
    return 0;
}

int translate(FILE *fd, struct ast_node *root){
    return parse_function_def_list(fd, root);
}
