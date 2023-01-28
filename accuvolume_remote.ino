#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_LEDBackpack.h>
#include <EEPROM.h>

/*
 This code intended to run on an Arduino uno or mini
 driving a Adafruit 1.2" 7 segment backpack on i2c.
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
	Serial.begin(115200,SERIAL_8O1); //modbus needs odd parity in this case
	//Serial.begin(115200);

	pinMode(A7, INPUT);
	//interrupts();

	delay(1000);
	Serial.println("initializing big display.");
	if(matrix.begin(0x70)) {
		delay(500);
		matrix.print(8888, DEC);
		matrix.writeDisplay();

		delay(500);
		matrix.print(10000, DEC);
		matrix.writeDisplay();

	} else {
		while (1) {
			Serial.println("I2C Error!");
			delay(1000);
		}
	}

	//delay(5000);
	//Serial.println("accuvolume external display.");
	
}

void loop()
{
	uint8_t c = 0;

	uint8_t index = 0;
	uint16_t gallons = 0;

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
