#include <stdio.h>
#include <stdlib.h>
#include <libserialport.h>
#include <string.h>
#include "serial.h"
#include "config.h"

#define timeout_ms 100          //Timeout for UART transmit and receive

char buffer;                    //For storing character received over UART.

extern Config_Info cfg;         //Contains necessary communication parameters extracted from config_file.txt . 

struct sp_port *port=NULL;      //To accesss a COM port.

struct sp_port **port_list;     //To point to a list of COM ports.

enum sp_return result;          //To hold return results of function execution.


//Function to configure & setup UART communication.
void serial_open(void)          
{
    result=sp_list_ports(&port_list);   //Fetch list of available COM ports.
    if(result!=SP_OK)
        serial_error_handle(no_port_err);    //If no available ports present.

    //Check if port in config_file present among any of the available COM ports.
    for(int i=0; port_list[i]!=NULL; i++)
    {
        /*
         * Compare the COM port names between vaialble port and port name in config_file.
         * sp_get_port_name() used for fetching port names.
         */
        if( strcmp( sp_get_port_name(port_list[i]), cfg.port_name )==0 )
        {
            port=port_list[i];
            break;
        }
    }
    if(!port)
        serial_error_handle(port_match_err);      //If COM port mentioned in config_file is not available. 

    result=sp_open(port, SP_MODE_READ_WRITE);   //Open COM port for communication. 
    if(result!=SP_OK)
        serial_error_handle(port_open_err);       //If COM port opening failed.
    
    //Set the communication parameters.    
    sp_set_baudrate(port, cfg.baud_rate);   //Baud rate extracted from config_file.
    sp_set_bits(port, 8);
    sp_set_parity(port, SP_PARITY_NONE);
    sp_set_stopbits(port, 1);
    sp_set_flowcontrol(port, SP_FLOWCONTROL_NONE);

    sp_flush(port, SP_BUF_BOTH);             //Flush any stale data remaianing over the UART lines.

    return;
}

/*
 * 1. Function to transmit data over UART.
 * 2. Input string buf contains the data to be transmitted.
 */
void serial_send(char *buf)
{
    result=sp_blocking_write(port, buf, strlen(buf), timeout_ms); //Returns number of bytes written successfully.

    if(result<=0)
        serial_error_handle(send_err);

    return;
}

//Function to receive data over UART. 
char* serial_receive(void)
{    
    result=sp_blocking_read(port, &buffer, 1, timeout_ms);  //'buffer' is a global vriable. Function returns number of bytes read successfully.
    
    if(result<=0)  
        serial_error_handle(receive_err);
    
    return &buffer;
}

//Function for terminating communication.
void serial_close(void)
{
    sp_close(port);                        //Close opened COM port.

    sp_free_port_list(port_list);   //Free the COM port list.

    free(cfg.port_name);           //Free memory for config_file data storage.

    return;
}

//Flush any stale data remaianing over the UART lines.
void serial_flush(void)
{
    sp_flush(port, SP_BUF_BOTH);

    return;
}

//Function to handle errors during setup & operation of serial communication.
void serial_error_handle(serial_error err)
{
    switch(err)
    {
        case no_port_err:
            printf("Error: no available COM ports found.");
            exit(1);
        
        case port_open_err:
            sp_free_port_list(port_list);
            printf("Error: COM port opening failed.");
            exit(1);
            
        case port_match_err:
            sp_free_port_list(port_list);
            printf("Error: configured COM port not available.");
            exit(1);

        case send_err:
            serial_close();
            printf("Error: serial transmission failed.");
            exit(1);

        case receive_err:
            serial_close();
            printf("Error: serial receive failed.");
            exit(1);

    }

}

