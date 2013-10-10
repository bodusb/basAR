#include "State.h"
#include <stdio.h>

#include <list>
#include "Action.h"
#include "iPoint.h"
#include "Base.h"
#include "Rules.h"
#include "Arpe.h"

using namespace std;

void State::addAction(Action* value){
	list<Action*>::iterator it;
	it = listAction.begin();
	listAction.push_back(value);}

int State::parseState(){

	State *s = this;
	int retValue = 0;

	list<Action*>::iterator itA;

	//	printf("\n Size of action list to parse: %d", this->listAction.size());

	for( itA = this->listAction.begin() ; itA != this->listAction.end() ; itA++){
		iPoint* ip = new iPoint();
		Action *lastAction = new Action();
		ip = this->myRules->myArpe->findIPoint((*itA)->ipointID);
		//(*(*(myRules)).myArpe).findIPoint((*(*itA)).ipointID);
		printf("\n	%s", ip->name); printf(" pID: %d", ip->id);// printf(" ListAction: %d", listAction); //printf(" This: %d", this);
		lastAction = (*ip).actualAction;
		
		switch( (*itA)->type) {
		case 0:{ // CONFIGURATION ACTION, APPY RIGHT NOW
			ip->configAction = (*itA);
			printf("(%s)",ip->configAction->opcodeName);
			switch( (*(*itA)).opcode){
				//CONFIGURATION COMMANDS
			case 12:{ // CHGVM
				(*(this->myRules)).chgvm(ip);
				break;}
			case 13:{	// TRA
				(*(this->myRules)).tra(ip);
				break;}
			case 14:{	// ROT
				(*(this->myRules)).rot(ip);
				break;}
			case 15:{	// SCL
				(*(this->myRules)).scl(ip);
				break;}
			case 16:{	// CHGM
				(*(this->myRules)).chgm(ip);
				break;}
			case 17:{	// SETS
				(*(this->myRules)).sets(ip);
				break;}
			case 18:{	// SETL
				(*(this->myRules)).setl(ip);
				break;}
			case 19:{	// GETS
				(*(this->myRules)).gets(ip);
				break;}
			case 20:{	// GETL
				(*(this->myRules)).getl(ip);
				break;}
			case 23:{	// CHGNM
				(*(this->myRules)).chgnm(ip);
				break;}

					// MATH COMMANDS
			case 24: {	//sintaxe: [P_ID] [ADDA] [Value]
				this->myRules->adda(ip);
				break;}
			case 25: {	//sintaxe: [P_ID] [ADDB] [Value]
				this->myRules->addb(ip);
				break;}
			case 26: {	//sintaxe: [P_ID] [AMB]
				this->myRules->amb(ip);
				break;}
			case 27: {	//sintaxe: [P_ID] [BMA]
				this->myRules->bma(ip);
				break;}
			case 28: {	//sintaxe: [P_ID] [NEGA]
				this->myRules->nega(ip);
				break;}
			case 29: {	//sintaxe: [P_ID] [NEGB]
				this->myRules->negb(ip);
				break;}
			case 30: {	//sintaxe: [P_ID] [MULA] [Value]
				this->myRules->mula(ip);
				break;}
			case 31: {	//sintaxe: [P_ID] [MULB] [Value]
				this->myRules->mulb(ip);
				break;}
			case 32: {	//sintaxe: [P_ID] [SWAB]
				this->myRules->swab(ip);
				break;}
			case 33: {	//sintaxe: [P_ID] [CMP] [GREATER/LESSER/EQUALS] [NEXTSTATE]
				this->myRules->cmp(ip);
				if( this->myRules->nextStateMath != -1){
					//(*ip).actualAction = lastAction;
					printf(" A:%3.2f, B:%3.2f, NSM:%d",ip->A,ip->B,this->myRules->nextStateMath);
					return 1;}
				break;}
			case 34: {	//sintaxe: [P_ID] [CMPV] [Value] [GREATER/LESSER/EQUALS] [NEXTSTATE]
				this->myRules->cmpv(ip);
				if( this->myRules->nextStateMath != -1){
					//(*ip).actualAction = lastAction;
					printf(" A:%3.2f, B:%3.2f, NSM:%d",ip->A,ip->B,this->myRules->nextStateMath);
					return 1;}
				break;}
			case 35: {	//sintaxe: [P_ID] [LOADA] [Value] 
				this->myRules->loada(ip);
				break;}
			case 36: {	//sintaxe: [P_ID] [LOADB] [Value] 
				this->myRules->loadb(ip);
				break;}
			case 37: {  //sintaxe: [P_ID] [DIST] [A_ID]
				this->myRules->dist(ip);
				break;}
			case 38: {//sintaxe: [P_ID] [LOADQ] [Value]
				this->myRules->loadq(ip);
				break;}
			case 39:{//sintaxe: [P_ID] [GETQ]
				this->myRules->getq(ip);
				break;}
			case 40:{//sintaxe: [P_ID] [SETQ]
				this->myRules->setq(ip);
				break;}
			case 41:{//sintaxe: [P_ID] [RANDI] [MIN] [MAX]
				this->myRules->randi(ip);
				break;}
			case 42:{//sintaxe: [P_ID] [GETBX] [M_ID]
				this->myRules->getbx(ip);
				break;}
			case 43:{//sintaxe: [P_ID] [GETBY] [M_ID]
				this->myRules->getby(ip);
				break;}
			case 44:{//sintaxe: [P_ID] [GETBZ] [M_ID]
				this->myRules->getbz(ip);
				break;}
			case 46:{//sintaxe: [P_ID] [N255B] [MAXVALUE]
				this->myRules->n255b(ip);
				break;}
			default: break;};
				//(*ip).actualAction = lastAction;
				printf(" A:%3.2f, B:%3.2f, Q:%d",ip->A,ip->B,this->myRules->queueIndex);
				break;}

		case 1:{ // NORMAL WORK ACTIONS, PARSE TO IPOINT
			(*ip).actualAction = (*itA);
			printf("(%s)",ip->actualAction->opcodeName);
			//ONLY NORMAL WORK ACTIONS CHANGE THE VISUALIZATION MODE
			if( (*ip).actualAction->pointMode != 8 )  // pointMode = 8 is KEEP_LAST, doesn't change
				(*ip).viewMode = (*ip).actualAction->pointMode;
			printf(" Q:%d",this->myRules->queueIndex);
			break;}

		case 2:{ // EXTERNAL POINT ACTION, APPLY RIGHT NOW
			//printf("\n EXTERNAL POINT ACTION");
			(*ip).actualAction = (*itA);
			printf("(%s)",ip->actualAction->opcodeName);
			switch( (*(*itA)).opcode){
			case 21:{
				this->myRules->esnd(ip);
				printf("\n HERE OUT OF ESND, %d", (*itA)->opcode);
				break;}
			case 22:{	// ERCV
				this->myRules->ercv(ip);
				break;}
			case 45:{
				this->myRules->esndb(ip);
				printf("\n HERE OUT OF ESND, %d", (*itA)->opcode);
				break;}
			default: break;};

				break;}
		default: 
			printf("\n %s", ip->name); printf(" pID: %d", ip->id); printf(" ListAction: %d", listAction); printf(" This: %d", this);
			printf("\n Action %d parsed on ipoint %d", (*itA)->id, ip->id); printf(" ListAction: %d", listAction); printf(" This: %d", this);		
			
			break;
		}

	//printf("\n ---------------------------------------------------------------------------");
	}
	//printf("\n State %d parsed", this->id);
	printf("\n Returning to main Interaction thread");
	return 1;
}

State::State(){

	id = 0;
	onUse = 0;
	waiting = 0;
	parsedTime = 0;
	time = 0;
	nextState = -1; //if !=0 "CONFIGURATION STATE"
}

State::~State(){
}
