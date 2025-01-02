#ifndef DRAW_H
#define DRAW_H
#include "raylib.h"

extern Texture2D gameboardUI;
extern Texture2D blocksSpriteSheet;
extern Texture2D digitsSpriteSheet;
extern Texture2D titleText;
extern Texture2D githubLink;
extern Texture2D levelSelectInfoText;
extern Texture2D hitEnterText;
extern Texture2D levelText;


void drawTitleScreen();
void drawGameState(State* state);
void drawScore();
void drawLevel(int level,int x,int y);
void drawBoard();
void drawBlock();
void drawNextPiece(State* state);

#endif