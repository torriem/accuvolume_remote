#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_LEDBackpack.h>
// This code uses the SevSeg library, which can be donwnloaded from:
// https://github.com/sparkfun/SevSeg
#include <EEPROM.h>

//#define USE_SSEG 1

#include <SevSeg.h>
#include "settings.h"



#define S7S            1
#define OPENSEGMENT    2
#define S7SHIELD       3
#define DISPLAY_TYPE S7S

#ifdef USE_SSEG
SevSeg myDisplay;

#endif

/*
 This code intended to run on a Sparkfun 7 segment display module,
 driving a Adafruit 1.2" 7 segment backpack on i2c.

 The Sparkfun 7 segment is a sparge pressure display, and the
 1.2" backpack is the accuvolume tank volume.
 */

uint8_t buffer[80];
bool last_rc = false;
Adafruit_7segment matrix = Adafruit_7segment();

unsigned int analogValue7 = 0;
int psi_filtered = 0;

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

struct display
{
  char digits[4];
  unsigned char decimals;
  unsigned char cursor;
} 
display;  // displays be displays

void set_digits(struct display &display, uint16_t number) {
	if (number > 999) { //three digits
		display.digits[0] = number / 1000;
		display.digits[1] = (number / 100) % 10;
		display.digits[2] = (number / 10) % 10;
		display.digits[3] = number % 10;
	} else if (number > 99) { //three digits
		display.digits[0] = number / 100;
		display.digits[1] = (number / 10) % 10;
		display.digits[2] = number % 10;
		display.digits[3] = 'x';
	} else if (number > 9) { //two digits
		display.digits[0] = number / 10;
		display.digits[1] = number % 10;
		display.digits[2] = 'x';
		display.digits[3] = 'x';
	} else { //one digit
		display.digits[0] = number;
		display.digits[1] = 'x';
		display.digits[2] = 'x';
		display.digits[3] = 'x';
	}
}

void setup()
{
#ifdef USE_SSG	
	setupDisplay(myDisplay);
	myDisplay.SetBrightness(100);
#endif
	Serial.begin(115200,SERIAL_8O1); //modbus needs odd parity in this case
	//Serial.begin(115200);

	pinMode(A7, INPUT);
	//interrupts();

	delay(5000);
	Serial.println("initializing big display.");
	if(matrix.begin(0x70)) {
		delay(500);
		matrix.print(8888, DEC);
		matrix.writeDisplay();

		delay(500);
		matrix.print(10000, DEC);
		matrix.writeDisplay();

#ifdef USE_SSG
		set_digits(display,8888);
		while(1) {
			myDisplay.DisplayString(display.digits, display.decimals); //(numberToDisplay, decimal point location)
			
		}
#endif
	} else {
		while (1) {
#ifdef USE_SSEG
			for(int i=0; i<100; i++) {
				display.digits[0] = 'I';
				display.digits[1] = '2';
				display.digits[2] = 'c';
				display.digits[3] = ' ';
				display.decimals = 0;
			
				myDisplay.DisplayString(display.digits, display.decimals); //(numberToDisplay, decimal point location)
				delay(10);
			}

			for(int i=0; i<100; i++) {
				display.digits[0] = 'E';
				display.digits[1] = 'r';
				display.digits[2] = 'r';
				display.digits[3] = ' ';
				display.decimals = 0;
			
				myDisplay.DisplayString(display.digits, display.decimals); //(numberToDisplay, decimal point location)
				delay(10);
			}
#else
			Serial.println("I2C Error!");
			delay(1000);
#endif
		}
	}

	

	//matrix.print(9999, DEC);


	//delay(5000);
	//Serial.println("accuvolume external display.");
	
}

void loop()
{
	uint8_t c = 0;

	uint8_t index = 0;
	uint16_t gallons = 0;

	display.digits[0] = ' ';
	display.digits[1] = 'G';
	display.digits[2] = 'A';
	display.digits[3] = 'L';
	display.decimals = 0;
	
#ifdef USE_SSEG
	myDisplay.DisplayString(display.digits, display.decimals); //(numberToDisplay, decimal point location)
#endif
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

	display.decimals = 0;
	myDisplay.DisplayString(display.digits, display.decimals); //(numberToDisplay, decimal point location)
	*/

	while (c != '%') { //look for start of message
		if(Serial.available()) {
			c = Serial.read();
		}
#ifdef USE_SSEG
		myDisplay.DisplayString(display.digits, display.decimals); //(numberToDisplay, decimal point location)
#endif
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
	//Serial.println((const char *)buffer);
	//Serial.println(index);
	set_digits(display, (uint16_t) index);

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
				Serial.print(gallons);
				Serial.println(" gallons.");
				matrix.print(gallons, DEC);
				matrix.writeDisplay();
				last_rc = false;
			}
		}
	}
}
