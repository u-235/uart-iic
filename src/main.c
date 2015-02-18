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
        puts_P(PSTR("Nick Egorov 2014 ©®"));
        puts_P(PSTR("uart to iic adaptor v0.3"));
        print_help();

        while (1) {
                fgets(buff, INPUT_BUFFER_SIZE, stdin);
                if (cmd_parse(buff, &act) != 0) {
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
                puts_P(iic_status_message(act->status));
                iic_clear();
                return;
        }

        if (act->addrFlags == ACTION_ADDRESS_GET) {
                printf_P(PSTR("address = %x\n"), act->address);
        }

        if (act->readSize != 0) {
                unsigned char * buff = act->buff;
                puts_P(PSTR("read"));
                for (unsigned int c = 0; c < act->readSize; c++) {
                        printf_P(PSTR(" %x"), *buff++);
                }
                putchar('\n');
        }
}

void print_help()
{
        puts_P(PSTR("Usage:"));
}
