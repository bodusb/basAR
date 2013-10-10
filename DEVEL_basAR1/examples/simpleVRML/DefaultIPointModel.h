#pragma once
#include <list>

using namespace std;

class DefaultIPointModel
{
public:
		
	int		m_id;
	int		n_actionModels;
	int		m_type;	//1 - VRML ; 2 - ASSIMP
	int		m_modelHolding_VRMLID;
	int		m_modelCanWork_VRMLID;
	int		m_modelCannotWork_VRMLID;


	DefaultIPointModel(void);
	~DefaultIPointModel(void);
};

