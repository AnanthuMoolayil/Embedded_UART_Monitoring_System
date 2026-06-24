#ifndef UART_H
#define	UART_H

#include <stdbool.h>

void uart_init(void);

bool getchar_uart(unsigned char *c);

void putchar_uart(unsigned char c);

void putstring_uart(unsigned char *arr);

#endif	

