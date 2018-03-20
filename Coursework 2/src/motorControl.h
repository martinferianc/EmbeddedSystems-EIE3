#ifndef _motorControl_h_
#define _motorControl_h_

#include "mbed.h"
#include "rtos.h"
#include "messages.h"
#include "decodeCommands.h"

//Photointerrupter input pins
#define I1pin D2
#define I2pin D11
#define I3pin D12

//Incremental encoder input pins
#define CHA   D7
#define CHB   D8

//Motor Drive output pins   //Mask in output byte
#define L1Lpin D4           //0x01
#define L1Hpin D5           //0x02
#define L2Lpin D3           //0x04
#define L2Hpin D6           //0x08
#define L3Lpin D9           //0x10
#define L3Hpin D10          //0x20

#define MVELOCITY_PRINT_FREQUENCY 10    // How often motor velocity is printed

// Parameters for proportional control
#define PWM_LIMIT 1000
#define PROPORTIONAL_CONST 30
#define DIFFERENTIAL_CONST 20
#define INTEGRAL_CONST     5
#define REFERENCE_CONST    1
#define INTEGRAL_ERR_MAX   100
//Set a given drive state
extern void motorOut(int8_t driveState, uint32_t scale);

extern inline void updateState();

//Basic synchronisation routine
extern void motorHome();

extern void setISRPhotoSensors();

extern void setPWMPeriod(int period);

extern void pinInit();

extern void motorRun();

extern void motorISR();

extern void motorPower();

extern void measureInit();

extern void motorCtrlFn();

extern void motorCtrlTick();

void motorVelocityController();

void motorRotationCotroller();

extern Thread motorCtrlT;

void motorRotorController();

#endif
