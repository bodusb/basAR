#include "Ball.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <AR/ar.h>
#include <AR/gsub.h>
#include <AR/gsub_lite.h>
#include <AR/arvrml.h>

#include "iVrml.h"
#include "iObject3D.h"

Ball::Ball(){
	this->cannotWork = 0;
	this->canWork = 0;
	this->holding = 0;
	this->distCollision = 0;
	this->senseStatus = 2;
	this->activeBall = 0;
	this->flashingTime = 0;
}

Ball::~Ball(){
}

char Ball::getBuff(char *buf, int n, FILE *fp){
    char *ret;
	
    for(;;) {
        ret = fgets(buf, n, fp);
        if (ret == NULL) return(NULL);
        if (buf[0] != '\n' && buf[0] != '#') return(1); // Skip blank lines and comments.
    }
}

int Ball::ballReadFile(){
	
	FILE          *fp;
	char           buf[256],buf1[256],buf2[256],fileDAT[256];
	int			   numObjects;

	//--------------------------------------------------------------------------
	// Open the Ball Configuration file
	//--------------------------------------------------------------------------
	printf("\n --------------------------------------------------------------------------");
	printf("\n Setting up action point balls file %s", this->filename);
	printf("\n --------------------------------------------------------------------------");

	if( strcmp(this->filename,"") == 0) {
		printf("\n Any balls will be assigned to this action point"); 
		return 0;
	}

	if( (fp=fopen(this->filename,"r")) == NULL) { printf("\n Error on opening %s action point balls file!! ",this->filename);	return 0;}

	//--------------------------------------------------------------------------
	// Read the amount of models
	//--------------------------------------------------------------------------
	getBuff(buf,256,fp);
    if (sscanf(buf, "%d", &numObjects) != 1) {
		 printf("\n Check %s file format", this->filename);fclose(fp); return(0);
	}
	printf("\n About to load %d balls.", numObjects);

	//Looping para ler todos os modelos do iPoint
    for (int i = 0; i < numObjects; i++) {
		//--------------------------------------------------------------------------
		// Read the objects
		//--------------------------------------------------------------------------
        getBuff(buf, 256, fp);
		if (sscanf(buf, "%s", &buf1) != 1) { printf("\n Check %s file format", this->filename); fclose(fp); return(0); }
		
		if ( strcmp(buf1,"MODEL3D") == 0 ){
			if (sscanf(buf, "%s %s %s", &buf1, &buf2, &fileDAT) != 3) { printf("\n Check %s file format", this->filename); fclose(fp); return(0); }
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
					printf("\n Ball %s VRML id: %d ", (*obj).filename, (*obj).vrmlID);

					switch(i){
					case 0:{ // Holding
							this->holding = obj;
							break;}
					case 1:{ // Canwork
							this->canWork = obj;
							break;}
					case 2:{ // Cannotwork
							this->cannotWork = obj;
							break;}
					default: {
							printf("\n The file %d will be ignore", fileDAT);
							break;}
					};


				}

			} else {
				printf("\n This type of object (%s-%s) is not coded on Matrix yet.",buf1,buf2);
			}
		} else { 
			printf("\n This type of object (%s) is not coded on Matrix yet.",buf1);
		}
	
	}

	printf("\n %d Balls correctly read!",numObjects);
	fclose(fp);

	return 1;
}