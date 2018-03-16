#ifndef _decodeCommands_h_
#define  _decodeCommands_h_

#include "rtos.h"
#include "messages.h"

<<<<<<< HEAD
extern volatile uint64_t newKey;
extern volatile uint32_t torqueKey;
=======
extern volatile uint64_t* key;
>>>>>>> origin/master

void serialISR();

extern void decode();

#endif
