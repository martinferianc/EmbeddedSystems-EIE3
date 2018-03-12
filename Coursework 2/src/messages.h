#ifndef _messages_h_
#define  _messages_h_

#include "rtos.h"

//TODO:
//define message codes
#define NONCE 2

extern RawSerial pc;

// Structure to hold the unique code and the data of each message in memory

typedef struct{
  uint8_t code;
  uint8_t data;
  } message_t;

//////// /C SERIAL FUNCTION PROTOTYPES //////////

void commOutFn();

void putMessage(uint8_t code, uint32_t data);

//////// /E SERIAL FUNCTION PROTOTYPES //////////

#endif
