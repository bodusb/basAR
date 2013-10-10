#ifndef Serial_h
#define Serial_h

#define ARDUINO_WAIT_TIME 2000

#include <list>
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "SerialCommand.h"

// http://arduino.cc/playground/Interfacing/CPPWindows

using namespace std;

class Serial
{
private:
	HANDLE hSerial;		// HANDLE hSerial
	bool connected;		// Connection status
	COMSTAT status;		// Connection information
	DWORD errors;		// Last Error

	char getBuff(char *buf, int n, FILE *fp);
public:
	int serialReadFile();
	char configFilename[256];
	char *bufferPulling;
	char buffer[3];
	int enableReceive; // 1 = canReceive ; 0 - cannotReceive
	char portName[256];

	void addCommand(SerialCommand* value);
	std::list< SerialCommand* > listCommand;
	SerialCommand* findCommand(char *valueMSG);
	SerialCommand* findCommand(int valueMSG);
	bool sendFromLookupTable(char *valueMSG); 
	bool sendNormInt(int value);

	Serial();
	Serial(char *portName); // Initialize serial with given COMM port
	int serialSetup();
	~Serial(); // close the connection
	//Read data in a buffer, if nbChar is greater than the
    //maximum number of bytes available, it will return only the
    //bytes available. The function return -1 when nothing could
    //be read, the number of bytes actually read.
	int readData(char *buffer, unsigned int nbChar);
    //Writes data from a buffer through the Serial connection
    //return true on success.
	bool writeData(char *buffer, unsigned int nbChar); 
    //Check if we are actually connected
    bool isConnected();
};


#endif // Serial_h