#ifndef _motorControl_h_
#define _motorControl_h_

#include "rtos.h"

extern Thread motorCtrlT (osPriorityNormal,1024); //, 1024); // Thread to hold task running every 100ms

#endif
