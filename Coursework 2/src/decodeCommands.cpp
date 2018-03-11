/*
#include "decodeCommands.h"

enum outputCodes{
  ROTATE,
  VELOCITY,
  KEY,
  TUNE
};

void decode(){
  pc.attach(&serialISR);
  while(1){
    osEvent newEvent = inCharQ.get();
    uint8_t newChar = (uint8_t)newEvent.value.p;
    // check for the buffer index, prevent overflow
    if(charBufferCounter > 17){
      charBufferCounter = 0;
    }
    if(newChar == '\r'){
      charBuffer[charBufferCounter] = '\0';
      // reset to read next command
      charBufferCounter = 0;
      // test the first character
      switch(charBuffer[0]){
        case 'R': break; //max_rotations
        case 'V': break;
        case 'K': newKey_mutex.lock();
                  sscanf(charBuffer,"K%x",&newKey);
                  putMessage(ROTATE, 0xFF);
                  newKey_mutex.unlock();
                  break;
        case 'T': break;

      }

    }
    charBuffer[charBufferCounter] = newChar;
    charBufferCounter++;
  }
}
*/
