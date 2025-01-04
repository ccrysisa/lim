#ifndef LIM_H
#define LIM_H

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG 0
#define ARRAY_SIZE(xs) (sizeof(xs) / sizeof(xs[0]))
#define LIM_STACK_CAPACITY 1024
#define LIM_PROGRAM_CAPACITY 1024
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

typedef int64_t Word;

typedef enum {
    INST_NOP = 0,
    INST_PUSH,
    INST_PLUS,
    INST_MINUS,
    INST_MULT,
    INST_DIV,
    INST_JMP,
    INST_HALT,
    INST_EQ,
    INST_JNZ,
    INST_JZ,
    INST_DUP,
    INST_PRINT_DEBUG,
} Inst_Type;

const char *inst_type_as_cstr(Inst_Type type);

typedef struct {
    Inst_Type type;
    Word operand;
} Inst;

#define /*Inst*/ MAKE_INST_NOP(/*void*/) \
    {                                    \
        .type = INST_NOP                 \
    }

#define /*Inst*/ MAKE_INST_PUSH(/*Word*/ value) \
    {                                           \
        .type = INST_PUSH, .operand = (value),  \
    }

#define /*Inst*/ MAKE_INST_PLUS(/*void*/) \
    {                                     \
        .type = INST_PLUS                 \
    }

#define /*Inst*/ MAKE_INST_MINUS(/*void*/) \
    {                                      \
        .type = INST_MINUS                 \
    }

#define /*Inst*/ MAKE_INST_MULT(/*void*/) \
    {                                     \
        .type = INST_MULT                 \
    }

#define /*Inst*/ MAKE_INST_DIV(/*void*/) \
    {                                    \
        .type = INST_DIV                 \
    }

#define /*Inst*/ MAKE_INST_JMP(/*Word*/ addr) \
    {                                         \
        .type = INST_JMP, .operand = (addr),  \
    }

#define /*Inst*/ MAKE_INST_HALT(/*void*/) \
    {                                     \
        .type = INST_HALT                 \
    }

#define /*Inst*/ MAKE_INST_EQ(/*void*/) \
    {                                   \
        .type = INST_EQ                 \
    }

#define /*Inst*/ MAKE_INST_JNZ(/*Word*/ addr) \
    {                                         \
        .type = INST_JNZ, .operand = (addr),  \
    }

#define /*Inst*/ MAKE_INST_JZ(/*Word*/ addr) \
    {                                        \
        .type = INST_JZ, .operand = (addr),  \
    }

#define /*Inst*/ MAKE_INST_DUP(/*Word*/ offset) \
    {                                           \
        .type = INST_DUP, .operand = (offset),  \
    }

#define /*Inst*/ MAKE_INST_PRINT_DEBUG(/*void*/) \
    {                                            \
        .type = INST_PRINT_DEBUG                 \
    }

typedef struct {
    size_t count;
    const char *data;
} String_View;

String_View cstr_as_sv(char *str);
String_View sv_trim_left(String_View sv);
String_View sv_trim_right(String_View sv);
String_View sv_trim(String_View sv);
String_View sv_chop_delim(String_View *sv, char delim);
int sv_equal(String_View a, String_View b);
Word sv_to_word(String_View sv);

typedef struct {
    String_View name;
    Word addr;
} Label;

typedef struct {
    Word addr;
    String_View label;
} Unresolved_Jmp;

typedef struct {
    Label labels[LABEL_CAPACITY];
    size_t labels_size;
    Unresolved_Jmp unresolved_jmps[UNRESOLVED_JMPS_CAPACITY];
    size_t unresolved_jmps_size;
} Label_Table;

int label_table_find(const Label_Table *lt, String_View label);
void label_table_push(Label_Table *lt, String_View label, Word addr);
void label_table_push_unresolved_jmp(Label_Table *lt,
                                     Word addr,
                                     String_View label);

/* Lisp Virtual Machine */
typedef struct {
    /* Stack */
    Word stack[LIM_STACK_CAPACITY];
    Word stack_size;

    /* Code */
    Inst program[LIM_PROGRAM_CAPACITY];
    Word program_size;

    /* Labels */
    Label_Table label_table;

    /* State */
    Word ip;
    int halt;
} Lim;

Trap lim_execute_program(Lim *lim);
void lim_load_program_from_memory(Lim *lim, Inst *program, Word program_size);
void lim_load_program_from_file(Lim *lim, const char *file_path);
void lim_save_program_to_file(Lim *lim, const char *file_path);
String_View slurp_file(const char *file_path);
void lim_translate_source(String_View source, Lim *lim);
void lim_dump_stack(FILE *stream, const Lim *lim);
extern Lim lim;

const char *shift_args(int *argc, char ***argv);

#endif
