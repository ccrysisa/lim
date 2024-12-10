#include <stdio.h>
#include "lim.h"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: ./delasm <input.lim>\n");
        fprintf(stderr, "Error: expect input file\n");
        exit(1);
    }

    const char *input_file_path = argv[1];

    lim_load_program_from_file(&lim, input_file_path);
    for (size_t i = 0; i < (size_t) lim.program_size; i++) {
        switch (lim.program[i].type) {
        case INST_NOP:
            printf("nop\n");
            break;
        case INST_PUSH:
            printf("push %ld\n", lim.program[i].operand);
            break;
        case INST_PLUS:
            printf("plus\n");
            break;
        case INST_MINUS:
            printf("minus\n");
            break;
        case INST_MULT:
            printf("mult\n");
            break;
        case INST_DIV:
            printf("div\n");
            break;
        case INST_JMP:
            printf("jmp %ld\n", lim.program[i].operand);
            break;
        case INST_HALT:
            printf("halt\n");
            break;
        case INST_EQ:
            printf("eq\n");
            break;
        case INST_JNZ:
            printf("jnz %ld\n", lim.program[i].operand);
            break;
        case INST_JZ:
            printf("jz %ld\n", lim.program[i].operand);
            break;
        case INST_DUP:
            printf("dup %ld\n", lim.program[i].operand);
            break;
        case INST_PRINT_DEBUG:
            break;
        default:
            fprintf(stderr, "ERROR: unknown instruction `%u%ld`\n",
                    lim.program[i].type, lim.program[i].operand);
            return 1;
        }
    }

    return 0;
}
