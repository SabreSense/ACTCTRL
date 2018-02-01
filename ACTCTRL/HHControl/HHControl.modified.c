/*
 Name:		HHControl.ino
 Created:	12/2/2017 5:54:31 PM
 Author:	AdamD
            DanM
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

// Redundant Network Communications Definition
SerialCom comNet;

// Pin address declarations
const int tenBtnNum = A3;
const int zeroBtnNum = A2;
const int takeoffBtnNum = A1;
const int thrityBtnNum = A0;

//const int motorLight = 9;
//const int setLight = 8;
//const int tenLight = 7;
//const int zeroLight = 6;
//const int thirtyLight = 4;

const int tenGreen = 5;
const int tenRed = 4;
const int zeroGreen = 7;
const int zeroRed = 6;
const int takeoffGreen = 9;
const int takeoffRed = 8;
const int thrityGreen = 11;
const int thirtyRed = 10;
const int statusGreen = 13;
const int statusRed = 12;

// Variables for keeping track of the flap position
float flapAvePos = 0;
float flapSetPos = 0;
float flapTakeoffAngle = 15;

// LCD Definitions
// Connect via i2c, default address #0 (A0-A2 not jumpered)
Adafruit_LiquidCrystal lcd(0);

const int rowSize = 4;
const int columnSize = 20;

//Array to store desired flap positions
int FlapPosArray[1000] = {};
//Initialise first position of the array with 0
FlapPosArray[0] = 0;
//Variable used to count the position in the array at which the loop runs (will increase each loop with 1) 
int k = 1;

void setup() {
	// put your setup code here, to run once:
	// Set up input button pins (using analog pins A0-A3)
	pinMode(tenBtnNum, INPUT);
	pinMode(zeroBtnNum, INPUT);
	pinMode(takeoffBtnNum, INPUT);
	pinMode(thrityBtnNum, INPUT);
	// Set up the output pins
	pinMode(tenGreen, OUTPUT);
	pinMode(tenRed, OUTPUT);
	pinMode(zeroGreen, OUTPUT);
	pinMode(zeroRed, OUTPUT);
	pinMode(takeoffGreen, OUTPUT);
	pinMode(takeoffRed, OUTPUT);
	pinMode(thrityGreen, OUTPUT);
	pinMode(thirtyRed, OUTPUT);
	pinMode(statusGreen, OUTPUT);
	pinMode(statusRed, OUTPUT);
	//pinMode(LED_BUILTIN, OUTPUT);

	// Initialise the pin outputs (statusRed high showing not initialised)
	digitalWrite(tenGreen, LOW);
	digitalWrite(tenRed, LOW);
	digitalWrite(zeroGreen, LOW);
	digitalWrite(zeroRed, LOW);
	digitalWrite(takeoffGreen, LOW);
	digitalWrite(takeoffRed, LOW);
	digitalWrite(thrityGreen, LOW);
	digitalWrite(thirtyRed, LOW);
	digitalWrite(statusGreen, LOW);
	digitalWrite(statusRed, LOW);

	// Test induvidual colors
	digitalWrite(tenGreen, HIGH);
	digitalWrite(tenRed, LOW);
	digitalWrite(zeroGreen, HIGH);
	digitalWrite(zeroRed, LOW);
	digitalWrite(takeoffGreen, HIGH);
	digitalWrite(takeoffRed, LOW);
	digitalWrite(thrityGreen, HIGH);
	digitalWrite(thirtyRed, LOW);
	digitalWrite(statusGreen, HIGH);
	digitalWrite(statusRed, LOW);

	delay(1000);

	digitalWrite(tenGreen, LOW);
	digitalWrite(tenRed, HIGH);
	digitalWrite(zeroGreen, LOW);
	digitalWrite(zeroRed, HIGH);
	digitalWrite(takeoffGreen, LOW);
	digitalWrite(takeoffRed, HIGH);
	digitalWrite(thrityGreen, LOW);
	digitalWrite(thirtyRed, HIGH);
	digitalWrite(statusGreen, LOW);
	digitalWrite(statusRed, HIGH);

	delay(1000);

	// Set all to low, except statusRed showing not initialised
	digitalWrite(tenGreen, LOW);
	digitalWrite(tenRed, LOW);
	digitalWrite(zeroGreen, LOW);
	digitalWrite(zeroRed, LOW);
	digitalWrite(takeoffGreen, LOW);
	digitalWrite(takeoffRed, LOW);
	digitalWrite(thrityGreen, LOW);
	digitalWrite(thirtyRed, LOW);
	digitalWrite(statusGreen, LOW);
	digitalWrite(statusRed, HIGH);


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
	digitalWrite(statusGreen, HIGH);
	digitalWrite(statusRed, LOW);
}

// put your main code here, to run repeatedly:
void loop() {
	// If serial input available, run serial input functions
	if (Serial.available() > 0) {
		comNet.readSerialInput();
	}

	if (digitalRead(zeroBtnNum) == LOW)
	{
		comNet.sendSerialCommand(1, 1, 0);
		flapSetPos = 0;
	    FlapPosArray[k] == flapSetPos; 
	}
	else if (digitalRead(tenBtnNum) == LOW) {
		comNet.sendSerialCommand(1, 1, -10);
		flapSetPos = -10;
		FlapPosArray[k] == flapSetPos; 
	}
	else if (digitalRead(takeoffBtnNum) == LOW) {
		comNet.sendSerialCommand(1, 1, flapTakeoffAngle);
		flapSetPos = flapTakeoffAngle;
		FlapPosArray[k] == flapSetPos; 
	}
	else if (digitalRead(thrityBtnNum) == LOW) {
		comNet.sendSerialCommand(1, 1, 30);
		// Set desired position
		flapSetPos = 30;
		FlapPosArray[k] == flapSetPos; 
	}

	// Update current position
	flapAvePos = comNet.serialFlapPos;

	/*digitalWrite(tenGreen, LOW);
	digitalWrite(tenRed, HIGH);
	digitalWrite(zeroGreen, LOW);
	digitalWrite(zeroRed, HIGH);
	digitalWrite(takeoffGreen, LOW);
	digitalWrite(takeoffRed, HIGH);
	digitalWrite(thrityGreen, LOW);
	digitalWrite(thirtyRed, HIGH);
	digitalWrite(statusGreen, HIGH);
	digitalWrite(statusRed, LOW);*/

	/*if (isFlapAtSet()) {
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
	}*/

	if (!isFlapAtSet()) {
		lcd.setCursor(0, 1);
		lcd.print("Moving to ");
		lcd.print(flapSetPos);
		lcd.print("deg ");
	}
	else {
		lcd.setCursor(0, 1);
		lcd.print("                    ");
	}

	if (flapAvePos < 0) {
	   
        // fade in from LOW to HIGH
        for (int fadeValue = 0 ; fadeValue <= 255; fadeValue += (int)(1 + abs((FlapPosArray[k] - FlapPosArray[k-1])/100))) {
        // sets the value (range from 0 to 255)
        analogWrite(tenGreen, fadeValue);
        // wait for 30 milliseconds to see the dimming effect
        delay(30);
        }

       digitalWrite(tenRed, LOW);
	}
	else {
	    // fade out from HIGH to LOW
        for (int fadeValue = 255 ; fadeValue >= 0; fadeValue -= (int)(1 + abs((FlapPosArray[k] - FlapPosArray[k-1])/100))) {
        analogWrite(tenGreen, fadeValue);
        delay(30);
        }
        
        digitalWrite(tenRed, HIGH);
	}

	if (flapAvePos < flapTakeoffAngle && flapAvePos > -10) {
	    // fade in from LOW to HIGH
        for (int fadeValue = 0 ; fadeValue <= 255; fadeValue += (int)(1 + abs((FlapPosArray[k] - FlapPosArray[k-1])/100))) {
        analogWrite(zeroGreen, fadeValue);
        delay(30);
        }
        
        digitalWrite(zeroRed, LOW)
	}
	else {
	    // fade in from HIGH to LOW
        for (int fadeValue = 255 ; fadeValue >= 0; fadeValue -= (int)(1 + abs((FlapPosArray[k] - FlapPosArray[k-1])/100))) {
        analogWrite(zeroGreen, fadeValue);
        delay(30);
        }
        
        digitalWrite(zeroRed, HIGH);
	}

	if (flapAvePos < 30 && flapAvePos > 0) {
	    // fade in from LOW to HIGH
        for (int fadeValue = 0 ; fadeValue <= 255; fadeValue += (int)(1 + abs((FlapPosArray[k] - FlapPosArray[k-1])/100))) {
        analogWrite(takeoffGreen, fadeValue);
        delay(30);
        }
        
        digitalWrite(takeoffRed, LOW);
	}
	else {
	    // fade in from HIGH to LOW
        for (int fadeValue = 255 ; fadeValue >= 0; fadeValue -= (int)(1 + abs((FlapPosArray[k] - FlapPosArray[k-1])/100))) {
        analogWrite(takeoffGreen, fadeValue);
        delay(30);
        }
        
        digitalWrite(takeoffRed, HIGH);
	}

	if (flapAvePos > flapTakeoffAngle) {
	    // fade in from LOW to HIGH
        for (int fadeValue = 0 ; fadeValue <= 255; fadeValue += (int)(1 + abs((FlapPosArray[k] - FlapPosArray[k-1])/100))) {
        analogWrite(thrityGreen, fadeValue);
        delay(30);
        }
        
        digitalWrite(thirtyRed, LOW);
	}
	else {
	    // fade in from HIGH to LOW
        for (int fadeValue = 255 ; fadeValue >= 0; fadeValue -= (int)(1 + abs((FlapPosArray[k] - FlapPosArray[k-1])/100))) {
        analogWrite(thrityGreen, fadeValue);
        delay(30);
        }
        
        digitalWrite(thirtyRed, HIGH);
	}

    //Increment k for the next loop
    k++;
    
	// LCD functions go last otherwise they interfere with smooth light operation
	lcd.setCursor(0, 0);
	lcd.print("Flap angle:");
	if (flapAvePos > 0) {
		lcd.print(" ");
	}
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