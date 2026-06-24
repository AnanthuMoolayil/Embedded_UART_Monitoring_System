/*
 * Conversion from raw ADC values to relevant engineering units.
 * 
 * Raw ADC output   : 0   - 1023
 * Voltage range    : 220 - 270 V
 * Temperature range: -20 - 100 °C
 * 
 * Line formula: output = slope*input + constant is used.
 * 
 * Obtained results have a scaling factor of 10.
 */

#include <stdbool.h>
#include <string.h>
#include "sensor.h"
#include "adc.h"

#define voltage_constant 2200
#define voltage_slope 293


#define temp_constant 200
#define temp_slope 1173


#define scale_factor 1000   //To ensure final result has a scaling factor of 10.

int t_limit;        //To store temperature threshold value.

//For state monitoring(above or below temperature threshold).
bool curr=false;   
bool prev=false;

int get_voltage(void)
{
    return (int)( (long)voltage_slope*adc_read(VOLTAGE)/scale_factor + voltage_constant );  //adc_read() to fetch raw ADC value.
    
}    

int get_temp(void)
{
    return (int)( (long)temp_slope*adc_read(TEMP)/scale_factor - temp_constant );
}   

/*
 * Function to determine state based on temperature threshold.
 * Returns state in character string form.
 */
char* get_status(void)
{
    int temp_value = ( (long)temp_slope*adc_read(TEMP)/scale_factor - temp_constant );
    if(t_limit>=temp_value)
        return "OK";
    else
        return "OVERHEAT";
}

void status_monitor(void)
{
    if(strcmp("OVERHEAT", get_status()))
        curr=false;
    else
        curr=true;
    
}