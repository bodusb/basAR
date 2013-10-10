#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include <GL/glut.h>

#include <AR/ar.h>
#include <AR/gsub.h>
#include <AR/gsub_lite.h>
#include <AR/arvrml.h>


#include "iPuzzle.h"

#include "iARTKMarker.h"


using namespace std;

iPuzzle::iPuzzle(void)
{
	this->a_engine = createIrrKlangDevice();
	g_corrects = 0;
	g_mistakes = 0;
	strcpy_s(this->m_puzzleFile,"Data/config_puzzle");
}


iPuzzle::~iPuzzle(void)
{


}

char iPuzzle::getBuff(char *buf, int n, FILE *fp)
{
    char *ret;
	
    for(;;) {
        ret = fgets(buf, n, fp);
        if (ret == NULL) return(NULL);
        if (buf[0] != '\n' && buf[0] != '#') return(1); // Skip blank lines and comments.
    }
}

int iPuzzle::puzzleFill(){
	FILE        *fp,*fp2;
    char        buf[256],buf1[256],buf2[256],fileModel[256];
	int			tempActType;
	errno_t		err;
	
	//Try to open the config_puzzle file
	if((err = fopen_s(&fp,this->m_puzzleFile,"r")) != 0 ) {			
		printf("\n ERROR opening the puzzle configuration file!! Verify name and location! (Should be Data/config_puzzle)");
		return 0;}
	printf("\n ************************************************************");
	printf("\n -> Opening file: %s",this->m_puzzleFile);
	//************************************************************
	//	Read application name
	//************************************************************
	getBuff(buf, 256, fp);
	if(sscanf(buf, "%s", &this->m_appName) != 1){ fclose(fp); return 0;} 
	printf("\n Application Name: %s",this->m_appName);
	//************************************************************
	// Read SoundTrackAudio
	//************************************************************
	getBuff(buf, 256, fp);
	if(sscanf(buf, "%s", &buf1) != 1){ fclose(fp); return 0;} 
	if( strcmp(buf1,"NO_SOUNDTRACK") != 0){
		// HAS SOUNDTRACK
		if(sscanf(buf, "%s %s %lf", &this->a_soundTrack_filename,&buf2,&this->a_soundTrack_volume) != 3) {
				printf("\n Please correct soundtrack setup"); fclose(fp);	return 0;}
		this->a_soundTrack = (this->a_engine)->addSoundSourceFromFile(this->a_soundTrack_filename);
		// ACQUIRE EXECUTION MODE
			if( strcmp(buf2,"LOOP") == 0) { this->a_soundTrack_status = 3;} // 3 audio loop
			if( strcmp(buf2,"ONCE") == 0) {	this->a_soundTrack_status = 2;} // 2 audio play once
			if( strcmp(buf2,"QUIET") == 0) { this->a_soundTrack_status = 1;} // 1 audio not playing
		// SET VOLUME
			if (this->a_soundTrack) this->a_soundTrack->setDefaultVolume(this->a_soundTrack_volume);
			printf("\n Soundtrack file: %s on %s, with volume: %3.2f",this->a_soundTrack_filename,buf2,this->a_soundTrack_volume);
	} else {
		// DON`T HAS SOUNDTRACK
			printf("\n No Soundtrack audio is avaiable.");
			this->a_soundTrack = 0;
			this->a_soundTrack_status = 0;} // 0 no audio
	//************************************************************
	// Read StartAudio
	//************************************************************
	getBuff(buf, 256, fp);
	if(sscanf(buf, "%s", &buf1) != 1){ fclose(fp); return 0;} 
	if( strcmp(buf1,"NO_STARTSOUND") != 0){
		// HAS SOUNDTRACK
			if(sscanf(buf, "%s %s %lf", &this->a_start_filename,&buf2,&this->a_start_volume) != 3) {
				printf("\n Please correct soundtrack setup"); fclose(fp);	return 0;}
			this->a_start = (this->a_engine)->addSoundSourceFromFile(this->a_start_filename);
		// ACQUIRE EXECUTION MODE
			if( strcmp(buf2,"LOOP") == 0) {	this->a_start_status = 3;} // 3 audio loop
			if( strcmp(buf2,"ONCE") == 0) {	this->a_start_status = 2;} // 2 audio play once
			if( strcmp(buf2,"QUIET") == 0) { this->a_start_status = 1;} // 1 audio not playing
		// SET VOLUME
			if (this->a_soundTrack) this->a_soundTrack->setDefaultVolume(this->a_soundTrack_volume);
			printf("\n Start audio file: %s on %s, with volume: %3.2f",this->a_start_filename,buf2,this->a_start_volume);
	} else {
		// DON`T HAS SOUNDTRACK
		printf("\n No openning audio is avaiable.");
			this->a_start = 0;
			this->a_start_status = 0;} // 0 no audio
	//************************************************************
	// Read rule configuration file
	//************************************************************
	getBuff(buf, 256, fp);
	if(sscanf(buf, "%s", &this->rules.rule_filename) != 1){ fclose(fp); return 0;} 
	printf("\n Rules file: %s. ",this->rules.rule_filename);

	//************************************************************
	// Read Number of bases
	//************************************************************
	getBuff(buf, 256, fp);
	if(sscanf(buf, "%d", &this->n_bases) != 1){ fclose(fp); return 0;} 
	printf("\n Bases to assign: %d. ",this->n_bases);
	//************************************************************
	// Loop to read the amount of bases
	//************************************************************
	for(int iB = 1; iB < this->n_bases + 1; iB++){
		getBuff(buf, 256, fp);
		iBase newBase;
		if(sscanf(buf, "%s", &newBase.m_filename) != 1){ fclose(fp); return 0;}
		newBase.m_id = iB;
		this->addBase(newBase);
		printf("(%d, %s), ", newBase.m_id, newBase.m_filename);
	}
	//************************************************************
	// Read Number of actuators
	//************************************************************
	getBuff(buf, 256, fp);
	if(sscanf_s(buf, "%d", &this->n_actuators) != 1){ fclose(fp); return 0;} 
	printf("\n Actuators to assign: %d. ",this->n_actuators);
	//************************************************************
	// Loop to read the actuators
	//************************************************************
	for(int iA = 1; iA < this->n_actuators + 1; iA++){
		getBuff(buf, 256, fp);
		if(sscanf(buf, "%s %s", &buf1, &buf2) != 2){ fclose(fp); return 0;}
		
		if(strcmp(buf1,"ARTKM") == 0) {tempActType = 1;}

		switch(tempActType){
		case 1:{
			iARTKMarker newAct;
			newAct.m_id = iA;
			strcpy(newAct.m_configFile,buf2);
			newAct.m_type = 1;

			//************************************************************
			// OPEN ARTKMARKER Configuration file
			//************************************************************
			if((err = fopen_s(&fp2,newAct.m_configFile,"r")) != 0 ) {			
				printf("\n ERROR opening the puzzle configuration file!! Verify name and location! (Should be Data/config_puzzle)");
				return 0;}
			printf("\n ************************************************************");
			printf("\n -> Opening file: %s",newAct.m_configFile);
			//************************************************************
			// Read collision distance
			//************************************************************
			getBuff(buf, 256, fp2);
			if(sscanf_s(buf, "%d", &newAct.m_collisionDist) != 1){ fclose(fp2); return 0;} 
			//************************************************************
			// Read marker pattern 
			//************************************************************
			getBuff(buf, 256, fp2);
			if(sscanf(buf, "%s", &buf1) != 1){ fclose(fp); return 0;} 
			if ((newAct.m_patternID = arLoadPatt(buf1)) < 0) { fclose(fp2);  return(0);	}
			//************************************************************
			//	Read marker size
			//************************************************************
			getBuff(buf, 256, fp2);
			if (sscanf(buf, "%lf", &newAct.m_mkr_width) != 1) { fclose(fp2); return(0);  }
			//************************************************************
			//	Read marker center
			//************************************************************
			getBuff(buf, 256, fp2);
			if (sscanf(buf, "%lf %lf", &newAct.m_mkr_center[0], &newAct.m_mkr_center[1]) != 2) 
				{ fclose(fp2); return(0); }
			//************************************************************
			//	Read marker cover
			//************************************************************
			getBuff(buf, 256, fp2);
			if (sscanf(buf, "%s %s", &buf1, &fileModel) != 2) {fclose(fp2); return(0); }
			//CALL VRML LOADER TO VRML MARKER COVER
			if (strcmp(buf1, "VRML") == 0) {
				newAct.m_cover_VRML_ID = arVrmlLoadFile(fileModel);
				printf("\n Marker Cover VRML id: %d ", newAct.m_cover_VRML_ID);
				if (newAct.m_cover_VRML_ID < 0) {fclose(fp2); return(0);	}
			} else {
				//OTHER TYPE OF FILE TO COVER
				newAct.m_cover_VRML_ID = -1;
			}
			//************************************************************
			//	Read abstract representation
			//************************************************************
			getBuff(buf, 256, fp2);
			if (sscanf(buf, "%s %s", &buf1, &fileModel) != 2) {fclose(fp2); return(0); }

			//CALL VRML LOADER TO VRML MARKER COVER
			if (strcmp(buf1, "VRML") == 0) {
				newAct.m_rep_VRML_ID = arVrmlLoadFile(fileModel);
				printf("\n Abstract Representation VRML id: %d ", newAct.m_rep_VRML_ID);
				if (newAct.m_rep_VRML_ID < 0) {fclose(fp2); return(0);	}
			} else {
				//OTHER TYPE OF FILE TO COVER
				newAct.m_rep_VRML_ID = -1;
			}		
			//************************************************************
			//	Read actionPoint representation
			//************************************************************
			getBuff(buf, 256, fp2);
			if (sscanf(buf, "%s %s", &buf1, &fileModel) != 2) {fclose(fp2); return(0); }
			//CALL VRML LOADER TO VRML MARKER COVER
			if (strcmp(buf1, "VRML") == 0) {
				newAct.m_action_VRMLID = arVrmlLoadFile(fileModel);
				printf("\n ActionPoint Representation VRML id: %d ", newAct.m_action_VRMLID);
				if (newAct.m_action_VRMLID < 0) {fclose(fp2); return(0);	}
			} else {
				//OTHER TYPE OF FILE TO COVER
				newAct.m_action_VRMLID = -1;
			}		
			//************************************************************
			//	Read actionPoint representation translation
			//************************************************************
			getBuff(buf, 256, fp2);
			if( sscanf(buf, "%lf %lf %lf",	&newAct.m_action_tra[0], 
											&newAct.m_action_tra[1], 
											&newAct.m_action_tra[2]) != 3 ) {
				fclose(fp2); return -1;  }
			//************************************************************
			//	Read actionPoint representation scale
			//************************************************************
			getBuff(buf, 256, fp2);
			if( sscanf(buf, "%lf %lf %lf",	&newAct.m_action_scl[0], 
											&newAct.m_action_scl[1], 
											&newAct.m_action_scl[2]) != 3 ) {
				fclose(fp2); return -1;  }

			newAct.m_active = 1;

			this->addARTKMarkerActuator(newAct);
			printf("\n Correctly Assigned Actuator: (%d, %s, %s), ", newAct.m_id, newAct.m_configFile, buf1);

			fclose(fp2);
			break;}
		default: 
			printf(" \n This type of Actuator is not avaiable... yet!!!!" );
			break;
		}



	}
	fclose(fp);
	return 1;
}

int iPuzzle::automaticFill(){
	int ipointCounter = 1;

	printf("\n AUTOMATIC SETUP");

	this->puzzleFill();

	list<iBase>::iterator itBase;

	for(itBase = this->listBase.begin(); itBase != this->listBase.end(); itBase++){
		ipointCounter += (*itBase).baseFill(ipointCounter);

	}
	printf("\n ************************************************************");
	ipointCounter -=1;
	printf("\n Total of iBases: %d",this->n_bases);
	printf("\n Total of iPoints: %d",ipointCounter);
	this->rules.loadRules();

	// START ALL SOUNDSOURCES

	// SETUP ACTUATOR

	// CHECK ALL RULES


	return 0;
}

void iPuzzle::addBase(iBase value) { 
	
	list<iBase>::iterator it;
	it = this->listBase.begin();
	listBase.push_back(value);

}

void iPuzzle::addARTKMarkerActuator(iARTKMarker value) { 
	
	list<Actuator>::iterator it;
	it = this->listActuator.begin();
	listActuator.push_back(value);

}



int iPuzzle::findIPointNearToCollide( float *distance, double actTrans[3][4]){

	list<iBase>::iterator itBase;
	list<iPoint>::iterator itPoint;
	float xPto,yPto,zPto, dist;
	double matInv[3][4], matResult[3][4];

	for(itBase = this->listBase.begin() ; itBase != this->listBase.end() ; itBase++){

		arUtilMatInv((*itBase).position.m_base_trans,matInv);
		arUtilMatMul(matInv,actTrans,matResult);

		for(itPoint = (*itBase).listPoint.begin() ; itPoint != (*itBase).listPoint.end() ; itPoint++){
			// Varrendo todos os pontos de interesse e verificando qual eh o mais proximo do ponto de interesse da pa
			
			xPto =  (*itPoint).actionPosition.m_actualTra[0] - matResult[0][3];
			yPto =  (*itPoint).actionPosition.m_actualTra[1] - matResult[1][3];
			zPto =  (*itPoint).actionPosition.m_actualTra[2] - matResult[2][3];

			dist = xPto*xPto + yPto*yPto + yPto*zPto;

			if( (dist > 0 ) && (dist < (*itPoint).m_distCollision)){
				    *distance = dist;
					return (*itPoint).m_id;
			}
		}
	}
		return 0;
}


int iPuzzle::interactionControl(){
	
	//************************************************************
	//	PARSE RULES AND FLAGS
	//************************************************************
	list<iRulesState>::iterator itState;
	list<iRulesInput>::iterator itInput;
	list<iPoint>::iterator itPoint;

	//TEST IF STATE ASKED IS VALID
	if((this->rules.m_actualState > 0) &&  (this->rules.m_actualState <= this->rules.n_states)){	
		//SEARCH FOR THE STATE
		for(itState = this->rules.listState.begin(); itState != this->rules.listState.end(); itState++)
			if( (*itState).m_id == this->rules.m_actualState ){

			//Parse state inputs to ipoint
				for(itInput = (*itState).listInput.begin() ; itInput != (*itState).listInput.end() ; itInput++){

					itPoint	= this->findIPoint((*itInput).m_iPoint);

					(*itPoint).m_opcode = (*itInput).m_opcode;
					(*itPoint).m_pointMode = (*itInput).m_pointMode;
					(*itPoint).m_nextState = (*itInput).m_nextState;
					(*itPoint).m_pointWaited = (*itInput).m_pointWaited;
					(*itPoint).m_modelToChange = (*itInput).m_modelToChange;
					(*itPoint).m_generateError = (*itInput).m_generateError;
					strcpy((*itPoint).a_action_filename,(*itInput).a_action_filename);
					(*itPoint).a_overplay = (*itInput).a_overplay;

				}

				(*itState).m_active = 1;
				break;

			}

	
	//TRY FLAGS AND DO ATTITUDES	
	
			list<Actuator>::iterator itActuator;
			//************************************************************
			//	Search with actuator is active and test the interaction that it is doing
			//************************************************************
			for(itActuator = this->listActuator.begin(); itActuator != this->listActuator.end(); itActuator++){

			//************************************************************
			//	Attitude check for a marker type actuator
			//************************************************************
				switch((*itActuator).m_transporting){
					case 0:{ // It`s not transporting anything.
							switch((*itActuator).m_visible){
								case 1:	// Actuator is visible on the infra-structure
								case 0: // Actuator isn`t visible on the infra-structure
								default: break;}
							break;}
					case 1:{ // Transporting Something
							switch((*itActuator).m_visible){
								case 1:	// Actuator is visible on the infra-structure
								case 0: // Actuator isn`t visible on the infra-structure
								default: break;}
							break;}
					default: break;
				}



			}




	
	
	
	
	
	
	
	
	
	
	
	
	}





	return 0;
}


list <iPoint>::iterator iPuzzle::findIPoint(int value){

	list<iBase>::iterator it1;
	list<iPoint>::iterator it2;

	for(it1 = this->listBase.begin(); it1 != this->listBase.end();it1++)
		for(it2 = (*it1).listPoint.begin(); it2 != (*it1).listPoint.end();it2++)
			if((*it2).m_id == value) return it2;

	printf("*******iPoint not found");
}