#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Actuator.h"

char Actuator::getBuff(char *buf, int n, FILE *fp)
{
    char *ret;
	
    for(;;) {
        ret = fgets(buf, n, fp);
        if (ret == NULL) return(NULL);
        if (buf[0] != '\n' && buf[0] != '#') return(1); // Skip blank lines and comments.
    }
}


void Actuator::updateTrans(double t[3][4])
{
	

	for(int i=0; i<3 ; i++)
		for(int j=0; j<4 ; j++)
			this->interactionTrans[i][j] = t[i][j];
}

void Actuator::updateButton0( int value)
{
	this->buttons[0] = value;
}

Actuator::Actuator()
{
	this->transporting = 0;
	this->transportingPoint = NULL;
	for(int i= 0 ; i<20;i++) this->buttons[i] = 0;
}

Actuator::~Actuator()
{
}

int Actuator::grabPoint(iPoint *value){

	this->transporting = 1;
	this->transportingPoint = value;
	return 1;
}

int Actuator::releasePoint(){
	this->transporting = 0;
	this->transportingPoint = 0;
	return 1;
}