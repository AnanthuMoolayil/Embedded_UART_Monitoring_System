#include <xc.h>
#include <string.h>
#include <stdbool.h>
#include "uart.h"
#include "command.h"
#include "adc.h"
#include "sensor.h"
#include "eeprom.h"
#include "log.h"
#include "uptime.h"

#define _XTAL_FREQ 20000000UL   //Clock frequency: 20 MHz

#define max_command_len 32      //Size of buffer used to store received input commands.

static unsigned char command_buffer[max_command_len];   //Buffer used to store received input commands.
static unsigned char command_index;                     //Position wherein next received character is to be stored in command_buffer.

static unsigned char output_buffer[max_command_len];    //Buffer used to store results to be displayed.
static unsigned char output_index;                      //Position wherein next received character is to be stored in output_buffer.

static unsigned char space_index=(unsigned char)max_command_len;    //To capture ' ' position in received input commands.

#define end_str "***\n"         //Character sequence used to terminate commands. 

static bool command_overflow;   //Flag to determine when input exceeds command_buffer size.

#define t_uplimit 1000          //Temperature threshold upper limit.

#define t_lowlimit -200         //Temperature threshold lower limit.

extern int t_limit;             //Temperature threshold.

static int number;              //To store number extracted from input commands.

extern volatile unsigned long uptime;  

//To link input command to associated functions to further process & output results.
typedef enum
{
    CMD_NONE,
    CMD_VOLTAGE,  
    CMD_TEMP,
    CMD_STATUS,
    CMD_ALL,
    CMD_SET_TLIMIT,
    CMD_GET_TLIMIT,
    CMD_TIME,
    CMD_GLOG,
    CMD_CLOG,
    CMD_T_OR,
    INVALID_DECIMAL,
    INVALID_NUM,       
}CMD_TYPE;

static CMD_TYPE curr_cmd;

//Function to handle command input.
/*
 * 1. Function to handle command input.
 * 2. Continue to receive characters one by one until newline character is encountered.
 * 3. Received characters stored in command_buffer in lowercase format.
 */
void command_receive(void)
{
    while(1)
    {
        if( !getchar_uart( &command_buffer[command_index] ) )   //Check if character received successfully over UART. 
            return;
    
        /*
         * When input exceeds command_buffer size, following steps are taken:
         * 1. command_overflow flag is set.
         * 2. Keep receiving data until newline character is encountered.
         * 3. Discard received data and print error message.
         * 4. Reset flag and command_buffer index.
         */
        if(command_overflow)
        {
            if( command_buffer[command_index] == '\n' )
            {
                command_overflow=0;
                command_index=0;
                putstring_uart((unsigned char* )"Invalid command: too long!\n");
                return;
            }
            if( command_index==(max_command_len-1) )
            {
                command_index=0;
                continue;
            }
            command_index++;
            continue; 
        }
        if( ( command_index==(max_command_len-1) ) && ( command_buffer[command_index] != '\n' ) )
        {
            command_overflow=1;
            command_index=0;
            continue;
        }
        
        /*
         * When space is encountered within input command, consider the following:
         * 1. Input might be command with number, for eg. for setting temperature threshold to number specified in input.
         * 2. Capture the index at which space occurs for extracting input number later.
         * 3. Replace space with '\0' so as to check whether the alphabetical command part is correct. 
         */
        if( command_buffer[command_index] == ' ' )
        {
            command_buffer[command_index] = '\0';
            
            space_index=command_index;
            
            command_index++;
            
            continue;
        }
        
        //Certain communication terminals may handle pressing Enter(\n) as '\r\n'. 
        if( command_buffer[command_index] == '\r' )
            continue;
        
        if( command_buffer[command_index] == '\n' )
        {
            if( command_index==0 )  //On pressing Enter with no input.
                return;
            
            command_buffer[command_index] = '\0';   //Terminate received data with '\0' to perform string operations.

            parse_command();    //To determine command to be executed based on input.
            
            execute_command();  //To execute & display command operations

            //Reset buffer index values.
            command_index=0; 
            space_index=(unsigned char)max_command_len;

            return;

        }   
        //Convert received characters to lower case.
        if( ( command_buffer[command_index] >= 'A' ) && ( command_buffer[command_index] <= 'Z' ) )
            command_buffer[command_index] = command_buffer[command_index] + 32;
        
        command_index++;
    }    
}

//Function for determining command to be executed based on command_buffer data.
void parse_command(void)
{
    curr_cmd=CMD_NONE;
    
    if( !strcmp( (char *)command_buffer, "get_voltage" ) )
    {
        curr_cmd=CMD_VOLTAGE;
        
        return;
    }
    if( !strcmp( (char *)command_buffer, "get_temp" ) )
    {
        curr_cmd=CMD_TEMP;
        
        return;
    }
    if( !strcmp( (char *)command_buffer, "get_status" ) )
    {
        curr_cmd=CMD_STATUS;
        
        return;
    }
    if( !strcmp( (char *)command_buffer, "get_tlimit" ) )
    {
        curr_cmd=CMD_GET_TLIMIT;
        
        return;
    }
    if( !strcmp( (char *)command_buffer, "get_time" ) )
    {
        curr_cmd=CMD_TIME;
        
        return;
    }
    if( !strcmp( (char *)command_buffer, "get_log" ) )
    {
        curr_cmd=CMD_GLOG;
        
        return;
    }
    if( !strcmp( (char *)command_buffer, "clear_log" ) )
    {
        curr_cmd=CMD_CLOG;
        
        return;
    }
    if( !strcmp( (char *)command_buffer, "get_all" ) )
    {
        curr_cmd=CMD_ALL;
        
        return;
    }
    
    //Command consists of input number
    if( ( !strcmp( (char *)command_buffer, "set_tlimit" ) ) && (space_index!=(unsigned char)max_command_len) )
    {
        number=number_fetch(); //Fetch input number
        
        return;
    }
}

//Function for executing and displaying operation results.
void execute_command(void)
{
    switch(curr_cmd)
    {       
        case CMD_VOLTAGE:
            exec_voltage();
            putstring_uart((unsigned char*)end_str);
            return;
            
        case CMD_TEMP:
            exec_temp();
            putstring_uart((unsigned char*)end_str);
            return;
            
        case CMD_STATUS:
            exec_status();
            putstring_uart((unsigned char*)end_str);
            return;
            
        case CMD_GET_TLIMIT:
            exec_tlimit();
            putstring_uart((unsigned char*)end_str);
            return;
            
        case CMD_TIME:
            exec_uptime();
            putstring_uart((unsigned char*)end_str);
            return;
            
        case CMD_GLOG:
            log_read();
            putstring_uart((unsigned char*)end_str);
            return;
            
        case CMD_CLOG:
            log_clear();
            putstring_uart((unsigned char*)end_str);
            return;
            
        case CMD_ALL:
            exec_voltage();
            exec_temp();
            exec_status();
            exec_tlimit();
            exec_uptime();
            putstring_uart((unsigned char*)end_str);
            return;
        
        /*
         * Consider the command for setting temperature threshold:
         * 1. Input value is written to EEPROM for persistent storage.
         * 2. Error message printed on EEPROM write failure. 
         * 3. 'number' is the input value; obtained after doing number=number_fetch() in parse_command() function.
         */
        case CMD_SET_TLIMIT:
            if(my_eeprom_write_int(number, 0x0201)) //Write to EEPROM register
                putstring_uart((unsigned char* )"Updation failed; restart!\n");
            else
            {
                t_limit=number;                     //Update the threshold variable. 
                putstring_uart((unsigned char* )"DONE\n");
            }
            
            putstring_uart((unsigned char*)end_str);
            return;
        
        case CMD_T_OR:
            putstring_uart((unsigned char* )"Input number not within set range!\n");
            putstring_uart((unsigned char*)end_str);
            return;
        
        //Handling input numbers having invalid format.
        case INVALID_NUM:
            putstring_uart((unsigned char* )"Invalid input number!\n");
            putstring_uart((unsigned char*)end_str);
            return;
            
      
        case INVALID_DECIMAL:
            putstring_uart((unsigned char* )"Input number cannot have decimal place exceeding 1!\n");
            putstring_uart((unsigned char*)end_str);
            return;
        
        case CMD_NONE:
            putstring_uart((unsigned char* )"Command not found!\n"); 
            putstring_uart((unsigned char*)end_str);
    }
}

/*
 * 1. Function to extract number from input command(command_buffer).
 * 2. Space index value acts as reference point to start extracting number.  
 * 3. Returns extracted number.
 * 4. Returns an integer(t_uplimit+1) outside set range in case of invalid input.
 */
int number_fetch(void)
{
    int num=0;
    
    bool neg=false;
    
    unsigned char dpoint=0;
    
    space_index++;
    
    //neg flag for negative numbers.
    if(command_buffer[space_index]=='-')
    {
        neg=true;
        space_index++;
    }
    
    //Invalid number format handling.
    if(command_buffer[space_index]=='\0') 
    {
        curr_cmd=INVALID_NUM;
        return t_uplimit+1;
    } 
    
    while( !(command_buffer[space_index]=='\0') )
    {
        //dpoint used to count decimal place of number.
        if(dpoint)
            dpoint++;
        
        if(command_buffer[space_index]=='.')
        {
            dpoint++;
            space_index++;
            continue;
        }
        
        //Incorrect decimal place handling.
        if((dpoint-1)>1)
        {
            curr_cmd=INVALID_DECIMAL;
            return t_uplimit+1;
        }  
        
        //Handling invalid input.
        if( (command_buffer[space_index]<'0') || (command_buffer[space_index]>'9') )
        {
            curr_cmd=INVALID_NUM;
            return t_uplimit+1;
        }  
        num=(int)( 10*(unsigned int)num+(command_buffer[space_index]-'0') );
        
        //Check if number is within set range.
        if((num<t_lowlimit)||(num>t_uplimit))
        {
            curr_cmd=CMD_T_OR;
            return t_uplimit+1;
        }
     
        space_index++;
    }
    //For cases such as '80.'
    if(dpoint==1)
    {
        curr_cmd=INVALID_NUM;
        return t_uplimit+1;
    }
    
    //For integer scaling incase of whole number.
    if(!dpoint)
        num=num*10;
    
    //Negative number handling.
    if(neg)
        num=-num;
     
    curr_cmd=CMD_SET_TLIMIT;
    return num; 
}

/*
 * Function to add am integer input to string.
 * String, to which number is to be added, is output_buffer.
 * Note: output_index should be at the position from where number insertion starts.
 */
void addint_tostring(int num)
{
    unsigned char digit_count=0;
    
    if(num<0)
    {
        num=-num;
        output_buffer[output_index]='-';
        output_index++;
    }
    int temp=num;
    
    if(num==0)
        digit_count++;
    
    while(temp)
    {
        temp=temp/10;
        digit_count++;
    }
    
    temp=num;
    unsigned char i=digit_count;
    while(i)
    {
        //To obtain digits in left-to-right manner.
        for(unsigned char j=i; j>1; j--)
            temp=temp/10;
        
        //For decimal point positioning.
        if(i==1)
        {
            output_buffer[output_index]='.';
            output_index++;
            
        }
        output_buffer[output_index]=(unsigned char)('0'+(unsigned int)temp%10);
        output_index++;
        
        temp=num;
        
        i--;
    }
    output_buffer[output_index]='\0';  
}

/*
 * 1. Function to convert number(num) to string.
 * 2. Converted string stored in 'buf'.
 */
void ultoa(char *buf, unsigned long num)
{
    char temp[11];
    unsigned char i = 0;

    if(num == 0)
    {
        buf[0] = '0';
        buf[1] = '\0';
        return;
    }
    while(num)
    {
        temp[i] = (num % 10) + '0';
        i++;
        num=num/10;
    }
    for(unsigned char j = 0; j < i; j++)
    {
        buf[j] = temp[i - 1 - j];
    }

    buf[i] = '\0';
    
    return;
}

/*
 * 1. Function to display voltage readings.
 * 2. Result stored in output_buffer.
 */
void exec_voltage(void)
{
    strcpy((char* )output_buffer, "VOLTAGE: ");
            
    output_index=0;
    while(output_buffer[output_index]!='\0')
        output_index++;

    addint_tostring(get_voltage()); //get_voltage() fetches the voltage value.

    putstring_uart(output_buffer);
    
    putchar_uart((unsigned char)'\n');
}

/*
 * 1. Function to display voltage readings.
 * 2. Result stored in output_buffer.
 */
void exec_temp(void)
{
    strcpy((char* )output_buffer, "TEMP: ");
            
    output_index=0;
    while(output_buffer[output_index]!='\0')
        output_index++;

    addint_tostring(get_temp());    //get_temp() fetches the voltage value.

    putstring_uart(output_buffer);
    
    putchar_uart((unsigned char)'\n');
    
    
}

/*
 * 1. Function to display temperature threshold.
 * 2. Result stored in output_buffer.
 */
void exec_tlimit(void)
{
    strcpy((char* )output_buffer, "TEMP LIMIT: ");
            
    output_index=0;
    while(output_buffer[output_index]!='\0')
        output_index++;

    addint_tostring(t_limit);   //t_limit global variable contains the temperature threshold value.

    putstring_uart(output_buffer);
    
    putchar_uart((unsigned char)'\n');
    
    
}

/*
 * 1. Function to display temperature threshold.
 * 2. Result stored in output_buffer.
 */
void exec_status(void)
{
    strcpy((char* )output_buffer, "STATUS: ");
    
    strcat((char* )output_buffer, get_status());    //get_status() fetches the current state.    
    
    putstring_uart(output_buffer);
    
    putchar_uart((unsigned char)'\n');
}

/*
 * 1. Function to display uptime.
 * 2. Result stored in output_buffer.
 */
void exec_uptime(void)
{
    char buf[time_size]={0};
    
    strcpy((char* )output_buffer, "UPTIME: ");
    
    format_uptime(buf); //format_uptime() to display in HH:MM:SS format
    
    strcat((char*)output_buffer, buf);     
    
    putstring_uart(output_buffer);
    
    putchar_uart((unsigned char)'\n');
    
    return;
    
    
}


/*
 case NUM: 
            strcpy((char* )output_buffer, "NUM: ");
            
            output_index=0;
            while(output_buffer[output_index]!='\0')
                output_index++;
            
            addint_tostring(number);
            
            putstring_uart(output_buffer);
            putchar_uart((unsigned char)'\n');
            
            return;
 * 
 * 
 * void adc_op(void)
    {
        addint_tostring( ADCON0 );
        putstring_uart(output_buffer);
        putchar_uart((unsigned char)'\n');
        output_index=0;

        addint_tostring( ADCON1 );
        putstring_uart(output_buffer);
        putchar_uart((unsigned char)'\n');
        output_index=0;

        while(1)
        {
            addint_tostring( (int)adc_read(VOLTAGE) );

            putstring_uart(output_buffer);
            putchar_uart((unsigned char)'\n');
            output_index=0;

            __delay_ms(3000);
        }
    }
 */