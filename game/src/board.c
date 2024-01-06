#include <stdio.h>
#include "headers/board.h"
#include "headers/globals.h"

/**
* FUNCTION: writeBlocks() 
* DESCRIPTION: Attempts to write the desired piece to the board at a specific x y position.
* Also note that negative x,ys may be used, becuase the algorithm only cares about non-zero positons. 
* RETURNS: true on success to place the values
*          false if the piece is colliding with another piece
*          or will have non-zero values outside the board
*/
int writeBlocks(int x, int y, int block[][SHAPE_SIZE], int board[][ROWS]) {
    int offsetRow,offsetCol;
    // Loop through 
    for (int row = 0; row < SHAPE_SIZE; row++) {
        for (int col = 0; col < SHAPE_SIZE; col++) {      
            if (block[row][col] > 0) {
                offsetRow = row + y; 
                offsetCol = col + x;
                board[offsetRow][offsetCol] = block[row][col];
            }
            
        }
    }
    return TRUE;
    
}

/**
* FUNCTION: colliding() 
* DESCRIPTION: Checks to see if a piece has any non-zero values colliding with other non-zero values
* on the game board. Also checks to see if non-zero pieces are outside the game board.
* RETURN: 1 if coliding with another piece.
*         2 if being placed outside the game board.
*         0/false if no collisions.
*/ 
int colliding(int x, int y, int block[][SHAPE_SIZE], int board[][ROWS]) {
    int offsetRow,offsetCol;
    for (int row = 0; row < SHAPE_SIZE; row++) {
        for (int col = 0; col < SHAPE_SIZE; col++) {
            offsetRow = row + y; 
            offsetCol = col + x; 
            // if we are attemping to place a piece outside the array in the X dimension
            if ((block[row][col] > 0 && ((offsetCol >= (COLUMNS) || offsetCol < 0) || (offsetRow >= (ROWS) || offsetRow < 0)))) {
                return COLLISION_OUT_OF_BOUNDS;
            }
            // if we are attemping to place a piece at the same position of another piece
            else if (((block[row][col] > 0) && (board[offsetRow][offsetCol] > 0))) {
                return COLLISION_BLOCK_ALREADY_EXISTS;
            }
        }
    }
    return FALSE;
}

/**
* FUNCTION: clearBoard() 
* DESCRIPTION: Clears the specified board.
*/ 
void clearBoard(int board[][ROWS]) {
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLUMNS; col++) {
            board[row][col] = 0;
        }
    }
}
