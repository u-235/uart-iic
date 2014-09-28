/**
 *
 */

#include <avr/io.h>
#include <inttypes.h>
#include <avr/pgmspace.h>

/*
 * Инициализация модуля UART.
 */
void uart_init()
{
}

void uart_write(char data)
{
        while (!(UCSRA & (1 << UDRE))){
                // TODO timeout
        }
        UDR = data;
}

char uart_read()
{
        while (!(UCSRA & (1 << RXC))){
                // TODO timeout
        }
        return UDR;
}
