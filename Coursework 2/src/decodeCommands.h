/*
#ifndef _decodeCommands_h_
#define  _decodeCommands_h_

#include "rtos.h"
#include "messages.h"

extern RawSerial pc;
extern Queue<void,8> inCharQ;
extern void (*serialISR)();
extern char charBuffer[16];
extern int charBufferCounter;
extern uint64_t newKey;
extern Mutex newKey_mutex;

void decode();

#endif
*/
