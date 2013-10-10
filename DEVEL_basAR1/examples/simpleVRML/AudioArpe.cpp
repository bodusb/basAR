#include "AudioArpe.h"

#include <irrKlang\irrKlang.h>
using namespace irrklang;

#include "Arpe.h"

AudioArpe::AudioArpe()
{
	this->position->X = 0;
	this->position->Y = 0;
	this->position->Z = 0;
}

AudioArpe::~AudioArpe()
{
}


int AudioArpe::play2D(){

	if( (strcmp(this->filename,"") !=0) && (this->audioSource != 0)){

		if( this->status == 2){ // LOOP
			(*this->myArpe).audioEngine->play2D(this->audioSource,true);
			return 2;
		} else{
			(*this->myArpe).audioEngine->play2D(this->audioSource);
			return 1;
		}
		
	}

	return 0;
}

int AudioArpe::play3D(){

	return 0;
}

int AudioArpe::loadSound(){

	this->audioSource = (*this->myArpe).audioEngine->addSoundSourceFromFile(this->filename);

	if( (this->audioSource != 0)){ 
		this->audioSource->setDefaultVolume(this->volume);
	}
	return 1;
}