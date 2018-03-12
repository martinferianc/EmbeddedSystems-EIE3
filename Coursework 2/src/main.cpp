#include "motorControl.h"
#include "bitcoinMine.h"
#include "mbed.h"
#include "rtos.h"
#include "messages.h"
#include "decodeCommands.h"

// THREADS
Thread hashThread;
Thread decodeThread;
Thread commOutT;
Thread motorRunT;

int main() {
        // Start the serial communication thread
        pc.printf("Beginning the program!\n\r");

        putMessage(0x01, 0x35);

        // THREADS
        decodeThread.start(decode);
        commOutT.start(commOutFn);
        hashThread.start(computeHash);
        motorRunT.start(motorRun);

        //THREAD PRIORITY
        hashThread.set_priority(osPriorityLow);

        /*
        //PROFILING
        Ticker t;
        t.attach(&countHash, 1.0);
        */

        while (1);
}
