#ifndef ipAction_h
#define ipAction_h

#include "Action.h"

class iPoint;

class ipAction {

 public:

    ipAction();

    ~ipAction();

 public:
    int id;
    Action action;

 public:

    /**
     * @element-type iPoint
     */
    iPoint *myiPoint;
};

#endif // ipAction_h
