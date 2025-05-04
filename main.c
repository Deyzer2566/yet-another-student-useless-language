#include <stdio.h>
#include "out/syntaxer.tab.h"
#include "ast.h"
void yyerror(const char *str)
{
    fprintf(stderr,"ошибка: %s\n",str);
}

void draw_ast_tree(struct ast_node *node, int offset);

void draw_ast_tree(struct ast_node *node, int offset) {
    for(int i = 0;i<offset-1;i++) {
        printf(" ");
    }
    switch (node->type) {
    case INTEGER_T:
        printf("->(int)%d", node->value.i);
        break;
    case REAL_T:
        printf("->(real)%f", node->value.f);
        break;
    case STRING_T:
        printf("->(string)%s", node->value.str);
        break;
    case IDENT_T:
        printf("->(ident)%s", node->value.str);
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
        default:
            printf("unimplemnted operation\n");
            break;
        }
        draw_ast_tree(node->value.expression.left, offset+1);
        draw_ast_tree(node->value.expression.right, offset+1);
        break;
    case AST_LIST_ELEMENT_T:
        struct ast_node *cur = node;
        for(struct ast_node *cur = node; cur != NULL; cur = cur->value.ast_list_element.next)
            draw_ast_tree(cur->value.ast_list_element.node, offset+1);
        break;
    default:
        printf("unimplemented ast node type");
        break;
    }
    printf("\n");
}

extern struct ast_node *root;
  
int main()
{
    yyparse();
    draw_ast_tree(root, 1);
    free_node(root);
}