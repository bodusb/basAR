#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GL/glut.h>

#include "ipPosition.h"

using namespace std;


void ipPosition::bakeGL(double para[3][4], double gl_para[16]){

    int     i, j;

    for( j = 0; j < 3; j++ ) {
        for( i = 0; i < 4; i++ ) {
            gl_para[i*4+j] = para[j][i];
        }
    }
    gl_para[0*4+3] = gl_para[1*4+3] = gl_para[2*4+3] = 0.0;
    gl_para[3*4+3] = 1.0;


}

void ipPosition::bakeActualTrans()
{
	double matGl[16];
	for(int i=0;i<16;i++)
		matGl[i] = 0;

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
		glMatrixMode(GL_MODELVIEW);
		glTranslated(this->actualTra[0],this->actualTra[1],this->actualTra[2]);
		if(this->actualRot[0] !=0) glRotated(this->actualRot[0] ,1.0,0.0,0.0);
		if(this->actualRot[1] !=0) glRotated(this->actualRot[1] ,0.0,1.0,0.0);
		if(this->actualRot[2] !=0) glRotated(this->actualRot[2] ,0.0,0.0,1.0);
		glScaled(this->actualScl[0],this->actualScl[1],this->actualScl[2]);
		glGetDoublev(GL_MODELVIEW_MATRIX,matGl);

	glPopMatrix();


	this->trans[0][0] = matGl[0];
	this->trans[1][0] = matGl[1];
	this->trans[2][0] = matGl[2];

	this->trans[0][1] = matGl[4];
	this->trans[1][1] = matGl[5];
	this->trans[2][1] = matGl[6];

	this->trans[0][2] = matGl[8];
	this->trans[1][2] = matGl[9];
	this->trans[2][2] = matGl[10];

	this->trans[0][3] = matGl[12]; //Tx
	this->trans[1][3] = matGl[13]; //Ty
	this->trans[2][3] = matGl[14]; //Tz
	

}

ipPosition::ipPosition(){


	this->startTra[0] =0; this->startTra[1] =0; this->startTra[2] =0;
	this->startRot[0] =0; this->startRot[1] =0; this->startRot[2] =0;
	this->startScl[0] =1; this->startScl[1] =1; this->startScl[2] =1;	 	

	this->actualTra[0] =0; this->actualTra[1] =0; this->actualTra[2] =0;
	this->actualRot[0] =0; this->actualRot[1] =0; this->actualRot[2] =0;
	this->actualScl[0] =0; this->actualScl[1] =0; this->actualScl[2] =0;		




}

ipPosition::~ipPosition(){
}
