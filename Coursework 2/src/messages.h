#ifndef _messages_h_
#define  _messages_h_

#include "rtos.h"
// Define Output Codes
#define ROTATE 0
#define ROTOR_STATE 1
#define VELOCITY 2
#define HASH 3
#define NONCE 4

extern RawSerial pc;

// Structure to hold the unique code and the data of each message in memory

typedef struct {
        uint8_t code;
        uint8_t data;
} message_t;

//////// /C SERIAL FUNCTION PROTOTYPES //////////

void commOutFn();

extern void putMessage(uint8_t code, uint32_t data);

//////// /E SERIAL FUNCTION PROTOTYPES //////////

#endif
