#ifndef SRC_HAL_ACTION_H_
#define SRC_HAL_ACTION_H_

#define ACTION_BUFFER_SIZE 128

#define ACTION_ADDRESS_GET 0x02
#define ACTION_ADDRESS_SET 0x03

typedef struct _action {
        unsigned char status;
        unsigned char addrFlags; //! #ACTION_ADDRESS_GET or #ACTION_ADDRESS_SET
        unsigned char address;
        unsigned int writeSize;  //! If zero, then no write
        unsigned int readSize;   //! If zero, then no read
        unsigned char buff[ACTION_BUFFER_SIZE];
} action_t;

#endif // SRC_HAL_ACTION_H_
