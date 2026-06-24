#include <string.h>
#include "uptime.h"
#include "command.h"

extern volatile unsigned long uptime;

//Function to change uptime into HH:MM:SS format 
void format_uptime(char *time_buffer)
{
    char buf[11]={0};
    
    unsigned long hr=uptime/(60*60);
    
    unsigned char min= (unsigned char)( (uptime/60) - (hr*60) ); 
    
    unsigned char sec= (unsigned char) ( uptime - ( (unsigned long)min*60 + hr*60*60 ) );
    
    ultoa(buf, hr);
    strcat(time_buffer, buf);
    strcat(time_buffer, ":");
    
    ultoa(buf, (unsigned long)min);
    strcat(time_buffer, buf);
    strcat(time_buffer, ":");
    
    ultoa(buf, (unsigned long)sec);
    strcat(time_buffer, buf);
    
    return;
}

/*
 * 1. Function to change time(in seconds) into HH:MM:SS format
 * 2. Input argument is 'time'. 
 * 3. Output time format string stored in 'time_buffer'. 
 */
void log_format_uptime(char *time_buffer, unsigned long time)
{
    char buf[11]={0};
    
    unsigned long hr=time/(60*60);
    
    unsigned char min= (unsigned char)( (time/60) - (hr*60) ); 
    
    unsigned char sec= (unsigned char) ( time - ( (unsigned long)min*60 + hr*60*60 ) );
    
    ultoa(buf, hr);
    strcat(time_buffer, buf);
    strcat(time_buffer, ":");
    
    ultoa(buf, (unsigned long)min);
    strcat(time_buffer, buf);
    strcat(time_buffer, ":");
    
    ultoa(buf, (unsigned long)sec);
    strcat(time_buffer, buf);
    
    return;
}