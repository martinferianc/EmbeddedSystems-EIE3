#include "decodeCommands.h"

// Global Queueing variable
Queue<void, 8> inCharQ;
// Buffer for holding chars to decode
char charBuffer[17];
// buffer index
int charBufferCounter = 0;

// Key for testing the motor torque
volatile uint32_t motorPWM = 0;

volatile uint32_t err = 0;

//Motor control variables
volatile float tar_velocity = 0;
volatile float tar_rotations = 0;


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
                if(charBufferCounter > 18) {
                        charBufferCounter = 0;
                }
                else{
                        charBuffer[charBufferCounter] = newChar;
                }
                if(newChar == '\r') {
                        charBuffer[charBufferCounter] = '\0';
                        // reset to read next command
                        charBufferCounter = 0;
                        // test the first character
                        switch(charBuffer[0]) {
                        case 'R':
                                rotations_mutex.lock();
                                sscanf(charBuffer, "R%f", &tar_rotations);
                                rotations_mutex.unlock();
                                break;
                        case 'V':
                                //pc.printf("VELOCITY");
                                velocity_mutex.lock();
                                sscanf(charBuffer, "V%f", &tar_velocity);
                                tar_velocity = (tar_velocity == 0) ? 2000 : tar_velocity;
                                velocity_mutex.unlock();
                                putMessage(TAR_VELOCITY, tar_velocity);
                                break;
                        case 'K':
                                key_mutex.lock();
                                sscanf(charBuffer,"K%x",&key);
                                key_mutex.unlock();
                                putMessage(KEY, key);
                                break;
                        case 'T':
                                sscanf(charBuffer, "T%d", &motorPWM);
                                break;
                        default:
                                putMessage(ERROR, &err);
                                break;

                        }

                }
                else{
                        charBufferCounter++;
                }

        }
}
