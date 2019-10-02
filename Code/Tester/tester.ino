//#include "serialComLib.h"

#define PRINTING_SENSOR 2 // Print Head Movement Sensor
#define LOCK_PIN 12 // Key turn overide of kiosk
#define RELAY_A 3 // NO- Relay pin output pin, HIGH for activated
#define RELAY_B 4 // NC- Relay pin output pin, LOW for activated

//LEDS
#define LED_ACKTIV 9// Device Activated
#define LED_POWER 7//  Ready
#define LED_MACHINE_RUNNING 8//  Print Head Moving
#define LED_LOCK 6//

//Vars for serial communication

#define  CMD_WRITE_DOUT    1
#define  CMD_READ_DIN      2
#define  BUFFER_LENGTH     32

#define  SENSOR_DEFAULT HIGH // this is for the case that the sensor values are inverted- 

// Checking for signal from Kiosk at intervals

unsigned long lastSignal = 0;
const long kioskInterval = 15000;  // how long to wait before assuming kiosk is closed.
// swift runns intervals slower on mac when the application is in the background.

boolean billing = true; // if the kiosk should charge or not
//
int DeviceActivated = LOW;
//
// serial com
//
//SerialComLib   comLib;
unsigned char  bufferLength;
unsigned char  serialBuffer[BUFFER_LENGTH];
unsigned char  cmdId;
int lastLockVal;
bool activatedFlag = false;
//
//


// Sensor debounceing
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 4000;    // amount of time before sensor picks up printing action
int SensorState;             // the current reading from the input pin
int lastSensorState = LOW;   // the previous reading from the input pin

void setup()
{
  pinMode(RELAY_A, OUTPUT);
  pinMode(RELAY_B, OUTPUT);
  pinMode(LED_POWER, OUTPUT);
  pinMode(LED_MACHINE_RUNNING, OUTPUT);
  pinMode(LED_LOCK, OUTPUT);
  pinMode(LED_ACKTIV, OUTPUT);
  pinMode(LOCK_PIN, INPUT_PULLUP);
  pinMode(PRINTING_SENSOR, INPUT);
  Serial.begin(9600);
  //comLib.open();
  digitalWrite(LED_POWER, HIGH);
  //
  digitalWrite(LED_MACHINE_RUNNING, HIGH);
  digitalWrite(LED_LOCK, HIGH);
  digitalWrite(LED_POWER, HIGH);
  digitalWrite(LED_ACKTIV, HIGH);
  delay(2000);
  digitalWrite(LED_MACHINE_RUNNING, LOW);
  digitalWrite(LED_LOCK, LOW);
  digitalWrite(LED_ACKTIV, LOW);
  activateDevice(true);
}

void loop()
{
  boolean printing = false;
  int sensor = digitalRead(PRINTING_SENSOR);
  Serial.println(sensor);
   digitalWrite(LED_MACHINE_RUNNING, sensor);
//  delay(3000);
//  Serial.println("activated");
//  activateDevice(true);
//  delay(6000);
//  Serial.println("de-activated");
//  activateDevice(false);
}


int activateDevice(boolean val) {
  int  RelayAState = LOW;
  int  RelayBState = LOW;
  if (val == HIGH) {
    activatedFlag = true;
    digitalWrite(LED_ACKTIV, HIGH);
    RelayAState =  HIGH;
    RelayBState = HIGH;
  } else {
    digitalWrite(LED_ACKTIV, LOW);
    RelayAState =  LOW;
    RelayBState = LOW;
  }
  digitalWrite(RELAY_A, RelayAState);
  digitalWrite(RELAY_B, RelayBState);
}
