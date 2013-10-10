#pragma once
#include <Actuator.h>

class iARTKMarker :  public Actuator
{
public:
	//ARTK Marker	
		//Attributes
			int		m_visible;
			
		//Methods
			//virtual int actFill() const;

	//Position
		//Attributes
			int		m_patternID;
			double	m_mkr_center[2];
			double	m_mkr_width;

	//OBJECT REPRESENTATION
		//Attibutes
			int		m_rep_VRML_ID;
			double	m_rep_trans[3];
			double	m_rep_scale[3];

	//MARKER COVER
		//Attributes
			int		m_cover_VRML_ID;




	iARTKMarker(void);
	~iARTKMarker(void);

private:
	char getBuff(char *buf, int n, FILE *fp);
};

