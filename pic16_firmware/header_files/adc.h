#ifndef ADC_H
#define	ADC_H

typedef enum
{
    TEMP,
    VOLTAGE        
}ADC_CH;

void adc_init(void);

unsigned int adc_read(ADC_CH ch);

void adc_op(void);



#endif	

