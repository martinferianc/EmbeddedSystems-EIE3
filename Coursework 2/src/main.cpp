#include "motorControl.h"
#include "bitcoinMine.h"
#include "mbed.h"
#include "rtos.h"
#include "messages.h"
#include "decodeCommands.h"

// THREADS
Thread decodeThread;
Thread commOutT;
//Thread motorDriveT;
Thread motorCtrlT (osPriorityNormal, 1024); // Thread to hold task running every 100ms



int main() {
        // Start the serial communication thread

        setPWMPeriod(2000);

        // THREADS
        decodeThread.start(decode);
        commOutT.start(commOutFn);
        motorCtrlT.start(motorCtrlFn);
        //motorCtrlT.start(motorCtrlFn);
        //THREAD PRIORITY

        //PROFILING
        Ticker t;
        t.attach(&countHash, 1.0);
        computeHash();
}
