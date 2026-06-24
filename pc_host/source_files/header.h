#ifndef HEAD_H
#define HEAD_H

//To handle errors encountered in main.c
typedef enum
{
    NO_CMD,
    WRNG_IN,
    IN_OR
}MAIN_ERR;

void str_tolower(char *buf);

int in_num_process(char *buf);

void main_error_handle(MAIN_ERR err);

#endif