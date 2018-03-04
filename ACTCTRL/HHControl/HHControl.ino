/*
 Name:		HHControl.ino
 Created:	12/2/2017 5:54:31 PM
 Author:	AdamD
*/

// Copyright Adam I M Dobson 2017
// With assistance from:
// -Manuel Martinez
// -Dan Paul Mirea
// For use for AVDASI 2 TEAM A Wing Build 2017
// -------------------------------------------

// THIRD-PARTY LIBRARIES
// ---------------------
// Encoder.h (Paul Stoffregen, 2011) https://github.com/ericbarch/arduino-libraries/blob/master/Encoder/Encoder.h

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
// Update flap display: ABCD210001500002112 (15 deg)
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

// Library for handling encoder input
#include <Encoder-master\Encoder.h>

// Network Communications Definition
SerialCom comNet;

// Pin address declarations
const int upperLimitPin = A3;
const int zeroBtnPin = A2;
const int takeoffBtnPin = A1;
const int lowerLimitPin = A0;
const int stopBtnPin = 4;
const int serialConnectPin = 7;
const int serialSwitchPin = 8;

//const int tenGreen = 5;
//const int tenRed = 4;
//const int zeroGreen = 7;
//const int zeroRed = 6;
//const int takeoffGreen = 9;
//const int takeoffRed = 8;
//const int thrityGreen = 11;
//const int thirtyRed = 10;
const int statusGreenPin = 13;
const int statusRedPin = 12;
const int actGreenPin = 11;
const int actRedPin = 10;
const int trimIndPin = 9;

const int EncoderPinA = 2;
const int EncoderPinB = 3;

// Variables for keeping track of the flap position
float flapAvePos = 0;
float flapSetPos = 0;
float flapTakeoffAngle = 15;
bool moving = true;
bool stopSet = false;

// Fine-tune Encoder Object Definition and Position Memory Definition
Encoder thisEncoder(EncoderPinA, EncoderPinB);
int lastEncoderPos = thisEncoder.read();

// LCD Definitions
// Connect via i2c, default address #0 (A0-A2 not jumpered)
Adafruit_LiquidCrystal lcd(0);

const int rowSize = 4;
const int columnSize = 20;

void setup() {
	// put your setup code here, to run once:
	// Set up input button pins (using pins A0-A3 and D4)
	pinMode(upperLimitPin, INPUT);
	pinMode(zeroBtnPin, INPUT);
	pinMode(takeoffBtnPin, INPUT);
	pinMode(lowerLimitPin, INPUT);
	pinMode(stopBtnPin, INPUT);
	// Set up other inputs
	pinMode(serialConnectPin, INPUT);
	// Set up the output pins
	pinMode(statusGreenPin, OUTPUT);
	pinMode(statusRedPin, OUTPUT);
	pinMode(actGreenPin, OUTPUT);
	pinMode(actRedPin, OUTPUT);
	pinMode(trimIndPin, OUTPUT);
	pinMode(serialSwitchPin, OUTPUT);

	// Initialise the pin outputs (Red High First as Part of Initialisation Process)
	digitalWrite(trimIndPin, HIGH);
	digitalWrite(statusRedPin, HIGH);
	digitalWrite(statusGreenPin, LOW);
	digitalWrite(actRedPin, HIGH);
	digitalWrite(actGreenPin, LOW);

	digitalWrite(serialSwitchPin, LOW);

	//Initialise the LCD screen
	lcd.begin(columnSize, rowSize);
	lcd.setBacklight(HIGH);
	// Print boot message to the LCD.
	lcd.print("Flap Controller v0.2");
	lcd.setCursor(0, 1);
	lcd.print("Net Size: ");
	//lcd.print(netSize);
	lcd.print(comNet.configuration.NetSize());
	lcd.setCursor(0, 2);
	lcd.print("Pkt Length: ");
	//lcd.print(crtPackLength);
	lcd.print(comNet.configuration.CrtPackLength());
	lcd.setCursor(0, 3);
	lcd.print("Initialising...");

	// Test green LEDS

	delay(1000);

	digitalWrite(trimIndPin, LOW);
	digitalWrite(statusRedPin, LOW);
	digitalWrite(statusGreenPin, HIGH);
	digitalWrite(actRedPin, LOW);
	digitalWrite(actGreenPin, HIGH);

	delay(1000);

	// Reset colours so only power Red, showing connection in progress

	digitalWrite(statusRedPin, HIGH);
	digitalWrite(statusGreenPin, LOW);
	digitalWrite(actRedPin, LOW);
	digitalWrite(actGreenPin, LOW);

	delay(1500);

	// DEBUG ONLY
	//Serial.begin(9600);
	//while (!Serial) {
	//; // Wait for USB serial to connect. Remove for chip-to-chip serial
	//}
	//establishContact();

	setSerialConnection();

	// ESTABLISH COMMUNICATIONS. COMMENT OUT FOR DEBUGGING IF NECESSARY
	comNet.Begin();
	lcd.setCursor(0, 3);
	lcd.print("Connecting...   ");
	comNet.establishContactPing();
	clrScreen();
	digitalWrite(statusGreenPin, HIGH);
	digitalWrite(statusRedPin, LOW);
	lastEncoderPos = thisEncoder.read();
}

// put your main code here, to run repeatedly:
void loop() {

	// If serial input available, run serial input functions
	comNet.readSerialInput();

	if (digitalRead(zeroBtnPin) == LOW)
	{
		comNet.sendSerialCommand(1, 1, 0);
		flapSetPos = 0;
		stopSet = false;
		digitalWrite(trimIndPin, LOW);
	}
	else if (digitalRead(upperLimitPin) == LOW) {
		comNet.sendSerialCommand(1, 1, comNet.configuration.UpperLimit());
		flapSetPos = comNet.configuration.UpperLimit();
		stopSet = false;
		digitalWrite(trimIndPin, LOW);
	}
	else if (digitalRead(takeoffBtnPin) == LOW) {
		comNet.sendSerialCommand(1, 1, comNet.configuration.TakeOff());
		flapSetPos = comNet.configuration.TakeOff();
		stopSet = false;
		digitalWrite(trimIndPin, LOW);
	}
	else if (digitalRead(lowerLimitPin) == LOW) {
		comNet.sendSerialCommand(1, 1, comNet.configuration.LowerLimit());
		// Set desired position
		flapSetPos = comNet.configuration.LowerLimit();
		stopSet = false;
		digitalWrite(trimIndPin, LOW);
	}
	else if (digitalRead(stopBtnPin) == LOW) {
		comNet.sendSerialCommand(1, 2, flapAvePos);
		// Set desired position
		flapSetPos = flapAvePos;
		stopSet = true;
		digitalWrite(trimIndPin, LOW);
	}

	// Update current position from serial communication
	flapAvePos = comNet.serialFlapPos;

	int encDiff = thisEncoder.read() - lastEncoderPos;

	if (abs(encDiff) > 4) {
		int angleDiff = encDiff / 4;
		comNet.sendSerialCommand(1, 1, flapAvePos + angleDiff);
		// Set desired position
		flapSetPos = flapSetPos + angleDiff;
		if (flapSetPos > comNet.configuration.LowerLimit()) {
			flapSetPos = comNet.configuration.LowerLimit();
		}
		else if (flapSetPos < comNet.configuration.UpperLimit()) {
			flapSetPos = comNet.configuration.UpperLimit();
		}
		lastEncoderPos = thisEncoder.read();
		digitalWrite(trimIndPin, HIGH);
	}

	// LCD functions go at the end otherwise they interfere with smooth operation

	if (!isFlapAtSet()) {
		lcd.setCursor(0, 1);
		lcd.print("Moving to ");
		lcd.print(flapSetPos);
		lcd.print("deg ");
		digitalWrite(actRedPin, HIGH);
		digitalWrite(actGreenPin, LOW);
		moving = true;
	}
	else if (moving) {
		lcd.setCursor(0, 1);
		lcd.print("                    ");
		digitalWrite(actRedPin, LOW);
		digitalWrite(actGreenPin, HIGH);
		moving = false;
	}
	if (stopSet && !comNet.stopped) {
		lcd.setCursor(0, 2);
		lcd.print("STOPPING...");
	}
	else if (stopSet && comNet.stopped) {
		lcd.setCursor(0, 2);
		lcd.print("STOPPED!   ");
		stopSet = false;
	}
	else if (!stopSet && !comNet.stopped){
		lcd.setCursor(0, 2);
		lcd.print("                    ");
	}

	lcd.setCursor(0, 0);
	lcd.print("Flap angle:");
	if (flapAvePos > 0) {
		lcd.print(" ");
	}
	lcd.print(flapAvePos);
	lcd.print("deg  ");

	// Finally, if command to be sent, send command
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

// SERIAL CONNECTION CHECKING
// --------------------------

// Connect to correct serial port
void setSerialConnection() {
	if (digitalRead(serialConnectPin) == LOW) {
		digitalWrite(serialSwitchPin, LOW);
	}
	else {
		digitalWrite(serialSwitchPin, HIGH);
	}
}
