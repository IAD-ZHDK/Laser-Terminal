#include "serialComLib.h"

#define PRINTING_SENSOR A0 // Print Head Movement Sensor
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
SerialComLib   comLib;
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
  pinMode(LOCK_PIN, INPUT_PULLUP);
  pinMode(PRINTING_SENSOR, INPUT);
  //Serial.begin(9600);
  comLib.open();
  digitalWrite(LED_POWER, HIGH);
}

void loop()
{
  boolean printing = false;
  int sensor = digitalRead(PRINTING_SENSOR);
  sensor = debounce(sensor);

  //digitalWrite(LED_PRINTING, sensor);

  unsigned long currentMillis = millis();

  // check if printer has been moving for period
  if (sensor == SENSOR_DEFAULT) {
    printing = true;
  } else {
    printing = false;
  }

  digitalWrite(LED_MACHINE_RUNNING, printing);
  if (comLib.available())
  {
    lastSignal = millis();
    //Read data from Serial Port
    if (comLib.readCmd(&cmdId, &bufferLength, serialBuffer, BUFFER_LENGTH) != SerialComLib::Ok)
      return;
    switch (cmdId)
    {
      case CMD_WRITE_DOUT:
        switch (serialBuffer[0])
        {
          case 0:
            // printer on/off
            DeviceActivated = serialBuffer[1];
            break;
          case 1:
          default:
            //nothing
            break;
        }
        break;

      case CMD_READ_DIN:
      if (billing) { //
        if (!printing)
        {
          //Comminicate with Kiosk that the print head is at 0 point
          serialBuffer[0] = 0;
        }
        else
        {
          //Comminicate with Kiosk that the print head is moving
         // if (activatedFlag) { 
          //   serialBuffer[0] = 0; // if the device was deactivated during print, then send a 0 on reactivation
        //  } else {
            serialBuffer[0] = 1;
          //  }
         // activatedFlag = false;
        }
      }
        // send the command packet over serial
        bufferLength = 1;
        // write
        comLib.writeCmd(CMD_READ_DIN, bufferLength, serialBuffer);
        break;
    }
  } else
  {
    //no comminication
    //digitalWrite(LED_COMM, LOW);
  }

   // serial comminicatiion
  if (millis() - lastSignal >= kioskInterval) {
       DeviceActivated = false;
  } else {

  }
  int lock = digitalRead(LOCK_PIN);
  if (lock == LOW) {
    digitalWrite(LED_LOCK, HIGH);
    DeviceActivated = true;
    billing = false;
  } else {
    digitalWrite(LED_LOCK, LOW);
    if (lastLockVal != HIGH) {
      DeviceActivated = false;
    }
      billing = true;
  }
  lastLockVal = lock;
  activateDevice(DeviceActivated);
  //delay(1); // for stability 
}


int activateDevice(boolean val) {
  int  RelayAState = LOW;
  int  RelayBState = LOW;
  if (val == HIGH) {
    activatedFlag = true;
    digitalWrite(LED_ACKTIV, HIGH);
    RelayAState =  HIGH;
    RelayBState = LOW;
  } else {
    digitalWrite(LED_ACKTIV, LOW);
    RelayAState =  LOW;
    RelayBState = HIGH;
  }
  digitalWrite(RELAY_A, RelayAState);
  digitalWrite(RELAY_B, RelayBState);
}

int debounce(int NewReading) { // 
  if (NewReading != lastSensorState ) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:
    // if the button state has changed:
    if (NewReading != SensorState) {
      SensorState = NewReading;
    }
  }
  lastSensorState = NewReading;
  return SensorState;
}
