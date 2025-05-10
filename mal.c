#include "mal.h"
#include <stdlib.h>
#include <string.h>

struct ident_t {
    char* name;
    enum {
        ON_REGISTER,
        ON_STACK
    } map;
    storage_t mapped_reg;
    union {
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
    bool border;
};

static storage_t get_least_used_register();
storage_t get_storage(FILE *fd);
void free_storage(FILE *fd, storage_t storage);
void allocate_storage(storage_t storage);
void get_specific_storage(FILE *fd, storage_t storage);
static struct ident_t *find_ident_by_name(char *name);
static struct ident_t *find_ident_by_reg(storage_t storage);
int load_ident(FILE *fd, storage_t dest, char* name, bool create_if_not_exists);
static void save_ident(FILE *fd, struct ident_t *ident);
static void inc_idents_addresses();
static void dec_idents_addresses();

static int storage_access[STORAGE_COUNT]={0};
static storage_t get_least_used_register() {
    int max_access = 0;
    storage_t reg_num = 0;
    for(int i = 0;i < STORAGE_COUNT; i++) {
        if(storage_access[i] > max_access) {
            max_access = storage_access[i];
            reg_num = i;
        }
        storage_access[i] ++;
    }
    reg_num += r1;
    return reg_num;
}

struct register_info_stack_list_t {
    struct list_header_t *next;
    struct ident_t *ident;
    int last_access;
};
struct register_info_stack_list_t *mapping_history[STORAGE_COUNT] = {NULL};
static void push_in_mapping_history(storage_t storage, struct ident_t *ident) {
    struct register_info_stack_list_t *new_info = malloc(sizeof(struct register_info_stack_list_t *));
    new_info->next = (struct list_header_t *)mapping_history[storage-r1];
    new_info->ident = ident;
    new_info->last_access = storage_access[storage-r1];
    mapping_history[storage-r1] = new_info;
}
static void pop_from_mapping_history(FILE* fd, storage_t storage) {
    struct register_info_stack_list_t *info = mapping_history[storage-r1];
    mapping_history[storage-r1] = (struct register_info_stack_list_t *)info->next;
    if(info->ident != NULL)
        load_ident(fd, storage, info->ident->name, false);
    storage_access[storage-r1] = info->last_access;
    free(info);
}
storage_t get_storage(FILE *fd) {
    storage_t least_used = get_least_used_register();
    get_specific_storage(fd, least_used);
    return least_used;
}

void free_storage(FILE *fd, storage_t storage) {
    struct ident_t *ident = find_ident_by_reg(storage);
    if(ident != NULL) {
        save_ident(fd, ident);
    }
    load_oper_backend(fd, storage, sp, 0);
    addi_oper_backend(fd, sp, sp, WORD_SIZE);
    dec_idents_addresses();
    pop_from_mapping_history(fd, storage);
}

void allocate_storage(storage_t storage) {
    if(storage != zero)
        storage_access[storage - r1] = (int)-STORAGE_COUNT;
}

void get_specific_storage(FILE *fd, storage_t storage) {
    addi_oper_backend(fd, sp, sp, (sword_t)-WORD_SIZE);
    save_oper_backend(fd, storage, sp, 0);
    struct ident_t *ident = find_ident_by_reg(storage);
    if(ident != NULL) {
        save_ident(fd, ident);
    }
    inc_idents_addresses();
    push_in_mapping_history(storage, ident);
    storage_access[storage-r1] = 0;
}

static struct ident_list_t *idents = NULL;
static struct ident_t *find_ident_by_name(char *name) {
    for(struct ident_list_t *ident = idents; ident != NULL; ident = (struct ident_list_t *)ident->next) {
        if(strcmp(ident->ident.name, name) == 0) {
            return &ident->ident;
        }
    }
    return NULL;
}
static struct ident_t *find_ident_by_reg(storage_t storage) {
    for(struct ident_list_t *ident = idents; ident != NULL; ident = (struct ident_list_t *)ident->next) {
        if(ident->ident.map == ON_REGISTER && ident->ident.mapped_reg == storage) {
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
        struct ident_list_t *new_ident = malloc(sizeof(struct ident_list_t));
        new_ident->ident.name = malloc(strlen(name)+1);
        new_ident->ident.name[0] = '\0';
        strcpy(new_ident->ident.name, name);
        new_ident->ident.map = ON_REGISTER;
        new_ident->ident.mapped_reg = dest;
        new_ident->next = (struct list_header_t *)idents;
        new_ident->ident.addr.offset = size_space()+WORD_SIZE;
        idents = new_ident;
        return 0;
    }
    switch (ident->map) {
    case ON_REGISTER:
        addi_oper_backend(fd, dest, ident->mapped_reg, 0);
        break;
    case ON_STACK:
        if(abs(ident->addr.offset) > 1<<11) {
            storage_t temp = get_storage(fd);
            li_oper_backend(fd, temp, ident->addr.offset);
            add_oper_backend(fd, temp, temp, sp);
            load_oper_backend(fd, dest, temp, 0);
            free_storage(fd, temp);
        } else {
            load_oper_backend(fd, dest, sp, ident->addr.offset);           
        }
        ident->map = ON_REGISTER;
        ident->mapped_reg = dest;
        break;
    default:
        fprintf(stderr, "unsupported load_ident map type");
        break;
    }
    return 0;
}
static void save_ident(FILE *fd, struct ident_t *ident) {
    ident->map = ON_STACK;
    save_oper_backend(fd, ident->mapped_reg, sp, ident->addr.offset);
}
static void inc_idents_addresses() {
    for (struct ident_list_t *cur = idents; cur != NULL; cur = (struct ident_list_t *)cur->next) {
        if(cur->ident.map == ON_STACK) {
            cur->ident.addr.offset += WORD_SIZE;
        }
    }
}
static void dec_idents_addresses() {
    for (struct ident_list_t *cur = idents; cur != NULL; cur = (struct ident_list_t *)cur->next) {
        if(cur->ident.map == ON_STACK) {
            cur->ident.addr.offset -= WORD_SIZE;
        }
    }
}

void new_space() {
    if(idents != NULL) {
        idents->border = true;
    }
}

void pop_space() {
    for (; idents != NULL && !idents->border; ) {
        struct ident_list_t *next = (struct ident_list_t *)idents->next;
        free(idents);
        idents = next;
    }
    if(idents != NULL)
        idents->border = false;
}
size_t size_space() {
    size_t count = 0;
    for (struct ident_list_t *cur = idents; cur != NULL && !cur->border; cur = (struct ident_list_t *)cur->next) {
        count++;
    }
    return count;
}

size_t count_idents() {
    size_t count = 0;
    for (struct ident_list_t *cur = idents; cur != NULL; cur = (struct ident_list_t *)cur->next) {
        count++;
    }
    return count;
}

void allocate_stack(FILE *fd, unsigned int word_count) {
    storage_t temp = r1;
    addi_oper_backend(fd, sp, sp, (sword_t)(-WORD_SIZE));
    save_oper_backend(fd, temp, sp, 0);
    li_oper_backend(fd, temp, word_count);
    sub_oper_backend(fd, sp, sp, temp);
    add_oper_backend(fd, temp, sp, temp);
    load_oper_backend(fd, temp, sp, WORD_SIZE);
}

void allocate_stack_label(FILE *fd, char *stack_size_label) {
    storage_t temp = r1;
    addi_oper_backend(fd, sp, sp, (sword_t)(-WORD_SIZE));
    save_oper_backend(fd, temp, sp, 0);
    li_oper_backend_label(fd, temp, stack_size_label);
    load_oper_backend(fd, temp, temp, 0);
    sub_oper_backend(fd, sp, sp, temp);
    add_oper_backend(fd, temp, sp, temp);
    load_oper_backend(fd, temp, sp, WORD_SIZE);
}

void free_stack(FILE *fd, unsigned int word_count) {
    storage_t temp = r1;
    addi_oper_backend(fd, sp, sp, (sword_t)(-WORD_SIZE));
    save_oper_backend(fd, temp, sp, 0);

    li_oper_backend(fd, temp, word_count);
    addi_oper_backend(fd, temp, temp, 2*WORD_SIZE);
    add_oper_backend(fd, sp, sp, temp);

    sub_oper_backend(fd, temp, sp, temp);
    addi_oper_backend(fd, temp, temp, (sword_t)-WORD_SIZE);
    load_oper_backend(fd, temp, temp, 0);
}

void free_stack_label(FILE *fd, char *stack_size_label) {
    storage_t temp = r1;
    addi_oper_backend(fd, sp, sp, (sword_t)(-WORD_SIZE));
    save_oper_backend(fd, temp, sp, 0);

    li_oper_backend_label(fd, temp, stack_size_label);
    load_oper_backend(fd, temp, temp, 0);
    addi_oper_backend(fd, temp, temp, 2*WORD_SIZE);
    add_oper_backend(fd, sp, sp, temp);

    sub_oper_backend(fd, temp, sp, temp);
    addi_oper_backend(fd, temp, temp, (sword_t)-WORD_SIZE);
    load_oper_backend(fd, temp, temp, 0);
}
