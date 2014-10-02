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
{/* Set baud rate */
        UBRRH = (unsigned char) (51 >> 8);
        UBRRL = (unsigned char) 51;
        /* Enable receiver and transmitter */UCSRB = (1 << RXEN) | (1 << TXEN);
        /* Set frame format: 8data, 2stop bit */
        UCSRC = (1 << URSEL) | (1 << USBS) | (3 << UCSZ0);
}

void uart_write(char data)
{
        while (!(UCSRA & (1 << UDRE))) {
                // TODO timeout
        }
        UDR = data;
}

char uart_read()
{
        while (!(UCSRA & (1 << RXC))) {
                // TODO timeout
        }
        return UDR;
}

void uart_write_string(prog_char* str)
{
        char s;
        while (1) {
                s = pgm_read_byte(str++);
                if (s == 0) break;
                uart_write(s);
        }
}

void uart_write_line(prog_char* str)
{
        uart_write_string(str);
        uart_write('\n');
}
