#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_LEDBackpack.h>
// This code uses the SevSeg library, which can be donwnloaded from:
// https://github.com/sparkfun/SevSeg
#include <EEPROM.h>
#include <SevSeg.h>
#define DISPLAY_TYPE 1 //S7S driver
#include "settings.h"

/*
 This code intended to run on a Sparkfun 7 segment display module,
 driving a Adafruit 1.2" 7 segment backpack on i2c.

 The Sparkfun 7 segment is a sparge pressure display, and the
 1.2" backpack is the accuvolume tank volume.
 */

uint8_t buffer[80];
bool last_rc = false;
Adafruit_7segment matrix = Adafruit_7segment();
SevSeg myDisplay;

unsigned int analogValue7 = 0;
int psi_filtered = 0;

struct display
{
  char digits[4];
  unsigned char decimals;
  unsigned char cursor;
} 
display;  // displays be displays

uint8_t hex_to_dec(char d1, char d2) {
	uint8_t value = 0;

	if (d1 >= '0' && d1 <= '9') {
		value = (d1 - '0') << 4;
	} else if (d1 >= 'A' && d1 <= 'F') {
		value = (d1 - 'A' + 10) << 4;
	}

	if (d2 >= '0' && d2 <= '9') {
		value = value + d2 - '0';
	} else if (d2 >= 'A' && d2 <= 'F') {
		value = value + d2 - 'A' + 10;
	}

	return value;
}

void setup()
{
	Serial.begin(115200,SERIAL_8O1); //modbus needs odd parity in this case
	matrix.begin(0x70);
	setupDisplay();
	checkEmergencyReset();

	pinMode(A7, INPUT);
	interrupts();

	delay(1000);
	//matrix.print(9999, DEC);
	matrix.print(8888, DEC);
	matrix.writeDisplay();
	delay(500);
	matrix.print(10000, DEC);
	matrix.writeDisplay();

	display.digits[0] = ' ';
	display.digits[1] = 'G';
	display.digits[2] = 'A';
	display.digits[3] = 'L';
	display.decimals = 0;
	myDisplay.DisplayString(display.digits, display.decimals); //(numberToDisplay, decimal point location)

	//delay(5000);
	//Serial.println("accuvolume external display.");

}

void loop()
{
	uint8_t c = 0;

	uint8_t index = 0;
	uint16_t gallons = 0;

	/*
	analogValue7 = analogRead(A7);
	float raw = ((analogValue7 * 5) / (float)1024) / 0.016 - 60.625;
	int psi = round(raw);

	if (psi<0) {
		display.digits[0]='-';
		display.digits[1]='-';
		display.digits[2]='-';
		display.digits[3]='-';

	} else {

		psi_filtered = psi_filtered * 0.3 + psi * 0.7;

		psi = psi_filtered;

		//TODO: filter PSI

		//Serial.print(raw);
		//Serial.print(' ');
		//Serial.println(psi);

		if (psi > 99) { //three digits
			display.digits[0] = psi / 100;
			display.digits[1] = (psi / 10) % 10;
			display.digits[2] = psi % 10;
			display.digits[3] = 'x';
		} else if (psi > 9) { //two digits
			display.digits[0] = psi / 10;
			display.digits[1] = psi % 10;
			display.digits[2] = 'x';
			display.digits[3] = 'x';
		} else { //one digit
			display.digits[0] = psi;
			display.digits[1] = 'x';
			display.digits[2] = 'x';
			display.digits[3] = 'x';
		}
	}
	*/

	display.decimals = 0;
	myDisplay.DisplayString(display.digits, display.decimals); //(numberToDisplay, decimal point location)

	while (c != '%') { //look for start of message
		if(Serial.available()) {
			c = Serial.read();
		}
	}

	while (c != 13 and index < 79) { //look for end of message
		if(Serial.available()) {
			c = Serial.read();
			buffer[index] = c;
			index++;
		}
	}

	buffer[index] = 0; //terminate
	//Serial.print("buffer is length ");
	//Serial.println(index);

	if(index == 12) {
		//we have a packet that's the right length
		//maybe it's what we want.
		if (buffer[4] == 'C') {
			last_rc = true;

		} else if (buffer[4] == 'D') {
			if (last_rc) {
				//last message was an RC message, so this one will be
				//solution tank gallons.
				gallons = hex_to_dec(buffer[7], buffer[8]) << 8;
				gallons += hex_to_dec(buffer[5], buffer[6]);
				//Serial.print(gallons);
				//Serial.println(" gallons.");
const unsigned char BRIGHTNESS_ADDRESS = 0;
				matrix.print(gallons, DEC);
				matrix.writeDisplay();
				last_rc = false;
			}
		}
	}
}
