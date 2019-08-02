/*
   Block 1 - Build 022
   TeensyLC R2P2
   Repeatable Recoverable Payload Package

   WA6PZB - 02/25/2019

   5/20/2019 - Modified battVoltage section for actual voltage with no decimal point (divide by 100)
             - Serial debug printing integer GPS altitude in meters
   6/23/2019 - 019: added altitude function and calling in "B" telemetry slot
   6/25/2019 - 020: added character array terminator for altitude and the status flag variable
   7/11/2019 - 021: added gps fix bit to statusFlags
   8/01/2019 - 022: modified gps fix section and cleaned up debug serial logging
*/

#include <TimeLib.h>
#include <TinyGPS.h>

#include <si5351.h>
#include <JTEncode.h>
#include <rs_common.h>
#include <int.h>
#include <string.h>

#include "Wire.h"

// Mode defines

#define FT8_TONE_SPACING        625          // ~6.25 Hz
#define FT8_DELAY               159          // Delay value for FT8
#define FT8_DEFAULT_FREQ        7079000UL
#define DEFAULT_MODE            MODE_FT8

#define LED_PIN                 13			// Pin 13 on TeensyLC is the LED

const int tonePin = 14;						// Pin 14 on TeensyLC for tone device


// Class instantiation
Si5351 si5351;
JTEncode jtencode;

TinyGPS gps;

// Global variables for JTEncode
unsigned long freq;
char message[] = "B01AAAAAVVVFF";

uint8_t tx_buffer[255];
uint8_t symbol_count;
uint16_t tone_delay, tone_spacing;

// General global variables
char grid[] = "AAAAAAAAAAAAA";  // gridPrint char array
char volt[] = "1234567890";     // battVoltage char array
char alt[] = "12345";           // altitude char array
int missionTime = 14;            // initial mission time (normally 0)
int gpsAge = 0;
char missionTime_chr[] = "FFFF";
char missionTime_chr2[] = "FFFF";

int statusFlags = 0;             // initial status flags all clear

HardwareSerial Uart = HardwareSerial();

// Offset hours from gps time (UTC)
const int offset = 0;   // UTC zero offset
//const int offset = -5;  // Eastern Standard Time (USA)
//const int offset = -4;  // Eastern Daylight Time (USA)
//const int offset = -8;  // Pacific Standard Time (USA)
//const int offset = -7;  // Pacific Daylight Time (USA)


void setup()
{
  // initialize the digital pin as an output for tone device.
  pinMode(tonePin, OUTPUT);
  Serial.begin(115200);
  //while (!Serial) ; // Needed for Leonardo only
  Uart.begin(9600);
  //Serial.println("Waiting for GPS time ... ");

  // Initialize the Si5351
  // Change the 2nd parameter in init if using a ref osc other
  // than 25 MHz
  si5351.init(SI5351_CRYSTAL_LOAD_8PF, 0, 0);

  // Use the Arduino's on-board LED as a keying indicator.
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);



  // Set the proper frequency, tone spacing, symbol count, and
  // tone delay depending on mode


  freq = FT8_DEFAULT_FREQ;
  symbol_count = FT8_SYMBOL_COUNT; // From the library defines
  tone_spacing = FT8_TONE_SPACING;
  tone_delay = FT8_DELAY;



  // Set CLK0 output
  si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_8MA); // Set for max power if desired
  si5351.output_enable(SI5351_CLK0, 0); // Disable the clock initially

  // Setup to read TeensyLC Vcc battery voltage
  //analogReference(DEFAULT);
  analogReadResolution(10);
  //analogReadAveraging(32);
  //PMC_REGSC |= PMC_REGSC_BGBE;



}

void loop()
{
  while (Uart.available()) {
    if (gps.encode(Uart.read())) { // process gps messages
      // when TinyGPS reports new data...

      unsigned long age;
      int Year;
      byte Month, Day, Hour, Minute, Second;

      gps.crack_datetime(&Year, &Month, &Day, &Hour, &Minute, &Second, NULL, &age);

      gpsAge = age;
      /*
        if (gps.satellites() > 3 && age < 17000) {
        bitSet(statusFlags, 0);
        }
        else {
        bitClear(statusFlags, 0);
        }
        Serial.print(" FIX=");
        Serial.println(statusFlags); // Age
        Serial.println(missionTime_chr2);    // Debug - missionTime in hex
      */


      if (age < 500) {
        // set the Time to the latest GPS reading
        setTime(Hour, Minute, Second, Day, Month, Year);
        adjustTime(offset * SECS_PER_HOUR);
      }
    }
  }

  //convert missionTime to char array
  String missionTime_str = String(missionTime, HEX);
  missionTime_str.toCharArray(missionTime_chr, 4);

  //add leading zero for single digit hex values
  if (missionTime < 16) {
    strcpy(missionTime_chr2, "0");
    strcat(missionTime_chr2, missionTime_chr);
  }
  else {
    strcpy(missionTime_chr2, missionTime_chr);
  }

  //Serial.println(missionTime_chr2);    // Debug - missionTime in hex

  if (timeStatus() != timeNotSet) {
    if (second() == 0) {
      //delay(1400);

      Serial.println("FRAME 1");
      Serial.print(" MissionTime=");
      Serial.println(missionTime_chr2);    // Debug - missionTime in hex


      alertTone();
      strcpy(message, "WA6PZB HAB\0");
      set_tx_buffer();          // Encode the message in the transmit buffer
      encode();
    }
    if (second() == 15 ) {
      //delay(1400);

      Serial.println("FRAME 2");
      Serial.print(" MissionTime=");
      Serial.println(missionTime_chr2);    // Debug - missionTime in hex

      alertTone();
      char *grid = gridPrint();
      strcpy(message, "A");
      strcat(message, missionTime_chr2);
      strcat(message, grid);
      set_tx_buffer();          // Encode the message in the transmit buffer
      encode();
    }
    if (second() == 30 ) {
      //delay(1400);

      Serial.println("FRAME 3");
      Serial.print(" MissionTime=");
      Serial.println(missionTime_chr2);    // Debug - missionTime in hex
      Serial.print(" AGE=");
      Serial.println(gpsAge); // Age

      if (gps.satellites() >= 3 && gpsAge < 500) {
        bitSet(statusFlags, 0);
      }
      else {
        bitClear(statusFlags, 0);
      }
      Serial.print(" FIX=");
      Serial.println(statusFlags);

      alertTone();
      char *volt = battVoltage();
      strcpy(message, "B");
      strcat(message, missionTime_chr2);
      strcat(message, volt);
      char *alt = altitude();
      strcat(message, alt); // add altitude char array
      set_tx_buffer();          // Encode the message in the transmit buffer
      encode();
    }
    if (second() == 45 ) {
      //delay(1400);

      Serial.println("FRAME 4");
      Serial.print(" MissionTime=");
      Serial.println(missionTime_chr2);    // Debug - missionTime in hex

      alertTone();
      missionTime++;
    }
  }
}

