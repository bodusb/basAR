#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include <GL/glut.h>
#include <AR/ar.h>
#include <AR/arvrml.h>
#include <AR/gsub_lite.h>

#include "Actuator.h"
#include "ActuatorARTKSM.h"
#include "Arpe.h"
#include "iVrml.h"
#include "iObject3D.h"

#define VIEW_SCALEFACTOR_1		1.0			// 1.0 ARToolKit unit becomes 1.0 of my OpenGL units.


ActuatorARTKSM::ActuatorARTKSM()
{
	this->transporting = false;
	this->transportingPoint = 0;
	this->onUse = true;
}

ActuatorARTKSM::~ActuatorARTKSM()
{
}

int ActuatorARTKSM::actuatorReadFile(){
	FILE          *fp;
	char		  buf[256],buf1[256],fileDAT[256];

	//--------------------------------------------------------------------------
	// Open the ActuatorARTKSM Configuration file
	//--------------------------------------------------------------------------
	printf("\n --------------------------------------------------------------------------");
	if( (fp=fopen(this->configFilename,"r")) == NULL) {
		printf("\n Error on opening %s !! ",this->configFilename);
		return  -1;
	}
	printf("\n Opening file %s ,  ARTKSM Actuator", this->configFilename);
	printf("\n --------------------------------------------------------------------------");

	//--------------------------------------------------------------------------
	// READ THE Actuator Name
	//--------------------------------------------------------------------------
	getBuff(buf, 256, fp);
	if (sscanf(buf, "%s", &this->name) != 1) { printf("\n Check %s file format", this->configFilename); fclose(fp); return -1;	}
	printf("\n Actuator name: %s", this->name);

	//--------------------------------------------------------------------------
	// READ THE MARKER CONFIGURATION
	//--------------------------------------------------------------------------
	// Marker that defines the actuator
	getBuff(buf, 256, fp);
	if (sscanf(buf, "%s", &buf1) != 1) { printf("\n Check %s file format", this->configFilename); fclose(fp); return -1;	}
	if ((this->patternNumber = arLoadPatt(buf1)) < 0) { fclose(fp);  return(0);	}
	printf("\n Using marker: %s, id: %d", buf1, this->patternNumber); 
	// Marker Width
	getBuff(buf, 256, fp);
	if (sscanf(buf, "%lf", &this->markerWidth) != 1) { printf("\n Check %s file format", this->configFilename);fclose(fp); return -1;  }
	// Marker Center
	getBuff(buf, 256, fp);
	if (sscanf(buf, "%lf %lf", &this->markerCenter[0], &this->markerCenter[1]) != 2) {
			printf("\n Check %s file format", this->configFilename); fclose(fp); return(0);  }
	printf(" W: %3.2f, Center(%3.2f,%3.2f)", this->markerWidth , this->markerCenter[0], this->markerCenter[1]);
	// Marker cover (USE_DEFAULT to get default marker cover / NO_COVER to don't use a marker cover)
	getBuff(buf, 256, fp);
	if (sscanf(buf, "%s %s", &buf1, &fileDAT) != 2) { printf("\n Check %s file format", this->configFilename); fclose(fp); return -1; }
	
	if( strcmp(buf1, "NO_COVER") == 0){
		this->cover = 0;
		printf("\n Actuator Marker without cover.");
	}
	else{

		if ( strcmp(buf1, "USE_DEFAULT") == 0) { 
			this->cover = (*myArpe).myGenericItens.markCover;
			printf("\n Using Default Cover ");
		}
		else{
			if (strcmp(buf1, "VRML") == 0) {
				//VRML Object
				iVrml *c = new iVrml();
				(*c).modelType = 1;
				(*c).vrmlID = arVrmlLoadFile(fileDAT); 
				if ((*c).vrmlID < 0) {
					printf("\n Error on Marker Base Cover %s file or on VRML file (%d)",fileDAT,(*c).vrmlID);
					this->cover = 0;
				} else { 
					printf("\n Cover object VRML id: %d ", (*c).vrmlID); 
					this->cover = c;}
				
			} else {
				printf("\n type not yet coded!!! Patience! ");
			}
		}
	}
	//--------------------------------------------------------------------------
	// READ THE Symbolic
	//--------------------------------------------------------------------------
	getBuff(buf, 256, fp);
	if (sscanf(buf, "%s %s", &buf1, &fileDAT) != 2) { printf("\n Check %s file format", this->configFilename); fclose(fp); return -1; }
	if (strcmp(buf1, "VRML") == 0) {
		//VRML Object
		iVrml *s = new iVrml();
		(*s).modelType = 1;
		(*s).vrmlID = arVrmlLoadFile(fileDAT); 
		if ((*s).vrmlID < 0) {
			printf("\n Error on Symbolic object %s file or on VRML file (%d)",fileDAT,(*s).vrmlID);
			this->symbol = 0;
		} else { 
			printf("\n Symbolic object VRML id: %d ", (*s).vrmlID); 
			this->symbol = s;}
	} else {
		printf("\n type not yet coded!!! Patience! ");
	}

	//--------------------------------------------------------------------------
	// READ Point were the action occors on the ARTSM Actuator
	//--------------------------------------------------------------------------
	// Point model
	getBuff(buf, 256, fp);
	if (sscanf(buf, "%s %s", &buf1, &fileDAT) != 2) { printf("\n Check %s file format", this->configFilename); fclose(fp); return -1; }

	if ( strcmp(buf1, "DEFAULT_IPOINT") == 0) { 
		this->interactionPoint = (*myArpe).myGenericItens.holding;
		printf("\n Using Default Interaction Point Representation.");
	}
	else{

		if (strcmp(buf1, "VRML") == 0) {
			//VRML Object
			iVrml *ip = new iVrml();
			(*ip).modelType = 1;
			(*ip).vrmlID = arVrmlLoadFile(fileDAT); 
			printf("\n Point model object VRML id: %d ", (*ip).vrmlID);
			if ((*ip).vrmlID < 0) {
				printf("\n Error on Point model object %s file or on VRML file (%d)",fileDAT,(*ip).vrmlID);
				this->interactionPoint = 0;
			} else { 
				printf("\n Point model object VRML id: %d ", (*ip).vrmlID); 
				this->interactionPoint = ip;}
			if ((*ip).vrmlID < 0) { fclose(fp); return(0); }
			
		} else {
			printf("\n type not yet coded!!! Patience! ");
		}
	}
	// Translation (x,y,z) (mm)
	getBuff(buf, 256, fp);
	if( sscanf(buf, "%lf %lf %lf",	&this->ipTra[0], &this->ipTra[1], &this->ipTra[2]) != 3 ) {
			printf("\n Check %s file format", this->configFilename);fclose(fp); return -1;  }
	// Action radius of the point
	getBuff(buf, 256, fp);
	if (sscanf(buf, "%lf", &this->distCollision) != 1) { printf("\n Check %s file format", this->configFilename); fclose(fp); return -1; }
	printf("\n Point position from center (%3.2f,%3.2f,%3.2f), radius: %3.2f", 
		this->ipTra[0], this->ipTra[1], this->ipTra[2], this->distCollision);

	fclose(fp);
	
	return 0;
}

int ActuatorARTKSM::showActuatorItens(){
	GLdouble m[16];

	glPushMatrix();
		glLoadIdentity();
		arglCameraViewRH(this->markerTrans,m,VIEW_SCALEFACTOR_1);
		glLoadMatrixd(m);
		
		// DRAW MARKER COVER
		if( cover != 0)
			switch((*cover).modelType){
			case 1: { // VRML
				iVrml* model = static_cast<iVrml*>(cover);
				(*model).draw();
				break;}
			case 2: { // Assimp
				break;}
			default: break;
		};

		// DRAW SYMBOLIC OBJECT
		if( symbol != 0)
			switch((*symbol).modelType){
			case 1: { // VRML
				iVrml* model = static_cast<iVrml*>(symbol);
				(*model).draw();
				break;}
			case 2: { // Assimp
				break;}
			default: break;
		};

		// DRAW INTERACTION POINT OBJECT
		glTranslated(this->ipTra[0],this->ipTra[1],this->ipTra[2]);
		if( this->interactionPoint != 0)
			switch((*interactionPoint).modelType){
			case 1: { // VRML
				iVrml* model = static_cast<iVrml*>(interactionPoint);
				(*model).draw();
				break;}
			case 2: { // Assimp
				break;}
			default: break;
		};

	glPopMatrix();

	return 1;

}


int ActuatorARTKSM::searchActuatorOnFrame(ARMarkerInfo **marker_info, int marker_num){

	bool	cdChanged = false;
	int		cd=100;
	int		toggleBase = 0;
	int		j, k;

		//SACRA ACOCHAMBRO
//	double	m1Trans[3][4];
//	double	m2Trans[3][4];
//	double	mResult[3][4]; 
//	double	matInv[3][4];

	static	double diff[3][4];
	static	int matInit=1;

	ARMarkerInfo           *auxMarker_info;

	auxMarker_info = *marker_info;


				k = -1;
				for (j = 0; j < marker_num; j++) {
					if (auxMarker_info[j].id == this->patternNumber) {
						if( k == -1 ) k = j; // First marker detected.
						else if (auxMarker_info[k].cf < auxMarker_info[j].cf) k = j; }
				}

				if (k != -1) {
					if (this->visible == 0) {
						arGetTransMat(&auxMarker_info[k], this->markerCenter, this->markerWidth, this->markerTrans);

						//if(matInit || cdChanged){
						//	for (int u=0;u<3;u++)
						//		for (int v=0;v<4;v++){ m1Trans[u][v]=0.0; m2Trans[u][v]=0.0; }

						//		m1Trans[0][0] = m2Trans[0][0] = 1.0;
						//		m1Trans[1][1] = m1Trans[2][2] = -1.0;
						//		m2Trans[1][1] = m2Trans[2][2] = -1.0;
						//		arUtilMatInv(m1Trans, matInv);
						//		arUtilMatMul(matInv,m2Trans,diff);
						//		matInit = 0;
						//		cdChanged=false;
						//}

					} else { 
						arGetTransMatCont(&auxMarker_info[k], this->markerTrans, this->markerCenter, this->markerWidth, this->markerTrans);
					}
					this->visible = 1;
				/*	arUtilMatMul(this->markerTrans,diff,mResult);
					for(int u=0;u<3;u++)
						for(int v=0;v<4;v++)
							this->markerTrans[u][v] = mResult[u][v];*/

					this->updateTrans( this->markerTrans );
					this->updateButton0( this->visible );
					//printf("\n Found Actuator %d %s", (*a).id, (*a).name);
					return TRUE;
				} else { 
					this->visible = 0; 
					this->updateButton0( this->visible);
					return 2;
				}
}
