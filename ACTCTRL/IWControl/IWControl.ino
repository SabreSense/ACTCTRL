/*
 Name:		IWControl.ino
 Created:	12/2/2017 5:55:06 PM
 Author:	AdamD
*/

// Copyright Adam I M Dobson 2017
// With assistance from:
// -Manuel Martinez
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
// Servo test packet: ABCD110003000210008 (30 Deg)
// Servo test packet: ABCD110000000210005 (0 Deg)
// Servo test packet: ABCD111001000210007 (-10 Deg)
// Stop test packet:  ABCD120000000210006 (STOP with Zero Degrees Bonus Info)
// ------------------------------------------------------------------
// NB Standard packet size is 19 bytes + 2 bytes for each controller.
// ------------------------------------------------------------------

// SETUP
// -----

// Libraries for using the redundant communications network
#include "DataByteConverter.h"
#include "RedNet.h"

// Library for handling servo motor
#include <Servo.h>

// Library for handling encoder input
#include <Encoder-master\Encoder.h>

// Redundant Network Communications Definition
SerialCom comNet;

// Declare servo motors
Servo servoA;
Servo servoB;

// Declare Storage of Postion
float servoAngleA = 0;
float servoAngleB = 0;
float servoAngle = (servoAngleA+servoAngleB)/2;

float desiredAngle = 0;
float lastTransmitAngle = 0;

// Declares last error for PI loop feedback control
float lastError = 0;
float lastTime = 0;

const float servoPGain = 2;
const float servoIGain = 0.25;

// Declare storage of maximum encoder positions
float encoderMaxAngleA = 30;
float encoderMinAngleA = -10;
float encoderMaxAngleB = 30;
float encoderMinAngleB = -10;

// Declare light output pins
const int statusGreen = 12;
const int statusRed = 13;

// Declare limit switch input pins
const int downAPin = 25;
const int upAPin = 23;
const int downBPin = 27;
const int upBPin = 29;

// Declare servo movement speeds
const int upSpeed = 0;
const int downSpeed = 180;
const int upCreepSpeed = 70;
const int downCreepSpeed = 110;
const int stopSpeed = 90;

const int zeroPos = 90;
int upPos = 50;
int downPos = 110;
int servoBOffset = 0;

// Declare pin to detect whether cable connected
const int connectPin = 4;
byte lastPinInput;

// Declare all-stop memory which can be checked before movement
bool stopped = true;

// Declare encoder object
Encoder encoderA(2, 3);
Encoder encoderB(18, 19);

void setup() {
	// Set servo and builtin LED pins
	servoA.attach(6);
	servoA.write(stopSpeed);
	servoB.attach(7);
	servoB.write(stopSpeed);
	pinMode(statusRed, OUTPUT);
	pinMode(statusGreen, OUTPUT);
	pinMode(connectPin, INPUT);

	// Set microswitch input pins
	pinMode(downAPin, INPUT);
	pinMode(upAPin, INPUT);
	pinMode(downBPin, INPUT);
	pinMode(upBPin, INPUT);

	// Reset servo position
	//MoveServoToPosition(0);	

	/*while (servoAngle != 0) {
		ServoControl();
	}*/
	
	digitalWrite(statusRed, HIGH);
	digitalWrite(statusGreen, LOW);

	// Begin network communications
	serialPortUpdater(0);
	
	//while (!Serial) {
	//; // Wait for USB serial to connect. Remove for chip-to-chip serial
	//}

	// Establish contact with controller and convey initial position
	comNet.establishContact(0);

	//encoderTest();
	bool stopped = false;
	delay(500);
	initialiseServos();

	digitalWrite(statusRed, HIGH);
	digitalWrite(statusGreen, LOW);
	desiredAngle = 0;
}

// put your main code here, to run repeatedly:
void loop() {
	// Update which Serial input to use
	serialPortUpdater(servoAngle);

	// If serial input available, run serial input functions
	SerialCommand command = comNet.readSerialInput();
	if (command.notEmpty) {
		//DEBUG ONLY
		//Serial.print("Executing command");
		executeCommand(command);
	}

	// Reinitialise servos if new config data available
	if (comNet.configReset = true && comNet.timedOut) {
		//initialiseServos();
		comNet.configReset = false;
	}

	if (digitalRead(upAPin) == LOW) {
		digitalWrite(LED_BUILTIN, HIGH);
		Serial.print("Up A Switch Triggered");
		delay(200);
	}
	if (digitalRead(downAPin) == LOW) {
		digitalWrite(LED_BUILTIN, HIGH);
		Serial.print("Down A Switch Triggered");
		delay(200);
	}
	if (digitalRead(upBPin) == LOW) {
		digitalWrite(LED_BUILTIN, HIGH);
		Serial.print("Up B Switch Triggered");
		delay(200);
	}
	if (digitalRead(downBPin) == LOW) {
		digitalWrite(LED_BUILTIN, HIGH);
		Serial.print("Down B Switch Triggered");
		delay(200);
	}

	ServoControl();

	float transmitDiff = lastTransmitAngle - servoAngle;

	if (abs(transmitDiff) > 1) {
		//comNet.sendSerialCommand(2, 1, servoAngle);
		// DEBUG ONLY
		//digitalWrite(statusRed, HIGH);
		//digitalWrite(statusGreen, LOW);
	}

	if (comNet.timedOut == true) {
		//comNet.sendSerialCommand(2, 1, servoAngle);
		comNet.timedOut = false;
	}

	// If command to be sent, send command
	bool sent = comNet.WriteSerialCommand();
	if (sent) {
		lastTransmitAngle = servoAngle;
		// DEBUG ONLY
		//digitalWrite(statusRed, LOW);
		//digitalWrite(statusGreen, HIGH);
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

		//createOutputPacket(2, 1, angle, responsePacket);
		//Serial.write(responsePacket, crtPackLength);
		MoveServoToPosition(angle);
		comNet.sendSerialCommand(2, 1, servoAngle);
		stopped = false;

		// DEBUG ONLY
		//Serial.write(" Moving Servo: ");
		//Serial.print(angle);  
		//Serial.write(" degrees. ");
	}
	break;
	case 2:
	{
		MoveServoToPosition(command.Value);
		servoA.write(stopSpeed);
		servoB.write(stopSpeed);
		stopped = true;
		comNet.sendSerialCommand(2, 2, servoAngle);
	}
	default:
		//Serial.print("No instruction");
		stopped = true;
		break;
	}
}

// Function for selecting correct serial port for communication
void serialPortUpdater(float flapAngle) {
	byte pinInput = digitalRead(connectPin);
	if (pinInput != lastPinInput) {
		if (pinInput == HIGH) {
			comNet.Begin(&Serial2, flapAngle);
		}
		else {
			comNet.Begin(&Serial, flapAngle);
		}
	}
	lastPinInput = pinInput;
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
	if (!stopped) {
		/*if (digitalRead(thirtyInput) == HIGH) {
		servoAngle = 30;
		}
		else if (digitalRead(tenInput) == HIGH) {
		servoAngle = -10;
		}
		else if (digitalRead(zeroInput) == HIGH) {
		servoAngle = 0;
		}*/

		/*float linearAngle = (((thisEncoder.read() - encoderMinAngle) / (encoderMaxAngle - encoderMinAngle)) * 40);

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

		float servoPower = stopSpeed + error*servoPGain + deadzoneCorrection;

		thisServo.write(servoPower);*/

		float rotaryAngleA = (((encoderA.read() - encoderMinAngleA) / (encoderMaxAngleA - encoderMinAngleA)) * 35);
		float rotaryAngleB = (((encoderB.read() - encoderMinAngleB) / (encoderMaxAngleB - encoderMinAngleB)) * 35);
		servoAngleA = rotaryToServoAngle(rotaryAngleA);
		servoAngleB = rotaryToServoAngle(rotaryAngleB);
		servoAngle = (servoAngleA + servoAngleB) / 2;
		//float servoPower = (0.025*pow(desiredAngle, 2)) + 4 * pow(10, -15)*desiredAngle - 10;
		//thisServo.write(servoPower);
		
		//float desiredServoInput = servoAngleToLinearInput(desiredAngle);
		float servoRange = downPos - upPos;
		//float desiredServoInput = rotaryToServoAngle(((desiredAngle+10)/40)*35);
		//float desiredServoInput = upPos + (((0.0152*pow(desiredAngle, 2)) + 0.6095 *desiredAngle - 10)/35);
		//float desiredServoInput = upPos + servoRange * ((desiredAngle + 10) / 40);
		float desiredServoInput = servoAngleToLinearInput(desiredAngle);
		/*if (desiredAngle = 0) {
			desiredServoInput = 60;
		}*/

		if (desiredServoInput < upPos) {
			desiredServoInput = upPos;
		}
		else if (desiredServoInput > downPos) {
			desiredServoInput = downPos;
		}
		servoA.write(desiredServoInput);
		servoB.write(desiredServoInput + servoBOffset);
	}
}

void initialiseServos() {
	upPos = comNet.configuration.GetServoUpLimit();
	downPos = comNet.configuration.GetServoDownLimit();
	servoBOffset = comNet.configuration.GetServoBOffset();
	servoA.write(upPos);
	servoB.write(upPos + servoBOffset);
	delay(200);

	int lastEncoderPosA = encoderA.read();
	int lastEncoderPosB = encoderB.read();
	int timeOut = 0;

	while (timeOut < 50)
	{
		int newPosA = encoderA.read();
		int newPosB = encoderB.read();
		int encoderDiffA = newPosA - lastEncoderPosA;
		int encoderDiffB = newPosB - lastEncoderPosB;
		if (encoderDiffA != 0 && encoderDiffB != 0) {
			timeOut = 0;
			digitalWrite(statusRed, HIGH);
			digitalWrite(statusGreen, LOW);
		}
		timeOut = timeOut + 1;
		lastEncoderPosA = newPosA;
		lastEncoderPosB = newPosB;
		delay(200);
		digitalWrite(statusRed, LOW);
		digitalWrite(statusGreen, HIGH);

		// DEBUG ONLY
		//Serial.print(encoderDiff);
		//Serial.print(newPos);
	}

	//thisServo.write(stopSpeed);
	encoderMinAngleA = encoderA.read();
	encoderMinAngleB = encoderB.read();
	lastEncoderPosA = encoderA.read();
	lastEncoderPosB = encoderB.read();
	timeOut = 0;
	delay(100);
	servoA.write(downPos);
	servoB.write(downPos + servoBOffset);
	delay(200);
	while (timeOut < 50)
	{
		int newPosA = encoderA.read();
		int newPosB = encoderB.read();
		int encoderDiffA = newPosA - lastEncoderPosA;
		int encoderDiffB = newPosB - lastEncoderPosB;
		if (encoderDiffA != 0 && encoderDiffB != 0) {
			timeOut = 0;
			digitalWrite(statusRed, HIGH);
			digitalWrite(statusGreen, LOW);
		}
		timeOut = timeOut + 1;
		lastEncoderPosA = newPosA;
		lastEncoderPosB = newPosB;
		delay(200);
		digitalWrite(statusRed, LOW);
		digitalWrite(statusGreen, HIGH);

		// DEBUG ONLY
		//Serial.print(encoderDiff);
		//Serial.print(newPos);
	}

	//thisServo.write(stopSpeed);
	encoderMaxAngleA = encoderA.read();
	encoderMaxAngleB = encoderB.read();
	servoAngleA = 30;
	servoAngleB = 30;
	servoAngle = 30;
	//thisEncoder.
}

// Convert rotary encoder angle to the angle of the flap
float rotaryToServoAngle(float rotaryAngle) {
	return (0.0152*pow(rotaryAngle, 2)) + 0.6095 *rotaryAngle - 10;
}

// Convert a desired flap angle to a linear servo input
float servoAngleToLinearInput(float desiredAngle) {
	float servoRange = downPos - upPos;
	return upPos + ((((-0.6095 + pow((pow(0.6095, 2) - 4 * 0.0152*(-10 - desiredAngle)), 0.5)) / (2 * 0.0152)) / 35)*servoRange);
}

//void encoderTest() {
//	float newPos;
//	float oldPos;
//	int countOut = 0;
//
//	while (countOut < 50) {
//		newPos = thisEncoder.read();
//		if (newPos != oldPos) {
//			Serial.print(newPos);
//			oldPos = newPos;
//		}
//	}	
//}
