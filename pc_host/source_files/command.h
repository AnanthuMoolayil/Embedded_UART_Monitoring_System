#ifndef CMD_H
#define CMD_H

/*
 * Commands are sent to PIC and results are received back from PIC.
 * A command table is used; made up of data of the type struct COMMAND consisting of the following members:
 * 1. cmd_name : User input to send a command.      
 * 2. send_cmd : Pointer to function responsible for sending command.
 * 3. exec_cmd : Pointer to function responsible for processing & displaying command results.
 */
typedef struct COMMAND
{
    char *cmd_name;
    void (*send_cmd)(void);
    void (*exec_cmd)(char *buf);

}COMMAND;

//Functions to send commands to PIC. 

int cmd_select(char *buf);

void send_volt(void);

void send_temp(void);

void send_status(void);

void send_uptime(void);

void send_gtlimit(void);

void send_all(void);

void send_stlimit(void);

void send_log(void);

void send_clog(void);

//Function to receive response from PIC.
void command_receive(void);


//Function for processing & displaying command results.

void display_result(void);

void display_voltage(char *buf);

void display_temp(char *buf);

void display_status(char *buf);

void display_uptime(char *buf);

void display_glimit(char *buf);

void display_slimit(char *buf);

void display_all(char *buf);

void display_log(char *buf);

void clear_log(char *buf);

#endif