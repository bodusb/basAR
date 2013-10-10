#ifndef uProfileParam_h
#define uProfileParam_h

class User;

class uProfileParam
{
public:
	uProfileParam(void);
	~uProfileParam(void);

	int id;
	char name[256];
	double loadedValue;
	double actualValue;
	double toSaveValue;
	int nItensOnUse;

	User *myUser;

};

#endif  // uProfileParam_h