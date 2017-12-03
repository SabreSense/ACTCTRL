/*
 Name:		HHControl.ino
 Created:	12/2/2017 5:54:31 PM
 Author:	AdamD
*/

//Copyright Adam I M Dobson 2017
//For use for AVDASI 2 TEAM A Wing Build 2017
//-------------------------------------------

// SUMMARY
// -------
// A set of serial functions serving to lay groundwork for communication between arduinos. 
// The arduino will return instruction commands in the standard format when buttons are pressed

// FOR DEBUGGING PURPOSES
// Ping test packet: ABCD100000000210004
// Get control number: ABCD190000000210013
// Set control number: ABCD000000X0021000(3+X)
// e.g. Set to 1: ABCD00000010021000004 (Net size 2)
// e.g. Set to 2: ABCD00000020021000005 (Net size 2)
// Get net size:  ABCD180000000210012
// Set net size:  ABCD010000X0021000(4+X)
// e.g. Set to 1: ABCD01000010021000005 (Net size 2)
// e.g. Set to 2: ABCD010000200210006 (Net size 1)
// Update flap display: ABCD210003000002109 (30 deg)
// Update flap display: ABCD210000000002106 (0 deg)
// Update flap display: ABCD211001000002108 (-10 deg)
// ------------------------------------------------------------------
// NB Standard packet size is 19 bytes + 2 bytes for each controller.
// ------------------------------------------------------------------

// SETUP
// -----

// Library for driving servo motors
#include <Servo.h>

// Libraries for use with the adafruit LCD backpack
#include "Wire.h"
#include "Adafruit_LiquidCrystal.h"

// Libraries for using the redundant communications network
#include "RedNet.h"

// Redundant Network Communications Definition
SerialCom comNet;

// Pin address declarations
const int tenBtnNum = 3;
const int zeroBtnNum = 12;
const int thrityBtnNum = 11;

const int motorLight = 9;
const int setLight = 8;
const int tenLight = 7;
const int zeroLight = 6;
const int thirtyLight = 4;

// Variables for keeping track of the flap position
float flapAvePos = 0;
float flapSetPos = 0;

// LCD Definitions
// Connect via i2c, default address #0 (A0-A2 not jumpered)
Adafruit_LiquidCrystal lcd(0);

const int rowSize = 4;
const int columnSize = 20;

void setup() {
	// put your setup code here, to run once:
	pinMode(tenBtnNum, INPUT);
	pinMode(zeroBtnNum, INPUT);
	pinMode(thrityBtnNum, INPUT);
	pinMode(motorLight, OUTPUT);
	pinMode(setLight, OUTPUT);
	pinMode(tenLight, OUTPUT);
	pinMode(zeroLight, OUTPUT);
	pinMode(thirtyLight, OUTPUT);
	pinMode(LED_BUILTIN, OUTPUT);

	lcd.begin(columnSize, rowSize);
	lcd.setBacklight(HIGH);
	// Print boot message to the LCD.
	lcd.print("Flap Controller v0.1");
	lcd.setCursor(0, 1);
	lcd.print("Net Size: ");
	//lcd.print(netSize);
	lcd.print(comNet.configuration.netSize);
	lcd.setCursor(0, 2);
	lcd.print("Pkt Length: ");
	//lcd.print(crtPackLength);
	lcd.print(comNet.configuration.crtPackLength);

	Serial.begin(9600);
	//while (!Serial) {
	//; // Wait for USB serial to connect. Remove for chip-to-chip serial
	//}
	//establishContact();
	delay(1500);
	comNet.establishContactPing();
	clrScreen();
}

// put your main code here, to run repeatedly:
void loop() {
	// If serial input available, run serial input functions
	if (Serial.available() > 0) {
		comNet.readSerialInput();
	}

	if (digitalRead(zeroBtnNum) == HIGH)
	{
		comNet.sendSerialCommand(1, 1, 0);
		flapSetPos = 0;
	}
	else if (digitalRead(tenBtnNum) == HIGH) {
		comNet.sendSerialCommand(1, 1, -10);
		flapSetPos = -10;
	}
	else if (digitalRead(thrityBtnNum) == HIGH) {
		comNet.sendSerialCommand(1, 1, 30);
		flapSetPos = 30;
	}

	flapAvePos = comNet.serialFlapPos;

	digitalWrite(motorLight, LOW);
	digitalWrite(setLight, LOW);
	digitalWrite(tenLight, LOW);
	digitalWrite(zeroLight, LOW);
	digitalWrite(thirtyLight, LOW);

	if (isFlapAtSet()) {
		if (flapSetPos == -10) {
			digitalWrite(tenLight, HIGH);
		}
		else if (flapSetPos == 0) {
			digitalWrite(zeroLight, HIGH);
		}
		else if (flapSetPos == 30) {
			digitalWrite(thirtyLight, HIGH);
		}
		digitalWrite(setLight, HIGH);			
	}	
	else {
		digitalWrite(motorLight, HIGH);
		lcd.setCursor(0, 0);
		lcd.print("Moving to");
		lcd.print(flapSetPos);
		lcd.print("deg  ");
	}

	// LCD functions go last otherwise they interfere with smooth light operation
	lcd.setCursor(0, 1);
	lcd.print("Flap angle:");
	lcd.print(flapAvePos);
	lcd.print("deg  ");

	// If command to be sent, send command
	comNet.WriteSerialCommand();
}

// ADDITIONAL LCD DISPLAY FUNCTIONS
// --------------------------------

void clrScreen() {
	for (int ii = 0; ii <= rowSize; ii++) {
		for (int jj = 0; jj <= columnSize; jj++) {
			lcd.setCursor(jj, ii);
			lcd.print(" ");
		}
	}
}

// FLAP ANGLE CHECKING
// -------------------

bool isFlapAtSet() {
	if ((flapAvePos <= flapSetPos + 0.5) && (flapAvePos >= flapSetPos - 0.5)) {
		return true;
	}
	else {
		return false;
	}
}
