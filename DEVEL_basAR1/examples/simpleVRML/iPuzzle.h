#pragma once
#include <list>

#include <iBase.h>
#include <iPoint.h>
#include <iRules.h>
#include <Actuator.h>
#include <iARTKMarker.h>

#include <irrKlang\irrKlang.h>
using namespace irrklang;
#pragma comment(lib, "irrKlang.lib")

using namespace std;

class iPuzzle
{
public:
	//Application 
		//Attributes
			char m_appName[256];
			char m_puzzleFile[256];
		//Methods
			int	puzzleFill();
			int	automaticFill();
			
			int	interactionControl();


	//BASES
		//Attributes
			list <iBase> listBase;
			int n_bases;
		//methods
			void addBase(iBase value);
			list<iPoint>::iterator findIPoint(int value);
			int findIPointNearToCollide(float *distance, double actTrans[3][4]);
			int findIPointNearToCollideWithMoving(float *distance);
			int findIPointNearToCollideWithMovingToSense(float *distance);
			
	//RULES
		//Attributes
			iRules	rules;
		//Methods



	//AUDIO
		//Attributes
			ISoundEngine*	a_engine;

			ISoundSource*	a_soundTrack;
			char			a_soundTrack_filename[256];
			int				a_soundTrack_status; // 0 no audio, 1 audio not playing, 2 audio play once, 3 audio loop
			double			a_soundTrack_volume;
			ISoundSource*	a_start;
			char			a_start_filename[256];
			int				a_start_status; // 0 no audio, 1 audio not playing, 2 audio play once, 3 audio loop
			double			a_start_volume;
		//Methods

	//Actuator
		//Attributes
			list <Actuator> listActuator;
			int n_actuators;
		//Methods
			//void addActuator(Actuator value); 
			void addARTKMarkerActuator(iARTKMarker value);

	//GAME
		//Attributes
			int g_corrects;
			int g_mistakes;


	iPuzzle(void);
	~iPuzzle(void);

private:
	char getBuff(char *buf, int n, FILE *fp);
};

