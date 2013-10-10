#ifndef Ball_h
#define Ball_h

#include "iObject3D.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


class Ball {

 public:
	Ball();
    ~Ball();
	int ballReadFile();

    double distCollision;
    iObject3D* holding;
    iObject3D* canWork;
    iObject3D* cannotWork;
    int activeBall;
	int senseStatus; // 2 - HOLDING / 3 - CANWORK / 4 - CANNOT WORK 
	int flashingTime;

	char filename[256];

 private:
	char getBuff(char *buf, int n, FILE *fp);
	
};

#endif // Ball_h
