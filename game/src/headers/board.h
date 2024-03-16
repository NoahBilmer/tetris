#ifndef BOARD_H
#define BOARD_H
#include "globals.h"

int writeBlocks(int x, int y, int block[][SHAPE_SIZE], int board[][ROWS], int flashPiece);
int colliding(int x, int y, int block[][SHAPE_SIZE], int board[][ROWS]);
void clearBoard(int board[][ROWS]);
int checkLineClears(int board[][ROWS], int rowToClear[4]);
void toggleRowColor(int board[][ROWS], int row, int color);
void clearLineRows(int board[][ROWS],int rowsToClear[4]);

#endif