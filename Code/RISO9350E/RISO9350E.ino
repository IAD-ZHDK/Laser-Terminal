//Bibliothek serialComLib

#include "serialComLib.h"


#define COPY_PIN A0 // Paper feed sensor input pin

#define MASTER_PIN 8 // Reed sensor input pin

#define HATCH_PIN 3 // Relay pin output pin

#define LED 12 //

// Checking for signal from Kiosk at intervalss

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



int copyValue = 0;

int copyThreshold = 200; // threshold for registering a change in the paper feed sensor

int FeedSensorBase = 550; // The baseline measurement from feed sensor

int masterValue = 0;

unsigned long masterTimer = 0;

int masterDuration = 20000;



boolean copyStart = false;

boolean masterStart = false;



//boolean CopyFlag = false;

boolean MasterFlag = false;

// smoothing

const int numReadings = 20;

int readings[numReadings];

int readIndex = 0;

int total = 0;



unsigned long startMillis;  //some global variables available anywhere in the program

unsigned long currentMillis;



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
  copyValue = analogRead(COPY_PIN);
  masterValue = digitalRead(MASTER_PIN);

  copyValue = smooth(copyValue);

  int difference = abs(copyValue - FeedSensorBase);

  //COPY

  if (difference > copyThreshold && copyStart == false && timerGreaterThan(200))
  {
    copyStart = true;
  }



  if (difference < copyThreshold && copyStart == true && timerGreaterThan(200))

  {

    copyStart = false;

    //CopyFlag = true;

    serialBuffer[0] = 0;

    comLib.writeCmd(CMD_READ_DIN, 1, serialBuffer);

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



boolean timerGreaterThan(int period) {

  currentMillis = millis();
  if (currentMillis - startMillis >= period)

  {

    startMillis = currentMillis;

    return true;

  } else {

    return false;

  }

}



int smooth(int NewReading) {

  // subtract the last reading:

  total = total - readings[readIndex];

  // read from the sensor:

  readings[readIndex] = NewReading;

  // add the reading to the total:

  total = total + readings[readIndex];

  // advance to the next position in the array:

  readIndex = readIndex + 1;

  // if we're at the end of the array...

  if (readIndex >= numReadings) {

    // ...wrap around to the beginning:

    readIndex = 0;

  }

  // calculate the average:

  int average = total / numReadings;

  return average;

}
