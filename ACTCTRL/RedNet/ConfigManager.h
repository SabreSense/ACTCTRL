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
	//#include "EEPROM\src\EEPROM.h"
	//#include "avr\eeprom.h"
	#include "DataByteConverter.h"
	#include "EEPROM.h"
#else
	#include "WProgram.h"
#endif
class ConfigManager {
public:
	ConfigManager();
	void Init();

	// Publically accessible memory setting functions
	void SetControlID(int value);
	void SetNetSize(int value);
	void SetLowerLimit(int value);
	void SetUpperLimit(int value);
	void SetTakeOff(int value);
	void SetServoUpLimit(int value);
	void SetServoDownLimit(int value);
	void SetServoBOffset(int value);

	// These are the publically accessible versions of the variables
	int NetSize() { if (!loaded) { return GetNetSize(); } else { return netSize; } }
	int ControlID() { if (!loaded) { return GetControlID(); } else { return controlID; } }
	int LowerLimit() { if (!loaded) { return GetLowerLimit(); } else { return lowerLimit; } }
	int UpperLimit() { if (!loaded) { return GetUpperLimit(); } else { return upperLimit; } }
	int TakeOff() { if (!loaded) { return GetTakeOff(); } else { return takeOff; } }
	int ServoUpLimit() { if (!loaded) { return GetServoUpLimit(); } else { return servoUpLimit; } }
	int ServoDownLimit() { if (!loaded) { return GetServoDownLimit(); } else { return servoDownLimit; } }
	int ServoBOffset() { if (!loaded) { return GetServoBOffset(); } else { return servoBOffset; } }

	// Track current packet length
	int CrtPackLength() { if (!loaded) { return GetCrtPackLength(); } else { return 19; } }

	// These function calls reload the variables from memory
	int GetNetSize();
	int GetControlID();
	int GetLowerLimit();
	int GetUpperLimit();
	int GetTakeOff();
	int GetServoUpLimit();
	int GetServoDownLimit();
	int GetServoBOffset();
	int GetCrtPackLength();

	// Memory address declarations
	const int _controlIDMem = 0;
	const int _netSizeMem = 1;

	const int _lowerLimitMem = 10;
	const int _upperLimitMem = 20;
	const int _takeOffMem = 30;
	const int _servoUpLimitMem = 40;
	const int _servoDownLimitMem = 60;
	const int _servoBOffset = 70;
protected:
	// Declare size of control network i.e. number of in-wing arduinos, plus this controller's ID
	
private:
	// Bool to show if loaded
	bool loaded = false;

	// Declare standard packet length minus validation parts
	const int _stdPack = 19;

	int netSize;
	int controlID;
	int lowerLimit;
	int upperLimit;
	int takeOff;
	int crtPackLength;
	int servoUpLimit;
	int servoDownLimit;
	int servoBOffset;

	//float getFloatValue(int memAddress);

	DataByteConverter dataConverter;
	
	//void writeFloatValue(int memoryAddress ,float value);
};

#endif

