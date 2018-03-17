#ifndef _decodeCommands_h_
#define  _decodeCommands_h_

#include "rtos.h"
#include "messages.h"

extern volatile uint64_t* key;
volatile int32_t velocity;
volatile int32_t rotations;

void serialISR();

extern void decode();

#endif
