#ifndef CONFIG_H
#define CONFIG_H

//Struct to hold parameters extracted.
typedef struct Config_Info
{
    int baud_rate;
    char* port_name;    //COM port name.
}Config_Info;

//For error handling related to config_file opening & parameter extraction.
typedef enum  
{
    fopen_err,
    fread_err,
    malloc_err

}config_error;

void config_load(void);

void config_error_handling(config_error cfg_err);

#endif
