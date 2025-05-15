#include "mal.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>

struct ident_t {
    char* name;
    enum {
        ON_REGISTER,
        IN_MEMORY
    } map;
    storage_t mapped_reg;
    union {
        offset_t offset;
    } addr;
    enum {
        ON_STACK
    } memory_mapping_type;
    enum ident_type_t type;
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
int load_ident(FILE *fd, storage_t dest, char* name);
static void save_ident(FILE *fd, struct ident_t *ident, storage_t storage);

static int storage_access[STORAGE_COUNT]={0};
static storage_t get_least_used_register() {
    int max_access = INT_MIN;
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
    enum ident_type_t type;
};
struct register_info_stack_list_t *mapping_history[STORAGE_COUNT] = {NULL};
static void push_in_mapping_history(storage_t storage, struct ident_t *ident) {
    struct register_info_stack_list_t *new_info = malloc(sizeof(struct register_info_stack_list_t *));
    new_info->next = (struct list_header_t *)mapping_history[storage-r1];
    new_info->ident = ident;
    new_info->last_access = storage_access[storage-r1];
    new_info->type = get_storage_type(storage);
    mapping_history[storage-r1] = new_info;
}
static void pop_from_mapping_history(FILE* fd, storage_t storage) {
    struct register_info_stack_list_t *info = mapping_history[storage-r1];
    mapping_history[storage-r1] = (struct register_info_stack_list_t *)info->next;
    if(info->ident != NULL)
        load_ident(fd, storage, info->ident->name);
    storage_access[storage-r1] = info->last_access;
    set_storage_type(storage, info->type);
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
        save_ident(fd, ident, storage);
    }
    pop_oper(fd, storage);
    pop_from_mapping_history(fd, storage);
}

void allocate_storage(storage_t storage) {
    if(storage != zero)
        storage_access[storage - r1] = (int)-STORAGE_COUNT;
}

void get_specific_storage(FILE *fd, storage_t storage) {
    push_oper(fd, storage);
    struct ident_t *ident = find_ident_by_reg(storage);
    if(ident != NULL) {
        save_ident(fd, ident, storage);
    }
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
int load_ident(FILE *fd, storage_t dest, char* name) {
    struct ident_t *ident = find_ident_by_name(name);
    if(ident == NULL) {
        return -1;
    }
    set_storage_type(dest, ident->type);
    switch (ident->map) {
    case ON_REGISTER:
        addi_oper_backend(fd, dest, ident->mapped_reg, 0);
        break;
    case IN_MEMORY:
        switch(ident->memory_mapping_type) {
        case ON_STACK:
            if(abs(ident->addr.offset) > 1<<11) {
                storage_t temp = get_storage(fd);
                li_oper_backend(fd, temp, ident->addr.offset);
                add_oper_backend(fd, temp, temp, fp);
                load_oper_backend(fd, dest, temp, 0);
                free_storage(fd, temp);
            } else {
                load_oper_backend(fd, dest, fp, ident->addr.offset);
            }
            ident->map = ON_REGISTER;
            ident->mapped_reg = dest;
            break;
        }
        break;
    default:
        fprintf(stderr, "unsupported load_ident map type");
        break;
    }
    return 0;
}
void update_ident(FILE *fd, storage_t storage, char *name) {
    struct ident_t *ident = find_ident_by_name(name);
    assert(ident != NULL);
    ident->type = get_storage_type(storage);
    switch(ident->map) {
    case IN_MEMORY:
        save_ident(fd, ident, storage);
        break;
    case ON_REGISTER:
        add_oper_backend(fd, ident->mapped_reg, storage, zero);
        break;
    }
}
static void save_ident(FILE *fd, struct ident_t *ident, storage_t storage) {
    ident->map = IN_MEMORY;
    switch(ident->memory_mapping_type) {
    case ON_STACK:
        save_oper_backend(fd, storage, fp, ident->addr.offset);
        break;
    }
}
void create_ident(char *name) {
    struct ident_list_t *new_ident = malloc(sizeof(struct ident_list_t));
    new_ident->ident.name = malloc(strlen(name)+1);
    new_ident->ident.name[0] = '\0';
    strcpy(new_ident->ident.name, name);
    new_ident->ident.map = IN_MEMORY;
    new_ident->ident.mapped_reg = zero;
    new_ident->next = (struct list_header_t *)idents;
    new_ident->ident.memory_mapping_type = ON_STACK;
    new_ident->ident.addr.offset = -1;
    if(idents != NULL) {
        for(struct ident_list_t *ident = idents; ident != NULL && !ident->border; ident = (struct ident_list_t *)ident->next) {
            if(new_ident->ident.addr.offset > ident->ident.addr.offset) {
                new_ident->ident.addr.offset = ident->ident.addr.offset - 1;
            }
        }
    }
    new_ident->ident.type = UNKNOWN;
    idents = new_ident;
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

void add_function_param(char *name, storage_t mapped_register, offset_t offset_rel_fp, enum ident_type_t type) {
    create_ident(name);
    idents->ident.mapped_reg = mapped_register;
    idents->ident.addr.offset = offset_rel_fp;
    idents->ident.type = type;
    if(mapped_register != zero) {
        idents->ident.map = ON_REGISTER;
    } else {
        idents->ident.map = IN_MEMORY;
    }
}

void allocate_stack(FILE *fd, unsigned int word_count) {
    storage_t temp = r1;
    push_oper(fd, temp);

    li_oper_backend(fd, temp, word_count);
    sub_oper_backend(fd, sp, sp, temp);
    add_oper_backend(fd, temp, sp, temp);
    load_oper_backend(fd, temp, sp, WORD_SIZE);
}

void allocate_stack_label(FILE *fd, char *stack_size_label) {
    push_oper(fd, fp);
    add_oper_backend(fd, fp, sp, zero);

    storage_t temp = r1;
    push_oper(fd, temp);

    li_oper_backend_label(fd, temp, stack_size_label);
    load_oper_backend(fd, temp, temp, 0);
    sub_oper_backend(fd, sp, sp, temp);
    load_oper_backend(fd, temp, fp, -WORD_SIZE);
}

void free_stack(FILE *fd) {
    add_oper_backend(fd, sp, fp, zero);
    pop_oper(fd, fp);
}

enum ident_type_t storage_types[STORAGE_COUNT] = {UNKNOWN};

enum ident_type_t get_storage_type(storage_t storage) {
    if(storage == zero) {
        return UNKNOWN;
    }
    return storage_types[storage-r1];
}

void set_storage_type(storage_t storage, enum ident_type_t type) {
    if(storage != zero) {
        storage_types[storage-r1] = type;
    }
}
