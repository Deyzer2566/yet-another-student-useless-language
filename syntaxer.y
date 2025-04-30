%{
#include <stdio.h>
#include <string.h>
%}

%token NUMBER
%token LBRACKET RBRACKET
%token EOL 
%token ADD MUL SUB DIV

%token IDENT
%union {
    int i;
    float f;
    char *str;
}

%type <str> IDENT
%type <i> NUMBER

%token ASSIGN

%{ 
#include <stdbool.h>
#include <stdlib.h>
struct list_header_t {
    struct list_header_t *next;
};

struct idents_list_t {
    struct list_header_t list_header;
    const char *key;
    YYSTYPE value;
};

struct idents_list_t *ident = NULL;

void update_ident(const char* key, YYSTYPE value) {
    struct idents_list_t *cur = ident;
    while(cur != NULL) {
        if(strcmp(cur->key, key) == 0) {
            cur->value = value;
            return;
        }
        if(cur->list_header.next == NULL)
            break;
        else 
            cur = (struct idents_list_t*)cur->list_header.next;
    }
    if(cur == NULL) {
        cur = malloc(sizeof(struct idents_list_t));
		ident = cur;
    } else {
        cur->list_header.next = malloc(sizeof(struct idents_list_t));
        cur = (struct idents_list_t*)cur->list_header.next;
    }
    cur->key = key;
    cur->value = value;
	cur->list_header.next = NULL;
}

struct yystype_or_err {
    YYSTYPE ret_value;
    bool valid;
};

struct yystype_or_err get_ident(const char* key) {
    struct idents_list_t *cur = ident;
    while(cur != NULL) {
        if(strcmp(cur->key, key) == 0) {
            return (struct yystype_or_err){.valid = true, .ret_value = cur->value};
        }
        cur = (struct idents_list_t*)cur->list_header.next;
    }
    return (struct yystype_or_err){.valid = false};
}
%}

%%

stmt_list:
	| stmt_list stmt EOL
	;

stmt:
	| expr { printf("=%d\n", $<i>1); }
	| IDENT ASSIGN expr { printf("%s now is %d\n", $<str>1, $<i>3); update_ident($<str>1, (YYSTYPE)$<i>3);}
	;

expr
	: expr ADD term { $<i>$ = $<i>1 + $<i>3; }
	| expr SUB term { $<i>$ = $<i>1 - $<i>3; }
	| term
	;

term
	: term MUL factor { $<i>$ = $<i>1 * $<i>3; }
	| term DIV factor { $<i>$ = $<i>1 / $<i>3; }
	| factor
	;

factor
	: LBRACKET expr RBRACKET { $<i>$ = $<i>2; }
	| NUMBER
	| IDENT { struct yystype_or_err ret_value = get_ident($<str>1); if(ret_value.valid) { $<i>$ = ret_value.ret_value.i; } else { fprintf(stderr, "Ошибка доступа к идентификатору %s", $<str>1); exit(1);} }
	;

%%

void yyerror(const char *str)
{
		fprintf(stderr,"ошибка: %s\n",str);
} 
  
int main()
{
		yyparse();
}