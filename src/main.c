#include <assert.h>
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
    assert(program_size > 0 && program_size <= LIM_PROGRAM_CAPACITY);

    memcpy(lim->program, program, sizeof(program[0]) * program_size);
    lim->program_size = program_size;
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

int main()
{
    lim_load_program_from_memory(&lim, program, ARRAY_SIZE(program));
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
