#pragma once

#include <list>
using namespace std;

#include <iRulesInput.h>

class iRulesState
{
public:
	//State
		//Attributes
			int		m_id;
			int		m_active;
		//Methods

	//iPoint
		//Attirbutes
			list	<iRulesInput> listInput;
			int		n_inputs;
		//Methods
			void	addInput(iRulesInput value);

	iRulesState(void);
	~iRulesState(void);
};

