#ifndef uLog_h
#define uLog_h

#include <time.h>

class User;

class uLog
{
public:
	uLog(void);
	~uLog(void);

	double id;
	double userID;
	int operationID;
	int baseID;
	int iPointID;
	time_t time;

	User *myUser;

};

#endif // uLog_h