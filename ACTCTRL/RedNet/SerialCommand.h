// SerialCommand.h

#ifndef _SERIALCOMMAND_h
#define _SERIALCOMMAND_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

class SerialCommand
{
 protected:


 public:
	SerialCommand(int type, float value);
	int Type;
	float Value;
	bool notEmpty = false;
};

//extern SerialCommand SerialCommand;

#endif

