#include <stdio.h>
#include  <avr/pgmspace.h>
#include "action.h"
#include "cmd.h"
#include "uart.h"
#include "iic.h"

#define INPUT_BUFFER_SIZE 128

void execute(action_t *act);
void report(action_t *act);
void print_help();

int main(void)
{
        static char buff[INPUT_BUFFER_SIZE];
        action_t act;

        uart_init();
        iic_init();
        stderr = stdout;

        puts_P(PSTR("Nick Egorov 2014"));
        puts_P(PSTR("uart to iic adaptor v1.0"));
        print_help();

        while (1) {
                fgets(buff, INPUT_BUFFER_SIZE, stdin);
                if (cmd_parse(buff, &act) != 0) {
                        print_help();
                        continue;
                }
                execute(&act);
                report(&act);
        }

        return 0;
}

void execute(action_t *act)
{
        if (act->addrFlags == ACTION_ADDRESS_GET) {
                act->address = iic_get_address();
        }

        if (act->addrFlags == ACTION_ADDRESS_SET) {
                iic_set_address(act->address);
        }

        iic_write_read(act->buff, act->writeSize, act->buff, act->readSize);

        act->status = iic_status();
}

void report(action_t *act)
{
        if (act->status != IIC_STATUS_OK) {
                iic_put_status(stderr);
                iic_clear();
                return;
        }

        if (act->addrFlags == ACTION_ADDRESS_GET) {
                printf_P(PSTR("get address = %#02.2x\n"), act->address);
        }

        if (act->addrFlags == ACTION_ADDRESS_SET) {
                printf_P(PSTR("set address = %#02.2x\n"), act->address);
        }

        if (act->writeSize != 0) {
                printf_P(PSTR("write %i bytes\n"), act->writeSize);
        }

        if (act->readSize != 0) {
                unsigned char *buff = act->buff;
                printf_P(PSTR("read %i bytes\n"), act->readSize);
                for (unsigned int c = 0; c < act->readSize; c++) {
                        printf_P(PSTR(" %#02.2x"), *buff++);
                }
                putchar('\n');
        }
}

void print_help()
{
        puts_P(PSTR("\nUsage:"));
        puts_P(PSTR("[a [int]] [w int {int}] [r int]"));
        puts_P(PSTR("  a [int] - get or set address"));
        puts_P(PSTR("  w int {int} - write byte(s)"));
        puts_P(PSTR("  r int - read int byte"));
        puts_P(PSTR("  int - unsigned integer in range from 0 to 255 as:"));
        puts_P(PSTR("    0b01011 - binary"));
        puts_P(PSTR("    072     - octal"));
        puts_P(PSTR("    221     - decimal"));
        puts_P(PSTR("    0xa9    - hex\n"));
}
