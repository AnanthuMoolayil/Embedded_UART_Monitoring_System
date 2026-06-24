/*
 * 1. Two ADC channels used for reading temperature & voltage readings.
 * 2. Conisder the Picgenios board potentiometers(P1 & P2) as temperature & voltage sensors.
 * 2. port A pins 0 & 1 are analog channels 0(AN0) and 1(AN1).  
 */

#include <xc.h>
#include "adc.h"

#define _XTAL_FREQ 20000000UL   //Clock frequency: 20 MHz.

#define Tacq_us 20              //ADC accquisition time.

//ADC configuration/setup.
void adc_init(void)
{
    //Set port A pins 0 & 1 as inputs.
    TRISA0=1;
    TRISA1=1;  
    
    //Set A/D conversion clock(Fosc/32).
    ADCON1bits.ADCS2=0;
    ADCON0bits.ADCS=0b10;
    
    //A/D port configuration. 
    ADCON1bits.PCFG=0b0100;
    
    //Set result format as right justified.
    ADCON1bits.ADFM=1;
    
    //ADC converter module is powered ON.
    ADCON0bits.ADON=1;
}

/*
 * 1. Function to fetch ADC output.
 * 2. 'ch' variable of enum ADC_CH is for channel selection. 
 */
unsigned int adc_read(ADC_CH ch)
{
    switch(ch)
    {
        case VOLTAGE:               //For voltage sensor readings.
            ADCON0bits.CHS=0b000;   //Channel selection.
      
            __delay_us(Tacq_us);    //Wait for sensor reading to be captured properly.
            
            ADCON0bits.GO=1;
            while(ADCON0bits.GO);   //Wait for ADC conversion to finish.
            
            /*
             * 1. Return ADC result stored in ADRESH & ADRESL registers.
             * 2. Note that result is in right justified format.
             */
            return (unsigned int)( ( (unsigned int)ADRESH<<8 ) | ADRESL );  
            
        case TEMP:                  //For temperature sensor readings.
            ADCON0bits.CHS=0b001;
      
            __delay_us(Tacq_us);
            
            ADCON0bits.GO=1;
            while(ADCON0bits.GO);
            
            return (unsigned int)( ( (unsigned int)ADRESH<<8 ) | ADRESL ); 
    }
}
