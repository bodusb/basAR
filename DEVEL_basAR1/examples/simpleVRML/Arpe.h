#ifndef Arpe_h
#define Arpe_h



#include <list>


#include "Actuator.h"
#include "ActuatorARTKSM.h"
#include "AudioArpe.h"
#include "Base.h"
#include "iPoint.h"
#include "GenericItens.h"
#include "Game.h"
#include "Serial.h"
#include "User.h"

#include <irrKlang\irrKlang.h>
using namespace irrklang;
#pragma comment(lib, "irrKlang.lib")

class Actuator;
class Game;
class Rules;


class Arpe {

 public:

    Arpe();
    ~Arpe();

	//basAR application
    int arpeReadFiles();
	int arpeSetupEnvironment();
    void arpeWriteFiles();
	int verifyConsistency();
	CRITICAL_SECTION	parserCS;
	int s;
	bool projection;			// Turn projection on/off
	int prefWindowed;
	int prefWidth;					// Fullscreen mode width.
	int prefHeight;				// Fullscreen mode height.
	int prefDepth;					// Fullscreen mode bit depth.
	int prefRefresh;					// Fullscreen mode refresh rate. Set to 0 to use default rate.
    char appName[256];
    char configFilename[256];

	//Audio Interface
    ISoundEngine* audioEngine;
    AudioArpe* soundTrack;
    AudioArpe* startAudio;
	int playAudio(iPoint* value);
	int playActionAudio(iPoint* value);

	//Interaction 
	int interactionControl();
    iPoint* findPointNearActuator(Actuator* actuator, double *distance);

	//Moviment commands
    int movePoint(iPoint* value, Actuator* actuator);
    int releasePoint(iPoint* value);
    int grabPoint(iPoint* value, Actuator* actuator);
    int dropPoint(iPoint* move,iPoint* collide);
    int attractPoint(iPoint* move,iPoint* collide);
    int repelPoint(iPoint* value);
 	int showStatus(iPoint* value);
 	
	//Actuator
	std::list<Actuator*> listActuator;
	void addActuator(Actuator* value);
	Actuator* findActuator(int valueID);

	//Base
	std::list< Base* > listBase;
    void addBase(Base* value);
    Base* findBase(int valueID);
	iPoint* findIPoint(int valueID);

	//Infrastructure
    Game myGame;
    Rules *myRules;
	GenericItens myGenericItens;

	//
	User *myUser;

private:
	
	char getBuff(char *buf, int n, FILE *fp);
};

#endif // Arpe_h
   