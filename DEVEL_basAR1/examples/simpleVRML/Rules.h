#ifndef Rules_h
#define Rules_h

#include <list>

#include "Action.h"
#include "State.h"
#include "iPoint.h"
#include "Actuator.h"
#include "queueState.h"

class Arpe;

class Rules {

 public:

	//Rules
	//int  interpretPointMode(char *value);
	char getBuff(char *buf, int n, FILE *fp);
	int parseRule();
	char configFilename[256];
    Rules();
	~Rules();
	int rulesReadFile();
    int rulesWriteFile();
    int verifyConsistency();
    int reloadRules();

	//States
	int nextState;
	int nextStateMath;
	int actualState;
	int lastState;
	int ardState;
	int lockParser;
    std::list< State* > listState;
    
	void addState(State* value);
    State* findState(int valueID);
    Action* findAction(int valueID);

	// STATE QUEUE
	std::list< queueState* > qS;
	bool getFromQueue;
	int queueIndex;
	State* findQS(int valueID);
	int setNextQueueItem(int valueID);
	void addQS(queueState* value);

	// Commands actions to parse
	int stat(iPoint *CollidePoint);												// 1 = STATIC (COLLIDE)
	int drgf(iPoint *collidePoint, Actuator *a);								// 2 = CAN_MOVE (GRAB) and drop if carying
	int drgrp_grab(iPoint *collidePoint, Actuator *a);							// 3 = CAN_MOVE (GRAB) and repels if touched by moving
	int drgrp_collided(iPoint *collidePoint, Actuator *a,iPoint *movingPoint);	// 3 = CAN_MOVE (COLLIDE) and repels if touched by moving
	int chgst(iPoint *collidePoint);											// 11 = GOTO_STATE (COLLIDE), move to other state
	
	int atto(iPoint *collidePoint, Actuator *a,iPoint *movingPoint);			// 4 = ATTRACT A SPECIFIC POINT
	int attrp(iPoint *collidePoint, Actuator *a,iPoint *movingPoint);			// 5 = ATTRACT a specific point, repel others
	int atta(iPoint *collidePoint, Actuator *a, iPoint *movingPoint);			// 6 = ATTRACT everything
	int drpo(iPoint *collidePoint, Actuator *a, iPoint *movingPoint);			// 7 = DROP with check A SPECIFIC POINT
	int drpa(iPoint *collidePoint, Actuator *a, iPoint *movingPoint);			// 8 = DROP with check everything
	int rplo(iPoint *collidePoint, Actuator *a, iPoint *movingPoint);			// 9 = REPELS a SPECIFIC POINT
	int rpla(iPoint *collidePoint, Actuator *a, iPoint *movingPoint);			// 10 = REPELS everything.

	int chgvm(iPoint *p);
	int tra(iPoint *p);
	int rot(iPoint *p);
	int scl(iPoint *p);
	int chgm(iPoint *p);
	int sets(iPoint *p);
	int setl(iPoint *p);
	int gets(iPoint *p);
	int getl(iPoint *p);
	int chgnm(iPoint *p);

	int esnd(iPoint *p);
	int esndb(iPoint *p);
	int n255b(iPoint *p);
	int ercv(iPoint *p);


	int adda(iPoint *p);
	int addb(iPoint *p);
	int amb(iPoint *p);
	int bma(iPoint *p);
	int nega(iPoint *p);
	int negb(iPoint *p);
	int mula(iPoint *p);
	int mulb(iPoint *p);
	int swab(iPoint *p);
	int cmp(iPoint *p);
	int cmpv(iPoint *p);
	int loada(iPoint *p);
	int loadb(iPoint *p);
	int dist(iPoint *p); 

	int loadq(iPoint *p); 
	int getq(iPoint *p);
	int setq(iPoint *p);

	int randi(iPoint *p);
	int getbx(iPoint *p);
	int getby(iPoint *p);
	int getbz(iPoint *p);
	//void callTraThread();
	
 
    Arpe *myArpe;
	void wait( double seconds);

private:
	void convParaToGl(const double para[3][4], double m_modelview[16], const double scale);
	void loadIdentity(double value[3][4]);
	void copyMatrix(double source[3][4],double destiny[3][4]);
};

#endif // Rules_h

