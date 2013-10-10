#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <process.h>
#include <time.h>

#include <AR/gsub_lite.h>
#include <AR/ar.h>

#include "Rules.h"
#include "State.h"
#include "Action.h"
#include "Arpe.h"
#include "iPoint.h"
#include "InfraStructure.h"
#include "Serial.h"
#include "SerialCommand.h"

#include <GL/glut.h>

#include <list>
using namespace std;

void callTraThread(void *data){ 
	//	iPoint *p = static_cast<iPoint *>(data);
	//	p->animTraThread(p->actualAction); 

	Action *a = static_cast<Action *>(data);
	iPoint *p = a->myState->myRules->myArpe->findIPoint(a->ipointID);

	printf("\n animTraThread: %s", p->name);

	// Critical Section ->lock parser of this point
	double tstep;
	int nstep;
	double x,y,z;

	nstep = fabs(a->step);
	tstep = (fabs(a->time))/nstep;

	x = a->x/nstep;
	y = a->y/nstep;
	z = a->z/nstep;

	for(int n = 0; n < nstep; n++){
		//printf("t");
		p->position.trans[0][3] += x;
		p->position.trans[1][3] += y;
		p->position.trans[2][3] += z;

		p->wait(tstep);
	}
}

void callRotThread(void *data){ 
	//	iPoint *p = static_cast<iPoint *>(data);
	//	p->animRotThread(p->actualAction); 

	Action *a = static_cast<Action *>(data);
	iPoint *p = a->myState->myRules->myArpe->findIPoint(a->ipointID);

	printf("\n animRotThread");

	double tstep;
	int nstep;
	int     i, j;
	double temp[3][4];
	double rotMatX[3][4];
	double rotMatY[3][4];
	double rotMatZ[3][4];
	double x,y,z;

	nstep = fabs(a->step);
	tstep = (fabs(a->time))/nstep;

	x = a->x/nstep;
	y = a->y/nstep;
	z = a->z/nstep;

	p->createGLRotateMatrix(x,1.0,0.0,0.0,rotMatX);
	p->createGLRotateMatrix(y,0.0,1.0,0.0,rotMatY);
	p->createGLRotateMatrix(z,0.0,0.0,1.0,rotMatZ);

	for(int n = 0; n < nstep; n++){
		//printf("r");
		if(x != 0.0){
			for( j = 0; j < 3; j++ ) 
				for( i = 0; i < 4; i++ ) 
					temp[j][i] = p->position.trans[j][i];
			arUtilMatMul(temp,rotMatX,p->position.trans);
		}
		if(y != 0.0){

			for( j = 0; j < 3; j++ ) 
				for( i = 0; i < 4; i++ ) 
					temp[j][i] = p->position.trans[j][i];
			arUtilMatMul(temp,rotMatY,p->position.trans);
		}
		if(z != 0.0){

			for( j = 0; j < 3; j++ ) 
				for( i = 0; i < 4; i++ ) 
					temp[j][i] = p->position.trans[j][i];
			arUtilMatMul(temp,rotMatZ,p->position.trans);
		}

		p->wait(tstep);

	}

}

void callSclThread(void *data){ 
//	iPoint *p = static_cast<iPoint *>(data);
//	p->animSclThread(p->actualAction);

	Action *a = static_cast<Action *>(data);
	iPoint *p = a->myState->myRules->myArpe->findIPoint(a->ipointID);

	
	int     i, j;
	double tstep;
	int nstep;
	double temp[3][4],sclMat[3][4];
	nstep = fabs(a->step);
	tstep = fabs((a->time))/nstep;

	for( j = 0; j < 3; j++ ) 
		for( i = 0; i < 4; i++ ){ 
			sclMat[j][i] = 0;
			temp[j][i] = p->position.trans[j][i];
		}
 	sclMat[0][0] = pow(fabs(a->x),1./nstep);
	sclMat[1][1] = pow(fabs(a->y),1./nstep);
	sclMat[2][2] = pow(fabs(a->z),1./nstep);

	for(int n = 0; n < nstep; n++){
		for( j = 0; j < 3; j++ ) 
			for( i = 0; i < 4; i++ ){ 
				temp[j][i] =p->position.trans[j][i];
			}
		arUtilMatMul(temp,sclMat,p->position.trans);
		p->wait(tstep);

	}

}

void Rules::addState(State* value){
	list<State*>::iterator it;
	it = listState.begin();
	listState.push_back(value);
}

State* Rules::findState(int valueID){

	list<State*>::iterator it;
	//printf("\n Size of List State %d, asked state %d", this->listState.size(), valueID);
	for( it = this->listState.begin(); it != this->listState.end(); it++)
		if((*(*it)).id == valueID)
			return (*it);
	return 0;

}

Action* Rules::findAction(int valueID){
	list<State*>::iterator it;
	list<Action*>::iterator itA;

	for( it = this->listState.begin(); it != this->listState.end(); it++)
		for( itA = (*(*it)).listAction.begin(); itA != (*(*it)).listAction.end(); itA++)
			if( (*(*itA)).id == valueID)
				return (*itA);
	
	printf("\n *******Object not found");
	return 0;

}

Rules::Rules(){

	this->lastState = 0;
	this->actualState = 0;
	this->nextState = 1;
	this->nextStateMath = -1;
	this->lockParser = 0;

	getFromQueue = false;
	queueIndex = 1;
}

int Rules::rulesReadFile()
{
	FILE			*fp;
	char			buf[256];
	int				numPoints, numStates, pID;
	double			nextState,time;
	int				retScan, readingState, tempID;
	char			tempOP[256],tempStr1[256],tempStr2[256],tempStr3[256];
	int				q[20];

	int qCount = 1;
	int qCountPerLine = 0;
	int qLineCounter = 1;


	printf("\n --------------------------------------------------------------------------");
	printf("\n Reading Behaviour configuration file");
	printf("\n --------------------------------------------------------------------------");
	//--------------------------------------------------------------------------
	// Open the Behavior Configuration file
	//--------------------------------------------------------------------------
	printf("\n --------------------------------------------------------------------------");
	if( (fp=fopen(this->configFilename,"r")) == NULL) {
		printf("\n Error on opening %s !! ",this->configFilename);
		exit(0);
	}
	printf("\n Opening file %s ", this->configFilename);
	printf("\n --------------------------------------------------------------------------");

	//--------------------------------------------------------------------------
	// Read file
	//--------------------------------------------------------------------------	
	numPoints = 1;
	numStates = 0;
	State* s = new State();
	(*s).onUse = 0;
	readingState = 0;
	while(getBuff(buf,256,fp) != NULL){
		
		//--------------------------------------------------------------------------
		// BEGIN_STATE 
		//--------------------------------------------------------------------------	
		retScan = sscanf(buf,"%s %d ",&tempStr1, &tempID);
		if((strcmp(tempStr1,"BEGIN_STATE") == 0) && ((*s).onUse == 0)){
			numStates++;
			s = new State();
			(*s).listAction.clear();
			readingState = 1;
			(*s).id = tempID;
			(*s).time = 0;

			printf("\n BEGIN_STATE: %d", (*s).id);
			//numPoints = 1;
		}
		//--------------------------------------------------------------------------
		// DEAL WITH AN INTERNAL ACTION
		//--------------------------------------------------------------------------
		retScan = sscanf(buf,"%d %s", &pID, &tempOP);

		if((retScan == 2) && (readingState == 1)){
			Action* a = new Action();
			(*a).cleanAction(numPoints);
			(*a).fillAction(buf);
			printf("\n ---%d I:%d OP:%s(%d) PM:%d NS:%d", (*a).id, (*a).ipointID, tempOP,(*a).opcode, (*a).pointMode, (*a).nextState );
			(*a).myState = s;
			(*s).addAction(a);

			numPoints++;
		}


		//--------------------------------------------------------------------------
		// END_STATE 
		//--------------------------------------------------------------------------
		// END_STATE
		// END_STATE GO_TO N
		// END_STATE AFTER K
		// END_STATE GO_TO N AFTER K
		retScan = sscanf(buf, "%s %s %lf %s %lf", &tempStr1, &tempStr2, &nextState, &tempStr3, &time);
		if (strcmp(tempStr1,"END_STATE") == 0) {
			//printf("\n END_STATE (%d)",retScan);
	/*		if(strcmp(tempStr2,"GO_TO") == 0){*/

				switch(retScan){
				case 3:{ // CORRECT READING NEXT STATE

					if(strcmp(tempStr2,"GO_TO") == 0 ) {
						(*s).time = 0;
						(*s).nextState = (int)nextState;
						printf("\n END_STATE %d GO_TO %d",(*s).id, (*s).nextState);
						break;}
					if(strcmp(tempStr2,"AFTER") == 0) {
						sscanf(buf, "%s %s %lf", &tempStr1, &tempStr2, &time);
						(*s).time = time;
						(*s).nextState = -1;
						printf("\n END_STATE %d AFTER %lf",(*s).id, (*s).time);
						break;}
					//if(strcmp(tempStr2,"AFTER") == 0 ) {
					//	retScan = sscanf(buf, "%s %s %lf", &tempStr1, &tempStr2, &time);
					//	(*s).time = time;
					//	(*s).nextState = 0;
					//	printf("\n END_STATE %d GO_TO %d",(*s).id, (*s).nextState);
					//	break;}
					break;}
				case 5:{
						if(strcmp(tempStr2,"GO_TO") == 0 ) {
							(*s).nextState = (int)nextState;
							printf("\n END_STATE %d GO_TO %d",(*s).id, (*s).nextState);}
						if(strcmp(tempStr3,"AFTER") == 0 ) {
							(*s).time = time;
							printf(" AFTER %3.2f", (*s).time);}					
						break;}
				default:{
					(*s).nextState = -1;
					(*s).time = 0;
					printf("\n END_STATE %d (NS: %d) ",(*s).id, (*s).nextState);
					break;}
				};

		//	} else {
		//			(*s).nextState = 0;
		//			(*s).time = 0;
		////			printf("\n END_STATE %d (NS: %d)  ",(*s).id, (*s).nextState);
		//	}

			//Check if temp state is correctly set than add
			(*s).onUse = 0;
			(*s).myRules = this;

			State* realState = new State();
			realState = s;
			this->addState(realState);

			readingState = 0;
		}

		retScan = sscanf(buf,"%s %d ",&tempStr1, &tempID);
		if(strcmp(tempStr1,"BEGIN_QUEUE") == 0){
			
			//--------------------------------------------------------------------------
			// READING QUEUE
			//--------------------------------------------------------------------------
			printf("\n BEGIN_QUEUE (20 itens per line)");
			// Get the queue size
			while(getBuff(buf,256,fp) != NULL){
				retScan = sscanf(buf,"%s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",&tempStr1,&q[0],&q[1],&q[2],&q[3],&q[4],&q[5],&q[6],&q[7],&q[8],&q[9],&q[10],&q[11],&q[12],&q[13],&q[14],&q[15],&q[16],&q[17],&q[18],&q[19]); 
				if( strcmp(tempStr1,"QUEUE_LINE")==0){
					//Create state queue
					printf("\n---QUEUE_LINE");
					for(int i = 0; i<(retScan - 1);i++){
						
						State *auxS = new State();
						auxS = 0;
						// Verify if the state is valid
						auxS = this->findState(q[i]);
						printf(".");
						//printf("\n State ID: %d , state asked: %d", auxS, q[i]);
						if( auxS == 0 ){
							// State isn't valid
							printf("\n State %d doesn't exist, please check the queue line %d!!!", q[i], qLineCounter);
							exit(0);
						} else {
							// State is valid
							queueState *qs = new queueState;
							qs->s = auxS;
							qs->id = qCount;
							qCount++;
							qCountPerLine++;
							//printf(".%d.",qCountPerLine);
							this->addQS(qs);
						}

					}

					printf(" - itens read at line %d: %d",qLineCounter,qCountPerLine);
					qCountPerLine = 0;
					qLineCounter++;

				} else {
					if (strcmp(tempStr1,"END_QUEUE") == 0){
						printf("\n END_QUEUE - Queued itens: %d",this->qS.size());
						break;
					}
				}

			}


			break;
		}

	}

	list<State *>::iterator iS;

	printf("\n %d States loaded.", this->listState.size());
	for(iS = this->listState.begin(); iS != this->listState.end(); iS++){
		printf("\n State %d has %d Actions loaded.", (*(*iS)).id, (*(*iS)).listAction.size());
	}

	printf("\n All Rules loaded.");






	//printf("\n Queue itens: %d",this->qS.size());
	fclose(fp);
	return 1;
}

int Rules::rulesWriteFile()
{
	return 0;
}

int Rules::verifyConsistency(){

	// loop state

	// each state verify the commands
	// test if point waited is correct

	// test if next state exist

	list<State*>::iterator s;
	list<Action*>::iterator a;

	for( s = this->listState.begin(); s != this->listState.end(); s++){ 

		if( (*(*s)).id == 1){ // First state must setup all points ===== npoints = nactions
		}


		for( a = (*s)->listAction.begin(); a != (*s)->listAction.end(); a++){

			switch ( (*a)->opcode){
			case 4: // verifify point waited and next state
			case 5: //
			case 7:
			case 9:{

				// Check if point waited is correct
				if ( this->myArpe->findIPoint( (*a)->pointWaited) == 0  ){
					printf("\n State: %d, Action:%d - pointWaited field (%d) is out of range!", (*s)->id, (*a)->id, (*a)->pointWaited);
				}


				// Check next State
				if( ((*a)->nextState <= 0) || ( this->listState.size() < (*a)->nextState ) || (this->qS.empty() == true) ){
					printf("\n State: %d, Action:%d - NextState field (%d) is out of range!", (*s)->id, (*a)->id, (*a)->nextState);
				}


				break;}
			case 6: // verify next state
			case 8:
			case 10:
			case 11:{
				// Check next State
				if( ((*a)->nextState <= 0) || ( this->listState.size() < (*a)->nextState ) ){
					printf("\n State: %d, Action:%d - NextState field (%d) is out of range!", (*s)->id, (*a)->id, (*a)->nextState);
				}
				break;}
			default:break;};
		}
	}
	return 0;
}

int Rules::reloadRules()
{
	return 0;
}

Rules::~Rules()
{
}

char Rules::getBuff(char *buf, int n, FILE *fp)
{
    char *ret;
	
    for(;;) {
        ret = fgets(buf, n, fp);
        if (ret == NULL) return (NULL);
		if(feof(fp)) {printf("\n -------------------------------- EOF"); return (EOF);}
        if (buf[0] != '\n' && buf[0] != '#') return(1); // Skip blank lines and comments.
    }
}


int Rules::stat(iPoint *collidePoint){		// 1 = STATIC

	(*this->myArpe).playActionAudio(collidePoint);
	
	return 1;

}
int Rules::drgf(iPoint *collidePoint, Actuator *a){		// 2 = CAN_MOVE and drop if carying

	(*this->myArpe).playActionAudio(collidePoint);

	(*a).grabPoint(collidePoint);

	return (*this->myArpe).grabPoint(collidePoint,a);
}
int Rules::drgrp_grab(iPoint *collidePoint, Actuator *a){		// 3 = CAN_MOVE and repels if touched by moving

	(*this->myArpe).playActionAudio(collidePoint);

	(*a).grabPoint(collidePoint);

	return (*this->myArpe).grabPoint(collidePoint,a);
}
int Rules::drgrp_collided(iPoint *collidePoint, Actuator *a,iPoint *movingPoint) {

	if( (*(*movingPoint).myBase).errorSound != 0) { (*(*(*movingPoint).myBase).errorSound).play2D();}
	EnterCriticalSection(&this->myArpe->parserCS);
	if( (*(*collidePoint).actualAction).nextState == 0){
		this->getFromQueue = true;
	} else {
		this->nextState  = (*(*collidePoint).actualAction).nextState;
		this->getFromQueue = false;	}
	LeaveCriticalSection(&this->myArpe->parserCS);
	(*a).releasePoint();
	return (*this->myArpe).repelPoint(movingPoint);
}
int Rules::chgst(iPoint *collidePoint){		// 11 = GOTO_STATE, move to other state

	(*this->myArpe).playActionAudio(collidePoint);
	EnterCriticalSection(&this->myArpe->parserCS);
	if( (*(*collidePoint).actualAction).nextState == 0){
		this->getFromQueue = true;
	} else {
		this->nextState  = (*(*collidePoint).actualAction).nextState;
		this->getFromQueue = false;	}
	LeaveCriticalSection(&this->myArpe->parserCS);
	return 1;
}
int Rules::atto(iPoint *collidePoint, Actuator *a,iPoint *movingPoint){		// 4 = ATTRACT A SPECIFIC POINT

	if( (*movingPoint).id == (*(*collidePoint).actualAction).pointWaited){
		// COLLIDED WITH A WAITED POINT, TREAT THE REACTION
		(*this->myArpe).playActionAudio(collidePoint);
		EnterCriticalSection(&this->myArpe->parserCS);
		if( (*(*collidePoint).actualAction).nextState == 0){
			this->getFromQueue = true;
		} else {
			this->nextState  = (*(*collidePoint).actualAction).nextState;
			this->getFromQueue = false;	}
		LeaveCriticalSection(&this->myArpe->parserCS);
		(*a).releasePoint();
		return (*this->myArpe).attractPoint(movingPoint,collidePoint);
	} else { 
		// KEEP MOVING
		//if( (*(*movingPoint).myBase).errorSound != 0) { (*(*(*movingPoint).myBase).errorSound).play2D();}
		return (*this->myArpe).movePoint(movingPoint,a);
	}
	return 1;
}
int Rules::attrp(iPoint *collidePoint, Actuator *a,iPoint *movingPoint){		// 5 = ATTRACT a specific point, repel others

	if( (*movingPoint).id == (*(*collidePoint).actualAction).pointWaited){
		(*this->myArpe).playActionAudio(collidePoint);
		EnterCriticalSection(&this->myArpe->parserCS);
		if( (*(*collidePoint).actualAction).nextState == 0){
			this->getFromQueue = true;
		} else {
			this->nextState  = (*(*collidePoint).actualAction).nextState;
			this->getFromQueue = false;	}
		LeaveCriticalSection(&this->myArpe->parserCS);
		(*a).releasePoint();
		return (*this->myArpe).attractPoint(movingPoint,collidePoint);
	} else { 
		// REPEL IF WRONG POINT
		if( (*(*movingPoint).myBase).errorSound != 0) { (*(*(*movingPoint).myBase).errorSound).play2D();}
		(*a).releasePoint();
		return (*this->myArpe).repelPoint(movingPoint);
	}
		return 1;
}
int Rules::atta(iPoint *collidePoint, Actuator *a, iPoint *movingPoint){		// 6 = ATTRACT everything

	(*this->myArpe).playActionAudio(collidePoint);
	EnterCriticalSection(&this->myArpe->parserCS);
	if( (*(*collidePoint).actualAction).nextState == 0){
		this->getFromQueue = true;
	} else {
		this->nextState  = (*(*collidePoint).actualAction).nextState;
		this->getFromQueue = false;	}
	LeaveCriticalSection(&this->myArpe->parserCS);
	(*a).releasePoint();
	return (*this->myArpe).attractPoint(movingPoint,collidePoint);

}
int Rules::drpo(iPoint *collidePoint, Actuator *a, iPoint *movingPoint){		// 7 = DROP with check A SPECIFIC POINT

	if( (*movingPoint).id == (*(*collidePoint).actualAction).pointWaited){
		(*this->myArpe).playActionAudio(collidePoint);
		EnterCriticalSection(&this->myArpe->parserCS);
		if( (*(*collidePoint).actualAction).nextState == 0){
			this->getFromQueue = true;
		} else {
			this->nextState  = (*(*collidePoint).actualAction).nextState;
			this->getFromQueue = false;	}
		LeaveCriticalSection(&this->myArpe->parserCS);
		(*a).releasePoint();
		return (*this->myArpe).dropPoint(movingPoint,collidePoint);
	} else { 
		// REPEL IF WRONG POINT
		if( (*(*movingPoint).myBase).errorSound != 0) { (*(*(*movingPoint).myBase).errorSound).play2D();}
		(*a).releasePoint();
		return (*this->myArpe).repelPoint(movingPoint);
	}

}
int Rules::drpa(iPoint *collidePoint, Actuator *a, iPoint *movingPoint){		// 8 = DROP with check everything

	(*this->myArpe).playActionAudio(collidePoint);
	EnterCriticalSection(&this->myArpe->parserCS);
	if( (*(*collidePoint).actualAction).nextState == 0){
		this->getFromQueue = true;
	} else {
		this->nextState  = (*(*collidePoint).actualAction).nextState;
		this->getFromQueue = false;	}
	LeaveCriticalSection(&this->myArpe->parserCS);
	(*a).releasePoint();
	return (*this->myArpe).dropPoint(movingPoint,collidePoint);
}
int Rules::rplo(iPoint *collidePoint, Actuator *a, iPoint *movingPoint){		// 9 = REPELS a SPECIFIC POINT

	if( (*movingPoint).id == (*(*collidePoint).actualAction).pointWaited){
		(*this->myArpe).playActionAudio(collidePoint);
		EnterCriticalSection(&this->myArpe->parserCS);
		if( (*(*collidePoint).actualAction).nextState == 0){
			this->getFromQueue = true;
		} else {
			this->nextState  = (*(*collidePoint).actualAction).nextState;
			this->getFromQueue = false;	}
		LeaveCriticalSection(&this->myArpe->parserCS);
		(*a).releasePoint();
		return (*this->myArpe).repelPoint(movingPoint);
	} else { 
		// KEEP MOVING
		return (*this->myArpe).movePoint(movingPoint,a);
	}
	return 1;
}
int Rules::rpla(iPoint *collidePoint, Actuator *a, iPoint *movingPoint){		// 10 = REPELS everything.

	(*this->myArpe).playActionAudio(collidePoint);
	EnterCriticalSection(&this->myArpe->parserCS);
	if( (*(*collidePoint).actualAction).nextState == 0){
		this->getFromQueue = true;
	} else {
		this->nextState  = (*(*collidePoint).actualAction).nextState;
		this->getFromQueue = false;	}
	LeaveCriticalSection(&this->myArpe->parserCS);
	(*a).releasePoint();
	return (*this->myArpe).repelPoint(movingPoint);
	
}

int Rules::chgvm(iPoint *p){
	//ONLY NORMAL WORK ACTIONS CHANGE THE VISUALIZATION MODE
	if( p->configAction->pointMode != 8 )  // pointMode = 8 is KEEP_LAST, doesn't change
		p->viewMode = p->configAction->pointMode;
	return 1;
}
int Rules::tra(iPoint *p){
	int     i, j;
	double m[16];

	if( p->configAction->time > 0) {
		// Start animation thread
		_beginthread(callTraThread,0, p->configAction);
	} else {
		// apply here

		for( j = 0; j < 3; j++ ) 
			for( i = 0; i < 4; i++ ) 
				m[i*4+j] = (*p).position.trans[j][i];
		m[0*4+3] = m[1*4+3] = m[2*4+3] = 0.0;
		m[3*4+3] = 1.0;

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glPushMatrix();
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixd(m);
		glTranslated((*(*p).configAction).x,(*(*p).configAction).y,(*(*p).configAction).z); 
		glGetDoublev(GL_MODELVIEW_MATRIX,m);
		glPopMatrix();

		(*p).position.trans[0][0] = m[0];
		(*p).position.trans[1][0] = m[1];
		(*p).position.trans[2][0] = m[2];

		(*p).position.trans[0][1] = m[4];
		(*p).position.trans[1][1] = m[5];
		(*p).position.trans[2][1] = m[6];

		(*p).position.trans[0][2] = m[8];
		(*p).position.trans[1][2] = m[9];
		(*p).position.trans[2][2] = m[10];

		(*p).position.trans[0][3] = m[12]; //Tx
		(*p).position.trans[1][3] = m[13]; //Ty
		(*p).position.trans[2][3] = m[14]; //Tz


	}

	return 1;
}
int Rules::rot(iPoint *p){

	if( p->configAction->time > 0) {
		// Start animation thread
		_beginthread(callRotThread,0, p->configAction);
	} else {
		// apply here
		int     i, j;
		double m[16];
		
		for( j = 0; j < 3; j++ ) 
			for( i = 0; i < 4; i++ ) 
				m[i*4+j] = (*p).position.trans[j][i];


		m[0*4+3] = m[1*4+3] = m[2*4+3] = 0.0;
		m[3*4+3] = 1.0;
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glPushMatrix();
		glMatrixMode(GL_MODELVIEW);

		glLoadMatrixd(m);
		if((*(*p).configAction).x !=0) glRotated((*(*p).configAction).x ,1.0,0.0,0.0);
		if((*(*p).configAction).y !=0) glRotated((*(*p).configAction).y ,0.0,1.0,0.0);
		if((*(*p).configAction).z !=0) glRotated((*(*p).configAction).z ,0.0,0.0,1.0);

		glGetDoublev(GL_MODELVIEW_MATRIX,m);
		glPopMatrix();

		(*p).position.trans[0][0] = m[0];
		(*p).position.trans[1][0] = m[1];
		(*p).position.trans[2][0] = m[2];

		(*p).position.trans[0][1] = m[4];
		(*p).position.trans[1][1] = m[5];
		(*p).position.trans[2][1] = m[6];

		(*p).position.trans[0][2] = m[8];
		(*p).position.trans[1][2] = m[9];
		(*p).position.trans[2][2] = m[10];

		(*p).position.trans[0][3] = m[12]; //Tx
		(*p).position.trans[1][3] = m[13]; //Ty
		(*p).position.trans[2][3] = m[14]; //Tz



	}

	return 1;
}
int Rules::scl(iPoint *p){

	if( p->configAction->time > 0) {
		// Start animation thread
		_beginthread(callSclThread,0, p->configAction);
	} else {
		// apply here

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glPushMatrix();
		glMatrixMode(GL_MODELVIEW);
		int     i, j;
		double m[16];
		for( j = 0; j < 3; j++ ) 
			for( i = 0; i < 4; i++ ) 
				m[i*4+j] = (*p).position.trans[j][i];
		m[0*4+3] = m[1*4+3] = m[2*4+3] = 0.0;
		m[3*4+3] = 1.0;
		glLoadMatrixd(m);
		glScaled((*(*p).configAction).x,(*(*p).configAction).y,(*(*p).configAction).z);

		glGetDoublev(GL_MODELVIEW_MATRIX,m);
		glPopMatrix();

		(*p).position.trans[0][0] = m[0];
		(*p).position.trans[1][0] = m[1];
		(*p).position.trans[2][0] = m[2];

		(*p).position.trans[0][1] = m[4];
		(*p).position.trans[1][1] = m[5];
		(*p).position.trans[2][1] = m[6];

		(*p).position.trans[0][2] = m[8];
		(*p).position.trans[1][2] = m[9];
		(*p).position.trans[2][2] = m[10];

		(*p).position.trans[0][3] = m[12]; //Tx
		(*p).position.trans[1][3] = m[13]; //Ty
		(*p).position.trans[2][3] = m[14]; //Tz
	}

	return 1;
}
int Rules::chgm(iPoint *p){

	if ( (*p).findObject( (*(*p).configAction).modelToChange ) != 0 ){

		(*p).activeObjectID = (*(*p).configAction).modelToChange;
		printf("Model to change: %d , PID: %d", (*p).activeObjectID , (*p).id );
		return 1;
	}
	return 0;
}
int Rules::sets(iPoint *p){
	for(int i=0;i<3;i++)
		for(int j=0;j<4;j++)
			(*p).position.startTrans[i][j] = (*p).position.trans[i][j];
	return 1;
}
int Rules::setl(iPoint *p){
	for(int i=0;i<3;i++)
		for(int j=0;j<4;j++)
			(*p).position.lastTrans[i][j] = (*p).position.trans[i][j];
	return 1;
}
int Rules:: gets(iPoint *p){
	for(int i=0;i<3;i++)
		for(int j=0;j<4;j++)
			(*p).position.trans[i][j] = (*p).position.startTrans[i][j];
	return 1;
}
int Rules::getl(iPoint *p){
	for(int i=0;i<3;i++)
		for(int j=0;j<4;j++)
			(*p).position.trans[i][j] = (*p).position.lastTrans[i][j];
	return 1;
}

int Rules::esnd(iPoint *p){

	char *msg;
	msg = p->actualAction->eMsg;

	if( p->arduino != 0 ){
		if ( p->arduino->sendFromLookupTable(p->actualAction->eMsg) ){ 
			(*this->myArpe).playActionAudio(p);
			printf("\n Dentro de ESND Name: %s", p->name);
			printf("\n Returning");
			return 1;
		} else {
			if( (*(*p).myBase).errorSound != 0) { (*(*(*p).myBase).errorSound).play2D();}
			return -1;

		}
	} else {
		// Couldn't connect to it.
		printf("\n Could not connect to hardware, !");
		if( (*(*p).myBase).errorSound != 0) { (*(*(*p).myBase).errorSound).play2D();}
		//p->actualAction = 0;
		return -1;}
	//printf("\n Sent: %s (%d).",p->actualAction->eMsg,p->arduino->findCommand(p->actualAction->eMsg)->requestNumber);
	//p->actualAction = 0;

	printf("\n Shouldn't be passing by here!!!! basAR is crazy!!!");
	return 1;
	
}

int Rules::esndb(iPoint *p){

	if( p->arduino != 0 ){
		if ( p->arduino->sendNormInt(p->B) ){  // SENTENCE TO SEND TO ARDUINO A NORMALIZED VALUE
			(*this->myArpe).playActionAudio(p);
			printf("\n Dentro de ESND Name: %s", p->name);
			printf("\n Returning");
			return 1;
		} else {
			if( (*(*p).myBase).errorSound != 0) { (*(*(*p).myBase).errorSound).play2D();}
			return -1;

		}
	} else {
		// Couldn't connect to it.
		printf("\n Could not connect to hardware, !");
		if( (*(*p).myBase).errorSound != 0) { (*(*(*p).myBase).errorSound).play2D();}
		//p->actualAction = 0;
		return -1;}
	//printf("\n Sent: %s (%d).",p->actualAction->eMsg,p->arduino->findCommand(p->actualAction->eMsg)->requestNumber);
	//p->actualAction = 0;

	printf("\n Shouldn't be passing by here!!!! basAR is crazy!!!");

	return 1;
}
int Rules::ercv(iPoint *p){

	if( (p->arduino != 0 ) && (p->arduino->isConnected())){

		// Prepare what need to be scanned to 
		if( (strcmp(p->actualAction->eMsg,"0") == 0 ) || (strcmp(p->actualAction->eMsg,"") == 0 ) ){
			// Disabel reading thread to get data.
			EnterCriticalSection(&p->commCS);
			p->arduino->enableReceive = 0 ;
			LeaveCriticalSection(&p->commCS);

		} else {
			// Enable reading thread to get data.
			EnterCriticalSection(&p->commCS);
			p->arduino->enableReceive = 1;
			LeaveCriticalSection(&p->commCS);
		}
	}
	return 1;
}

int Rules::n255b(iPoint *p){
	p->B = ( 255 * p->B) / p->configAction->mathValue;
	return 1;
}

int Rules::chgnm(iPoint *p){

	// find actual model
	list<ipObject *>::iterator itObj;

	//printf("\n HERE ");

	if( !(*p).listObject.empty())
		for( itObj = (*p).listObject.begin() ; itObj != (*p).listObject.end() ; itObj++)
		{ //printf("X");
			if( (*itObj)->id == (*p).activeObjectID ){
				
				if( (*itObj)->id == (*p).listObject.size()){
					// test if last
					(*p).activeObjectID = 1;
					//printf("showfirst %d", (*p).activeObjectID);
					break;
				} else {
					// not the last can 
					(*p).activeObjectID++;
					//printf("shownext %d", (*p).activeObjectID);
					break;
				}
			}
		}
		return 1;
}
int Rules::adda(iPoint *p){
	p->A = p->A + p->configAction->mathValue;
	return 1; }
int Rules::addb(iPoint *p){
	p->B = p->B + p->configAction->mathValue;
	return 1; }
int Rules::amb(iPoint *p){
	p->A = p->A - p->B;
	return 1; }
int Rules::bma(iPoint *p){
	p->B = p->B - p->A;
	return 1; }
int Rules::nega(iPoint *p){
	p->A = -(p->A);
	return 1; }
int Rules::negb(iPoint *p){
	p->B = -(p->B);
	return 1; }
int Rules::mula(iPoint *p){
	p->A = p->A * p->configAction->mathValue;
	return 1; }
int Rules::mulb(iPoint *p){
	p->B = p->B * p->configAction->mathValue;
	return 1; }
int Rules::swab(iPoint *p){
	double value = 0;
	value = p->A;
	p->A = p->B;
	p->B = value;
	return 1;
}
int Rules::cmp(iPoint *p){
	double value = 0;
	value = p->A - p->B;
	// CHANGE THE NEXT STATE OF THE 
	switch(p->configAction->cmpAction){
	case 1: // GREATER
		if( value > 0 ) {
			if( p->configAction->nextState == 0){
				this->getFromQueue = true;
			} else {
				this->nextStateMath = p->configAction->nextState;
				this->getFromQueue = false;	}
		(*this->myArpe).playActionAudio(p);
		}
		break;
	case 2: // LESSER
		if( value < 0 ) {
			if( p->configAction->nextState == 0){
				this->getFromQueue = true;
			} else {
				this->nextStateMath = p->configAction->nextState;
				this->getFromQueue = false;	}
			(*this->myArpe).playActionAudio(p);
		}
		break;
	case 3: // EQUALS
		if( value == 0 ){
			if( p->configAction->nextState == 0){
				this->getFromQueue = true;
			} else {
				this->nextStateMath = p->configAction->nextState;
				this->getFromQueue = false;	}
			(*this->myArpe).playActionAudio(p);
		}
		break;
	default: break;
	}
	return 1; }
int Rules::cmpv(iPoint *p){
	double value = 0;
	value = p->A - p->configAction->mathValue;
	switch(p->configAction->cmpAction){
	case 1: // GREATER
		if( value > 0 ) {
			if( p->configAction->nextState == 0){
				this->getFromQueue = true;
			} else {
				this->nextStateMath = p->configAction->nextState;
				this->getFromQueue = false;	}
			(*this->myArpe).playActionAudio(p);
		}
		break;
	case 2: // LESSER
		if( value < 0 ) {
			if( p->configAction->nextState == 0){
				this->getFromQueue = true;
			} else {
				this->nextStateMath = p->configAction->nextState;
				this->getFromQueue = false;	}
			(*this->myArpe).playActionAudio(p);
		}
		break;
	case 3: // EQUALS
		if( value == 0 ){
			if( p->configAction->nextState == 0){
				this->getFromQueue = true;
			} else {
				this->nextStateMath = p->configAction->nextState;
				this->getFromQueue = false;	}
			(*this->myArpe).playActionAudio(p);
		}
		break;
	default: break;
	}
	return 1; }
int Rules::loada(iPoint *p){
	p->A = p->configAction->mathValue;
	return 1; }
int Rules::loadb(iPoint *p){
	p->B = p->configAction->mathValue;
	return 1; }
int Rules::dist(iPoint *p) {
	p->B = p->getDistanceTo(p->configAction->pointWaited);
	return 1;
}

int Rules::loadq(iPoint *p){
	// Copy value to QueueIndex
	this->queueIndex = p->configAction->mathValue;
	return 1;
}
int Rules::getq(iPoint *p){
	// Copy QueueIndex to B
	p->B = this->queueIndex;
	return 1;
}
int Rules::setq(iPoint *p){
	// Copy B to QueueIndex
	this->queueIndex = p->B;
	return 1;
}

int Rules::randi(iPoint *p){
	 srand(time(NULL));
	 p->B = (int)( rand() % (int)(p->configAction->mathValue2) + (int)(p->configAction->mathValue));
	 return 1;}

int Rules::getbx(iPoint *p){
	Base *b = 0;
	b = this->myArpe->findBase(p->configAction->pointWaited);
	if(b!=0){
		if (b->myInfraStructure->visible){
			p->B = b->myInfraStructure->baseTrans[0][3];
			return 1;}
		else { return 0;}
	}
	else { return -1;}
}
int Rules::getby(iPoint *p){
	Base *b = 0;
	b = this->myArpe->findBase(p->configAction->pointWaited);
	if(b!=0){
		if (b->myInfraStructure->visible){
			p->B = b->myInfraStructure->baseTrans[1][3];
			return 1;}
		else { return 0;}
	}
	else { return -1;}
}
int Rules::getbz(iPoint *p){
	Base *b = 0;
	b = this->myArpe->findBase(p->configAction->pointWaited);
	if(b!=0){
		if (b->myInfraStructure->visible){
			p->B = b->myInfraStructure->baseTrans[2][3];
			return 1;}
		else { return 0;}
	}
	else { return -1;}
}


void Rules::loadIdentity(double value[3][4]){
	for(int i=0;i<3;i++)
		for(int j=0;j<4;j++)
			value[i][j] = 0;
	value[0][0] = value[1][1] = value[2][2] = 1;
}

void Rules::copyMatrix(double source[3][4],double destiny[3][4]){
	for(int i=0;i<3;i++)
		for(int j=0;j<4;j++)
			destiny[3][4] = source[3][4];

}

void parserLock( void *data){

	State *st = static_cast<State *>(data);
	st->myRules->lockParser = 1;
	st->myRules->wait(st->time);
	st->myRules->lockParser = 0;
}

int Rules::parseRule(){

	State* s;
	State* ls;
	time_t actualTime = 0;
	int canParse = 0;
	//printf("\n AS: %d, NS: %d",this->actualState,this->nextState);
	
	if(this->getFromQueue){
		
		if ( this->queueIndex > 0) {
			
			// find which state from the queue to parse
			s = this->findQS(this->queueIndex);
			printf("\n Requesting queue state %d", s->id);
			if( s == 0 ) { printf("\n App went crazy, you asked State %d and we couldn't find. Please review app!!", this->nextState); exit(0);}  
			(*s).parseState();
			// AFTER THE QUEUED STATE is parsed SET THE NEXT TO QUEUE STATE TO PARSE
			this->queueIndex = this->setNextQueueItem(this->queueIndex );
			this->getFromQueue = false;

			EnterCriticalSection(&this->myArpe->parserCS);
			this->lastState = this->actualState;
			this->actualState = (*s).id;
			this->nextState = this->actualState;
			// Put the actual state onUse
			(*s).onUse = 1;
			// Last state isn't onUse anymore
			if (this->lastState != 0){
				ls = this->findState(this->lastState);
				(*ls).onUse = 0;
			}


			if( this->nextStateMath > 0 ) { // If a math next state is set, applies, has priority over configuration state and working state
				this->nextState = this->nextStateMath;
				printf(" *Q_NSM* ");
			} else {
				if( this->nextStateMath == 0) {
					this->getFromQueue = true;
					printf(" *Q_NSM-GFQ* ");
				} else {
					if( (*s).nextState > 0) { // This test if it is a configuration state.
						this->nextState = (*s).nextState;
						this->getFromQueue = false;
						printf(" *Q_NS* ");
					} else {
						if( (*s).nextState == 0){
							this->getFromQueue = true;
							printf(" *Q_NS-GFQ* ");
						}
					}
				}
			}
			this->nextStateMath = -1;

			//printf(" NS: %d", this->nextState);
			LeaveCriticalSection(&this->myArpe->parserCS);

			if( (*s).time != 0 ) _beginthread( parserLock, 0, s);
			printf(" ... OK, NS:%d, AS:%d",this->nextState,this->actualState);

			return 1;

			// point to the next state of the queue
		} else {
			printf("\n Queue must be a value above 0, app will be terminated, verify state %d.", this->actualState);
		}

	}
	else{
		//printf("(Here %d %d)", this->actualState,this->nextState);
		// IF WAITING DOESN'T PARSE NEXT.
		//FIND ACTIVE STATE
		if( this->nextState != this->actualState ){
			// If next state isn't the same of actual state, it's need to be parsed
			//s = this->findState(this->actualState);
			//if( (*s).onUse == 0)

			//do{
			printf("\n asking for state: %d", this->nextState);
			EnterCriticalSection(&this->myArpe->parserCS);
			LeaveCriticalSection(&this->myArpe->parserCS);
			s = this->findState(this->nextState);
			if( s == 0 ) { printf("\n App went crazy, you asked State %d and we couldn't find. Please review app!!", this->nextState); exit(0);}  

			(*s).parseState();
			//(*s).parsedTime = time(NULL); printf (" %ld", (*s).parsedTime );

			EnterCriticalSection(&this->myArpe->parserCS);
			this->lastState = this->actualState;
			this->actualState = (*s).id;
			// Put the actual state onUse
			(*s).onUse = 1;
			// Last state isn't onUse anymore
			/*if (this->lastState != 0){
				ls = this->findState(this->lastState);
				(*ls).onUse = 0;
			}*/

			// SOLVE NEXT STATE
			if( this->nextStateMath > 0 ) { // If a math next state is set, applies, has priority over configuration state and working state
				this->nextState = this->nextStateMath;
				printf(" *S_NSM* ");
			} else {
				if( this->nextStateMath == 0) {
					this->getFromQueue = true;
					printf(" *S_NSM-GFQ* ");
				} else {
					if( (*s).nextState > 0) { // This test if it is a configuration state.
						this->nextState = (*s).nextState;
						//printf("\n ********************* %d",this->nextState);
						this->getFromQueue = false;
						printf(" *S_NS* ");
					} else {
						if( (*s).nextState == 0){
							this->getFromQueue = true;
							printf(" *S_NS-GFQ* ");
						} 
					}

				}
			}
			this->nextStateMath = -1;
			//printf(" NS: %d", this->nextState);
			LeaveCriticalSection(&this->myArpe->parserCS);

			//printf(" --- time: %lf",s->time);
			if( (*s).time != 0 ) _beginthread( parserLock, 0, s);

			//} while ( (*s).nextState != 0 );
			printf(" ... OK, NS:%d, AS:%d",this->nextState,this->actualState);

			return 1;
		}
	}
	return 0;
}

void Rules::convParaToGl(const double para[3][4], GLdouble m_modelview[16], const double scale)
{
	m_modelview[0 + 0*4] = para[0][0]; // R1C1
	m_modelview[0 + 1*4] = para[0][1]; // R1C2
	m_modelview[0 + 2*4] = para[0][2];
	m_modelview[0 + 3*4] = para[0][3];
	m_modelview[1 + 0*4] = para[1][0]; // R2
	m_modelview[1 + 1*4] = para[1][1];
	m_modelview[1 + 2*4] = para[1][2];
	m_modelview[1 + 3*4] = para[1][3];
	m_modelview[2 + 0*4] = para[2][0]; // R3
	m_modelview[2 + 1*4] = para[2][1];
	m_modelview[2 + 2*4] = para[2][2];
	m_modelview[2 + 3*4] = para[2][3];
	m_modelview[3 + 0*4] = 0.0;
	m_modelview[3 + 1*4] = 0.0;
	m_modelview[3 + 2*4] = 0.0;
	m_modelview[3 + 3*4] = 1.0;

}

void Rules::wait( double seconds){
	clock_t endwait;
	endwait = clock () + seconds*CLOCKS_PER_SEC;
	while (clock() < endwait) {}
}

State* Rules::findQS(int valueID){

	list<queueState*>::iterator it;
	//printf("\n Size of List State %d, asked state %d", this->listState.size(), valueID);
	for( it = this->qS.begin(); it != this->qS.end(); it++)
		if((*(*it)).id == valueID)
			return (*it)->s;
	return 0;

}


int Rules::setNextQueueItem(int valueID){

	list<queueState*>::iterator it;
		for( it = this->qS.begin(); it != this->qS.end(); it++)
		if((*(*it)).id == valueID){
			if( it == this->qS.end()){
				it = this->qS.begin();
				return (*it)->id;
			} else {
				it++;
				return (*it)->id;
			}
		}
			
	return 0;

}

void Rules::addQS(queueState* value){
	list<queueState*>::iterator it;
	it = this->qS.begin();
	this->qS.push_back(value);
}