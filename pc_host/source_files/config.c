//To extract baudrate & COM port name from config_file.txt

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "config.h"

#define file_maxchar_count 50 //No. of characters to be read from config_file.

//Below mentioned macros help in extracting desired parameter from buffer containing confi_file data.
#define token_count 4         
#define port_token_index 1      
#define baud_token_index 3

Config_Info cfg={0};    //To hold parameters extracted.

//Function to extract parameters.
void config_load(void)
{
    //Transfer contents of file to memory buffer.

    FILE *fptr=fopen("config_file.txt", "r");
    if(!fptr)
        config_error_handling(fopen_err);   //If file opening failed.

    char *config_mem=calloc(file_maxchar_count, 1); 
    if(!config_mem)
    {
        fclose(fptr);
        config_error_handling(malloc_err);  //Memory allocation failed.
    }
    
    fread(config_mem, sizeof(char), file_maxchar_count, fptr ); //Read a fixed no. of characters, known to cover all relevant data.
    if(ferror(fptr))    //If file read operation failed.
    {
        fclose(fptr);
        free(config_mem);
        config_error_handling(fread_err);
    }

    fclose(fptr);

    int i=0;

    char *token[token_count]={0};

    const char *delim=" =;\r\n";

    //Tokenize config_file buffer.
    token[i]=strtok(config_mem, delim); 
    while(i<token_count-1)
    {
        i++;
        token[i]=strtok(NULL, delim); 
    }

    //COM port name extraction.
    cfg.port_name=malloc(strlen(token[port_token_index] )+1 );
    if(!cfg.port_name)
    {
        free(config_mem);
        config_error_handling(malloc_err);
    }
    strcpy(cfg.port_name, token[port_token_index]);                    //Due to data within config_file having a set format, position of desired token is known.

    //Baud rate extraction.
    cfg.baud_rate=(int)strtol(token[baud_token_index],NULL, 10); //Due to data within config_file having a set format, position of desired token is known.

    free(config_mem);
    
    return;
}

//Function to handle errors related to config_file.
void config_error_handling(config_error cfg_err)
{
    switch(cfg_err)
    {
        case fopen_err:
            printf("Error: configuration file opening failed.\nCommand aborted.");
            exit(1);

        case fread_err:
            printf("Error: configuration file read error.\nCommand aborted.");
            exit(1);

        case malloc_err:
            printf("Error: memory allocation failed.\nCommand aborted.");
            exit(1);

    }
}
