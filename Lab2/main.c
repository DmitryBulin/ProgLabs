#include <stdio.h>
#include <stdlib.h>
#include "uint1024.h"


int main()
{
    printf("Struct size: %zu\n", sizeof(uint1024_t));
    uint1024_test();

    return 0;
}

void uint1024_test()
{
    uint1024_t first_uint1024, second_uint1024, temp_result;
    unsigned int inputed_uint = 0;

    printf("Enter random unsigned int([0, 4294967295]):\n");
    scanf("%u", &inputed_uint);
    printf("Entered unsigned int in uint1024: ");
    temp_result = from_uint(inputed_uint);
    printf_value(temp_result);

    printf("Enter first uint A:\n");
    scanf_value(&first_uint1024);
    printf("Enter second uint B:\n");
    scanf_value(&second_uint1024);

    printf("A + B = ");
    temp_result = add_op(first_uint1024, second_uint1024);
    printf_value(temp_result);

    printf("B + A = ");
    temp_result = add_op(second_uint1024, first_uint1024);
    printf_value(temp_result);

    printf("A - B = ");
    temp_result = subtr_op(first_uint1024, second_uint1024);
    printf_value(temp_result);

    printf("B - A = ");
    temp_result = subtr_op(second_uint1024, first_uint1024);
    printf_value(temp_result);

    printf("A * B = ");
    temp_result = mult_op(first_uint1024, second_uint1024);
    printf_value(temp_result);

    printf("B * A = ");
    temp_result = mult_op(second_uint1024, first_uint1024);
    printf_value(temp_result);

    printf("A * A * A * A * A = ");
    temp_result = mult_op(first_uint1024, first_uint1024);
    temp_result = mult_op(first_uint1024, mult_op(temp_result, temp_result));
    printf_value(temp_result);

    printf("B * B * B * B * B = ");
    temp_result = mult_op(second_uint1024, second_uint1024);
    temp_result = mult_op(second_uint1024, mult_op(temp_result, temp_result));
    printf_value(temp_result);
}

