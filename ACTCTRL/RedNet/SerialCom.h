/*
 Name:		SerialCom.h
 Created:	12/2/2017 5:55:32 PM
 Author:	AdamD
 Editor:	http://www.visualmicro.com
*/

#ifndef _SerialCom_h
#define _SerialCom_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
	#include "ConfigManager.h"
	#include "SerialCommand.h"
#else
	#include "WProgram.h"
#endif

class SerialCom {
public:
	SerialCom();
	void Begin();

	void establishContact();
	void establishContactPing();
	void establishContact(float angle);
	void sendSerialCommand(int packetType, int dataType, float value);
	bool WriteSerialCommand();
	SerialCommand readSerialInput();

	// Keeps track of current network configuration
	ConfigManager configuration;

	float serialFlapPos;

private:
	// Declare packet for response
	byte _messagePacket[30];
	// Keep track of whether message sent
	bool _sent = false;

	// Counts pings for arbitrary data return
	int _pingCount = 0;

	// Blank packet for re-initialisation
	byte _blankPacket[30];
	// Serial buffer
	byte _packetIn[30];
	// Array of useful digits in an input data packet

	// Storing input packets
	int _packetInInt[30];
	// Variables for storing useful info
	int _valuePacket[7] = { 0,0,0,0,0,0,0 };
	int _val = 0;

	const int _statusRedLight = 12;
	const int _statusGreenLight = 13;

	// Various private functions
	void executeCommonCommands(int packet[30]);
	SerialCommand returnCommandInfo(int packet[30]);
	void executeConfig(int packet[30]);
	void readInfo(int packet[30]);
	bool checkSenderValid(int packet[30]);
	bool checkInitial(byte packet[30]);
	bool checkSumCheck(int packet[30]);
	int calcChkSum(int packet[30]);
	bool getValueSection(int returnArray[7], int inputPacket[30]);
	float messageValue(int valueArray[7]);
	void getChkBytes(int chkSum, int values[2]);
	void createOutputPacket(int type, int dataType, float value, byte responsePacket[30]);
	void floatToMessageValue(float value, int responseBytes[7]);
};

#endif

