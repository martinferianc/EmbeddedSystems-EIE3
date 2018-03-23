#ifndef _messages_h_
#define  _messages_h_

#include "rtos.h"

// Define Output Codes
#define ROTATE        0
#define ROTOR_STATE   1
#define VELOCITY      2
#define HASH          3
#define NONCE_LOW     4
#define NONCE_UPPER   6
#define KEY           5
#define STARTUP       13
#define TAR_VELOCITY  14
#define TEST_MSG      15
#define ROTATION      16
#define TAR_ROTATION  17
#define TAR_VELOCITY_SET  18
#define TAR_ROTATION_SET  19
#define TORQUE_TEST       20
#define HEX_TEST          21
#define ERROR      99

extern RawSerial pc;

// Structure to hold the unique code and the data of each message in memory

typedef struct {
        uint8_t code;
        int32_t data;
} message_t;


void commOutFn();

extern void putMessage(uint8_t code, int32_t data);

#endif
