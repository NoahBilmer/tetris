#ifndef DRAW_H
#define DRAW_H
#include "raylib.h"
#include "state.h"

extern Texture2D gameboardUI;
extern Texture2D blocksSpriteSheet;
extern Texture2D digitsSpriteSheet;
extern Texture2D titleText;
extern Texture2D githubLink;
extern Texture2D levelSelectInfoText;
extern Texture2D hitEnterText;
extern Texture2D levelText;


void drawTitleScreen(uint8_t level);
void drawGameState(State* state);
void drawScore(uint32_t score);
void drawLevel(int level,int x,int y);
void drawBlock(int x, int y, int color);
void drawBoard(uint8_t (*board)[ROWS]);
void drawNextPiece(State* state);

#endif