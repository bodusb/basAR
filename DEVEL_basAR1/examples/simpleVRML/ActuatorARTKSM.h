#ifndef ActuatorARTKSM_h
#define ActuatorARTKSM_h

#include "Actuator.h"
#include "iObject3D.h"
#include <AR/ar.h>


class ActuatorARTKSM : public Actuator {

 public:

	int actuatorReadFile();
	int showActuatorItens();

	int searchActuatorOnFrame(ARMarkerInfo **marker_info, int marker_num);

    ActuatorARTKSM();

    ~ActuatorARTKSM();

    bool visible;
    int patternNumber;
    
    double markerCoord[4][2];
    double markerTrans[3][4];
    double markerWidth;
    double markerCenter[2];

	iObject3D* cover;
    iObject3D* symbol;
    iObject3D* interactionPoint;
    double ipTra[3];
};

#endif // ActuatorARTKSM_h
