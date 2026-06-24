#ifndef CONFIG_H
#define	CONFIG_H

#define _XTAL_FREQ 20000000UL   //Clock frequency: 20 MHz.

#pragma config FOSC = HS        //Select High speed crystal oscillator.
#pragma config WDTE = OFF       //Watchdog timer disabled.
#pragma config PWRTE = ON       //Power-up Timer (PWRT) enabled.
#pragma config BOREN = ON       //Brown-out Reset (BOR) enabled.
#pragma config LVP = OFF        //Low-Voltage Programming (LVP) disabled.
#pragma config CPD = OFF        //Data EEPROM Memory Code Protection disabled.
#pragma config WRT = OFF        //Flash Program Memory Write Enable disabled.
#pragma config CP = OFF         //Flash Program Memory Code Protection disabled.

#endif	

