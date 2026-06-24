#include <string.h>
#include "log.h"
#include "uart.h"
#include "command.h"
#include "uptime.h"

#define log_size 5          //Maximum log entries possible.

LOG log[log_size]={0};      //Array 'log' of type stuct LOG, defined in log.h

unsigned char r_index=0;    //Index position to start reading log entries from. 
unsigned char w_index=0;    //Index position to write new log entry into.
unsigned char count=0;      //Keep count of elements in 'log' array.

extern volatile unsigned long uptime;   //Seconds elapsed since PIC power ON.

/*
 * 1. Function to add a new log entry.
 * 2. Circular buffer is implemented.
 * 3. If log buffer is full, the oldest entry is overwritten.
 * 4. Input argument: 'OVERHEAT' or 'OK'.
 */
void log_write(char *buf)
{
    //When log buffer is full;
    if(count>=log_size-1)
    {
        strcpy( log[w_index].state, buf );
        log[w_index].time=uptime;
        
        //Index updation incase of rollover to first index position.
        if(w_index==log_size-1)
        {
            w_index=0;
            r_index=0;
            return;
        }
        //Index updation.
        w_index++;
        r_index=w_index;
        return;
    }
    //Log write operation when log buffer is not yet full.
    strcpy( log[w_index].state, buf );
    log[w_index].time=uptime;

    //Index updation.
    w_index++;
    count++;
        
    return;
}

/*
 * 1. Function to display log entries.
 * 2. Circular buffer is implemented.
 * 3. If log buffer is full, start reading from the position of the oldest entry.
 */
void log_read(void)
{
    //Incase of no log entries.
    if(count==0)
    {
        putstring_uart((unsigned char*)"NO LOGS PRESENT\n");
        return;
    }
    
    char time_buf[time_size]={0};
    
    unsigned char i=r_index;
    
    
    unsigned char k;
    if(count>=log_size-1)
        k=count;
    else
        k=count-1;
    
    //No. of times the loop executes, depends on log buffer being full or not.
    for(int j=0; j<=k; j++)
    {
        putstring_uart((unsigned char*)"UPTIME: ");
        
        log_format_uptime(time_buf, log[i].time);   //Format the state transition time to HH:MM:SS format.
        putstring_uart((unsigned char*)time_buf);
        time_buf[0]='\0';
        
        
        putstring_uart((unsigned char*)"   ");
        
        putstring_uart((unsigned char*)log[i].state);//Display the state transition.
        
        putchar_uart((unsigned char)'\n');
        
        if(i==log_size-1)   //Rollover to first position on reaching last position of buffer.       
        {
            i=0;
            continue;
        }
        i++;
    }
}

//Function to clear log entries.
void log_clear(void)
{
    //Incase there are no log entries to clear.
    if(count==0)
    {
        putstring_uart((unsigned char*)"NO LOGS PRESENT\n");
        return;
    }
    
    //Reset all index positions & log entry count.
    count=0;
    r_index=0;
    w_index=0;
    
    putstring_uart((unsigned char*)"DONE\n");
    
    return;
}