#include "lim.h"

int main(int argc, char *argv[])
{
    const char *program = shift_args(&argc, &argv);
    const char *input_file_path = NULL;

    while (argc > 0) {
        const char *flag = shift_args(&argc, &argv);

        if (!strcmp(flag, "-i")) {
            if (argc == 0) {
                fprintf(stderr, "Error: expect input file\n");
                return 1;
            }
            input_file_path = shift_args(&argc, &argv);
        } else if (!strcmp(flag, "-h")) {
            fprintf(stdout, "Usage: %s -i <input.lim> [-h]\n", program);
            return 0;
        } else {
            fprintf(stderr, "Error: unknown flag `%s`\n", flag);
            return 1;
        }
    }

    if (input_file_path == NULL) {
        fprintf(stderr, "Error: input file is not provided\n");
        return 1;
    }

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
