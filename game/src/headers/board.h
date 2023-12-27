#ifndef BOARD_H
#define BOARD_H

int writeBlocks(int x, int y, int block[][4], int board[][23]);
int colliding(int x, int y, int block[][4], int board[][23]);

#endif