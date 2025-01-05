#include "lim.h"

int main(int argc, char *argv[])
{
    const char *program = shift_args(&argc, &argv);
    const char *input_file_path = NULL;
    const char *output_file_path = NULL;

    while (argc > 0) {
        const char *flag = shift_args(&argc, &argv);

        if (!strcmp(flag, "-i")) {
            if (argc == 0) {
                fprintf(stderr, "Error: expect input file\n");
                return 1;
            }
            input_file_path = shift_args(&argc, &argv);
        } else if (!strcmp(flag, "-o")) {
            if (argc == 0) {
                fprintf(stderr, "Error: expect output path\n");
                return 1;
            }
            output_file_path = shift_args(&argc, &argv);
        } else if (!strcmp(flag, "-h")) {
            fprintf(stdout, "Usage: %s -i <input.lasm> -o <output.lim> [-h]\n",
                    program);
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
    if (output_file_path == NULL) {
        fprintf(stderr, "Error: output path is not provided\n");
        return 1;
    }

    String_View source = slurp_file(input_file_path);
    lim_translate_source(source, &lim, &lasm);
    lim_save_program_to_file(&lim, output_file_path);

    return 0;
}
