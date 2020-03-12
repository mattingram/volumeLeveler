#include <Arduino.h>

//Map PCB for Arduino to NodeMCU
const int GPIO0 = 3;
const int GPIO2 = 4;
const int GPIO3 = 9;
const int GPIO4 = 2; // may need to swap 4/5
const int GPIO5 = 1;
const int GPIO9 = 11;
const int GPIO10 = 12;
const int GPIO12 = 6;
const int GPIO13 = 7;
const int GPIO14 = 5;
const int GPIO15 = 8;
const int GPIO16 = 0;
const int ADC = 17;
// const int SCL = GPIO5;
// const int SDA = GPIO4;
const int REDLED = 0; //GPIO0 in Lua
const int BLUELED = 2; //GPI02 in Lua
const bool ON = LOW;
const bool OFF = HIGH;

int micLevel = 0;
int micLevelInit = 0;
const int threshold = 4;


void setup() {
  pinMode(BLUELED, OUTPUT);

  Serial.begin(115200);

  micLevelInit = analogRead(ADC);

  Serial.printf("Mic threshold=%d\n", threshold);
  Serial.printf("Mic levelinit=%d\n", micLevelInit);
}

void loop() {
  micLevel = analogRead(ADC);

  if (micLevel >= micLevelInit + threshold)
  {
     Serial.printf("Above threshold=%d\n", micLevel);
     digitalWrite(BLUELED, ON);
  }
  else if (micLevel <= micLevelInit - threshold)
  {
    Serial.printf("init=%d, current=%d\n", micLevelInit, micLevel);
  }
  else
  {
    digitalWrite(BLUELED, OFF);
  }
  
  delay(100);
}