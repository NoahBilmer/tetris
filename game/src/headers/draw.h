#ifndef DRAW_H
#define DRAW_H
#include "raylib.h"

extern Texture2D gameboardUI;
extern Texture2D blocksSpriteSheet;
extern Texture2D digitsSpriteSheet;
extern Texture2D backgroundColor;
extern Texture2D titleText;
extern Texture2D githubLink;
extern Texture2D levelSelectText;

void drawGameState();
void drawScore();
void drawLevel(int level);
void drawBoard();
void drawBlock();
void drawNextPiece();

#endif