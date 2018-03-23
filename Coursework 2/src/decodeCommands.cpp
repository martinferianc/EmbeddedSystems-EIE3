#include "decodeCommands.h"

// Global Queueing variable
Queue<void, 8> inCharQ;

// Buffer for holding chars to decode
char charBuffer[17];

// Buffer index
int charBufferCounter = 0;

// Key for testing the motor torque
volatile uint32_t motorPWM = 0;

// Motor control variables
volatile float tar_velocity = 0.0;
volatile float tar_rotations = 0.0;
volatile float tar_rotations_tmp = 0.0;

//Key
volatile uint64_t new_key;
volatile bool new_key_set = false;


// Mutex for the new key
Mutex key_mutex;

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

                // Check for the buffer index, prevent overflow
                if(charBufferCounter > 18) {
                        charBufferCounter = 0;
                }
                else{
                        charBuffer[charBufferCounter] = newChar;
                }
                // Start decoding
                if(newChar == '\r') {
                        charBuffer[charBufferCounter] = '\0';
                        // Reset for the next command

                        charBufferCounter = 0;

                        // Test for the first character
                        switch(charBuffer[0]) {
                        // ROTATIONS
                        case 'R':
                                sscanf(charBuffer, "R%f", &tar_rotations_tmp);
                                putMessage(TAR_ROTATION_SET,*(int32_t*)&tar_rotations_tmp);
                                tar_rotations = ((float)motorPosition)/6 + tar_rotations_tmp;
                                break;
                        // VELOCITY
                        case 'V':
                                sscanf(charBuffer, "V%f", &tar_velocity);
                                putMessage(TAR_VELOCITY_SET,*(int32_t*)&tar_velocity);
                                tar_velocity = (tar_velocity == 0) ? 2000 : tar_velocity;
                                break;
                        // KEY
                        case 'K':
                                key_mutex.lock();
                                sscanf(charBuffer,"K%x",&new_key);
                                key_mutex.unlock();
                                putMessage(KEY_UPPER, (uint32_t)((new_key>>32)&0xFFFFFFFF));
                                putMessage(KEY_LOW, (uint32_t)(new_key&0xFFFFFFFF));
                                new_key_set = true;
                                break;
                        //TORQUE
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
