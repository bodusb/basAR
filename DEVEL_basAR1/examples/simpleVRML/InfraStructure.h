#ifndef InfraStructure_h
#define InfraStructure_h

#include <list>

using namespace std;

class InfraSource;
class Base;

class InfraStructure {

 public:

    int updateInfraData(double t[3][4], int v);
	void addInfraSource(InfraSource* value);

    InfraStructure();

    ~InfraStructure();
	
    double baseTrans[3][4];
	
    int visible;
	
    list< InfraSource* > listSource;

    Base *myBase;
};

#endif // InfraStructure_h
