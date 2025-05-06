// medium abstractions layer
// реализует операции работы с идентификаторами
#include "riscv.h"

int load_ident(FILE *fd, storage_t dest, char* name, int create_if_not_exists);
