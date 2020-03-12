#include <Arduino.h>
#include <ir_Panasonic.h>
#include <IRsend.h>
#include <IRremoteESP8266.h>

#include <ESP8266WiFi.h>

void setup() {
  pinMode(0, OUTPUT);
  Serial.begin(115200);
  Serial.println("Hello World!");
}

void loop() {
//  digitalWrite(0, HIGH); // off
//  delay(900);
//  digitalWrite(0, LOW); // on
//  delay(100);
}
