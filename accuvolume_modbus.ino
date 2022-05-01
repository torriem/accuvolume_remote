#include <BluetoothSerial.h>

uint8_t buffer[80];
bool last_rc = false;

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
	delay(5000);
	Serial.println("accuvolume external display.");
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
				Serial.print(gallons);
				Serial.println(" gallons.");
				last_rc = false;
			}
		}
	}
}
