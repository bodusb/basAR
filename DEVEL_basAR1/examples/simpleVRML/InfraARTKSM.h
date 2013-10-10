#ifndef InfraARTKSM_h
#define InfraARTKSM_h

#include "InfraSource.h"
#include "iObject3D.h"
#include <AR/ar.h>

class InfraARTKSM : public InfraSource {

 public:

    InfraARTKSM();

    ~InfraARTKSM();

	int searchBaseOnFrame(ARMarkerInfo **marker_info, int marker_num);

 
    bool visible;
    int patternNumber;
    iObject3D *cover;
    double markerCoord[4][2];
    double markerTrans[3][4];
    double markerWidth;
    double markerCenter[2];
};

#endif // InfraARTKSM_h
