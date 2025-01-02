#ifndef STATE_H
#define STATE_H
#include "globals.h"

GameStateEnum MainGameLoop(State* state);
GameStateEnum MuzzleFlash(State* state);
GameStateEnum ClearBlocks(State* state);
GameStateEnum GameOver(State* state);
GameStateEnum TitleScreen(State* state);

#endif