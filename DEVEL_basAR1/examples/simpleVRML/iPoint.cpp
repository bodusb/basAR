#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include <AR/gsub_lite.h>
#include <AR/ar.h>
#include <GL/glut.h>
#include <AR/arvrml.h>

#include "iPoint.h"
#include "iVrml.h"
#include "Base.h"
#include "InfraStructure.h"
#include "Arpe.h"
#include "Rules.h"
#include "State.h"
#include "GenericItens.h"
//#include "ipObject.h"
#include "iObject3D.h"
#include "Ball.h"
#include "ipPosition.h"
#include "Serial.h"
#include "SerialCommand.h"
#include "ipDist.h"

#include <list>
using namespace std;

void callReadThread(void *data){ 
	iPoint *p = static_cast<iPoint *>(data);
	p->poolingThread(); }

void iPoint::addObject(ipObject* value){
	list<ipObject*>::iterator it;
	it = this->listObject.begin();
	this->listObject.push_back(value);
}

ipObject*  iPoint::findObject(int valueID){

	list<ipObject*>::iterator it;

	for( it = this->listObject.begin(); it != this->listObject.end(); it++)
		if((*(*it)).id == valueID)
			return (*it);
	
	//printf("\n *******Object not found");
	return 0;

}

void iPoint::showBall(){
	if( this->type == 1){
		double mat[3][4], m[16];

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		arUtilMatMul((*(*this->myBase).myInfraStructure).baseTrans, this->position.trans, mat);
		arglCameraViewRH(mat,m,1);
		glLoadMatrixd(m);
		//glTranslated(this->position.actualTra[0],this->position.actualTra[1],this->position.actualTra[2]);
		//printf("\n TRANS: (%3.2f,%3.2f,%3.2f)",this->position.actualTra[0],this->position.actualTra[1],this->position.actualTra[2]);
		
		switch( this->viewMode ){
		case 0:{	// 0 = HIDE			- hide point and object
			break;}
		case 1:{	// 1 = ONLY_BALL	- shows only point
			if( this->ball.holding != 0) {
				switch((*this->ball.holding).modelType){
				case 1: { // VRML
					iVrml* model = static_cast<iVrml*>(this->ball.holding);
					//	printf("\n Base name: %s Ipoint %d VRML TO PRINT: %d", (*this->myBase).name, this->id, (*model).vrmlID);
					(*model).draw();
					break;}
				case 2: { // Assimp
					break;}
				default: break; }
			} else {
				switch( this->myBase->myArpe->myGenericItens.holding->modelType){
				case 1: { // VRML
					iVrml* model = static_cast<iVrml*>(this->myBase->myArpe->myGenericItens.holding);
					(*model).draw();
					break;}
				case 2: { // Assimp
					break;}
				default: break;}

			}
			break;}
		case 2:{	// 2 = ONLY_OBJECT		- shows only enabled objects
			break;}
		case 3:{	// 3 = BOTH			- shows enabled objects and point
			if( this->ball.holding != 0) {
				switch((*this->ball.holding).modelType){
				case 1: { // VRML
					iVrml* model = static_cast<iVrml*>(this->ball.holding);
					//	printf("\n Base name: %s Ipoint %d VRML TO PRINT: %d", (*this->myBase).name, this->id, (*model).vrmlID);
					(*model).draw();
					break;}
				case 2: { // Assimp
					break;}
				default: break; }
			} else {
				switch( this->myBase->myArpe->myGenericItens.holding->modelType){
				case 1: { // VRML
					iVrml* model = static_cast<iVrml*>(this->myBase->myArpe->myGenericItens.holding);
					(*model).draw();
					break;}
				case 2: { // Assimp
					break;}
				default: break;}
			}
			break;}
		case 4:{	// 4 = FLASHING		- shows a flashing poing
			if( this->ball.flashingTime < 5){
				if( this->ball.holding != 0) {
					switch((*this->ball.holding).modelType){
					case 1: { // VRML
						iVrml* model = static_cast<iVrml*>(this->ball.holding);
						//	printf("\n Base name: %s Ipoint %d VRML TO PRINT: %d", (*this->myBase).name, this->id, (*model).vrmlID);
						(*model).draw();
						break;}
					case 2: { // Assimp
						break;}
					default: break; }
				} else {
					switch( this->myBase->myArpe->myGenericItens.holding->modelType){
					case 1: { // VRML
						iVrml* model = static_cast<iVrml*>(this->myBase->myArpe->myGenericItens.holding);
						(*model).draw();
						break;}
					case 2: { // Assimp
						break;}
					default: break;}
				}
			} else {
				if( this->ball.flashingTime > 10 ){ 
					this->ball.flashingTime = 0;
				}
			}
			this->ball.flashingTime++;




			break;}
		case 5:{	// 5 = GHOST		- show a ghost object 
			break;}
		case 6:{	// 6 = SENSE_PROX   - shows only point with sensing properties. (Only works if has the sensing points correcty)
			//printf("\n SENSE %d %d", this->ball.senseStatus, this->ball.activeBall);
			switch( this->ball.senseStatus){
			case 0:
			case 1:
				break;
			case 2:{	// HOLDING MODEL

				if( this->ball.holding != 0) {
					switch((*this->ball.holding).modelType){
					case 1: { // VRML
						iVrml* model = static_cast<iVrml*>(this->ball.holding);
						//	printf("\n Base name: %s Ipoint %d VRML TO PRINT: %d", (*this->myBase).name, this->id, (*model).vrmlID);
						(*model).draw();
						break;}
					case 2: { // Assimp
						break;}
					default: break; }
				} else {
					switch( this->myBase->myArpe->myGenericItens.holding->modelType){
					case 1: { // VRML
						iVrml* model = static_cast<iVrml*>(this->myBase->myArpe->myGenericItens.holding);
						(*model).draw();
						break;}
					case 2: { // Assimp
						break;}
					default: break;}
				}
				break;}
			case 3:{	// CAN WORK MODEL
				if( this->ball.canWork != 0) {
					switch((*this->ball.canWork).modelType){
					case 1: { // VRML
						iVrml* model = static_cast<iVrml*>(this->ball.canWork);
						//	printf("\n Base name: %s Ipoint %d VRML TO PRINT: %d", (*this->myBase).name, this->id, (*model).vrmlID);
						(*model).draw();
						break;}
					case 2: { // Assimp
						break;}
					default: break; }
				} else {
					switch( this->myBase->myArpe->myGenericItens.canwork->modelType){
					case 1: { // VRML
						iVrml* model = static_cast<iVrml*>(this->myBase->myArpe->myGenericItens.canwork);
						(*model).draw();
						break;}
					case 2: { // Assimp
						break;}
					default: break;}
				}
				break;}
			case 4:{	// CANNOT WORK MODEL
				if( this->ball.cannotWork != 0) {
					switch((*this->ball.cannotWork).modelType){
					case 1: { // VRML
						iVrml* model = static_cast<iVrml*>(this->ball.cannotWork);
						//	printf("\n Base name: %s Ipoint %d VRML TO PRINT: %d", (*this->myBase).name, this->id, (*model).vrmlID);
						(*model).draw();
						break;}
					case 2: { // Assimp
						break;}
					default: break; }
				} else {
					switch( this->myBase->myArpe->myGenericItens.cannotWork->modelType){
					case 1: { // VRML
						iVrml* model = static_cast<iVrml*>(this->myBase->myArpe->myGenericItens.cannotWork);
						(*model).draw();
						break;}
					case 2: { // Assimp
						break;}
					default: break;}
				}
				break;}
			default: break;}

			break;}
		case 7:{	// 7 = ALL_OBJECTS  - shows all objects at once	
			if( this->ball.holding != 0) {
				switch((*this->ball.holding).modelType){
				case 1: { // VRML
					iVrml* model = static_cast<iVrml*>(this->ball.holding);
					//	printf("\n Base name: %s Ipoint %d VRML TO PRINT: %d", (*this->myBase).name, this->id, (*model).vrmlID);
					(*model).draw();
					break;}
				case 2: { // Assimp
					break;}
				default: break; }
			} else {
				switch( this->myBase->myArpe->myGenericItens.holding->modelType){
				case 1: { // VRML
					iVrml* model = static_cast<iVrml*>(this->myBase->myArpe->myGenericItens.holding);
					(*model).draw();
					break;}
				case 2: { // Assimp
					break;}
				default: break;}
			}
			break;}

		default: break;
		};

		glPopMatrix();
	}
}

void iPoint::showObjects(){
	if( this->type){
		double mat[3][4], m[16];

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		arUtilMatMul((*(*this->myBase).myInfraStructure).baseTrans, this->position.trans, mat);
		arglCameraViewRH(mat,m,1);
		glLoadMatrixd(m);

		switch(this->viewMode ){
		case 0:{	// 0 = HIDE			- hide point and object
			break;}
		case 1:{	// 1 = ONLY_BALL	- shows only point
			break;}
		case 2:{	// 2 = ONLY_OBJECT		- shows only enabled objects

			if((this->activeObjectID > 0) && (this->listObject.size() > 0)){
				ipObject* obj = this->findObject(this->activeObjectID);
				switch((*obj).type){
				case 1: { // MODEL 3D
					iObject3D* i3D = static_cast<iObject3D*>(obj);
					switch((*i3D).modelType){
					case 1: { // VRML
						iVrml* model = static_cast<iVrml*>(i3D);
						(*model).draw();
						break;}
					case 2: { // Assimp
						break;}
					default: break;
					};
					break;}
				case 2: { // STATUS

					break;}
				default: break;
				};
			}

			break;}
		case 3:{	// 3 = BOTH			- shows enabled objects and point

			if( this->listObject.size() == 0) break;

			if((this->activeObjectID > 0) ){
				ipObject* obj = this->findObject(this->activeObjectID);
				switch((*obj).type){
				case 1: { // MODEL 3D
					iObject3D* i3D = static_cast<iObject3D*>(obj);
					switch((*i3D).modelType){
					case 1: { // VRML
						iVrml* model = static_cast<iVrml*>(i3D);
						(*model).draw();
						break;}
					case 2: { // Assimp
						break;}
					default: break;
					};
					break;}
				case 2: { // STATUS

					break;}
				default: break;
				};
			}

			break;}
		case 4:{	// 4 = FLASHING		- shows a flashing point
			break;}
		case 5:{	// 5 = GHOST		- show a ghost object 
			break;}
		case 6:{	// 6 = SENSE_PROX   - shows only point with sensing properties. (Only works if has the sensing points correcty)
			break;}
		case 7:{	// 7 = ALL_OBJECTS  - shows all objects at once	
			if( this->listObject.size() == 0) break;

			list<ipObject*>::iterator itO;
			for(itO = this->listObject.begin() ; itO != this->listObject.end() ; itO++){
				switch((*(*itO)).type){
				case 1: { // MODEL 3D
					iObject3D* i3D = static_cast<iObject3D*>((*itO));
					switch((*i3D).modelType){
					case 1: { // VRML
						iVrml* model = static_cast<iVrml*>(i3D);
						(*model).draw();
						break;}
					case 2: { // Assimp
						break;}
					default: break;
					};
					break;}
				case 2: { // STATUS

					break;}
				default: break;
				};
			}
			break;}

		default: break;
		};

		glPopMatrix();

	}
}

iPoint::iPoint(){
//	position = new ipPosition();
	this->activeObjectID	= 0;
	this->viewMode			= 0;
//	int			viewMode = 0;
	this->adaptType			= 0;
}

iPoint::~iPoint(){
	if( this->arduino != 0)
		if( this->arduino->isConnected())
			this->arduino->~Serial();
}

int iPoint::iPointReadFile(){
	
	FILE          *fp;
	char           buf[256],buf1[256],buf2[256],fileDAT[256];
	int			   numObjects;




	//--------------------------------------------------------------------------
	// Open the Object Configuration file
	//--------------------------------------------------------------------------
	printf("\n --------------------------------------------------------------------------");
	printf("\n Setting up Object file %s of %s", this->objFilename,this->name);
	printf("\n --------------------------------------------------------------------------");

	if( strcmp(this->objFilename,"") == 0) {
		printf("\n Any Object will be assigned to this action point"); 
		return  0;
	}

	if( (fp=fopen(this->objFilename,"r")) == NULL) { printf("\n Error on opening %s action point object file!! ",this->objFilename);	return -1;}

	//--------------------------------------------------------------------------
	// Read the amount of models
	//--------------------------------------------------------------------------
	getBuff(buf,256,fp);
    if (sscanf(buf, "%d", &numObjects) != 1) {
		 printf("\n Check %s file format", this->objFilename);fclose(fp); return -1;
	}
	printf("\n About to load %d objects.", numObjects);

	//Looping para ler todos os modelos do iPoint
    for (int i = 0; i < numObjects; i++) {
		//--------------------------------------------------------------------------
		// Read the objects
		//--------------------------------------------------------------------------
        getBuff(buf, 256, fp);
		if (sscanf(buf, "%s", &buf1) != 1) { printf("\n Check %s file format", this->objFilename); fclose(fp); return -1; }
		
		if ( strcmp(buf1,"MODEL3D") == 0 ){
			if (sscanf(buf, "%s %s %s", &buf1, &buf2, &fileDAT) != 3) { printf("\n Check %s file format", this->objFilename); fclose(fp); return -1; }
			if(  strcmp(buf2,"VRML") == 0 ){
				iVrml* obj = new iVrml();

				strcpy((*obj).filename,fileDAT);

				(*obj).id = i +1;
				(*obj).type = 1; // MODEL3D
				(*obj).modelType = 1;  //VRML
				(*obj).vrmlID = arVrmlLoadFile(fileDAT);
				if ((*obj).vrmlID < 0) {
					printf("\n Error on %s file or on VRML file (%d)",fileDAT,(*obj).vrmlID);
				} else {
					printf("\n Object %s VRML id: %d ", (*obj).filename, (*obj).vrmlID);
					(*obj).myiPoint = this;
					this->addObject(obj);
				}

			} else {
				printf("\n This type of object (%s-%s) is not coded on Matrix yet.",buf1,buf2);
			}
		} else { 
			printf("\n This type of object (%s) is not coded on Matrix yet.",buf1);
		}
	
	}

	this->activeObjectID = 1;
	printf("\n First Active Object ID: %d, of %d", (*this->findObject(1)).id, this->listObject.size());

	printf("\n %s correctly read!",this->name);
	fclose(fp);

	return 1;
}

int iPoint::externiPointReadFile(){
	
	FILE			*fp;
	char			buf[256],buf1[256],buf2[256];
	int				retScan;
	int				commandCounter = 1;
	int				readCmd = 0;
	int				br = 0;
	int				countReadInt = 0;

	//--------------------------------------------------------------------------
	// Open the Object Configuration file
	//--------------------------------------------------------------------------
	printf("\n --------------------------------------------------------------------------");
	printf("\n Setting up Object file %s of %s", this->objFilename,this->name);
	printf("\n Setting up ARDUINO ipoint configuration");
	printf("\n --------------------------------------------------------------------------");

	if( strcmp(this->objFilename,"") == 0) {
		printf("\n Any Object will be assigned to this action point"); 
		return  0;
	}

	if( (fp=fopen(this->objFilename,"r")) == NULL) { printf("\n Error on opening %s action point object file!! ",this->objFilename); return -1;}

	this->arduino = 0;

	//Serial *ard = 0;

	//--------------------------------------------------------------------------
	// Read the COM
	//--------------------------------------------------------------------------
	getBuff(buf, 256, fp);
	if (sscanf(buf, "%s %s", &buf1, &buf2) != 2) { printf("\n Check %s file format", this->objFilename); fclose(fp); return -1; }

	if( strcmp(buf1,"COM1") == 0) this->arduino = new Serial("\\\\.\\COM1");
	if( strcmp(buf1,"COM2") == 0) this->arduino = new Serial("\\\\.\\COM2");
	if( strcmp(buf1,"COM3") == 0) this->arduino = new Serial("\\\\.\\COM3");

	if( strcmp(buf1,"COM4") == 0) this->arduino = new Serial("\\\\.\\COM4");
	if( strcmp(buf1,"COM5") == 0) this->arduino = new Serial("\\\\.\\COM5");
	if( strcmp(buf1,"COM6") == 0) this->arduino = new Serial("\\\\.\\COM6");
	if( strcmp(buf1,"COM7") == 0) this->arduino = new Serial("\\\\.\\COM7");
	if( strcmp(buf1,"COM8") == 0) this->arduino = new Serial("\\\\.\\COM8");
	if( strcmp(buf1,"COM9") == 0) this->arduino = new Serial("\\\\.\\COM9");
	if( strcmp(buf1,"COM10") == 0) this->arduino = new Serial("\\\\.\\COM10");
	if( strcmp(buf1,"COM11") == 0) this->arduino = new Serial("\\\\.\\COM11");
	if( strcmp(buf1,"COM12") == 0) this->arduino = new Serial("\\\\.\\COM12");
	if( strcmp(buf1,"COM13") == 0) this->arduino = new Serial("\\\\.\\COM13");
	if( strcmp(buf1,"COM14") == 0) this->arduino = new Serial("\\\\.\\COM14");
	if( strcmp(buf1,"COM15") == 0) this->arduino = new Serial("\\\\.\\COM15");
	if( strcmp(buf1,"COM16") == 0) this->arduino = new Serial("\\\\.\\COM16");


	printf(" COM: %s . Max supported COM16.",buf1);

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

		this->arduino->addCommand(c);
		commandCounter++;


	}

	printf("\n Trying to connect to hardware");
	if( this->arduino == 0 || this->arduino->isConnected()){
		strcpy(this->arduino->configFilename,buf2);
		//	this->arduino = ard;

		if(this->arduino->sendFromLookupTable("aliveTest")){ // SEND aliveTest to check connection
			printf("\n ... Test byte correctly sent ");

			// test reading
			if ( this->arduino->sendFromLookupTable("aliveAnswer") ){ 
				////printf("\n ... Check Int");
				br = -1;
				Sleep(1000);
				while(br != 3){ br = this->arduino->readData(arduino->buffer,3); 
				if( countReadInt > 10 ) break;
				countReadInt++;
				}
				if( br == 3) {
					readCmd = atoi ( this->arduino->buffer);
					if ( readCmd == 2 ){
						printf("\n Alive read success");
					} else { 
						printf("\n Alive read failed");
					}
				} else {
					printf("\n Alive read %d bytes", br);
				}
				printf("....Received aliveAnswer");
			}
			else { printf("\n Failure to send hardware aliveAnswer!!"); }


			EnterCriticalSection(&this->commCS);
			this->arduino->enableReceive = 0;
			LeaveCriticalSection(&this->commCS);
			// START READING THREAD
			//_beginthread(callReadThread,0, this);



		}


	} else {
		this->arduino = 0;
		printf("\n Verify COM on configuration file");
	}

	printf("\n %s correctly read!",this->name);
	fclose(fp);

	return 1;
}

int iPoint::iPointWriteFile(){
	return 0;
}

char iPoint::getBuff(char *buf, int n, FILE *fp)
{
    char *ret;
	
    for(;;) {
        ret = fgets(buf, n, fp);
        if (ret == NULL) return(NULL);
        if (buf[0] != '\n' && buf[0] != '#') return(1); // Skip blank lines and comments.
    }
}

void iPoint::poolingThread(){
	Sleep(1000);
	
	// Split Int pooling and Rcv pooling
	int enableReceive;
	int br = -1;
	int counter = 0;
	int countReadInt = 0;
	int countReadRq = 0;
	int intNewState = 0;
	int readCmd = 0;
	SerialCommand *sc = 0;
	SerialCommand *scTest = 0;
	printf("\n Starting serial on COM %s",this->arduino->portName);
	while( TRUE ){
		Sleep(1000);
		//this->wait(1);
		countReadInt = 0;
		countReadRq = 0;
		strcpy(this->arduino->buffer,"0");

		printf(".x.");
		EnterCriticalSection(&this->commCS);
		if( this->arduino!=0 && this->arduino->isConnected()) enableReceive = this->arduino->enableReceive;
		LeaveCriticalSection(&this->commCS);
		
		// interrupt testing - intRequest
		// Interruption returns the number of the next state to be called
		if ( this->arduino->sendFromLookupTable("intRequest") ){ 
			//printf("\n ... Check Int");
			br = -1;
			while(br != 3){ br = this->arduino->readData(arduino->buffer,3); 
				if( countReadInt > 10 ) break;
				countReadInt++;
				}
			intNewState = atoi ( this->arduino->buffer);
			if ( intNewState > 0 && intNewState < this->myBase->myArpe->myRules->listState.size() ){
				printf ("NS: %d",intNewState);
				EnterCriticalSection(&this->myBase->myArpe->parserCS);
				this->myBase->myArpe->myRules->nextState = intNewState;
				LeaveCriticalSection(&this->myBase->myArpe->parserCS);
			} else { 
				if( intNewState != 0) printf("\n Asking by %d, it´s a not valid state. Sorry!", intNewState);
			}
		}
		else { printf("\n Failure to send hardware int request!!"); }


		// Test if ERCV POOLING - readRequest
		// Interruption returns the lookup code to match with code asked
		if(enableReceive == 1){
			if ( this->arduino->sendFromLookupTable("readRequest") ){ 
				printf("\n ... Check Read");
				br = -1;
				while(br != 3){ br = this->arduino->readData(arduino->buffer,3); 
				if( countReadRq > 10 ) break;
				countReadRq++;
				}
				// VERIFIES IF readRequestAnswer is correct
				readCmd = atoi ( this->arduino->buffer);
				sc = this->arduino->findCommand(readCmd);
				scTest = this->arduino->findCommand("readRequestAnswer");
				if (sc != 0 && sc->requestNumber == scTest->requestNumber){
					//HARDWARE IS WAITING COMMAND TO BE READ

					if ( this->arduino->sendFromLookupTable(this->actualAction->eMsg)){
						printf("\n ... Requested command has been sent!");
						br = -1;
						countReadRq = 0;
						while(br != 3){ br = this->arduino->readData(arduino->buffer,3); 
							if( countReadRq > 10 ) break;
							countReadRq++;
							}
						//VERIFIES IF command exists
						sc = 0;
						readCmd = atoi ( this->arduino->buffer);
						sc = this->arduino->findCommand(readCmd);	
						scTest = this->arduino->findCommand(this->actualAction->eMsg);
						if( sc != 0 && sc->requestNumber == scTest->requestNumber){
							//HARDWARE SENT THE ASKED COMMAND
							EnterCriticalSection(&this->myBase->myArpe->parserCS);
							this->myBase->myArpe->myRules->nextState = sc->nextState;
							LeaveCriticalSection(&this->myBase->myArpe->parserCS);				
							enableReceive = 0;
							this->myBase->myArpe->playActionAudio(this);
						} else { if( this->myBase->errorSound != 0) { this->myBase->errorSound->play2D();} }
					} else { if( this->myBase->errorSound != 0) { this->myBase->errorSound->play2D();} }
				} else { if( this->myBase->errorSound != 0) { this->myBase->errorSound->play2D();} }
			} else { printf("\n Failure to send read request!!"); if( this->myBase->errorSound != 0) { this->myBase->errorSound->play2D();} }
		}
	}
}

void iPoint::wait( double seconds){
	clock_t endwait;
	endwait = clock () + seconds*CLOCKS_PER_SEC;
	while (clock() < endwait) {}
}

void iPoint::createGLRotateMatrix(double angle, double x, double y, double z, double matrix[3][4]){

	double radAngle = angle*(3.14159265/180);

	//Normalize vector
	double invLen = sqrt( x*x + y*y + z*z );
	x = x*invLen;
	y = y*invLen;
	z = z*invLen;

	double c = cos(radAngle);
	double s = sin(radAngle);

	matrix[0][0] = (x*x)*(1-c)+c; matrix[0][1] = y*x*(1-c)-z*s; matrix[0][2] = x*z*(1-c)+y*s; 
	matrix[1][0] = x*y*(1-c)+z*s; matrix[1][1] = (y*y)*(1-c)+c; matrix[1][2] = y*z*(1-c)-x*s;
	matrix[2][0] = x*z*(1-c)-y*s; matrix[2][1] = y*z*(1-c)+x*s; matrix[2][2] = (z*z)*(1-c)+c;

	
	matrix[0][3] = matrix[1][3] = matrix [2][3] = 0;

}

void iPoint::addActuator(ipDist* value){
	list<ipDist*>::iterator it;
	it = this->listDistances.begin();
	this->listDistances.push_back(value);	
}

ipDist* iPoint::findActuator(int valueID){
	list<ipDist*>::iterator it;

	for( it = this->listDistances.begin(); it != this->listDistances.end(); it++)
		if( (*it)->actuator->id == valueID)
			return (*it);
	
	printf("\n *******Actuator not found");
	return 0;
}

double	iPoint::getDistanceTo(int valueID){
	list<ipDist*>::iterator it;

	for( it = this->listDistances.begin(); it != this->listDistances.end(); it++)
		if( (*it)->actuator->id == valueID)
			return (*it)->distance;
	
	printf("\n *******Actuator not found");
	return 0;
}