//Bibliothek serialComLib

#include "serialComLib.h"


#define COPY_PIN A0 // Paper feed sensor input pin

#define MASTER_PIN 8 // Reed sensor input pin

#define HATCH_PIN 3 // Relay pin output pin

#define LED 12 //

// Checking for signal from Kiosk at interval

unsigned long lastSignal = 0;

const long intervals = 3000;


//Vars for serial communication

#define  CMD_WRITE_DOUT    1

#define  CMD_READ_DIN      2

#define  BUFFER_LENGTH     32

SerialComLib   comLib;

unsigned char  bufferLength;

unsigned char  serialBuffer[BUFFER_LENGTH];

unsigned char  cmdId;



int copyValue = HIGH;

int lastCopyValue = 0;
unsigned long copyTimer = 0; 

int masterValue = 0;
unsigned long masterTimer = 0;
int masterDuration = 20000;

boolean masterStart = false;



//boolean CopyFlag = false;

boolean MasterFlag = false;



void setup()
{

  pinMode(MASTER_PIN, INPUT_PULLUP);//pinMode(MASTER_PIN, INPUT); //MACHINE//TEST

  pinMode(HATCH_PIN, OUTPUT);

  pinMode(LED, OUTPUT);

  pinMode(13, OUTPUT);

  //Serial.begin(9600);

  //Lib öffnen

  comLib.open();

}



void loop()
{
  copyValue = digitalRead(COPY_PIN);
  masterValue = digitalRead(MASTER_PIN);

  if (copyValue != lastCopyValue) {
    copyTimer = millis();
  }

 if ((millis() - copyTimer) > 50) {
    if (copyValue != lastCopyValue) {
      lastCopyValue = copyValue;
      if (copyValue == HIGH) {
          // todo: If value stays fixed (ie sensor covered) then there will be no copies charged. This is very suseptable to hacking
           serialBuffer[0] = 0;
           comLib.writeCmd(CMD_READ_DIN, 1, serialBuffer);
      }
    }
  }



  //MASTER
  if (masterValue == 0 && masterStart == false)

  {

    masterStart = true;

  }



  if (masterValue == 1 && masterStart == true)

  {

    masterStart = false;

    if (millis() - masterTimer > masterDuration)

    {
      MasterFlag = true;
      serialBuffer[0] = 1;
      comLib.writeCmd(CMD_READ_DIN, 1, serialBuffer);
      masterTimer = millis();
    }
  }

  // serial comminicatiion
  if (millis() - lastSignal >= intervals) {
    digitalWrite(HATCH_PIN, LOW);
    digitalWrite(LED, LOW);
  }

  if (comLib.available())
  {
    lastSignal = millis();
    if (comLib.readCmd(&cmdId, &bufferLength, serialBuffer, BUFFER_LENGTH) != SerialComLib::Ok) {
      return;
    }
    //digitalWrite(LED, HIGH);
    if (cmdId == CMD_WRITE_DOUT && serialBuffer[0] == 0) {
      //lastSignal = currentTime;
      digitalWrite(HATCH_PIN, serialBuffer[1]);
      digitalWrite(LED, serialBuffer[1]);
    }
  }
  ///
}
