#include <Arduino.h>

#define LED PB5
#define RAIN_ITS PA0

volatile byte ledState = LOW;

void counting() {
  ledState = !ledState;
  digitalWrite(LED, ledState);
}

void setup() {

  pinMode(LED, OUTPUT);
  pinMode(RAIN_ITS, INPUT);
   attachInterrupt(digitalPinToInterrupt(RAIN_ITS), counting, CHANGE);
}


// the loop function runs over and over again forever

void loop() {             // wait for a second
  
}