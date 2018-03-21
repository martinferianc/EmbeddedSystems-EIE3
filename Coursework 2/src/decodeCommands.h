#ifndef _decodeCommands_h_
#define  _decodeCommands_h_

#include "rtos.h"
#include "messages.h"
#include "motorControl.h"

extern volatile uint64_t* key;
extern volatile float tar_velocity;
extern volatile float tar_rotations;
extern volatile uint32_t motorPWM;
extern volatile uint32_t err;

void serialISR();

extern void decode();

#endif
