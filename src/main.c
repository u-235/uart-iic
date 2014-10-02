/*
 */

#include "uart.h"
#include "iic.h"

void promt();

int main(void)
{
        uart_init();
        iic_init();
        promt();

        while (1) {
        }

        return 0;
}

void promt()
{
        uart_write_line(PSTR("Nick Egorov 2014"));
        uart_write_line(PSTR("uart to iic adaptor v0.3"));
        uart_write_line(PSTR("type <h> or <?> to print help"));
}
