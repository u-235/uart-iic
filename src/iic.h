#ifndef IIC_H_
#define IIC_H_

#include <avr/pgmspace.h>

#define IIC_STATUS_OK 0
#define IIC_STATUS_ERROR_START 1
#define IIC_STATUS_ERROR_ADDR  2
#define IIC_STATUS_ERROR_WAIT  3
#define IIC_STATUS_ERROR_WRITE 4
#define IIC_STATUS_ERROR_READ  5
#define IIC_STATUS_ERROR_STOP  6

void iic_init();
unsigned char iic_status();
prog_char* iic_status_message(unsigned char status);
void iic_clear();
void iic_set_address(unsigned char adr);
unsigned char iic_get_address();
void iic_write(unsigned char data);
unsigned char iic_read();
void iic_write_read(unsigned char write[], unsigned char wSz,
                unsigned char read[], unsigned char rSz);

#endif // IIC_H_
