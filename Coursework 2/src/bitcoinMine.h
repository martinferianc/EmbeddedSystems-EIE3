#ifndef _bitcoinMine_h_
#define  _bitcoinMine_h_

#include "messages.h"
#include "hash/SHA256.h"
#include "rtos.h"
#include "mbed.h"
#include "decodeCommands.h"

extern uint8_t sequence[64];
extern volatile uint64_t* key;
extern uint64_t* nonce;
extern uint8_t hash[32];
extern volatile uint32_t hash_counter;

extern void countHash();
extern void computeHash();

#endif
