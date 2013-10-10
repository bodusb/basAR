#ifndef AudioArpe_h
#define AudioArpe_h

#include <irrKlang\irrKlang.h>
using namespace irrklang;

class Arpe;

class AudioArpe {

 public:

    AudioArpe();

    ~AudioArpe();

    char filename[256];
    double volume;
    int status;
    ISoundSource* audioSource;
	vec3df position[3];

	int loadSound();
	int play2D();
	int play3D();

	Arpe *myArpe;
};

#endif // AudioArpe_h
