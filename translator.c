#include "translator.h"
#include "mal.h"
#include "list.h"
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
char *exit_function_label;
void inc_label_branch_count();
enum ident_type_t ast_type_to_mal(enum type_t type) {
    switch (type) {
    case INTEGER_T:
        return INTEGER;
        break;
    case REAL_T:
        return REAL;
        break;
    case STRING_T:
        return POINTER;
        break;
    default:
        return UNKNOWN;
        break;
    }
}


struct function_definition_t {
    char *name;
    enum ident_type_t return_value_type;
};
make_list(function_definition_t, struct function_definition_t);
make_find_by_str(struct function_definition_t, function_definition_t, name);
make_add(function_definition_t, struct function_definition_t);
make_free(function_definition_t, struct function_definition_t);
struct function_definition_t_list_t *functions = NULL;

struct static_ident_t {
    char *label;
    uint32_t *contain;
    size_t len;
    enum ident_type_t type;
};
make_list(static_ident_t, struct static_ident_t);
make_add(static_ident_t, struct static_ident_t);
make_free(static_ident_t, struct static_ident_t);
struct static_ident_t_list_t *static_idents = NULL;

int translate_int_operation(FILE *fd, struct ast_node *node, storage_t dest, storage_t src1, storage_t src2) {
    switch(node->value.expression.operation) {
        case PLUS_OP:
            add_oper_backend(fd, dest, src1, src2);
            break;
        case MINUS_OP:
            sub_oper_backend(fd, dest, src1, src2);
            break;
        case EQ_OP:
            seq_oper_backend(fd, dest, src1, src2);
            break;
        case MULTIPLICATION_OP:
            mul_oper_backend(fd, dest, src1, src2);
            break;
        case LSH_OP:
            sll_oper_backend(fd, dest, src1, src2);
            break;
        case RSH_OP:
            sra_oper_backend(fd, dest, src1, src2);
            break;
        case BITWISE_AND_OP:
            and_oper_backend(fd, dest, src1, src2);
            break;
        case BITWISE_OR_OP:
            or_oper_backend(fd, dest, src1, src2);
            break;
        case SMALLER_OP:
            slt_oper_backend(fd, dest, src1, src2);
            break;
        case LARGER_OR_EQ_OP:
            sge_oper_backend(fd, dest, src1, src2);
            break;
        case LARGER_OP:
            push_oper(fd, src1);
            sge_oper_backend(fd, dest, src1, src2);
            push_oper(fd, dest);
            sne_oper_backend(fd, dest, src1, src2);
            pop_oper(fd, src1);
            and_oper_backend(fd, dest, src1, dest);
            pop_oper(fd, src1);
            break;
        case SMALLER_OR_EQ_OP:
            push_oper(fd, src1);
            slt_oper_backend(fd, dest, src1, src2);
            push_oper(fd, dest);
            seq_oper_backend(fd, dest, src1, src2);
            pop_oper(fd, src1);
            or_oper_backend(fd, dest, src1, dest);
            pop_oper(fd, src1);
            break;
        case NEGATION_OP:
            sub_oper_backend(fd, dest, zero, src1);
            break;
        case LOGICAL_NOT_OP:
            sne_oper_backend(fd, dest, src1, zero);
            xori_oper_backend(fd, dest, dest, (sword_t)1);
            break;
        case DIVISION_OP:
            div_oper_backend(fd, dest, src1, src2);
            break;
        case XOR_OP:
            xor_oper_backend(fd, dest, src1, src2);
            break;
        case MOD_OP:
            rem_oper_backend(fd, dest, src1, src2);
            break;
        case NEQ_OP:
            sne_oper_backend(fd, dest, src1, src2);
            break;
        case LOGICAL_AND_OP:
            push_oper(fd, src1);
            push_oper(fd, src2);
            sne_oper_backend(fd, src1, src1, zero);
            sne_oper_backend(fd, src2, src2, zero);
            and_oper_backend(fd, dest, src1, src2);
            pop_oper(fd, src2);
            pop_oper(fd, src1);
            break;
        case LOGICAL_OR_OP:
            push_oper(fd, src1);
            push_oper(fd, src2);
            sne_oper_backend(fd, src1, src1, zero);
            sne_oper_backend(fd, src2, src2, zero);
            or_oper_backend(fd, dest, src1, src2);
            pop_oper(fd, src2);
            pop_oper(fd, src1);
            break;
        default:
            fprintf(stderr,"not implemented expression %d", node->value.expression.operation);
            return -1;
            break;
    }
    return 0;
}

int translate_real_operation(FILE *fd, struct ast_node *node, storage_t dest, storage_t src1, storage_t src2) {
    push_oper(fd, ret);
    push_oper(fd, lr);
    push_oper(fd, r1);
    push_oper(fd, r2);
    push_oper(fd, src1);
    push_oper(fd, src2);
    load_oper_backend(fd, r2, sp, 0);
    load_oper_backend(fd, r1, sp, WORD_SIZE);
    enum ident_type_t dest_type = UNKNOWN;
    switch(node->value.expression.operation) {
        case PLUS_OP:
            jal_oper_backend_label(fd, lr, "__real_sum");
            break;
        case NEGATION_OP:
            jal_oper_backend_label(fd, lr, "__real_negation");
            break;
        case MINUS_OP:
            jal_oper_backend_label(fd, lr, "__real_sub");
            break;
        case MULTIPLICATION_OP:
            jal_oper_backend_label(fd, lr, "__real_mul");
            break;
        case DIVISION_OP:
            jal_oper_backend_label(fd, lr, "__real_div");
            break;
        case EQ_OP:
            jal_oper_backend_label(fd, lr, "__real_eq");
            dest_type = INTEGER;
            break;
        case NEQ_OP:
            jal_oper_backend_label(fd, lr, "__real_neq");
            dest_type = INTEGER;
            break;
        case SMALLER_OP:
            jal_oper_backend_label(fd, lr, "__real_smaller");
            dest_type = INTEGER;
            break;
        case SMALLER_OR_EQ_OP:
            jal_oper_backend_label(fd, lr, "__real_smaller_or_eq");
            dest_type = INTEGER;
            break;
        case LARGER_OP:
            jal_oper_backend_label(fd, lr, "__real_larger");
            dest_type = INTEGER;
            break;
        case LARGER_OR_EQ_OP:
            jal_oper_backend_label(fd, lr, "__real_larger_or_eq");
            dest_type = INTEGER;
            break;
        default:
            fprintf(stderr,"not implemented expression");
            return -1;
            break;
    }
    pop_oper(fd, src2);
    pop_oper(fd, src1);
    pop_oper(fd, r2);
    pop_oper(fd, r1);
    pop_oper(fd, lr);
    add_oper_backend(fd, dest, ret, zero);
    set_storage_type(dest, dest_type);
    pop_oper(fd, ret);
    return 0;
}

int translate_expression(FILE *fd, struct ast_node *node, storage_t dest, bool can_allocate_ident) {
    storage_t src1, src2;
    set_storage_type(dest, UNKNOWN);
    allocate_storage(dest);
    src1 = get_storage(fd);
    bool isSingleOperandOperation = \
        node->value.expression.operation == NEGATION_OP || \
        node->value.expression.operation == DEREF_POINTER_OP || \
        node->value.expression.operation == LOGICAL_NOT_OP;
    if(parse_expression(fd, node->value.expression.left, src1, !isSingleOperandOperation && can_allocate_ident) == -1)
        return -1;
    if(!isSingleOperandOperation) {
        allocate_storage(dest);
        allocate_storage(src1);
        src2 = get_storage(fd);
        if(parse_expression(fd, node->value.expression.right, src2, false) == -1) {
            return -1;
        }
        switch(node->value.expression.operation) {
        case ASSIGN_OP:
            if(node->value.expression.left->type == IDENT_T) {
                addi_oper_backend(fd, src1, src2, 0);
                set_storage_type(src1, get_storage_type(src2));
                update_ident(fd, src1, node->value.expression.left->value.str);
                set_storage_type(dest, get_storage_type(src1));
            } else {
                save_oper_backend(fd, src2, src1, 0);
                set_storage_type(src1, get_storage_type(src2));
            }
            addi_oper_backend(fd, dest, src1, 0);
            break;
        default:
            if((get_storage_type(src1) != get_storage_type(src2)) &&
                ((get_storage_type(src1) != POINTER || get_storage_type(src2) != INTEGER) &&
                (get_storage_type(src1) != INTEGER || get_storage_type(src2) != POINTER))) {
                fprintf(stderr, "different types of operands");
                return -1;
            } else {
                switch(get_storage_type(src2)) {
                case INTEGER:
                case POINTER:
                    if(translate_int_operation(fd, node, dest, src1, src2) == -1) {
                        return -1;
                    }
                    break;
                case REAL:
                    if(translate_real_operation(fd, node, dest, src1, src2) == -1) {
                        return -1;
                    }
                    break;
                default:
                    fprintf(fd, "invalid type for expression");
                    return -1;
                    break;
                }
            }
        }
        free_storage(fd, src2);
        if(get_storage_type(dest) == UNKNOWN) {
            set_storage_type(dest, get_storage_type(src1));
        }
    } else {
        switch(node->value.expression.operation) {
        case DEREF_POINTER_OP:
            load_oper_backend(fd, dest, src1, 0);
            set_storage_type(dest, INTEGER);
            break;
        case LOGICAL_NOT_OP:
        case NEGATION_OP:
            switch(get_storage_type(src1)) {
            case INTEGER:
                if(translate_int_operation(fd, node, dest, src1, dummy) == -1) {
                    return -1;
                }
                break;
            case REAL:
                if(translate_real_operation(fd, node, dest, src1, dummy) == -1) {
                    return -1;
                }
                break;
            default:
                fprintf(fd, "invalid type for expression");
                return -1;
                break;
            }
            if(get_storage_type(dest) == UNKNOWN) {
                set_storage_type(dest, get_storage_type(src1));
            }
            break;
        default:
            fprintf(stderr, "unsupported single operand operation");
            return -1;
            break;
        }
    }
    free_storage(fd, src1);
}

uword_t real_to_word(float f) {
    union {
        float f;
        int i;
    } converter;
    converter.f = f;
    return converter.i;
}

// 0xxxxxxx — если для кодирования потребуется один октет;
// 110xxxxx — если для кодирования потребуется два октета;
// 1110xxxx — если для кодирования потребуется три октета;
// 11110xxx — если для кодирования потребуется четыре октета;
// 10xxxxxx — продолжение символа.
uint32_t *utf8_to_unicode(uint8_t *utf8) {
    int byte_counter = 0;
    int must_be_bytes = 0;
    size_t len = 0;
    for(uint8_t *cur = utf8; (*cur) != '\0'; cur++) {
        if((*cur & 0xf8) == 0xf0 && byte_counter == 0) {
            must_be_bytes = 4;
            byte_counter = 1;
        } else if((*cur & 0xC0) == 0x80 && byte_counter < must_be_bytes && byte_counter != 0) {
            byte_counter++;
            if(byte_counter == must_be_bytes) {
                byte_counter = must_be_bytes = 0;
            }
            len++;
        } else if((*cur & 0xF0) == 0xE0 && byte_counter == 0) {
            must_be_bytes = 3;
            byte_counter = 1;
        } else if((*cur & 0xE0) == 0xC0 && byte_counter == 0) {
            must_be_bytes = 2;
            byte_counter = 1;
        } else if((*cur & 0x80) == 0 && byte_counter == 0) {
            len++;
        } else {
            return NULL;
        }
    }
    uint32_t cur_char = 0;
    size_t cur_char_counter = 0;
    uint32_t *unicode = malloc(sizeof(uint32_t)*len+1);
    unicode[len] = 0;
    for(uint8_t *cur = utf8; (*cur) != '\0'; cur++) {
        if((*cur & 0xf8) == 0xf0 && byte_counter == 0) {
            must_be_bytes = 4;
            cur_char = (*cur & 0x7);
            byte_counter = 1;
        } else if((*cur & 0xC0) == 0x80 && byte_counter < must_be_bytes && byte_counter != 0) {
            cur_char <<= 6;
            cur_char |= *cur & 0x3f;
            byte_counter++;
            if(byte_counter == must_be_bytes) {
                byte_counter = must_be_bytes = 0;
                unicode[cur_char_counter++] = cur_char;
            }
        } else if((*cur & 0xF0) == 0xE0 && byte_counter == 0) {
            must_be_bytes = 3;
            cur_char = (*cur & 0xf);
            byte_counter = 1;
        } else if((*cur & 0xE0) == 0xC0 && byte_counter == 0) {
            must_be_bytes = 2;
            cur_char = (*cur & 0x1f);
            byte_counter = 1;
        } else if((*cur & 0x80) == 0 && byte_counter == 0) {
            cur_char = *cur;
            unicode[cur_char_counter++] = cur_char;
        } else {
            free(unicode);
            return NULL;
        }
    }
    return unicode;
}

size_t unicode_len(uint32_t *unicode) {
    size_t len = 0;
    if(unicode == NULL)
        return (size_t)0;
    for(;unicode[len]!=0;len++);
    return len;
}

int parse_cast(FILE *fd, struct ast_node *node, storage_t res) {
    if(parse_expression(fd, node->value.cast.fact, res, false) == -1) {
        return -1;
    }
    switch(*node->value.cast.cast_type) {
    case INTEGER_T:
        switch(get_storage_type(res)) {
        case INTEGER:
            break;
        case REAL:
            push_oper(fd, ret);
            push_oper(fd, lr);
            push_oper(fd, r1);
            add_oper_backend(fd, r1, res, zero);
            jal_oper_backend_label(fd, lr, "__real_to_int");
            pop_oper(fd, r1);
            pop_oper(fd, lr);
            add_oper_backend(fd, res, ret, zero);
            push_oper(fd, ret);
            set_storage_type(res, INTEGER);
            break;
        default:
            fprintf(stderr, "unexpected cast");
            return -1;
            break;
        }
        break;
    case REAL_T:
        switch(get_storage_type(res)) {
        case INTEGER:
            push_oper(fd, ret);
            push_oper(fd, lr);
            push_oper(fd, r1);
            add_oper_backend(fd, r1, res, zero);
            jal_oper_backend_label(fd, lr, "__int_to_real");
            pop_oper(fd, r1);
            pop_oper(fd, lr);
            add_oper_backend(fd, res, ret, zero);
            push_oper(fd, ret);
            set_storage_type(res, REAL);
            break;
        case REAL:
            break;
        default:
            fprintf(stderr, "unexpected cast");
            return -1;
            break;
        }
        break;
    default:
        fprintf(stderr, "unexcepted target cast");
        return -1;
        break;
    }
    return 0;
}

int parse_expression(FILE *fd, struct ast_node *node, storage_t res, bool can_allocate_ident) {
    switch(node->type) {
    case INTEGER_T:
        li_oper_backend(fd, res, node->value.i);
        set_storage_type(res, INTEGER);
        break;
    case REAL_T:
        li_oper_backend(fd, res, (sword_t)real_to_word(node->value.f));
        set_storage_type(res, REAL);
        break;
    case STRING_T:
        inc_label_branch_count();
        char *label = get_label_with_counter();
        uint32_t *unicode= utf8_to_unicode(node->value.str);
        add_static_ident_t(&static_idents, (struct static_ident_t){
            .contain = unicode,
            .len = unicode_len(unicode)+1,
            .type = POINTER,
            .label = label
        });
        li_oper_backend_label(fd, res, label);
        set_storage_type(res, POINTER);
        break;
    case IDENT_T:
        int ret = load_ident(fd, res, node->value.str);
        if(ret == -1 && can_allocate_ident) {
            create_ident(node->value.str);
            return load_ident(fd, res, node->value.str);
        }
        return ret;
        break;
    case EXPRESSION_T:
        return translate_expression(fd, node, res, can_allocate_ident);
        break;
    case FUNCTION_CALL_T:
        return parse_function_call(fd, node, res);
        break;
    case CAST_T:
        return parse_cast(fd, node, res);
        break;
    default:
        fprintf(stderr, "not implemented operation! %d", node->type);
        return -1;
        break;
    }
    return 0;
}

int parse_loop(FILE *fd, struct ast_node *node) {
    struct {
        struct ast_node *limit;
        struct ast_node *stmts;
    } loop;
    switch (node->type) {
    case FOR_T:
        loop.limit = node->value.for_loop.limit;
        loop.stmts = node->value.for_loop.stmt;
        break;
    case WHILE_T:
        loop.limit = node->value.while_loop.expression;
        loop.stmts = node->value.while_loop.stmt;
        break;
    default:
        fprintf(stderr, "unimplemented loop");
        return -1;
        break;
    }
    if(node->type == FOR_T) {
        if(parse_expression(fd, node->value.for_loop.init, zero, true) == -1) {
            return -1;
        }
    }
    inc_label_branch_count();
    char *loop_start = get_label_with_counter();
    inc_label_branch_count();
    char *loop_end = get_label_with_counter();
    storage_t res = get_storage(fd);
    fprintf(fd, "%s:\n", loop_start);
    if(parse_expression(fd, loop.limit, res, false) == -1) {
        return -1;
    }
    beq_oper_backend_label(fd, res, zero, loop_end);
    if(parse_statement_list(fd, loop.stmts) == -1) {
        return -1;
    }
    if(node->type == FOR_T) {
        if(parse_expression(fd, node->value.for_loop.step, zero, false) == -1) {
            return -1;
        }
    }
    jal_oper_backend_label(fd, zero, loop_start);
    fprintf(fd, "%s:\n", loop_end);
    free_storage(fd, res);
    free(loop_start);
    free(loop_end);
    return 0;
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
    else
        begin = NULL;
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
        if(num%radix >= 0 && num%radix < 10)
            local_buff[pointer] = num % radix+'0';
        else
            local_buff[pointer] = num % radix+'a';
        pointer++;
        num /= radix;
    } while(num > 0);
    for(int i = 0;i<pointer;i++) {
        buff[i] = local_buff[pointer-i-1];
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

int min(int a, int b) {
    return (a>b)?b:a;
}

int parse_function_call(FILE *fd, struct ast_node *node, storage_t res) {
    push_oper(fd, ret);
    push_oper(fd, lr);
    int arg_count = 0;
    for(struct ast_node *arg = node->value.function_call.param; arg != NULL; arg = arg->value.ast_list_element.next) {
        arg_count ++;
    }
    struct ast_node *cur_param = node->value.function_call.param;
    for(int i = 0;i < min(arg_count, ABI_REGS_COUNT); i++) {
        get_specific_storage(fd, r1+i);
        if(parse_expression(fd, cur_param->value.ast_list_element.node, r1+i, false) == -1) {
            return -1;
        }
        cur_param = cur_param->value.ast_list_element.next;
    }
    if(arg_count > ABI_REGS_COUNT) {
        addi_oper_backend(fd, sp, sp, WORD_SIZE*(-arg_count+ABI_REGS_COUNT));
        add_oper_backend(fd, ret, sp, zero);
        for(int i = ABI_REGS_COUNT; i < arg_count; i++) {
            storage_t temp = get_storage(fd);
            if(parse_expression(fd, cur_param->value.ast_list_element.node, temp, false) == -1) {
                return -1;
            }
            save_oper_backend(fd, temp, ret, i-ABI_REGS_COUNT);
            free_storage(fd, temp);
            cur_param = cur_param->value.ast_list_element.next;
        }
    }
    jal_oper_backend_label(fd, lr, node->value.function_call.function_name->value.str);
    if(arg_count > ABI_REGS_COUNT) {
        addi_oper_backend(fd, sp, sp, WORD_SIZE*(arg_count-ABI_REGS_COUNT));
    }
    for(int i = min(arg_count, ABI_REGS_COUNT)-1;i >=0; i--) {
        free_storage(fd, r1+i);
    }
    pop_oper(fd, lr);
    add_oper_backend(fd, res, ret, zero);
    struct function_definition_t *function = find_in_function_definition_t_list_by_str(functions, node->value.function_call.function_name->value.str);
    if(function == NULL) {
        return -1;
    }
    set_storage_type(res, function->return_value_type);
    pop_oper(fd, ret);
    return 0;
}

int parse_return(FILE *fd, struct ast_node *node) {
    storage_t return_storage = get_storage(fd);
    if(parse_expression(fd, node->value.return_.return_value, return_storage, false) == -1) {
        return -1;
    }
    add_oper_backend(fd, ret, return_storage, zero);
    free_storage(fd, return_storage);
    jal_oper_backend_label(fd, zero, exit_function_label);
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
    case RETURN_T:
        return parse_return(fd, node);
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
    inc_label_branch_count();
    exit_function_label = get_label_with_counter();
    size_t arg_counter = 0;
    for(struct ast_node *param = node->value.function.params; param != NULL; param = param->value.ast_list_element.next) {
        add_function_param(param->value.ast_list_element.node->value.ident_description.ident->value.str,
            (arg_counter<ABI_REGS_COUNT)?(r1+arg_counter):(zero),
            ((arg_counter<ABI_REGS_COUNT)?(-1-(int)arg_counter):(arg_counter-ABI_REGS_COUNT+1))*WORD_SIZE,
            ast_type_to_mal(*param->value.ast_list_element.node->value.ident_description.type));
        arg_counter++;
    }
    allocate_stack_label(fd, stack_size_label);
    if(parse_statement_list(fd, node->value.function.stmt) == -1) {
        return -1;
    }
    fprintf(fd, "%s:\n", exit_function_label);
    free_stack(fd);
    jalr_oper_backend(fd, zero, lr, 0);
    fprintf(fd, "%s:\ndata %d*1\n", stack_size_label, (int32_t)(size_space())*WORD_SIZE);
    for(struct static_ident_t_list_t *ident = static_idents; ident != NULL; ident = (struct static_ident_t_list_t *)ident->next) {
        fprintf(fd, "%s:\n", ident->value.label);
        for(size_t i = 0; i<ident->value.len; i++) {
            fprintf(fd, "data %d*1\n", (int)ident->value.contain[i]);
        }
        free(ident->value.contain);
    }
    free_static_ident_t_list(static_idents);
    static_idents = NULL;
    pop_space();
    free(stack_size_label);
    pop_label_level();
    free(exit_function_label);
    return 0;
}

int parse_function_def_list(FILE *fd, struct ast_node *node) {
    li_oper_backend(fd, sp, 0xffff+1);

    li_oper_backend(fd, r1, 87654321);//проверяем, что после выполнения main
    li_oper_backend(fd, fp, 12345678);//содержимое стека будет корректным

    jal_oper_backend_label(fd, lr, "main");
    ebreak_oper_backend(fd);
    
    fprintf(fd, "print:\newrite x1\njalr x0, x%d, 0\n", lr);
    fprintf(fd, "exit:\nebreak\n");
    
    add_function_definition_t(&functions,
        (struct function_definition_t){
            .name = "print",
            .return_value_type = INTEGER
        }
    );

    add_function_definition_t(&functions,
        (struct function_definition_t){
            .name = "exit",
            .return_value_type = INTEGER
        }
    );

    switch(node->type) {
    case AST_LIST_ELEMENT_T:
        for(struct ast_node *cur = node; cur != NULL; cur = cur->value.ast_list_element.next) {
            add_function_definition_t(&functions,
                (struct function_definition_t){
                    .name = cur->value.ast_list_element.node->value.function.function_name->value.str,
                    .return_value_type = ast_type_to_mal(*cur->value.ast_list_element.node->value.function.type)
                }
            );
        }
        for(struct ast_node *cur = node; cur != NULL; cur = cur->value.ast_list_element.next) {
            if(parse_function_def(fd, cur->value.ast_list_element.node) == -1) {
                return -1;
            }
        }
        break;
    case FUNCTION_T:
        add_function_definition_t(&functions,
            (struct function_definition_t){
                .name = node->value.function.function_name->value.str,
                .return_value_type = ast_type_to_mal(*node->value.function.type)
            }
        );
        return parse_function_def(fd, node);
        break;
    default:
        fprintf(stderr, "something else in function definition space!");
        return -1;
        break;
    }
    free_function_definition_t_list(functions);
    return 0;
}

int translate(FILE *fd, struct ast_node *root){
    return parse_function_def_list(fd, root);
}
