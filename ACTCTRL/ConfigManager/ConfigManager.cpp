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
	controlID = EEPROM.read(_controlIDMem);
	netSize = EEPROM.read(_netSizeMem);

	crtPackLength = _stdPack + 2 * netSize;
}

// CONFIG FUNCTIONS
// ----------------

void ConfigManager::setControlID(int value) {
	controlID = value;
	EEPROM.write(_controlIDMem, controlID);
}

void ConfigManager::setNetSize(int value) {
	netSize = value;
	EEPROM.write(_netSizeMem, netSize);
	crtPackLength = _stdPack + 2 * netSize;
}

