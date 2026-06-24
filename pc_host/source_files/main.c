#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"
#include "serial.h"
#include "command.h"
#include "config.h"

#define in_maxchar 20           //Maximum character limit, beyond which user input is invalid.

extern COMMAND cmd[];           //Command table conatining commands & associated function.

extern char rx_buffer[];        //Buffer conatining response received fromPIC to sent command.

char num_str[in_maxchar]={0};   //To hold number input by user.

extern Config_Info cfg;         //Contains parameters extracted from config_file.


int main(int argc, char *argv[])
{
    if( (argc!=2) && (argc!=3) )                //Incorrect user input handling.
        main_error_handle(WRNG_IN);

    if( strlen(argv[1])>=in_maxchar )       //Incorrect user input handling.
        main_error_handle(NO_CMD);

    //When input command contains a number.    
    if(argc==3)
    {
        if( strlen(argv[2])>=in_maxchar )   //Incorrect user input handling.
            main_error_handle(NO_CMD);

        int i=in_num_process(argv[2]);  

        if(i==1)       //in_num_process() used to check whether entered number is valid.  
            main_error_handle(WRNG_IN);

        if(i==2)       //in_num_process() used to check whether entered number is within range.  
            main_error_handle(IN_OR);

    }

    char* buf=calloc(in_maxchar, 1);
    strcpy(buf, argv[1]);  //Transfer input command to buffer.
    str_tolower(buf);                   //Convert string to lowercase.                 
    
    config_load();                      //Extract paramters from config_file.
    
    serial_open();                      //Setup serial communication.

    int cmd_index=cmd_select(buf);      //Determine the command entered by user.
    free(buf);

    if(cmd_index<0)                     //Invalid user input handling.
    {
        serial_close();
        main_error_handle(NO_CMD);
    }
    if( (cmd_index==6) && (argc==2) )   //Invalid user input handling.
    {   
        serial_close();
        main_error_handle(WRNG_IN);
    }
        
    cmd[cmd_index].send_cmd();          //Send command to PIC.
    
    command_receive();                  //Receive response from PIC in rx_buufer.

    cmd[cmd_index].exec_cmd(rx_buffer); //Process & diplay result.

    serial_close();                     //Terminate serial communication.

    return 0;
}

//Function to convert input string(buf) to lowercase.
void str_tolower(char *buf) 
{   
    int len=(int)strlen(buf);

    for(int i=0; i<len; i++)
    {
        if( buf[i]>='A' && buf[i]<='Z' )
            buf[i]=buf[i]+32;
    }
    return;
}

//To determine whether input string(buf) is a valid number or not.
int in_num_process(char *buf)
{
    int i=0;
    while(buf[i])   //Check whether input is numeric.
    {
        if( (buf[i]<'0' || buf[i]>'9') && buf[i]!='.' )
            return 1;
        i++;
    }

    float num=strtof(buf, NULL);//Convert to number.

    if( (num<-20) || (num>100) )            //Check if within required range(-20 to 100 °C).
        return 2;

    //Convert the number back to string after rounding off to having decimal place of 1.  
    snprintf(num_str, in_maxchar, "%0.1f", num);
    strcat(num_str, "\n");  //Input number now stored in global variable num_str[].

    return 0;
}

void main_error_handle(MAIN_ERR err)
{
    switch(err)
    {
        case NO_CMD:
            printf("Error: command not found.");
            exit(1);

        case WRNG_IN:
            printf("Error: incorrect command input format.");
            exit(1);
        
        case IN_OR:
            printf("Error: input number not within range (-20 to 100 degC).");
            exit(1);
    }
}
