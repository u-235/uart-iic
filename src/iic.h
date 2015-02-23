#ifndef IIC_H_
#define IIC_H_

#include <avr/pgmspace.h>

#define IIC_STATUS_OK 0
#define IIC_STATUS_ERROR_WAIT  0x80
#define IIC_STATUS_ERROR_START 0x01
#define IIC_STATUS_ERROR_ADDR  0x02
#define IIC_STATUS_ERROR_WRITE 0x03
#define IIC_STATUS_ERROR_READ  0x04
#define IIC_STATUS_ERROR_STOP  0x05

void iic_init();
unsigned char iic_status();
void iic_put_status(FILE *out);
void iic_clear();
void iic_set_address(unsigned char adr);
unsigned char iic_get_address();
void iic_write(unsigned char data);
unsigned char iic_read();
void iic_write_read(unsigned char write[], unsigned char wSz,
                unsigned char read[], unsigned char rSz);

#endif // IIC_H_
