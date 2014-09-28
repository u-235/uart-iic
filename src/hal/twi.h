/*
 * twi.h
 *
 *  Created on: 20 дек. 2013 г.
 *      Author: Николай
 */

#ifndef TWI_H_
#define TWI_H_

/*!
 \file twi.h
 Константы и макросы для TWI.

 В WinAVR есть заголовочный файл для работы с \ref twi "TWI", но
 он не полон.

 \section twi Модуль TWI.
 Модуль TWI предназначен для связи микроконтроллера с внешними устройствами
 через шину I2C.
 \n \ref twireg
 \n \ref twistate

 \subsection twireg Регистры TWI.
 Модуль содержит следующие регистры

 \par Регистр TWBR.
 Совместно с \b "TWSR" управляет частотой сигнала \b SCL. Частота
 вычисляется по формуле
 \code
 Частота CPU
 SCL=----------------------
 TWPS
 16 + 2(TWBP)*4
 \endcode
 Регистр доступен для чтения и записи.

 \par Регистр управления TWCR.
 \b |TWINT|TWEA|TWSTA|TWSTO|TWWC|TWEN|---|TWIE|
 \n\n\ref TWINT. Флаг прерывания.
 \n\ref TWEA. Бит разрешения подтверждения \b ACK.
 \n\ref TWSTA. Бит состояния \b START.
 \n\ref TWSTO. Бит состояния \b STOP.
 \n\ref TWWC. Флаг коллизии записи.
 \n\ref TWEN. Флаг разрешения TWI.
 \n\ref TWIE. Бит разрешения прерываний TWI.

 \par  Регистр состояния TWSR.
 \b |TWS7|TWS6|TWS5|TWS4|TWS3|---|TWPS1|TWPS0|
 \n\n Биты TWS7 - TWS3 отражают состояние модуля и доступны только для чтения.
 Что бы получить значение только этих битов, Вы можете использовать макрос
 \ref _twi_status. Значения этих битов при различных условиях обсуждаются
 на странице \ref twistate "состояния TWI".
 \n\n Биты \ref TWPS1 и \ref TWPS0 управляют предделителем и доступны для чтения
 и записи. Для комбинаций этих битов определены следующие константы:
 - \ref TWI_PRESCALER_1
 - \ref TWI_PRESCALER_4
 - \ref TWI_PRESCALER_16
 - \ref TWI_PRESCALER_64

 \par  Регистр TWDR.

 \par  Регистр TWAR.

 \par  Регистр TWAMR.

 \subsection twistate Состояния TWI.
 \par twistatemt Ведущий передатчик.
 - \ref TWI_MTS_START
 - \ref TWI_MTS_RESTART
 - \ref TWI_MTS_ADR_ACK
 - \ref TWI_MTS_ADR_NOACK
 - \ref TWI_MTS_DATA_ACK
 - \ref TWI_MTS_DATA_NOACK
 - \ref TWI_MTS_LOST_CONTROL
 */

/*! \def TWINT
 \brief Флаг прерывания.
 \details Этот бит устанавливается аппаратно при завершении текущей
 операции. Сброс происходит только программно при записи в этот бит
 единицы. После сброса этого флага запускается операция TWI, поэтому
 любые обращения к регистрам  "адреса",  "статуса" и
 "данных" должны быть завершены до сброса этого флага.


 \def TWEA
 \brief Бит разрешения подтверждения \b ACK.
 \details Если этот бит установлен, то подтверждение \b ACK генерируется
 в следующих случаях:
 - Был получен адрес от ведущего устройства.
 - Был получен общий вызов и биты \ref TWGCE и "TWAR" установлены.
 - В режиме ведущего устройства был получен байт данных от ведомого
 устройства.


 \def TWSTA
 \brief Бит состояния \b START.
 \details Установка этого бита переводит модуль в режим ведущего и, если
 шина I2C свободна, генерирует состояние \b START. Если шина занята, модуль
 ожидает её освобождения, то есть состояния \b STOP. После генерации
 \b START этот бит должен быть сброшен программно.


 \def TWSTO
 \brief Бит состояния \b STOP.
 \details В режиме ведущего установка этого бита приводит к генерации
 состояния \b STOP. Когда \b STOP выполнен, этот бит сбрасывается
 автоматически.
 \n В режиме ведомого используется при ошибках. Генерации состояния \b STOP
 не происходит. Модуль переводится в режим не выбранного устройства и линии
 \b SCL и \b SDA переходят в высокоомное состояние.


 \def TWWC
 \brief Флаг коллизии записи.
 \details Этот флаг устанавливается при попытке записи в
 "регистр данных" когда \ref TWINT сброшен. Этот флаг сбрасывается при
 записи в регистр данных при высоком уровне TWINT.


 \def TWEN
 \brief Бит разрешения TWI.
 \details Этот бит разрешает операции TWI и включает интерфейс TWI. Когда
 бит установлен в единицу, модуль TWI получает управление над выводами,
 подключенными к \b SCL и \b SDA.


 \def TWIE
 \brief Бит разрешения прерываний TWI.
 \details Когда этот бит и бит \b I регистра \b SREG установлены в единицу
 запрос прерывания TWI активен пока \ref TWINT находится в высоком состоянии.
 */
#ifndef TWINT
#   define TWINT   7
#endif

#ifndef TWEA
#   define TWEA    6
#endif

#ifndef TWSTA
#   define TWSTA   5
#endif

#ifndef TWSTO
#   define TWSTO   4
#endif

#ifndef TWWC
#   define TWWC    3
#endif

#ifndef TWEN
#   define TWEN    2
#endif

#ifndef TWIE
#   define TWIE    0
#endif

/*! \def TWPS1
 \brief Бит 1 предделителя TWI.
 \details
 , \ref TWI_PRESCALER_1, \ref TWI_PRESCALER_4,
 \ref TWI_PRESCALER_16, \ref TWI_PRESCALER_64

 \def TWPS0
 \brief Бит 0 предделителя TWI.
 \details
 , \ref TWI_PRESCALER_1, \ref TWI_PRESCALER_4,
 \ref TWI_PRESCALER_16, \ref TWI_PRESCALER_64

 \def _twi_status
 \brief Получение статуса TWI.
 \details Макрос возвращает значение старших пяти битов регистра состояния
 TWI.

 */

#ifndef TWPS1
#   define TWPS1   1
#endif

#ifndef TWPS0
#   define TWPS0   0
#endif

#define _twi_status (TWSR & 0xF8)
#define _twsr_is(st) (_twi_status == st)
#define _twsr_not(st) (_twi_status != st)

/*! \def TWI_PRESCALER_1
 \brief Предделитель TWI равен 1.
 \details Макрос устанавливает значение двух младших битов регистра состояния
 TWI для выбора предделителя.
 , \ref TWI_PRESCALER_4, \ref TWI_PRESCALER_16,
 \ref TWI_PRESCALER_64

 \def TWI_PRESCALER_4
 \brief Предделитель TWI равен 4.
 \details Макрос устанавливает значение двух младших битов регистра состояния
 TWI для выбора предделителя.
 , \ref TWI_PRESCALER_1, \ref TWI_PRESCALER_16,
 \ref TWI_PRESCALER_64

 \def TWI_PRESCALER_16
 \brief Предделитель TWI равен 16.
 \details Макрос устанавливает значение двух младших битов регистра состояния
 TWI для выбора предделителя.
 , \ref TWI_PRESCALER_1, \ref TWI_PRESCALER_4,
 \ref TWI_PRESCALER_64

 \def TWI_PRESCALER_64
 \brief Предделитель TWI равен 64.
 \details Макрос устанавливает значение двух младших битов регистра состояния
 TWI для выбора предделителя.
 , \ref TWI_PRESCALER_1, \ref TWI_PRESCALER_4,
 \ref TWI_PRESCALER_16
 */
#define TWI_PRESCALER_1     0
#define TWI_PRESCALER_4     (1<<TWPS0)
#define TWI_PRESCALER_16    (1<<TWPS1)
#define TWI_PRESCALER_64    ((1<<TWPS1)|(1<<TWPS0))

/*!
 \def TWI_MTS_START
 \brief Было передано состояние START.
 \details TWI работает в режиме "ведущий передатчик".

 \b Реакция
 - Загрузка в "регистр данных" адреса устройства и бита
 записи. Установка бита \ref TWINT. Ожидается передача адреса и состояние
 \ref TWI_MTS_ADR_ACK или \ref TWI_MTS_ADR_NOACK.
 "регистр состояния", \ref _twi_status

 \def TWI_MTS_RESTART
 \brief Было передано повторное состояние START.
 \details TWI работает в режиме "ведущий передатчик".

 \b Реакция
 - Загрузка в "регистр данных" адреса устройства и бита
 записи. Установка бита \ref TWINT. Ожидается передача адреса и состояние
 \ref TWI_MTS_ADR_ACK или \ref TWI_MTS_ADR_NOACK.
 - Загрузка в "регистр данных" адреса устройства и бита
 чтения. Установка бита \ref TWINT. TWI переходит в режим ведущего получателя
 данных. Ожидается состояние \ref TWI_MRS_ADR_ACK или \ref TWI_MRS_ADR_NOACK.
 "регистр состояния", \ref _twi_status

 \def TWI_MTS_ADR_ACK
 \brief Был передан адрес и получено состояние ACK.
 \details TWI работает в режиме  "ведущий передатчик".

 \b Реакция
 - Загрузка в "регистр данных" байта данных. Установка бита
 \ref TWINT. Ожидается состояние \ref TWI_MTS_DATA_ACK или
 \ref TWI_MTS_DATA_NOACK.
 - В "регистр данных" ничего не загружается. Устанавливаются биты
 \ref TWINT и \ref TWSTA. Ожидается состояние \ref TWI_MTS_RESTART.
 - В "регистр данных" ничего не загружается. Устанавливаются биты
 \ref TWINT и \ref TWSTO. Будет передано состояние STOP и очищен бит
 \ref TWSTO.
 - В "регистр данных" ничего не загружается. Устанавливаются биты
 \ref TWINT, \ref TWSTA  и \ref TWSTO. Будет передано состояние START, затем
 STOP и очищен бит \ref TWSTO.
 "регистр состояния", \ref _twi_status

 \def TWI_MTS_ADR_NOACK
 \brief Был передан адрес, но состояние ACK не получено.
 \details TWI работает в режиме "ведущий передатчик".

 \b Реакция
 - Загрузка в "регистр данных" байта данных. Установка бита
 \ref TWINT. Ожидается состояние \ref TWI_MTS_DATA_ACK или
 \ref TWI_MTS_DATA_NOACK.
 - В "регистр данных" ничего не загружается. Устанавливаются биты
 \ref TWINT и \ref TWSTA. Ожидается состояние \ref TWI_MTS_RESTART.
 - В "регистр данных" ничего не загружается. Устанавливаются биты
 \ref TWINT и \ref TWSTO. Будет передано состояние STOP и очищен бит
 \ref TWSTO.
 - В "регистр данных" ничего не загружается. Устанавливаются биты
 \ref TWINT, \ref TWSTA  и \ref TWSTO. Будет передано состояние START, затем
 STOP и очищен бит \ref TWSTO.
 "регистр состояния", \ref _twi_status

 */
#define TWI_MTS_START           ((unsigned char)0x08)
#define TWI_MTS_RESTART         ((unsigned char)0x10)
#define TWI_MTS_ADR_ACK         ((unsigned char)0x18)
#define TWI_MTS_ADR_NOACK       ((unsigned char)0x20)
#define TWI_MTS_DATA_ACK        ((unsigned char)0x28)
#define TWI_MTS_DATA_NOACK      ((unsigned char)0x30)
#define TWI_MTS_LOST_CONTROL    ((unsigned char)0x38)

#define TWI_MRS_START           ((unsigned char)0x08)
#define TWI_MRS_RESTART         ((unsigned char)0x10)
#define TWI_MRS_ADR_ACK         ((unsigned char)0x40)
#define TWI_MRS_ADR_NOACK       ((unsigned char)0x48)
#define TWI_MRS_DATA_ACK        ((unsigned char)0x50)
#define TWI_MRS_DATA_NOACK      ((unsigned char)0x58)
#define TWI_MRS_LOST_CONTROL    ((unsigned char)0x38)

#define TWI_STS_ADR_ACK         ((unsigned char)0xA8)
#define TWI_STS_DATA_ACK        ((unsigned char)0xB8)
#define TWI_STS_DATA_NOACK      ((unsigned char)0xC0)
#define TWI_STS_LOST_CONTROL    ((unsigned char)0xB0)

#define TWI_SRS_START           ((unsigned char)0x08)
#define TWI_SRS_RESTART         ((unsigned char)0x10)
#define TWI_SRS_ADR_ACK         ((unsigned char)0x40)
#define TWI_SRS_ADR_NOACK       ((unsigned char)0x48)
#define TWI_SRS_DATA_ACK        ((unsigned char)0x50)
#define TWI_SRS_DATA_NOACK      ((unsigned char)0x58)
#define TWI_SRS_LOST_CONTROL    ((unsigned char)0x68)

#endif /* TWI_H_ */
