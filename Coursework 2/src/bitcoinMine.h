#ifndef _bitcoinMine_h_
#define  _bitcoinMine_h_

#include "decodeCommands.h"
#include "hash/SHA256.h"
#include "rtos.h"
#include "mbed.h"

uint8_t sequence[];
extern volatile uint64_t* key;
uint64_t* nonce;
uint8_t hash[32];
volatile uint32_t hash_counter;

extern void countHash();
extern void computeHash();

#endif
