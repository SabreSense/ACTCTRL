/*
 Name:		ConfigMemSet.ino
 Created:	12/3/2017 3:02:37 PM
 Author:	AdamD
*/

#include <EEPROM.h>

// the setup function runs once when you press reset or power the board
void setup() {
	// initialize the LED pin as an output.
	pinMode(13, OUTPUT);

	/***
	Iterate through each byte of the EEPROM storage.

	Larger AVR processors have larger EEPROM sizes, E.g:
	- Arduno Duemilanove: 512b EEPROM storage.
	- Arduino Uno:        1kb EEPROM storage.
	- Arduino Mega:       4kb EEPROM storage.

	Rather than hard-coding the length, you should use the pre-provided length function.
	This will make your code portable to all AVR processors.
	***/

	for (int i = 0; i < EEPROM.length(); i++) {
		EEPROM.write(i, 0);
	}


	// The values to set the control values to
	int controlVal = 1;
	int netVal = 1;

	// Write these values to the specific locations in memory
	EEPROM.write(0, controlVal);
	EEPROM.write(1, netVal);
	// turn the LED on when we're done
	digitalWrite(13, HIGH);
}

// the loop function runs over and over again until power down or reset
void loop() {
  
}
