#ifndef iStatus_h
#define iStatus_h

#include "ipObject.h"


class iStatus : public ipObject {

 public:

    void draw();

    iStatus();
	~iStatus(){};

 public:
    int vrmlID;
};

#endif // iStatus_h
