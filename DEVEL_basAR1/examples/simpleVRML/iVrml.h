#ifndef iVrml_h
#define iVrml_h




#include "iObject3D.h"


class iVrml : public iObject3D {

 public:

    int draw();

	iVrml* allocate(char *filename);

    iVrml();

    ~iVrml();

 public:
    int vrmlID;
};

#endif // iVrml_h
