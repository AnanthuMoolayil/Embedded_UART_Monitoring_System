//Temeprature threshold value is set to have persistent storage.

#include <xc.h>
#include <stdbool.h>
#include "eeprom.h"
#include "uart.h"


#define default_limit 700   //Initial temeprature threshold  value on loading firmware for the first time.  

#define end_str "***\n"     //Character sequence used to terminate commands. 

extern int t_limit;         //Temperature threshold value storage.


/*
 * 1. Function to read register data.
 * 2. Input argument is address of register from which data is to be read.
 * 3. Returns data at the register.
 */
unsigned char my_eeprom_read(unsigned char add)
{
    EEADR=add;              
    
    EECON1bits.EEPGD=0;     //Set to access data memory.
    
    EECON1bits.RD=1;        //Initiate EEPROM read.
    
    while(EECON1bits.RD);   //Wait till read operation is complete.
    
    return EEDATA;
    
}

/*
 * 1. Function to write single byte of data to specified register.
 * 2. Input arguments: a. Data to be written  b. Address of register to which data is to be written.
 * 3. Returns 'true' or 'false' depending on write operation being successful(false) or not(true). 
 */
bool my_eeprom_write(unsigned char data, unsigned char add)
{
    EEADR=add;
    
    EEDATA=data;
    
    EECON1bits.EEPGD=0;     //Set to access data memory.
    
    EECON1bits.WREN=1;      //Enable EEPROM write.
    
    INTCONbits.GIE = 0;     //Disable all interrupts. 
    
    EECON2=0x55;
    EECON2=0xAA;
    
    EECON1bits.WR=1;        //Initiate EEPROM write.
    
    while(EECON1bits.WR);   //Wait till write operation is complete.
    
    INTCONbits.GIE = 1;     //Enable all interrupts. 
    
    EECON1bits.WREN=0;      //Disable EEPROM write.
    
    if(EECON1bits.WRERR)    //Check if error occurred during write operation.
        return true;
    
    return false;
}


/*
 * 1. Function to read an integer(2 bytes) from EEPROM memory.
 * 2. Input argument is address of registers from which data will to be read.
 * 3. Returns read integer.
 */
int my_eeprom_read_int(unsigned int add)
{
    int data;
    
    //Read data at registers & merge them before returning.
    
    unsigned char char_add = add & 0xFF;
    data=my_eeprom_read( char_add) & 0x00FF; 
        
    char_add = add>>8 & 0xFF;
    int temp=my_eeprom_read(char_add) & 0x00FF;
    data=(int)( (((unsigned int)temp)<<8 ) | (unsigned int)data );
       
    return data;
}

/*
 * 1. Function to write integer data(2 bytes) to registers.
 * 2. Input arguments: a. Data to be written  b. Address of registers to which data is to be written(in integer format).
 * 3. Returns 'true' or 'false' depending on write operation being successful(false) or not(true). 
 */
bool my_eeprom_write_int(int data, unsigned int add)
{
    /*
     * Suppose data=0x1234 & add=0x5678;
     * Then data=0x12 stored at address 0x56
     *      data=0x34 stored at address 0x78
     */
    
    //Handling Lower byte of  data.
    unsigned char char_data = data & 0xFF;
    unsigned char char_add = add & 0xFF; 
    
    if( my_eeprom_write(char_data, char_add) )      //Using my_eeprom_write() to handle individual bytes.
        return true;
    
    //HAndling Higher byte of data.
    char_data = ( (unsigned int)data>>8 ) & 0xFF;
    char_add = add>>8;
    
    if( my_eeprom_write(char_data, char_add) )      //Using my_eeprom_write() to handle individual bytes.
        return true;
    
    
    return false;
}

//Function to initialize temperature threshold storage(t_limit) with a value on PIC startup.
void eeprom_init(void)
{
    /*
     * 1. On startup, check if a particular address(in this case, 0x00) has a pre-determined value.
     * 2. If check comes out true, it means value of t_limit at EEPROM memory is valid and can be loaded into t_limit.
     * 3. If check comes out false, it means t_limit and EEPROM memory needs to be set to defualt value.    
     */    
    unsigned char init_chk=my_eeprom_read(0x00);
    
    if(init_chk==0xBD)
    {
        t_limit=my_eeprom_read_int(0x0201);
        
        return;
    }    
  
    if(my_eeprom_write(0xBD, 0x00))
    {
        putstring_uart((unsigned char* )"eeprom load failed; restart!\n");
        putstring_uart((unsigned char*)end_str);
        return;
    }
    
    if(my_eeprom_write_int(default_limit, 0x0201))
    {
        putstring_uart((unsigned char* )"eeprom load failed; restart!\n");
        putstring_uart((unsigned char*)end_str);
        return;
    }
 
    t_limit=default_limit;
    
    return;
}

