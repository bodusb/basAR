#pragma once
class iRulesInput
{
public:
	//Input
		//Attributes
			int	m_id;
		//Methods
	
	//Where to apply
		//Attributes
			int	m_iPoint;
			int m_iBase;
		//Methods

	//Action
		//Attributes
			int	m_opcode;
			int m_pointMode;
			int m_nextState;
			int m_pointWaited;
			int m_modelToChange;
			int m_generateError;
		//Methods		
		
	//Audio
		//Attributes
			char	a_action_filename[256];
			int		a_overplay;		// 0 - Stop earlier // 1 - Play over the earlier
		//Methods
	
	iRulesInput(void);
	~iRulesInput(void);
};

