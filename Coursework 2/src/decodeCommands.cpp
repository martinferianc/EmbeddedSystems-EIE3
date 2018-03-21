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
volatile int32_t tar_velocity = 0;
volatile int32_t tar_rotations = 0;
volatile int32_t tar_rotations_tmp = 0;

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
                                //rotations_mutex.lock();
                                sscanf(charBuffer, "R%d", &tar_rotations_tmp);
                                putMessage(TAR_ROTATION_SET, tar_rotations_tmp);
                                tar_rotations += 6*tar_rotations_tmp;
                                //rotations_mutex.unlock();
                                break;
                        case 'V':
                                //pc.printf("VELOCITY");
                                //velocity_mutex.lock();
                                sscanf(charBuffer, "V%d", &tar_velocity);
                                putMessage(TAR_VELOCITY_SET, tar_velocity);
                                tar_velocity = (tar_velocity == 0) ? 2000 : 6*tar_velocity;
                                //velocity_mutex.unlock();
                                break;
                        case 'K':
                                key_mutex.lock();
                                sscanf(charBuffer,"K%x",&key);
                                key_mutex.unlock();
                                break;
                        case 'T':
                                sscanf(charBuffer, "T%d", &motorPWM);
                                putMessage(TORQUE_TEST, motorPWM);
                                break;
                        default:
                                break;

                        }

                }
                else{
                        charBufferCounter++;
                }

        }
}
