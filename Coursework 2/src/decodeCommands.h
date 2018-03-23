#ifndef _decodeCommands_h_
#define  _decodeCommands_h_

#include "rtos.h"
#include "messages.h"
#include "motorControl.h"
#include "bitcoinMine.h"

extern volatile float tar_velocity;
extern volatile float tar_rotations;
extern volatile uint32_t motorPWM;

void serialISR();

extern void decode();

#endif
