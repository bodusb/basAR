#ifndef adaptTechnique_h
#define adaptTechnique_h

#include "Rules.h"
#include "Adapt.h"

class adaptTechnique
{
public:
	adaptTechnique(void);
	~adaptTechnique(void);

	Rules *myRules;
	Adapt *myAdapt;
};

#endif