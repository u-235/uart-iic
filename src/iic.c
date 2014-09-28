#include <avr/io.h>
#include <inttypes.h>
#include "hal/twi.h"
#include "iic.h"

#define _twi_wait()     while(!(TWCR & (1<<TWINT)));
#define _twi_stop()     TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWSTO);\
                        while (TWCR & (1<<TWSTO));

static void twi_address(uint8_t addr);
static uint8_t twi_read(void);
static void twi_write(uint8_t data);

static uint8_t twierror;
static unsigned char address; // device address

void iic_init()
{
#if   defined (__AVR_ATmega8__)    || \
      defined (__AVR_ATmega8U2__)

        TWBR = 36;
        TWSR = 0;
        PORTC |= 0x30;
        DDRC &= ~0x30;

#elif defined (__AVR_ATmega16__)   || \
      defined (__AVR_ATmega16A__)  || \
      defined (__AVR_ATmega16M1__) || \
      defined (__AVR_ATmega16U2__) || \
      defined (__AVR_ATmega16U4__)

        TWBR = 36;
        TWSR = 0;
        PORTC |= 0x30;
        DDRC &= ~0x30;

#else
#error "MCU type is not valid"
#endif
}

/**
 * Возвращает состояние модуля twi, макроопределение вида IIC_STATUS_xxx.
 * Если состояние не IIC_STATUS_OK, то перед использованием функций чтения/записи
 * следует вызвать iic_clear(). Для получения описания состояния используйте
 * iic_status_message(unsigned char).
 */
unsigned char iic_status()
{
        return twierror;
}
/**
 * Возвращает строку с описанием ошибки модуля TWI.
 * @param status статус модуля, см. iic_status()
 * @return строка с описанием, расположенная во флеше
 */
prog_char* iic_status_message(unsigned char status)
{
        switch (status) {
        case IIC_STATUS_OK:
                return PSTR("OK, no error");
        case IIC_STATUS_ERROR_START:
                return PSTR("fail while start");
        case IIC_STATUS_ERROR_ADDR:
                return PSTR("fail while send address");
        case IIC_STATUS_ERROR_WAIT:
                return PSTR("fail while wait");
        case IIC_STATUS_ERROR_WRITE:
                return PSTR("fail while write");
        case IIC_STATUS_ERROR_READ:
                return PSTR("fail while read");
        case IIC_STATUS_ERROR_STOP:
                return PSTR("fail while stop");
        default:
                return PSTR("unknown error");
        }
}

void iic_clear()
{
        twierror = IIC_STATUS_OK;
}

void iic_set_address(unsigned char adr)
{
        address = adr;
}

unsigned char iic_get_address()
{
        return address;
}

void iic_write(unsigned char data)
{
        twi_address(address);
        twi_write(data);
}

unsigned char iic_read()
{
        twi_address(address);
        return twi_read();
}

static void twi_address(uint8_t addr)
{
        if (twierror) return;

        TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTA);
        _twi_wait()

        if (_twsr_not(TWI_MTS_START) && _twsr_not(TWI_MTS_RESTART)) {
                twierror = IIC_STATUS_ERROR_START;
                return;
        }
        TWDR = addr;
        TWCR = (1 << TWINT) | (1 << TWEN);
        _twi_wait()

        if (_twsr_is(TWI_MTS_ADR_ACK) || _twsr_is(TWI_MRS_ADR_ACK)) return;
        twierror = IIC_STATUS_ERROR_ADDR;
        return;
}

static void twi_write(uint8_t data)
{
        if (twierror) return;
        TWDR = data;
        TWCR = (1 << TWINT) | (1 << TWEN);
        _twi_wait()

        if (_twsr_is(TWI_MTS_DATA_ACK)) return;
        twierror = IIC_STATUS_ERROR_WRITE;
        return;
}

static uint8_t twi_read(void)
{
        if (twierror) return 0;
        TWCR = (1 << TWINT) | (1 << TWEN);
        _twi_wait()

        if (_twsr_is(TWI_MRS_DATA_NOACK)) return TWDR;
        twierror = IIC_STATUS_ERROR_READ;
        return 0;
}

