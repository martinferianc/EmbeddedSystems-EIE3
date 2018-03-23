#include "motorControl.h"


//           MOTOR STATE VARIABLES
//Drive state to output table
const int8_t driveTable[] = {0x12,0x18,0x09,0x21,0x24,0x06,0x00,0x00};
//Mapping from interrupter inputs to sequential rotor states. 0x00 and 0x07 are not valid
const int8_t stateMap[] = {0x07,0x05,0x03,0x04,0x01,0x00,0x02,0x07};

//            MOTOR TORQUE VARIABLES
volatile int8_t lead = 2;  //2 for forwards, -2 for backwards

//            VELOCITY AND ROTATION VARIABLES
float act_velocity    = 0.0;
float act_rotations   = 0.0;
float prev_tar_rotations  = 0.0;
float prev_tar_velocity   = 0.0;

//            MOTOR POSITION VARIABLES
volatile int32_t motorPosition   = 0;
uint32_t orState = 0;

//            ERROR VARIABLES
float integralVelocityError = 0;
float prevVelocityError = 0;
float integralRotationsError = 0;
float prevRotationError = 0;

float previous_rot_velocity     = 0.0;
float previous_vel_velocity     = 0.0;

//            I/O CONNECTIONS
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

//Led
DigitalOut led1(LED1);

//Set a given drive stats
void motorOut(int8_t driveState, uint32_t torque){

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
        if (driveOut & 0x01) L1L.pulsewidth_us(torque);
        if (driveOut & 0x02) L1H = 0;
        if (driveOut & 0x04) L2L.pulsewidth_us(torque);
        if (driveOut & 0x08) L2H = 0;
        if (driveOut & 0x10) L3L.pulsewidth_us(torque);
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
        motorOut(0,1000);
        wait(2);
        orState = readRotorState(); //initialises oldRotorState
}

// Set the PWM period
void setPWMPeriod(int period) {
        L1L.period_us(period);
        L2L.period_us(period);
        L3L.period_us(period);
}

void setISRPhotoSensors(){
        I3.rise(&motorISR);
        I2.rise(&motorISR);
        I1.rise(&motorISR);
        I3.fall(&motorISR);
        I2.fall(&motorISR);
        I1.fall(&motorISR);
}

// ISR to handle the updating of the motor position
void motorISR(){
        led1 = !led1;
        static int8_t oldRotorState = 0;
        int8_t rotorState = readRotorState();

        // Avoid doing a modulo divide
        int8_t tmpDriveState = (rotorState - orState + lead + 6);
        while(tmpDriveState >= 6) {
                tmpDriveState -= 6;
        }

        motorOut(tmpDriveState,motorPWM);
        if(rotorState - oldRotorState == 5) motorPosition--;
        else if (rotorState - oldRotorState == -5) motorPosition++;
        else motorPosition += (rotorState - oldRotorState);
        oldRotorState = rotorState;
}

// Function to instantiate thread which will run every 100ms to calcualte
// the velocity at this time.
void motorCtrlFn(){
        // Set the motor to home position
        motorHome();

        // Ticker used to control how often motor control thread runs
        Ticker motorCtrlTicker;
        motorCtrlTicker.attach_us(&motorCtrlTick, 100000);

        // Control variables & Printing variable
        float oldMotorPosition = 0.0;
        uint32_t motorPWM_rot;
        uint32_t motorPWM_vel;
        int8_t print_count= 0;

        while(1) {
                if (tar_rotations != prev_tar_rotations) {
                        integralRotationsError = 0;
                        prev_tar_rotations = tar_rotations;
                }
                else if (tar_velocity  != prev_tar_velocity ) {
                        integralVelocityError  = 0;
                        prev_tar_velocity  = tar_velocity;
                }

                // Update the motor position
                oldMotorPosition = ((float)motorPosition)/6;

                // Suspend until signal occurs.
                motorCtrlT.signal_wait(0x1);

                // Calculate actual velocity and number of rotations
                act_velocity  =   10*(((float)motorPosition)/6 - oldMotorPosition);
                act_rotations =       ((float)motorPosition)/6;

                // Start up the motor if the avtual speed is zero and it needs to move
                if(act_velocity==0 && (tar_velocity || tar_rotations))
                {
                        int8_t rotorState = readRotorState();
                        // Avoid doing a modulo divide
                        int8_t tmpDriveState = (rotorState - orState + lead + 6);
                        while(tmpDriveState >= 6) {
                                tmpDriveState -= 6;
                        }
                        motorOut(tmpDriveState,motorPWM);
                }
                // Velocity is set
                if(tar_velocity == 2000) {
                        motorPWM = motorVelocityController();
                        if (print_count==0) {
                                putMessage(VELOCITY,*(int32_t*)&act_velocity);
                                putMessage(TAR_VELOCITY,*(int32_t*)&tar_velocity);
                        }

                }
                // Velocity is set
                else if(tar_velocity && !tar_rotations) {
                        motorPWM = motorVelocityController();
                        if (print_count==0) {
                                putMessage(VELOCITY,*(int32_t*)&act_velocity);
                                putMessage(TAR_VELOCITY,*(int32_t*)&tar_velocity);
                        }

                }
                // Rotation is set
                else if(!tar_velocity && tar_rotations) {
                        motorPWM = motorRotationController();
                        if (print_count==0) {
                                putMessage(ROTATION,*(int32_t*)&act_rotations);
                                putMessage(TAR_ROTATION,*(int32_t*)&tar_rotations);
                        }
                }
                // Rotations are set and velocity is set
                else if (tar_velocity && tar_rotations) {
                        motorPWM_vel = motorVelocityController();
                        motorPWM_rot = motorRotationController();
                        if (act_velocity<0) {
                                motorPWM = (motorPWM_vel>motorPWM_rot) ? motorPWM_vel : motorPWM_rot;
                        } else {
                                motorPWM = (motorPWM_vel<motorPWM_rot) ? motorPWM_vel : motorPWM_rot;
                        }
                        if (print_count==0) {
                                putMessage(VELOCITY,*(int32_t*)&act_velocity);
                                putMessage(TAR_VELOCITY,*(int32_t*)&tar_velocity);
                                putMessage(ROTATION,*(int32_t*)&act_rotations);
                                putMessage(TAR_ROTATION,*(int32_t*)&tar_rotations);
                        }
                }
                // Reset printing
                print_count = (print_count==0) ? PRINT_FREQUENCY : print_count;
                print_count-= 1;

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
        float y_s;

        // If the target velocity is 0 reverse the directon
        lead = (tar_velocity<0) ? -2 : 2;

        // Error term
        float velocityError = abs(tar_velocity) - abs(act_velocity);

        // Differential error term
        float differentialVelocityError = velocityError - prevVelocityError;
        prevVelocityError = velocityError;

        // Integral error
        integralVelocityError +=  velocityError*INTEGRAL_VEL_CONST;
        if(integralVelocityError > INTEGRAL_VEL_ERR_MAX) integralVelocityError = INTEGRAL_VEL_ERR_MAX;
        if(integralVelocityError < (-INTEGRAL_VEL_ERR_MAX)) integralVelocityError =-INTEGRAL_VEL_ERR_MAX;

        y_s = PROPORTIONAL_VEL_CONST*(abs(tar_velocity) - abs(act_velocity)) + DIFFERENTIAL_VEL_CONST*differentialVelocityError + integralVelocityError;

        // Set y to positive
        y_s = (y_s>0) ? y_s : 0;

        // Check if we are not going over the hardware limit
        y_s = (y_s > (float)PWM_LIMIT) ? (float)PWM_LIMIT : y_s;

        return (y_s) ? (uint32_t)y_s : (uint32_t)DEAD_BAND_VEL;
}


// Implement the proportional speed control
uint32_t motorRotationController(){
        float y_r;

        // Error term
        float rotationError = tar_rotations - act_rotations;
        prevRotationError = rotationError;

        // Differential error term
        float differentialRotationError = rotationError - prevRotationError;

        // Integral error term
        integralRotationsError = rotationError*INTEGRAL_ROT_CONST;
        if(integralRotationsError > INTEGRAL_ROT_ERR_MAX) integralRotationsError    = INTEGRAL_ROT_ERR_MAX;
        if(integralRotationsError < (-INTEGRAL_ROT_ERR_MAX)) integralRotationsError =-INTEGRAL_ROT_ERR_MAX;

        y_r = PROPORTIONAL_ROT_CONST*(rotationError) + DIFFERENTIAL_ROT_CONST*differentialRotationError + integralRotationsError; // Need to divide by time

        // In case the rotations were overshot change the direction back
        lead = (y_r > 0) ?  2 : -2;
        y_r = abs(y_r);
        y_r = (y_r > ROT_LIMIT) ? ROT_LIMIT : y_r;

        return (uint32_t)y_r;
}
