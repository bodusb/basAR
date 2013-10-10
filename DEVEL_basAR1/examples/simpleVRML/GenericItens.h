#ifndef GenericItens_h
#define GenericItens_h

#include "AudioArpe.h"
#include "iObject3D.h"


class GenericItens {

 public:

    GenericItens();
    ~GenericItens();

 public:
    iObject3D* holding;
    iObject3D* canwork;
    iObject3D* cannotWork;
    iObject3D* markCover;
    AudioArpe* errorSound;
};

#endif // GenericItens_h
