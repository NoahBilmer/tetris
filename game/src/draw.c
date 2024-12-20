#include "raylib.h"
#include "headers/globals.h"
#include "headers/draw.h"
#include <stdio.h>
#include <math.h>


/**
* FUNCTION: drawGameState() 
* DESCRIPTION: draws the state of the game on this current frame. 
*/ 
void drawGameState() {
    // Draw the UI texture to screen, properly scaled
    drawScore();
    drawLevel(level);   
    drawBoard(landedBoard);
    drawBoard(fallingBoard);
    drawNextPiece();
}


/**
* FUNCTION: drawScore() 
* DESCRIPTION: Draws the current score to the screen.
*/ 
void drawScore() {
    int digit = 0;
    int digitArr[6] = {0,0,0,0,0,0};
    int index = 5;
    uint8_t scoreCpy = score;
    while(scoreCpy)
    {
        digitArr[index] = scoreCpy % 10;
        scoreCpy /= 10;
        index--;
    }
    for (int i = 0; i < 6; i++) {    
       DrawTexturePro(digitsSpriteSheet, (Rectangle){ 0.0f, (digitArr[i])*44.2f, 32,42 }, 
            (Rectangle){ (570 + 32*i),100,
            digitsSpriteSheet.width,44.5f}, 
            (Vector2){0,0}, 
            0.0f, 
            WHITE);
    }  
}

/**
* FUNCTION: drawLevel() 
* DESCRIPTION: Draws the provided level to the screen.
*/ 
void drawLevel(int level) {
    int digitArr[2] = {0,0};
    int index = 1;
    while(level)
    {
        digitArr[index] = level % 10;
        level /= 10;
        index--;
    }
    for (int i = 0; i < 2; i++) {
        DrawTexturePro(digitsSpriteSheet, (Rectangle){ 0.0f, (digitArr[i]*44.2f), 32,42 }, 
            (Rectangle){ (60 + 32*i),120,
            digitsSpriteSheet.width,44.5f}, 
            (Vector2){0,0}, 
            0.0f, 
            WHITE);

    }
}

/**
* FUNCTION: drawBoard() 
* DESCRIPTION: Draws the provided board to the screen.
*/ 
void drawBoard(uint8_t (*board)[ROWS]) {
    for (int row = 3; row < ROWS; row++) {
        for (int col = 0; col < COLUMNS; col++) {
            if (board[row][col] != 0)      
            drawBlock(col,row - 3,board[row][col]);
        }
    }
}

/**
* FUNCTION: drawBlock() 
* DESCRIPTION: Draws the provided block to the screen.
*/ 
void drawBlock(int x, int y, int color) {
    color--; // we start at 0 here but the blocks in the array start at 1, so subtract 1.
    unsigned blockHeight = 32;
    DrawTexturePro(blocksSpriteSheet, (Rectangle){ 0.0f, color*32.0f, 32,32 }, 
            (Rectangle){ (199.0f + 32*x), (57.0f + 32*(y)),
            blocksSpriteSheet.width,blockHeight}, 
            (Vector2){0,0}, 
            0.0f, 
            WHITE);

}

/**
* FUNCTION: drawNextPiece()
* DESCRIPTION: Draws the next piece to the screen. 
*/ 
void drawNextPiece() { 
    for (int i = 0; i < SHAPE_SIZE; i++) {
            for (int j = 0; j < SHAPE_SIZE; j++) {
                if (nextPiece[0][i][j] > 0)
                    drawBlock(13 + j,5 + i,nextPiece[0][i][j]);
            }
        }
}