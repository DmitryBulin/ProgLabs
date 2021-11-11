#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

char check_arguments_validation(int argc, char** argv);
void print_arguments_info();
unsigned long int get_line_count(FILE* file);
unsigned short get_month_value(char* month_buff);

typedef struct error_statistic
{
    char* request_info;
    unsigned int error_quantity;
} error_statistic;

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
    printf("Got valid arguments. Starting to perform.\n");

    unsigned long int input_lines_count = get_line_count(input_file);
    time_t *request_times = malloc(input_lines_count * sizeof(time_t));
    printf("Lines in input file: %lu\n", input_lines_count);
    printf("Memory allocated: %zu\n", input_lines_count * sizeof(time_t));

    char line_buff[256];
    int whitespaces_passed_number, line_index;
    struct tm local_time;

    for (int i = 0; i < input_lines_count; i++)
    {
        fgets(line_buff, sizeof(line_buff), input_file);
        line_index = 0;
        whitespaces_passed_number = 0;
        while (line_index < 256 && whitespaces_passed_number != 3)
        {
            if (line_buff[line_index] == ' ')
            {
                whitespaces_passed_number++;
            }
            line_index++;
        }
        line_index++;

        local_time.tm_mday = (line_buff[line_index] - '0') * 10 + (line_buff[line_index + 1] - '0');
        line_index += 3;

        char *month_buff = malloc(3 * sizeof(char));
        strncpy(month_buff, &line_buff[line_index], 3);
        line_index += 4;
        local_time.tm_mon = get_month_value(month_buff);

        local_time.tm_year = (line_buff[line_index] - '0') * 1000 + (line_buff[line_index + 1] - '0') * 100 + (line_buff[line_index + 2] - '0') * 10 + (line_buff[line_index + 3] - '0') - 1900;
        line_index += 5;

        local_time.tm_hour = (line_buff[line_index] - '0') * 10 + (line_buff[line_index + 1] - '0');
        line_index += 3;

        local_time.tm_min = (line_buff[line_index] - '0') * 10 + (line_buff[line_index + 1] - '0');
        line_index += 3;

        local_time.tm_sec = (line_buff[line_index] - '0') * 10 + (line_buff[line_index + 1] - '0');
        line_index += 3;

        request_times[i] = mktime(&local_time);

        free(month_buff);
    }

    free(request_times);
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

unsigned long int get_line_count(FILE* file)
{
    long unsigned int line_count = 1;
    char temp_char;

    while (!feof(file))
    {
        temp_char = fgetc(file);
        if (temp_char == '\n')
        {
            line_count++;
        }
    }
    fseek(file, 0L, SEEK_SET);

    return line_count;
}

unsigned short get_month_value(char* month_buff)
{
    if (strcmp(month_buff, "Jan") == 0)
        return 0;
    if (strcmp(month_buff, "Feb") == 0)
        return 1;
    if (strcmp(month_buff, "Mar") == 0)
        return 2;
    if (strcmp(month_buff, "Apr") == 0)
        return 3;
    if (strcmp(month_buff, "May") == 0)
        return 4;
    if (strcmp(month_buff, "Jun") == 0)
        return 5;
    if (strcmp(month_buff, "Jul") == 0)
        return 6;
    if (strcmp(month_buff, "Aug") == 0)
        return 7;
    if (strcmp(month_buff, "Sep") == 0)
        return 8;
    if (strcmp(month_buff, "Oct") == 0)
        return 9;
    if (strcmp(month_buff, "Nov") == 0)
        return 10;
    if (strcmp(month_buff, "Dec") == 0)
        return 11;
    return 0;
}
