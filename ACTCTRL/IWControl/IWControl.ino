/*
 Name:		IWControl.ino
 Created:	12/2/2017 5:55:06 PM
 Author:	AdamD
*/

//Copyright Adam I M Dobson 2017
//For use for AVDASI 2 TEAM A Wing Build 2017
//-------------------------------------------

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

// Library for handling encoder input
#include <Encoder-master\Encoder.h>

// Declare servo motor
Servo thisServo;
// Declare Storage of Postion
float servoAngle = 0;
float desiredAngle = 0;
float lastTransmitAngle = 0;

// Declares last error for PI loop feedback control
float lastError = 0;
float lastTime = 0;

const float servoPGain = 2;
const float servoIGain = 0.25;

// Declare storage of maximum encoder position
float encoderMaxAngle = 30;
float encoderMinAngle = -10;

const int statusGreen = 12;
const int statusRed = 13;

const int thirtyInput = 8;
const int zeroInput = 9;
const int tenInput = 10;

const int EncoderPinA = 2;
const int EncoderPinB = 3;

const int upSpeed = 0;
const int downSpeed = 180;
const int upCreepSpeed = 70;
const int downCreepSpeed = 110;
const int stopSpeed = 90;

Encoder thisEncoder(EncoderPinA, EncoderPinB);

void setup() {
	// Set servo and builtin LED pins
	thisServo.attach(7);
	thisServo.write(stopSpeed);
	pinMode(statusRed, OUTPUT);
	pinMode(statusGreen, OUTPUT);

	// Set light gate input pins
	pinMode(thirtyInput, INPUT);
	pinMode(zeroInput, INPUT);
	pinMode(tenInput, INPUT);

	digitalWrite(statusRed, LOW);
	digitalWrite(statusGreen, LOW);

	// Reset servo position
	//MoveServoToPosition(0);

	// Zero encoder (rough approximation)
	

	/*while (servoAngle != 0) {
		ServoControl();
	}*/
	
	digitalWrite(statusRed, HIGH);
	digitalWrite(statusGreen, LOW);

	// Begin network communications
	comNet.Begin();
	//while (!Serial) {
	//; // Wait for USB serial to connect. Remove for chip-to-chip serial
	//}

	// Establish contact with controller and convey initial position
	comNet.establishContact(0);

	//encoderTest();

	initialiseServos();

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

	ServoControl();

	float transmitDiff = lastTransmitAngle - servoAngle;

	if (abs(transmitDiff) > 1) {
		comNet.sendSerialCommand(2, 1, servoAngle);
		digitalWrite(statusRed, HIGH);
		digitalWrite(statusGreen, LOW);
	}

	// If command to be sent, send command
	bool sent = comNet.WriteSerialCommand();
	if (sent) {
		lastTransmitAngle = servoAngle;
		digitalWrite(statusRed, LOW);
		digitalWrite(statusGreen, HIGH);
	}
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
		comNet.sendSerialCommand(2, 1, servoAngle);

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

// Function to enact a servo response. Kept from days of regular servos, and maintained so that a return to regular servos
// would not be too diffulct to enable
void MoveServoToPosition(float angle) {
	//servoAngle = angle + 90;
	//servoAngle = (angle + 10) * 180 / 40;
	//thisServo.write(servoAngle);
	//delay(3000);
	//thisServo.write(90);
	desiredAngle = angle;
}

// Function to actually control the servos, including taking imputs as relevent. Should be nested in loops.
void ServoControl() {
	/*if (digitalRead(thirtyInput) == HIGH) {
		servoAngle = 30;
	}
	else if (digitalRead(tenInput) == HIGH) {
		servoAngle = -10;
	}
	else if (digitalRead(zeroInput) == HIGH) {
		servoAngle = 0;
	}*/

	float linearAngle = (((thisEncoder.read() - encoderMinAngle) / (encoderMaxAngle - encoderMinAngle)) * 40);

	servoAngle = (0.025*pow(linearAngle, 2)) + 4 * pow(10, -15)*linearAngle - 10;

	float error = desiredAngle - servoAngle;

	//float deltaError = error - lastError;

	float integralError = (micros() - lastTime)*(error - lastError);
	lastError = error;
	lastTime = micros();

	float deadzoneCorrection = 0;

	if (abs(error) < 5 && abs(error) > 1) {
		deadzoneCorrection = 5;
		if (error < 0) {
			deadzoneCorrection = -deadzoneCorrection;
		}
	}

	// DEBUG ONLY
	//Serial.print(error);

	// Simple servo movement based upon light gate sensesing
	/*if (servoAngle > desiredAngle) {
		if ((servoAngle - desiredAngle) < 3) {
			thisServo.write(upCreepSpeed);
		}
		else {
			thisServo.write(upSpeed);
		}
	}
	else if (servoAngle < desiredAngle) {
		if ((desiredAngle - servoAngle) < 3) {
			thisServo.write(downCreepSpeed);
		}
		else {
			thisServo.write(downSpeed);
		}
	}
	else {
		thisServo.write(stopSpeed);
	}*/
	
	//thisServo.write(stopSpeed + error*servoPGain + integralError*servoIGain);
	float servoPower = stopSpeed + error*servoPGain + deadzoneCorrection;

	thisServo.write(servoPower);
}

void initialiseServos() {
	thisServo.write(upCreepSpeed);
	delay(200);

	int lastEncoderPos = thisEncoder.read();
	int timeOut = 0;

	while (timeOut < 10)
	{
		int newPos = thisEncoder.read();
		int encoderDiff = newPos - lastEncoderPos;
		if (encoderDiff != 0) {
			timeOut = 0;
		}
		timeOut = timeOut + 1;
		lastEncoderPos = newPos;
		delay(200);

		// DEBUG ONLY
		//Serial.print(encoderDiff);
		Serial.print(newPos);
	}

	thisServo.write(stopSpeed);
	encoderMinAngle = thisEncoder.read();
	lastEncoderPos = thisEncoder.read();
	timeOut = 0;
	delay(100);
	thisServo.write(downCreepSpeed);
	delay(200);
	while (timeOut < 10)
	{
		int newPos = thisEncoder.read();
		int encoderDiff = newPos - lastEncoderPos;
		if (encoderDiff != 0) {
			timeOut = 0;
		}
		timeOut = timeOut + 1;
		lastEncoderPos = newPos;
		delay(200);

		// DEBUG ONLY
		//Serial.print(encoderDiff);
		Serial.print(newPos);
	}

	thisServo.write(stopSpeed);
	encoderMaxAngle = thisEncoder.read();
	servoAngle = 30;
	//thisEncoder.
}



void encoderTest() {
	float newPos;
	float oldPos;
	int countOut = 0;

	while (countOut < 50) {
		newPos = thisEncoder.read();
		if (newPos != oldPos) {
			Serial.print(newPos);
			oldPos = newPos;
		}
	}	
}
