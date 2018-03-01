#include "mbed.h"
#include "hash/SHA256.h"

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

//Mapping from sequential drive states to motor phase outputs
/*
   State   L1  L2  L3
   0       H   -   L
   1       -   H   L
   2       L   H   -
   3       L   -   H
   4       -   L   H
   5       H   L   -
   6       -   -   -
   7       -   -   -
 */
// Required for the bitcoin mining
uint8_t sequence[] = {0x45,0x6D,0x62,0x65,0x64,0x64,0x65,0x64, 0x20,0x53,0x79,0x73,0x74,0x65,0x6D,0x73, 0x20,0x61,0x72,0x65,0x20,0x66,0x75,0x6E, 0x20,0x61,0x6E,0x64,0x20,0x64,0x6F,0x20, 0x61,0x77,0x65,0x73,0x6F,0x6D,0x65,0x20, 0x74,0x68,0x69,0x6E,0x67,0x73,0x21,0x20, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
uint64_t* key = (uint64_t*)((int)sequence + 48);
uint64_t* nonce = (uint64_t*)((int)sequence + 56);
uint8_t hash[32];
uint32_t hash_counter=0;

//Drive state to output table
const int8_t driveTable[] = {0x12,0x18,0x09,0x21,0x24,0x06,0x00,0x00};

//Mapping from interrupter inputs to sequential rotor states. 0x00 and 0x07 are not valid
const int8_t stateMap[] = {0x07,0x05,0x03,0x04,0x01,0x00,0x02,0x07};
//const int8_t stateMap[] = {0x07,0x01,0x03,0x02,0x05,0x00,0x04,0x07}; //Alternative if phase order of input or drive is reversed

//Phase lead to make motor spin
const int8_t lead = -2;  //2 for forwards, -2 for backwards

//State variabel of the motor
volatile int8_t state = -1;

// Buffer length for the input string
static uint8_t buffer_length = 24;

//Status LED
DigitalOut led1(LED1);

//Photointerrupter inputs
InterruptIn I1(I1pin);
InterruptIn I2(I2pin);
InterruptIn I3(I3pin);

//Motor Drive outputs
DigitalOut L1L(L1Lpin);
DigitalOut L1H(L1Hpin);
DigitalOut L2L(L2Lpin);
DigitalOut L2H(L2Hpin);
DigitalOut L3L(L3Lpin);
DigitalOut L3H(L3Hpin);

// Checks if the input is a digit
uint8_t isdigit(const char c) {
        if ((c - '0' > -1) && (c - '0' < 10))
                return 1;
        else
                return 0;
}
// The output sequence determines the type of the output
// 1 --- FLOAT
// 0 --- INT
uint8_t parseRegex(char* regex, char type){
        uint8_t result_type = 0;
        if (regex[1]== '-') {
                for (uint8_t i = 1; i < 23; i++) {
                        if (regex[i]=='.') {
                                // Number is a float
                                result_type = 1;
                        }
                        regex[i-1] = regex[i];
                }
        }
        return result_type;
}

//Set a given drive state
void motorOut(int8_t driveState){

        //Lookup the output byte from the drive state.
        int8_t driveOut = driveTable[driveState & 0x07];

        //Turn off first
        if (~driveOut & 0x01) L1L = 0;
        if (~driveOut & 0x02) L1H = 1;
        if (~driveOut & 0x04) L2L = 0;
        if (~driveOut & 0x08) L2H = 1;
        if (~driveOut & 0x10) L3L = 0;
        if (~driveOut & 0x20) L3H = 1;

        //Then turn on
        if (driveOut & 0x01) L1L = 1;
        if (driveOut & 0x02) L1H = 0;
        if (driveOut & 0x04) L2L = 1;
        if (driveOut & 0x08) L2H = 0;
        if (driveOut & 0x10) L3L = 1;
        if (driveOut & 0x20) L3H = 0;
}

void updateState(){
        state = stateMap[I1 + 2*I2 + 4*I3];
}
//Basic synchronisation routine
void motorHome() {
        //Put the motor in drive state 0 and wait for it to stabilise
        motorOut(0);
        wait(1.0);
        return;
}
void countHash(){
        printf("Counted %d hashes/s\n\r", hash_counter);
        hash_counter = 0;
}


//Main
int main() {
        Serial pc(SERIAL_TX, SERIAL_RX);
        pc.printf("Beginning the program!\n\r");
        int8_t orState = 0; //Rotor offset at motor state 0

        // This is the buffer to hold the input commands
        static char buffer[24];
        static uint8_t count = 0;

        int8_t intState = 0;
        int8_t intStateOld = 0;

        //Run the motor synchronisation
        motorHome();
        orState = state;
        pc.printf("Rotor origin: %x\n\r",orState);
        //orState is subtracted from future rotor state inputs to align rotor and motor states

        //Use interrups to check for the state of the rotor
        I3.rise(&updateState);
        I2.rise(&updateState);
        I1.rise(&updateState);
        I3.fall(&updateState);
        I2.fall(&updateState);
        I1.fall(&updateState);

        // Initialize the timer for the hash calculation
        Ticker t;
        t.attach(&countHash, 1.0);

        //Poll the rotor state and set the motor outputs accordingly to spin the motor
        while (1) {
                char c;
                if (pc.readable()) {
                        c = pc.getc();
                        pc.putc(c);
                        buffer[count++] = c;
                        if (count > buffer_length || c == '\r' || c == '\n') {
                                pc.printf("\r\n");
                                count = 0;
                                // Execute the rotor commands
                                if (buffer[0] == 'R' || buffer[0] == 'V') {
                                        uint8_t result_type = parseRegex(buffer, (char)buffer[0]);
                                        if (result_type) {
                                                pc.printf(buffer);
                                                // pc.printf("%f\n\r", atof(buffer));
                                        } else {
                                                pc.printf(buffer);
                                                // pc.printf("%d\n\r", atoi(buffer));
                                        }
                                }

                        } else if (buffer[0] == 'K') {

                                // Set the tune
                        } else if (buffer[0] == 'T') {



                        }
                        count = 0;
                        // Reset the buffer and the counter
                        for (int i = 0; i < buffer_length; i++) {
                                buffer[i] = ';';
                        }

                }
                // Compute the hash
                SHA256::computeHash(hash, sequence, 64);
                if ((hash[0]==0) || (hash[1]==0))
                        *nonce+=1;
                hash_counter+=1;

        }
}
