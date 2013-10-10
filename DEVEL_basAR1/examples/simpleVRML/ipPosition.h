#ifndef ipPosition_h
#define ipPosition_h

class iPoint;

class ipPosition {

 public:

    void bakeGL(double para[3][4], double gl_para[16]);
    void bakeActualTrans();
    ipPosition();
    ~ipPosition();

    double trans[3][4];

	double startTrans[3][4];
	double lastTrans[3][4];

    double startTra[3];
    double startRot[3];
    double startScl[3];
    double actualTra[3];
    double actualRot[3];
    double actualScl[3];

    iPoint *myiPoint;
};

#endif // ipPosition_h
