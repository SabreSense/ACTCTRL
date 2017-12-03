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
	controlID = EEPROM.read(_controlIDMem);
	netSize = EEPROM.read(_netSizeMem);

	crtPackLength = _stdPack + 2 * netSize;
}

// CONFIG FUNCTIONS
// ----------------

int ConfigManager::ControlID() {
	controlID = EEPROM.read(_controlIDMem);
	return controlID;
}

int ConfigManager::NetSize() {
	netSize = EEPROM.read(_netSizeMem);
	return netSize;
}

void ConfigManager::setControlID(int value) {
	EEPROM.write(_controlIDMem, value);
}

void ConfigManager::setNetSize(int value) {
	EEPROM.write(_netSizeMem, value);
	crtPackLength = _stdPack + 2 * value;
}

