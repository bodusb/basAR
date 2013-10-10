#ifndef Adapt_h
#define Adapt_h

#include <list>

class User;
class adaptTechnique;
class StructureAdaptationDef;

class Adapt
{
public:
	Adapt(void);
	~Adapt(void);

	// Adaptation Techniques
	std::list< adaptTechnique*> listTechnique;
	void addTechnique( adaptTechnique* value);
	adaptTechnique* findTechnique(int valueID);

	// Structure Adaptation Definitions
	std::list< StructureAdaptationDef*> listSAD;
	void addSAD( StructureAdaptationDef* value);
	StructureAdaptationDef* findSAD(int valueID);

	User *myUser;
};

#endif