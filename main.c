#include <stdio.h>
#include "out/syntaxer.tab.h"
#include "ast.h"
#include "ast_list.h"
void yyerror(const char *str)
{
    fprintf(stderr,"ошибка: %s\n",str);
}

void draw_ast_list_tree(struct ast_node_list *root, int offset);
void draw_ast_tree(struct ast_node *node, int offset);

void draw_ast_list_tree(struct ast_node_list *root, int offset) {
    for(int i = 0;i<offset;i++) {
        printf(" ");
    }
    for(; root != NULL; root = (struct ast_node_list *)root->next) {
        draw_ast_tree(root->node, offset+1);
    }
    printf("\n");
}

void draw_ast_tree(struct ast_node *node, int offset) {
    for(int i = 0;i<offset;i++) {
        printf(" ");
    }
    if(node->type != EXPRESSION_T) {
        switch (node->type) {
        case INTEGER_T:
            printf("(int)->%d", node->value.i);
            break;
        case REAL_T:
            printf("(real)->%f", node->value.f);
            break;
        case STRING_T:
            printf("(string)->%s", node->value.str);
            break;
        case IDENT_T:
            printf("(ident)->%s", node->value.str);
            break;
        }
    } else {
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
        }
        draw_ast_tree(node->value.expression.left, offset+1);
        draw_ast_tree(node->value.expression.right, offset+1);
    }
    printf("\n");
}

extern struct ast_node_list *root;
  
int main()
{
    yyparse();
    draw_ast_list_tree(root, 0);
}