#include "lim.h"

int main(int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(stderr, "Usage: ./lasm <input.lasm> <output.lim>\n");
        fprintf(stderr, "Error: expect input and output paths\n");
        exit(1);
    }

    const char *input_file_path = argv[1];
    const char *output_file_path = argv[2];

    String_View source = slurp_file(input_file_path);
    lim.program_size =
        lim_translate_source(source, lim.program, LIM_PROGRAM_CAPACITY);
    lim_save_program_to_file(&lim, output_file_path);

    return 0;
}
