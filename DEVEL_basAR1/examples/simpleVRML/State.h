#ifndef State_h
#define State_h

#include <time.h>
#include <list>

#include "Action.h"

class Rules;

class State {

 public:

    void addAction(Action* value);
    int parseState();
	State();
	~State();

    int id;
	int onUse;

	int waiting;
	time_t parsedTime;

	double time;
	int nextState; //if =0 "CONFIGURATION STATE"
   

	std::list< Action* > listAction;
	Rules *myRules;
};

#endif // State_h
