#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

#define EXP_MASK (((1LL << 11) - 1) << 52)
#define FRACTION_MASK ((1LL << 52) - 1)
#define SIGN_MASK (1LL << 63)
#define TYPE_MASK (((1LL << 4) - 1) << 48)
#define VALUE_MASK ((1LL << 48) - 1)

#define INSPECT_DOUBLE(type, value)                       \
    do {                                                  \
        type name = (value);                              \
        printf("% lf = ", name);                          \
        print_bits((uint8_t *) &name, sizeof(name));      \
        printf("  is_nan = %d, ", is_nan(name));          \
        printf("  isnan  = %d\n", isnan(name));           \
        printf("  is_inf = %d, ", is_inf(name));          \
        printf("  isinf  = %d\n", isinf(name));           \
        if (is_nan(name)) {                               \
            printf("  NaN Boxing: \n");                   \
            printf("    type  = %ld\n", get_type(name));  \
            printf("    value = %ld\n", get_value(name)); \
        }                                                 \
    } while (0)

#define TYPE(t) ((1 << 3) + t)

// Type of NaN boxing
enum Type {
    DOUBLE_T = 0,
    INTEGER_T,
    POINTER_T,
};

static void print_bits(uint8_t *bytes, size_t bytes_size)
{
    for (size_t i = 0; i < bytes_size; i++) {
        uint8_t byte = bytes[bytes_size - 1 - i];
        uint8_t mask = (1 << 7);
        for (size_t j = 0; j < 8; j++) {
            printf("%d", !!(byte & mask));
            mask >>= 1;
        }
        printf(" ");
    }
    printf("\n");
}

static int is_nan(double x)
{
    uint64_t y = (*(uint64_t *) &x);
    return ((y & EXP_MASK) == EXP_MASK) && ((y & FRACTION_MASK) != 0);
}

static int is_inf(double x)
{
    uint64_t y = (*(uint64_t *) &x);
    int result = ((y & EXP_MASK) == EXP_MASK) && ((y & FRACTION_MASK) == 0);
    return ((y & SIGN_MASK) == 0) ? result : -result;
}

static uint64_t get_type(double x)
{
    assert(is_nan(x));
    uint64_t y = *(uint64_t *) &x;
    return (y & TYPE_MASK) >> 48;
}

static double set_type(double x, uint64_t type)
{
    assert(is_inf(x) || is_nan(x));
    uint64_t *y = (uint64_t *) &x;
    *y = (*y & (~TYPE_MASK)) | ((type << 48) & TYPE_MASK);
    return x;
}

static uint64_t get_value(double x)
{
    assert(is_nan(x));
    uint64_t y = *(uint64_t *) &x;
    return (y & VALUE_MASK);
}

static double set_value(double x, uint64_t value)
{
    assert(is_inf(x) || is_nan(x));
    uint64_t *y = (uint64_t *) &x;
    *y = (*y & (~VALUE_MASK)) | (value & VALUE_MASK);
    return x;
}

static double make_inf(void)
{
    uint64_t x = EXP_MASK;
    return *(double *) &x;
}

static int is_double(double x)
{
    return !is_nan(x) || (is_nan(x) && get_type(x) == TYPE(DOUBLE_T));
}

static int is_integer(double x)
{
    return is_nan(x) && get_type(x) == TYPE(INTEGER_T);
}

static int is_pointer(double x)
{
    return is_nan(x) && get_type(x) == TYPE(POINTER_T);
}

static double as_double(double x)
{
    assert(is_double(x));
    return x;
}

static uint64_t as_integer(double x)
{
    assert(is_integer(x));
    return get_value(x);
}

static void *as_pointer(double x)
{
    assert(is_pointer(x));
    return (void *) get_value(x);
}

static double box_double(double x)
{
    return x;
}

static double box_integer(uint64_t x)
{
    return set_value(set_type(make_inf(), TYPE(INTEGER_T)), x);
}

static double box_pointer(void *x)
{
    return set_value(set_type(make_inf(), TYPE(POINTER_T)), (uint64_t) x);
}

#define VALUES_CAPACITY 256
static double values[VALUES_CAPACITY];
static size_t values_size = 0;

int main(void)
{
    // INSPECT_DOUBLE(double, 3.141592653);
    // INSPECT_DOUBLE(double, 1.0);
    // INSPECT_DOUBLE(double, -1.0);
    // INSPECT_DOUBLE(double, 0.0);
    // INSPECT_DOUBLE(double, -0.0);
    // INSPECT_DOUBLE(double, 1.0 / 0.0);
    // INSPECT_DOUBLE(double, -1.0 / 0.0);
    // INSPECT_DOUBLE(double, 0.0 / 0.0);
    // INSPECT_DOUBLE(double, -0.0 / 0.0);

    INSPECT_DOUBLE(double, make_inf());

    double pi = 3.141592653;
    values[values_size++] = box_double(pi);
    values[values_size++] = box_integer(123456LL);
    values[values_size++] = box_pointer(&pi);

    for (size_t i = 0; i < values_size; i++) {
        INSPECT_DOUBLE(double, values[i]);

        printf("  is double  = %d\n", is_double(values[i]));
        printf("  is integer = %d\n", is_integer(values[i]));
        printf("  is pointer = %d\n", is_pointer(values[i]));
    }

    assert(pi == as_double(values[0]));
    assert(123456LL == as_integer(values[1]));
    assert(&pi == as_pointer(values[2]));

    printf("Ok\n");

    return 0;
}
