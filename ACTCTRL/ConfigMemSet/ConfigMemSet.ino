/*
 Name:		ConfigMemSet.ino
 Created:	12/3/2017 3:02:37 PM
 Author:	AdamD
*/

#include <EEPROM.h>
#include "RedNet.h"

// the setup function runs once when you press reset or power the board
void setup() {
	// initialize the LED pin as an output.
	pinMode(13, OUTPUT);
	digitalWrite(13, LOW);
	ConfigManager configuration;

	/***
	Iterate through each byte of the EEPROM storage.

	Larger AVR processors have larger EEPROM sizes, E.g:
	- Arduno Duemilanove: 512b EEPROM storage.
	- Arduino Uno:        1kb EEPROM storage.
	- Arduino Mega:       4kb EEPROM storage.

	Rather than hard-coding the length, you should use the pre-provided length function.
	This will make your code portable to all AVR processors.
	***/

	// The values to set the control values to
	// controlVal = 0 for HHController
	int controlVal = 0;

	Serial.begin(9600);
	while (!Serial) {
	; // wait for usb serial to connect. remove for chip-to-chip serial
	}
	delay(500);
	Serial.write("Clear all memory?");
	bool moveOn = false;
	while (moveOn == false) {
		if (Serial.available()) {
			byte inByte = Serial.read();
			if (inByte == 'Y') {
				Serial.write("Clearing all memory.");
				moveOn = true;
				for (int i = 0; i < EEPROM.length(); i++) {
					EEPROM.write(i, 0);
				}
			}
			else if (inByte == 'N') {
				moveOn = true;
			}
		}
	}
	delay(500);
	moveOn = false;
	Serial.write("Which control value?");
	while (moveOn == false) {
		if (Serial.available()) {
			byte inByte = Serial.read();
			for (int ii = 0; ii <= 9; ii++) {
				if (int(inByte) - 48 == ii) {
					controlVal = ii;
					moveOn = true;
				}
			}
		}
	}
	Serial.write("Setting control value to ");
	Serial.write(controlVal + 48);
	
	// netVal = number of IW arduinos
	int netVal = 1;
	float lowerLimit = 30;
	float upperLimit = -10;
	float takeOffVal = 15;
	float servoUpLimitVal = 50;
	float servoDownLimitVal = 110;
	float servoBOffsetVal = 0;

	// Use configManager to write these values to the specific locations in memory
	configuration.SetControlID(controlVal);
	configuration.SetNetSize(netVal);
	configuration.SetLowerLimit(lowerLimit);
	configuration.SetUpperLimit(upperLimit);
	configuration.SetTakeOff(takeOffVal);
	configuration.SetServoUpLimit(servoUpLimitVal);
	configuration.SetServoDownLimit(servoDownLimitVal);
	configuration.SetServoBOffset(servoBOffsetVal);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                


	// Check values written correctly
	Serial.write(" ControlID: ");
	Serial.println(configuration.GetControlID());
	Serial.write(" NetVal: ");
	Serial.println(configuration.GetNetSize());
	Serial.write(" LowerLimit: ");
	Serial.println(float(configuration.GetLowerLimit()), 3);
	Serial.write(" UpperLimit: ");
	Serial.println(float(configuration.GetUpperLimit()), 3);
	Serial.write(" TakeOff: ");
	Serial.println(float(configuration.GetTakeOff()), 3);
	
	// turn the LED on when we're done
	digitalWrite(13, HIGH);
}

// the loop function runs over and over again until power down or reset
void loop() {
  
}
