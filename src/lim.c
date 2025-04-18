#include "lim.h"

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
        return "nop";
    case INST_PUSH:
        return "push";
    case INST_POP:
        return "pop";
    case INST_DUP:
        return "dup";
    case INST_PLUS:
        return "plus";
    case INST_MINUS:
        return "minus";
    case INST_MULT:
        return "mult";
    case INST_DIV:
        return "div";
    case INST_FPLUS:
        return "fplus";
    case INST_FMINUS:
        return "fminus";
    case INST_FMULT:
        return "fmult";
    case INST_FDIV:
        return "fdiv";
    case INST_GT:
        return "gt";
    case INST_LT:
        return "lt";
    case INST_GE:
        return "ge";
    case INST_LE:
        return "le";
    case INST_EQ:
        return "eq";
    case INST_JMP:
        return "jmp";
    case INST_JNZ:
        return "jnz";
    case INST_JZ:
        return "jz";
    case INST_SWAP:
        return "swap";
    case INST_CALL:
        return "call";
    case INST_RET:
        return "ret";
    case INST_NATIVE:
        return "native";
    case INST_HALT:
        return "halt";
    case INST_PRINT_DEBUG:
        return "print_debug";
    case INST_NUM:
    default:
        assert(false && "unreachable");
    }
}

bool inst_has_operand(Inst_Type type)
{
    switch (type) {
    case INST_PUSH:
    case INST_DUP:
    case INST_JMP:
    case INST_JNZ:
    case INST_JZ:
    case INST_SWAP:
    case INST_CALL:
    case INST_NATIVE:
        return true;

    case INST_NOP:
    case INST_POP:
    case INST_PLUS:
    case INST_MINUS:
    case INST_MULT:
    case INST_DIV:
    case INST_FPLUS:
    case INST_FMINUS:
    case INST_FMULT:
    case INST_FDIV:
    case INST_GT:
    case INST_LT:
    case INST_GE:
    case INST_LE:
    case INST_EQ:
    case INST_RET:
    case INST_HALT:
    case INST_PRINT_DEBUG:
        return false;

    case INST_NUM:
    default:
        assert(false && "unreachable");
    }
}

String_View cstr_as_sv(const char *str)
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

// Delimite a word by `delim` with changing `sv`
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

// Delimite a word by `delim` without changing `sv`
String_View sv_delim(String_View sv, char delim)
{
    size_t i = 0;
    while (i < sv.count && sv.data[i] != delim) {
        i++;
    }

    return (String_View){
        .count = i,
        .data = sv.data,
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
    int64_t result = 0;
    for (size_t i = 0; i < sv.count && isdigit(sv.data[i]); i++) {
        result = result * 10 + sv.data[i] - '0';
    }
    return (Word){.as_i64 = result};
}

int label_table_find(const Lasm *lasm, String_View label)
{
    for (size_t i = 0; i < lasm->labels_size; i++) {
        if (sv_equal(label, lasm->labels[i].name)) {
            return i;
        }
    }
    return -1;
}

void label_table_push(Lasm *lasm, String_View label, Inst_Addr addr)
{
    assert(lasm->labels_size < LABEL_CAPACITY);
    lasm->labels[lasm->labels_size++] = (Label){
        .name = label,
        .addr = addr,
    };
}

void label_table_push_unresolved_jmp(Lasm *lasm,
                                     Inst_Addr addr,
                                     String_View label)
{
    assert(lasm->unresolved_jmps_size < LABEL_CAPACITY);
    lasm->unresolved_jmps[lasm->unresolved_jmps_size++] = (Unresolved_Jmp){
        .addr = addr,
        .label = label,
    };
}

Trap lim_execute_inst(Lim *lim)
{
    if (lim->ip >= lim->program_size) {
        return TRAP_ILLEGAL_INST_ACCESS;
    }

    Inst inst = lim->program[lim->ip];
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

    case INST_POP:
        if (lim->stack_size < 1) {
            return TRAP_STACK_UNDERFLOW;
        }
        lim->stack_size--;
        lim->ip++;
        break;

    case INST_DUP:
        // This instruction's operand is offset about the top element of
        // stack. e.g. operand equal 0 means duplicate and push the current
        // top element in the stack.
        if (lim->stack_size >= LIM_STACK_CAPACITY) {
            return TRAP_STACK_OVERFLOW;
        }
        if (lim->stack_size <= inst.operand.as_u64) {
            return TRAP_STACK_UNDERFLOW;
        }

        lim->stack[lim->stack_size] =
            lim->stack[lim->stack_size - 1 - inst.operand.as_u64];
        lim->stack_size++;
        lim->ip++;
        break;

    case INST_PLUS:
        if (lim->stack_size < 2) {
            return TRAP_STACK_UNDERFLOW;
        }
        lim->stack[lim->stack_size - 2].as_i64 +=
            lim->stack[lim->stack_size - 1].as_i64;
        lim->stack_size--;
        lim->ip++;
        break;

    case INST_MINUS:
        if (lim->stack_size < 2) {
            return TRAP_STACK_UNDERFLOW;
        }
        lim->stack[lim->stack_size - 2].as_i64 -=
            lim->stack[lim->stack_size - 1].as_i64;
        lim->stack_size--;
        lim->ip++;
        break;

    case INST_MULT:
        if (lim->stack_size < 2) {
            return TRAP_STACK_UNDERFLOW;
        }
        lim->stack[lim->stack_size - 2].as_i64 *=
            lim->stack[lim->stack_size - 1].as_i64;
        lim->stack_size--;
        lim->ip++;
        break;

    case INST_DIV:
        if (lim->stack_size < 2) {
            return TRAP_STACK_UNDERFLOW;
        }
        if (lim->stack[lim->stack_size - 1].as_i64 == 0) {
            return TRAP_DIV_BY_ZERO;
        }
        lim->stack[lim->stack_size - 2].as_i64 /=
            lim->stack[lim->stack_size - 1].as_i64;
        lim->stack_size--;
        lim->ip++;
        break;

    case INST_FPLUS:
        if (lim->stack_size < 2) {
            return TRAP_STACK_UNDERFLOW;
        }
        lim->stack[lim->stack_size - 2].as_f64 +=
            lim->stack[lim->stack_size - 1].as_f64;
        lim->stack_size--;
        lim->ip++;
        break;

    case INST_FMINUS:
        if (lim->stack_size < 2) {
            return TRAP_STACK_UNDERFLOW;
        }
        lim->stack[lim->stack_size - 2].as_f64 -=
            lim->stack[lim->stack_size - 1].as_f64;
        lim->stack_size--;
        lim->ip++;
        break;

    case INST_FMULT:
        if (lim->stack_size < 2) {
            return TRAP_STACK_UNDERFLOW;
        }
        lim->stack[lim->stack_size - 2].as_f64 *=
            lim->stack[lim->stack_size - 1].as_f64;
        lim->stack_size--;
        lim->ip++;
        break;

    case INST_FDIV:
        if (lim->stack_size < 2) {
            return TRAP_STACK_UNDERFLOW;
        }
        lim->stack[lim->stack_size - 2].as_f64 /=
            lim->stack[lim->stack_size - 1].as_f64;
        lim->stack_size--;
        lim->ip++;
        break;

    case INST_GT:
        if (lim->stack_size < 2) {
            return TRAP_STACK_UNDERFLOW;
        }
        lim->stack[lim->stack_size - 2].as_i64 =
            lim->stack[lim->stack_size - 2].as_i64 >
            lim->stack[lim->stack_size - 1].as_i64;
        lim->stack_size--;
        lim->ip++;
        break;

    case INST_LT:
        if (lim->stack_size < 2) {
            return TRAP_STACK_UNDERFLOW;
        }
        lim->stack[lim->stack_size - 2].as_i64 =
            lim->stack[lim->stack_size - 2].as_i64 <
            lim->stack[lim->stack_size - 1].as_i64;
        lim->stack_size--;
        lim->ip++;
        break;

    case INST_GE:
        if (lim->stack_size < 2) {
            return TRAP_STACK_UNDERFLOW;
        }
        lim->stack[lim->stack_size - 2].as_i64 =
            lim->stack[lim->stack_size - 2].as_i64 >=
            lim->stack[lim->stack_size - 1].as_i64;
        lim->stack_size--;
        lim->ip++;
        break;

    case INST_LE:
        if (lim->stack_size < 2) {
            return TRAP_STACK_UNDERFLOW;
        }
        lim->stack[lim->stack_size - 2].as_i64 =
            lim->stack[lim->stack_size - 2].as_i64 <=
            lim->stack[lim->stack_size - 1].as_i64;
        lim->stack_size--;
        lim->ip++;
        break;

    case INST_EQ:
        if (lim->stack_size < 2) {
            return TRAP_STACK_UNDERFLOW;
        }
        lim->stack[lim->stack_size - 2].as_i64 =
            lim->stack[lim->stack_size - 2].as_i64 ==
            lim->stack[lim->stack_size - 1].as_i64;
        lim->stack_size--;
        lim->ip++;
        break;

    case INST_JMP:
        // In this instruction, its address counld be illegal, since during
        // next instruction execution this function would return trap/error
        // TRAP_ILLEGAL_INST_ACCESS.
        lim->ip = inst.operand.as_u64;
        break;

    case INST_JNZ:
        if (lim->stack_size < 1) {
            return TRAP_STACK_UNDERFLOW;
        }
        // In this instruction, its address counld be illegal, since during
        // next instruction execution this function would return trap/error
        // TRAP_ILLEGAL_INST_ACCESS.
        if (lim->stack[--lim->stack_size].as_u64) {
            lim->ip = inst.operand.as_u64;
        } else {
            lim->ip++;
        }
        break;

    case INST_JZ:
        if (lim->stack_size < 1) {
            return TRAP_STACK_UNDERFLOW;
        }
        // In this instruction, its address counld be illegal, since during
        // next instruction execution this function would return trap/error
        // TRAP_ILLEGAL_INST_ACCESS.
        if (!lim->stack[--lim->stack_size].as_u64) {
            lim->ip = inst.operand.as_u64;
        } else {
            lim->ip++;
        }
        break;

    case INST_SWAP:
        if (lim->stack_size <= inst.operand.as_u64) {
            return TRAP_STACK_UNDERFLOW;
        }
        if (inst.operand.as_u64 > 0) {
            // skip swap with itself
            uint64_t t = lim->stack[lim->stack_size - 1].as_u64;
            lim->stack[lim->stack_size - 1].as_u64 =
                lim->stack[lim->stack_size - 1 - inst.operand.as_u64].as_u64;
            lim->stack[lim->stack_size - 1 - inst.operand.as_u64].as_u64 = t;
        }
        lim->ip++;
        break;

    case INST_CALL:
        if (lim->stack_size >= LIM_STACK_CAPACITY) {
            return TRAP_STACK_OVERFLOW;
        }
        lim->stack[lim->stack_size++].as_u64 = lim->ip + 1;
        lim->ip = inst.operand.as_u64;
        break;

    case INST_RET:
        // Calling Convention: after return from function call, the return value
        // (if exists) should store in the top of stack.
        if (lim->stack_size < 1) {
            return TRAP_STACK_UNDERFLOW;
        }
        lim->ip = lim->stack[--lim->stack_size].as_u64;
        break;

    case INST_NATIVE:
        if (inst.operand.as_u64 >= lim->natives_size) {
            return TRAP_ILLEGAL_OPERAND;
        }
        Trap trap = lim->natives[inst.operand.as_u64](lim);
        if (trap != TRAP_OK) {
            return trap;
        }
        lim->ip++;
        break;

    case INST_HALT:
        lim->halt = true;
        break;

    case INST_PRINT_DEBUG:
        if (lim->stack_size < 1) {
            return TRAP_STACK_UNDERFLOW;
        }
        Word word = lim->stack[--lim->stack_size];
        printf("%lu %ld %lf %p\n", word.as_u64, word.as_i64, word.as_f64,
               word.as_ptr);
        lim->ip++;
        break;

    case INST_NUM:
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

void lim_load_program_from_memory(Lim *lim,
                                  Inst *program,
                                  uint64_t program_size)
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

Word number_literal_as_word(String_View sv)
{
    assert(sv.count < 1024);
    char str[sv.count + 1];

    memcpy(str, sv.data, sv.count);
    str[sv.count] = '\0';

    Word result = {0};

    char *endptr = NULL;
    result.as_i64 = strtoll(str, &endptr, 10);
    if ((size_t) (endptr - str) != sv.count) {
        result.as_f64 = strtod(str, &endptr);
        if ((size_t) (endptr - str) != sv.count) {
            fprintf(stderr, "ERROR: %s is not a valid number literal\n", str);
            assert(false);
            exit(1);
        }
    }

    return result;
}

static Inst lim_translate_line(Lasm *lasm, Inst_Addr addr, String_View line)
{
    line = sv_trim_left(line);
    String_View inst_name = sv_chop_delim(&line, ' ');
    String_View operand = sv_trim(sv_chop_delim(&line, '#'));

    if (sv_equal(inst_name, cstr_as_sv(inst_type_as_cstr(INST_NOP)))) {
        return MAKE_INST_NOP();
    } else if (sv_equal(inst_name, cstr_as_sv(inst_type_as_cstr(INST_PUSH)))) {
        return MAKE_INST_PUSH(number_literal_as_word(operand));
    } else if (sv_equal(inst_name, cstr_as_sv(inst_type_as_cstr(INST_POP)))) {
        return MAKE_INST_POP();
    } else if (sv_equal(inst_name, cstr_as_sv(inst_type_as_cstr(INST_DUP)))) {
        return MAKE_INST_DUP(number_literal_as_word(operand));
    } else if (sv_equal(inst_name, cstr_as_sv(inst_type_as_cstr(INST_PLUS)))) {
        return MAKE_INST_PLUS();
    } else if (sv_equal(inst_name, cstr_as_sv(inst_type_as_cstr(INST_MINUS)))) {
        return MAKE_INST_MINUS();
    } else if (sv_equal(inst_name, cstr_as_sv(inst_type_as_cstr(INST_MULT)))) {
        return MAKE_INST_MULT();
    } else if (sv_equal(inst_name, cstr_as_sv(inst_type_as_cstr(INST_DIV)))) {
        return MAKE_INST_DIV();
    } else if (sv_equal(inst_name, cstr_as_sv(inst_type_as_cstr(INST_FPLUS)))) {
        return MAKE_INST_FPLUS();
    } else if (sv_equal(inst_name,
                        cstr_as_sv(inst_type_as_cstr(INST_FMINUS)))) {
        return MAKE_INST_FMINUS();
    } else if (sv_equal(inst_name, cstr_as_sv(inst_type_as_cstr(INST_FMULT)))) {
        return MAKE_INST_FMULT();
    } else if (sv_equal(inst_name, cstr_as_sv(inst_type_as_cstr(INST_FDIV)))) {
        return MAKE_INST_FDIV();
    } else if (sv_equal(inst_name, cstr_as_sv(inst_type_as_cstr(INST_GT)))) {
        return MAKE_INST_GT();
    } else if (sv_equal(inst_name, cstr_as_sv(inst_type_as_cstr(INST_LT)))) {
        return MAKE_INST_LT();
    } else if (sv_equal(inst_name, cstr_as_sv(inst_type_as_cstr(INST_GE)))) {
        return MAKE_INST_GE();
    } else if (sv_equal(inst_name, cstr_as_sv(inst_type_as_cstr(INST_LE)))) {
        return MAKE_INST_LE();
    } else if (sv_equal(inst_name, cstr_as_sv(inst_type_as_cstr(INST_EQ)))) {
        return MAKE_INST_EQ();
    } else if (sv_equal(inst_name, cstr_as_sv(inst_type_as_cstr(INST_JMP)))) {
        if (operand.count > 0 && isdigit(*operand.data)) {
            return MAKE_INST_JMP(number_literal_as_word(operand));
        } else {
            label_table_push_unresolved_jmp(lasm, addr, operand);
            return MAKE_INST_JMP_WITHOUT_OPERAND();
        }
    } else if (sv_equal(inst_name, cstr_as_sv(inst_type_as_cstr(INST_JNZ)))) {
        if (operand.count > 0 && isdigit(*operand.data)) {
            return MAKE_INST_JNZ(number_literal_as_word(operand));
        } else {
            label_table_push_unresolved_jmp(lasm, addr, operand);
            return MAKE_INST_JNZ_WITHOUT_OPERAND();
        }
    } else if (sv_equal(inst_name, cstr_as_sv(inst_type_as_cstr(INST_JZ)))) {
        if (operand.count > 0 && isdigit(*operand.data)) {
            return MAKE_INST_JZ(number_literal_as_word(operand));
        } else {
            label_table_push_unresolved_jmp(lasm, addr, operand);
            return MAKE_INST_JZ_WITHOUT_OPERAND();
        }
    } else if (sv_equal(inst_name, cstr_as_sv(inst_type_as_cstr(INST_SWAP)))) {
        return MAKE_INST_SWAP(number_literal_as_word(operand));
    } else if (sv_equal(inst_name, cstr_as_sv(inst_type_as_cstr(INST_CALL)))) {
        // call instruction only support label
        label_table_push_unresolved_jmp(lasm, addr, operand);
        return MAKE_INST_CALL_WITHOUT_OPERAND();
    } else if (sv_equal(inst_name, cstr_as_sv(inst_type_as_cstr(INST_RET)))) {
        return MAKE_INST_RET();
    } else if (sv_equal(inst_name,
                        cstr_as_sv(inst_type_as_cstr(INST_NATIVE)))) {
        return MAKE_INST_NATIVE(number_literal_as_word(operand));
    } else if (sv_equal(inst_name, cstr_as_sv(inst_type_as_cstr(INST_HALT)))) {
        return MAKE_INST_HALT();
    } else if (sv_equal(inst_name,
                        cstr_as_sv(inst_type_as_cstr(INST_PRINT_DEBUG)))) {
        return MAKE_INST_PRINT_DEBUG();
    } else {
        fprintf(stderr, "ERROR: unknown instruction `%.*s`\n",
                (int) inst_name.count, inst_name.data);
        assert(false);
        exit(-1);
    }

    return MAKE_INST_NOP();
}

void lim_translate_source(String_View source, Lim *lim, Lasm *lasm)
{
    lim->program_size = 0;

    // First pass
    while (source.count > 0) {
        assert(lim->program_size < LIM_PROGRAM_CAPACITY);
        String_View line = sv_chop_delim(&source, '\n');
        line = sv_trim_left(line);

        String_View word = sv_delim(line, ' ');

        // example:
        // ```
        // label: # comment
        // ```
        // Note: this implementation requires label and instruction can not
        // locate in the same line

        // labels
        if (word.count > 0 && word.data[word.count - 1] == ':') {
            label_table_push(
                lasm, (String_View){.count = word.count - 1, .data = word.data},
                lim->program_size);
            sv_chop_delim(&line, ' ');
            line = sv_trim_left(line);
            word = sv_delim(line, ' ');
        }

        // skip blank lines and comments
        if (word.count == 0 || *word.data == '#')
            continue;

        Inst inst = lim_translate_line(lasm, lim->program_size, line);
        lim->program[lim->program_size++] = inst;
    }

    // Second pass
    for (size_t i = 0; i < lasm->unresolved_jmps_size; i++) {
        int j = label_table_find(lasm, lasm->unresolved_jmps[i].label);
        lim->program[lasm->unresolved_jmps[i].addr].operand.as_i64 =
            lasm->labels[j].addr;
    }
}

void lim_dump_stack(FILE *stream, const Lim *lim)
{
    fprintf(stream, "Stack:\n");
    fprintf(stream, "  u64\ti64\tf64\tptr\n");
    if (lim->stack_size > 0) {
        for (uint64_t i = 0; i < lim->stack_size; i++) {
            fprintf(stream, "  %lu\t%ld\t%lf\t%p\n", lim->stack[i].as_u64,
                    lim->stack[i].as_i64, lim->stack[i].as_f64,
                    lim->stack[i].as_ptr);
        }
    } else {
        fprintf(stream, "  [empty]\n");
    }
}

static Trap lim_alloc(Lim *lim)
{
    if (lim->stack_size < 1) {
        return TRAP_STACK_UNDERFLOW;
    }
    lim->stack[lim->stack_size - 1].as_ptr =
        malloc(lim->stack[lim->stack_size - 1].as_u64);
    return TRAP_OK;
}

static Trap lim_free(Lim *lim)
{
    if (lim->stack_size < 1) {
        return TRAP_STACK_UNDERFLOW;
    }
    free(lim->stack[--lim->stack_size].as_ptr);
    return TRAP_OK;
}

static Trap lim_print_u64(Lim *lim)
{
    if (lim->stack_size < 1) {
        return TRAP_STACK_UNDERFLOW;
    }
    printf("%lu\n", lim->stack[--lim->stack_size].as_u64);
    return TRAP_OK;
}

static Trap lim_print_i64(Lim *lim)
{
    if (lim->stack_size < 1) {
        return TRAP_STACK_UNDERFLOW;
    }
    printf("%ld\n", lim->stack[--lim->stack_size].as_i64);
    return TRAP_OK;
}

static Trap lim_print_f64(Lim *lim)
{
    if (lim->stack_size < 1) {
        return TRAP_STACK_UNDERFLOW;
    }
    printf("%lf\n", lim->stack[--lim->stack_size].as_f64);
    return TRAP_OK;
}

static Trap lim_print_ptr(Lim *lim)
{
    if (lim->stack_size < 1) {
        return TRAP_STACK_UNDERFLOW;
    }
    printf("%p\n", lim->stack[--lim->stack_size].as_ptr);
    return TRAP_OK;
}

void lim_attach_natives(Lim *lim)
{
    lim_push_native_func(lim, lim_alloc);      // native number 0
    lim_push_native_func(lim, lim_free);       // native number 1
    lim_push_native_func(lim, lim_print_u64);  // native number 2
    lim_push_native_func(lim, lim_print_i64);  // native number 3
    lim_push_native_func(lim, lim_print_f64);  // native number 4
    lim_push_native_func(lim, lim_print_ptr);  // native number 5
}

void lim_push_native_func(Lim *lim, Lim_Native_Func func)
{
    assert(lim->natives_size < LIM_NATIVES_CAPACITY);
    lim->natives[lim->natives_size++] = func;
}

const char *shift_args(int *argc, char ***argv)
{
    assert(*argc > 0);
    char *result = **argv;
    *argv += 1;
    *argc -= 1;
    return result;
}

Lim lim = {0};
Lasm lasm = {0};
