#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG 1
#define ARRAY_SIZE(xs) (sizeof(xs) / sizeof(xs[0]))
#define LIM_STACK_CAPACITY 1024
#define LIM_PROGRAM_CAPACITY 1024

typedef enum {
    TRAP_OK = 0,
    TRAP_STACK_OVERFLOW,
    TRAP_STACK_UNDERFLOW,
    TRAP_DIV_BY_ZERO,
    TRAP_ILLEGAL_INST,
    TRAP_ILLEGAL_INST_ACCESS,
    TRAP_ILLEGAL_OPERAND,
} Trap;

const char *trap_as_cstr(Trap trap)
{
    switch (trap) {
    case TRAP_OK:
        return "TRAP_OK";
    case TRAP_STACK_OVERFLOW:
        return "TRAP_STACK_OVERFLOW";
    case TRAP_STACK_UNDERFLOW:
        return "TRAP_STACK_UNDERFLOW";
    case TRAP_DIV_BY_ZERO:
        return "TRAP_DIV_BY_ZERO";
    case TRAP_ILLEGAL_INST:
        return "TRAP_ILLEGAL_INST";
    case TRAP_ILLEGAL_INST_ACCESS:
        return "TRAP_ILLEGAL_INST_ACCESS";
    case TRAP_ILLEGAL_OPERAND:
        return "TRAP_ILLEGAL_OPERAND";
    default:
        assert(0 && "trap_as_cstr: unreachable");
    }
}

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

const char *inst_type_as_cstr(Inst_Type type)
{
    switch (type) {
    case INST_NOP:
        return "INST_NOP";
    case INST_PUSH:
        return "INST_PUSH";
    case INST_PLUS:
        return "INST_PLUS";
    case INST_MINUS:
        return "INST_MINUS";
    case INST_MULT:
        return "INST_MULT";
    case INST_DIV:
        return "INST_DIV";
    case INST_JMP:
        return "INST_JMP";
    case INST_HALT:
        return "INST_HALT";
    case INST_EQ:
        return "INST_EQ";
    case INST_JNZ:
        return "INST_JNZ";
    case INST_JZ:
        return "INST_JZ";
    case INST_DUP:
        return "INST_DUP";
    case INST_PRINT_DEBUG:
        return "INST_PRINT_DEBUG";
    default:
        assert(0 && "inst_type_as_cstr: unreachable");
    }
}

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

/* Lisp Virtual Machine */
typedef struct {
    /* Stack */
    Word stack[LIM_STACK_CAPACITY];
    Word stack_size;
    /* Code */
    Inst program[LIM_PROGRAM_CAPACITY];
    Word program_size;
    /* State */
    Word ip;
    int halt;
} Lim;

typedef struct {
    size_t count;
    char *data;
} String_View;

String_View cstr_as_sv(char *str)
{
    return (String_View){
        .count = strlen(str),
        .data = str,
    };
}

String_View sv_trim_left(String_View sv)
{
    assert(0 && "ERROR: sv_trim_left is not implemented");
}

String_View sv_trim_right(String_View sv)
{
    assert(0 && "ERROR: sv_trim_right is not implemented");
}

String_View sv_chop_delim(String_View sv, char c)
{
    assert(0 && "ERROR: sv_chop_delim is not implemented");
}

int sv_equal(String_View a, String_View b)
{
    assert(0 && "ERROR: sv_equal is not implemented");
}

int sv_to_int(String_View sv)
{
    assert(0 && "ERROR: sv_equal is not implemented");
}

Trap lim_execute_inst(Lim *lim)
{
    if (lim->ip < 0 || lim->ip >= lim->program_size) {
        return TRAP_ILLEGAL_INST_ACCESS;
    }

    Inst inst = lim->program[lim->ip];
#if DEBUG
    printf("%s\n", inst_type_as_cstr(inst.type));
#endif
    switch (inst.type) {
    case INST_NOP:
        lim->ip++;
        break;

    case INST_PUSH:
        if (lim->stack_size >= LIM_STACK_CAPACITY) {
            return TRAP_STACK_OVERFLOW;
        }
        lim->stack[lim->stack_size++] = inst.operand;
        lim->ip++;
        break;

    case INST_PLUS:
        if (lim->stack_size < 2) {
            return TRAP_STACK_UNDERFLOW;
        }
        lim->stack[lim->stack_size - 2] += lim->stack[lim->stack_size - 1];
        lim->stack_size--;
        lim->ip++;
        break;

    case INST_MINUS:
        if (lim->stack_size < 2) {
            return TRAP_STACK_UNDERFLOW;
        }
        lim->stack[lim->stack_size - 2] -= lim->stack[lim->stack_size - 1];
        lim->stack_size--;
        lim->ip++;
        break;

    case INST_MULT:
        if (lim->stack_size < 2) {
            return TRAP_STACK_UNDERFLOW;
        }
        lim->stack[lim->stack_size - 2] *= lim->stack[lim->stack_size - 1];
        lim->stack_size--;
        lim->ip++;
        break;

    case INST_DIV:
        if (lim->stack_size < 2) {
            return TRAP_STACK_UNDERFLOW;
        }
        if (lim->stack[lim->stack_size - 1] == 0) {
            return TRAP_DIV_BY_ZERO;
        }
        lim->stack[lim->stack_size - 2] /= lim->stack[lim->stack_size - 1];
        lim->stack_size--;
        lim->ip++;
        break;

    case INST_JMP:
        // In this instruction, its address counld be illegal, since during next
        // instruction execution this function would return trap/error
        // TRAP_ILLEGAL_INST_ACCESS.
        lim->ip = inst.operand;
        break;

    case INST_HALT:
        lim->halt = 1;
        break;

    case INST_EQ:
        if (lim->stack_size < 2) {
            return TRAP_STACK_UNDERFLOW;
        }
        lim->stack[lim->stack_size - 2] =
            lim->stack[lim->stack_size - 2] == lim->stack[lim->stack_size - 1];
        lim->stack_size--;
        lim->ip++;
        break;

    case INST_JNZ:
        if (lim->stack_size < 1) {
            return TRAP_STACK_UNDERFLOW;
        }
        // In this instruction, its address counld be illegal, since during next
        // instruction execution this function would return trap/error
        // TRAP_ILLEGAL_INST_ACCESS.
        if (lim->stack[--lim->stack_size]) {
            lim->ip = inst.operand;
        } else {
            lim->ip++;
        }
        break;

    case INST_JZ:
        if (lim->stack_size < 1) {
            return TRAP_STACK_UNDERFLOW;
        }
        // In this instruction, its address counld be illegal, since during next
        // instruction execution this function would return trap/error
        // TRAP_ILLEGAL_INST_ACCESS.
        if (!lim->stack[--lim->stack_size]) {
            lim->ip = inst.operand;
        } else {
            lim->ip++;
        }
        break;

    case INST_DUP:
        // This instruction's operand is offset about the top element of stack.
        // e.g. operand equal 0 means duplicate and push the current top element
        // in the stack.
        if (lim->stack_size >= LIM_STACK_CAPACITY) {
            return TRAP_STACK_OVERFLOW;
        }
        if (inst.operand < 0) {
            return TRAP_ILLEGAL_OPERAND;
        }
        if (lim->stack_size - inst.operand <= 0) {
            return TRAP_STACK_UNDERFLOW;
        }

        lim->stack[lim->stack_size] =
            lim->stack[lim->stack_size - 1 - inst.operand];
        lim->stack_size++;
        lim->ip++;
        break;

    case INST_PRINT_DEBUG:
        if (lim->stack_size < 1) {
            return TRAP_STACK_UNDERFLOW;
        }
        printf("%ld\n", lim->stack[--lim->stack_size]);
        lim->ip++;
        break;

    default:
        return TRAP_ILLEGAL_INST;
    }
    return TRAP_OK;
}

void lim_load_program_from_memory(Lim *lim, Inst *program, Word program_size)
{
    assert(program_size <= LIM_PROGRAM_CAPACITY);

    memcpy(lim->program, program, sizeof(program[0]) * program_size);
    lim->program_size = program_size;
}

void lim_load_program_from_file(Lim *lim, const char *file_path)
{
    FILE *f = fopen(file_path, "rb");
    if (f == NULL) {
        fprintf(stderr, "ERROR: Counld not open file `%s`: %s\n", file_path,
                strerror(errno));
        exit(1);
    }

    if (fseek(f, 0, SEEK_END) < 0) {
        fprintf(stderr, "ERROR: Counld not read file `%s`: %s\n", file_path,
                strerror(errno));
        exit(1);
    }

    long m = ftell(f);
    if (m < 0) {
        fprintf(stderr, "ERROR: Counld not read file `%s`: %s\n", file_path,
                strerror(errno));
        exit(1);
    }

    assert(m % sizeof(lim->program[0]) == 0);
    assert((size_t) m <= LIM_PROGRAM_CAPACITY * sizeof(lim->program[0]));

    if (fseek(f, 0, SEEK_SET) < 0) {
        fprintf(stderr, "ERROR: Counld not read file `%s`: %s\n", file_path,
                strerror(errno));
        exit(1);
    }

    lim->program_size = fread(lim->program, sizeof(lim->program[0]),
                              m / sizeof(lim->program[0]), f);

    if (ferror(f)) {
        fprintf(stderr, "ERROR: Counld not write file `%s`: %s\n", file_path,
                strerror(errno));
        exit(1);
    }

    fclose(f);
}

void lim_save_program_to_file(Inst *program,
                              size_t program_size,
                              const char *file_path)
{
    FILE *f = fopen(file_path, "wb");
    if (f == NULL) {
        fprintf(stderr, "ERROR: Counld not open file `%s`: %s\n", file_path,
                strerror(errno));
        exit(1);
    }

    fwrite(program, sizeof(program[0]), program_size, f);

    if (ferror(f)) {
        fprintf(stderr, "ERROR: Counld not write file `%s`: %s\n", file_path,
                strerror(errno));
        exit(1);
    }

    fclose(f);
}

Inst lim_translate_line(String_View line)
{
    line = sv_trim_left(line);
    String_View inst_name = sv_chop_delim(line, ' ');

    if (sv_equal(inst_name, cstr_as_sv("push"))) {
        line = sv_trim_left(line);
        int operand = sv_to_int(line);
        return (Inst) MAKE_INST_PUSH(operand);
    } else {
        fprintf(stderr, "ERROR: `%.*s` is not a number\n",
                (int) inst_name.count, inst_name.data);
    }

    return (Inst) MAKE_INST_NOP();
}

size_t lim_translate_source(char *source,
                            size_t source_size,
                            Inst *program,
                            size_t program_size)
{
    while (source_size > 0) {
        char *end = memchr(source, '\n', source_size);
        size_t n = end != NULL ? (size_t) (end - source) : source_size;

        printf("#%.*s#\n", (int) n, source);

        source = end;
        source_size -= n;

        if (source != NULL) {
            source += 1;
            source_size -= 1;
        }
    }

    (void) program;
    (void) program_size;

    return 0;
}

void lim_dump_stack(FILE *stream, const Lim *lim)
{
    fprintf(stream, "Stack:\n");
    if (lim->stack_size > 0) {
        for (Word i = 0; i < lim->stack_size; i++) {
            fprintf(stream, "  %ld\n", lim->stack[i]);
        }
    } else {
        fprintf(stream, "  [empty]\n");
    }
}

Lim lim = {0};

// calculate the Fibonacci numbers
Inst program[] = {
    MAKE_INST_PUSH(0),       MAKE_INST_PUSH(1), MAKE_INST_DUP(1),
    MAKE_INST_DUP(1),        MAKE_INST_PLUS(),  MAKE_INST_DUP(0),
    MAKE_INST_PUSH(2584),    MAKE_INST_EQ(),    MAKE_INST_JZ(2),
    MAKE_INST_PRINT_DEBUG(), MAKE_INST_HALT(),
};

char *source_code =
    "push 0\n"
    "push 1\n"
    "dup 1\n"
    "dup 1\n"
    "plus\n"
    "dup 0\n"
    "push 2854\n"
    "eq\n"
    "jz 2\n"
    "halt\n";

int main()
{
    lim_translate_source(source_code, strlen(source_code), NULL, 0);
    return 0;
}

int main2()
{
    // lim_load_program_from_memory(&lim, program, ARRAY_SIZE(program));
    // lim_save_program_to_file(lim.program, lim.program_size,
    // "./tests/fib.lim");
    lim_load_program_from_file(&lim, "./tests/fib.lim");
    lim_dump_stack(stdout, &lim);
    while (!lim.halt) {
        Trap trap = lim_execute_inst(&lim);
#if DEBUG
        lim_dump_stack(stdout, &lim);
#endif
        if (trap != TRAP_OK) {
            fprintf(stderr, "Trap activated: %s\n", trap_as_cstr(trap));
            lim_dump_stack(stderr, &lim);
            exit(1);
        }
    }
    lim_dump_stack(stdout, &lim);
    return 0;
}
