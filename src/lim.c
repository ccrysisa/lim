#include "lim.h"
#include <assert.h>
#include <stdio.h>

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
    case TRAP_ILLEGAL_INST_ACCESS:
        return "TRAP_ILLEGAL_INST_ACCESS";
    case TRAP_ILLEGAL_OPERAND:
        return "TRAP_ILLEGAL_OPERAND";
    default:
        assert(0 && "trap_as_cstr: unreachable");
    }
}

const char *inst_type_as_cstr(Inst_Type type)
{
    switch (type) {
    case INST_NOP:
        return "INST_NOP";
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
    case INST_JMP:
        return "INST_JMP";
    case INST_HALT:
        return "INST_HALT";
    case INST_EQ:
        return "INST_EQ";
    case INST_JNZ:
        return "INST_JNZ";
    case INST_JZ:
        return "INST_JZ";
    case INST_DUP:
        return "INST_DUP";
    case INST_PRINT_DEBUG:
        return "INST_PRINT_DEBUG";
    default:
        assert(0 && "inst_type_as_cstr: unreachable");
    }
}

String_View cstr_as_sv(char *str)
{
    return (String_View){
        .count = strlen(str),
        .data = str,
    };
}

String_View sv_trim_left(String_View sv)
{
    size_t i = 0;
    while (i < sv.count && isspace(sv.data[i])) {
        i++;
    }

    return (String_View){
        .count = sv.count - i,
        .data = sv.data + i,
    };
}

String_View sv_trim_right(String_View sv)
{
    size_t i = 0;
    while (i < sv.count && isspace(sv.data[sv.count - 1 - i])) {
        i++;
    }

    return (String_View){
        .count = sv.count - i,
        .data = sv.data,
    };
}

String_View sv_trim(String_View sv)
{
    return sv_trim_right(sv_trim_left(sv));
}

String_View sv_chop_delim(String_View *sv, char delim)
{
    size_t i = 0;
    while (i < sv->count && sv->data[i] != delim) {
        i++;
    }

    const char *data = sv->data;
    int flag = i < sv->count;
    sv->count -= flag ? i + 1 : i;
    sv->data += flag ? i + 1 : i;

    return (String_View){
        .count = i,
        .data = data,
    };
}

int sv_equal(String_View a, String_View b)
{
    return a.count == b.count && !strncmp(a.data, b.data, a.count);
}

Word sv_to_word(String_View sv)
{
    // can just using atoi/atol/atoll to convert, but for compatibility we
    // implement this function by hand.
    Word result = 0;
    for (size_t i = 0; i < sv.count && isdigit(sv.data[i]); i++) {
        result = result * 10 + sv.data[i] - '0';
    }
    return result;
}

int label_table_find(const Label_Table *lt, String_View label)
{
    for (size_t i = 0; i < lt->labels_size; i++) {
        if (sv_equal(label, lt->labels[i].name)) {
            return i;
        }
    }
    return -1;
}

void label_table_push(Label_Table *lt, String_View label, Word addr)
{
    assert(lt->labels_size < LABEL_CAPACITY);
    lt->labels[lt->labels_size++] = (Label){
        .name = label,
        .addr = addr,
    };
}

void label_table_push_unresolved_jmp(Label_Table *lt,
                                     Word addr,
                                     String_View label)
{
    assert(lt->unresolved_jmps_size < LABEL_CAPACITY);
    lt->unresolved_jmps[lt->unresolved_jmps_size++] = (Unresolved_Jmp){
        .addr = addr,
        .label = label,
    };
}

Trap lim_execute_inst(Lim *lim)
{
    if (lim->ip < 0 || lim->ip >= lim->program_size) {
        return TRAP_ILLEGAL_INST_ACCESS;
    }

    Inst inst = lim->program[lim->ip];
#if DEBUG
    printf("%s\n", inst_type_as_cstr(inst.type));
#endif
    switch (inst.type) {
    case INST_NOP:
        lim->ip++;
        break;

    case INST_PUSH:
        if (lim->stack_size >= LIM_STACK_CAPACITY) {
            return TRAP_STACK_OVERFLOW;
        }
        lim->stack[lim->stack_size++] = inst.operand;
        lim->ip++;
        break;

    case INST_PLUS:
        if (lim->stack_size < 2) {
            return TRAP_STACK_UNDERFLOW;
        }
        lim->stack[lim->stack_size - 2] += lim->stack[lim->stack_size - 1];
        lim->stack_size--;
        lim->ip++;
        break;

    case INST_MINUS:
        if (lim->stack_size < 2) {
            return TRAP_STACK_UNDERFLOW;
        }
        lim->stack[lim->stack_size - 2] -= lim->stack[lim->stack_size - 1];
        lim->stack_size--;
        lim->ip++;
        break;

    case INST_MULT:
        if (lim->stack_size < 2) {
            return TRAP_STACK_UNDERFLOW;
        }
        lim->stack[lim->stack_size - 2] *= lim->stack[lim->stack_size - 1];
        lim->stack_size--;
        lim->ip++;
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
        lim->ip++;
        break;

    case INST_JMP:
        // In this instruction, its address counld be illegal, since during next
        // instruction execution this function would return trap/error
        // TRAP_ILLEGAL_INST_ACCESS.
        lim->ip = inst.operand;
        break;

    case INST_HALT:
        lim->halt = 1;
        break;

    case INST_EQ:
        if (lim->stack_size < 2) {
            return TRAP_STACK_UNDERFLOW;
        }
        lim->stack[lim->stack_size - 2] =
            lim->stack[lim->stack_size - 2] == lim->stack[lim->stack_size - 1];
        lim->stack_size--;
        lim->ip++;
        break;

    case INST_JNZ:
        if (lim->stack_size < 1) {
            return TRAP_STACK_UNDERFLOW;
        }
        // In this instruction, its address counld be illegal, since during next
        // instruction execution this function would return trap/error
        // TRAP_ILLEGAL_INST_ACCESS.
        if (lim->stack[--lim->stack_size]) {
            lim->ip = inst.operand;
        } else {
            lim->ip++;
        }
        break;

    case INST_JZ:
        if (lim->stack_size < 1) {
            return TRAP_STACK_UNDERFLOW;
        }
        // In this instruction, its address counld be illegal, since during next
        // instruction execution this function would return trap/error
        // TRAP_ILLEGAL_INST_ACCESS.
        if (!lim->stack[--lim->stack_size]) {
            lim->ip = inst.operand;
        } else {
            lim->ip++;
        }
        break;

    case INST_DUP:
        // This instruction's operand is offset about the top element of stack.
        // e.g. operand equal 0 means duplicate and push the current top element
        // in the stack.
        if (lim->stack_size >= LIM_STACK_CAPACITY) {
            return TRAP_STACK_OVERFLOW;
        }
        if (inst.operand < 0) {
            return TRAP_ILLEGAL_OPERAND;
        }
        if (lim->stack_size - inst.operand <= 0) {
            return TRAP_STACK_UNDERFLOW;
        }

        lim->stack[lim->stack_size] =
            lim->stack[lim->stack_size - 1 - inst.operand];
        lim->stack_size++;
        lim->ip++;
        break;

    case INST_PRINT_DEBUG:
        if (lim->stack_size < 1) {
            return TRAP_STACK_UNDERFLOW;
        }
        printf("%ld\n", lim->stack[--lim->stack_size]);
        lim->ip++;
        break;

    default:
        return TRAP_ILLEGAL_INST;
    }
    return TRAP_OK;
}

Trap lim_execute_program(Lim *lim)
{
    while (!lim->halt) {
        Trap trap = lim_execute_inst(lim);
        if (trap != TRAP_OK) {
            return trap;
        }
    }

    return TRAP_OK;
}

void lim_load_program_from_memory(Lim *lim, Inst *program, Word program_size)
{
    assert(program_size <= LIM_PROGRAM_CAPACITY);

    memcpy(lim->program, program, sizeof(program[0]) * program_size);
    lim->program_size = program_size;
}

void lim_load_program_from_file(Lim *lim, const char *file_path)
{
    FILE *f = fopen(file_path, "rb");
    if (f == NULL) {
        fprintf(stderr, "ERROR: Counld not open file `%s`: %s\n", file_path,
                strerror(errno));
        exit(1);
    }

    if (fseek(f, 0, SEEK_END) < 0) {
        fprintf(stderr, "ERROR: Counld not read file `%s`: %s\n", file_path,
                strerror(errno));
        exit(1);
    }

    long m = ftell(f);
    if (m < 0) {
        fprintf(stderr, "ERROR: Counld not read file `%s`: %s\n", file_path,
                strerror(errno));
        exit(1);
    }

    assert(m % sizeof(lim->program[0]) == 0);
    assert((size_t) m <= LIM_PROGRAM_CAPACITY * sizeof(lim->program[0]));

    if (fseek(f, 0, SEEK_SET) < 0) {
        fprintf(stderr, "ERROR: Counld not read file `%s`: %s\n", file_path,
                strerror(errno));
        exit(1);
    }

    lim->program_size = fread(lim->program, sizeof(lim->program[0]),
                              m / sizeof(lim->program[0]), f);

    if (ferror(f)) {
        fprintf(stderr, "ERROR: Counld not write file `%s`: %s\n", file_path,
                strerror(errno));
        exit(1);
    }

    fclose(f);
}

void lim_save_program_to_file(Lim *lim, const char *file_path)
{
    FILE *f = fopen(file_path, "wb");
    if (f == NULL) {
        fprintf(stderr, "ERROR: Counld not open file `%s`: %s\n", file_path,
                strerror(errno));
        exit(1);
    }

    fwrite(lim->program, sizeof(lim->program[0]), lim->program_size, f);

    if (ferror(f)) {
        fprintf(stderr, "ERROR: Counld not write file `%s`: %s\n", file_path,
                strerror(errno));
        exit(1);
    }

    fclose(f);
}

String_View slurp_file(const char *file_path)
{
    FILE *f = fopen(file_path, "rb");
    if (f == NULL) {
        fprintf(stderr, "ERROR: Counld not open file `%s`: %s\n", file_path,
                strerror(errno));
        exit(1);
    }

    if (fseek(f, 0, SEEK_END) < 0) {
        fprintf(stderr, "ERROR: Counld not read file `%s`: %s\n", file_path,
                strerror(errno));
        exit(1);
    }

    long m = ftell(f);
    if (m < 0) {
        fprintf(stderr, "ERROR: Counld not read file `%s`: %s\n", file_path,
                strerror(errno));
        exit(1);
    }

    char *buffer = malloc(m);
    if (buffer == NULL) {
        fprintf(stderr, "ERROR: Counld not allocate memory for file: %s\n",
                strerror(errno));
        exit(1);
    }

    if (fseek(f, 0, SEEK_SET) < 0) {
        fprintf(stderr, "ERROR: Counld not read file `%s`: %s\n", file_path,
                strerror(errno));
        exit(1);
    }

    size_t n = fread(buffer, 1, m, f);

    if (ferror(f)) {
        fprintf(stderr, "ERROR: Counld not write file `%s`: %s\n", file_path,
                strerror(errno));
        exit(1);
    }

    fclose(f);

    return (String_View){
        .count = n,
        .data = buffer,
    };
}

Inst lim_translate_line(Label_Table *lt, Word addr, String_View line)
{
    line = sv_trim_left(line);
    String_View inst_name = sv_chop_delim(&line, ' ');
    String_View operand = sv_trim(sv_chop_delim(&line, '#'));

    if (sv_equal(inst_name, cstr_as_sv("nop"))) {
        return (Inst) MAKE_INST_NOP();
    } else if (sv_equal(inst_name, cstr_as_sv("push"))) {
        return (Inst) MAKE_INST_PUSH(sv_to_word(operand));
    } else if (sv_equal(inst_name, cstr_as_sv("plus"))) {
        return (Inst) MAKE_INST_PLUS();
    } else if (sv_equal(inst_name, cstr_as_sv("minus"))) {
        return (Inst) MAKE_INST_MINUS();
    } else if (sv_equal(inst_name, cstr_as_sv("mult"))) {
        return (Inst) MAKE_INST_MULT();
    } else if (sv_equal(inst_name, cstr_as_sv("div"))) {
        return (Inst) MAKE_INST_DIV();
    } else if (sv_equal(inst_name, cstr_as_sv("jmp"))) {
        label_table_push_unresolved_jmp(lt, addr, operand);
        return (Inst){.type = INST_JMP};
    } else if (sv_equal(inst_name, cstr_as_sv("halt"))) {
        return (Inst) MAKE_INST_HALT();
    } else if (sv_equal(inst_name, cstr_as_sv("eq"))) {
        return (Inst) MAKE_INST_EQ();
    } else if (sv_equal(inst_name, cstr_as_sv("jnz"))) {
        label_table_push_unresolved_jmp(lt, addr, operand);
        return (Inst){.type = INST_JNZ};
    } else if (sv_equal(inst_name, cstr_as_sv("jz"))) {
        label_table_push_unresolved_jmp(lt, addr, operand);
        return (Inst){.type = INST_JZ};
    } else if (sv_equal(inst_name, cstr_as_sv("dup"))) {
        return (Inst) MAKE_INST_DUP(sv_to_word(operand));
    } else if (sv_equal(inst_name, cstr_as_sv("print_debug"))) {
        return (Inst) MAKE_INST_PRINT_DEBUG();
    } else {
        fprintf(stderr, "ERROR: unknown instruction `%.*s`\n",
                (int) inst_name.count, inst_name.data);
    }

    return (Inst) MAKE_INST_NOP();
}

void lim_translate_source(String_View source, Lim *lim)
{
    lim->program_size = 0;

    // First pass
    while (source.count > 0) {
        assert(lim->program_size < LIM_PROGRAM_CAPACITY);
        String_View line = sv_chop_delim(&source, '\n');
        // skip blank lines and comments
        if (line.count == 0 || *line.data == '#')
            continue;
        if (line.count > 0 && line.data[line.count - 1] == ':') {
            label_table_push(
                &lim->label_table,
                (String_View){.count = line.count - 1, .data = line.data},
                lim->program_size);
            continue;
        }

        Inst inst =
            lim_translate_line(&lim->label_table, lim->program_size, line);
        lim->program[lim->program_size++] = inst;
    }

    // Second pass
    for (size_t i = 0; i < lim->label_table.unresolved_jmps_size; i++) {
        int j = label_table_find(&lim->label_table,
                                 lim->label_table.unresolved_jmps[i].label);
        lim->program[lim->label_table.unresolved_jmps[i].addr].operand =
            lim->label_table.labels[j].addr;
    }
}

void lim_dump_stack(FILE *stream, const Lim *lim)
{
    fprintf(stream, "Stack:\n");
    if (lim->stack_size > 0) {
        for (Word i = 0; i < lim->stack_size; i++) {
            fprintf(stream, "  %ld\n", lim->stack[i]);
        }
    } else {
        fprintf(stream, "  [empty]\n");
    }
}

Lim lim = {0};

const char *shift_args(int *argc, char ***argv)
{
    assert(*argc > 0);
    char *result = **argv;
    *argv += 1;
    *argc -= 1;
    return result;
}
