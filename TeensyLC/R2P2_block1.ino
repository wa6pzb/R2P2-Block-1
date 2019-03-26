/*
 * Block 1 - Build 015
 * TeensyLC R2P2
 * Repeatable Recoverable Payload Package
 * 
 * WA6PZB - 02/25/2019
 *
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
char grid[] = "AAAAAAAAAAAAA";
char volt[] = "1234567890";

uint8_t tx_buffer[255];

uint8_t symbol_count;
uint16_t tone_delay, tone_spacing;



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
      if (age < 500) {
        // set the Time to the latest GPS reading
        setTime(Hour, Minute, Second, Day, Month, Year);
        adjustTime(offset * SECS_PER_HOUR);
      }
    }
  }
  if (timeStatus()!= timeNotSet) {
    if (second()==0) {
	  alertTone();
	  strcpy(message, "WA6PZB HAB\0");
	  set_tx_buffer();          // Encode the message in the transmit buffer
	  encode();
	}
	if (second()==15 ) {
      alertTone();
      char *grid = gridPrint();
	  strcpy(message, grid);
	  set_tx_buffer();          // Encode the message in the transmit buffer
	  encode();
    }
	if (second()==30 ) {
      alertTone();
      char *volt = battVoltage();
	  strcpy(message, volt);
	  set_tx_buffer();          // Encode the message in the transmit buffer
	  encode();
	}
  }
}


