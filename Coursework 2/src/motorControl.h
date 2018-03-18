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

// MOTOR STATE VARIABLES
//Drive state to output table
extern const int8_t driveTable[];
//Mapping from interrupter inputs to sequential rotor states. 0x00 and 0x07 are not valid
extern const int8_t stateMap[];
extern volatile int8_t state;
//const int8_t stateMap[] = {0x07,0x01,0x03,0x02,0x05,0x00,0x04,0x07}; //Alternative if phase order of input or drive is reversed

//MOTOR TORQUE VARIABLES
extern const int8_t lead;  //2 for forwards, -2 for backwards

//MOTOR POSITION VARIABLES;
extern volatile int32_t rotations;

//Set a given drive state
extern void motorOut(int8_t driveState, uint32_t scale);

extern inline void updateState();

//Basic synchronisation routine
extern void motorHome();

extern void photoISRSetup();

extern void setPWMPeriod(int period);

extern void pinInit();

extern void motorRun();

extern void motorISR();

extern void measureInit();

extern void motorCtrlFn();

extern void motorCtrlTick();

extern volatile uint32_t motorTorque;
extern volatile int32_t tar_velocity;
extern volatile int32_t rotations;

extern Thread motorCtrlT;

#endif
