#include "riscv.h"
#include <stdlib.h>

#define REGISTER_PREFIX "x"
void li_oper_backend(FILE *fd, enum register_t dest, sword_t value) {
    fprintf(fd, "li "REGISTER_PREFIX"%d, %d\n", dest, value);
}
void sub_oper_backend(FILE *fd, enum register_t dest, enum register_t src1, enum register_t src2) {
    fprintf(fd, "sub "REGISTER_PREFIX"%d, "REGISTER_PREFIX"%d, "REGISTER_PREFIX"%d\n", dest, src1, src2);
}
void load_oper_backend(FILE *fd, enum register_t dest, enum register_t pointer, offset_t off) {
    fprintf(fd, "lw "REGISTER_PREFIX"%d, "REGISTER_PREFIX"%d, %d\n", dest, pointer, off);
}
void save_oper_backend(FILE *fd, enum register_t src, enum register_t pointer, offset_t off) {
    fprintf(fd, "sw "REGISTER_PREFIX"%d, %d, "REGISTER_PREFIX"%d\n", pointer, off, src);
}
void add_oper_backend(FILE *fd, enum register_t dest, enum register_t src1, enum register_t src2) {
    fprintf(fd, "add "REGISTER_PREFIX"%d, "REGISTER_PREFIX"%d, "REGISTER_PREFIX"%d\n", dest, src1, src2);
}
void addi_oper_backend(FILE *fd, enum register_t dest, enum register_t src, sword_t imm) {
    // if(abs(imm) <= 2<<11)
        fprintf(fd, "addi "REGISTER_PREFIX"%d, "REGISTER_PREFIX"%d, %d\n", dest, src, imm);
    // else {
    //     storage_t temp = get_storage(fd);
    //     li_oper_backend(fd, temp, imm);
    //     add_oper_backend(fd, dest, src, temp);
    //     free_storage(fd, temp);
    // }
}