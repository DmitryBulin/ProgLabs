#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MAX_FILEPATH 260
#define TEMPORARY_FILE_NAME "temp_file.mp3"

typedef enum command_type
{
    SHOW, SET, GET
} command_type;

typedef struct cmd_args 
{ 
    char* frame_value;
    command_type command; 
    char frame_name[5];        
    char filepath[MAX_FILEPATH + 1];
} cmd_args;

unsigned int bytes_to_size(char dest[4], char is_header);
void size_to_bytes(unsigned int size, char dest[4], char is_header);
char parse_cmdargs(int argc, char** argv, cmd_args *args);
void process_cmds(cmd_args *args);
void set_field(FILE* input_file, cmd_args *args, char frame_exists, int frame_size, long empty_frame_padding);

int main(int argc, char** argv) 
{    
    cmd_args args = { NULL, 0, { 0, 0, 0, 0, '\0' } };
    if (parse_cmdargs(argc, argv, &args) == 1) 
    {
        process_cmds(&args);
    }
    return 0;
}

char parse_cmdargs(int argc, char** argv, cmd_args* args) 
{     
    if (argc < 3) 
    {
        printf("Not enough arguments\n");
        return 0;
    }

    if (*(argv[1] + 2) == 'f') 
    {  
        int filepath_len = strlen(argv[1]) - 11;        
        strncpy(args->filepath, argv[1] + 11, MAX_FILEPATH);  
        args->filepath[filepath_len] = '\0';
    } 
    else 
    {
        printf("Incorrect filepath argument, format: --filepath=file.mp3\n");
        return 0;
    }

    if (*(argv[2] + 2) == 's' && *(argv[2] + 3) == 'h') 
    {
        args->command = SHOW;
    }
    else if (*(argv[2] + 2) == 'g') 
    {
        args->command = GET;
        strncpy(args->frame_name, argv[2] + 6, 4);        
    }
    else if (*(argv[2] + 2) == 's') 
    {  
        if (argc < 4 || strlen(argv[3]) - 8 == 0) 
        {
            printf("No value to set, format: --set=XXXX --value=NewValue");
            return 0;
        }        

        args->command = SET;
        strncpy(args->frame_name, argv[2] + 6, 4);  

        int frame_value_length = strlen(argv[3]);
        args->frame_value = (char*) malloc(frame_value_length * sizeof(char));
        strncpy(args->frame_value, argv[3] + 8, frame_value_length);        
    } 
    else 
    {
        printf("Incorrect second argument.\n");
        return 0;
    }    

    return 1;
}

void process_cmds(cmd_args* args) 
{ 
    if (args->command == SET)
    {
        remove(TEMPORARY_FILE_NAME);
    }

    FILE* input_file = fopen(args->filepath, "rb");
    if (input_file == NULL) 
    {
        printf("Failed to open input file");
        exit(1);
    }

    char frame_exists = 0; 
    int saved_frame_size = 0;
    long empty_frame_padding = -1;
    unsigned long bytes_read = 0;
    unsigned long frame_size = 0;

    int tag_size;
    char header[10];
    char frame_name[5] = {0, 0, 0, 0, '\0'};
    unsigned int buffer_size = 100;
    char *buffer = (char*) malloc(buffer_size * sizeof(char));

    fread(header, sizeof(header), 1, input_file);
    tag_size = bytes_to_size(header + 6, 1);
    
    while (1)
    {
        fread(header, sizeof(header), 1, input_file);
        bytes_read += 10;
        if (bytes_read > tag_size)
        {
            break;
        }
        frame_size = bytes_to_size(header + 4, 0);

        if (frame_size == 0) 
        {
            if (empty_frame_padding == -1) 
            {
                empty_frame_padding = ftell(input_file) - sizeof(header);
            }
            fseek(input_file, 1, SEEK_CUR);
            bytes_read++;
            continue;
        }

        if (frame_size > buffer_size)
        {
            buffer = (char*) realloc(buffer, frame_size * sizeof(char));
        }
        fread(buffer, frame_size, 1, input_file);
        bytes_read += frame_size;    
        if (bytes_read > tag_size)
        { 
            break;
        }

        strncpy(frame_name, header, 4);
        if (args->command == SHOW || (args->command == GET && strcmp(frame_name, args->frame_name) == 0)) 
        {
            printf("Frame %s: ", frame_name);
            for (int x = 0; x < frame_size; x++)
            {
                printf("%c", buffer[x]);
            }
            printf("\n\n");
        }
        else if (args->command == SET && strcmp(frame_name, args->frame_name) == 0) 
        { 
            fseek(input_file, -(10 + frame_size), SEEK_CUR);         
            frame_exists = 1;
            saved_frame_size = frame_size;
            break;
        }
    }

    if (args->command == SET) 
    {
        set_field(input_file, args, frame_exists, saved_frame_size, empty_frame_padding);
    }

    fclose(input_file);
}

void set_field(FILE* input_file, cmd_args* args, char frame_exists, int frame_size, long empty_frame_padding) 
{
    FILE* output_file = fopen(TEMPORARY_FILE_NAME, "wb+");

    unsigned long bytes_number_to_copy;
    if (frame_exists == 1)
    {
        bytes_number_to_copy = ftell(input_file); 
    }
    else
    {
        bytes_number_to_copy = empty_frame_padding;
    }
    bytes_number_to_copy -= 10;

    char header[10];
    fseek(input_file, 0, SEEK_SET);
    fread(header, 10, 1, input_file);
    int tag_size = bytes_to_size(header + 6, 1);
    if (frame_exists == 1)
    {
        tag_size = tag_size - frame_size + strlen(args->frame_value);
    }
    else
    {
        tag_size += 10 + strlen(args->frame_value);
    }
    
    size_to_bytes(tag_size, &header[6], 1);        
    fwrite(header, 10, 1, output_file);

    char temp_char;
    for (unsigned long i = 0; i < bytes_number_to_copy; i++) 
    {
        temp_char = fgetc(input_file);
        fputc(temp_char, output_file);
    }

    strcpy(header, args->frame_name);
    size_to_bytes(strlen(args->frame_value) + 1, &header[4], 0);   
    header[8] = 0x00;
    header[9] = 0x00;
    fwrite(header, 10, 1, output_file);    
    fputc(0, output_file);
    fwrite(args->frame_value, strlen(args->frame_value), 1, output_file);
    
    if (frame_exists == 1)
    {
        fseek(input_file, frame_size + 10, SEEK_CUR); 
    }
    else
    {
        fseek(input_file, empty_frame_padding, SEEK_SET);
    }
    while (!feof(input_file))
    {
        temp_char = fgetc(input_file);
        fputc(temp_char, output_file);
    }

    fclose(input_file);
    fclose(output_file);    
    remove(args->filepath);
    usleep(1000 * 10);
    char rename_result = rename(TEMPORARY_FILE_NAME, args->filepath);
    if (rename_result == 1) 
    {
        printf("Failed to rename file due to: %s\n", strerror(errno));        
    }
}

unsigned int bytes_to_size(char dest[4], char is_header) 
{
    unsigned int result = 0;
    for (int i = 0; i < 4; i++) 
    {
        result += dest[i] << ((8 - is_header) * (3 - i));
    }    
    return result;
}

void size_to_bytes(unsigned int size, char dest[4], char is_header) 
{    
    for (int i = 0; i < 4; i++) 
    {
        dest[i] = size >> ((8 - is_header) * (3 - i)) & (is_header == 1 ? 0x7f : 0xff);
    }        
}