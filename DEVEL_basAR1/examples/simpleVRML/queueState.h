#ifndef queueState_h
#define queueState_h

#include "State.h"


class queueState
{
public:

	int id;
	State *s;

	queueState(void);
	~queueState(void);
};

#endif // queueState