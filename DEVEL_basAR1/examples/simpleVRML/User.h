#ifndef User_h
#define User_h

#include <list>

class Arpe;
class uLog;
class uProfileParam;
class uStaticParam;
class uIdent;
class Adapt;

class User
{
public:
	User(void);
	~User(void);

	char filename[256];
	
	//Setup functions
	char		getBuff(char *buf, int n, FILE *fp);
	int			userReadFile();

	
	char		uStaticFilename[256];
	char		uProfileFilename[256];
	

	// User Identifications
	uIdent*		ident;
	// std::list< uIdent*> listIdent;
	// void		addIdent( uIdent* value);
	// uIdent*		findUIdent(int valueID);

	// EVENT LOG

	char		uLogFilename[256];
	int			uLogRefreshSize;
	bool		uLogIncremental;
	std::list< uLog* > uLogList;  //ADJUSTABLE SAVING DELTA
	void		addLog( uLog* value);
	uLog*		findULog(int valueID);

	// STATIC PARAMETERS
	std::list< uStaticParam* > uStaticList;
	void		addStaticParam( uStaticParam* value);
	uStaticParam* findUStaticParam(int valueID);

	// PROFILE PARAMETERS
	std::list< uProfileParam* > uProfileList;
	void		addProfileParam( uProfileParam* value);
	uProfileParam* findUProfileParam(int valueID);

	// ADAPTATION CAPABILITIES
	char		adaptFilename[256];
	Adapt		*myAdapt;

	Arpe		*myArpe;

};

#endif 