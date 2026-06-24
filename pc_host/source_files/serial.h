#ifndef SERIAL_H
#define SERIAL_H

//For error handling in serial communication.
typedef enum
{
    no_port_err,
    port_open_err,
    port_match_err,
    send_err,
    receive_err
}serial_error;

void serial_open(void);

void serial_send(char *buffer);

char* serial_receive(void);

void serial_close(void);

void serial_flush(void);

void serial_error_handle(serial_error err);

#endif
