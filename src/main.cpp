// TV Volume Leveler
// Microphone threshold code based on https://github.com/joekrie/arduino-volume-detector/blob/master/volume-detector/volume-detector.ino
// IR Receiver/Transmitter code from https://github.com/mdhiggins/ESP8266-HTTP-IR-Blaster

#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRrecv.h>
#include <IRutils.h>

const int ADC = 17; // analog pin
const int IR_Receiver = 14; // IR Receiver
const int IR_LED = 15;
const int REDLED = 0;
const int BLUELED = 2;
const bool ON = LOW; // onboard LEDs are reversed
const bool OFF = HIGH;

// IR Receiver/Transmitter Config
const int captureBufSize = 150;             // Size of the IR capture buffer.
IRrecv irrecv(IR_Receiver, captureBufSize);
IRsend irsend(IR_LED);

// Microphone Config
const int microphoneSampleWindow = 50;  // # of milliseconds per sample
const int microphoneThreshold = 10; // tolerance of sound level difference peakToPeak

void setup() {
  pinMode(BLUELED, OUTPUT);
  pinMode(IR_LED, OUTPUT);
  Serial.begin(115200);
  irsend.begin();
  irrecv.enableIRIn();
}

//+=============================================================================
// Display encoding type
//
String encoding(decode_results *results) {
  String output;
  switch (results->decode_type) {
    default:
    case UNKNOWN:      output = "UNKNOWN";            break;
    case NEC:          output = "NEC";                break;
    case SONY:         output = "SONY";               break;
    case RC5:          output = "RC5";                break;
    case RC6:          output = "RC6";                break;
    case DISH:         output = "DISH";               break;
    case SHARP:        output = "SHARP";              break;
    case JVC:          output = "JVC";                break;
    case SANYO:        output = "SANYO";              break;
    case SANYO_LC7461: output = "SANYO_LC7461";       break;
    case MITSUBISHI:   output = "MITSUBISHI";         break;
    case SAMSUNG:      output = "SAMSUNG";            break;
    case LG:           output = "LG";                 break;
    case WHYNTER:      output = "WHYNTER";            break;
    case AIWA_RC_T501: output = "AIWA_RC_T501";       break;
    case PANASONIC:    output = "PANASONIC";          break;
    case DENON:        output = "DENON";              break;
    case COOLIX:       output = "COOLIX";             break;
    case GREE:         output = "GREE";               break;
    case LUTRON:       output = "LUTRON";             break;
  }
  return output;
}

//+=============================================================================
// Code to string
//
void fullCode (decode_results *results)
{
  Serial.print("One line: ");
  serialPrintUint64(results->value, 16);
  Serial.print(":");
  Serial.print(encoding(results));
  Serial.print(":");
  Serial.print(results->bits, DEC);
  if (results->repeat) Serial.print(" (Repeat)");
  Serial.println("");
  if (results->overflow)
    Serial.println("WARNING: IR code too long. "
                   "Edit IRController.ino and increase captureBufSize");
}

//+=============================================================================
// Dump out the decode_results structure.
//
void dumpInfo(decode_results *results) {
  if (results->overflow)
    Serial.println("WARNING: IR code too long. "
                   "Edit IRrecv.h and increase RAWBUF");

  // Show Encoding standard
  Serial.print("Encoding  : ");
  Serial.print(encoding(results));
  Serial.println("");

  // Show Code & length
  Serial.print("Code      : ");
  serialPrintUint64(results->value, 16);
  Serial.print(" (");
  Serial.print(results->bits, DEC);
  Serial.println(" bits)");
}


//+=============================================================================
// Dump out the decode_results structure.
//
void dumpRaw(decode_results *results) {
  // Print Raw data
  Serial.print("Timing[");
  Serial.print(results->rawlen - 1, DEC);
  Serial.println("]: ");

  for (uint16_t i = 1;  i < results->rawlen;  i++) {
    if (i % 100 == 0)
      yield();  // Preemptive yield every 100th entry to feed the WDT.
    uint32_t x = results->rawbuf[i] * kRawTick;
    if (!(i & 1)) {  // even
      Serial.print("-");
      if (x < 1000) Serial.print(" ");
      if (x < 100) Serial.print(" ");
      Serial.print(x, DEC);
    } else {  // odd
      Serial.print("     ");
      Serial.print("+");
      if (x < 1000) Serial.print(" ");
      if (x < 100) Serial.print(" ");
      Serial.print(x, DEC);
      if (i < results->rawlen - 1)
        Serial.print(", ");  // ',' not needed for last one
    }
    if (!(i % 8)) Serial.println("");
  }
  Serial.println("");  // Newline
}


//+=============================================================================
// Dump out the decode_results structure.
//
void dumpCode(decode_results *results) {
  // Start declaration
  Serial.print("uint16_t  ");              // variable type
  Serial.print("rawData[");                // array name
  Serial.print(results->rawlen - 1, DEC);  // array size
  Serial.print("] = {");                   // Start declaration

  // Dump data
  for (uint16_t i = 1; i < results->rawlen; i++) {
    Serial.print(results->rawbuf[i] * kRawTick, DEC);
    if (i < results->rawlen - 1)
      Serial.print(",");  // ',' not needed on last one
    if (!(i & 1)) Serial.print(" ");
  }

  // End declaration
  Serial.print("};");  //

  // Comment
  Serial.print("  // ");
  Serial.print(encoding(results));
  Serial.print(" ");
  serialPrintUint64(results->value, 16);

  // Newline
  Serial.println("");

  // Now dump "known" codes
  if (results->decode_type != UNKNOWN) {
    // Some protocols have an address &/or command.
    // NOTE: It will ignore the atypical case when a message has been decoded
    // but the address & the command are both 0.
    if (results->address > 0 || results->command > 0) {
      Serial.print("uint32_t  address = 0x");
      Serial.print(results->address, HEX);
      Serial.println(";");
      Serial.print("uint32_t  command = 0x");
      Serial.print(results->command, HEX);
      Serial.println(";");
    }

    // All protocols have data
    Serial.print("uint64_t  data = 0x");
    serialPrintUint64(results->value, 16);
    Serial.println(";");
  }
}

//+=============================================================================
// Read IR Receiver then decode and print the IR Code
//
void readAndPrintIrCode()
{
    // Read IR Receiver and print results in HEX
  decode_results results;
  if (irrecv.decode(&results))
  {
    //Serial.print("HEX=" + uint64ToString(results.value, 16) + "\r\n");
    //fullCode(&results);
    dumpInfo(&results);
    //dumpCode(&results);
    //dumpRaw(&results);
	delay(500);
    irrecv.resume(); // Receive the next value
  }
}

//+=============================================================================
// Read from analog microphone sensor.
// Sample for a duration of milliseconds in the microphoneSampleWindow.
// Determine min/max volume for that sample duration.
// Return difference as the volume.
//
int getMicrophoneVolume()
{
  long startMillis = millis();
  int signalMax = 0;
  int signalMin = 1024;

  while (millis() - startMillis < microphoneSampleWindow)
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
  Serial.printf("Min=%d, Max=%d, Diff=%d\n", signalMin, signalMax, signalMax - signalMin);
  return signalMax - signalMin;  // we care about the difference
}

const int IR_Enabled=true;

void runVolumeLeveler()
{
  int volume = getMicrophoneVolume();

  if (volume > microphoneThreshold)
  {
    digitalWrite(BLUELED, ON);

	// Send volume down code if IR enabled
	if (IR_Enabled)
	{
    	irsend.sendPanasonic64(0x400401008485);
		delay(500);
	}
  }
  else
  {
    digitalWrite(BLUELED, OFF);
  }
}

void loop() {
  readAndPrintIrCode();
  //runVolumeLeveler();
}