#include "motorControl.h"

// MOTOR STATE VARIABLES
//Drive state to output table
const int8_t driveTable[] = {0x12,0x18,0x09,0x21,0x24,0x06,0x00,0x00};
//Mapping from interrupter inputs to sequential rotor states. 0x00 and 0x07 are not valid
const int8_t stateMap[] = {0x07,0x05,0x03,0x04,0x01,0x00,0x02,0x07};
volatile int8_t state;
//const int8_t stateMap[] = {0x07,0x01,0x03,0x02,0x05,0x00,0x04,0x07}; //Alternative if phase order of input or drive is reversed

// MOTOR TORQUE VARIABLES
const int8_t lead = 2;  //2 for forwards, -2 for backwards

// MOTOR POSITION VARIABLES
volatile int32_t rotations;
uint8_t direction = 1;

// MOTOR VELOCITY VARIABLES
volatile float    speed = 0;
volatile float    max_speed = 0;
volatile int      rotor_speed = 0;
volatile uint8_t  dir_prev;

Timer rotor_speed_timer;
Ticker motorDrive;

//Status LED
DigitalOut led1(LED1);

//Photointerrupter inputs
InterruptIn I1(I1pin);
InterruptIn I2(I2pin);
InterruptIn I3(I3pin);

//Motor Drive outputs
PwmOut      L1L(L1Lpin);
DigitalOut  L1H(L1Hpin);
PwmOut      L2L(L2Lpin);
DigitalOut  L2H(L2Hpin);
PwmOut      L3L(L3Lpin);
DigitalOut  L3H(L3Hpin);


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

inline void updateState(){
        state = stateMap[I1 + 2*I2 + 4*I3];
        if(direction) rotations++;
        else          rotations--;
}
//Basic synchronisation routine
void motorHome() {
        //Put the motor in drive state 0 and wait for it to stabilise
        motorOut(0,1.0);
        wait(2.0);
        updateState();
        return;
}

void setPWMPeriod(int period) {
  L1L.period_us(period);
  L2L.period_us(period);
  L3L.period_us(period);
  return;
}

void pinInit() {
    I3.rise(&updateState);
    I2.rise(&updateState);
    I1.rise(&updateState);
    I3.fall(&updateState);
    I2.fall(&updateState);
    I1.fall(&updateState);
    return;
}

void motorRun() {

  pinInit();

  int8_t orState = 0;
  int8_t intState = 0;
  int8_t intStateOld = 0;

  //Run the motor synchronisation
  motorHome();
  orState = state;

  pc.printf("Rotor origin: %x\n\r",orState);

  while (1) {
      intState = state;
      if (intState != intStateOld) {
          intStateOld = intState;
          motorOut((intState-orState+lead+6)%6,1.0); //+6 to make sure the remainder is positive
      }
  }
}
