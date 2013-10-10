#ifndef Action_h
#define Action_h

class State;

class Action {

 public:

    Action();

    ~Action();

	int			cleanAction(int valueID);
	int			fillAction(char *buf);
	int			playAudio();

	int			type; // 0 - Config // 1 - Work // 2 - extern // 

	int			id;
    int			baseID;
    int			ipointID;

    int			opcode;
	char		opcodeName[10];
    int			pointMode; // 0-HIDE, 1-ONLY_BALL, 2-ONLY_OBJECT, 3-BOTH, 4-FLASH_BALL, 5-GHOST, 6-SENSE_PROX, 7-ALL_OBJECTS
    int			modelToChange;
	int			nextState;
	int			pointWaited;

    bool		generateError;
    
	char		audioFilename[256];
    bool		audioOverplay;
	int			audio3D; // 0 - 2D, 1 - 3D
    
	char		arduinoCommand[256];
	char		eMsg[256];

	int			cmpAction;	// 1 - GREATER, 2 - LESSER, 3 - EQUALS
	double		mathValue;
	double		mathValue2;

	//Config actions 
	double		x;
	double		y;
	double		z;

	double		time;
	double		step;

	State*		myState;

	
};

#endif // Action_h
