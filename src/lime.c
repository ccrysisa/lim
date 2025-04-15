#include "lim.h"

int main(int argc, char *argv[])
{
    const char *program = shift_args(&argc, &argv);
    const char *input_file_path = NULL;
    bool debug = false;

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
        } else if (!strcmp(flag, "-d")) {
            debug = true;
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
    lim_attach_natives(&lim);

    Trap trap = TRAP_OK;
    if (debug) {
        // In debug mode, stack as state and instruction as event to construct a
        // FSM
        lim_dump_stack(stdout, &lim);
        while (!lim.halt) {
            const Inst *const inst = &lim.program[lim.ip];
            printf("> %s", inst_type_as_cstr(inst->type));
            if (inst_has_operand(inst->type)) {
                printf(" %lu\n", inst->operand.as_u64);
            }

            trap = lim_execute_inst(&lim);
            lim_dump_stack(stdout, &lim);
            if (trap != TRAP_OK) {
                break;
            }

            getchar();
        }
    } else {
        trap = lim_execute_program(&lim);
    }

    if (trap != TRAP_OK) {
        fprintf(stderr, "Error: %s\n", trap_as_cstr(trap));
        return 1;
        ;
    }

    return 0;
}
