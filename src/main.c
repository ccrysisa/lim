#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define ARRAY_SIZE(xs) (sizeof(xs) / sizeof(xs[0]))
#define LIM_STACK_CAPACITY 1024

typedef enum {
    TRAP_OK = 0,
    TRAP_STACK_OVERFLOW,
    TRAP_STACK_UNDERFLOW,
    TRAP_DIV_BY_ZERO,
    TRAP_ILLEGAL_INST,
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
    default:
        assert(0 && "trap_as_cstr: unreachable");
    }
}

typedef int64_t Word;

typedef struct {
    Word stack[LIM_STACK_CAPACITY];
    size_t stack_size;
} Lim;

typedef enum {
    INST_PUSH,
    INST_PLUS,
    INST_MINUS,
    INST_MULT,
    INST_DIV,
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

Trap lim_execute_inst(Lim *lim, Inst inst)
{
    switch (inst.type) {
    case INST_PUSH:
        if (lim->stack_size >= LIM_STACK_CAPACITY) {
            return TRAP_STACK_OVERFLOW;
        }
        lim->stack[lim->stack_size++] = inst.operand;
        break;

    case INST_PLUS:
        if (lim->stack_size < 2) {
            return TRAP_STACK_UNDERFLOW;
        }
        lim->stack[lim->stack_size - 2] += lim->stack[lim->stack_size - 1];
        lim->stack_size--;
        break;

    case INST_MINUS:
        if (lim->stack_size < 2) {
            return TRAP_STACK_UNDERFLOW;
        }
        lim->stack[lim->stack_size - 2] -= lim->stack[lim->stack_size - 1];
        lim->stack_size--;
        break;

    case INST_MULT:
        if (lim->stack_size < 2) {
            return TRAP_STACK_UNDERFLOW;
        }
        lim->stack[lim->stack_size - 2] *= lim->stack[lim->stack_size - 1];
        lim->stack_size--;
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
        break;

    default:
        return TRAP_ILLEGAL_INST;
    }
    return TRAP_OK;
}

void lim_dump(FILE *stream, Lim *lim)
{
    fprintf(stream, "Stack:\n");
    if (lim->stack_size > 0) {
        for (size_t i = 0; i < lim->stack_size; i++) {
            fprintf(stream, "  %ld\n", lim->stack[i]);
        }
    } else {
        fprintf(stream, "  [empty]\n");
    }
}

Lim lim = {0};

Inst program[] = {
    MAKE_INST_PUSH(69), MAKE_INST_PUSH(22), MAKE_INST_PUSH(43),
    MAKE_INST_PLUS(),   MAKE_INST_MINUS(),  MAKE_INST_PUSH(21),
    MAKE_INST_PUSH(3),  MAKE_INST_DIV(),    MAKE_INST_MULT(),
};

int main()
{
    lim_dump(stdout, &lim);
    for (size_t i = 0; i < ARRAY_SIZE(program); i++) {
        Trap trap = lim_execute_inst(&lim, program[i]);
        /* DEBUG */ printf("%s\n", inst_type_as_cstr(program[i].type));
        /* DEBUG */ lim_dump(stdout, &lim);
        if (trap != TRAP_OK) {
            fprintf(stderr, "Trap activated: %s\n", trap_as_cstr(trap));
            lim_dump(stderr, &lim);
            exit(1);
        }
    }
    lim_dump(stdout, &lim);
    return 0;
}