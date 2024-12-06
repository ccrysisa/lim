#include "lim.h"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: ./limi <input.lim>\n");
        fprintf(stderr, "Error: expect input file\n");
        exit(1);
    }

    const char *input_file_path = argv[1];

    lim_load_program_from_file(&lim, input_file_path);
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
