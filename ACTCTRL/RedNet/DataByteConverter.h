#ifndef _DataByteConverter_h
#define _DataByteConverter_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif
class DataByteConverter {
public:
	DataByteConverter();
	float MessageValue(int valueArray[7]);
	void FloatToMessageValue(float value, int responseBytes[7]);
private:

};

#endif