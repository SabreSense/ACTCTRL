/*
 Name:		SerialCom.cpp
 Created:	12/2/2017 5:55:32 PM
 Author:	AdamD
 Editor:	http://www.visualmicro.com
*/

#include "SerialCom.h"

// SETUP SERIAL CONNECTION
// -----------------------

//// Blank packet for re-initialisation
//byte blankPacket[30];
//// Serial buffer
//byte packetIn[30];
//// Counts pings for arbitrary data return
//int pingCount = 0;
//// Array of useful digits in an input data packet
//int packetInInt[30];
//// For useful info
//int valuePacket[7] = { 0,0,0,0,0,0,0 };
//int val = 0;
//
//// Declare packet for response
//byte messagePacket[30];
//bool sent = true;

SerialCom::SerialCom() {
	//upNetConfig(configuration);
	//_configuration = new ConfigManager();
	//HardwareSerial *thisSerial;
	//thisSerial = &Serial;
}

void SerialCom::Begin(HardwareSerial *desiredSerial, float flapAngle) {
	thisSerial = desiredSerial;
	thisSerial->begin(9600);
	configuration.Init();
	serialFlapPos = flapAngle;
}

void SerialCom::Begin() {
	//thisSerial = Serial;
	thisSerial->begin(9600);
	configuration.Init();
	serialFlapPos = 0;
	// DEBUGGING ONLY
	/*Serial.print("Control ID: ");
	Serial.println(configuration.controlID, DEC);
	Serial.print("Net Size: ");
	Serial.println(configuration.netSize, DEC);*/
}

// Establish contact with debugger
void SerialCom::establishContact() {
	digitalWrite(LED_BUILTIN, HIGH);
	while (thisSerial->available() <= 0) {
		thisSerial->print("A");
		delay(200);
	}
	digitalWrite(LED_BUILTIN, LOW);
	//Serial.print("A");
	//Serial.print("Contact established");
}

// Establish contact by waiting for an instruction and responding with an angle
void SerialCom::establishContact(float angle) {
	digitalWrite(LED_BUILTIN, HIGH);
	while (thisSerial->available() <= 0) {
		digitalWrite(LED_BUILTIN, HIGH);
		delay(100);
		digitalWrite(LED_BUILTIN, LOW);
		delay(100);
	}
	readSerialInput();
	sendSerialCommand(2, 1, angle);
	WriteSerialCommand();
	digitalWrite(LED_BUILTIN, LOW);
	//Serial.print("A");
	//Serial.print("Contact established");
}

// Establish contact by requesting ping
void SerialCom::establishContactPing() {
	digitalWrite(_statusRedLight, HIGH);
	digitalWrite(_statusGreenLight, LOW);
	
	bool contact = false;
	while (contact == false) {
		sendSerialCommand(1,0,0);
		// If command to be sent, send command
		WriteSerialCommand();
		// If serial input available, run serial input functions
		if (thisSerial->available() > 0) {
			readSerialInput();
			contact = true;
			break;
		}
		delay(200);
	}
	digitalWrite(_statusRedLight, LOW);
	digitalWrite(_statusGreenLight, HIGH);
	//Serial.print("A");
	//Serial.print("Contact established");
}

// SERIAL OUTPUT
// -------------

// Sends a serial command of the appropriate type
void SerialCom::sendSerialCommand(int packetType, int dataType, float value)
{
	//byte commandPacket[30];
	createOutputPacket(packetType, dataType, value, _messagePacket);
	_sent = false;
}

bool SerialCom::WriteSerialCommand() {
	// If command to be sent, send command
	if (thisSerial->available() <= 0 && _sent == false) {
		thisSerial->write(_messagePacket, configuration.CrtPackLength());
		_sent = true;
		//delay(500);
		//digitalWrite(motorLight, LOW);
		return true;
	}
	//else if (_sent == false) {
	//	//digitalWrite(motorLight, HIGH);
	//	return false;
	//}
	else {
		return false;
	}
}

// SERIAL INPUT AND COMMAND EXECUTION
// ----------------------------------

// Read and interpret the incoming packet
SerialCommand SerialCom::readSerialInput() {
	SerialCommand command(NULL, NULL);
	if (thisSerial->available() > 0) {
		thisSerial->readBytes(_packetIn, configuration.CrtPackLength());
		if (checkInitial(_packetIn) == true) {
			// For use with usb only
			for (int ii = 4; ii <= configuration.CrtPackLength() - 1; ii++) {
				_packetInInt[ii - 4] = int(_packetIn[ii]) - 48;
			}
			if (checkSumCheck(_packetInInt) == true) {
				switch (_packetInInt[0]) {
				case 0:
					executeConfig(_packetInInt);
					break;
				case 1:
					executeCommonCommands(_packetInInt);
					if (checkSenderValid(_packetInInt)) {
						command = returnCommandInfo(_packetInInt);
					}
					break;
				case 2:
					readInfo(_packetInInt);
					break;
				default:
					// DEBUG ONLY
					thisSerial->print("Invalid type");
					break;
				}
			}
			else {
				thisSerial->print("Checksum failed");
			}
		}
		else {
			//thisSerial->print("Leading check failed");		
		}
	}
	return command;
}

void SerialCom::executeCommonCommands(int packet[30]) {
	if (checkSenderValid(packet)) {
		switch (packet[1]) {
		case 0:
			_pingCount = _pingCount + 1;
			sendSerialCommand(2, 0, _pingCount);
			break;
		case 8:
			sendSerialCommand(2, 8, configuration.NetSize());
			break;
		case 9:
			sendSerialCommand(2, 9, configuration.ControlID());
			break;
		default:
			// DEBUG ONLY
			// thisSerial->print("No instruction");
			break;
		}
	}
}

SerialCommand SerialCom::returnCommandInfo(int packet[30]) {
	if (checkSenderValid(packet)) {
		bool done = getValueSection(_valuePacket, packet);
		_val = dataConverter.MessageValue(_valuePacket);
		SerialCommand command(packet[1], _val);
		return command;
	}
	else {
		SerialCommand command(NULL, NULL);
		return command;
	}
}

void SerialCom::executeConfig(int packet[30]) {
	if (checkSenderValid(packet)) {
		thisSerial->print("Configging");
		switch (packet[1]) {
		case 0:
		{
			//thisSerial->print("ControlID");
			bool done = getValueSection(_valuePacket, packet);
			_val = dataConverter.MessageValue(_valuePacket);
			//thisSerial->print(val);
			if (_val <= configuration.NetSize() && _val > 0) {
				configuration.SetControlID(_val);
				//createOutputPacket(2, 9, controlID, responsePacket);
				sendSerialCommand(2, 9, configuration.ControlID());
				//DEBUG ONLY
				//thisSerial->write(" Response: ");
				//thisSerial->write(responsePacket, crtPackLength);
			}
		}
		break;
		case 1:
		{
			bool done = getValueSection(_valuePacket, packet);
			_val = dataConverter.MessageValue(_valuePacket);
			if (_val < 10 && _val >= 1) {
				configuration.SetNetSize(_val);
				//createOutputPacket(2, 8, netSize, responsePacket);
				sendSerialCommand(2, 8, configuration.NetSize());
				//DEBUG ONLY
				//thisSerial->write(" Response: ");
				//thisSerial->write(responsePacket, crtPackLength);
			}
		}
		break;
		default:
			// FOR DEBUGGING
			thisSerial->print("No config");
			break;
		}
	}
}

void SerialCom::readInfo(int packet[30]) {
	if (checkSenderValid(packet)) {
		byte responsePacket[30];
		switch (packet[1]) {
		case 1:
		{
			bool done = getValueSection(_valuePacket, packet);
			serialFlapPos = dataConverter.MessageValue(_valuePacket);
		}
		break;
		case 2:
		{
			bool done = getValueSection(_valuePacket, packet);
			serialFlapPos = dataConverter.MessageValue(_valuePacket);
			stopped = true;
		}
		break;
		default:
			//FOR DEBUGGING
			//thisSerial->print("No info");
			break;
		}
	}
}

// SERIAL INPUT INTERPRETATION
// ---------------------------

// Check that the packet sender is valid
bool SerialCom::checkSenderValid(int packet[30]) {
	bool valid = false;
	switch (packet[0]) {
	case 0:
		if (int(packet[9]) == 2 && int(packet[10]) == 1) {
			valid = true;
		}
		break;
	case 1:
		// FOR DEBUGGING
		//thisSerial->print("Case 1 Control ID = ");
		//thisSerial->print(configuration.controlID, DEC);
		if (configuration.ControlID() == 0) {
			if (int(packet[9]) == 2 && int(packet[10]) == 1) {
				valid = true;
			}
		}
		else {
			if (int(packet[9]) == 2 && int(packet[10]) == 1 && packet[9 + 2 * configuration.ControlID()] == 0 && packet[10 + 2 * configuration.ControlID()] == 0) {
				valid = true;
			}
		}
		break;
	case 2:
		if (configuration.ControlID() == 0) {
			if (int(packet[11]) == 2 && int(packet[12]) == 1) {
				valid = true;
			}
		}
		break;
	default:
		valid = false;
		break;
	}
	// FOR DEBUGGING
	/*if (valid) {
		thisSerial->print(" Valid");
	}
	else {
		thisSerial->print(" Invalid");
	}*/
	return valid;
}

// Check that the packet starts with the appropriate string
bool SerialCom::checkInitial(byte packet[30]) {
	if (packet[0] == 'A' && packet[1] == 'B' && packet[2] == 'C' && packet[3] == 'D') {
		return true;
	}
	else {
		return false;
	}
}

// Checks that the checksum value matches the value delivered
bool SerialCom::checkSumCheck(int packet[30]) {
	//if (isAlphaNumeric(packet[18]) == true && isAlphaNumeric(packet[17]) == true && isAlpha(packet[18]) == false && isAlpha(packet[17]) == false){
	//if (isAlphaNumeric(packet[18]) == true && isAlphaNumeric(packet[17]) == true){
	int chkValue = (packet[configuration.CrtPackLength() - 6] * 10) + packet[configuration.CrtPackLength() - 5];
	int streamValue = calcChkSum(packet);
	if (chkValue == streamValue) {
		return true;
	}
	else {
		//FOR DEBUGGING
		thisSerial->write("Stream val: ");
		thisSerial->print(streamValue, DEC);
		thisSerial->write(" chkValue: ");
		thisSerial->print(chkValue, DEC);
		return false;
	}
	//  }
	//  else {
	//    thisSerial->print("Alphanumeric problem");
	//    return false;
	//  }
}

// Calculate checksum value
int SerialCom::calcChkSum(int packet[30]) {
	int streamValue = 0;
	//thisSerial->write(" CHKSUM LENGTH: ");
	//thisSerial->print(crtPackLength - 7, DEC);
	for (int ii = 0; ii <= configuration.CrtPackLength() - 7; ii++) {
		streamValue = streamValue + packet[ii];
	}
	return streamValue;
}

// Gets the value section of the message packet
bool SerialCom::getValueSection(int returnArray[7], int inputPacket[30]) {
	for (int ii = 2; ii <= 8; ii++) {
		returnArray[ii - 2] = inputPacket[ii];
	}
	return true;
}

// SERIAL RESPONSE
// ---------------

// Convert the checksum number into induvidual digits for transmittion
void SerialCom::getChkBytes(int chkSum, int values[2]) {
	//DEBUG ONLY
	//  thisSerial->print(chkSum, DEC);
	for (int jj = 1; jj <= 2; jj++) {
		for (int ii = 1; ii <= 10; ii++) {
			int test = ii*pow(10, 2 - jj) + (values[0] * 10) + values[1];
			// DEBUG ONLY
			//      thisSerial->print(test, DEC);
			//      thisSerial->write(",");
			if (test > chkSum) {
				values[jj - 1] = ii - 1;
				//DEBUG ONLY        
				//        thisSerial->print(values[jj-1], DEC);
				//        thisSerial->write(",,");
				break;
			}
		}
	}
}

// Generate the output packet for transmittion
void SerialCom::createOutputPacket(int type, int dataType, float value, byte responsePacket[30]) {
	int valueArray[] = { 0,0,0,0,0,0,0 };
	dataConverter.FloatToMessageValue(value, valueArray);
	int holdResponsePacket[30];
	//= {type, dataType, valueArray[0], valueArray[1], valueArray[2], valueArray[3], valueArray[4], valueArray[5], valueArray[6], 0, 0, 2, 1, 0, 0};
	holdResponsePacket[0] = type;
	holdResponsePacket[1] = dataType;
	for (int iii = 2; iii <= 8; iii++) {
		holdResponsePacket[iii] = valueArray[iii - 2];
	}
	for (int iii = 9; iii <= configuration.CrtPackLength() - 3; iii++) {
		holdResponsePacket[iii] = 0;
	}
	holdResponsePacket[9 + 2 * configuration.ControlID()] = 2;
	holdResponsePacket[10 + 2 * configuration.ControlID()] = 1;
	int chkSum = calcChkSum(holdResponsePacket);
	int values[2] = { 0,0 };
	getChkBytes(chkSum, values);
	holdResponsePacket[configuration.CrtPackLength() - 6] = values[0];
	holdResponsePacket[configuration.CrtPackLength() - 5] = values[1];

	//DEBUG ONLY
	//thisSerial->write("chk num: ");
	//thisSerial->print(chkSum, DEC);
	//thisSerial->write("chk val: ");
	//thisSerial->print(values[0], DEC);
	//thisSerial->print(values[1], DEC);

	for (int ii = 4; ii <= configuration.CrtPackLength() - 1; ii++) {
		responsePacket[ii] = holdResponsePacket[ii - 4] + 48;
	}
	responsePacket[0] = 'A';
	responsePacket[1] = 'B';
	responsePacket[2] = 'C';
	responsePacket[3] = 'D';
}

// Convert a message float value to an array of digits for transmittion
//void SerialCom::floatToMessageValue(float value, int responseBytes[7]) {
//	responseBytes[0] = 1;
//	if (value >= 0) {
//		responseBytes[0] = 0;
//	}
//	else {
//		value = -value;
//	}
//	for (int jj = 1; jj <= 6; jj++) {
//		for (int ii = 1; ii <= 10; ii++) {
//			float test = ii*pow(10, 4 - jj) + responseBytes[1] * 1000 + responseBytes[2] * 100 + responseBytes[3] * 10 + responseBytes[4] + responseBytes[5] * 0.1 + responseBytes[6] * 0.01;
//			if (test > value) {
//				responseBytes[jj] = ii - 1;
//				ii = 11;
//			}
//		}
//	}
//}
