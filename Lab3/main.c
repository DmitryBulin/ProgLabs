#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

char check_arguments_validation(int argc, char** argv);
void print_arguments_info();
time_t parsed_time(char* time_buff);
unsigned short get_month_value(char* month_buff);
unsigned int find_max_requests_in_time_gap(int total_lines_count);

typedef struct request_statistic
{
    char* request_info;
    unsigned int request_number;
} request_statistic;

const short MAX_LINE_LENGTH = 512;
unsigned long long int required_time_gap;
time_t *request_times;

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

    request_times = calloc(0, sizeof(time_t));
    request_statistic *failed_requests = calloc(0, sizeof(request_statistic));
    unsigned long int total_statistic_entry = 0;

    char *line_buff = calloc(MAX_LINE_LENGTH, sizeof(char)), *time_buff = calloc(20, sizeof(char));
    unsigned char temp_index, request_start_index, request_length;
    unsigned short request_status;
    unsigned long int total_failed_request_number = 0, current_line_count = 0;

    while (fgets(line_buff, MAX_LINE_LENGTH, input_file))
    {
        temp_index = 0;
        while (line_buff[++temp_index - 1] != '[');
        strncpy(time_buff, line_buff + temp_index, 20);
        request_times = realloc(request_times, (current_line_count + 1) * sizeof(time_t));
        request_times[current_line_count] = parsed_time(time_buff);

        while (line_buff[++temp_index - 1] != '"');
        request_start_index = temp_index;
        while (line_buff[++temp_index + 1] != '"');
        request_length = temp_index - request_start_index + 1;
        char *request_buff = calloc(request_length, sizeof(char) + 1);
        strncpy(request_buff, line_buff + request_start_index, request_length);
        request_buff[request_length] = '\0';

        temp_index += 3;
        request_status = (line_buff[temp_index] - '0') * 100 + (line_buff[temp_index + 1] - '0') * 10 + (line_buff[temp_index + 2] - '0');
        if (request_status / 100 == 5)
        {
            total_failed_request_number++;
            char found_simular_request = 0;
            for (temp_index = 0; temp_index < total_statistic_entry; temp_index++)
            {
                if (strcmp(failed_requests[temp_index].request_info, request_buff) == 0)
                {
                    found_simular_request = 1;
                    failed_requests[temp_index].request_number++;
                    break;
                }
            }
            if (found_simular_request == 0)
            {
                failed_requests = realloc(failed_requests, (total_statistic_entry + 1) * sizeof(request_statistic));
                failed_requests[total_statistic_entry].request_info = calloc(request_length + 1, sizeof(char));
                strncpy(failed_requests[total_statistic_entry].request_info, request_buff, request_length + 1);
                failed_requests[total_statistic_entry].request_number = 1;
                total_statistic_entry++;
            }
        }

        free(request_buff);
        current_line_count++;
    }

    unsigned int max_requests_in_time_gap = find_max_requests_in_time_gap(current_line_count);
    fprintf(output_file, "Maximum requests in %lu seconds: %lu\n\n", required_time_gap, max_requests_in_time_gap);
    fprintf(output_file, "Total failed requests: %d\n", total_failed_request_number);
    fprintf(output_file, "Percentage of failed requests: %f%%\n", ((double)total_failed_request_number)/((double)current_line_count) * 100);
    for (temp_index = 0; temp_index < total_statistic_entry; temp_index++)
    {
        fprintf(output_file, "%s => %d times\n", failed_requests[temp_index].request_info, failed_requests[temp_index].request_number);
    }
    printf("Result put in file %s\n", argv[3]);

    free(time_buff);
    free(line_buff);
    free(failed_requests);
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

time_t parsed_time(char* time_buff)
{
    struct tm result_time;
    result_time.tm_mday = (time_buff[0] - '0') * 10 + (time_buff[1] - '0');
    result_time.tm_year = (time_buff[7] - '0') * 1000 + (time_buff[8] - '0') * 100 + (time_buff[9] - '0') * 10 + (time_buff[10] - '0') - 1900;
    result_time.tm_hour = (time_buff[12] - '0') * 10 + (time_buff[13] - '0');
    result_time.tm_min = (time_buff[15] - '0') * 10 + (time_buff[16] - '0');
    result_time.tm_sec = (time_buff[18] - '0') * 10 + (time_buff[19] - '0');

    char *month_buff = malloc(3 * sizeof(char));
    strncpy(month_buff, time_buff + 3, 3);
    result_time.tm_mon = get_month_value(month_buff);
    free(month_buff);

    return mktime(&result_time);
}

unsigned int find_max_requests_in_time_gap(int total_lines_count)
{
    unsigned int result = 0;

    unsigned int left = 0, right = 0, temp_result = 0;
    while(right < total_lines_count)
    {
        temp_result++;
        while(right > left && request_times[right] - request_times[left] > required_time_gap)
        {
            left++;
            temp_result--;
        }
        right++;
        result = (temp_result > result ? temp_result : result);
    }

    return result;
}
