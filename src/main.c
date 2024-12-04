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
} Inst_Type;

typedef struct {
    Inst_Type type;
    Word operand;
} Inst;

#define /*Inst*/ MAKE_INST_PUSH(/*Word*/ value) \
    {                                           \
        .type = INST_PUSH, .operand = value,    \
    }

#define /*Inst*/ MAKE_INST_PLUS(/*void*/) \
    {                                     \
        .type = INST_PLUS                 \
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
    MAKE_INST_PUSH(69),
    MAKE_INST_PUSH(22),
    MAKE_INST_PUSH(43),
    MAKE_INST_PLUS(),
};

int main()
{
    lim_dump(stdout, &lim);
    for (size_t i = 0; i < ARRAY_SIZE(program); i++) {
        Trap trap = lim_execute_inst(&lim, program[i]);
        if (trap != TRAP_OK) {
            fprintf(stderr, "Trap activated: %s\n", trap_as_cstr(trap));
            lim_dump(stderr, &lim);
            exit(1);
        }
    }
    lim_dump(stdout, &lim);
    return 0;
}