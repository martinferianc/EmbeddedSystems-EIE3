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

volatile float act_velocity    = 0;
volatile int32_t motorPosition   = 0;
volatile float act_rotations  = 0;

static int8_t orState; //declaring outside function to be shared by others
static int32_t oldRotationError = 0;
volatile int32_t integralError = 0;

// MOTOR VELOCITY VARIABLES
volatile int8_t vel_count = 0;

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

// Returns the state of the motor when called

inline uint8_t readRotorState(){
        return stateMap[I1 + 2*I2 + 4*I3];
}

// Basic synchronisation routine
// Drives the motor to a deafult position, waits for it to arrive and updates
// the motor state.

void motorHome() {
        //Put the motor in drive state 0 and wait for it to stabilise
        motorOut(0,1000);
        wait(2);
        motorPosition = readRotorState(); //initialises oldRotorState
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

void setISRPhotoSensors(){
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
        static int8_t oldRotorState= 0;
        int8_t rotorState = readRotorState();
        motorOut((rotorState - orState + lead + 6)%6, motorPWM);
        if(rotorState - oldRotorState == 5) motorPosition--;
        else if (rotorState - oldRotorState == -5) motorPosition++;
        else motorPosition += (rotorState - oldRotorState); //is it meant to be (rotorState = oldRotorState) or (rotorState - oldRotorState) ?
        oldRotorState = rotorState;
        orState = rotorState;
}

// Function to instantiate thread which will run every 100ms to calcualte
// the velocity at this time.
void motorCtrlFn(){
        motorHome();      // Home the motor
        //static int32_t oldMotorPosition;
        Ticker motorCtrlTicker; // Used to control how often motor control thread runs
        motorCtrlTicker.attach_us(&motorCtrlTick, 100000);
        while(1) {
                motorCtrlT.signal_wait(0x1); // Suspend until signal occurs.
                act_velocity = 10*(motorPosition - orState); // Calculate the velocity of the motor.
                //orState = motorPosition; // Update the motor position
                //if(act_velocity == 0 && (tar_velocity || rotations)) motorISR(); //need this for start up
                motorVelocityController();
                //motorRotorController();
                //motorPWM = 1000;
                if(vel_count == 0) {
                        vel_count = MVELOCITY_PRINT_FREQUENCY;
                        putMessage(VELOCITY, act_velocity); // Print the velocity
                        putMessage(TAR_VELOCITY, tar_velocity); // Print the velocity
                }
                vel_count -= 1;
        }
}

// ISR which will be called each time 100ms has passed. Performs no computation
// so Photointerrupter is not blocked.
void motorCtrlTick(){
        motorCtrlT.signal_set(0x1); // Set signal to calculate velocity
}

//Controller for motor velocity
void motorVelocityController()
{
        // y_s = k_p(s-|v|)
        int32_t y_s;
        int32_t y_r;
        static int32_t oldVelocityError;


        //If the target velocity is 0 reverse the directon
        if (tar_velocity<0) {
                lead = -2;
        } else {
                lead = 2;
        }

        //Error term
        int32_t velocityError = tar_velocity - act_velocity;

        //integral error
        integralError += INTEGRAL_CONST * velocityError;
        if(integralError > INTEGRAL_ERR_MAX) integralError=INTEGRAL_ERR_MAX;
        if(integralError < INTEGRAL_ERR_MAX) integralError=-INTEGRAL_ERR_MAX;

        y_s = PROPORTIONAL_CONST*(tar_velocity - abs(act_velocity)) + integralError;


        oldVelocityError = velocityError;

        //if 0, set lead to 0
        //TODO: I think y needs to have modulus taken
        y_s = (y_s > PWM_LIMIT) ? (y_s = PWM_LIMIT) :
              (y_s <-PWM_LIMIT) ? (y_s =-PWM_LIMIT) :
              y_s;

        motorPWM = (uint32_t)y_s;
}

/*
   // Implement the proportional speed control
   void motorRotorController(){
        int32_t y_r;
        //Error term
        int32_t rotationError = rotations - motorPosition;

        y_r = PROPORTIONAL_CONST*(rotationError) + DIFFERENTIAL_CONST*(rotationError - oldRotationError); // Need to divide by time

        oldRotationError = rotationError;

        //changes direction if overshoots
        lead = (y_r > 0) ?  2 :
               -2;

        //sets y to within max/min limits
        y_r = (y_r > PWM_LIMIT) ? (y_r = PWM_LIMIT) :
              y_r;

        //stops rotating of no more (integral) error
        //if(rotationIntegralError==0) lead = 0;

        motorPWM = (uint32_t)y_r;
   }
 */
