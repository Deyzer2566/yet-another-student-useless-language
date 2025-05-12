// medium abstractions layer
// реализует операции работы с идентификаторами

#include "riscv.h"
#include <stdbool.h>

enum ident_type_t {
    UNKNOWN,
    INTEGER,
    REAL,
    POINTER
};

int load_ident(FILE *fd, storage_t dest, char* name);
void update_ident(FILE *fd, storage_t storage, char *name);
void create_ident(char *name);
storage_t get_storage(FILE *fd);
void free_storage(FILE *fd, storage_t storage);
void allocate_storage(storage_t storage);
void get_specific_storage(FILE *fd, storage_t storage);
void new_space();
void pop_space();
size_t size_space();
size_t count_idents();
void add_function_param(char *name, storage_t mapped_register, offset_t offset_rel_fp, enum ident_type_t type);
void allocate_stack(FILE *fd, unsigned int word_count);
void allocate_stack_label(FILE *fd, char *stack_size_label);
void free_stack(FILE *fd);
enum ident_type_t get_storage_type(storage_t storage);
void set_storage_type(storage_t storage, enum ident_type_t type);
