#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "serial.h"
#include "command.h"

/*
 * 1. 'cmd' is an array of type struct COMMAND.
 * 2. It contains list of all defined input commands and associated functions to execute & display results.
 */
COMMAND cmd[]={
    {"--voltage",       send_volt,      display_voltage},
    {"--temp",          send_temp,      display_temp},
    {"--status",        send_status,    display_status},
    {"--uptime",        send_uptime,    display_uptime},
    {"--templimit",     send_gtlimit,   display_glimit},
    {"--all",           send_all,       display_all},
    {"--set_templimit", send_stlimit,   display_slimit},
    {"--log",           send_log,       display_log},
    {"--clear_log",     send_clog,      clear_log}
};

#define cmd_count (int)( sizeof(cmd)/sizeof(cmd[0]) )    //Size of 'cmd' array.

#define end_str "***\n"                 //String used to terminate responses received from PIC side. 
#define end_str_size 5                  //Terminator string size.

#define rx_buffer_size 160              //Size of buffer used to store response received from PIC side.
char rx_buffer[rx_buffer_size]={0};     //Used to store response received from PIC side.

//Below mentioned macros are useful for separating & displaying results when command requesting all readings(--all) is sent.
#define all_element_count   5           
#define volt_index          0
#define temp_index          1
#define status_index        2
#define limit_index         3
#define time_index          4

#define log_entry_size 20

extern char  num_str[];     //Buffer used to concatenate user input number to command string to PIC.

/*
 * 1. Function to determine which command was input, based on argument *buf.
 * 2. Returns the index position of command from 'cmd' array.
 * 3. Returns -1 on user input not matching any of the defined commands.
 */
int cmd_select(char *buf)
{
    for(int i=0; i<cmd_count; i++)  //cmd_count is the total number of commands.
    {
        if( !strcmp(buf, cmd[i].cmd_name) )
            return i;
    }
    return -1;
}

/*
 * 1. Function to receive response from PIC side.
 * 2. Keeps receiving until terminating string encountered or receive buffer overflows.
 */
void command_receive(void)
{
    char buf[end_str_size]={0};
    
    int i=0;

    /*
     * Operations are carried out in the following manner:
     * 1. No. of characters equal to that in terminator string are read. 
     * 2. Check if read string is same as terminator string; if yes, stop receiving.
     * 3. Else if not a match; then send the first character of buf to receive buffer.
     * 4. Shift the contents of buf back by 1 index; load the newly read character as the last character of string in buf.
     * 5. Repeat from step 2.
     */
    while(1)
    {
        if(i==0)
        {
            for(int j=0; j<end_str_size-1; j++)
            {
                buf[j]=*( serial_receive() ); //serial_receive() function used to read a character over UART.
            }
        }

        if(strcmp(buf, end_str))
        {
            rx_buffer[i]=buf[0];      
            i++;
            if(i==rx_buffer_size)
            {
                printf("Error: command receive buffer overflow");
                serial_close();
                exit(1);
            }
                   
            for(int j=0; j<end_str_size-2; j++)
                buf[j]=buf[j+1];

            buf[end_str_size-2]=*( serial_receive() );
        }
        else
        {
            rx_buffer[i]='\0';
            break;
        }
    }
    return;
}

//Function to send command for fetching voltage reading from PIC.
void send_volt(void)
{
    serial_flush();     //Flush out stale data remaining over UART line.

    serial_send("GET_VOLTAGE\n");   //serial_send() function used to send commands to PIC over UART.
    return;
}

//Function to send command for fetching temperature reading from PIC.
void send_temp(void)
{
    serial_flush();     //Flush out stale data remaining over UART line.

    serial_send("GET_TEMP\n");  //serial_send() function used to send commands to PIC over UART.
    return;
}

//Function to send command for fetching status from PIC.
void send_status(void)
{
    serial_flush();     //Flush out stale data remaining over UART line.

    serial_send("GET_STATUS\n");    //serial_send() function used to send commands to PIC over UART.
    return;
}

//Function to send command for fetching uptime from PIC.
void send_uptime(void)
{
    serial_flush();     //Flush out stale data remaining over UART line.

    serial_send("GET_TIME\n");  //serial_send() function used to send commands to PIC over UART.
    return;
}

//Function to send command for fetching temeprature threshold value from PIC.
void send_gtlimit(void)
{
    serial_flush();     //Flush out stale data remaining over UART line.

    serial_send("GET_TLIMIT\n"); //serial_send() function used to send commands to PIC over UART.
    return;
}

//Function to send command for fetching all displayable parameters from PIC.
void send_all(void)
{
    serial_flush();      //Flush out stale data remaining over UART line.

    serial_send("GET_ALL\n");   //serial_send() function used to send commands to PIC over UART.
    return;
}

//Function to send command for setting temeprature threshold value, entered as user input, in PIC.
void send_stlimit(void)
{
    char out_buffer[rx_buffer_size]={0};

    strcpy(out_buffer, "SET_TLIMIT ");
    strcat(out_buffer, num_str);        //num_str[], from main.c, contains the user input number in string format. 
    
    serial_flush();                  //Flush out stale data remaining over UART line.

    serial_send(out_buffer); //serial_send() function used to send commands to PIC over UART.
    return;
}

//Function to send command for fetching log entries from PIC.
void send_log(void)
{
    serial_flush();                   //Flush out stale data remaining over UART line.

    serial_send("GET_LOG\n"); //serial_send() function used to send commands to PIC over UART.
    return;
}

//Function to send command for clearing log entries in PIC.
void send_clog(void)
{
    serial_flush();                    //Flush out stale data remaining over UART line.

    serial_send("CLEAR_LOG\n");//serial_send() function used to send commands to PIC over UART.
    return;
}

/*
 * 1. Function to display voltage reading received from PIC.
 * 2. Input argument: buf = rx_buffer(containing PIC response received over UART)
 */
void display_voltage(char *buf)
{
    char *token;

    //Split buffer into tokens.
    token=strtok(buf, " :\r\n");
    int i=0;        //Keep track of split tokens.
    while(token)
    {
        token=strtok(NULL, " :\r\n");
        
        i++;
        if(i&1)     //Due to response having a set format, position of desired token is known.
            break;
    }

    float voltage=strtof(token, NULL);  //Extract number from string.

    printf("%-17s : %g V\n", "Voltage", voltage);

    return;
}


/*
 * 1. Function to display temperature reading received from PIC.
 * 2. Input argument: buf = rx_buffer(containing PIC response received over UART)
 */
void display_temp(char *buf)
{
    char *token;

    //Split buffer into tokens.
    token=strtok(buf, " :\r\n");
    int i=0;        //Keep track of split tokens.
    while(token)
    {
        token=strtok(NULL, " :\r\n");
        
        i++;
        if(i&1)     //Due to response having a set format, position of desired token is known.
            break;
    }

    float temp=strtof(token, NULL); //Extract number from string.

    printf("%-17s : %g degC\n", "Temperature", temp);

    return;
}


/*
 * 1. Function to display status received from PIC.
 * 2. Input argument: buf = rx_buffer(containing PIC response received over UART)
 */
void display_status(char *buf)
{
    char *token;

    //Split buffer into tokens.
    token=strtok(buf, " :\r\n");
    int i=0;            //Keep track of split tokens.
    while(token)
    {
        token=strtok(NULL, " :\r\n");
        
        i++;            
        if(i&1)         //Due to response having a set format, position of desired token is known.
            break;
    }

    printf("%-17s : %s\n", "Status", token);    

    return;
}


/*
 * 1. Function to display uptime received from PIC.
 * 2. Input argument: buf = rx_buffer(containing PIC response received over UART)
 */
void display_uptime(char *buf)
{
    char *token;
    unsigned int time;

    printf("%-17s :", "Uptime");

    //Split buffer into tokens.
    token=strtok(buf, " :\r\n");
    int i=0;   //Keep track of split tokens.  
    while(token)
    {
        token=strtok(NULL, " :\r\n");
        time=(unsigned int)strtoul(token, NULL, 10); //Extract number from string.
        
        i++;
        
        /*
         * 1. Due to response having a set format, position of desired token is known.
         * 2. Time is displayed in HH hr: MM min : SS secs format.
         */
        switch(i)    
        {
            case 1:
                printf(" %-2d hr ", time);
                break;

            case 2:
                printf(" %-2d min ", time);
                break;

            case 3:
                printf(" %-2d secs ", time);
        }
    }
    return;
}


/*
 * 1. Function to display temperature threshold value received from PIC.
 * 2. Input argument: buf = rx_buffer(containing PIC response received over UART)
 */
void display_glimit(char *buf)
{
    char *token;

    //Split buffer into tokens.
    token=strtok(buf, " :\r\n");
    int i=0;        //Keep track of split tokens.
    while(token)
    {
        token=strtok(NULL, " :\r\n");
        
        i++;
        if(i==2)    //Due to response having a set format, position of desired token is known.
            break;
    }

    float temp=strtof(token, NULL); //Extract number from string.

    printf("%-17s : %g degC\n", "Temperature limit", temp);

    return;
}

//Function to display response received for command sent to set temeprature threshold value. 
void display_slimit(char *buf)
{
    if(strcmp(buf, "DONE\n"))
        printf("Error: updation failed; reset node device.");
    else
        printf("Temperature limit value updated successfully.");

    return;
}


/*
 * 1. Function to display list of all displayable parameters received from PIC.
 * 2. Input argument: buf = rx_buffer(containing PIC response received over UART)
 */
void display_all(char *buf)
{
    char *temp=calloc(strlen(buf)+1,1);
    if(!temp)
    {
        printf("Error: memory allocation failed.\nCommand aborted.");
        return;
    }
    char *next_line=temp;

    strcpy(next_line, buf);

    int index=0;

    /*
     * 1. A single continuous buffer is received, wherein info related to each parameter is separated by \n.
     * 2. Split this buffer into individual tokens based on \n position using strchr function. 
     * 3. These individual token are further split using strtok to extract desired parameters.
     */
    while (next_line && *next_line != '\0') 
    {
        char *line_start = next_line;
        char *line_end = strchr(line_start, '\n'); //Split buffer.

        if (line_end) 
            *line_end = '\0';   //Terminate the current line.
      
        /*
         * 1. Due to response having a set format, position of desired term to be extracted is known.
         * 2. Individual lines are sent to associated functions for value extraction.
         * 3. volt_index, etc. are macros to represent lines containing associated parameters.
         */
        switch(index)
        {
            case volt_index:
                display_voltage(line_start);
                break;
        
            case temp_index:
                display_temp(line_start);
                break;

            case status_index:
                display_status(line_start);
                break;

            case limit_index:
                display_glimit(line_start);
                break;

            case time_index:
                display_uptime(line_start);

        }
        index++;

        if (line_end) 
            next_line = line_end + 1;  //Track where the next line begins.
        else 
            next_line = NULL;          //Last line reached.
    }    
    free(temp);

    return;
}

/*
 * 1. Function to display list of all displayable parameters received from PIC.
 * 2. Input argument: buf = rx_buffer(containing PIC response received over UART)
 */
void display_log(char *buf)
{
    if(!strcmp(buf, "NO LOGS PRESENT\n"))
    {
        printf("No log entries found.");
        return;
    }
    
    char *temp=calloc(rx_buffer_size,1);
    if(!temp)
    {
        printf("Error: memory allocation failed.\nCommand aborted.");
        return;
    }
    char *next_line=temp;
    strcpy(next_line, buf);

    /*
     * 1. A single continuous buffer is received, wherein each log entry is separated by \n.
     * 2. Split this buffer into individual tokens based on \n position using strchr function. 
     * 3. These individual token are further split using strtok to extract status & uptime values.
     */
    int j=1; 
    while (next_line && *next_line != '\0') 
    {
        char *line_start = next_line;
        char *line_end = strchr(line_start, '\n');
        unsigned int time;

        if (line_end) 
            *line_end = '\0';    //Terminate the current line.
         
        printf("%-10s  %d |  uptime ", "Log entry", j);
        j++;
        
        int i=0;
    
        char *token;
        token=strtok(next_line, " :\r\n");
        while(token)
        {
            time=(unsigned int)strtoul(token, NULL, 10);
            switch(i)   //Due to response having a set format, position of desired token is known.
            {
                case 1:
                    printf(" %-2d hr ", time);
                    break;

                case 2:
                    printf(" %-2d min ", time);
                    break;

                case 3:
                    printf(" %-2d secs  ", time);
                    break;

                case 4:
                    printf("%s : %s\n", "|  state transition to ", token);
            }
            
            token=strtok(NULL, " :\r\n"); 
            i++;
        }

        if (line_end) 
            next_line = line_end + 1;  //Track where the next line begins.
        else 
            next_line = NULL;          //Last line reached.
    }    
    free(temp);
    return;
}

/*
 * 1. Function to display response for clear_log command sent to PIC.
 * 2. Input argument: buf = rx_buffer(containing PIC response received over UART)
 */
void clear_log(char *buf)
{
    if(strcmp(buf, "DONE\n"))
        printf("No log entries present to be cleared.");
    else
        printf("Log entries cleared.");

    return;
}

















/*
void display_all(char *buf)
{
    char *token, *get_all[all_element_count];

    int i=0, j=0;

    token=strtok(buf, " :\r\n");
    
    while(token)
    {
        token=strtok(NULL, " :\r\n");
        
        i++;
        if( (i&1) && (j<all_element_count-2) )
        {
            get_all[j]=malloc(strlen(token)+1);
            
            strcpy(get_all[j], token);

            j++;
        }
        if(i==8)
        {
            get_all[j]=malloc(strlen(token)+1);
            
            strcpy(get_all[j], token);

            j++;
        }
        if( (i>8) && !(i&1) )
        {
            get_all[j]=malloc(strlen(token)+1);
            
            strcpy(get_all[j], token);
        }
    }

    float voltage=strtof(get_all[volt_index], NULL);
    printf("%-17s : %g V\n", "Voltage", voltage);
    
    float temp=strtof(get_all[temp_index], NULL);
    printf("%-17s : %g degC\n", "Temperature", temp);

    printf("%-17s : %s\n", "Status", get_all[status_index]);

    float limit=strtof(get_all[limit_index], NULL);
    printf("%-17s : %g degC\n", "Temperature limit", limit);

    unsigned int time=strtoul(get_all[time_index], NULL, 10);
    printf("%-17s : %d secs", "Uptime", time);

    for(int k=0; k<all_element_count; k++)
        free(get_all[k]);

    return;
}

*/


/*
void display_log(void)
{
    if(!strcmp(rx_buffer, "NO LOGS PRESENT\n"))
    {
        printf("No log entries found.");
        return;
    }
    
    char *token;
    int i=1;
    char temp[log_entry_size]={0};

    token=strtok(rx_buffer, "\r\n");

    while(token)
    {   
        printf("%-10s  %d:   ", "Log entry", i);
        i++;

        //strcpy(temp, token);
        //print_log_entry(temp);
        
        token=strtok(NULL, "\r\n");
    }
    return;
}
*/

/*
void print_log_entry(char *buf)
{
    //char temp[log_entry_size]={0};
    //strcpy(temp, buf);

    char *token=strtok(buf, " :\r\n");
    int i=0;
    while(token)
    {
        token=strtok(NULL, " :\r\n");
        
        i++;
        if(i==1)
        {
            unsigned int time=strtoul(token, NULL, 10);

            printf("%s : %d secs    ", "Uptime", time);
        }
        if(i==2)
        {
            printf("%s : %s\n", "state transition to ", token);
        }
    }
    return;
}

*/

/*1st cut
void command_receive(void)
{
    if(cmd_curr==CMD_NONE)
        return;

    int i=0;
    while(1)
    {
        if(i==rx_buffer_size)
        {
            printf("Error: command receive buffer overflow");
            serial_close();
            exit(1);
        }
        
        rx_buffer[i]=*( serial_receive() );

        if(rx_buffer[i]=='E')
        {
            if( cmd_end_chk(i) )
            {
                rx_buffer[i]='\0';
                break;
            }
            i=i+cmd_end_str_size-1;
        }
        else
            i++;
    }
    return;
}

int cmd_end_chk(int i)
{
    char buf[cmd_end_str_size]={0};
    buf[0]='E';

    for(int j=1; j<cmd_end_str_size-1; j++)
        buf[j]=*( serial_receive() );

    if(!strcmp(buf, "END\n"))  
        return 1;
    
    for(int j=1; j<cmd_end_str_size-1; j++)
    {
        i++;
        rx_buffer[i]=buf[j];
    }
    return 0;
}

*/
