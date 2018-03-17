#include "decodeCommands.h"

// Global Queueing variable
Queue<void, 8> inCharQ;
// Buffer for holding chars to decode
char charBuffer[17];
// buffer index
int charBufferCounter = 0;

// Key for testing the motor torque
volatile uint32_t torqueKey;

//Motor control variables
volatile int32_t velocity = 0;
volatile int32_t rotations = 0;

// mutex for the new key
Mutex key_mutex;
Mutex velocity_mutex;
Mutex rotations_mutex;

void serialISR(){
        uint8_t newChar = pc.getc();
        inCharQ.put((void*)newChar);
}

// Decoding
void decode(){
        pc.attach(&serialISR);
        while(1) {
                osEvent newEvent = inCharQ.get();
                uint8_t newChar = (uint8_t)newEvent.value.p;

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
                        case 'R':
                                rotations_mutex.lock();
                                sscanf(charBuffer, "R%lf", &rotations);
                                putMessage(ROTATE, rotations);
                                rotations_mutex.unlock();
                                break;
                        case 'V':
                                velocity_mutex.lock();
                                sscanf(charBuffer, "V%lf", &velocity);
                                putMessage(VELOCITY, velocity);
                                velocity_mutex.unlock();
                                break;
                        case 'K':
                                key_mutex.lock();
                                sscanf(charBuffer,"K%x",&key);
                                putMessage(KEY, velocity);
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
