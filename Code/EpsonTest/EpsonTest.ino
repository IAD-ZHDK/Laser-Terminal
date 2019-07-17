#define RELAY_A 3 // Relay pin output pin
#define RELAY_B 4 // Relay pin output pin
#define LOCK_PIN 12
#define LED_LOCK 6//


// These constants won't change. They're used to give names to the pins used:
const int analogInPin = A0;  // Analog input pin that the potentiometer is attached to
//const int analogOutPin = 9; // Analog output pin that the LED is attached to

int sensorValue = 0;        // value read from the pot
int outputValue = 0;        // value output to the PWM (analog out)
//
// Variables will change:
int RelayAState = LOW;             // RelayAState used to set the LED
int RelayBState = HIGH;
// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;        // will store last time LED was updated

// constants won't change:
const long interval = 8000;           // interval at which to blink (milliseconds)

// Sensor debounce
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 1000;    // the debounce time; increase if the output flickers
int buttonState = LOW;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin



void setup() {
  pinMode(RELAY_A, OUTPUT);
  pinMode(RELAY_B, OUTPUT);
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  RelayAState = HIGH;
  RelayBState = LOW;
  // digitalWrite(RELAY_A, LOW);
  // digitalWrite(RELAY_B, LOW);
  // digitalWrite(RELAY_A, LOW);
  // digitalWrite(RELAY_B, HIGH);
  pinMode(LOCK_PIN, INPUT_PULLUP);
  pinMode(analogInPin, INPUT);
}

void loop() {
  //digitalWrite(RELAY_A, RelayAState);
  //digitalWrite(RELAY_B, RelayBState);
  // read the analog in value:
  sensorValue = digitalRead(analogInPin);
  sensorValue = debounce(sensorValue);
  //sensorValue = smooth(sensorValue);
  // map it to the range of the analog out:

  // change the analog out value:
  //analogWrite(analogOutPin, outputValue);

  // print the results to the Serial Monitor:
  //  Serial.print("sensor = ");
  // Serial.print(RelayAState*1000);
  // Serial.print(",");
  Serial.println(sensorValue);
  // wait 2 milliseconds before the next loop for the analog-to-digital
  // converter to settle after the last reading:
  delay(4);
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {

    previousMillis = currentMillis;
    
    if (RelayAState == LOW) {
      // Serial.println("ON");
      RelayAState = HIGH;
      RelayBState = LOW;
    } else {
      //  Serial.println("OFF");
      RelayAState = LOW;
      RelayBState = HIGH;
    }


  }
  // set the LED with the RelayAState of the variable:
  //digitalWrite(RELAY_A, RelayAState);
  // digitalWrite(RELAY_B, RelayBState);
  int lock = digitalRead(LOCK_PIN);

  if (lock == LOW) {
    digitalWrite(LED_LOCK, HIGH);
    digitalWrite(RELAY_A, HIGH);
    digitalWrite(RELAY_B, LOW);
  } else {
    digitalWrite(LED_LOCK, LOW);
    digitalWrite(RELAY_A, LOW);
    digitalWrite(RELAY_B, HIGH);
  }
}





int debounce(int NewReading) {
  if (NewReading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (NewReading != buttonState) {
      buttonState = NewReading;
    }

  }
  lastButtonState = NewReading;
  return buttonState;
}
