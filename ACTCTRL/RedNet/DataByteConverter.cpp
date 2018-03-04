#include "DataByteConverter.h"

DataByteConverter::DataByteConverter()
{

}

// Convert the message values into a useable float value
float DataByteConverter::MessageValue(int valueArray[7]) {
	float value = 0;
	value = value + valueArray[1] * 1000 + valueArray[2] * 100 + valueArray[3] * 10 + valueArray[4] + valueArray[5] * 0.1 + valueArray[6] * 0.01;
	if (valueArray[0] == 1) {
		value = -value;
	}
	return value;
}

// Convert a message float value to an array of digits for transmittion
void DataByteConverter::FloatToMessageValue(float value, int responseBytes[7]) {
	responseBytes[0] = 1;
	if (value >= 0) {
		responseBytes[0] = 0;
	}
	else {
		value = -value;
	}
	for (int jj = 1; jj <= 6; jj++) {
		for (int ii = 1; ii <= 10; ii++) {
			float test = ii*pow(10, 4 - jj) + responseBytes[1] * 1000 + responseBytes[2] * 100 + responseBytes[3] * 10 + responseBytes[4] + responseBytes[5] * 0.1 + responseBytes[6] * 0.01;
			if (test > value) {
				responseBytes[jj] = ii - 1;
				ii = 11;
			}
		}
	}
}