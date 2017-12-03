// 
// 
// 

#include "SerialCommand.h"

SerialCommand::SerialCommand(int type, float value) {
	Type = type;
	Value = value;
	if (Type != NULL) {
		notEmpty = true;
	}
}


//SerialCommandClass SerialCommand;

