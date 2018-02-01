/*
 Name:		IWControl.ino
 Created:	12/2/2017 5:55:06 PM
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
// Servo test packet: ABCD110003000210008 (30 Deg)
// Servo test packet: ABCD110000000210005 (0 Deg)
// Servo test packet: ABCD111001000210007 (-10 Deg)
// ------------------------------------------------------------------
// NB Standard packet size is 19 bytes + 2 bytes for each controller.
// ------------------------------------------------------------------

// SETUP
// -----

// Libraries for using the redundant communications network
#include "RedNet.h"

// Redundant Network Communications Definition
SerialCom comNet;

// Library for handling servo motor
#include <Servo.h>

// Declare servo motor
Servo thisServo;
// Declare Storage of Postion
float servoAngle;

const int statusGreen = 12;
const int statusRed = 13;

void setup() {
	// Set servo and builtin LED pins
	thisServo.attach(2);
	pinMode(statusRed, OUTPUT);
	pinMode(statusGreen, OUTPUT);

	digitalWrite(statusRed, HIGH);
	digitalWrite(statusGreen, LOW);

	// Reset servo position
	MoveServoToPosition(0);
	
	// Begin network communications
	comNet.Begin();
	//while (!Serial) {
	//; // Wait for USB serial to connect. Remove for chip-to-chip serial
	//}
	//comNet.establishContact();

	// Establish contact with controller and convey initial position
	comNet.establishContact(0);

	digitalWrite(statusRed, LOW);
	digitalWrite(statusGreen, HIGH);
}

// put your main code here, to run repeatedly:
void loop() {

	// If serial input available, run serial input functions
	if (Serial.available() > 0) {
		SerialCommand command = comNet.readSerialInput();
		if (command.notEmpty) {
			//DEBUG ONLY
			//Serial.print("Executing command");
			executeCommand(command);
		}
	}

	// If command to be sent, send command
	comNet.WriteSerialCommand();
}

// SERIAL INPUT AND COMMAND EXECUTION
// ----------------------------------

void executeCommand(SerialCommand command) {
	switch (command.Type) {
	case 1:
	{
		float angle = command.Value;

		//FOR DEBUGGING
		/*Serial.print(angle);
		if (command.notEmpty) {
			Serial.println("Not Empty");
		}
		else {
			Serial.println("Empty");
		}*/

		MoveServoToPosition(angle);
		//createOutputPacket(2, 1, angle, responsePacket);
		//Serial.write(responsePacket, crtPackLength);
		comNet.sendSerialCommand(2, 1, angle);

		// DEBUG ONLY
		//Serial.write(" Moving Servo: ");
		//Serial.print(angle);  
		//Serial.write(" degrees. ");
	}
	break;
	default:
		//Serial.print("No instruction");
		break;
	}
}

// SERVO RESPONSE
// --------------

void MoveServoToPosition(float angle) {
	//servoAngle = angle + 90;
	servoAngle = (angle + 10) * 180 / 40;
	thisServo.write(servoAngle);
	delay(3000);
	thisServo.write(90);
}
