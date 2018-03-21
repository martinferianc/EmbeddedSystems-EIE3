#include "motorControl.h"
#include "bitcoinMine.h"
#include "mbed.h"
#include "rtos.h"
#include "messages.h"
#include "decodeCommands.h"

#define DEBUG 1 

// THREADS
Thread decodeThread(osPriorityNormal,1024);
Thread commOutT(osPriorityNormal,1024);
Thread motorCtrlT (osPriorityHigh,1024); // Thread to hold task running every 100ms

int main() {
        // Set the constants and everything
        setPWMPeriod(2000);
        setISRPhotoSensors();

        // THREADS
        decodeThread.start(decode);
        commOutT.start(commOutFn);
        motorCtrlT.start(motorCtrlFn);

        //PROFILING
        if (DEBUG!=1) {
                Ticker t;
                t.attach(&countHash, 1.0);
                while (1) {
                        computeHash();
                }
        }
}
