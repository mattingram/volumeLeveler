// Microphone Volume Leveler
// Microphone threshold code based on https://github.com/joekrie/arduino-volume-detector/blob/master/volume-detector/volume-detector.ino

#include <Arduino.h>

const int ADC = 17; // analog pin
const int REDLED = 0;
const int BLUELED = 2;
const bool ON = LOW; // onboard LEDs are reversed
const bool OFF = HIGH;

const int sampleWindow = 50;  // # of milliseconds per sample
const int threshold = 50; // tolerance of sound level difference peakToPeak

void setup() {
  pinMode(BLUELED, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  long startMillis = millis();
  int peakToPeak = 0;
  int signalMax = 0;
  int signalMin = 1024;

  while (millis() - startMillis < sampleWindow)
  {
    int sample = analogRead(ADC);

    if (sample < 1024)  // reject invalid readings
    {
      if (sample > signalMax)
      {
        signalMax = sample;
      }
      else if (sample < signalMin)
      {
        signalMin = sample;
      }
    }
  }

  peakToPeak = signalMax - signalMin;  // we care about the difference 

  if (peakToPeak > threshold)
  {
    // turn on LED if volume exceeds threshold
    digitalWrite(BLUELED, ON);
    Serial.printf("Above threshold=%d\n", peakToPeak); // debug
  }
  else
  {
    digitalWrite(BLUELED, OFF);
  }
}