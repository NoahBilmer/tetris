#ifndef BOARD_H
#define BOARD_H
#include "globals.h"

int writeBlocks(int x, int y, uint8_t block[SHAPE_SIZE][SHAPE_SIZE], uint8_t (*board)[ROWS], int flashPiece);
int colliding(int x, int y, uint8_t block[SHAPE_SIZE][SHAPE_SIZE], uint8_t (*board)[ROWS]);
void clearBoard(uint8_t (*board)[ROWS]);
int checkLineClears(uint8_t (*board)[ROWS], int8_t rowToClear[4]);
void toggleRowColor(uint8_t (*board)[ROWS], int row, int color);
void clearLineRows(uint8_t (*board)[ROWS], int8_t rowsToClear[4]);
uint8_t getNewPieceIndex();

#endif