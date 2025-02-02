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
    Trap trap = lim_execute_program(&lim);
    lim_dump_stack(stdout, &lim);

    if (trap != TRAP_OK) {
        fprintf(stderr, "Error: %s\n", trap_as_cstr(trap));
        return 1;
        ;
    }

    return 0;
}
