#ifndef iPoint_h
#define iPoint_h

#include <windows.h>
#include <process.h>

#include <list>

#include "Ball.h"
#include "ipObject.h"
#include "ipPosition.h"
#include "Action.h"
#include "Serial.h"

class ipAction;
class Base;
class ipDist;

class iPoint {

 public:
    iPoint();
    ~iPoint();
	
	//Setup functions
	char		getBuff(char *buf, int n, FILE *fp);
	int			iPointReadFile();
	int			externiPointReadFile();
	int			iPointWriteFile();

	//iPoint characteristics
    int			id;
    char		name[256];
    int			type;  // 1 - VIRTUAL IPOINT / 2 - EXTERN IPOINT
	int			viewMode; // 0-HIDE, 1-ONLY_BALL, 2-ONLY_OBJECT, 3-BOTH, 4-FLASH_BALL, 5-GHOST, 6-SENSE_PROX, 7-ALL_OBJECTS
    int			slaveOf;
    int			slavesAmount;
    int			slavesConnected;
    bool		connectionCompleted;
	char		objFilename[256];
	int			adaptType; // Adaptation type depends on the User Profile.

	//iPoint position matrixes
	ipPosition	position;

	//iPoint actual action
	Action*		actualAction;
	Action*		configAction;

	//List of models assigned
    Ball		ball;
    std::list< ipObject* > listObject;
	int			activeObjectID;
	void		addObject(ipObject* value);
	ipObject*	findObject(int valueID);
	void		showBall();
    void		showObjects();

	//External hardware configuration
	CRITICAL_SECTION	commCS;
	Serial		*arduino;
	int			enableReceiveOnThread;
	void		poolingThread();

	//Animation control
	CRITICAL_SECTION	animCS;
	void		wait(double seconds);
	void		createGLRotateMatrix(double angle, double x, double y, double z, double matrix[3][4]);

	//Accumulators
	double A;
	double B;

	//Distance to actuators
	std::list<ipDist*>		listDistances;
	void					addActuator(ipDist* value);
	ipDist*					findActuator(int valueID);
	double					getDistanceTo(int valueID);
    Base					*myBase;
};

#endif // iPoint_h
