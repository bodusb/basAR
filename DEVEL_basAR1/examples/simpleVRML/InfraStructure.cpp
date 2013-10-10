#include "InfraStructure.h"

#include <list>

using namespace std;

#include "InfraSource.h"


int InfraStructure::updateInfraData(double t[3][4], int v){
	for(int i=0; i<3 ; i++)
		for(int j=0; j<4 ; j++)
			this->baseTrans[i][j] = t[i][j];

	this->visible = v;

    return 1;
}

InfraStructure::InfraStructure()
{
}

InfraStructure::~InfraStructure()
{
}

void InfraStructure::addInfraSource(InfraSource* value){
	list<InfraSource*>::iterator it;
	it = this->listSource.begin();
	this->listSource.push_back(value);
}



