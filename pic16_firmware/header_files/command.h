#ifndef COMMAND_H
#define	COMMAND_H

void command_receive(void);

void parse_command(void);


void execute_command(void);

void exec_voltage(void);

void exec_temp(void);

void exec_tlimit(void);

void exec_uptime(void);

void exec_status(void);


int number_fetch(void);

void addint_tostring(int num);

void ultoa(char *buf, unsigned long num);


#endif	

