#include <xc.h>
#include <stdbool.h>
#include "uart.h"

#define UART_BAUD 9600  //Set baud rate.

#define _SPBRG 129      //Value to be loaded in SPBRG register to acheive desired baud rate.

//Function to configure/setup UART.
void uart_init(void)
{
    //Setting pins 6 & 7 of port C as input to reserve them for UART.
    TRISCbits.TRISC6=1;
    TRISCbits.TRISC7=1;
    
    TXSTAbits.BRGH = 1; //Set baud rate mode as 'high speed'.
    SPBRG = _SPBRG;
    
    RCSTAbits.SPEN = 1; //Configures 6 & 7 pins of port C as serial port pins.
    
    TXSTAbits.TXEN = 1; //Enable UART transmit.
    
    RCSTAbits.CREN = 1; //Enable UART receive.
    
    return;
}

/*
 * Function to capture single character received over UART.
 * Returns 'true' on successful capture, else 'false'. 
 */
bool getchar_uart(unsigned char *c)
{
    //To handle receive buffer overflow.
    if(RCSTAbits.OERR==1)
    {
        //Reset UART receive.
        RCSTAbits.CREN=0;
        
        RCSTAbits.CREN=1;
        
        return false;
    }
    
    if(PIR1bits.RCIF)   //Check RCIF bit to determine if any data is received.
    {
        //To handle framing error.
        if(RCSTAbits.FERR==1)
        {
            *c=RCREG;   //RCREG register contains data received over UART.

            return false;
        }
        *c=RCREG;
        
        return true;
    }
    return false;
    
}

//Function to transmit single character over UART.
void putchar_uart(unsigned char c)
{
    while(!PIR1bits.TXIF);  //Check whether UART transmission can be started.
        
    TXREG=c;
    
    return;
}

//Function to transmit character string over UART.
void putstring_uart(unsigned char *arr)
{
    unsigned char i=0;
    
    while(arr[i]!='\0')
    {
        putchar_uart(arr[i]);
        i++;
    }    
    return;
}
