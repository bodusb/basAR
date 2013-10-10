#include "Base.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GL/glut.h>
#include <AR/ar.h>
#include <AR/arvrml.h>
#include <AR/gsub_lite.h>

#include "Arpe.h"
#include "InfraStructure.h"
#include "InfraSource.h"
#include "InfraARTKSM.h"
#include "iVrml.h"
#include "iObject3D.h"
#include "iPoint.h" 
#include "Action.h"

#include <list>
using namespace std;

#define VIEW_SCALEFACTOR_1		1.0			// 1.0 ARToolKit unit becomes 1.0 of my OpenGL units.


char Base::getBuff(char *buf, int n, FILE *fp)
{
    char *ret;
	
    for(;;) {
        ret = fgets(buf, n, fp);
        if (ret == NULL) return(NULL);
        if (buf[0] != '\n' && buf[0] != '#') return(1); // Skip blank lines and comments.
    }
}

void Base::addPoint(iPoint* value)
{
	list<iPoint*>::iterator it;
	it = this->listPoint.begin();
	this->listPoint.push_back(value);

}

iPoint* Base::findPoint(int valueID)
{
	list<iPoint*>::iterator it;

	for( it = this->listPoint.begin(); it != this->listPoint.end(); it++)
		if((*(*it)).id == valueID)
			return (*it);
	
	printf("\n *******Base not found");
	return 0;
}

Base::Base()
{
	this->adaptType = 0;
}

int Base::baseReadFile()
{
	FILE          *fp;
	char		  buf[256],buf1[256],buf2[256],fileDAT[256];
	double		  auxVolume;
	int			  nIPoints;

	//--------------------------------------------------------------------------
	// Open the Base Configuration file
	//--------------------------------------------------------------------------
	printf("\n --------------------------------------------------------------------------");
	if( (fp=fopen(this->configFilename,"r")) == NULL) {
		printf("\n Error on opening %s !! ",this->configFilename);
		return  -1;
	}
	printf("\n Opening file %s ,  Base Configuration", this->configFilename);
	printf("\n --------------------------------------------------------------------------");

	//--------------------------------------------------------------------------
	// READ THE Base Name
	//--------------------------------------------------------------------------
	getBuff(buf, 256, fp);  // -> Read base name
	if (sscanf(buf, "%s", &this->name) != 1) { printf("\n Read base name - Check %s file format", this->configFilename); fclose(fp); return  -1;	}
	printf("\n Base name: %s", this->name);

	//--------------------------------------------------------------------------
	// READ the base infrastructure source
	//--------------------------------------------------------------------------
	InfraStructure* iS = new InfraStructure();

	getBuff(buf, 256, fp);	// -> Read source
	if (sscanf(buf, "%s", &buf1) != 1) { printf("\n Read source - Check %s file format", this->configFilename); fclose(fp); return  -1;	}
	if( strcmp(buf1,"ARTKSM") == 0){  
		//--------------------------------------------------------------------------
		// READ THE MARKER CONFIGURATION FOR A MARKER BASED BASE
		//--------------------------------------------------------------------------
		printf("\n Single ARToolKit marker based base.");
		InfraARTKSM* iA = new InfraARTKSM();
		strcpy((*iA).name,"ARTKSM Base Source");
		(*iA).type = 1;

		// Marker that defines the base
			getBuff(buf, 256, fp);	// -> Read patternSource
			if (sscanf(buf, "%s", &buf1) != 1) { printf("\n Read patternSource - Check %s file format", this->configFilename);fclose(fp); return  -1;	}
			if (((*iA).patternNumber = arLoadPatt(buf1)) < 0) { fclose(fp);  return(0);	}
			printf("\n Using marker: %s, id: %d", buf1, (*iA).patternNumber); 
		// Marker Width
			getBuff(buf, 256, fp);	// -> read patternWidth
			if (sscanf(buf, "%lf", &(*iA).markerWidth) != 1) {  printf("\n Read patternWidth - Check %s file format", this->configFilename); fclose(fp); return  -1;  }
		// Marker Center
			getBuff(buf, 256, fp);	// -> read patternCenter
			if (sscanf(buf, "%lf %lf", &(*iA).markerCenter[0], &(*iA).markerCenter[1]) != 2) {
				fclose(fp); return(0);  }
			printf(" W: %3.2f, Center(%3.2f,%3.2f)", (*iA).markerWidth , (*iA).markerCenter[0], (*iA).markerCenter[1]);
		// Marker cover (USE_DEFAULT to get default marker cover / NO_COVER to don't use a marker cover)
			getBuff(buf, 256, fp);	//	-> read patternCover
			if (sscanf(buf, "%s %s", &buf1, &fileDAT) != 2) { printf("\n read patternCover - Check %s file format", this->configFilename); fclose(fp); return  -1; }

			if( strcmp(buf1, "NO_COVER") == 0){
				(*iA).cover = 0;
				printf("\n Base Marker without cover.");
			}
			{
				if ( strcmp(buf1, "USE_DEFAULT") == 0) { 
					(*iA).cover = (*myArpe).myGenericItens.markCover;
					printf("\n Using Default Cover");
				}
				else{
					if (strcmp(buf1, "VRML") == 0) {
						//VRML Object
						iVrml *c = new iVrml();
						(*c).vrmlID = arVrmlLoadFile(fileDAT); 
						if ((*c).vrmlID < 0) {
							printf("\n Error on Marker Base Cover %s file or on VRML file (%d)",fileDAT,(*c).vrmlID);
						}
						printf("\n Marker Base Cover object VRML id: %d ", (*c).vrmlID);
						(*iA).cover = c;

					} else {
						printf("\n type not yet coded!!! Patience! ");
					}
				}
			}
		(*iA).myInfraStructure = iS;
		(*iS).addInfraSource(iA);
		this->myInfraStructure = iS;
	} else {
		printf("\n Another type of base tracking ain't coded yet. May the force be with you!");
	}

	//--------------------------------------------------------------------------
	// READ the visible sound
	//--------------------------------------------------------------------------
	
	getBuff(buf, 256, fp);	// -> Read visibleSound
	if (sscanf(buf, "%s ", &buf1) != 1) { printf("\n error Visible sound - Check %s file format", this->configFilename); fclose(fp); return  -1; }

	if( strcmp(buf1,"NO_VISIBLESOUND") == 0){ // NO_VISIBLESOUND
		printf("\n No visible base audio");
		this->visibleSound = 0;
	} else {
		if (sscanf(buf, "%s %s %lf", &buf1, &buf2, &auxVolume) != 2) { printf("\n error visible sound - Check %s file format", this->configFilename); fclose(fp); return  -1; }

		AudioArpe *vs = new AudioArpe();
		strcpy((*vs).filename,buf1);
		(*vs).volume = auxVolume;

		if(strcmp(buf2,"PAUSED")==0)	(*vs).status = 0;
		if(strcmp(buf2,"ONCE")==0)		(*vs).status = 1;
		if(strcmp(buf2,"LOOP")==0)		(*vs).status = 2;
		if(strcmp(buf2,"SILENCE")==0)	(*vs).status = 3;
		(*vs).status = 1;
		(*vs).myArpe = this->myArpe;
		(*vs).loadSound();

		this->visibleSound = vs;
		printf("\n Visible sound: %s",(*this->visibleSound).filename);
	}

	//--------------------------------------------------------------------------
	// READ the error sound
	//--------------------------------------------------------------------------

	getBuff(buf, 256, fp);
	if (sscanf(buf, "%s ", &buf1) != 1) { printf("\n error sound - Check %s file format", this->configFilename); fclose(fp); return  -1; }

	if( strcmp(buf1,"NO_ERRORSOUND") == 0){ 
		this->errorSound = 0;
		printf("\n No error audio");
	} else {
		if( strcmp(buf1,"USE_DEFAULT") == 0){
			this->errorSound = (*myArpe).myGenericItens.errorSound;
			printf("\n Using Default error sound");
		} else {
			if (sscanf(buf, "%s %lf", &buf1,&auxVolume) != 2) { printf("\n error sound - Check %s file format", this->configFilename); fclose(fp); return  -1; }

			AudioArpe *es = new AudioArpe();
			strcpy((*es).filename,buf1);
			(*es).volume = auxVolume;
			(*es).status = 1; //Play ONCE
			(*es).myArpe = this->myArpe;
			(*es).loadSound();

			if( (*es).audioSource == 0 ) {
				printf("\n Base error sound incorrect, using Generic error sound");
				this->errorSound = (*myArpe).myGenericItens.errorSound;
			}


			this->errorSound = es;
			printf("\n Base Error sound: %s",(*this->errorSound).filename);
		}
	}

	//--------------------------------------------------------------------------
	// Read Base Status
	//--------------------------------------------------------------------------
	getBuff(buf, 256, fp);
	if (sscanf(buf, "%s ", &fileDAT) != 1) { printf("\n Base Status - Check %s file format", this->configFilename); fclose(fp); return  -1; }
	iVrml* s = new iVrml();
	(*s).modelType = 1;
	(*s).vrmlID = arVrmlLoadFile(fileDAT); 
	if ((*s).vrmlID < 0) {
		printf("\n Error on Status object %s file or on VRML file (%d)",fileDAT,(*s).vrmlID);
		this->status = 0;
	} else {
		printf("\n Default Status object VRML id: %d ", (*s).vrmlID);
		this->status = s;
	}


	//--------------------------------------------------------------------------
	// READ number of ipoints
	//--------------------------------------------------------------------------
	getBuff(buf, 256, fp);
	if (sscanf(buf, "%d ", &nIPoints) != 1) {printf("\n number of ipoints - Check %s file format", this->configFilename); fclose(fp); return  -1; }
	printf("\n Number of iPoints: %d",nIPoints);

	//--------------------------------------------------------------------------
	// READ ipoints
	//--------------------------------------------------------------------------
	for(int i = 0; i < nIPoints; i++){

		iPoint* auxIP = new iPoint();

		//--------------------------------------------------------------------------
		// READ iPoint Name
		//--------------------------------------------------------------------------
		getBuff(buf, 256, fp);
		if (sscanf(buf, "%s", &(*auxIP).name) != 1) {printf("\n iPoint Name - Check %s file format", this->configFilename); fclose(fp); return  -1;}
		(*auxIP).id = i + 1;
		printf("\n Ipoint Name: %s , id: %d", (*auxIP).name, (*auxIP).id);

		//--------------------------------------------------------------------------
		// READ iPoint interactive model 
		//--------------------------------------------------------------------------
		getBuff(buf, 256, fp);
		if (sscanf(buf, "%s", &buf1) != 1) {printf("\n iPoint interactive model - Check %s file format", this->configFilename); fclose(fp); return  -1;}

		if( strcmp(buf1,"EXTERN_IPOINT") == 0) {
			// EXTERN IPOINT 
			(*auxIP).ball.activeBall = 1;
			(*auxIP).ball.holding = (*myArpe).myGenericItens.holding;
			(*auxIP).ball.canWork = (*myArpe).myGenericItens.canwork;
			(*auxIP).ball.cannotWork = (*myArpe).myGenericItens.cannotWork;			
			(*auxIP).type = 2;

			printf("\n Using Extern IPoint");

		}
		else {
			// INTERNAL IPOINT

			if( strcmp(buf1,"DEFAULT_IPOINT") == 0 ) {
				// Load generic iPoint
				(*auxIP).ball.activeBall = 1;
				(*auxIP).ball.holding = (*myArpe).myGenericItens.holding;
				(*auxIP).ball.canWork = (*myArpe).myGenericItens.canwork;
				(*auxIP).ball.cannotWork = (*myArpe).myGenericItens.cannotWork;
				(*auxIP).type = 1;
				printf("\n Using Default IPoint");
				

			} else {
				// LOAD Ipoints balls
				strcpy((*auxIP).ball.filename,buf1);
				(*auxIP).ball.ballReadFile();
				(*auxIP).type = 1;
				printf("\n Using %s ball file!",(*auxIP).ball.filename);
			}
		}





		//--------------------------------------------------------------------------
		// READ iPoint associated objects file
		//--------------------------------------------------------------------------
		getBuff(buf, 256, fp);
		if (sscanf(buf, "%s", &(*auxIP).objFilename) != 1) {printf("\n objects file - Check %s file format", this->configFilename); fclose(fp); return  -1;}
	
		if( strcmp((*auxIP).objFilename,"NO_OBJECT") == 0) {
			strcpy((*auxIP).objFilename,"");
			(*auxIP).activeObjectID = 0;
			(*auxIP).listObject.clear();}

		if( strcmp((*auxIP).objFilename,"STATUS")==0) {
			strcpy((*auxIP).objFilename,"");
			(*auxIP).activeObjectID = 1;
			printf("\n Status Point");
		}

		// IF EXTERN IPOINT, there's no object to assign.
		if ( (*auxIP).type == 2)  {
			(*auxIP).activeObjectID = 0;
			(*auxIP).listObject.clear();
		}

		//--------------------------------------------------------------------------
		// READ iPoint POSITIONS
		//--------------------------------------------------------------------------
		//Translations
		getBuff(buf, 256, fp);
		if( sscanf(buf, "%lf %lf %lf",	&(*auxIP).position.startTra[0], 
										&(*auxIP).position.startTra[1], 
										&(*auxIP).position.startTra[2]) != 3 ) {
	        printf("\n Translations - Check %s file format", this->configFilename); fclose(fp); return -1;  }

		(*auxIP).position.actualTra[0]=(*auxIP).position.startTra[0];
		(*auxIP).position.actualTra[1]=(*auxIP).position.startTra[1];
		(*auxIP).position.actualTra[2]=(*auxIP).position.startTra[2];

		//Rotations
		getBuff(buf, 256, fp);
		if( sscanf(buf, "%lf %lf %lf",	&(*auxIP).position.startRot[0], 
										&(*auxIP).position.startRot[1], 
										&(*auxIP).position.startRot[2]) != 3 ) {
	        printf("\n Rotations - Check %s file format", this->configFilename); fclose(fp); return -1;  }
		(*auxIP).position.actualRot[0]=(*auxIP).position.startRot[0];
		(*auxIP).position.actualRot[1]=(*auxIP).position.startRot[1];
		(*auxIP).position.actualRot[2]=(*auxIP).position.startRot[2];
		//printf(" R:(%3.2f,%3.2f,%3.2f)",(*auxIP).position.actualRot[0],(*auxIP).position.actualRot[1],(*auxIP).position.actualRot[2]);

		//Scales
		getBuff(buf, 256, fp);
		if( sscanf(buf, "%lf %lf %lf",	&(*auxIP).position.startScl[0], 
										&(*auxIP).position.startScl[1], 
										&(*auxIP).position.startScl[2]) != 3 ) {
	        printf("\n Scales - Check %s file format", this->configFilename); fclose(fp); return -1;  }	
		(*auxIP).position.actualScl[0]=(*auxIP).position.startScl[0];
		(*auxIP).position.actualScl[1]=(*auxIP).position.startScl[1];
		(*auxIP).position.actualScl[2]=(*auxIP).position.startScl[2];
		//printf(" S:(%3.2f,%3.2f,%3.2f)",(*auxIP).position.actualScl[0],(*auxIP).position.actualScl[1],(*auxIP).position.actualScl[2]);

		(*auxIP).position.bakeActualTrans();

		for(int ci = 0; ci<4;ci++)
			for(int cj = 0; cj<4;cj++){
				(*auxIP).position.startTrans[ci][cj] = (*auxIP).position.lastTrans[ci][cj] = (*auxIP).position.trans[ci][cj];
			}


		//--------------------------------------------------------------------------
		// READ iPoint action radius
		//--------------------------------------------------------------------------
		getBuff(buf, 256, fp);
		if( sscanf(buf, "%lf", &(*auxIP).ball.distCollision) != 1) { printf("\n action radius - Check %s file format", this->configFilename); fclose(fp); return -1;  }

		(*auxIP).myBase = this;
		this->addPoint(auxIP);
		printf("\n Ipoint %d - %s correctly added!",(*auxIP).id,(*auxIP).name);
	}

	printf("\n %s correctly read!",this->name);
	fclose(fp);
    return 1;
}

int Base::baseWriteFile()
{
    return 0;
}

int Base::verifyConsistency()
{
    return 0;
}

Base::~Base()
{
}

int Base::showBaseItens(){

	InfraStructure* infra = this->myInfraStructure;
	list<InfraSource*>::iterator iSource;
	double m[16];

	if(!this->myArpe->audioEngine->isCurrentlyPlaying(this->visibleSound->audioSource))	
		visibleSound->play2D();

	glPushMatrix();

	// DRAW MARKER COVER (if source is marker)
	// if a marker based base is used may have a marker cover to apply
	for( iSource = (*infra).listSource.begin(); iSource != (*infra).listSource.end(); iSource++){

		//if( (*(*iSource)).onUse)
			switch( (*(*iSource)).type ){
			case 1:{
				InfraARTKSM* b = static_cast<InfraARTKSM*>(*iSource);
					glPushMatrix();
					arglCameraViewRH((*this->myInfraStructure).baseTrans,m,VIEW_SCALEFACTOR_1);
					glLoadMatrixd(m);


				// DRAW MARKER COVER
				if( (*b).cover != 0)
					switch((*(*b).cover).modelType){
					case 1: { // VRML
						iVrml* model = static_cast<iVrml*>((*b).cover);
						(*model).draw();
						
						break;}
					case 2: { // Assimp
						break;}
					default: break;
				};

				glPopMatrix();
				break;}
			default: break;
		};
	}

	
	list<iPoint*>::iterator ip;

	for( ip = this->listPoint.begin(); ip != this->listPoint.end(); ip++){ //Search for iPoints
		if( (*ip)->type == 1){
			glPushMatrix();
				arglCameraViewRH((*this->myInfraStructure).baseTrans,m,VIEW_SCALEFACTOR_1);
				// DRAW IPOINTS
				glLoadMatrixd(m);	
				(*(*ip)).showBall();	

				// DRAW OBJECTS
				(*(*ip)).showObjects();

				glPopMatrix();
		}
	}

	glPopMatrix();

	return 1;
}