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
int writeBlocks(int x, int y, int block[][4], int board[][23]) {
    int offsetRow,offsetCol;
    // we are colliding with either another piece or beyond the edge of the baord.
    if (colliding(x,y,block,board)) {
        return FALSE; 
    }
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
* Checks to see if a piece has any non-zero values colliding with other non-zero values
* on the game board. Also checks to see if non-zero pieces are outside the game board.
* RETURN: false if coliding with another piece or being placed outside the game board.
*         true on success.
*/ 
int colliding(int x, int y, int block[][4], int board[][23]) {
    int offsetRow,offsetCol;
    for (int row = 0; row < SHAPE_SIZE; row++) {
        for (int col = 0; col < SHAPE_SIZE; col++) {
            offsetRow = row + y; 
            offsetCol = col + x;
            // if we are attemping to place a piece outside the bounds of an array return false.
            // only NON-ZERO elements are considered, meaning pieces should be able to go to all the way of the edge of the board
            if (block[row][col] > 0 && ((offsetRow >= (ROWS) || offsetRow < 0) || (offsetCol >= (COLUMNS)) || offsetCol < 0)) {
                return TRUE;
            } 
            else if ((block[offsetRow][offsetCol] > 0 && board[row][col] > 0)) {
                return TRUE;
            }
        }
    }
    return FALSE;
}