#ifndef Base_h
#define Base_h

#include <list>
using namespace std;

#include "AudioArpe.h"
#include "iPoint.h"
#include "iVrml.h"

class InfraStructure;
class Arpe;

class Base {

 public:
    Base();
	~Base();

	//base configuration
	char getBuff(char *buf, int n, FILE *fp);
    int baseReadFile();
    int baseWriteFile();
	int id;
    char name[256];
    char configFilename[256];
	int verifyConsistency();
	int			adaptType; // Adaptation type depends on the User Profile.

	// Show base Itens
	int showBaseItens();

	//Audio
	AudioArpe* visibleSound;
    AudioArpe* errorSound;

	//Infrastructure
    InfraStructure* myInfraStructure;

	//iPoints
	list< iPoint* > listPoint;
    void addPoint(iPoint* value);
    iPoint* findPoint(int valueID);

	Arpe *myArpe;
	iVrml *status;

};

#endif // Base_h
