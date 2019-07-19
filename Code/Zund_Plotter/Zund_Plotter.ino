//Bibliothek serialComLib
#include "serialComLib.h"

//cmdId constants
#define  CMD_WRITE_DOUT    1
#define  CMD_READ_DIN      2
#define  BUFFER_LENGTH     32
#define LOCK_PIN 12 // Key turn overide of kiosk

//Digital Outputs and Input pins
#define HALL_SENSOR  2
#define VENTILATOR 10 // Ventilator power (red cable)
#define POWER_LASER 11 // laser power (green cable)
#define LASER_LOCK  3

//LEDS
#define LED_VENT 6// Ventilation On

//LEDS
#define LED_ACKTIV 9// Device Activated
#define LED_POWER 7//  Ready
#define LED_MACHINE_RUNNING 8//  Print Head Moving
#define LED_LOCK 6//

//Serial communication
SerialComLib   comLib;
unsigned char  bufferLength;
unsigned char  serialBuffer[BUFFER_LENGTH];
unsigned char  cmdId;




int lastLockVal;
boolean billing = true; // if the kiosk should charge or not
int DeviceActivated = LOW;

//Vars für Bauteile
int hallSensor;

//Kontostand (wird aus DB gelesen, vielleicht besser boolean?)
char kontostand;

void setup()
{
  pinMode(VENTILATOR, OUTPUT);
  pinMode(POWER_LASER, OUTPUT);
  pinMode(HALL_SENSOR, INPUT_PULLUP);
  pinMode(LASER_LOCK, OUTPUT);
  pinMode(LOCK_PIN, INPUT_PULLUP);
  //
  pinMode(LED_ACKTIV, OUTPUT);
  pinMode(LED_POWER, OUTPUT);
  pinMode(LED_MACHINE_RUNNING, OUTPUT);
  pinMode(LED_LOCK, OUTPUT);

  //Lib öffnen
  comLib.open();
   digitalWrite(LED_POWER, HIGH);
}

void loop()
{
  //Werte lesen, wenn Daten auf dem Serialport sind
  hallSensor = digitalRead(HALL_SENSOR);

  //Ventilator EIN, wenn Nullpunkt verlassen, sonst AUS
  if (hallSensor > 0)
  {
    digitalWrite(VENTILATOR, LOW);
    digitalWrite(LED_MACHINE_RUNNING, LOW);
  }
  else
  {
    digitalWrite(VENTILATOR, HIGH);
    digitalWrite(LED_MACHINE_RUNNING, HIGH);
  }

  if (comLib.available())
  {

    //Daten von Serialport lesen
    if (comLib.readCmd(&cmdId, &bufferLength, serialBuffer, BUFFER_LENGTH) != SerialComLib::Ok)
    return;

    switch (cmdId)
    {
      case CMD_WRITE_DOUT:
      switch (serialBuffer[0])
      {
        case 0:
        // device on/off
        DeviceActivated = serialBuffer[1];
        break;
        case 1:
        //
        default:
        //
        break;
      }
      break;

      case CMD_READ_DIN:
      if (billing) { //
        //Auslesen, was Hallsensor angibt (Laserkopf an Nullpunkt)
        if (hallSensor == 0)
        {
          //Kommunikation mit Python, Laser ist am Nullpunkt
          serialBuffer[0] = 1;
                }
        else
        {
          //Kommunikation mit Python, Laser ist unterwegs
          serialBuffer[0] = 0;
        }
        // send the command packet over serial
        bufferLength = 1;

        // write
        comLib.writeCmd(CMD_READ_DIN, bufferLength, serialBuffer);
        break;
      }
    }
  }

//
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
}

int activateDevice(boolean val) {
  digitalWrite(LED_ACKTIV, val);
  digitalWrite(LASER_LOCK, val);
  digitalWrite(POWER_LASER, val);
}
