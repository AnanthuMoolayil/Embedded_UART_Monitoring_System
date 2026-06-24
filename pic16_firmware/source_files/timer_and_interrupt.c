#include <xc.h>
#include <stdbool.h>


// Below mentioned variables used to capture & subsequently trigger interrupt when state transitions from 'OK' to 'OVERHEAT' or vice-versa.
extern bool curr;
extern bool prev;
volatile bool status_ok;
volatile bool status_heat;

//Variables for uptime tracking.
volatile unsigned long uptime=0;
volatile unsigned char timer1_ovf = 0;

//Timer 1 configuration/setup.
void timer1_init(void) 
{
    T1CONbits.TMR1CS = 0;   //Set clock source: internal (Fosc/4).
    T1CONbits.T1CKPS = 3;   //Prescaler: 1:8.
    
    //Preload value.
    TMR1H = 0x00;           
    TMR1L = 0x00;           
    
    PIR1bits.TMR1IF = 0;    // Clear Timer1 interrupt flag.
    PIE1bits.TMR1IE = 1;    // Enable Timer1 interrupt.
    INTCONbits.PEIE = 1;    // Enable Peripheral interrupts.
    INTCONbits.GIE = 1;     // Enable Global interrupts.
    
    T1CONbits.TMR1ON = 1;   // Start Timer1.
}

void __interrupt() ISR(void) 
{    
    /*
     * 1. Timer 1 used for counting seconds.
     * 2. Configure the timer in a manner wherein 9 full and 1 partial overflow results 1 second elapsing
     * 3. 'timer1_ovf' indicates timer 1 overflow. 
     * 4. 'uptime' increments when a second elapses. 
     */
    if (PIR1bits.TMR1IF == 1)   //Interrupt on timer 1 overflow.
    {
        PIR1bits.TMR1IF = 0;    
        timer1_ovf++;           
        
        if (timer1_ovf < 9)         //Full overflow. 
        {
            TMR1H = 0x00;
            TMR1L = 0x00;
        } 
        else if (timer1_ovf == 9)   //Partial overflow. 
        {
            TMR1H = 0x76;       
            TMR1L = 0x8E;       
        } 
        else 
        {
            uptime++; 
            timer1_ovf = 0;    
            
            TMR1H = 0x00;
            TMR1L = 0x00;
        }
    }
     
   
    if( prev==false && curr==true ) //Interrupt on transition from 'OK' to 'OVERHEAT'.
        status_heat=true;
    
    if( prev==true && curr==false ) //Interrupt on transition from 'OVERHEAT' to 'OK'.
        status_ok=true;
    
}




