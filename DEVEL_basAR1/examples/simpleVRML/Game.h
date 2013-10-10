#ifndef Game_h
#define Game_h

class Arpe;

class Game {

 public:

    Game();
    ~Game();

    int correct;
    int mistake;

    Arpe *myArpe;
};

#endif // Game_h
