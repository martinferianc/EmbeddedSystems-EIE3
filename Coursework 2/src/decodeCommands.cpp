#include "decodeCommands.h"

// Global Queueing variable
Queue<uint8_t, 8> inCharQ;
// Buffer for holding chars to decode
char charBuffer[17];
// buffer index
int charBufferCounter = 0;

// New key from serial port for the bitcoin miner
volatile uint64_t newKey;

// Key for testing the motor torque
volatile uint32_t torqueKey;

// mutex for the new key
Mutex key_mutex;

void serialISR(){
        uint8_t newChar = pc.getc();
        inCharQ.put((uint8_t*)newChar);
}

// Decoding
void decode(){
        pc.attach(&serialISR);
        while(1) {
                osEvent newEvent = inCharQ.get();
                uint8_t newChar = (uint8_t)newEvent.value.v;
                //putMessage(1, newChar);
                // check for the buffer index, prevent overflow
                if(charBufferCounter > 17) {
                        charBufferCounter = 0;
                }
                if(newChar == '\r') {
                        charBuffer[charBufferCounter] = '\0';
                        // reset to read next command
                        charBufferCounter = 0;
                        // test the first character
                        switch(charBuffer[0]) {
                        case 'R': break; //max_rotations
                        case 'V': break;
                        case 'K':
                                key_mutex.lock();
                                sscanf(charBuffer,"K%x",key);
                                putMessage(ROTATE, 0xFF);
                                key_mutex.unlock();
                                break;
                        case 'T':
                                sscanf(charBuffer, "T%x", &torqueKey);
                                break;

                        }

                }
                charBuffer[charBufferCounter] = newChar;
                charBufferCounter++;
        }
}
