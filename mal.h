// medium abstractions layer
// реализует операции работы с идентификаторами

#include "riscv.h"
#include <stdbool.h>

int load_ident(FILE *fd, storage_t dest, char* name, bool create_if_not_exists);
storage_t get_storage(FILE *fd);
void free_storage(FILE *fd, storage_t storage);
void allocate_storage(storage_t storage);
