#ifndef iObject3D_h
#define iObject3D_h

#include "ipObject.h"


class iObject3D : public ipObject {

 public:

    iObject3D();

    ~iObject3D();

 public:
    int modelType; // 1 - VRML // 2 - Assimp
};

#endif // iObject3D_h
