#define HASH
//#define PARSE
#include "motorControl.h"
#include "bitcoinMine.h"

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

int main() {
        Serial pc(SERIAL_TX, SERIAL_RX);
        pc.printf("Beginning the program!\n\r");

        pinInit();        

        // This is the buffer to hold the input commands
        static char buffer[24];
        static uint8_t count = 0;

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
