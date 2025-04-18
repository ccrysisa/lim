#ifndef __LIM_H
#define __LIM_H

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARRAY_SIZE(xs) (sizeof(xs) / sizeof(xs[0]))
#define LIM_STACK_CAPACITY 1024
#define LIM_PROGRAM_CAPACITY 1024
#define LIM_NATIVES_CAPACITY 1024
#define LABEL_CAPACITY 1024
#define UNRESOLVED_JMPS_CAPACITY 1024

typedef enum {
    TRAP_OK = 0,
    TRAP_STACK_OVERFLOW,
    TRAP_STACK_UNDERFLOW,
    TRAP_DIV_BY_ZERO,
    TRAP_ILLEGAL_INST,
    TRAP_ILLEGAL_INST_ACCESS,
    TRAP_ILLEGAL_OPERAND,
} Trap;

const char *trap_as_cstr(Trap trap);

typedef uint64_t Inst_Addr;

typedef union Word {
    uint64_t as_u64;  // used for address
    int64_t as_i64;   // used for integer (and bit sequence)
    double as_f64;    // used for float point number
    void *as_ptr;     // used for pointer and object
} Word;

static_assert(sizeof(Word) == 8,
              "LIM's word is expected to be 8 bytes aka 64 bites");

typedef enum {
    INST_NOP = 0,
    INST_PUSH,
    INST_POP,
    INST_DUP,
    INST_PLUS,
    INST_MINUS,
    INST_MULT,
    INST_DIV,
    INST_FPLUS,
    INST_FMINUS,
    INST_FMULT,
    INST_FDIV,
    INST_GT,
    INST_LT,
    INST_GE,
    INST_LE,
    INST_EQ,
    INST_JMP,
    INST_JNZ,
    INST_JZ,
    INST_SWAP,
    INST_CALL,
    INST_RET,
    INST_NATIVE,
    INST_HALT,
    INST_PRINT_DEBUG,
    INST_NUM,
} Inst_Type;

const char *inst_type_as_cstr(Inst_Type type);
bool inst_has_operand(Inst_Type type);

typedef struct {
    Inst_Type type;
    Word operand;
} Inst;

#define /*Inst*/ MAKE_INST_NOP(/*void*/) \
    (Inst)                               \
    {                                    \
        .type = INST_NOP                 \
    }

#define /*Inst*/ MAKE_INST_PUSH(/*Word*/ value) \
    (Inst)                                      \
    {                                           \
        .type = INST_PUSH, .operand = (value),  \
    }

#define /*Inst*/ MAKE_INST_POP(/*void*/) \
    (Inst)                               \
    {                                    \
        .type = INST_POP                 \
    }

#define /*Inst*/ MAKE_INST_DUP(/*Word*/ offset) \
    (Inst)                                      \
    {                                           \
        .type = INST_DUP, .operand = (offset),  \
    }

#define /*Inst*/ MAKE_INST_PLUS(/*void*/) \
    (Inst)                                \
    {                                     \
        .type = INST_PLUS                 \
    }

#define /*Inst*/ MAKE_INST_MINUS(/*void*/) \
    (Inst)                                 \
    {                                      \
        .type = INST_MINUS                 \
    }

#define /*Inst*/ MAKE_INST_MULT(/*void*/) \
    (Inst)                                \
    {                                     \
        .type = INST_MULT                 \
    }

#define /*Inst*/ MAKE_INST_DIV(/*void*/) \
    (Inst)                               \
    {                                    \
        .type = INST_DIV                 \
    }

#define /*Inst*/ MAKE_INST_FPLUS(/*void*/) \
    (Inst)                                 \
    {                                      \
        .type = INST_FPLUS                 \
    }

#define /*Inst*/ MAKE_INST_FMINUS(/*void*/) \
    (Inst)                                  \
    {                                       \
        .type = INST_FMINUS                 \
    }

#define /*Inst*/ MAKE_INST_FMULT(/*void*/) \
    (Inst)                                 \
    {                                      \
        .type = INST_FMULT                 \
    }

#define /*Inst*/ MAKE_INST_FDIV(/*void*/) \
    (Inst)                                \
    {                                     \
        .type = INST_FDIV                 \
    }

#define /*Inst*/ MAKE_INST_GT(/*void*/) \
    (Inst)                              \
    {                                   \
        .type = INST_GT                 \
    }

#define /*Inst*/ MAKE_INST_LT(/*void*/) \
    (Inst)                              \
    {                                   \
        .type = INST_LT                 \
    }

#define /*Inst*/ MAKE_INST_GE(/*void*/) \
    (Inst)                              \
    {                                   \
        .type = INST_GE                 \
    }

#define /*Inst*/ MAKE_INST_LE(/*void*/) \
    (Inst)                              \
    {                                   \
        .type = INST_LE                 \
    }

#define /*Inst*/ MAKE_INST_EQ(/*void*/) \
    (Inst)                              \
    {                                   \
        .type = INST_EQ                 \
    }

#define /*Inst*/ MAKE_INST_JMP(/*Word*/ addr) \
    (Inst)                                    \
    {                                         \
        .type = INST_JMP, .operand = (addr),  \
    }

#define /*Inst*/ MAKE_INST_JMP_WITHOUT_OPERAND(/*void*/) \
    (Inst)                                               \
    {                                                    \
        .type = INST_JMP                                 \
    }

#define /*Inst*/ MAKE_INST_JNZ(/*Word*/ addr) \
    (Inst)                                    \
    {                                         \
        .type = INST_JNZ, .operand = (addr),  \
    }

#define /*Inst*/ MAKE_INST_JNZ_WITHOUT_OPERAND(/*void*/) \
    (Inst)                                               \
    {                                                    \
        .type = INST_JNZ                                 \
    }

#define /*Inst*/ MAKE_INST_JZ(/*Word*/ addr) \
    (Inst)                                   \
    {                                        \
        .type = INST_JZ, .operand = (addr),  \
    }

#define /*Inst*/ MAKE_INST_JZ_WITHOUT_OPERAND(/*void*/) \
    (Inst)                                              \
    {                                                   \
        .type = INST_JZ                                 \
    }

#define /*Inst*/ MAKE_INST_SWAP(/*Word*/ offset) \
    (Inst)                                       \
    {                                            \
        .type = INST_SWAP, .operand = (offset),  \
    }

#define /*Inst*/ MAKE_INST_CALL_WITHOUT_OPERAND(/*void*/) \
    (Inst)                                                \
    {                                                     \
        .type = INST_CALL                                 \
    }

#define /*Inst*/ MAKE_INST_RET(/*void*/) \
    (Inst)                               \
    {                                    \
        .type = INST_RET                 \
    }

#define /*Inst*/ MAKE_INST_NATIVE(/*Word*/ number) \
    (Inst)                                         \
    {                                              \
        .type = INST_NATIVE, .operand = number,    \
    }

#define /*Inst*/ MAKE_INST_HALT(/*void*/) \
    (Inst)                                \
    {                                     \
        .type = INST_HALT                 \
    }

#define /*Inst*/ MAKE_INST_PRINT_DEBUG(/*void*/) \
    (Inst)                                       \
    {                                            \
        .type = INST_PRINT_DEBUG                 \
    }

typedef struct {
    size_t count;
    const char *data;
} String_View;

String_View cstr_as_sv(const char *str);
String_View sv_trim_left(String_View sv);
String_View sv_trim_right(String_View sv);
String_View sv_trim(String_View sv);
String_View sv_chop_delim(String_View *sv, char delim);
String_View sv_delim(String_View sv, char delim);
int sv_equal(String_View a, String_View b);
Word sv_to_word(String_View sv);

typedef struct {
    String_View name;  // the name of the label
    Inst_Addr addr;    // corresponding address of the label
} Label;

typedef struct {
    Inst_Addr addr;     // the address which exists unresolved jump label
    String_View label;  // unresolved jump label in the instruction
} Unresolved_Jmp;

typedef struct {
    /* Label declarations */
    Label labels[LABEL_CAPACITY];
    size_t labels_size;

    /* Unresolved jump instructions */
    Unresolved_Jmp unresolved_jmps[UNRESOLVED_JMPS_CAPACITY];
    size_t unresolved_jmps_size;
} Lasm;

int label_table_find(const Lasm *lasm, String_View label);
void label_table_push(Lasm *lasm, String_View label, Inst_Addr addr);
void label_table_push_unresolved_jmp(Lasm *lasm,
                                     Inst_Addr addr,
                                     String_View label);
extern Lasm lasm;

/* Lisp Virtual Machine */
typedef struct Lim Lim;

typedef Trap (*Lim_Native_Func)(Lim *);

struct Lim {
    /* Stack */
    Word stack[LIM_STACK_CAPACITY];
    uint64_t stack_size;

    /* Code */
    Inst program[LIM_PROGRAM_CAPACITY];
    uint64_t program_size;

    /* Natives */
    Lim_Native_Func natives[LIM_NATIVES_CAPACITY];
    uint64_t natives_size;

    /* State */
    Inst_Addr ip;
    bool halt;
};

Trap lim_execute_inst(Lim *lim);
Trap lim_execute_program(Lim *lim);
void lim_load_program_from_memory(Lim *lim,
                                  Inst *program,
                                  uint64_t program_size);
void lim_load_program_from_file(Lim *lim, const char *file_path);
void lim_save_program_to_file(Lim *lim, const char *file_path);
String_View slurp_file(const char *file_path);
Word number_literal_as_word(String_View sv);
void lim_translate_source(String_View source, Lim *lim, Lasm *lasm);
void lim_dump_stack(FILE *stream, const Lim *lim);

void lim_attach_natives(Lim *lim);
void lim_push_native_func(Lim *lim, Lim_Native_Func func);

extern Lim lim;

const char *shift_args(int *argc, char ***argv);

#endif
