#include "iVrml.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <AR/ar.h>
#include <AR/config.h>
#include <AR/video.h>
#include <AR/param.h>
#include <AR/ar.h>
#include <AR/gsub_lite.h>
#include <AR/arvrml.h>


int iVrml::draw(){

	return arVrmlDraw(this->vrmlID);
}

iVrml::iVrml(){
}

iVrml::~iVrml(){
}

iVrml* iVrml::allocate(char *filename){

	return 0;
}