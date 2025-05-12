#include "riscv.h"
#include <stdlib.h>

#define REGISTER_PREFIX "x"
void li_oper_backend(FILE *fd, enum register_t dest, sword_t value) {
    fprintf(fd, "li "REGISTER_PREFIX"%d, %d\n", dest, value);
}
void li_oper_backend_label(FILE *fd, enum register_t dest, char *label) {
    fprintf(fd, "li "REGISTER_PREFIX"%d, %s\n", dest, label);
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
void bne_oper_backend_imm(FILE *fd, enum register_t src1, enum register_t src2, sword_t imm) {
    fprintf(fd, "bne "REGISTER_PREFIX"%d, "REGISTER_PREFIX"%d, %d\n", src1, src2, imm);
}
void beq_oper_backend_imm(FILE *fd, enum register_t src1, enum register_t src2, sword_t imm) {
    fprintf(fd, "beq "REGISTER_PREFIX"%d, "REGISTER_PREFIX"%d, %d\n", src1, src2, imm);
}
void bne_oper_backend_label(FILE *fd, enum register_t src1, enum register_t src2, char *label) {
    fprintf(fd, "bne "REGISTER_PREFIX"%d, "REGISTER_PREFIX"%d, %s\n", src1, src2, label);
}
void beq_oper_backend_label(FILE *fd, enum register_t src1, enum register_t src2, char *label) {
    fprintf(fd, "beq "REGISTER_PREFIX"%d, "REGISTER_PREFIX"%d, %s\n", src1, src2, label);
}
void jal_oper_backend_imm(FILE *fd, enum register_t dest, sword_t imm) {
    fprintf(fd, "jal "REGISTER_PREFIX"%d, %d\n", dest, imm);
}
void jal_oper_backend_label(FILE *fd, enum register_t dest, char *label) {
    fprintf(fd, "jal "REGISTER_PREFIX"%d, %s\n", dest, label);
}
void jalr_oper_backend(FILE *fd, enum register_t dest, enum register_t src, offset_t off) {
    fprintf(fd, "jalr "REGISTER_PREFIX"%d, "REGISTER_PREFIX"%d, %d\n", dest, src, off);
}
void push_oper(FILE *fd, enum register_t src) {
    addi_oper_backend(fd, sp, sp, (sword_t)-WORD_SIZE);
    save_oper_backend(fd, src, sp, 0);
}
void pop_oper(FILE *fd, enum register_t dest) {
    load_oper_backend(fd, dest, sp, 0);
    addi_oper_backend(fd, sp, sp, WORD_SIZE);
}
void ebreak_oper_backend(FILE *fd) {
    fprintf(fd, "ebreak\n");
}
void seq_oper_backend(FILE *fd, enum register_t dest, enum register_t src1, enum register_t src2) {
    fprintf(fd, "seq "REGISTER_PREFIX"%d, "REGISTER_PREFIX"%d, "REGISTER_PREFIX"%d\n", dest, src1, src2);
}
void mul_oper_backend(FILE *fd, enum register_t dest, enum register_t src1, enum register_t src2) {
    fprintf(fd, "mul "REGISTER_PREFIX"%d, "REGISTER_PREFIX"%d, "REGISTER_PREFIX"%d\n", dest, src1, src2);
}
