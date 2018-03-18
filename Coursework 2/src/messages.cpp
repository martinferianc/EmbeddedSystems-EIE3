#include "messages.h"

//////// /C SERIAL COMMUNCATION OBJECTS /////////
// Mail handles the message queue.
Mail<message_t, 16> outMessages;

// RawSerial object to write to serial port
RawSerial pc(SERIAL_TX, SERIAL_RX);

//////// /E SERIAL COMMUNCATION OBJECTS /////////

//////// /C SERIAL FUNCTION DEFINITIONS /////////

// putMessage() takes a message and a unique code, allocates the memory
// for the message and places the message in the FIFO mail queue

void putMessage(uint8_t code, uint32_t data){
        message_t *pMessage = outMessages.alloc();
        pMessage->code = code;
        pMessage->data = data;
        outMessages.put(pMessage);
}


// commOutFn() checks to see if there is a new message in the queue. If there is
// it fetches the pointer to it, prints its code and the data stored and then frees
// the memory allocated to it.

void commOutFn(){
        while(1) {
                osEvent newEvent = outMessages.get();
                message_t *pMessage = (message_t*)newEvent.value.p;
                switch(pMessage->code) {
                case (STARTUP):
                        pc.printf("STARTUP COMPLETE %d, %d \n", pMessage->code, pMessage->data);
                        break;
                case (ROTATE):
                        pc.printf("R 0x%016x\n", pMessage->code, pMessage->data);
                        break;
                case (ROTOR_STATE):
                        pc.printf("State for %d with data 0x%016x\r\n", pMessage->code, pMessage->data);
                        break;
                case (VELOCITY):
                        pc.printf("Motor Velocity: %d rev/s\r\n", pMessage->data);
                        break;
                case (HASH):
                        pc.printf("Bitcoin Hashes per second: %d\r\n", pMessage->data);
                        break;
                case (NONCE):
                        pc.printf("Bitcoin Nonce: 0x%016x\r\n", pMessage->data);
                        break;
                case (KEY):
                        pc.printf("New Key: 0x%016x\r\n", pMessage->data);
                        break;
                case (TAR_VELOCITY):
                        pc.printf("Target Velocity: %d rev/s\r\n", pMessage->data);
                        break;
                }
                outMessages.free(pMessage);
        }
}
