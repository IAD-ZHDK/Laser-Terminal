//Bibliothek serialComLib
#include "serialComLib.h"

//cmdId constants
#define  CMD_WRITE_DOUT    1
#define  CMD_READ_DIN      2
#define  BUFFER_LENGTH     32

//Vars für serielle Kommunikation
SerialComLib   comLib;
unsigned char  bufferLength;
unsigned char  serialBuffer[BUFFER_LENGTH];
unsigned char  cmdId;

//Vars für digitale Pins: Hall, 2x Relais
int hallPin = 2;
int relaisAPin = 3;
int relaisBPin = 4;

//Vars für die LEDs
int led_COMM = 9;
int led_LID = 8;
int led_HALL = 7;
int led_VENT = 6;

//Schalter zum Bedienen des Relais (wird später vom Wert aus der DB übernommen)
int schalterIn = 7;
int schalterON;

//Vars für Bauteile
int hallSensor;
boolean relaisAOutput;
boolean relaisBOutput;

//Kontostand (wird aus DB gelesen, vielleicht besser boolean?)
char kontostand;

void setup() 
{
  pinMode(hallPin, INPUT);
  pinMode(relaisAPin, OUTPUT);
  pinMode(relaisBPin, OUTPUT);
  pinMode(schalterIn, INPUT);

  pinMode(led_COMM, OUTPUT);  
  pinMode(led_LID, OUTPUT);
  pinMode(led_HALL, OUTPUT);
  pinMode(led_VENT, OUTPUT);

  //Internet Pull-Up Widerstand
  digitalWrite(hallPin, HIGH);

  //Lib öffnen
  comLib.open();
}

void loop()
{ 
  //Werte lesen, wenn Daten auf dem Serialport sind
  hallSensor = digitalRead(hallPin);
  schalterON = digitalRead(schalterIn);

  //Ventilator EIN, wenn Nullpunkt verlassen, sonst AUS
  if(hallSensor > 0)
  {
    digitalWrite(relaisBPin,HIGH);
    digitalWrite(led_VENT,HIGH);
    digitalWrite(led_HALL,HIGH);
  }
  else
  {
    digitalWrite(relaisBPin,LOW);
    digitalWrite(led_VENT,LOW);
    digitalWrite(led_HALL,LOW);
  }
  
  if(comLib.available())
  {

    //Daten von Serialport lesen
    if(comLib.readCmd(&cmdId, &bufferLength, serialBuffer,BUFFER_LENGTH) != SerialComLib::Ok)
      return;

    switch(cmdId)
    {
    case CMD_WRITE_DOUT:
      digitalWrite(led_COMM,HIGH);

      switch(serialBuffer[0])
      {
      case 0:
        //Signal an Relais A
        digitalWrite(relaisAPin,serialBuffer[1]);
        digitalWrite(led_LID,serialBuffer[1]);
        break;
      case 1:
      default:
        //Signal an Relais B
        digitalWrite(relaisBPin,serialBuffer[1]);
        digitalWrite(led_VENT,serialBuffer[1]);
        break;
      }
      break;

    case CMD_READ_DIN:
      digitalWrite(led_COMM,HIGH);

      //Auslesen, was Hallsensor angibt (Laserkopf an Nullpunkt)
      if (hallSensor == 0)
      {
        //Kommunikation mit Python, Laser ist am Nullpunkt
        serialBuffer[0] = 1;
        digitalWrite(led_HALL,HIGH);
      }
      else
      {
        //Kommunikation mit Python, Laser ist unterwegs
        serialBuffer[0] = 0;
        digitalWrite(led_HALL,LOW);
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
    digitalWrite(led_COMM,LOW);
  }
}



