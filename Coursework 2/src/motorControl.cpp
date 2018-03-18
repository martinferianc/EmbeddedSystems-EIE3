#include "motorControl.h"


// MOTOR STATE VARIABLES
//Drive state to output table
const int8_t driveTable[] = {0x12,0x18,0x09,0x21,0x24,0x06,0x00,0x00};
//Mapping from interrupter inputs to sequential rotor states. 0x00 and 0x07 are not valid
const int8_t stateMap[] = {0x07,0x05,0x03,0x04,0x01,0x00,0x02,0x07};
volatile int8_t state;
//const int8_t stateMap[] = {0x07,0x01,0x03,0x02,0x05,0x00,0x04,0x07}; //Alternative if phase order of input or drive is reversed

// MOTOR TORQUE VARIABLES
volatile int8_t lead = 2;  //2 for forwards, -2 for backwards

// MOTOR POSITION VARIABLES
uint8_t direction = 1; // 1: forward, 2: backward
volatile int32_t act_velocity;
volatile int32_t motorPosition;
volatile int32_t rotations_curr = 0;
volatile int32_t rotations_prev = 0;


// MOTOR VELOCITY VARIABLES
volatile int8_t vel_count = 0;

// TIMER INITIALISATIONS
//Ticker motorDrive;
//Ticker velocityT;

//Status LED
DigitalOut led1(LED1);

//Photointerrupter inputs
InterruptIn I1(I1pin);
InterruptIn I2(I2pin);
InterruptIn I3(I3pin);

//Motor Drive outputs
PwmOut L1L(L1Lpin);
DigitalOut L1H(L1Hpin);
PwmOut L2L(L2Lpin);
DigitalOut L2H(L2Hpin);
PwmOut L3L(L3Lpin);
DigitalOut L3H(L3Hpin);


//Set a given drive stats
void motorOut(int8_t driveState, uint32_t scale){

        //Lookup the output byte from the drive state.
        int8_t driveOut = driveTable[driveState & 0x07];

        //Turn off first
        if (~driveOut & 0x01) L1L.pulsewidth_us(0);
        if (~driveOut & 0x02) L1H = 1;
        if (~driveOut & 0x04) L2L.pulsewidth_us(0);
        if (~driveOut & 0x08) L2H = 1;
        if (~driveOut & 0x10) L3L.pulsewidth_us(0);
        if (~driveOut & 0x20) L3H = 1;

        //Then turn on
        if (driveOut & 0x01) L1L.pulsewidth_us(scale);
        if (driveOut & 0x02) L1H = 0;
        if (driveOut & 0x04) L2L.pulsewidth_us(scale);
        if (driveOut & 0x08) L2H = 0;
        if (driveOut & 0x10) L3L.pulsewidth_us(scale);
        if (driveOut & 0x20) L3H = 0;
}

// Updates the state of the motor based upon the light sensors.

inline void updateState(){
        state = stateMap[I1 + 2*I2 + 4*I3];
        if(direction) rotations++;
        else rotations--;
}

// Returns the state of the motor when called

const uint8_t readRotorState(){
        return stateMap[I1 + 2*I2 + 4*I3];
}

// Basic synchronisation routine
// Drives the motor to a deafult position, waits for it to arrive and updates
// the motor state.

void motorHome() {
        //Put the motor in drive state 0 and wait for it to stabilise
        motorOut(0,1000);
        wait(2);
        updateState();
        return;
}

// Set the PWM period
// NOTE: Due to a hardware fault on the board, this should be set to a maximum
// of 2000 uS.

void setPWMPeriod(int period) {
        L1L.period_us(period);
        L2L.period_us(period);
        L3L.period_us(period);
        return;
}

// Copy of pinInit to run with code Ed provided. Calls the motor ISR each time
// one of the state of the light sensor changes

void photoISRSetup(){
        I3.rise(&motorISR);
        I2.rise(&motorISR);
        I1.rise(&motorISR);
        I3.fall(&motorISR);
        I2.fall(&motorISR);
        I1.fall(&motorISR);
        return;
}

// ISR to handle the updating of the motor position

void motorISR(){
        static int8_t oldRotorState;
        int8_t rotorState = readRotorState();
        motorPower();
        motorOut((rotorState - oldRotorState + lead + 6)%6, motorPWM);
        if(rotorState - oldRotorState == 5) motorPosition--;
        else if (rotorState - oldRotorState == -5) motorPosition++;
        else motorPosition += (rotorState = oldRotorState);
        oldRotorState = rotorState;
}


// Function to instantiate thread which will run every 100ms to calcualte
// the velocity at this time.

void motorCtrlFn(){
        photoISRSetup();   // Attach the motorISR to the pins
        motorHome();      // Home the motor
        static int32_t oldMotorPosition;
        Ticker motorCtrlTicker; // Used to control how often motor control thread runs
        motorCtrlTicker.attach_us(&motorCtrlTick, 100000);
        while(1) {
                motorCtrlT.signal_wait(0x1); // Suspend until signal occurs.
                act_velocity = motorPosition - oldMotorPosition; // Calculate the velocity of the motor.
                oldMotorPosition = motorPosition; // Update the motor position
                if(vel_count == 0) {
                        vel_count = MVELOCITY_PRINT_FREQUENCY;
                        //putMessage(VELOCITY, act_velocity); // Print the velocity
                        //putMessage(TAR_VELOCITY, tar_velocity); // Print the velocity
                }
                vel_count -= 1;
        }
}

// ISR which will be called each time 100ms has passed. Performs no computation
// so Photointerrupter is not blocked.

void motorCtrlTick(){
        motorCtrlT.signal_set(0x1); // Set signal to calculate velocity
}

// Implement the proportional speed control
void motorPower(){
  // y_s = k_p(s-|v|)
  int32_t y_s;
  static int32_t oldVelocityError;
  int32_t velocityError = tar_velocity - act_velocity;

  // take the absolute value;
  act_velocity = (act_velocity < 0) ? (0 - act_velocity) : act_velocity;

  y_s = PROPORTIONAL_CONST*(velocityError); //+ DIFFERENTIAL_CONST*(velocityError - oldVelocityError); // Need to divide by time

  oldVelocityError = velocityError;
  if(y_s < 0){
      y_s = y_s*-1;
      lead = lead*-1;
  }
  y_s = (y_s > PWM_LIMIT) ? (y_s = PWM_LIMIT) : y_s;
  motorPWM = (uint32_t)y_s;
}


//############# Code below this line was written by Alex MC. Now works from code provided by Ed. ##############

/*
void pinInit() {
        I3.rise(&updateState);
        I2.rise(&updateState);
        I1.rise(&updateState);
        I3.fall(&updateState);
        I2.fall(&updateState);
        I1.fall(&updateState);
        return;
}

void velocityCalc() {
        rotations_curr = rotations;
        act_velocity = (rotations_curr-rotations_prev)*10;
        rotations_prev = rotations_curr;
        vel_count++;
        if(vel_count>=10) {
                vel_count = 0;
                putMessage(VELOCITY, act_velocity);
        }
}

void measureInit() {
        velocityT.attach(&velocityCalc,0.1);
}

void motorRun() {
        pinInit();
        measureInit();

        int8_t orState = 0;
        int8_t intState = 0;
        int8_t intStateOld = 0;

        //Run the motor synchronisation
        motorHome();
        orState = state;

        putMessage(ROTOR_STATE, orState);

        while (1) {
                intState = state;
                if (intState != intStateOld) {
                        intStateOld = intState;
                        motorOut((intState-orState+lead+6)%6,1.0); //+6 to make sure the remainder is positive
                }
        }
}

*/
