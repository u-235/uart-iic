#include <stdio.h>
#include <avr/io.h>
#include  <avr/pgmspace.h>
#include <util/delay.h>
#include "twi.h"
#include "../iic.h"

/**
 * Макрос ожидания завершения операции.
 */
#define _twi_wait() \
        {\
                for(unsigned char c = 0; !(TWCR & (1<<TWINT)); c++)\
                {\
                        if (c > 200){\
                                status |= IIC_STATUS_ERROR_WAIT;\
                                break;\
                        }\
                        _delay_ms(1);\
                }\
        }

/**
 * Макрос ожидания завершения остановки.
 */
#define _twi_stop() \
        {\
                TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWSTO);\
                for (unsigned char c = 0; TWCR & (1<<TWSTO); c++)\
                {\
                        if (c > 200){\
                                status |= IIC_STATUS_ERROR_STOP;\
                                break;\
                        }\
                        _delay_ms(1);\
                }\
        }

static void twi_address(unsigned char addr);
static unsigned char twi_read(void);
static unsigned char twi_reads(void);
static void twi_write(unsigned char data);

static unsigned char status;  // Статус устройства.
static unsigned char address; // device address

/**
 * Инициализация модуля IIC.
 */
void iic_init()
{
        TWBR = 250;
        TWSR = 0;
        PORTC |= 0x30;
        DDRC &= ~0x30;
        status = IIC_STATUS_OK;
}

/**
 * Возвращает состояние модуля IIC, макроопределение вида IIC_STATUS_xxx.
 * Если состояние не #IIC_STATUS_OK, то перед использованием функций
 * чтения/записи следует вызвать iic_clear(). Для получения описания состояния
 * используйте iic_status_message(unsigned char status).
 */
unsigned char iic_status()
{
        return status;
}

/**
 * \param out
 */
void iic_put_status(FILE *out)
{
        unsigned char sts = iic_status();

        if ((sts & IIC_STATUS_ERROR_WAIT) != 0) {
                sts &= !IIC_STATUS_ERROR_WAIT;
                fprintf_P(out, PSTR("IIC error: timeout\n"));
                if (sts == IIC_STATUS_OK) {
                        return;
                }
        }

        if (sts != IIC_STATUS_OK) {
                fprintf_P(out, PSTR("IIC error: "));
        }

        switch (sts) {
        case IIC_STATUS_OK:
                fprintf_P(out, PSTR("IIC OK\n"));
                break;
        case IIC_STATUS_ERROR_START:
                fprintf_P(out, PSTR("fail while start\n"));
                break;
        case IIC_STATUS_ERROR_ADDR:
                fprintf_P(out, PSTR("fail while send address\n"));
                break;
        case IIC_STATUS_ERROR_WRITE:
                fprintf_P(out, PSTR("fail while write\n"));
                break;
        case IIC_STATUS_ERROR_READ:
                fprintf_P(out, PSTR("fail while read\n"));
                break;
        case IIC_STATUS_ERROR_STOP:
                fprintf_P(out, PSTR("fail while stop\n"));
                break;
        default:
                fprintf_P(out, PSTR("unknown error\n"));
        }
}

/**
 * Сброс ошибки.
 */
void iic_clear()
{
        status = IIC_STATUS_OK;
}

/**
 * Установка нового адреса устройства.
 * \param adr Новый адрес устройства. Младший бит игнорируется и сбрасывается
 *      ноль.
 */
void iic_set_address(unsigned char adr)
{
        address = adr & 0xfe;
}

/**
 * Получение адреса устройства.
 * \return Адрес устройства.
 */
unsigned char iic_get_address()
{
        return address;
}

/**
 * Запись байта в устройство. Адрес устройства можно изменить функцией
 * iic_set_address(unsigned char adr).
 *
 * Перед вызовом необходимо убедиться, что функция iic_status() возвращает
 * #IIC_STATUS_OK. В ином случае нужно вызвать iic_clean().
 * \param data Байт для записи в устройство.
 */
void iic_write(unsigned char data)
{
        twi_address(address);
        twi_write(data);
        _twi_stop()
        ;
}

/**
 * Чтение байта из устройства. Адрес устройства можно изменить функцией
 * iic_set_address(unsigned char adr).
 *
 * Перед вызовом необходимо убедиться, что функция iic_status() возвращает
 * #IIC_STATUS_OK. В ином случае нужно вызвать iic_clean().
 * \return Байт данных из устройства.
 */
unsigned char iic_read()
{
        unsigned char res;
        twi_address(address | 0x01);
        res = twi_read();
        _twi_stop()
        ;
        return res;
}

/**
 * Запись и последующее чтение массивов из/в устройство. Поскольку сначала
 * происходит запись, то, при необходимости, считывание можно производить
 * в тот же буфер.
 * \param inBuff Массив с данными для записи. Если равен нулю, то записи
 *      не происходит.
 * \param wSz Количество записываемых данных. Если равен нулю, то записи
 *      не происходит.
 * \param outBuff Массив для считываемых данных. Если равен нулю, то записи
 *      не происходит.
 * \param rSz Количество считываемых байт. Если равен нулю, то записи
 *      не происходит.
 */
void iic_write_read(unsigned char inBuff[], unsigned char wSz,
                unsigned char outBuff[], unsigned char rSz)
{
        char stop = 0;

        if (inBuff != 0 && wSz != 0) {
                stop = 1;
                twi_address(address);
                for (unsigned char i = 0; i < wSz; i++) {
                        twi_write(*inBuff++);
                }
        }

        if (outBuff != 0 && rSz != 0) {
                stop = 1;
                twi_address(address | 0x01);
                for (unsigned char i = rSz; i > 0; i--) {
                        if (i != 1) {
                                *outBuff++ = twi_reads();
                        } else {
                                *outBuff++ = twi_read();
                        }
                }
        }

        if (stop != 0) {
                _twi_stop()
                ;
        }
}

/**
 * Выставляет на шину состояние старт/рестарт и адрес устройства.
 * \param addr адрес устройства. Младший бит должен быть равен нулю для
 *      записи и единице для чтения.
 */
static void twi_address(unsigned char addr)
{
        if (status) return;

        TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTA);
        _twi_wait()
                ;

        if (_twsr_not(TWI_MTS_START) && _twsr_not(TWI_MTS_RESTART)) {
                status |= IIC_STATUS_ERROR_START;
                return;
        }
        TWDR = addr;
        TWCR = (1 << TWINT) | (1 << TWEN);
        _twi_wait()
                ;

        if (_twsr_not(TWI_MTS_ADR_ACK) && _twsr_not(TWI_MRS_ADR_ACK)) {
                status |= IIC_STATUS_ERROR_ADDR;
        }
        return;
}

/**
 * Выставляет на шину байт данных.
 */
static void twi_write(unsigned char data)
{
        if (status) return;
        TWDR = data;
        TWCR = (1 << TWINT) | (1 << TWEN);
        _twi_wait()
                ;

        if (_twsr_not(TWI_MTS_DATA_ACK)) {
                status |= IIC_STATUS_ERROR_WRITE;
        }
        return;
}

/**
 * Считывает с шины байт.
 */
static unsigned char twi_read(void)
{
        if (status) return 0;
        TWCR = (1 << TWINT) | (1 << TWEN);
        _twi_wait()
                ;

        if (_twsr_not(TWI_MRS_DATA_NOACK)) {
                status |= IIC_STATUS_ERROR_READ;
        }
        return TWDR;
}

/**
 * Считывает с шины один из цепочки байт.
 */
static unsigned char twi_reads(void)
{
        if (status) return 0;
        TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
        _twi_wait()
                ;

        if (_twsr_not(TWI_MRS_DATA_ACK)) {
                status |= IIC_STATUS_ERROR_READ;
        }
        return TWDR;
}

