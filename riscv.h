#pragma once
#include <stdio.h>
#include <stdint.h>
enum register_t {
    zero=0,
    r1,
    r2,
    r3,
    r4,
    sp,
    lr,
    dummy = 0
};
#define STORAGE_COUNT 4

typedef enum register_t storage_t;

typedef uint16_t addr_t;
typedef int16_t offset_t;
typedef uint32_t uword_t;
typedef int32_t sword_t;
#define WORD_SIZE sizeof(uword_t)

void li_oper_backend(FILE *fd, enum register_t dest, sword_t value);
void sub_oper_backend(FILE *fd, enum register_t dest, enum register_t src1, enum register_t src2);
void load_oper_backend(FILE *fd, enum register_t dest, enum register_t pointer, offset_t off);
void save_oper_backend(FILE *fd, enum register_t src, enum register_t pointer, offset_t off);
void add_oper_backend(FILE *fd, enum register_t dest, enum register_t src1, enum register_t src2);
void addi_oper_backend(FILE *fd, enum register_t dest, enum register_t src, sword_t imm);
storage_t get_storage(FILE *fd);
void free_storage(FILE *fd, storage_t storage);
