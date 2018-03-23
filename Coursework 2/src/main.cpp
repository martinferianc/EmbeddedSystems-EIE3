#include "motorControl.h"
#include "bitcoinMine.h"
#include "mbed.h"
#include "rtos.h"
#include "messages.h"
#include "decodeCommands.h"

#define DEBUG 0

// THREADS
Thread decodeThread(osPriorityNormal,1536);
Thread commOutT(osPriorityNormal,1536);
Thread motorCtrlT (osPriorityHigh,1024); // Thread to hold task running every 100ms

int main() {
        // Set the constants
        putMessage(STARTUP, 13);

        // Set the constraints
        setPWMPeriod(2000);
        setISRPhotoSensors();

        // Initialize threads
        decodeThread.start(decode);
        commOutT.start(commOutFn);
        motorCtrlT.start(motorCtrlFn);

        // Profiling
        if (!DEBUG) {
                Ticker t;
                t.attach(&countHash, 1.0);
                while (1) {
                        computeHash();
                }
        }
}
