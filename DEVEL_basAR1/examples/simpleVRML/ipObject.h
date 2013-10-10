#ifndef ipObject_h
#define ipObject_h

class iPoint;

class ipObject {

 public:

    ipObject();
    int objectReadFile();
    int objectWriteFile();
    ~ipObject();

    int id;
    int type; // 1 - MODEL3D
    char filename[256];
    bool active;


    iPoint *myiPoint;
};

#endif // ipObject_h
