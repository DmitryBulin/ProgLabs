#include <stdio.h>
#include <string.h>
#include <stdbool.h>

void check_argument_for_valid_option(char* arg);
void process_requested_options(FILE* file);
long unsigned int get_line_count(FILE* file);
long unsigned int get_file_size(FILE* file);
long unsigned int get_word_count(FILE* file);

bool
    option_lines = false,
    option_bytes = false,
    option_words = false;

int main(int argc, char** argv)
{
    for (int i = 1; i < argc - 1; i++)
        check_argument_for_valid_option(argv[i]);

    FILE* operated_file = fopen(argv[argc - 1], "r");
    if (!operated_file)
    {
        perror("Failed to open file");
        return 1;
    }

    process_requested_options(operated_file);

    fclose(operated_file);
    return 0;
}

void check_argument_for_valid_option(char* arg)
{
    if (strcmp(arg, "-l") == 0 || strcmp(arg, "--lines") == 0)
        option_lines = true;

    else if (strcmp(arg, "-c") == 0 || strcmp(arg, "--bytes") == 0)
        option_bytes = true;

    else if (strcmp(arg, "-w") == 0 || strcmp(arg, "--words") == 0)
        option_words = true;
}

void process_requested_options(FILE* file)
{
    if (option_lines)
    {
        long unsigned int line_count = get_line_count(file);
        printf("Lines in file: %lu\n", line_count);
    }
    if (option_bytes)
    {
        long unsigned int file_size = get_file_size(file);
        printf("File size in bytes: %lu\n", file_size);
    }
    if (option_words)
    {
        long unsigned int word_count = get_word_count(file);
        printf("Words in file: %lu\n", word_count);
    }
}

long unsigned int get_file_size(FILE* file)
{
    fseek(file, 0L, SEEK_END);
    long long int file_size = ftell(file);
    fseek(file, 0L, SEEK_SET);

    return file_size;
}

long unsigned int get_line_count(FILE* file)
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

long unsigned int get_word_count(FILE* file)
{
    long unsigned int word_count = 0;
    unsigned char previous_char, current_char;

    if (!feof(file))
        previous_char = getc(file);
    while (!feof(file))
    {
        current_char = getc(file);
        if ((current_char == 255 || current_char == ' ' || current_char == '\n' || current_char == '\0' || current_char == '\t')
            && !(previous_char == ' ' || previous_char == '\n' || previous_char == '\0' || previous_char == '\t'))
            word_count++;
        previous_char = current_char;
    }
    fseek(file, 0L, SEEK_SET);

    return word_count;
}
