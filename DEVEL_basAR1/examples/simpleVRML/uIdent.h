#ifndef uIdent_h
#define uIdent_h

#include "User.h"

class uIdent
{
public:
	uIdent(void);
	~uIdent(void);

	char name[256];
	char basARID[256];
	char email[256];

	// UnifiedServer
	char unifiedNameServer[256];
	char unifiedLogin[256];
	char unifiedPass[256];

	User *myUser;
};


#endif 