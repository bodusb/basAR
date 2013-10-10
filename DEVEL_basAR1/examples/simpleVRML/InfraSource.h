#ifndef InfraSource_h
#define InfraSource_h

class InfraStructure;

class InfraSource {

 public:

    InfraSource();

    ~InfraSource();

 public:
    int id;
    bool onUse;
    char name[256];
	int type; //1 - ARTKSM

 public:

    InfraStructure *myInfraStructure;
};

#endif // InfraSource_h
