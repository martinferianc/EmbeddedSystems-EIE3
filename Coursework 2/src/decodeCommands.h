#ifndef _decodeCommands_h_
#define  _decodeCommands_h_

#include "rtos.h"
#include "messages.h"
#include "motorControl.h"
#include "bitcoinMine.h"

extern volatile float tar_velocity;
extern volatile float tar_rotations;
extern volatile uint32_t motorPWM;

extern volatile uint64_t new_key;
extern volatile bool new_key_set;

extern Mutex key_mutex;

void serialISR();

extern void decode();

#endif
