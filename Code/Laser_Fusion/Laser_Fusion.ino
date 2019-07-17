//Bibliothek serialComLib
#include "serialComLib.h"

//cmdId constants
#define  CMD_WRITE_DOUT    1
#define  CMD_READ_DIN      2
#define  BUFFER_LENGTH     32

//Serial communication
SerialComLib   comLib;
unsigned char  bufferLength;
unsigned char  serialBuffer[BUFFER_LENGTH];
unsigned char  cmdId;

//Digital Outputs and Input pins
int HALL_SENSOR = 2;
int VENTILATOR = 10; // Ventilator power (red cable)
int POWER_LASER = 11; // laser power (green cable)
int LASER_LOCK = 3;
//int relaisBPin = 4;

//LEDS
int LED_COMM = 9; // Communication
int LED_READY = 8; // Laser Ready
int LED_HALL = 7;// laser Moving
int LED_VENT = 6;// Ventilation On


//Vars für Bauteile
int hallSensor;

//Kontostand (wird aus DB gelesen, vielleicht besser boolean?)
char kontostand;

void setup()
{
  pinMode(VENTILATOR, OUTPUT);
  pinMode(POWER_LASER, OUTPUT);
  pinMode(HALL_SENSOR, INPUT);
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
  //Werte lesen, wenn Daten auf dem Serialport sind
  hallSensor = digitalRead(HALL_SENSOR);

  //Ventilator EIN, wenn Nullpunkt verlassen, sonst AUS
  if (hallSensor > 0)
  {
    digitalWrite(VENTILATOR, LOW);
    digitalWrite(LED_VENT, LOW);
    digitalWrite(LED_HALL, LOW);
  }
  else
  {
    digitalWrite(VENTILATOR, HIGH);
    digitalWrite(LED_VENT, HIGH);
    digitalWrite(LED_HALL, HIGH);
  }

  if (comLib.available())
  {

    //Daten von Serialport lesen
    if (comLib.readCmd(&cmdId, &bufferLength, serialBuffer, BUFFER_LENGTH) != SerialComLib::Ok)
      return;

    switch (cmdId)
    {
      case CMD_WRITE_DOUT:
        digitalWrite(LED_COMM, HIGH);

        switch (serialBuffer[0])
        {
          case 0:
            //Signal an Relais A
            digitalWrite(POWER_LASER, serialBuffer[1]); // Laser power on/off
            digitalWrite(LASER_LOCK, serialBuffer[1]);
            digitalWrite(LED_READY, serialBuffer[1]);
            break;
          case 1:
          default:
            //Signal an Relais B
            // digitalWrite(relaisBPin,serialBuffer[1]);
            //   digitalWrite(LED_VENT,serialBuffer[1]);
            break;
        }
        break;

      case CMD_READ_DIN:
        digitalWrite(LED_COMM, HIGH);

        //Auslesen, was Hallsensor angibt (Laserkopf an Nullpunkt)
        if (hallSensor == 0)
        {
          //Kommunikation mit Python, Laser ist am Nullpunkt
          serialBuffer[0] = 1;
          digitalWrite(LED_HALL, HIGH);
        }
        else
        {
          //Kommunikation mit Python, Laser ist unterwegs
          serialBuffer[0] = 0;
          digitalWrite(LED_HALL, LOW);
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
