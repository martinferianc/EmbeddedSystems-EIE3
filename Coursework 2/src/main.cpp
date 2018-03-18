#include "motorControl.h"
#include "bitcoinMine.h"
#include "mbed.h"
#include "rtos.h"
#include "messages.h"
#include "decodeCommands.h"

// THREADS
Thread decodeThread(osPriorityHigh);
Thread commOutT;
Thread motorCtrlT (osPriorityNormal, 1024); // Thread to hold task running every 100ms

int main() {
        // Start the serial communication thread

        putMessage(STARTUP, 13);

        setPWMPeriod(2000);

        // THREADS
        decodeThread.start(decode);
        commOutT.start(commOutFn);
        motorCtrlT.start(motorCtrlFn);
        //THREAD PRIORITY

        //PROFILING
        //Ticker t;
        //t.attach(&countHash, 1.0);
        //computeHash();
}
