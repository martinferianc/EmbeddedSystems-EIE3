#ifndef _bitcoinMine_h_
#define  _bitcoinMine_h_

#include "decodeCommands.h"
#include "hash/SHA256.h"
#include "rtos.h"
#include "mbed.h"

extern uint64_t* key;

extern void countHash();
extern void computeHash();

#endif
