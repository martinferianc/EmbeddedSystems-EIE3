/*  
 *  Code to be run on the Adafruit Feather. This code will monitor the accelerometer and transmitt 
 *  the data in real-time over WiFi to the server who will alert the user if a large shock is detected.
 * 
 */

#include <ESP8266WiFi.h>
#include <Wire.h>

//################## Variable Definitions ##################

#define WIFI_SSID ssid
#define WIFI_PASSWORD password
#define SERVER server

const char* ssid = "WIFI_SSID";
const char* password = "WIFI_PASSWORD";
const char* host = "SERVER";
const int MPU_addr = 0x68;

//################### Function Prototypes ##################

bool setupWifi();
void setupAccel();
void updateAccelValues(int16_t *vals);

//######################## Main Code #######################

void setup() {
  if(setupWifi()){
    setupAccel();
  }
}

void loop() {
  // Array to store the current values. Will be updated when updateAccelValues() is called.
  // Values: ACX;ACY;ACZ;TEMP;GYX;GYY;GYZ
  int16_t current_vals[7];
  updateAccelValues(current_vals);
  
}

//################## Function Definitions ##################

bool setupWifi()
{
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    delay(500); 
  }
}

void setupAccel()
{
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true); 
}

void updateAccelValues(int16_t *vals)
{
 Wire.beginTransmission(MPU_addr);
 Wire.write(0x3B);
 Wire.endTransmission(false);
 Wire.requestFrom(MPU_addr,14,true);
 for(int i = 0; i < 7; i++){
  vals[i] = Wire.read()<<8|Wire.read()
 }
}

