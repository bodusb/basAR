#ifndef ipDist_h
#define ipDist_h

class Actuator;

class ipDist
{
public:
	Actuator *actuator;
	double distance;

	ipDist(void);
	~ipDist(void);
};

#endif // InfraStructure_h