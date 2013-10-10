#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GL/glut.h>
#include <AR/ar.h>
#include <AR/arvrml.h>
#include <AR/gsub_lite.h>

#include "InfraARTKSM.h"
#include "InfraStructure.h"

InfraARTKSM::InfraARTKSM()
{
}

InfraARTKSM::~InfraARTKSM()
{
}


int InfraARTKSM::searchBaseOnFrame(ARMarkerInfo **marker_info, int marker_num){
	int             j, k;
	ARMarkerInfo           *auxMarker_info;
	auxMarker_info = *marker_info;


	k = -1;
	for (j = 0; j < marker_num; j++) {
		if (auxMarker_info[j].id == this->patternNumber) {
			if( k == -1 ) k = j; // First marker detected.
			else if (auxMarker_info[k].cf < auxMarker_info[j].cf) k = j; } 
	}// Higher confidence marker detected.

	if (k != -1) {
		// Get the transformation between the marker and the real camera.
		//fprintf(stderr, "Saw object %d.\n", (*itObject).objectID);
		if (this->visible == 0) {
			arGetTransMat(&auxMarker_info[k], this->markerCenter, this->markerWidth, this->markerTrans );
		} else {
			arGetTransMatCont(&auxMarker_info[k], this->markerTrans , this->markerCenter, this->markerWidth, this->markerTrans);
		}

		this->visible = 1;
		//Updatedata
		(*this->myInfraStructure).updateInfraData(this->markerTrans,this->visible); //Acquire the latest infra data and update on basetrans
		//printf("\n Found Base ARTKSM");	
		return TRUE;
		
	} else { 
		this->visible = 0; 
		(*this->myInfraStructure).visible = 0;
		return 2;

	}


}
