// medium abstractions layer
// реализует операции работы с идентификаторами

#include "riscv.h"
#include <stdbool.h>

int load_ident(FILE *fd, storage_t dest, char* name, bool create_if_not_exists);
storage_t get_storage(FILE *fd);
void free_storage(FILE *fd, storage_t storage);
void allocate_storage(storage_t storage);
void get_specific_storage(FILE *fd, storage_t storage);
void new_space();
void pop_space();
size_t size_space();
size_t count_idents();
void allocate_stack(FILE *fd, unsigned int word_count);
void allocate_stack_label(FILE *fd, char *stack_size_label);
void free_stack(FILE *fd);
