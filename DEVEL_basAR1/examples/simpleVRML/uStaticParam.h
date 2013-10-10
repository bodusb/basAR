#ifndef uStaticParam_h
#define uStaticParam_h

#include "User.h"

class uStaticParam
{
public:
	uStaticParam(void);
	~uStaticParam(void);

	int id;
	char name[256];
	char sAttribute[256];
	double nAttribute;
	int nItensOnUse;

	User *myUser;

};

#endif  // uStaticParam_h