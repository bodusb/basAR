#ifndef Actuator_h
#define Actuator_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "iPoint.h"

class Arpe;

class Actuator {

 public:

    void updateTrans( double t[3][4]);

    void updateButton0( int value);

	char getBuff(char *buf, int n, FILE *fp);
	//virtual int actuatorReadFile() const = 0;

	int grabPoint(iPoint *value);
	int releasePoint();

    Actuator();
    ~Actuator();


    int id;
    int name[256];
    char configFilename[256];
    int type; //1 = ARTKSM
    bool onUse;
    double interactionTrans[3][4];
	double transDiff[3][4];
    int buttons[20];
    double distCollision;
	int transporting;
	iPoint *transportingPoint;


    Arpe *myArpe;
};

#endif // Actuator_h
