/*
 */

#include "uart.h"
#include "iic.h"

int main(void)
{
        uart_init();
        iic_init();

        while (1) {

        }

        return 0;
}
