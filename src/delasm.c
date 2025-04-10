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
        const Inst inst = lim.program[i];
        printf("%s", inst_type_as_cstr(inst.type));
        if (inst_has_operand(inst.type)) {
            printf(" %ld", inst.operand.as_i64);
        }
        printf("\n");
    }

    return 0;
}
