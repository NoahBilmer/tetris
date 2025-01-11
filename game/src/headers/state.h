#ifndef STATE_H
#define STATE_H

typedef struct State {
    Vector2 moveVec;
    int x;
    int y;
    bool wishFastFall;
    bool wishRotate;
    bool newPiece;
    uint8_t rotation;
    uint8_t nextRotation;
    uint8_t currentPieceIndex;
    uint8_t nextPieceIndex;
    uint8_t speed;
    uint8_t fastFallSpeed;
    GameStateEnum state;
    Color titleScreenBackground;
    uint16_t frameCount;
    uint8_t lines;
    int8_t level;
    int8_t startLevel;
    uint32_t score;
    uint32_t scoreToAdd;
    uint32_t nextScore; 
    int8_t rowsToClearArr[4];
    uint8_t moveFrameCount;  

    // Board 
    uint8_t (*fallingBoard)[ROWS];
    uint8_t (*landedBoard)[ROWS];

} State;

void initState(State* state);
void resetState(State* state);
GameStateEnum MainGameLoop(State* state);
GameStateEnum MuzzleFlash(State* state);
GameStateEnum ClearBlocks(State* state);
GameStateEnum GameOver(State* state);
GameStateEnum TitleScreen(State* state);

#endif