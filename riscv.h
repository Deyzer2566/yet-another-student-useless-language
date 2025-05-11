#pragma once
#include <stdio.h>
#include <stdint.h>
enum register_t {
    zero=0,
    r1,
    r2,
    r3,
    r4,
    ret,
    sp,
    fp,
    lr,
    dummy = 0
};
#define STORAGE_COUNT 4
#define ABI_REGS_COUNT 2

typedef enum register_t storage_t;

typedef uint16_t addr_t;
typedef int16_t offset_t;
typedef uint32_t uword_t;
typedef int32_t sword_t;
#define WORD_SIZE 1

void li_oper_backend(FILE *fd, enum register_t dest, sword_t value);
void li_oper_backend_label(FILE *fd, enum register_t dest, char *label);
void sub_oper_backend(FILE *fd, enum register_t dest, enum register_t src1, enum register_t src2);
void load_oper_backend(FILE *fd, enum register_t dest, enum register_t pointer, offset_t off);
void save_oper_backend(FILE *fd, enum register_t src, enum register_t pointer, offset_t off);
void add_oper_backend(FILE *fd, enum register_t dest, enum register_t src1, enum register_t src2);
void addi_oper_backend(FILE *fd, enum register_t dest, enum register_t src, sword_t imm);
void bne_oper_backend_imm(FILE *fd, enum register_t src1, enum register_t src2, sword_t imm);
void beq_oper_backend_imm(FILE *fd, enum register_t src1, enum register_t src2, sword_t imm);
void bne_oper_backend_label(FILE *fd, enum register_t src1, enum register_t src2, char *label);
void beq_oper_backend_label(FILE *fd, enum register_t src1, enum register_t src2, char *label);
void jal_oper_backend_imm(FILE *fd, enum register_t dest, sword_t imm);
void jal_oper_backend_label(FILE *fd, enum register_t dest, char *label);
void jalr_oper_backend(FILE *fd, enum register_t dest, enum register_t src, offset_t off);
void push_oper(FILE *fd, enum register_t src);
void pop_oper(FILE *fd, enum register_t dest);
void ebreak_oper_backend(FILE *fd);
