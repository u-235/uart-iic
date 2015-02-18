#include <avr/io.h>
#include <stdio.h>

static int uart_put(char data, FILE *stream);
static int uart_get(FILE *stream);

static FILE uartOut = FDEV_SETUP_STREAM(uart_put, uart_get, _FDEV_SETUP_WRITE);
static FILE uartIn = FDEV_SETUP_STREAM(NULL, uart_get, _FDEV_SETUP_READ);

/**
 * Инициализация модуля UART.
 */
void uart_init()
{
        /* Set baud rate */
        UBRRH = (unsigned char) (51 >> 8);
        UBRRL = (unsigned char) 51;
        /* Enable receiver and transmitter */UCSRB = (1 << RXEN) | (1 << TXEN);
        /* Set frame format: 8data, 2stop bit */
        UCSRC = (1 << URSEL) | (1 << USBS) | (3 << UCSZ0);
        stdout = &uartOut;
        stderr= &uartOut;
        stdin= &uartIn;
}

static int uart_put(char data, FILE *stream)
{
        while (!(UCSRA & (1 << UDRE))) {
                // TODO timeout
        }
        UDR = data;
        return 0;
}

static int uart_get(FILE *stream)
{
        while (!(UCSRA & (1 << RXC))) {
                // TODO timeout
        }
        return (int) UDR & 0xff;
}
