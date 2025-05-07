#include "mal.h"
#include <stdlib.h>
#include <string.h>

static int storage_access[STORAGE_COUNT]={0};
storage_t get_least_used_register() {
    int max_access = 0;
    storage_t reg_num = 0;
    for(int i = 0;i < STORAGE_COUNT; i++) {
        if(storage_access[i] > max_access) {
            max_access = storage_access[i];
            reg_num = i;
        }
        storage_access[i] ++;
    }
    storage_access[reg_num] = (signed)-3;
    reg_num += r1;
    return reg_num;
}

storage_t get_storage(FILE *fd) {
    storage_t least_used = get_least_used_register();
    addi_oper_backend(fd, sp, sp, (sword_t)-WORD_SIZE);
    save_oper_backend(fd, least_used, sp, 0);
    return least_used;
}

void free_storage(FILE *fd, storage_t storage) {
    load_oper_backend(fd, storage, sp, 0);
    addi_oper_backend(fd, sp, sp, WORD_SIZE);
}

void allocate_storage(storage_t storage) {
    if(storage != zero)
        storage_access[storage - r1] = 0;
}

struct ident_t {
    char* name;
    enum {
        ON_REGISTER,
        ON_STACK
    } map;
    union {
        storage_t reg;
        offset_t offset;
    } addr;
    enum {
        INT,
        REAL,
        STRING
    } type;
};

struct ident_list_t {
    struct list_header_t *next;
    struct ident_t ident;
};

struct ident_list_t *idents = NULL;
struct ident_t *find_ident_by_name(char *name) {
    for(struct ident_list_t *ident = idents; ident != NULL; ident = (struct ident_list_t *)ident->next) {
        if(strcmp(ident->ident.name, name) == 0) {
            return &ident->ident;
        }
    }
    return NULL;
}
struct ident_t *find_ident_by_reg(storage_t storage) {
    for(struct ident_list_t *ident = idents; ident != NULL; ident = (struct ident_list_t *)ident->next) {
        if(ident->ident.map == ON_REGISTER && ident->ident.addr.reg == storage) {
            return &ident->ident;
        }
    }
    return NULL;
}
int load_ident(FILE *fd, storage_t dest, char* name, bool create_if_not_exists) {
    struct ident_t *ident = find_ident_by_name(name);
    if(ident == NULL) {
        if(!create_if_not_exists) {
            return -1;
        }
        struct ident_t *new_ident = malloc(sizeof(struct ident_t));
        new_ident->name = malloc(strlen(name)+1);
        strcpy(new_ident->name, name);
        new_ident->map = ON_REGISTER;
        new_ident->addr.reg = dest;
        return 0;
    }
    switch (ident->map) {
    case ON_REGISTER:
        addi_oper_backend(fd, dest, ident->addr.reg, 0);
        break;
    case ON_STACK:
        if(abs(ident->addr.offset) > 2<<11) {
            storage_t temp = get_storage(fd);
            li_oper_backend(fd, temp, ident->addr.offset);
            add_oper_backend(fd, temp, temp, sp);
            load_oper_backend(fd, dest, temp, 0);
            free_storage(fd, temp);
        } else {
            load_oper_backend(fd, dest, sp, ident->addr.offset);           
        }
        ident->map = ON_REGISTER;
        ident->addr.reg = dest;
        break;
    default:
        fprintf(stderr, "unsupported load_ident map type");
        break;
    }
    return 0;
}
