#define HASH
//#define PARSE
#include "motorControl.h"
#include "bitcoinMine.h"
#include "mbed.h"
#include "rtos.h"
#include "messages.h"

//#define SERIAL_TX D1
//#define SERIAL_RX D0

//Mapping from sequential drive states to motor phase outputs
/*
   State   L1  L2  L3
   0       H   -   L
   1       -   H   L
   2       L   H   -
   3       L   -   H
   4       -   L   H
   5       H   L   -
   6       -   -   -
   7       -   -   -
 */

// THREADS
Thread hashThread;

// Thread to decode the incoming instructions
Thread decodeThread;

//////// /C SERIAL COMMUNCATION OBJECTS /////////
// Thread to run hash
Thread commOutT;

// RawSerial object to write to serial port
//RawSerial pc(SERIAL_TX, SERIAL_RX);

//////// /E SERIAL COMMUNCATION OBJECTS /////////

// The output sequence determines the type of the output
// 1 --- FLOAT
// 0 --- INT


int main() {
        // Start the serial communication thread
        commOutT.start(commOutFn);
        pc.printf("Beginning the program!\n\r");


        pinInit();
        putMessage(0x01, 0x35);

        // This is the buffer to hold the input commands
        static char buffer[24];
        static uint8_t count = 0;

        // Start the decode Thread
        decodeThread.start(decode);


        int8_t orState = 0;
        int8_t intState = 0;
        int8_t intStateOld = 0;

        //Run the motor synchronisation
        motorHome();
        orState = state;

        pc.printf("Rotor origin: %x\n\r",orState);

        // Initialize the timer for the hash calculation
        // Begin the thread for hash calculation
        #ifdef HASH
        Ticker t;
        t.attach(&countHash, 1.0);
        hashThread.start(computeHash);
        hashThread.set_priority(osPriorityLow);
        #endif

        //motorDrive.attach(&motorDrive,0.1);

        //Poll the rotor state and set the motor outputs accordingly to spin the motor
        while (1) {
            intState = state;
            if (intState != intStateOld) {
                intStateOld = intState;
                motorOut((intState-orState+lead+6)%6,1.0); //+6 to make sure the remainder is positive
            }
        }
}
