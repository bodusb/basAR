#include "Action.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <irrKlang\irrKlang.h>
using namespace irrklang;
#pragma comment(lib, "irrKlang.lib")


Action::Action()
{
}

Action::~Action()
{
}

int Action::playAudio(){
	
	return 0;
}

int Action::cleanAction(int valueID){
	this->id = valueID;

	this->baseID = 0;
	this->ipointID = 0;

	this->opcode = 0;
	this->pointMode = 8;
	this->nextState = -1;
	this->pointWaited = 0;
	this->modelToChange = 0;

	strcpy(this->audioFilename,"");
	this->audioOverplay = false;
	this->audio3D = 0;

	strcpy(this->arduinoCommand,"");
	strcpy(this->eMsg,"");

	this->x = 0;
	this->y = 0;
	this->z = 0;

	this->step = 1;
	this->time = 0;

	this->cmpAction = 0;
	this->mathValue = 0;
	this->mathValue2 = 0;

	return 1;
}

int Action::fillAction(char *buf){
	char	tempOP[256],tempMode[256],tempStr1[256],tempStr2[256],tempStr3[256];
	int		retScan;

	//--------------------------------------------------------------------------
	// SCAN INTERNAL COMMANDS OPCODES
	//--------------------------------------------------------------------------
	//printf("\n Action %s", buf);
	sscanf(buf,"%d %s",  &this->ipointID, &tempOP);

	//--------------------------------------------------------------------------
	// UNDERSTAND OPCODE TO FIRST TRIAL 
	//--------------------------------------------------------------------------
	
	if (strcmp(tempOP,"STAT")  == 0)		this->opcode = 1;		// 1 = STATIC
	if (strcmp(tempOP,"DRGF") == 0)			this->opcode = 2;		// 2 = CAN_MOVE and drop if carying
	if (strcmp(tempOP,"DRGRP") == 0)		this->opcode = 3;		// 3 = CAN_MOVE and repels if touched by moving
	if (strcmp(tempOP,"CHGST") == 0)		this->opcode = 11;		// 11 = GOTO_STATE, move to other state

	if (strcmp(tempOP,"ATTO")  == 0)		this->opcode = 4;		// 4 = ATTRACT A SPECIFIC POINT
	if (strcmp(tempOP,"ATTRP")  == 0)		this->opcode = 5;		// 5 = ATTRACT a specific point, repel others
	if (strcmp(tempOP,"ATTA")  == 0)		this->opcode = 6;		// 6 = ATTRACT everything

	if (strcmp(tempOP,"DRPO") == 0)			this->opcode = 7;		// 7 = DROP with check A SPECIFIC POINT
	if (strcmp(tempOP,"DRPA") == 0)			this->opcode = 8;		// 8 = DROP with check everything

	if (strcmp(tempOP,"RPLO") == 0)			this->opcode = 9;		// 9 = REPELS a SPECIFIC POINT
	if (strcmp(tempOP,"RPLA") == 0)			this->opcode = 10;		// 10 = REPELS everything.

	if (strcmp(tempOP,"CHGVM") == 0)		this->opcode = 12;		// 12 = Change view mode
	if (strcmp(tempOP,"TRA") == 0)			this->opcode = 13;		// 13 = Change translaction
	if (strcmp(tempOP,"ROT") == 0)			this->opcode = 14;		// 14 = Change rotation
	if (strcmp(tempOP,"SCL") == 0)			this->opcode = 15;		// 15 = Change scale
	if (strcmp(tempOP,"CHGM") == 0)			this->opcode = 16;		// 16 = Change model
	if (strcmp(tempOP,"SETS") == 0)			this->opcode = 17;		// 17 = Copy Actual to Start
	if (strcmp(tempOP,"SETL") == 0)			this->opcode = 18;		// 18 = Copy Actual to Last
	if (strcmp(tempOP,"GETS") == 0)			this->opcode = 19;		// 19 = Copy Start to Actual
	if (strcmp(tempOP,"GETL") == 0)			this->opcode = 20;		// 20 = Copy Last to Actual

	if (strcmp(tempOP,"ESND") == 0)			this->opcode = 21;		// 21 = Send external command
	if (strcmp(tempOP,"ERCV") == 0)			this->opcode = 22;		// 22 = Receive external command
	
	if (strcmp(tempOP,"CHGNM") == 0)		this->opcode = 23;		// 23 = Change model to next

	if (strcmp(tempOP,"ADDA") == 0)			this->opcode = 24;		// 24 = A = A + NUMBER
	if (strcmp(tempOP,"ADDB") == 0)			this->opcode = 25;		// 25 = B = B + NUMBER
	if (strcmp(tempOP,"AMB") == 0)			this->opcode = 26;		// 26 = A = A - B
	if (strcmp(tempOP,"BMA") == 0)			this->opcode = 27;		// 27 = B = B - A
	if (strcmp(tempOP,"NEGA") == 0)			this->opcode = 28;		// 28 = A = -A
	if (strcmp(tempOP,"NEGB") == 0)			this->opcode = 29;		// 29 = B = -B
	if (strcmp(tempOP,"MULA") == 0)			this->opcode = 30;		// 30 = A = A*NUMBER
	if (strcmp(tempOP,"MULB") == 0)			this->opcode = 31;		// 31 = B = B*NUMBER
	if (strcmp(tempOP,"SWAB") == 0)			this->opcode = 32;		// 32 = A <-> B
	if (strcmp(tempOP,"CMP") == 0)			this->opcode = 33;		// 33 = VALOR = A - B ( TESTA VALOR SE ACERTA COMPARACAO MUDA DE ESTADO)
	if (strcmp(tempOP,"CMPV") == 0)			this->opcode = 34;		// 34 = VALOR = A - NUMBER (TESTA VALOR SE ACERTA COMPARACAO MUDA DE ESTADO)
	if (strcmp(tempOP,"LOADA") == 0)		this->opcode = 35;		// 35 = A <= 0
	if (strcmp(tempOP,"LOADB") == 0)		this->opcode = 36;		// 36 = B <= 0
	if (strcmp(tempOP,"DIST") == 0)			this->opcode = 37;		// 37 = B <= Dist(point,actuator)

	if (strcmp(tempOP,"LOADQ") == 0)		this->opcode = 38;		// 38 = Q <= VALUE
	if (strcmp(tempOP,"GETQ") == 0)			this->opcode = 39;		// 39 = B <= Q
	if (strcmp(tempOP,"SETQ") == 0)			this->opcode = 40;		// 40 = B <= Q

	if (strcmp(tempOP,"RANDI") == 0)		this->opcode = 41;		// 41 = B <= RandomInt(Min,Max)
	if (strcmp(tempOP,"GETBX") == 0)		this->opcode = 42;		// 42 = B <= xValueOf(BaseMarkerNumber)
	if (strcmp(tempOP,"GETBY") == 0)		this->opcode = 43;		// 42 = B <= yValueOf(BaseMarkerNumber)
	if (strcmp(tempOP,"GETBZ") == 0)		this->opcode = 44;		// 42 = B <= zValueOf(BaseMarkerNumber)

	if (strcmp(tempOP,"ESNDB") == 0)		this->opcode = 45;		// 45 = Send external command without the lookuptable (send B value)
	if (strcmp(tempOP,"N255B") == 0)		this->opcode = 46;		// 46 = normalize B value to 0-255 from 0-MAXVALUE given

	strcpy(this->opcodeName,tempOP);
	//--------------------------------------------------------------------------
	// UNDERSTAND ALL COMMAND
	//--------------------------------------------------------------------------

	switch(this->opcode){
	case 1:	{	// sintaxe: [P_ID] [STAT] [POINT_MODE] <AUDIO> <OVER/3D> <VOLUME> <X> <Y> <Z> 
		this->type = 1;
		retScan = sscanf(buf,"%d %s %s %s %s %lf %lf %lf %lf", &this->ipointID, &tempOP, &tempMode, &tempStr1, &tempStr2, &this->mathValue2, &this->x, &this->y, &this->z);
		switch(retScan){
		case 3: // CORRECT READING WITHOUT AUDIO
			break;
		case 4: // CORRECT READING WITH AUDIO AND NO OVERLOOP
			strcpy(this->audioFilename,tempStr1);
			this->audioOverplay = false;
			break;
		case 5: // CORRECT READING WITH AUDIO AND OVERLOOP
			strcpy(this->audioFilename,tempStr1);
			if( strcmp(tempStr2,"OVER") == 0)	{ this->audioOverplay = true;} else {this->audioOverplay = false; }
			if( strcmp(tempStr2,"3D") == 0)		{ this->audio3D = 1; this->mathValue2 = 1;}
			break;
		case 9: // CORRECT READING WITH AUDIO 3D 
			if( strcmp(tempStr2,"3D") == 0)		{ this->audio3D = 1;}
			break;
		default:break;
		};
		break;}
	case 2:	{	// sintaxe: [P_ID] [DRGF] [POINT_MODE] <AUDIO> <OVER/3D> <VOLUME> <X> <Y> <Z>
		this->type = 1;
		retScan = sscanf(buf,"%d %s %s %s %s %lf %lf %lf %lf", &this->ipointID, &tempOP, &tempMode, &tempStr1, &tempStr2, &this->mathValue2, &this->x, &this->y, &this->z);
		switch(retScan){
		case 3: // CORRECT READING WITHOUT AUDIO
			break;
		case 4: // CORRECT READING WITH AUDIO AND NO OVERLOOP
			strcpy(this->audioFilename,tempStr1);
			this->audioOverplay = false;
			break;
		case 5: // CORRECT READING WITH AUDIO AND OVERLOOP
			strcpy(this->audioFilename,tempStr1);
			if( strcmp(tempStr2,"OVER") == 0) { this->audioOverplay = true; } else {this->audioOverplay = false; }
			if( strcmp(tempStr2,"3D") == 0)		{ this->audio3D = 1; this->mathValue2 = 1;}
			break;
		case 9: // CORRECT READING WITH AUDIO 3D 
			if( strcmp(tempStr2,"3D") == 0)		{ this->audio3D = 1;}
			break;
		default:break;
		};
		break;}
	case 3:	{	//sintaxe: [P_ID] [DRGRP] [POINT_MODE] [NEXT_STATE] <AUDIO> <OVER/3D> <VOLUME> <X> <Y> <Z>
		this->type = 1;
		retScan = sscanf(buf,"%d %s %s %d %s %s %lf %lf %lf %lf", &this->ipointID, &tempOP, &tempMode,  &this->nextState, &tempStr1, &tempStr2, &this->mathValue2, &this->x, &this->y, &this->z);
		switch(retScan){
		case 4: // CORRECT READING WITHOUT AUDIO
			break;
		case 5: // CORRECT READING WITH AUDIO AND NO OVERLOOP
			strcpy(this->audioFilename,tempStr1);
			this->audioOverplay = false;
			break;
		case 6: // CORRECT READING WITH AUDIO AND OVERLOOP
			strcpy(this->audioFilename,tempStr1);
			if( strcmp(tempStr2,"OVER") == 0) { this->audioOverplay = true; } else {this->audioOverplay = false; }
			if( strcmp(tempStr2,"3D") == 0)		{ this->audio3D = 1; this->mathValue2 = 1;}
			break;
		case 10: // CORRECT READING WITH AUDIO 3D 
			if( strcmp(tempStr2,"3D") == 0)		{ this->audio3D = 1;}
			break;
		default:break;
		};
		break;}
	case 4:{		//sintaxe: [P_ID] [ATTO] [OBJ_WAITED] [POINT_MODE] [NEXT_STATE] <AUDIO> <OVER/3D> <VOLUME> <X> <Y> <Z>
		this->type = 1;
		retScan = sscanf(buf,"%d %s %d %s %d %s %s %lf %lf %lf %lf", &this->ipointID, &tempOP, &this->pointWaited, &tempMode, &this->nextState, &tempStr1, &tempStr2, &this->mathValue2, &this->x, &this->y, &this->z);
		switch(retScan){
		case 5: // CORRECT READING WITHOUT AUDIO
			break;
		case 6: // CORRECT READING WITH AUDIO AND NO OVERLOOP
			strcpy(this->audioFilename,tempStr1);
			this->audioOverplay = false;
			break;
		case 7: // CORRECT READING WITH AUDIO AND OVERLOOP
			strcpy(this->audioFilename,tempStr1);
			if( strcmp(tempStr2,"OVER") == 0) { this->audioOverplay = true; } else {this->audioOverplay = false; }
			if( strcmp(tempStr2,"3D") == 0)		{ this->audio3D = 1; this->mathValue2 = 1;}
			break;
		case 11: // CORRECT READING WITH AUDIO 3D 
			if( strcmp(tempStr2,"3D") == 0)		{ this->audio3D = 1;}
			break;
		default:break;
		};
		break;}
	case 5:{		//sintaxe:[P_ID] [ATTRP] [OBJ_WAINTED] [POINT_MODE] [NEXT_STATE] <AUDIO> <OVER/3D> <VOLUME> <X> <Y> <Z>
		this->type = 1;
		retScan = sscanf(buf,"%d %s %d %s %d %s %s %lf %lf %lf %lf",  &this->ipointID, &tempOP, &this->pointWaited, &tempMode, &this->nextState, &tempStr1, &tempStr2, &this->mathValue2, &this->x, &this->y, &this->z);
		switch(retScan){
		case 5: // CORRECT READING WITHOUT AUDIO
			break;
		case 6: // CORRECT READING WITH AUDIO AND NO OVERLOOP
			strcpy(this->audioFilename,tempStr1);
			this->audioOverplay = false;
			break;
		case 7: // CORRECT READING WITH AUDIO AND OVERLOOP
			strcpy(this->audioFilename,tempStr1);
			if( strcmp(tempStr2,"OVER") == 0) { this->audioOverplay = true; } else {this->audioOverplay = false; }
			if( strcmp(tempStr2,"3D") == 0)		{ this->audio3D = 1; this->mathValue2 = 1;}
			break;
		case 11: // CORRECT READING WITH AUDIO 3D 
			if( strcmp(tempStr2,"3D") == 0)		{ this->audio3D = 1;}
			break;
		default:break;
		};
		break;}
	case 6:	{	//sintaxe: [P_ID] [ATTA] [POINT_MODE] [NEXT_STATE] <AUDIO> <OVER/3D> <VOLUME> <X> <Y> <Z>
		this->type = 1;
		retScan = sscanf(buf,"%d %s %s %d %s %s %lf %lf %lf %lf", &this->ipointID, &tempOP, &tempMode, &this->nextState, &tempStr1, &tempStr2, &this->mathValue2, &this->x, &this->y, &this->z);
		switch(retScan){
		case 4: // CORRECT READING WITHOUT AUDIO
			break;
		case 5: // CORRECT READING WITH AUDIO AND NO OVERLOOP
			strcpy(this->audioFilename,tempStr1);
			this->audioOverplay = false;
			break;
		case 6: // CORRECT READING WITH AUDIO AND OVERLOOP
			strcpy(this->audioFilename,tempStr1);
			if( strcmp(tempStr2,"OVER") == 0) { this->audioOverplay = true; } else {this->audioOverplay = false; }
			if( strcmp(tempStr2,"3D") == 0)		{ this->audio3D = 1; this->mathValue2 = 1;}
			break;
		case 10: // CORRECT READING WITH AUDIO 3D 
			if( strcmp(tempStr2,"3D") == 0)		{ this->audio3D = 1;}
			break;
		default:break;
		};
		break;}
	case 7:	{	//sintaxe: [P_ID] [DRPO] [OBJ_WAITED] [POINT_MODE] [NEXT_STATE] <AUDIO> <OVER/3D> <VOLUME> <X> <Y> <Z>
		this->type = 1;
		retScan = sscanf(buf,"%d %s %d %s %d %s %s %lf %lf %lf %lf", &this->ipointID, &tempOP, &this->pointWaited, &tempMode, &this->nextState, &tempStr1, &tempStr2, &this->mathValue2, &this->x, &this->y, &this->z);
		switch(retScan){
		case 5: // CORRECT READING WITHOUT AUDIO
			break;
		case 6: // CORRECT READING WITH AUDIO AND NO OVERLOOP
			strcpy(this->audioFilename,tempStr1);
			this->audioOverplay = false;
			break;
		case 7: // CORRECT READING WITH AUDIO AND OVERLOOP
			strcpy(this->audioFilename,tempStr1);
			if( strcmp(tempStr2,"OVER") == 0) { this->audioOverplay = true; } else {this->audioOverplay = false; }
			if( strcmp(tempStr2,"3D") == 0)		{ this->audio3D = 1; this->mathValue2 = 1;}
			break;
		case 11: // CORRECT READING WITH AUDIO 3D 
			if( strcmp(tempStr2,"3D") == 0)		{ this->audio3D = 1;}
			break;
		default:break;
		};
		break;}
	case 8:	{	//sintaxe:  [P_ID] [DRPA] [POINT_MODE] [NEXT_STATE] <AUDIO> <OVER/3D> <VOLUME> <X> <Y> <Z>
		this->type = 1;
		retScan = sscanf(buf," %d %s %s %d %s %s %lf %lf %lf %lf", &this->ipointID, &tempOP, &tempMode, &this->nextState, &tempStr1, &tempStr2, &this->mathValue2, &this->x, &this->y, &this->z);
		switch(retScan){
		case 4: // CORRECT READING WITHOUT AUDIO
			break;
		case 5: // CORRECT READING WITH AUDIO AND NO OVERLOOP
			strcpy(this->audioFilename,tempStr1);
			this->audioOverplay = false;
			break;
		case 6: // CORRECT READING WITH AUDIO AND OVERLOOP
			strcpy(this->audioFilename,tempStr1);
			if( strcmp(tempStr2,"OVER") == 0) { this->audioOverplay = true; } else {this->audioOverplay = false; }
			if( strcmp(tempStr2,"3D") == 0)		{ this->audio3D = 1; this->mathValue2 = 1;}
			break;
		case 10: // CORRECT READING WITH AUDIO 3D 
			if( strcmp(tempStr2,"3D") == 0)		{ this->audio3D = 1;}
			break;
		default:break;
		};
		break;}
	case 9:	{	//sintaxe: [P_ID] [RPLO] [OBJ_WAITED] [POINT_MODE] [NEXT_STATE] <AUDIO> <OVER/3D> <VOLUME> <X> <Y> <Z>
		this->type = 1;
		retScan = sscanf(buf,"%d %s %d %s %d %s %s", &this->ipointID, &tempOP, &this->pointWaited, &tempMode, &this->nextState, &tempStr1, &tempStr2, &this->mathValue2, &this->x, &this->y, &this->z);
		switch(retScan){
		case 5: // CORRECT READING WITHOUT AUDIO
			break;
		case 6: // CORRECT READING WITH AUDIO AND NO OVERLOOP
			strcpy(this->audioFilename,tempStr1);
			this->audioOverplay = false;
			break;
		case 7: // CORRECT READING WITH AUDIO AND OVERLOOP
			strcpy(this->audioFilename,tempStr1);
			if( strcmp(tempStr2,"OVER") == 0) { this->audioOverplay = true; } else {this->audioOverplay = false; }
			if( strcmp(tempStr2,"3D") == 0)		{ this->audio3D = 1; this->mathValue2 = 1;}
			break;
		case 11: // CORRECT READING WITH AUDIO 3D 
			if( strcmp(tempStr2,"3D") == 0)		{ this->audio3D = 1;}
			break;
		default:break;
		};
		break;}
	case 10:	{	//sintaxe: [P_ID] [RPLA] [POINT_MODE] [NEXT_STATE] <AUDIO> <OVER/3D> <VOLUME> <X> <Y> <Z>
		this->type = 1;
		retScan = sscanf(buf,"%d %s %s %d %s %s %lf %lf %lf %lf", &this->ipointID, &tempOP, &tempMode, &this->nextState, &tempStr1, &tempStr2, &this->mathValue2, &this->x, &this->y, &this->z);
		switch(retScan){
		case 4: // CORRECT READING WITHOUT AUDIO
			break;
		case 5: // CORRECT READING WITH AUDIO AND NO OVERLOOP
			strcpy(this->audioFilename,tempStr1);
			this->audioOverplay = false;
			break;
		case 6: // CORRECT READING WITH AUDIO AND OVERLOOP
			strcpy(this->audioFilename,tempStr1);
			if( strcmp(tempStr2,"OVER") == 0) { this->audioOverplay = true; } else {this->audioOverplay = false; }
			if( strcmp(tempStr2,"3D") == 0)		{ this->audio3D = 1; this->mathValue2 = 1;}
			break;
		case 10: // CORRECT READING WITH AUDIO 3D 
			if( strcmp(tempStr2,"3D") == 0)		{ this->audio3D = 1;}
			break;
		default:break;};
		break;}
	case 11: {	//sintaxe: [P_ID] [CHGST] [POINT_MODE] [NEXT_STATE] <AUDIO> <OVER/3D> <VOLUME> <X> <Y> <Z>
		this->type = 1;
		retScan = sscanf(buf,"%d %s %s %d %s %s %lf %lf %lf %lf", &this->ipointID, &tempOP, &tempMode, &this->nextState, &tempStr1, &tempStr2, &this->mathValue2, &this->x, &this->y, &this->z);
		switch(retScan){
		case 4: // CORRECT READING WITHOUT AUDIO
			break;
		case 5: // CORRECT READING WITH AUDIO AND NO OVERLOOP
			strcpy(this->audioFilename,tempStr1);
			this->audioOverplay = false;
			break;
		case 6: // CORRECT READING WITH AUDIO AND OVERLOOP
			strcpy(this->audioFilename,tempStr1);
			if( strcmp(tempStr2,"OVER") == 0) { this->audioOverplay = true; } else {this->audioOverplay = false; }
			if( strcmp(tempStr2,"3D") == 0)		{ this->audio3D = 1; this->mathValue2 = 1;}
			break;
		case 10: // CORRECT READING WITH AUDIO 3D 
			if( strcmp(tempStr2,"3D") == 0)		{ this->audio3D = 1;}
			break;
		default:break;};
		break;}
	case 12: { //sintaxe: [P_ID] [CHGVM] [POINT_MODE]
		this->type = 0;
		retScan = sscanf(buf,"%d %s %s", &this->ipointID, &tempOP, &tempMode);
		break;}
	case 13: { //sintaxe: [P_ID] [TRA] [X] [Y] [Z] [Time] [Step]
		this->type = 0;
		retScan = sscanf(buf,"%d %s %lf %lf %lf %lf %lf", &this->ipointID, &tempOP, &this->x, &this->y, &this->z, &this->time, &this->step);
		if(retScan == 5) { this->time = 0; this->step = 0; }
		break;}
	case 14: { //sintaxe: [P_ID] [ROT] [X] [Y] [Z] [Time] [Step]
		this->type = 0;
		retScan = sscanf(buf,"%d %s %lf %lf %lf %lf %lf", &this->ipointID, &tempOP, &this->x, &this->y, &this->z, &this->time, &this->step);
		if(retScan == 5){ this->time = 0; this->step = 0; }
		break;}
	case 15: { //sintaxe: [P_ID] [SCL] [X] [Y] [Z] [Time] [Step]
		this->type = 0;
		retScan = sscanf(buf,"%d %s %lf %lf %lf %lf %lf", &this->ipointID, &tempOP, &this->x, &this->y, &this->z, &this->time, &this->step);
		if(retScan == 5){ this->time = 0; this->step = 0; }
		break;}
	case 16: { //sintaxe: [P_ID] [CHGM] [ModelToChange]
		this->type = 0;
		retScan = sscanf(buf,"%d %s %d", &this->ipointID, &tempOP, &this->modelToChange);
		break;}
	case 17: { //sintaxe: [P_ID] [SETS]
		this->type = 0;
		retScan = sscanf(buf,"%d %s", &this->ipointID, &tempOP);
		break;}
	case 18: {//sintaxe: [P_ID] [SETL]
		this->type = 0;
		retScan = sscanf(buf,"%d %s", &this->ipointID, &tempOP);
		break;}
	case 19: {//sintaxe: [P_ID] [GETS]
		this->type = 0;
		retScan = sscanf(buf,"%d %s", &this->ipointID, &tempOP);
		break;}
	case 20: {//sintaxe: [P_ID] [GETL]
		this->type = 0;
		retScan = sscanf(buf,"%d %s", &this->ipointID, &tempOP);
		break;}
	case 21: {//sintaxe: [P_ID] [ESND] [MSG] <AUDIO> <OVER/3D> <VOLUME> <X> <Y> <Z>
		this->type = 2;
		retScan = sscanf(buf,"%d %s %s %s %s %lf %lf %lf %lf", &this->ipointID, &tempOP, &this->eMsg, &tempStr1, &tempStr2, &this->mathValue2, &this->x, &this->y, &this->z);
		//printf("\n -> %s", this->eMsg);
		switch(retScan){
		case 3: // CORRECT READING WITHOUT AUDIO
			break;
		case 4: // CORRECT READING WITH AUDIO AND NO OVERLOOP
			strcpy(this->audioFilename,tempStr1);
			this->audioOverplay = false;
			break;
		case 5: // CORRECT READING WITH AUDIO AND OVERLOOP
			strcpy(this->audioFilename,tempStr1);
			if( strcmp(tempStr2,"OVER") == 0) { this->audioOverplay = true; } else {this->audioOverplay = false; }
			if( strcmp(tempStr2,"3D") == 0)		{ this->audio3D = 1; this->mathValue2 = 1;}
			break;
		case 9: // CORRECT READING WITH AUDIO 3D 
			if( strcmp(tempStr2,"3D") == 0)		{ this->audio3D = 1;}
			break;
		default:break;};
		break;}
	case 22: { //sintaxe: [P_ID] [ERCV] [MSG] [NEXT_STATE] <OVER/3D> <VOLUME> <X> <Y> <Z>
		this->type = 2;
		retScan = sscanf(buf,"%d %s %s %d %s %s %lf %lf %lf %lf", &this->ipointID, &tempOP, &this->eMsg, &this->nextState, &tempStr1, &tempStr2, &this->mathValue2, &this->x, &this->y, &this->z);
	switch(retScan){
		case 4: // CORRECT READING WITHOUT AUDIO
			break;
		case 5: // CORRECT READING WITH AUDIO AND NO OVERLOOP
			strcpy(this->audioFilename,tempStr1);
			this->audioOverplay = false;
			break;
		case 6: // CORRECT READING WITH AUDIO AND OVERLOOP
			strcpy(this->audioFilename,tempStr1);
			if( strcmp(tempStr2,"OVER") == 0) { this->audioOverplay = true; } else {this->audioOverplay = false; }
			if( strcmp(tempStr2,"3D") == 0)		{ this->audio3D = 1; this->mathValue2 = 1;}
			break;
		case 9: // CORRECT READING WITH AUDIO 3D 
			if( strcmp(tempStr2,"3D") == 0)		{ this->audio3D = 1;}
			break;
		default:break;};
		break;}
	case 23: {//sintaxe: [P_ID] [CHGNM]
		this->type = 0;
		retScan = sscanf(buf,"%d %s",&this->ipointID, &tempOP);
		break;}

	case 24: {//sintaxe: [P_ID] [ADDA] [Value]
		this->type = 0;
		retScan = sscanf(buf,"%d %s %lf",&this->ipointID, &tempOP, &this->mathValue);
		break;}
	case 25: {//sintaxe: [P_ID] [ADDB] [Value]
		this->type = 0;
		retScan = sscanf(buf,"%d %s %lf",&this->ipointID, &tempOP, &this->mathValue);
		break;}
	case 26: {//sintaxe: [P_ID] [AMB]
		this->type = 0;
		break;}
	case 27: {//sintaxe: [P_ID] [BMA]
		this->type = 0;
		break;}
	case 28: {//sintaxe: [P_ID] [NEGA]
		this->type = 0;
		break;}
	case 29: {//sintaxe: [P_ID] [NEGB]
		this->type = 0;
		break;}
	case 30: {//sintaxe: [P_ID] [MULA] [Value]
		this->type = 0;
		retScan = sscanf(buf,"%d %s %lf",&this->ipointID, &tempOP, &this->mathValue);
		break;}
	case 31: {//sintaxe: [P_ID] [MULB] [Value]
		this->type = 0;
		retScan = sscanf(buf,"%d %s %lf",&this->ipointID, &tempOP, &this->mathValue);
		break;}
	case 32:{//sintaxe: [P_ID] [SWAB]
		this->type = 0;
		break;}
	case 33: {//sintaxe: [P_ID] [CMP] [GREATER/LESSER/EQUALS] [NEXTSTATE] <AUDIO> <OVER/3D> <VOLUME> <X> <Y> <Z>
		this->type = 0;
		retScan = sscanf(buf,"%d %s %s %d %s %s %lf %lf %lf %lf",&this->ipointID, &tempOP, &tempStr1, &this->nextState,&tempStr2,&tempStr3, &this->mathValue2, &this->x, &this->y, &this->z);
		if( strcmp(tempStr1,"GREATER") == 0 ) this->cmpAction = 1;
		if( strcmp(tempStr1,"LESSER") == 0 ) this->cmpAction = 2;
		if( strcmp(tempStr1,"EQUALS") == 0 ) this->cmpAction = 3;
		//printf(" Compare type %d with B",this->cmpAction);
		switch(retScan){
		case 4: // CORRECT READING WITHOUT AUDIO
			break;
		case 5: // CORRECT READING WITH AUDIO AND NO OVERLOOP
			strcpy(this->audioFilename,tempStr2);
			this->audioOverplay = false;
			break;
		case 6: // CORRECT READING WITH AUDIO AND OVERLOOP
			strcpy(this->audioFilename,tempStr2);
			if( strcmp(tempStr3,"OVER") == 0) { this->audioOverplay = true; } else {this->audioOverplay = false; }
			if( strcmp(tempStr3,"3D") == 0)		{ this->audio3D = 1; this->mathValue2 = 1;}
			break;
		case 10: // CORRECT READING WITH AUDIO 3D 
			if( strcmp(tempStr3,"3D") == 0)		{ this->audio3D = 1;}
			break;
		default:break;};
		break;}
	case 34: {//sintaxe: [P_ID] [CMPV] [Value] [GREATER/LESSER/EQUALS] [NEXTSTATE] <AUDIO> <OVER/3D> <VOLUME> <X> <Y> <Z>
		this->type = 0;
		retScan = sscanf(buf,"%d %s %lf %s %d %s %s %lf %lf %lf %lf",&this->ipointID, &tempOP, &this->mathValue, &tempStr1, &this->nextState,&tempStr2,&tempStr3, &this->mathValue2, &this->x, &this->y, &this->z);
		if( strcmp(tempStr1,"GREATER") == 0 ) this->cmpAction = 1;
		if( strcmp(tempStr1,"LESSER") == 0 ) this->cmpAction = 2;
		if( strcmp(tempStr1,"EQUALS") == 0 ) this->cmpAction = 3;
		//printf(" Compare type %d with %3.2f", this->cmpAction, this->mathValue);
		switch(retScan){
		case 5: // CORRECT READING WITHOUT AUDIO
			break;
		case 6: // CORRECT READING WITH AUDIO AND NO OVERLOOP
			strcpy(this->audioFilename,tempStr2);
			this->audioOverplay = false;
			break;
		case 7: // CORRECT READING WITH AUDIO AND OVERLOOP
			strcpy(this->audioFilename,tempStr2);
			if( strcmp(tempStr3,"OVER") == 0) { this->audioOverplay = true; } else {this->audioOverplay = false; }
			if( strcmp(tempStr3,"3D") == 0)		{ this->audio3D = 1; this->mathValue2 = 1;}
			break;
		case 11: // CORRECT READING WITH AUDIO 3D 
			if( strcmp(tempStr3,"3D") == 0)		{ this->audio3D = 1;}
			break;
		default:break;};
		break;}
	case 35:{//sintaxe: [P_ID] [LOADA] [Value] 
		this->type = 0;
		retScan = sscanf(buf,"%d %s %lf",&this->ipointID, &tempOP, &this->mathValue);
		//printf(" RS:%d-%d-%s -> %3.2f",retScan,this->ipointID,tempOP,this->mathValue);
		break;}
	case 36:{//sintaxe: [P_ID] [LOADB] [Value] 
		this->type = 0;
		retScan = sscanf(buf,"%d %s %lf",&this->ipointID, &tempOP, &this->mathValue);
		//printf(" RS:%d-%d-%s -> %3.2f",retScan,this->ipointID,tempOP,this->mathValue);
		break;}
	case 37:{//sintaxe: [P_ID] [DIST] [A_ID]
		this->type = 0;
		retScan = sscanf(buf,"%d %s %d",&this->ipointID, &tempOP, &this->pointWaited);
		break;}
	case 38:{//sintaxe: [P_ID] [LOADQ] [Value]
		this->type = 0;
		retScan = sscanf(buf,"%d %s %lf",&this->ipointID, &tempOP, &this->mathValue);
		break;}
	case 39:{//sintaxe: [P_ID] [GETQ]
		this->type = 0;
		retScan = sscanf(buf,"%d %s",&this->ipointID, &tempOP);
		break;}
	case 40:{//sintaxe: [P_ID] [SETQ]
		this->type = 0;
		retScan = sscanf(buf,"%d %s",&this->ipointID, &tempOP);
		break;}
	case 41:{//sintaxe: [P_ID] [RANDI] [MIN] [MAX]
		this->type = 0;
		retScan = sscanf(buf,"%d %s %d %d",&this->ipointID, &tempOP, &this->mathValue, &this->mathValue2);
		break;}
	case 42:{//sintaxe: [P_ID] [GETBX] [MARKER_ID]
		this->type = 0;
		retScan = sscanf(buf,"%d %s %d",&this->ipointID, &tempOP, &this->pointWaited);
		break;}
	case 43:{//sintaxe: [P_ID] [GETBY] [M_ID]
		this->type = 0;
		retScan = sscanf(buf,"%d %s %d",&this->ipointID, &tempOP, &this->pointWaited);
		break;}
	case 44:{//sintaxe: [P_ID] [GETBZ] [M_ID]
		this->type = 0;
		retScan = sscanf(buf,"%d %s %d",&this->ipointID, &tempOP, &this->pointWaited);
		break;}
	case 45: {//sintaxe: [P_ID] [ESNDB] <AUDIO> <OVER/3D> <VOLUME> <X> <Y> <Z>
		this->type = 2;
		retScan = sscanf(buf,"%d %s %s %s %lf %lf %lf %lf", &this->ipointID, &tempOP, &tempStr1, &tempStr2, &this->mathValue2, &this->x, &this->y, &this->z);
		//printf("\n -> %s", this->eMsg);
		switch(retScan){
		case 2: // CORRECT READING WITHOUT AUDIO
			break;
		case 3: // CORRECT READING WITH AUDIO AND NO OVERLOOP
			strcpy(this->audioFilename,tempStr1);
			this->audioOverplay = false;
			break;
		case 4: // CORRECT READING WITH AUDIO AND OVERLOOP
			strcpy(this->audioFilename,tempStr1);
			if( strcmp(tempStr2,"OVER") == 0) { this->audioOverplay = true; } else {this->audioOverplay = false; }
			if( strcmp(tempStr2,"3D") == 0)		{ this->audio3D = 1; this->mathValue2 = 1;}
			break;
		case 8: // CORRECT READING WITH AUDIO 3D 
			if( strcmp(tempStr2,"3D") == 0)		{ this->audio3D = 1;}
			break;
		default:break;};
		break;}
	case 46: {//sintaxe: [P_ID] [N255B] [MAXValue]
		this->type = 0;
		retScan = sscanf(buf,"%d %s %lf", &this->ipointID, &tempOP, &this->mathValue);
		break;			 }
	default:
		break;
	}

	//UNDERSTAND MODE
	if(strcmp(tempMode,"HIDE" )  == 0)		{this->pointMode = 0;}
	if(strcmp(tempMode,"ONLY_BALL")  == 0)	{this->pointMode = 1;}
	if(strcmp(tempMode,"ONLY_OBJECT") == 0)	{this->pointMode = 2;}
	if(strcmp(tempMode,"BOTH") == 0)		{this->pointMode = 3;}
	if(strcmp(tempMode,"FLASH_BALL")  == 0)	{this->pointMode = 4;}
	if(strcmp(tempMode,"GHOST")  == 0)		{this->pointMode = 5;}
	if(strcmp(tempMode,"SENSE_PROX") == 0)	{this->pointMode = 6;}
	if(strcmp(tempMode,"ALL_OBJECTS") == 0)	{this->pointMode = 7;}
	if(strcmp(tempMode,"KEEP_LAST") == 0)	{this->pointMode = 8; }

	return 1;
}