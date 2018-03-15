/*
 Name:		ConfigManager.cpp
 Created:	12/2/2017 6:01:31 PM
 Author:	AdamD
 Editor:	http://www.visualmicro.com
*/

#include "ConfigManager.h"

// CONFIG INTITIALISATION
// ----------------------

ConfigManager::ConfigManager() {
	Init();
}

void ConfigManager::Init() {
	controlID = GetControlID();
	netSize = GetNetSize();
	lowerLimit = GetLowerLimit();
	upperLimit = GetUpperLimit();
	takeOff = GetTakeOff();
	servoUpLimit = GetServoUpLimit();
	servoDownLimit = GetServoDownLimit();
	servoBOffset = GetServoBOffset();

	crtPackLength = _stdPack + 2 * netSize;

	loaded = true;
}

// MEMORY LOAD FUNCTIONS
// ---------------------

int ConfigManager::GetControlID() {
	controlID = EEPROM.read(_controlIDMem);
	return controlID;
}

int ConfigManager::GetNetSize() {
	netSize = EEPROM.read(_netSizeMem);
	return netSize;
}

int ConfigManager::GetLowerLimit() {
	//lowerLimit = getFloatValue(_lowerLimitMem);
	int thisR = 30;
	lowerLimit = EEPROM.get(_lowerLimitMem, thisR);
	return lowerLimit;
}

int ConfigManager::GetUpperLimit() {
	//upperLimit = getFloatValue(_upperLimitMem);
	int thisR = -10;
	upperLimit = EEPROM.get(_upperLimitMem, thisR);
	return upperLimit;
}

int ConfigManager::GetTakeOff() {
	//takeOff = getFloatValue(_takeOffMem);
	int thisR = 15;
	takeOff = EEPROM.get(_takeOffMem, thisR);
	return takeOff;
}

int ConfigManager::GetServoUpLimit() {
	int thisR = 50;
	servoUpLimit = EEPROM.get(_servoUpLimitMem, thisR);
	return servoUpLimit;
}

int ConfigManager::GetServoDownLimit() {
	int thisR = 110;
	servoDownLimit = EEPROM.get(_servoDownLimitMem, thisR);
	return servoDownLimit;
}

int ConfigManager::GetServoBOffset() {
	int thisR = 110;
	servoBOffset = EEPROM.get(_servoBOffsetMem, thisR);
	return servoBOffset;
}

bool ConfigManager::GetLCDBrightness() {
	bool thisR = true;
	lcdBrightness = EEPROM.get(_lcdBrightnessMem, thisR);
	return lcdBrightness;
}

int ConfigManager::GetCrtPackLength() {
	GetNetSize();
	crtPackLength = _stdPack + 2 * netSize;
	return crtPackLength;
}

int ConfigManager::GetControlMode() {
	int thisR = 0;
	controlMode = EEPROM.get(_controlModeMem, thisR);
	return controlMode;
}

// Generic function for returning a float value from EEPROM
//////float ConfigManager::getFloatValue(int memAddress) {
//////	int valueArray[7];
//////	for (int ii = 0; ii <= 6; ii++) {
//////		valueArray[ii] = EEPROM.read(memAddress + ii);
//////	}
//////	return dataConverter.MessageValue(valueArray);
//////}

// MEMORY STORAGE FUNCTIONS
// ------------------------

void ConfigManager::SetControlID(int value) {
	EEPROM.write(_controlIDMem, value);
	controlID = value;
}

void ConfigManager::SetNetSize(int value) {
	EEPROM.write(_netSizeMem, value);
	crtPackLength = _stdPack + 2 * value;
	netSize = value;
}

void ConfigManager::SetLowerLimit(int value) {
	//writeFloatValue(_lowerLimitMem, value);
	EEPROM.put(_lowerLimitMem, value);
	lowerLimit = value;
}

void ConfigManager::SetUpperLimit(int value) {
	//writeFloatValue(_upperLimitMem, value);
	EEPROM.put(_upperLimitMem, value);
	upperLimit = value;
}

void ConfigManager::SetTakeOff(int value) {
	//writeFloatValue(_takeOffMem, value);
	EEPROM.put(_takeOffMem, value);
	takeOff = value;
}

void ConfigManager::SetServoUpLimit(int value) {
	EEPROM.put(_servoUpLimitMem, value);
	servoUpLimit = value;
}

void ConfigManager::SetServoDownLimit(int value) {
	EEPROM.put(_servoDownLimitMem, value);
	servoDownLimit = value;
}

void ConfigManager::SetServoBOffset(int value) {
	EEPROM.put(_servoBOffsetMem, value);
	servoBOffset = value;
}

void ConfigManager::SetLCDBrightness(bool value) {
	EEPROM.put(_lcdBrightnessMem, value);
	lcdBrightness = value;
}

void ConfigManager::SetControlMode(int value) {
	EEPROM.put(_controlModeMem, value);
	controlMode = value;
}

// Write a float or int value to an array location
//void ConfigManager::writeFloatValue(int memoryAddress, float value) {
//	int valueArray[7];
//	dataConverter.FloatToMessageValue(value, valueArray);
//	for (int ii = 0; ii <= 6; ii++) {
//		EEPROM.write(memoryAddress + ii, valueArray[ii]);
//	}
//}

