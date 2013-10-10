#include "iRulesState.h"


iRulesState::iRulesState(void)
{
}


iRulesState::~iRulesState(void)
{
}

void iRulesState::addInput(iRulesInput value){
	
	list<iRulesInput>::iterator it;
	it = this->listInput.begin();
	this->listInput.push_back(value);

}