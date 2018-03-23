#include "messages.h"

// Mail handles the message queue.
Mail<message_t, 16> outMessages;

// RawSerial object to write to serial port
RawSerial pc(SERIAL_TX, SERIAL_RX);

// putMessage() takes a message and a unique code, allocates the memory
// for the message and places the message in the FIFO mail queue
void putMessage(uint8_t code, int32_t data){
        message_t *pMessage = outMessages.alloc();
        pMessage->code = code;
        pMessage->data = data;
        outMessages.put(pMessage);
}

// commOutFn() checks to see if there is a new message in the queue. If there is
// It fetches the pointer to it, prints its code and the data stored and then frees
// the memory allocated to it.
void commOutFn(){
        while(1) {
                // Get a new event from the queue
                osEvent newEvent = outMessages.get();
                message_t *pMessage = (message_t*)newEvent.value.p;

                switch(pMessage->code) {
                case (STARTUP):
                        pc.printf("STARTUP COMPLETE %d, %d \r\n", pMessage->code, pMessage->data);
                        break;
                case (ROTATE):
                        pc.printf("R 0x%016x\r\n", pMessage->code, pMessage->data);
                        break;
                case (ROTOR_STATE):
                        pc.printf("State for %d with data 0x%016x\r\n", pMessage->code, pMessage->data);
                        break;
                case (VELOCITY):
                        pc.printf("Motor Velocity: %f rev/s\r\n", *(float*)&(pMessage->data));
                        break;
                case (HASH):
                        pc.printf("Bitcoin Hashes per second: %d\r\n", pMessage->data);
                        break;
                case (NONCE_LOW):
                        pc.printf("%x\r\n", pMessage->data);
                        break;
                case (NONCE_UPPER):
                        pc.printf("Bitcoin Nonce: 0x%x", pMessage->data);
                        break;
                case (KEY_UPPER):
                        pc.printf("New Key:  0x%x", pMessage->data);
                        break;
                case (KEY_LOW):
                        pc.printf("%x\r\n", pMessage->data);
                        break;
                case (TAR_VELOCITY):
                        pc.printf("Target Velocity: %f rev/s\r\n", *(float*)&(pMessage->data));
                        break;
                case (TEST_MSG):
                        pc.printf("Test: %d \r\n", pMessage->data);
                        break;
                case (ROTATION):
                        pc.printf("Rotation: %f \r\n", *(float*)&(pMessage->data));
                        break;
                case (TAR_ROTATION):
                        pc.printf("Target Rotation: %f \r\n", *(float*)&(pMessage->data));
                        break;
                case (TAR_ROTATION_SET):
                        pc.printf("Target Rotation SET: %f \r\n", *(float*)&(pMessage->data));
                        break;
                case (TAR_VELOCITY_SET):
                        pc.printf("Target Velocity SET: %f rev/s\r\n", *(float*)&(pMessage->data));
                        break;
                case (TORQUE_TEST):
                        pc.printf("Torque SET: %d rev/s\r\n", pMessage->data);
                        break;
                case (HEX_TEST):
                        pc.printf("HEX TEST: %x\r\n", pMessage->data);
                        break;
                default:
                        pc.printf("Unknown code, data: %d \r\n", pMessage->data);
                        break;
                }
                outMessages.free(pMessage);
        }
}
