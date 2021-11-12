#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h> //Required for file validation

char check_arguments_validation(int argc, char** argv);
void print_arguments_info();
time_t parsed_time(char* time_buff);
unsigned short get_month_value(char* month_buff);
void proccess_new_failed_request(unsigned char request_length, char* request_buff);
void print_result_info(FILE* output_file, char* output_file_name);

typedef struct request_statistic
{
    char* request_info;
    unsigned int request_number;
} request_statistic;

const short MAX_LINE_LENGTH = 512;
unsigned int max_requests_in_time_gap;
unsigned long int total_line_count;
unsigned long int total_statistic_entry;
unsigned long int total_failed_request_number;
unsigned long long int required_time_gap;
time_t *request_times;
request_statistic *failed_requests;

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

    //Global variables
    request_times = calloc(0, sizeof(time_t));
    failed_requests = calloc(0, sizeof(request_statistic));
    total_statistic_entry = 0;
    max_requests_in_time_gap = 0;
    total_failed_request_number = 0;
    total_line_count = 0;

    //Local variables
    char *request_buff;
    char *time_buff = calloc(20, sizeof(char));
    char *line_buff = calloc(MAX_LINE_LENGTH, sizeof(char));
    unsigned char temp_index;
    unsigned char request_length;
    unsigned char request_start_index;
    unsigned short request_status;
    unsigned int left = 0;
    unsigned int right = 0;
    unsigned int temp_max_requests = 0;

    while (fgets(line_buff, MAX_LINE_LENGTH, input_file))
    {
        temp_index = 0;

        //Parsing request time
        while (line_buff[++temp_index - 1] != '[');
        strncpy(time_buff, line_buff + temp_index, 20);
        request_times = realloc(request_times, (total_line_count + 1) * sizeof(time_t));
        request_times[total_line_count] = parsed_time(time_buff);

        //Recalculating max requests in time gap
        temp_max_requests++;
        while(right > left && request_times[right] - request_times[left] > required_time_gap)
        {
            left++;
            temp_max_requests--;
        }
        right++;
        max_requests_in_time_gap = (temp_max_requests > max_requests_in_time_gap ? temp_max_requests : max_requests_in_time_gap);

        //Parsing request body
        while (line_buff[++temp_index - 1] != '"');
        request_start_index = temp_index;
        while (line_buff[++temp_index + 1] != '"');
        request_length = temp_index - request_start_index + 1;
        request_buff = calloc(request_length, sizeof(char) + 1);
        strncpy(request_buff, line_buff + request_start_index, request_length);
        request_buff[request_length] = '\0';

        //Parsing request status
        temp_index += 3;
        request_status = (line_buff[temp_index] - '0') * 100 + (line_buff[temp_index + 1] - '0') * 10 + (line_buff[temp_index + 2] - '0');
        if (request_status / 100 == 5)
        {
            proccess_new_failed_request(request_length, request_buff);
        }

        free(request_buff);
        total_line_count++;
    }

    print_result_info(output_file, argv[3]);

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

void proccess_new_failed_request(unsigned char request_length, char* request_buff)
{
    total_failed_request_number++;

    unsigned char found_simular_request = 0;

    for (int i = 0; i < total_statistic_entry && found_simular_request == 0; i++)
    {
        if (strcmp(failed_requests[i].request_info, request_buff) == 0)
        {
            found_simular_request = 1;
            failed_requests[i].request_number++;
        }
    }

    //New type of failed requests found
    if (found_simular_request == 0)
    {
        failed_requests = realloc(failed_requests, (total_statistic_entry + 1) * sizeof(request_statistic));
        failed_requests[total_statistic_entry].request_info = calloc(request_length + 1, sizeof(char));
        strncpy(failed_requests[total_statistic_entry].request_info, request_buff, request_length + 1);
        failed_requests[total_statistic_entry].request_number = 1;
        total_statistic_entry++;
    }
}

void print_result_info(FILE* output_file, char* output_file_name)
{
    fprintf(output_file, "Maximum requests in %lu seconds: %lu\n\n", required_time_gap, max_requests_in_time_gap);
    fprintf(output_file, "Total failed requests: %d\n", total_failed_request_number);
    fprintf(output_file, "Percentage of failed requests: %f%%\n", ((double)total_failed_request_number)/((double)total_line_count) * 100);
    for (int i = 0; i < total_statistic_entry; i++)
    {
        fprintf(output_file, "%s => %d times\n", failed_requests[i].request_info, failed_requests[i].request_number);
    }
    printf("Result put in file %s\n", output_file_name);
}
