// TV Volume Leveler
// Microphone threshold code based on https://github.com/joekrie/arduino-volume-detector/blob/master/volume-detector/volume-detector.ino
// IR Receiver/Transmitter code from https://github.com/mdhiggins/ESP8266-HTTP-IR-Blaster

#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRrecv.h>
#include <IRutils.h>
#include "IRprint.h"

const int ADC = 17; // analog pin
const int IR_Receiver = 14; // IR Receiver
const int IR_LED = 15;
const int REDLED = 0;
const int BLUELED = 2;
const bool ON = LOW; // onboard LEDs are reversed
const bool OFF = HIGH;
const int IR_Enabled = true; // disable IR blaster

// IR Receiver/Transmitter Config
const int captureBufSize = 150;             // Size of the IR capture buffer.
IRrecv irrecv(IR_Receiver, captureBufSize);
IRsend irsend(IR_LED);

// Microphone Config
const int MicrophoneSampleDurationInMillis = 50;  // # of milliseconds per sample
const int VolumeThresholdMax = 12; // tolerance of sound level difference peakToPeak
const int NumberOfVolumeSamplesToAverage = 10;

int volumeReading[NumberOfVolumeSamplesToAverage]; // array to store volume readings to average
int loopCounter = 0;

void setup() {
  pinMode(BLUELED, OUTPUT);
  pinMode(IR_LED, OUTPUT);
  Serial.begin(115200);
  if (IR_Enabled)
  {
	irsend.begin();
	irrecv.enableIRIn();
  }
}


//+=============================================================================
// Read IR Receiver then decode and print the IR Code
//
void ReadAndPrintIrCode()
{
    // Read IR Receiver and print results in HEX
  decode_results results;
  if (irrecv.decode(&results))
  {
    Serial.print("HEX=" + uint64ToString(results.value, 16) + "\r\n");
    fullCode(&results);
    dumpInfo(&results);
    dumpCode(&results);
    dumpRaw(&results);
	delay(500);
    irrecv.resume(); // Receive the next value
  }
}

//+=============================================================================
// Read from analog microphone sensor.
// Sample for a duration of milliseconds in the MicrophoneSampleDurationInMillis.
// Determine min/max volume for that sample duration.
// Return difference as the volume.
//
int SampleVolumeLevel()
{
  long startMillis = millis();
  int signalMax = 0;
  int signalMin = 1024;

  while (millis() - startMillis < MicrophoneSampleDurationInMillis)
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
  //Serial.printf("Min=%d, Max=%d, Diff=%d\n", signalMin, signalMax, signalMax - signalMin);
  return signalMax - signalMin;  // we care about the difference
}

void PrintVolumeLevel(int volume)
{
	//if (volume > VolumeThresholdMax)
	{
		Serial.printf("%d ", volume);

		int level = volume / 2;
		for (int i=0; i<level; i++)
		{
			Serial.printf(">");
		}
		
		Serial.println();
	}
	// else
	// {
	// 	Serial.println("-");
	// }
}

int ComputeAverageVolume(int volume, int loopCounter)
{
  // store the volume in the array
  volumeReading[loopCounter % NumberOfVolumeSamplesToAverage] = volume;

  int sumOfVolume = 0;
  for (int i=0; i < NumberOfVolumeSamplesToAverage; i++)
  {
	  sumOfVolume += volumeReading[i];
  }
  if (loopCounter < NumberOfVolumeSamplesToAverage)
  {
	  return sumOfVolume / (loopCounter + 1);
  }
  return sumOfVolume / NumberOfVolumeSamplesToAverage;
}

void LevelVolume()
{
  int volume = SampleVolumeLevel();
  int averageVolume = ComputeAverageVolume(volume, loopCounter);

  //PrintVolumeLevel(volume);
  //PrintVolumeLevel(averageVolume);
  
  if (averageVolume > VolumeThresholdMax)
  {
	// Send volume down code if IR enabled
	if (IR_Enabled)
	{
		// only send IR code once per NumberOfVolumeSamplesToAverage
		if (loopCounter % NumberOfVolumeSamplesToAverage == 0)
		{
    		irsend.sendPanasonic64(0x400401008485);
			digitalWrite(BLUELED, ON);
		}
	}
  }
  else
  {
    digitalWrite(BLUELED, OFF);
  }

  loopCounter++;
}

void loop() {
  //ReadAndPrintIrCode();
  LevelVolume();
}