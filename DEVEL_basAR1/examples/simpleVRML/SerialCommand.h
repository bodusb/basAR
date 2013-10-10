#ifndef SerialCommand_h
#define SerialCommand_h

#pragma once

class SerialCommand
{
public:
	SerialCommand(void);
	~SerialCommand(void);

	int id;

	int requestNumber;
	char requestName[256];
	int nextState;

};

#endif // SerialCommand_h