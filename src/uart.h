#ifndef _UART_H_
#define _UART_H_

#include <avr/pgmspace.h>

void uart_init();
void uart_write(char data);
char uart_read();
void uart_write_string(prog_char* str);
void uart_write_line(prog_char* str);

#endif // _UART_H_
