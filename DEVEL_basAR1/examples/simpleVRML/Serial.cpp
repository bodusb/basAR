#include "serial.h"

#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Serial.h"

Serial::Serial(){

	strcpy(this->buffer,"");
}

Serial::Serial(char *portName)
{
    //We're not yet connected
    this->connected = false;
	strcpy(this->buffer,"");
	strcpy(this->portName,portName);

    //Try to connect to the given port throuh CreateFile
    this->hSerial = CreateFile(portName,
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL);

    //Check if the connection was successfull
    if(this->hSerial==INVALID_HANDLE_VALUE)
    {
        //If not success full display an Error
        if(GetLastError()==ERROR_FILE_NOT_FOUND){

            //Print Error if neccessary
            printf("ERROR: Handle was not attached. Reason: %s not available.\n", portName);

        }
        else
        {
            printf("ERROR!!!");
        }
    }
    else
    {
        //If connected we try to set the comm parameters
        DCB dcbSerialParams = {0};

        //Try to get the current
        if (!GetCommState(this->hSerial, &dcbSerialParams))
        {
            //If impossible, show an error
            printf("failed to get current serial parameters!");
        }
        else
        {
            //Define serial connection parameters for the arduino board
            dcbSerialParams.BaudRate=CBR_9600;
            dcbSerialParams.ByteSize=8;
            dcbSerialParams.StopBits=ONESTOPBIT;
            dcbSerialParams.Parity=NOPARITY;

             //Set the parameters and check for their proper application
             if(!SetCommState(hSerial, &dcbSerialParams))
             {
                printf("ALERT: Could not set Serial Port parameters");
             }
             else
             {
                 //If everything went fine we're connected
                 this->connected = true;
                 //We wait 2s as the arduino board will be reseting
                 Sleep(ARDUINO_WAIT_TIME);
             }
        }
    }

}

Serial::~Serial()
{
    //Check if we are connected before trying to disconnect
    if(this->connected)
    {
        //We're no longer connected
        this->connected = false;
        //Close the serial handler
        CloseHandle(this->hSerial);
    }
}

int Serial::readData(char *buffer, unsigned int nbChar)
{
    //Number of bytes we'll have read
    DWORD bytesRead;
    //Number of bytes we'll really ask to read
    unsigned int toRead;

    //Use the ClearCommError function to get status info on the Serial port
    ClearCommError(this->hSerial, &this->errors, &this->status);

    //Check if there is something to read
    if(this->status.cbInQue>0)
    {
        //If there is we check if there is enough data to read the required number
        //of characters, if not we'll read only the available characters to prevent
        //locking of the application.
        if(this->status.cbInQue>nbChar)
        {
            toRead = nbChar;
        }
        else
        {
            toRead = this->status.cbInQue;
        }

        //Try to read the require number of chars, and return the number of read bytes on success
        if(ReadFile(this->hSerial, buffer, toRead, &bytesRead, NULL) && bytesRead != 0)
        {
            return bytesRead;
        }

    }

    //If nothing has been read, or that an error was detected return -1
    return -1;

}

bool Serial::writeData(char *buffer, unsigned int nbChar)
{
    DWORD bytesSend;

    //Try to write the buffer on the Serial port
    if(!WriteFile(this->hSerial, (void *)buffer, nbChar, &bytesSend, 0))
    {
        //In case it don't work get comm error and return false
        ClearCommError(this->hSerial, &this->errors, &this->status);

        return false;
    }
    else
        return true;
}

bool Serial::isConnected()
{
    //Simply return the connection status
    return this->connected;
}

char Serial::getBuff(char *buf, int n, FILE *fp)
{
    char *ret;
	
    for(;;) {
        ret = fgets(buf, n, fp);
        if (ret == NULL) return(NULL);
		if(feof(fp)) {printf("\n -------------------------------- EOF"); return (EOF);}
        if (buf[0] != '\n' && buf[0] != '#') return(1); // Skip blank lines and comments.
    }
}

int Serial::serialReadFile(){

	
	FILE			*fp;
	char			buf[256];
	int				retScan;
	int				commandCounter = 1;

	printf("\n --------------------------------------------------------------------------");
	printf("\n Setting up ARDUINO configuration");
	printf("\n --------------------------------------------------------------------------");
	//--------------------------------------------------------------------------
	// Open the ARPE Configuration file
	//--------------------------------------------------------------------------
	printf("\n --------------------------------------------------------------------------");
	if( (fp=fopen(this->configFilename,"r")) == NULL) {
		printf("\n Error on opening %s !! ",this->configFilename);
		return -1;
	}
	printf("\n Opening file %s ", this->configFilename);
	printf("\n --------------------------------------------------------------------------");

	//--------------------------------------------------------------------------
	// Reading serial lookup table
	//--------------------------------------------------------------------------

	while( getBuff(buf,256,fp) != NULL){

		SerialCommand* c = new SerialCommand();

		retScan = sscanf(buf, "%d %s %d", &c->requestNumber,&c->requestName,&c->nextState);
		c->id = commandCounter;
		switch(retScan){
		case 3:{
			printf("\n Command: %d - %s - %d", c->requestNumber, c->requestName, c->nextState);
			break;
			   }
		case 2:{
			printf("\n Command: %d - %s", c->requestNumber, c->requestName);
			c->nextState = 0;
			break;
			   }
		default:{
			fclose(fp);
			printf("\n Error on command %d", commandCounter);
			exit(0);
			break;}
		}

		this->addCommand(c);
		commandCounter++;


	}

	printf("\n %s correctly read! %d commands",this->configFilename, this->listCommand.size());
	fclose(fp);
	return 1;
}

int Serial::serialSetup(){

	//We're not yet connected
    this->connected = false;
	strcpy(this->buffer,"");

    //Try to connect to the given port throuh CreateFile
    this->hSerial = CreateFile(this->portName,
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL);

    //Check if the connection was successfull
    if(this->hSerial==INVALID_HANDLE_VALUE)
    {
        //If not success full display an Error
        if(GetLastError()==ERROR_FILE_NOT_FOUND){

            //Print Error if neccessary
            printf("ERROR: Handle was not attached. Reason: %s not available.\n", this->portName);

        }
        else
        {
            printf("ERROR!!!");
        }
    }
    else
    {
        //If connected we try to set the comm parameters
        DCB dcbSerialParams = {0};

        //Try to get the current
        if (!GetCommState(this->hSerial, &dcbSerialParams))
        {
            //If impossible, show an error
            printf("failed to get current serial parameters!");
        }
        else
        {
            //Define serial connection parameters for the arduino board
            dcbSerialParams.BaudRate=CBR_9600;
            dcbSerialParams.ByteSize=8;
            dcbSerialParams.StopBits=ONESTOPBIT;
            dcbSerialParams.Parity=NOPARITY;

             //Set the parameters and check for their proper application
             if(!SetCommState(hSerial, &dcbSerialParams))
             {
                printf("ALERT: Could not set Serial Port parameters");
             }
             else
             {
                 //If everything went fine we're connected
                 this->connected = true;
                 //We wait 2s as the arduino board will be reseting
                 Sleep(ARDUINO_WAIT_TIME);
             }
        }
    }

	return 1;
}

void Serial::addCommand(SerialCommand* value){
	list<SerialCommand*>::iterator it;
	it = listCommand.begin();
	listCommand.push_back(value);
}

SerialCommand* Serial::findCommand(char *valueMSG){
	//printf("\n Searching for: %s", valueMSG);
	list<SerialCommand*>::iterator it;
	
	for( it = this->listCommand.begin(); it != this->listCommand.end(); it++)
		if( strcmp((*(*it)).requestName,valueMSG) == 0){
	//		printf("\n found: %s", (*(*it)).requestName );
			return (*it);
		}
	
	printf("\n *******Command not found");
	return 0;
}

SerialCommand* Serial::findCommand(int valueMSG){
	//printf("\n Searching for: %s", valueMSG);
	list<SerialCommand*>::iterator it;
	
	for( it = this->listCommand.begin(); it != this->listCommand.end(); it++)
		if( (*it)->requestNumber == valueMSG){
	//		printf("\n found: %s", (*(*it)).requestName );
			return (*it);
		}
	
	printf("\n *******Command not found");
	return 0;
}

bool Serial::sendFromLookupTable(char *valueMSG){
	
	if ( this->isConnected()){
		char toSend[1];
		SerialCommand *s;
		s = 0;
		s = this->findCommand(valueMSG);
		
		if ( s != 0) {
			// Command exists
			sprintf(toSend,"%c",s->requestNumber);	// Transform int to char
			
			if(!this->writeData(toSend,1)){ 
				printf("\n Failure to send!!"); 
			} else {
				//Sent to hardware
				printf("\n -----------> %s", valueMSG);
				//printf("\n HEEEEEEEEEREEEEEEEEEEEEEEEEEE");
				return true;}
		} else {
			// Command doesn't exist
			printf("\n This command doesn't exist");
			return false;} 
	} else {
		// Couldn't connect to it.
		printf("\n Hardware isn't connected!");
		return false;}

	return false;
}


bool Serial::sendNormInt(int value){
	if ( value > 0 && value < 256 ) {

		if ( this->isConnected()){
			char toSend[1];
			SerialCommand *s;
			s = 0;

			if ( s != 0) {
				// Command exists
				sprintf(toSend,"%c",value);	// Transform int to char

				if(!this->writeData(toSend,1)){ 
					printf("\n Failure to send!!"); 
				} else {
					//Sent to hardware
					printf("\n -----------> %d", value);
					//printf("\n HEEEEEEEEEREEEEEEEEEEEEEEEEEE");
					return true;}
			} else {
				// Command doesn't exist
				printf("\n This command doesn't exist");
				return false;} 
		} else {
			// Couldn't connect to it.
			printf("\n Hardware isn't connected!");
			return false;}

	} else {
		printf("\n Value is out of range 0-255! ( value = %d )",value);
		return false;
	}


	return false;
}