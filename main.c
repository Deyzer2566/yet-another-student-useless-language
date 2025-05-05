#include <stdio.h>
#include "out/syntaxer.tab.h"
#include "ast.h"
extern int yylineno;
int yyerror(const char *str)
{
    fprintf(stderr,"ошибка: %s на %d строке\n",str,yylineno);
}

void draw_ast_tree(struct ast_node *node, int offset);
void draw_type(enum type_t *type, int offset) {
    for(int i = 0;i<offset-1;i++) {
        printf(" ");
    }
    printf("|");
    switch(*type) {
    case INTEGER_T:
        printf("int\n");
        break;
    case REAL_T:
        printf("real\n");
        break;
    case STRING_T:
        printf("string\n");
        break;
    default:
        printf("unexpected type\n");
        break;
    }
}

void draw_ast_tree(struct ast_node *node, int offset) {
    for(int i = 0;i<offset-1;i++) {
        printf(" ");
    }
    printf("|");
    if(node == NULL) {
        printf("(NULL)\n");
        return;
    }
    switch (node->type) {
    case INTEGER_T:
        printf("->(int)%d\n", node->value.i);
        break;
    case REAL_T:
        printf("->(real)%f\n", node->value.f);
        break;
    case STRING_T:
        printf("->(string)%s\n", node->value.str);
        break;
    case IDENT_T:
        printf("->(ident)%s\n", node->value.str);
        break;
    case EXPRESSION_T:
        switch (node->value.expression.operation) {
        case PLUS_OP:
            printf("+\n");
            break;
        case MINUS_OP:
            printf("-\n");
            break;
        case MULTIPLICATION_OP:
            printf("*\n");
            break;
        case DIVISION_OP:
            printf("/\n");
            break;
        case ASSIGN_OP:
            printf("=\n");
            break;
        case EQ_OP:
            printf("==\n");
            break;
        case LARGER_OP:
            printf(">\n");
            break;
        case LARGER_OR_EQ_OP:
            printf(">=\n");
            break;
        case SMALLER_OP:
            printf("<\n");
            break;
        case SMALLER_OR_EQ_OP:
            printf("<=\n");
            break;
        case LOGICAL_AND_OP:
            printf("&&\n");
            break;
        case LOGICAL_OR_OP:
            printf("||\n");
            break;
        case LOGICAL_NOT_OP:
            printf("!\n");
            break;
        case BITWISE_AND_OP:
            printf("&\n");
            break;
        case BITWISE_OR_OP:
            printf("|\n");
            break;
        case BITWISE_NOT_OP:
            printf("~\n");
            break;
        case NEGATION_OP:
            printf("-\n");
            break;
        case XOR_OP:
            printf("^\n");
            break;
        case NEQ_OP:
            printf("!=\n");
            break;
        default:
            printf("unimplemnted operation\n");
            break;
        }
        draw_ast_tree(node->value.expression.left, offset+1);
        draw_ast_tree(node->value.expression.right, offset+1);
        break;
    case AST_LIST_ELEMENT_T:
        printf("\n");
        struct ast_node *cur = node;
        // for(struct ast_node *cur = node; cur != NULL; cur = cur->value.ast_list_element.next)
            draw_ast_tree(cur->value.ast_list_element.node, offset+1);
        draw_ast_tree(cur->value.ast_list_element.next, offset+1);
        break;
    case FUNCTION_CALL_T:
        printf("->(function_call)%s\n", node->value.function_call.function_name->value.str);
        draw_ast_tree(node->value.function_call.param, offset+1);
        break;
    case BRANCH_T:
        printf("->(if)\n");
        draw_ast_tree(node->value.branch.expression, offset+1);
        draw_ast_tree(node->value.branch.then_stmt, offset+1);
        draw_ast_tree(node->value.branch.else_stmt, offset+1);
        break;
    case WHILE_T:
        printf("->(while)\n");
        draw_ast_tree(node->value.while_loop.expression, offset+1);
        draw_ast_tree(node->value.while_loop.stmt, offset+1);
        break;
    case FOR_T:
        printf("->(for)\n");
        draw_ast_tree(node->value.for_loop.init, offset+1);
        draw_ast_tree(node->value.for_loop.limit, offset+1);
        draw_ast_tree(node->value.for_loop.step, offset+1);
        draw_ast_tree(node->value.for_loop.stmt, offset+1);
        break;
    case CAST_T:
        printf("->(cast)\n");
        draw_type(node->value.cast.cast_type, offset+1);
        draw_ast_tree(node->value.cast.fact, offset+1);
        break;
    case FUNCTION_T:
        printf("->(function_definition)\n");
        draw_type(node->value.function.type, offset+1);
        draw_ast_tree(node->value.function.function_name, offset+1);
        draw_ast_tree(node->value.function.params, offset+1);
        draw_ast_tree(node->value.function.stmt, offset+1);
        break;
    case IDENT_DESCRIPTION_T:
        printf("->(ident_description)\n");
        draw_type(node->value.ident_description.type, offset+1);
        draw_ast_tree(node->value.ident_description.ident, offset+1);
        break;
    case RETURN_T:
        printf("->(return)\n");
        draw_ast_tree(node->value.return_.return_value, offset+1);
        break;
    default:
        printf("unimplemented ast node type\n");
        break;
    }
}

extern struct ast_node *root;
  
int main()
{
    yyparse();
    draw_ast_tree(root, 1);
    free_node(root);
}