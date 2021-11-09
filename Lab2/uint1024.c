#include <uint1024.h>

char compare_uint(uint1024_t a, uint1024_t b)
{
    for (int i = UINT1024_BLOCK_COUNT - 1; i >= 0; i--)
    {
        if (a.data[i] > b.data[i])
            return 1;
        if (a.data[i] < b.data[i])
            return -1;
    }

    return 0;
}

uint1024_t from_uint(unsigned int x)
{
    uint1024_t result;

    result.data[0] = x;
    for (int i = 1; i < UINT1024_BLOCK_COUNT; i++)
    {
        result.data[i] = 0;
    }

    return result;
}

uint1024_t add_op(uint1024_t x, uint1024_t y)
{
    uint1024_t result;
    unsigned long carry = 0;
    unsigned long block_result;

    for (int i = 0; i < UINT1024_BLOCK_COUNT; i++)
    {
        block_result = x.data[i] + y.data[i] + carry;
        carry = block_result / UINT1024_BLOCK_MAX;
        block_result %= UINT1024_BLOCK_MAX;
        result.data[i] = block_result;
    }

    return result;
}

uint1024_t subtr_op(uint1024_t x, uint1024_t y)
{
    uint1024_t result;
    unsigned long carry = 0;
    unsigned long block_result;
    char compare_result = compare_uint(x, y);

    if (compare_result == 0)
        return from_uint(0);
    else if (compare_result == -1)
    {
        result = x;
        x = y;
        y = result;
    }

    for (int i = 0; i < UINT1024_BLOCK_COUNT; i++)
    {
        block_result = (UINT1024_BLOCK_MAX + x.data[i] - y.data[i] - carry) % UINT1024_BLOCK_MAX;
        carry = y.data[i] > x.data[i];
        result.data[i] = block_result;
    }

    return result;
}

uint1024_t mult_op(uint1024_t x, uint1024_t y)
{
    uint1024_t result = from_uint(0);
    unsigned long long carry = 0;
    unsigned long long block_result = 0;

    for (int i = 0; i < UINT1024_BLOCK_COUNT; i++)
    {
        if (y.data[i] == 0)
        {
            continue;
        }
        carry = 0;
        for (int j = 0; j < UINT1024_BLOCK_COUNT - i; j++)
        {
            block_result = result.data[j + i] + ((unsigned long long)x.data[j]) * ((unsigned long long)y.data[i]) + carry;
            carry = block_result / UINT1024_BLOCK_MAX;
            block_result %= UINT1024_BLOCK_MAX;
            result.data[j + i] = block_result;
        }
    }

    return result;
}

void printf_value(uint1024_t x)
{
    int i = UINT1024_BLOCK_COUNT - 1;
    while (i >= 0 && x.data[i] == 0)
    {
        i--;
    }
    if(i == -1)
    {
        printf("0");
    }
    else
    {
        printf("%u", x.data[i]);
        i--;
        for (; i >= 0; i--)
        {
            printf("%09u", x.data[i]);
        }
    }
    printf("\n");
}

void scanf_value(uint1024_t* x)
{
    char input_string[UINT1024_BLOCK_COUNT * 9 + 1];
    int string_index = 0, block_index = 0, power_index = 0;
    for (int i = 0; i < UINT1024_BLOCK_COUNT; i++)
        x->data[i] = 0;

    scanf("%315s", input_string);
    while (string_index < UINT1024_BLOCK_COUNT * 9 + 1 && input_string[string_index] != 0)
        string_index++;
    string_index--;

    unsigned int char_result;
    for (;string_index >= 0; string_index--)
    {
        char_result = CLAMP(input_string[string_index] - '0', 9, 0);
        x->data[block_index] += char_result * pow(10, power_index);
        power_index++;
        if (power_index == 9)
        {
            power_index = 0;
            block_index++;
        }
    }
}
