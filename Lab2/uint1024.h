#define UINT1024_BLOCK_COUNT 35
#define UINT1024_BLOCK_MAX 1000000000

#define MAX(x, y) (x > y ? x : y)
#define MIN(x, y) (x < y ? x : y)
#define CLAMP(x, upper, lower) (MIN(upper, MAX(x, lower)))

typedef struct uint1024_t
{
    unsigned int data[UINT1024_BLOCK_COUNT];
} uint1024_t;

char compare_uint(uint1024_t a, uint1024_t b);
uint1024_t from_uint(unsigned int x);
uint1024_t add_op(uint1024_t x, uint1024_t y);
uint1024_t subtr_op(uint1024_t x, uint1024_t y);
uint1024_t mult_op(uint1024_t x, uint1024_t y);
void printf_value(uint1024_t x);
void scanf_value(uint1024_t* x);

