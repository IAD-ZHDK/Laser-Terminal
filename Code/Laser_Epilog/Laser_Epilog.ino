//Bibliothek serialComLib
#include "serialComLib.h"

//cmdId constants
#define  CMD_WRITE_DOUT    1
#define  CMD_READ_DIN      2
#define  BUFFER_LENGTH     32

//Serial commu99nication
SerialComLib   comLib;
unsigned char  bufferLength;
unsigned char  serialBuffer[BUFFER_LENGTH];
unsigned char  cmdId;

//Digital Outputs and Input pins
int HALL_SENSOR = 2;
int VENTILATOR = 10; // Ventilator power (red cable)
int POWER_LASER = 11; // laser power (green cable)
int LASER_LOCK = 3; // Laser lock relay

//LEDS
int LED_COMM = 9; // Communication
int LED_READY = 8; // Laser Ready 
int LED_HALL = 7;// laser Moving
int LED_VENT = 6;// Ventilation On

//Laser position Hall sensor
int hallSensor;

void setup()
{
  pinMode(HALL_SENSOR, INPUT);

  pinMode(VENTILATOR, OUTPUT);
  pinMode(POWER_LASER, OUTPUT);
  pinMode(LASER_LOCK, OUTPUT);
  pinMode(LED_COMM, OUTPUT);
  pinMode(LED_READY, OUTPUT);
  pinMode(LED_HALL, OUTPUT);
  pinMode(LED_VENT, OUTPUT);

  //Internet Pull-Up Widerstand
  digitalWrite(HALL_SENSOR, HIGH);

  //Lib öffnen
  comLib.open();
}

void loop()
{
  hallSensor = digitalRead(HALL_SENSOR);
  //VENTILATOR on when laser leaves Null point, otherwise off
  if (hallSensor > 0)
  {
    digitalWrite(VENTILATOR, HIGH); // vent on
    digitalWrite(LED_VENT, HIGH);
    digitalWrite(LED_HALL, HIGH);
  } else {
    digitalWrite(VENTILATOR, LOW);
    digitalWrite(LED_VENT, LOW);
    digitalWrite(LED_HALL, LOW);
  }


  if (comLib.available())
  {
    //Read data from Serial Port
    if (comLib.readCmd(&cmdId, &bufferLength, serialBuffer, BUFFER_LENGTH) != SerialComLib::Ok)
      return;
    switch (cmdId)
    {
      case CMD_WRITE_DOUT:
        digitalWrite(LED_COMM, HIGH);
        switch (serialBuffer[0])
        {
          case 0:
            digitalWrite(POWER_LASER, serialBuffer[1]); // Laser power on/off
            digitalWrite(LED_READY, serialBuffer[1]);
             digitalWrite(LASER_LOCK, serialBuffer[1]); // Laser lock/unlock
            break;
          case 1:
          default:
            //nothing
            break;
        }
        break;

      case CMD_READ_DIN:
        digitalWrite(LED_COMM, HIGH);

        if (hallSensor == 0)
        {
          //Comminicate with Kiosk that the laser is at 0 point
          serialBuffer[0] = 0;
          digitalWrite(LED_HALL, LOW);
        }
        else
        {
          //Comminicate with Kiosk that the laser is moving
          serialBuffer[0] = 1;
          digitalWrite(LED_HALL, HIGH);
        }
        // send the command packet over serial
        bufferLength = 1;
        // write
        comLib.writeCmd(CMD_READ_DIN, bufferLength, serialBuffer);
        break;
    }
  }
  else
  {
    digitalWrite(LED_COMM, LOW);
  }
}



