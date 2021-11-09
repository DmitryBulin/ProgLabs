#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

char check_arguments_validation(int argc, char** argv);
void print_arguments_info();

unsigned long long int required_time_gap;

int main(int argc, char** argv)
{
    char valid_arguments = check_arguments_validation(argc, argv);
    if (valid_arguments == 0)
    {
        return 0;
    }
    FILE *input_file, *output_file;
    input_file = fopen(argv[1], "r");
    output_file = fopen(argv[3], "w");

    fclose(input_file);
    fclose(output_file);
    return 0;
}

char check_arguments_validation(int argc, char** argv)
{
    if (argc != 4)
    {
        print_arguments_info();
        return 0;
    }

    if (access(argv[1], R_OK) != 0)
    {
        printf("Failed to open input file %s", argv[1]);
        return 0;
    }

    if (access(argv[3], F_OK) != 0)
    {
        printf("Failed to find output file %s\n", argv[3]);
        printf("Required file will be created in current directory\n");
    }

    required_time_gap = atoll(argv[2]);
    if (required_time_gap == 0)
    {
        printf("Failed to convert time to valid value: %llu", required_time_gap);
        return 0;
    }

    return 1;
}
void print_arguments_info()
{
    printf("<------- Wrong application arguments ------->\n");
    printf("  Valid format:\n");
    printf("  input time output\n");
    printf("    input - input file name\n");
    printf("    time - required time in seconds (0, 18446744073709551615]\n");
    printf("    input - input file name\n");
}
