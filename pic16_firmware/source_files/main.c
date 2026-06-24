#include "config.h"
#include "uart.h"
#include "command.h"
#include "adc.h"
#include "eeprom.h"
#include "timer.h"
#include "log.h"
#include "sensor.h"

/*
 * 1. Below declared varriables used to track state transition from 'OK' to 'OVERHEAT' or vice-versa.
 * 2. Log entries are made on state transition.
 */
extern volatile bool status_ok;
extern volatile bool status_heat;
extern bool curr;
extern bool prev;


void main(void) 
{   
    uart_init();    //UART setup.
    
    timer1_init();  //Timer 1 setup.
   
    adc_init();     //ADC setup.
    
    eeprom_init();  //EEPROM setup.
    
    putstring_uart((unsigned char* )"Welcome!!\n");
    
    while(1)
    {
        /*
         * 1. COntinuously monitor if state transition happens or not.
         * 2. State transition is detected, make the associated log entry.
         */
        status_monitor();
        
        if(status_ok)
        {
            log_write("OK");
            status_ok=false;
            prev=false;
        }
        
        if(status_heat)
        {
            log_write("OVERHEAT");
            status_heat=false;
            prev=true;
        }
        
        //Continuously monitor for any received commands.
        command_receive();
    }
        
    return;
}
