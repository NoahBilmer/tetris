#ifndef BOARD_H
#define BOARD_H
#include "globals.h"

int writeBlocks(int x, int y, int block[][SHAPE_SIZE], int board[][ROWS]);
int colliding(int x, int y, int block[][SHAPE_SIZE], int board[][ROWS]);
void getInput(int* wishX, int* wishY, int*wishRotate, int* wishFastFall,int *moveFrameCount);
void clearBoard(int board[][ROWS]);

#endif