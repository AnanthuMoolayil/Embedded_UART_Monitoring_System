#ifndef EEPROM_H
#define	EEPROM_H

unsigned char my_eeprom_read(unsigned char add);

bool my_eeprom_write(unsigned char data, unsigned char add);

int my_eeprom_read_int(unsigned int add);

bool my_eeprom_write_int(int data, unsigned int add);

void eeprom_init(void);

#endif	

