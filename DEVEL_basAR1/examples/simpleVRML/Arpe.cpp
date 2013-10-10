#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GL/glut.h>
#include <AR/ar.h>
#include <AR/arvrml.h>

#include <windows.h>
#include <process.h>
#include <time.h>

#include "Arpe.h"
#include "iVrml.h"
#include "AudioArpe.h"
#include "Rules.h"
#include "Base.h"
#include "ActuatorARTKSM.h"
#include "Actuator.h"
#include "InfraStructure.h"
#include "InfraSource.h"
#include "InfraARTKSM.h"
#include "Game.h"
#include "Action.h"
#include "ipObject.h"
#include "iObject3D.h"
#include "ipDist.h"
#include "iPoint.h"


#include <list>
using namespace std;


char Arpe::getBuff(char *buf, int n, FILE *fp)
{
    char *ret;
	
    for(;;) {
        ret = fgets(buf, n, fp);
        if (ret == NULL) return(NULL);
        if (buf[0] != '\n' && buf[0] != '#') return(1); // Skip blank lines and comments.
    }
}

Arpe::Arpe()
{
	this->myGame.myArpe = this;
	this->audioEngine = createIrrKlangDevice();
	strcpy(configFilename,"Data/config_basar");
}

Arpe::~Arpe()
{

}

int Arpe::arpeSetupEnvironment(){
	


	return 1;
}

int Arpe::arpeReadFiles(){

	FILE			*fp;
	char			buf[256],buf1[256],buf2[256],fileDAT[256];
	double			auxVolume, scW, scH, scBD, scRR, retscan;
	int				numBases, numActuators;

	printf("\n --------------------------------------------------------------------------");
	printf("\n Setting up ARPE configuration");
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
	// Read Application name
	//--------------------------------------------------------------------------
	getBuff(this->appName, 256, fp);
	//if( sscanf(buf, "%s", &this->appName) != 1) { printf("\n Check %s file format", this->configFilename); fclose(fp); return(0);}
	printf("\n appName: %s ", this->appName);
	//--------------------------------------------------------------------------
	// Read Screen properties
	//--------------------------------------------------------------------------
	getBuff(buf, 256, fp);
	retscan = sscanf(buf,"%s %d %d %d %d",&buf1, &scW, &scH, &scBD, &scRR);

	if(strcmp(buf1,"FULLSCREEN") == 0){
		// FULLSCREEN [WIDTH] [HEIGHT] [BIT_DEPTH] [REFRESH_RATE]
		if( retscan == 5){
			prefWindowed = FALSE;
			prefWidth = scW;					// Fullscreen mode width.
			prefHeight = scH;					// Fullscreen mode height.
			prefDepth = scBD;					// Fullscreen mode bit depth.
			prefRefresh = scRR;					// Fullscreen mode refresh rate. Set to 0 to use default rate.
		} else {
			printf("\n Using default FULLSCREEN values.");
			prefWindowed = FALSE;
			prefWidth = 640;					// Fullscreen mode width.
			prefHeight = 480;					// Fullscreen mode height.
			prefDepth = 32;						// Fullscreen mode bit depth.
			prefRefresh = 0;					// Fullscreen mode refresh rate. Set to 0 to use default rate.
		} 

		projection = false;

	} else {
		if ( strcmp(buf1,"WINDOWED") == 0){
			projection = false;
			prefWindowed = TRUE;

		} else {
			if(strcmp(buf1,"PROJECTION") == 0){
				// PROJECTION [WIDTH] [HEIGHT] [BIT_DEPTH] [REFRESH_RATE]
				if( retscan == 5){
					prefWindowed = FALSE;
					prefWidth = scW;					// PROJECTION mode width.
					prefHeight = scH;					// PROJECTION mode height.
					prefDepth = scBD;					// PROJECTION mode bit depth.
					prefRefresh = scRR;					// PROJECTION mode refresh rate. Set to 0 to use default rate.
				} else {
					printf("\n Using default PROJECTION values.");
					prefWindowed = FALSE;
					prefWidth = 640;					// PROJECTION mode width.
					prefHeight = 480;					// PROJECTION mode height.
					prefDepth = 32;						// PROJECTION mode bit depth.
					prefRefresh = 0;					// PROJECTION mode refresh rate. Set to 0 to use default rate.
				} 

				projection = true;
			}
		} 

	}

	//--------------------------------------------------------------------------
	// Read Standard definitions
	//--------------------------------------------------------------------------
	// - HOLDING Default iPoint object
	getBuff(buf, 256, fp);
	if (sscanf(buf, "%s %s", &buf1, &fileDAT) != 2) { printf("\n Check %s file format", this->configFilename); fclose(fp); return -1; }
	// Load Object	
	if (strcmp(buf1, "VRML") == 0) {
		//VRML Object
		iVrml* h = new iVrml();
		(*h).modelType = 1;
		(*h).vrmlID = arVrmlLoadFile(fileDAT); 
		if ((*h).vrmlID < 0) {
			printf("\n Error on Default holding iPoint object %s file or on VRML file (%d)",fileDAT,(*h).vrmlID);
			this->myGenericItens.holding = 0;
		} else {
			printf("\n Default holding iPoint object VRML id: %d ", (*h).vrmlID);
			this->myGenericItens.holding = h;
		}

	} else {
		//objeto nao eh vrml carregadar de outra forma
		printf("\n type not yet coded!!! Patience! ");
	}
	// - CANWORK Default iPoint object
	getBuff(buf, 256, fp);
	if (sscanf(buf, "%s %s", &buf1, &fileDAT) != 2) { printf("\n Check %s file format", this->configFilename); fclose(fp); return -1; }
	// Load Object	
	if (strcmp(buf1, "VRML") == 0) {
		//VRML Object
		iVrml* cw = new iVrml();
		(*cw).modelType = 1;
		(*cw).vrmlID = arVrmlLoadFile(fileDAT); 
		if ((*cw).vrmlID < 0) {
			printf("\n Error on Default canwork iPoint %s file or on VRML file (%d)",fileDAT,(*cw).vrmlID);
			this->myGenericItens.canwork = 0;
		} else {
			printf("\n Default canwork iPoint object VRML id: %d ", (*cw).vrmlID);
			this->myGenericItens.canwork = cw;
		}
	
	} else {
		//objeto nao eh vrml carregadar de outra forma
		printf("\n type not yet coded!!! Patience! ");
	}
	// - CANNOTWORK Default holding iPoint object
	getBuff(buf, 256, fp);
	if (sscanf(buf, "%s %s", &buf1, &fileDAT) != 2) { printf("\n Check %s file format", this->configFilename); fclose(fp); return -1; }

	// Load Object	
	if (strcmp(buf1, "VRML") == 0) {
		//VRML Object
		iVrml* cnw = new iVrml();
		(*cnw).modelType = 1;
		(*cnw).vrmlID = arVrmlLoadFile(fileDAT);
		if ((*cnw).vrmlID < 0) {
			printf("\n Error on Default cannotwork iPoint %s file or on VRML file (%d)",fileDAT,(*cnw).vrmlID);
			this->myGenericItens.cannotWork = 0;

		} else {
			printf("\n Default cannotwork iPoint object VRML id: %d", (*cnw).vrmlID);
			this->myGenericItens.cannotWork = cnw;
		}

	} else {
		//objeto nao eh vrml carregadar de outra forma
		printf("\n type not yet coded!!! Patience! ");
	}
	// - Default marker Cover
	getBuff(buf, 256, fp);
	if (sscanf(buf, "%s %s", &buf1, &fileDAT) != 2) { printf("\n Check %s file format", this->configFilename); fclose(fp); return -1; }

	// Load Object	
	if (strcmp(buf1, "VRML") == 0) {
		//VRML Object
		iVrml* mc = new iVrml();
		(*mc).modelType = 1;
		(*mc).vrmlID = arVrmlLoadFile(fileDAT); 
		if ((*mc).vrmlID < 0) {
			printf("\n Error on Default marker Cover %s file or on VRML file (%d)",fileDAT,(*mc).vrmlID);
			this->myGenericItens.markCover = 0;
		} else {
			printf("\n Default marker Cover VRML id: %d ", (*mc).vrmlID);
			this->myGenericItens.markCover = mc;
		}

	} else {
		//objeto nao eh vrml carregadar de outra forma
		printf("\n type not yet coded!!! Patience! ");
	}
	//--------------------------------------------------------------------------
	// Read Standard error sound
	//--------------------------------------------------------------------------
	getBuff(buf, 256, fp);
	if (sscanf(buf, "%s ", &buf1) != 1) { printf("\n error sound - Check %s file format", this->configFilename); fclose(fp); return -1; }

	if( strcmp(buf1,"NO_ERRORSOUND") == 0){ 
		this->myGenericItens.errorSound = 0;
		printf("\n No default error audio");
	} else {
			if (sscanf(buf, "%s %lf", &buf1,&auxVolume) != 2) { printf("\n error sound - Check %s file format", this->configFilename); fclose(fp); return -1; }

			AudioArpe *se = new AudioArpe();
			strcpy((*se).filename,buf1);
			(*se).volume = auxVolume;
			(*se).status = 1;
			(*se).myArpe = this;
			strcpy((*se).filename,buf1);
			(*se).loadSound();


			this->myGenericItens.errorSound = se;
			printf("\n Default error sound: %s",(*this->myGenericItens.errorSound).filename);

	}

	//--------------------------------------------------------------------------
	// Read Soundtrack Audio
	//--------------------------------------------------------------------------
	getBuff(buf, 256, fp);
	if (sscanf(buf, "%s ", &buf1) != 1) { printf("\n Soundtrack sound - Check %s file format", this->configFilename); fclose(fp); return -1; }

	if( strcmp(buf1,"NO_BACKTRACK") == 0){ 
		this->soundTrack = 0;
		printf("\n No backtrack audio");
	} else {
		if (sscanf(buf, "%s %s %lf", &buf1, &buf2, &auxVolume) != 3) { printf("\n Soundtrack sound - Check %s file format", this->configFilename); fclose(fp); return -1; }

		AudioArpe *st = new AudioArpe();

		strcpy((*st).filename,buf1);
		(*st).volume = auxVolume;
		if(strcmp(buf2,"PAUSED")==0)	(*st).status = 0;
		if(strcmp(buf2,"ONCE")==0)		(*st).status = 1;
		if(strcmp(buf2,"LOOP")==0)		(*st).status = 2;
		if(strcmp(buf2,"SILENCE")==0)	(*st).status = 3;
		(*st).myArpe = this;
		(*st).loadSound();

		this->soundTrack = st;
		printf("\n SoundTrack sound: %s",(*this->soundTrack).filename);

	}
		
	//--------------------------------------------------------------------------
	// Read Start Audio
	//--------------------------------------------------------------------------
	getBuff(buf, 256, fp);
	if (sscanf(buf, "%s ", &buf1) != 1) { printf("\n Start sound - Check %s file format", this->configFilename); fclose(fp); return -1; }

	if( strcmp(buf1,"NO_STARTSOUND") == 0){ 
		this->startAudio = 0;
		printf("\n No start audio");
	} else {
		if (sscanf(buf, "%s %s %lf", &buf1, &buf2, &auxVolume) != 3) { printf("\n Start sound - Check %s file format", this->configFilename); fclose(fp); return -1; }

		AudioArpe *sa = new AudioArpe();

		strcpy((*sa).filename,buf1);
		(*sa).volume = auxVolume;
		if(strcmp(buf2,"PAUSED")==0)	(*sa).status = 0;
		if(strcmp(buf2,"ONCE")==0)		(*sa).status = 1;
		if(strcmp(buf2,"LOOP")==0)		(*sa).status = 2;
		if(strcmp(buf2,"SILENCE")==0)	(*sa).status = 3;
		(*sa).myArpe = this;
		(*sa).loadSound();

		this->startAudio = sa;	
		printf("\n Start sound: %s",(*this->startAudio).filename);
	}


	//--------------------------------------------------------------------------
	// Read	Behavior configuration file
	//--------------------------------------------------------------------------
	getBuff(buf, 256, fp);
	if (sscanf(buf, "%s", &buf1) != 1) { printf("\n Check %s file format", this->configFilename); fclose(fp); return -1; }

	Rules *mr = new Rules();
	strcpy((*mr).configFilename,buf1);
	this->myRules = mr;
	printf("\n Rules configuration file: %s",(*this->myRules).configFilename);

	//--------------------------------------------------------------------------
	// Read	number of bases to create
	//--------------------------------------------------------------------------
	getBuff(buf, 256, fp);
	if (sscanf(buf, "%d", &numBases) != 1) { printf("\n Check %s file format", this->configFilename); fclose(fp); return -1; }
	//--------------------------------------------------------------------------
	// Read	the bases configuration files and save on base structures
	//--------------------------------------------------------------------------
	for(int i = 0; i < numBases ; i++){

		Base *b = new Base();
		getBuff(buf, 256, fp);
		if (sscanf(buf, "%s", &buf1) != 1) { printf("\n Check %s file format", this->configFilename); fclose(fp); return -1; }
		(*b).id = i+1;
		strcpy((*b).configFilename,buf1);
		printf("\n Base %d configuration file: %s",(*b).id, (*b).configFilename);
		(*b).myArpe = this;
		this->addBase(b);
	}

	//--------------------------------------------------------------------------
	// Read	number of actuators to create
	//--------------------------------------------------------------------------
	getBuff(buf, 256, fp);
	if (sscanf(buf, "%d", &numActuators) != 1) { printf("\n Check %s file format", this->configFilename); fclose(fp); return -1; }
	//--------------------------------------------------------------------------
	// Read	the bases configuration files and save on base structures
	//--------------------------------------------------------------------------
	for(int j = 0; j < numActuators ; j++){

		getBuff(buf, 256, fp);
		if (sscanf(buf, "%s %s", &buf1, &buf2) != 2) { printf("\n Check %s file format", this->configFilename); fclose(fp); return -1; }
		
		if(strcmp(buf1,"ARTKSM")==0){
			ActuatorARTKSM *a = new ActuatorARTKSM();
			(*a).id = j+1;
			(*a).type = 1;
			strcpy((*a).configFilename,buf2);
			printf("\n Actuator %d ARTKM configuration file: %s",(*a).id, (*a).configFilename);
			(*a).myArpe = this;
			this->addActuator(a);
		} else {
			printf("\n *** Type of Actuator isn`t coded yet!");
		}
	}

	//--------------------------------------------------------------------------
	// Read	the user information
	//--------------------------------------------------------------------------
	getBuff(buf, 256, fp);

	retscan = sscanf(buf, "%s", &buf1);
	if ( retscan = 1 ) {

		if(strcmp(buf1,"NO_USER")==0){
			this->myUser = 0;
			printf("\n *** No user has been assigned!");
		} else {
			User *u = new User();
			strcpy(u->filename,buf1);
			printf("\n User filename: %s", u->filename);
			u->myArpe = this;
			this->myUser = u;
		}

	} else if ( retscan = 0 ) {
		this->myUser = 0;
		printf("\n *** No user has been assigned!");
	}





	printf("\n %s correctly read!",this->configFilename);
	fclose(fp);
	return 1;
}

void Arpe::arpeWriteFiles()
{
}

void Arpe::addBase(Base* value)
{
	list<Base*>::iterator it;
	it = this->listBase.begin();
	this->listBase.push_back(value);

}

int Arpe::interactionControl()
{
	int toReturn = 0;
	//printf(
	if(this->myRules->lockParser == 0 )
		(*this->myRules).parseRule();
	//printf("\n interactionControl... OK, NS:%d, AS:%d",this->myRules->nextState,this->myRules->actualState);
	//Test if actuator got a iPoint and the reactions
	list<Actuator*>::iterator itActuator;
	iPoint *movingPoint;
	iPoint *collidePoint;
	double distance = 0;
	int result = 0;

	//SCAN THE ACTUATORS AND TEST WHAT IT IS DOIND AND CHECK THE POINT ACTIONS
	for( itActuator = this->listActuator.begin() ; itActuator != this->listActuator.end() ; itActuator++){
	//	printf("\n Testing Act %s.", (*itActuator)->name);

		switch( (*(*itActuator)).transporting){

		case 0:{ // ACTUATOR ISN'T TRANSPORTING ANYTHING
			switch ( (*(*itActuator)).buttons[0]){
				case 0:{ // CONTROL BUTTON ISN'T ACTIVE (MARKER NOT VISIBLE)
					toReturn = 1;
					break;}
				case 1:{ // CONTROL BUTTON IS ACTIVE (MARKER IS VISIBLE)
					// TRY TO DO AN ACTION WITH ANY POINT
					// Search for the closest point to the actuator
					collidePoint = this->findPointNearActuator((*itActuator),&distance);
					if (collidePoint == 0) {toReturn = 1; // NO COLLISION BETWEEN ACTUATOR AND POINT
					} else {
						if(this->myRules->lockParser == 0 ){

							// COLLISION HANDLING
							switch(	(*(*collidePoint).actualAction).opcode){
							case 1: { // STAT
								return (*this->myRules).stat(collidePoint);
								break;}
							case 2: { // DRGF
								return (*this->myRules).drgf(collidePoint,(*itActuator));
								break;}
							case 3: { // DRGRP
								return (*this->myRules).drgrp_grab(collidePoint,(*itActuator));
								break;}
							case 11: { // CHGST
								return (*this->myRules).chgst(collidePoint);
								break;}
							default: break;				
							}
						}
					}
				break;}
			default:  break;};
			break;}

		case 1:{ // ACTUATOR IS TRANSPORTING A POINT
	
			// COLLISION HANDLING
			switch ( (*(*itActuator)).buttons[0]){
				case 0:{ // CONTROL BUTTON ISN'T ACTIVE (MARKER NOT VISIBLE)
					// SEARCH THE CLOSEST POINT EXCEPT BY THE ONE THAT IS BEEN CARRIED
					movingPoint = (*(*itActuator)).transportingPoint;
					collidePoint = this->findPointNearActuator((*itActuator),&distance);
					if (collidePoint == 0) {// NO COLLISION BETWEEN ACTUATOR AND POINT
						(*(*itActuator)).releasePoint();
						return this->releasePoint(movingPoint); 
					}
					// RELEASE OR DROP
					switch((*(*collidePoint).actualAction).opcode){
					case 7:{ // DRPO
						return (*this->myRules).drpo(collidePoint,(*itActuator),movingPoint);
						break;}
					case 8:{ // DRPA
						return (*this->myRules).drpa(collidePoint,(*itActuator),movingPoint);
						break;}
					default: 
						(*(*itActuator)).releasePoint();
						return this->releasePoint(movingPoint);
						break;
					};
					break;}
				case 1:{ // CONTROL BUTTON IS ACTIVE (MARKER IS VISIBLE)
					// SEARCH THE CLOSEST POINT EXCEPT BY THE ONE THAT IS BEEN CARRIED
					movingPoint = (*(*itActuator)).transportingPoint;
					collidePoint = this->findPointNearActuator((*itActuator),&distance);
					if (collidePoint == 0) return this->movePoint(movingPoint,(*itActuator)); // NO COLLISION BETWEEN ACTUATOR AND POINT
					// KEEP MOVING OR COLLIDE AND TEST REACTION
					switch((*(*collidePoint).actualAction).opcode){
						case 3: { // DRGRP (IF collides repels moving point to origin)
							return (*this->myRules).drgrp_collided(collidePoint,(*itActuator),movingPoint);
							break;}
						case 4: { // ATTO (If collides attract moving point to it) (Clone trans/rot/scale of the point)
							return (*this->myRules).atto(collidePoint,(*itActuator),movingPoint);
							break;}
						case 5: { // ATTRP (If collides attracts point to it and repels any other point) (Clone trans/rot/scale of the point)
							return (*this->myRules).attrp(collidePoint,(*itActuator),movingPoint);					
							break;}
						case 6: { // ATTA (If collides attracts any point)
							return (*this->myRules).atta(collidePoint,(*itActuator),movingPoint);		
							break;}
						case 9: { // RPLO (If collided repels a point)
							return (*this->myRules).rplo(collidePoint,(*itActuator),movingPoint);	
							break;}
						case 10: { // RPLA
							return (*this->myRules).rpla(collidePoint,(*itActuator),movingPoint);	
							break;}
						default: { // KEEP MOVING
							return this->movePoint(movingPoint,(*itActuator));
							break;}
					}
					break;}
			default:  break;};
			break;}
		default: 
			break;};

	//printf("\n END -- Testing Act %s.", (*itActuator)->name);
	}

	return toReturn;
}

Base* Arpe::findBase(int valueID)
{
	list<Base*>::iterator it;

	for( it = this->listBase.begin(); it != this->listBase.end(); it++)
		if((*(*it)).id == valueID)
			return (*it);
	
	printf("\n *******Object not found");
	return 0;
}

iPoint* Arpe::findIPoint(int valueID){
	list<Base*>::iterator it;
	list<iPoint*>::iterator itA;

	for( it = this->listBase.begin(); it != this->listBase.end(); it++)
		for( itA = (*(*it)).listPoint.begin(); itA != (*(*it)).listPoint.end(); itA++)
			if( (*(*itA)).id == valueID)
				return (*itA);
	
	//printf("\n *******Object not found");
	return 0;
}

iPoint* Arpe::findPointNearActuator(Actuator* a, double *distance)
{
	list<Base*>::iterator b;
	list<iPoint*>::iterator p;
	iPoint *ip_actual,*ip_near;
	ip_actual = 0;
	ip_near = 0;
	
	float xPto,yPto,zPto, dist;
	double matInv[3][4], matResult[3][4];

	// ONLY ACTUATOR TYPE ARTKSM
	ActuatorARTKSM* a2 = static_cast<ActuatorARTKSM*>(a);
	//printf("\n A %3.2f ,  %3.2f , %3.2f", a2->ipTra[0], a2->ipTra[1], a2->ipTra[2]);

	
	for(b = this->listBase.begin() ; b != this->listBase.end() ; b++){

		arUtilMatInv( (*(*(*b)).myInfraStructure).baseTrans, matInv);
		arUtilMatMul(matInv,(*a).interactionTrans,matResult);

		for(p = (*(*b)).listPoint.begin() ; p != (*(*b)).listPoint.end() ; p++){ //Search near iPoints

			xPto =  ((*(*p)).position.trans[0][3] + a2->ipTra[1]) - matResult[0][3]; //GAMBIARRA
			yPto =  ((*(*p)).position.trans[1][3] + a2->ipTra[0]) - matResult[1][3];  //GAMBIARRA DEVIDO AO EIXO DE COORDENADA REAL COM 
			zPto =  ((*(*p)).position.trans[2][3] + a2->ipTra[2]) - matResult[2][3];  //GAMBIARRA O DO OPENGL
			dist = xPto*xPto + yPto*yPto + yPto*zPto;
			//printf("\n IPOINT %d %3.2f ,  %3.2f , %3.2f : %3.2f", (*p)->id,  xPto , yPto, zPto, dist);
			*distance = dist;

			// Save distance on the point´s distance vector
			ipDist *ipdist;
			ipdist = (*p)->findActuator(a->id);
			ipdist->distance = dist;
			ip_actual = (*p);
			
			// SENSE TREATMENT
			(*(*p)).ball.senseStatus = 2;
			
			if( (dist > 0) && (dist < (*(*p)).ball.distCollision*2)) // Try if on sensing distance
				if( (*(*(*p)).actualAction).pointMode == 6) // IF SENSING
					switch( (*(*(*p)).actualAction).opcode ){
					case 1: // STATIC - With one point shows that can be touched
					case 2: // DRGF - With one point shows that can be moved
					case 11:{ // CHGST - With one point shows that can be touched
							if( (*a).transporting == 0) (*(*p)).ball.senseStatus = 3;
							break;}

					case 4: // ATTO - With one point shows nothing, with 2 poitns shows three (holding, canwork, cannotwork) 
					case 5: // ATTRP - With one point shows nothing, with 2 points shows three (holding, canwork, cannotwork) 
					case 7: // DRPO - With one point shows nothing, with 2 poitns shows three (holding, canwork, cannotwork) 
					case 9:{ // RPLO - With one point shows nothing, with 2 poitns shows three (holding, canwork, cannotwork) 
							if( (*a).transporting == 1)
								if ( (*(*a).transportingPoint).id == (*(*(*p)).actualAction).pointWaited ) {
									(*(*p)).ball.senseStatus = 3;
								} else {
									(*(*p)).ball.senseStatus = 4;
							} 
							break;}

					case 3:{ // DRGRP - With one point shows that can be moved, with 2 points shows that will repels (cannotwork)
							(*(*p)).ball.senseStatus = 3;
							if( (*a).transporting == 1) (*(*p)).ball.senseStatus = 4;
							break;}

					case 6: // ATTA - With one point shows nothing, with 2 points shows that will attract (canwork)
					case 8: // DRPA - With one point shows nothing, with 2 points shows that will drop (canwork)
					case 10:{ // RPLA - With one point shows nothing, with 2 points shows that will repels (canwork)
							if( (*a).transporting == 1) (*(*p)).ball.senseStatus = 3;
							break;}

					case 12: // Doen't need 
					case 13: // Doen't need 
					case 14: // Doen't need 
					case 15: // Doen't need 
					case 16: // Doen't need 
					case 17: // Doen't need 
					case 18: // Doen't need 
					case 19: // Doen't need 
					case 20: // Doen't need 
					default: break;					}

				//printf("\n Distance of point %d to actuator %d : %lf",ip_actual->id,a->id, dist);

				// Calculate the nearest point
				if( ip_actual != a->transportingPoint)
					if( ip_near == 0){
						
						ip_near = ip_actual;
					} else {
						if ( ip_actual->getDistanceTo(a->id) < ip_near->getDistanceTo(a->id)){
							
							ip_near = ip_actual;
						}
					}

				
		}
	}
	
	if ( ip_near != 0 ){
		if ( ip_near->getDistanceTo(a->id) > 0 && ( ip_near->getDistanceTo(a->id) < ip_near->ball.distCollision) )
			return ip_near;	}

	//if( (*a).transporting == 0){ // Not moving anything
	//	if( (dist > 0 ) &&  (dist < (*(*p)).ball.distCollision)){
	//		if( near != 0) return near;
	//		return (*p);
	//	}
	//} else {	// Moving a iPoint
	//	if( (*(*p)).id != (*(*a).transportingPoint).id){
	//		if( (dist > 0 ) &&  (dist < (*(*p)).ball.distCollision)){
	//			return (*p);
	//		}
	//	}
	//}

	return 0;
}

int Arpe::movePoint(iPoint* move, Actuator* actuator)
{


	double matInv [3][4], matResult[3][4],matTrans[3][4];
	static double matDiff[3][4];

	arUtilMatMul((*actuator).interactionTrans,(*actuator).transDiff,matTrans);
	arUtilMatInv((*(*(*move).myBase).myInfraStructure).baseTrans,matInv);
	arUtilMatMul(matInv,matTrans,matResult);

	for(int i=0;i<3;i++)
		for(int j=0;j<4;j++)
			(*move).position.trans[i][j] = matResult[i][j];
	
	//(*movingPoint).m_iPoint_trans[2][3]=0.0;


//	(*move).position.actualTra[0] = matResult[0][3];
//	(*move).position.actualTra[1] = matResult[1][3];
//	(*move).position.actualTra[2] = matResult[2][3];

	//(*movingPoint).m_iPoint_actualTra[2] = 0.0;
	return 2;
}

int Arpe::releasePoint(iPoint* value)
{
    return 0;
}

int Arpe::grabPoint(iPoint* value, Actuator* a)
{
	double matTrans[3][4],matInv[3][4];

	//(*value).position.bakeActualTrans();


	//if ( (*a).type = 1){
	//	ActuatorARTKSM* a2 = static_cast<ActuatorARTKSM*>(a);
	//	(*a).interactionTrans[0][3] += a2->ipTra[1];
	//	(*a).interactionTrans[1][3] += a2->ipTra[0];
	//	(*a).interactionTrans[2][3] += a2->ipTra[2];
	//}
		
	arUtilMatMul( (*(*(*value).myBase).myInfraStructure).baseTrans,(*value).position.trans,matTrans);

	arUtilMatInv((*a).interactionTrans,matInv);
	arUtilMatMul(matInv,matTrans,(*a).transDiff);

	// Copy ACTUAL to LAST
	for(int i = 0 ; i < 3 ; i++)
		for (int j = 0 ; j < 4 ; j++)
			(*value).position.lastTrans[i][j] = (*value).position.trans[i][j];

	return 1;
}

int Arpe::dropPoint(iPoint* move,iPoint* collide)
{
    
	//(*move).position.bakeActualTrans();

	return 3;
}

int Arpe::attractPoint(iPoint* move,iPoint* collide)
{

	for(int i=0;i<3;i++)
		for(int j=0;j<4;j++)
			(*move).position.trans[i][j] = (*collide).position.startTrans[i][j];

	//for(int i = 0; i<3;i++){
	//	(*move).position.actualRot[i] = (*collide).position.actualRot[i];
	//	(*move).position.actualTra[i] = (*collide).position.actualTra[i];
	//	(*move).position.actualScl[i] = (*collide).position.actualScl[i];
	//}
	//(*move).position.bakeActualTrans();

	return 3;
}

int Arpe::repelPoint(iPoint* move)
{
	int i,j;

	for(i=0;i<3;i++)
		for(j=0;j<4;j++)
			(*move).position.trans[i][j] = (*move).position.startTrans[i][j];

//	(*move).position.trans[0][0] = (*move).position.trans[1][1] = (*move).position.trans[2][2] = 1;


//	for(i=0;i<3;i++){
//		(*move).position.actualRot[i] = (*move).position.startRot[i];
//		(*move).position.actualTra[i] = (*move).position.startTra[i];
//		(*move).position.actualScl[i] = (*move).position.startScl[i];
//	}

	//(*move).position.bakeActualTrans();

	return 4;
}

int Arpe::showStatus(iPoint* value)
{
    return 0;
}

int Arpe::playAudio(iPoint* value)
{
    return 0;
}

void Arpe::addActuator(Actuator* value){
	list<Actuator*>::iterator it;
	it = listActuator.begin();
	listActuator.push_back(value);

}

Actuator* Arpe::findActuator(int valueID){
	list<Actuator*>::iterator it;

	for( it = this->listActuator.begin(); it != this->listActuator.end(); it++)
		if((*(*it)).id == valueID)
			return (*it);

	printf("\n *******Actuator not found");
	return 0;
}

int Arpe::playActionAudio(iPoint* value){

	if( strcmp( (*(*value).actualAction).audioFilename,"") != 0) { 
		if( (*(*value).actualAction).audioOverplay == false) 
			this->audioEngine->stopAllSounds();
		if( value->actualAction->audio3D ){
			ISoundSource* as = this->audioEngine->addSoundSourceFromFile(value->actualAction->audioFilename);
			vec3df pos(value->actualAction->x,value->actualAction->y,value->actualAction->z);
			as->setDefaultVolume(value->actualAction->mathValue2);
			this->audioEngine->play3D(as,pos);
		}
		else{
			this->audioEngine->play2D((*(*value).actualAction).audioFilename);	
		}
		return 1;
	}
	return 0;
}

int Arpe::verifyConsistency(){


	fflush(stdout);
    glEnable(GL_TEXTURE_2D);
	// Testing standard balls (4 points)
	if( (*this->myGenericItens.holding).modelType == 1){ 
					iVrml* vrmlModel = static_cast<iVrml*>(this->myGenericItens.holding);
					(*vrmlModel).draw();
					printf(".");
	}
	if( (*this->myGenericItens.canwork).modelType == 1){ 
					iVrml* vrmlModel = static_cast<iVrml*>(this->myGenericItens.canwork);
					(*vrmlModel).draw();
					printf(".");
	}
	if( (*this->myGenericItens.cannotWork).modelType == 1){ 
					iVrml* vrmlModel = static_cast<iVrml*>(this->myGenericItens.cannotWork);
					(*vrmlModel).draw();
					printf(".");
	}
	if( (*this->myGenericItens.markCover).modelType == 1){ 
					iVrml* vrmlModel = static_cast<iVrml*>(this->myGenericItens.markCover);
					(*vrmlModel).draw();
					printf(".");
	}		


	// Test Base itens

	list<Base*>::iterator itB;

	for(itB = this->listBase.begin(); itB != this->listBase.end(); itB++){
		// Base source covers		
		list<InfraSource*>::iterator infS;
		for( infS = (*(*(*itB)).myInfraStructure).listSource.begin() ; infS != (*(*(*itB)).myInfraStructure).listSource.end() ; infS ++ ){
			
			switch( (*(*infS)).type){
				case 1: {	// ARTKSM 
						InfraARTKSM* infARTK = static_cast<InfraARTKSM*>(*infS);
						if( (*(*infARTK).cover).modelType == 1) {
							iVrml* vrmlModel = static_cast<iVrml*>((*infARTK).cover);
							(*vrmlModel).draw();
							printf(".");
						}
						break;
						}
				default: break;
			}
		}

		// Base status
		(*(*(*itB)).status).draw();
		printf(".");

		list<iPoint*>::iterator itP;
		for(itP = (*(*itB)).listPoint.begin(); itP != (*(*itB)).listPoint.end(); itP++){

			// Testing standard balls (4 points)
			if( (*(*(*itP)).ball.holding).modelType == 1){ 
				iVrml* vrmlModel = static_cast<iVrml*>((*(*itP)).ball.holding);
				(*vrmlModel).draw();
				printf(".");
			}
			if( (*(*(*itP)).ball.canWork).modelType == 1){ 
				iVrml* vrmlModel = static_cast<iVrml*>((*(*itP)).ball.canWork);
				(*vrmlModel).draw();
				printf(".");
			}
			if( (*(*(*itP)).ball.canWork).modelType == 1){ 
				iVrml* vrmlModel = static_cast<iVrml*>((*(*itP)).ball.cannotWork);
				(*vrmlModel).draw();
				printf(".");
			}

			//Testing Models
			list<ipObject*>::iterator iObj;
			for( iObj = (*(*itP)).listObject.begin() ; iObj != (*(*itP)).listObject.end() ; iObj++){

				switch( (*(*iObj)).type){
						case 1: { // MODEL 3D
								iObject3D* i3D = static_cast<iObject3D*>(*iObj);
								switch((*i3D).modelType){
									case 1: { // VRML
										iVrml* model = static_cast<iVrml*>(i3D);
										(*model).draw();
										printf(".");
										break;}
									case 2: { // Assimp
										break;}
									default: break;
								};
								break;}
							case 2: { // STATUS
								break;}
							default: break;	}
				
			}

		}
	}

    glDisable(GL_TEXTURE_2D);
	fprintf(stdout, " done\n");


	return 1;

}