#include "motorControl.h"

// MOTOR STATE VARIABLES
//Drive state to output table
const int8_t driveTable[] = {0x12,0x18,0x09,0x21,0x24,0x06,0x00,0x00};
//Mapping from interrupter inputs to sequential rotor states. 0x00 and 0x07 are not valid
const int8_t stateMap[] = {0x07,0x05,0x03,0x04,0x01,0x00,0x02,0x07};
volatile int8_t state;

// MOTOR TORQUE VARIABLES
volatile int8_t lead = 2;  //2 for forwards, -2 for backwards

volatile int32_t act_velocity    = 0;
volatile int32_t act_rotations   = 0;
volatile int32_t motorPosition   = 0;
volatile uint32_t orState = 0;

volatile int32_t integralVelocityError = 0;
volatile int32_t prevVelocityError = 0;

volatile int32_t integralRotationsError = 0;
volatile int32_t prevRotationError = 0;

// MOTOR VELOCITY VARIABLES
volatile int8_t count= 0;


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
void motorOut(int8_t driveState){

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
        if (driveOut & 0x01) L1L.pulsewidth_us(motorPWM);
        if (driveOut & 0x02) L1H = 0;
        if (driveOut & 0x04) L2L.pulsewidth_us(motorPWM);
        if (driveOut & 0x08) L2H = 0;
        if (driveOut & 0x10) L3L.pulsewidth_us(motorPWM);
        if (driveOut & 0x20) L3H = 0;
}

// Returns the state of the motor when called

inline int8_t readRotorState(){
        return stateMap[I1 + 2*I2 + 4*I3];
}

// Basic synchronisation routine
// Drives the motor to a deafult position, waits for it to arrive and updates
// the motor state.

void motorHome() {
        //Put the motor in drive state 0 and wait for it to stabilise
        motorPWM = 0;
        motorOut(0);
        wait(2);
        orState = readRotorState(); //initialises oldRotorState
        return;
}

int32_t calculateActualRotations(){
        act_rotations = motorPosition;
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
        static int8_t oldRotorState = 0;
        int8_t rotorState = readRotorState();
        motorOut((rotorState - orState + lead + 6)%6);
        if(rotorState - oldRotorState == 5) motorPosition--;
        else if (rotorState - oldRotorState == -5) motorPosition++;
        else motorPosition += (rotorState - oldRotorState);
        oldRotorState = rotorState;
}

// Function to instantiate thread which will run every 100ms to calcualte
// the velocity at this time.
void motorCtrlFn(){
        motorHome();      // Home the motor
        Ticker motorCtrlTicker; // Used to control how often motor control thread runs
        int32_t oldMotorPosition = 0;
        int32_t currMotorPosition = 0;
        uint32_t motorPWM_rot;
        uint32_t motorPWM_vel;
        motorCtrlTicker.attach_us(&motorCtrlTick, 100000);
        //motorPWM = 500;
        while(1) {
                motorCtrlT.signal_wait(0x1); // Suspend until signal occurs.
                currMotorPosition = motorPosition;
                act_velocity = (currMotorPosition - oldMotorPosition); // Calculate the velocity of the motor.
                oldMotorPosition = currMotorPosition; // Update the motor position
                act_rotations   if= currMotorPosition;

                //only veloctity controller
                if(tar_velocity && !tar_rotations) {
                        motorPWM_vel = motorVelocityController();
                        motorPWM = motorPWM_vel;
                        if (count==0) {
                                putMessage(VELOCITY,act_velocity);
                                putMessage(TAR_VELOCITY,tar_velocity);
                        }

                }
                //only target rotation
                if(!tar_velocity && tar_rotations) {
                        motorPWM_rot = motorRotationController();
                        motorPWM = motorPWM_rot;
                        if (count==0) {
                                putMessage(ROTATION,act_rotations);
                                putMessage(TAR_ROTATION,tar_rotations);
                        }
                }
                /*
                   if (tar_velocity && tar_rotations) {
                        if(lead<0) motorPWM = (motorPWM_vel>motorPWM_rot) ? motorPWM_vel : motorPWM_rot;
                        else motorPWM = (motorPWM_vel<motorPWM_rot) ? motorPWM_vel : motorPWM_rot;
                        if (count==0) {
                                putMessage(VELOCITY,act_velocity);
                                putMessage(TAR_VELOCITY,tar_velocity);
                        }
                   }
                 */
                count = (count==0) ? PRINT_FREQUENCY : count;
                count-= 1;

        }
}

// ISR which will be called each time 100ms has passed. Performs no computation
// so Photointerrupter is not blocked.
void motorCtrlTick(){
        motorCtrlT.signal_set(0x1); // Set signal to calculate velocity
}

//Controller for motor velocity
uint32_t motorVelocityController()
{
        // y_s = k_p(s-|v|)
        int32_t y_s;

        //If the target velocity is 0 reverse the directon
        lead = (tar_velocity<0) ? -2 : 2;

        //velocity error term
        int32_t velocityError = tar_velocity - act_velocity;

        //calculating differential error
        int32_t differentialVelocityError = velocityError - prevVelocityError;
        prevVelocityError = velocityError;

        //calculating integral error (with bounds)
        integralVelocityError += velocityError*INTEGRAL_VEL_CONST;
        if(integralVelocityError> INTEGRAL_VEL_ERR_MAX) integralVelocityError = INTEGRAL_VEL_ERR_MAX;
        if(integralVelocityError<-INTEGRAL_VEL_ERR_MAX) integralVelocityError =-INTEGRAL_VEL_ERR_MAX;

        y_s = PROPORTIONAL_VEL_CONST*(abs(tar_velocity) - abs(act_velocity)) + integralVelocityError + DIFFERENTIAL_VEL_CONST*differentialVelocityError;

        //TODO: I think y needs to have modulus taken
        y_s = (y_s>0) ? y_s : 0;

        y_s = (y_s > PWM_LIMIT) ? PWM_LIMIT :
              y_s;

        //TODO: deadband motorPWM
        return (y_s) ? (uint32_t)y_s : DEAD_BAND_VEL;
}


// Implement the proportional speed control
uint32_t motorRotationController(){
        int32_t y_r;
        //Error term
        int32_t rotationError = tar_rotations - act_rotations;

        y_r = PROPORTIONAL_ROT_CONST*(rotationError) + DIFFERENTIAL_ROT_CONST*(rotationError - prevRotationError); // Need to divide by time

        prevRotationError = rotationError;

        //changes direction if overshoots
        lead = (y_r > 0) ?  2 : -2;

        //sets y to within max/min limits

        y_r = (y_r>0) ? y_r : 0;

        y_r = (y_r > PWM_LIMIT) ? PWM_LIMIT : y_r;

        //stops rotating of no more (integral) error
        //if(rotationIntegralError==0) lead = 0;
        return (y_r) ? (uint32_t)y_r : DEAD_BAND_ROT;
}
