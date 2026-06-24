#ifndef LOG_H
#define	LOG_H

#define size 10

/*
 * Log entry consists of the following:
 * 1. State to which transition has occurred i.e. 'OVERHEAT' or 'OK'.
 * 2. Time(in seconds) at which the state transition occurred.
 */
typedef struct LOG
{
    char state[size];
    unsigned long time;
}LOG;

void log_read(void);

void log_write(char *buf);

void log_clear(void);

#endif	

