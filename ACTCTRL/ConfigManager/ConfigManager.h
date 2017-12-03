/*
 Name:		ConfigManager.h
 Created:	12/2/2017 6:01:31 PM
 Author:	AdamD
 Editor:	http://www.visualmicro.com
*/

#ifndef _ConfigManager_h
#define _ConfigManager_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
	#include "EEPROM\src\EEPROM.h"
#else
	#include "WProgram.h"
#endif
class ConfigManager {
public:
	ConfigManager();
	void setControlID(int value);
	void setNetSize(int value);

	// Declare size of control network i.e. number of in-wing arduinos, plus this controller's ID
	int netSize;
	int controlID;

	// Track current packet length
	int crtPackLength;
private:
	// Declare standard packet length minus validation parts
	const int _stdPack = 17;
	
	// Memory address declarations
	const int _controlIDMem = 0;
	const int _netSizeMem = 1;
};

#endif

