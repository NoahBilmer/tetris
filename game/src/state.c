#include "headers/globals.h"
#include "headers/board.h"
#include "headers/state.h"
#include "raymath.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

uint32_t doScoreCalculations(uint8_t lines, int level);
void addScore(State* state);
GameStateEnum playClearAnimation(State* state);
void transitionBackgroundHue(Color* backgroundColor);

// Global state variables; all used for animation purposes 
int colorFadeDir = -1;
uint8_t row = 0;
uint8_t col = 0;
uint8_t color = 1; // start at 1 because 0 will just display nothing.
uint16_t animationFrameCounter = 0;
Color titleScreenBackground;

/**
* FUNCTION: setupDefaultValues() 
* DESCRIPTION: Sets up all the default values for everything in the State struct.
*/ 
void setupDefaultValues(State* state) {
    state->moveVec.x = 0;
    state->moveVec.y = 0;
    state->wishFastFall = FALSE;
    state->x = START_X;
    state->y = START_Y;
    state->wishRotate = 0;
    state->fastFallSpeed = 1;
    state->speed = levelSpeedArr[state->level];
    state->state = TITLE_SCREEN;
    state->titleScreenBackground = ((Color) {221,208,242,255});
    state->lines = 0;
    state->score = 0;
    state->scoreToAdd = 0;
    state->level = state->startLevel;
    state->newPiece = TRUE;
    for (int i = 0; i < 4; i++) 
        state->rowsToClearArr[i] = -1;
}

/**
* FUNCTION: initGameState() 
* DESCRIPTION: Sets up all the initial values for everything in the State struct.
*/ 
void initState(State* state) {
    setupDefaultValues(state);
    state->landedBoard =  (uint8_t (*)[23])calloc(ROWS*COLUMNS,sizeof(uint8_t*));
    state->fallingBoard = (uint8_t (*)[23])calloc(ROWS*COLUMNS,sizeof(uint8_t*)); 
    state->nextPieceIndex = getNewPieceIndex();
}

/**
* FUNCTION: resetState() 
* DESCRIPTION: resets the gamestate to match the default values.
*/ 
void resetState(State* state) {
   setupDefaultValues(state);
   clearBoard(state->fallingBoard);
   clearBoard(state->landedBoard);
}

/**
* FUNCTION: MainGameLoop() 
* DESCRIPTION: Does all the required game logic for this frame.
* RETURNS: Returns the next state.
*/ 
GameStateEnum MainGameLoop(State* state) {
     // move the piece down if enough frames have elapsed.
    if (state->frameCount > state->speed) {
        state->frameCount = 0;
        state->moveVec.y = 1;
    }
    
    // Create a new Piece
    if (state->newPiece) {
        clearBoard(state->fallingBoard);
        state->x = START_X;
        state->y = START_Y;
        state->wishRotate = FALSE;
        state->newPiece = FALSE;
        state->nextRotation = 0;
        state->rotation = state->nextRotation;
        state->currentPieceIndex = state->nextPieceIndex;
        state->nextPieceIndex = getNewPieceIndex();
        // check if we're colliding at the start x/y; this is the game over condition 
        if (colliding(state->x,state->y,(*pieceMapArr[state->currentPieceIndex])[state->rotation],state->landedBoard)) {
            return GAME_OVER;
        }
    }
    if (state->wishFastFall && !(colliding(state->x,state->y + 1,(*pieceMapArr[state->currentPieceIndex])[state->rotation],state->landedBoard) > 0)) {
        state->speed = state->fastFallSpeed;
    }
    else {
        state->speed = levelSpeedArr[state->level];
    }
            
    /*** Do all piece translations and or transformations ***/
    // Rotate the piece if we are not colliding 
    if (state->wishRotate && colliding(state->x,state->y,(*pieceMapArr[state->currentPieceIndex])[state->nextRotation],state->landedBoard) == 0) {
            state->rotation = state->nextRotation;
    }
    // Move in the x direction if we are not colliding.
    if (colliding(state->x + state->moveVec.x,state->y,(*pieceMapArr[state->currentPieceIndex])[state->rotation],state->landedBoard) == 0) {
            state->x = state->x + state->moveVec.x;             
    }
    /* Move in the y direction if we are not colliding. */
    if (colliding(state->x,state->y + state->moveVec.y,(*pieceMapArr[state->currentPieceIndex])[state->rotation],state->landedBoard) > 0) {
        // Place this piece
        // create a new piece next iteration
        state->newPiece = TRUE; 
        writeBlocks(state->x,state->y,(*pieceMapArr[state->currentPieceIndex])[state->rotation],state->landedBoard,FALSE);
        return ANIMATION_MUZZLE_FLASH;
    }
    /* NOT colliding in the y dimension; move the piece down one.*/
    else {
        state->y = state->y + state->moveVec.y; 
    }
    // Write to the fallingBoard
    if (state->wishRotate == TRUE || state->moveVec.x != 0 || state->moveVec.y != 0) {
        clearBoard(state->fallingBoard);
        writeBlocks(state->x,state->y,(*pieceMapArr[state->currentPieceIndex])[state->rotation],state->fallingBoard,FALSE);
    }

    if (checkLineClears(state->landedBoard,state->rowsToClearArr) == TRUE) {
        // Add up our lines
        uint8_t linesClearedThisFrame = 0;
        for (int i = 0; state->rowsToClearArr[i] != -1 && i < 4; i++) {
            linesClearedThisFrame++;
        }
        //
        state->nextScore = doScoreCalculations(linesClearedThisFrame,state->level) + state->score;
    
        state->lines += linesClearedThisFrame;
        if (state->lines >= 10) {
            state->lines = 0;
            state->level++;
            state->speed = levelSpeedArr[state->level];
        }
        return ANIMATION_CLEAR_BLOCKS;
    }
    return MAIN_GAME_LOOP;
}


/**
* FUNCTION: MuzzleFlash() 
* DESCRIPTION: Creates a "muzzle flash" effect by flashing the piece white as it gets placed.
* RETURNS: Returns the next state.
*/ 
GameStateEnum MuzzleFlash(State* state) {
    animationFrameCounter++;
    clearBoard(state->fallingBoard);
    writeBlocks(state->x,state->y,(*pieceMapArr[state->currentPieceIndex])[state->rotation],state->fallingBoard,TRUE);
    if (animationFrameCounter > 1) {
        animationFrameCounter = 0;
        state->score += 2;
        return MAIN_GAME_LOOP;
    }
    return ANIMATION_MUZZLE_FLASH;
}

/**
* FUNCTION: MuzzleFlash() 
* DESCRIPTION: Creates a "muzzle flash" effect by flashing the piece white as it gets placed.
* RETURNS: Returns the next state.
*/ 
GameStateEnum ClearBlocks(State* state) {
    animationFrameCounter++;
    // every 10 frames we should either change all lines to clear white,
    // or change them to 0 which will show the landedBoard. 
    if ((animationFrameCounter % 10) == 0) {
        // Change the color of the lines to clear
        if (state->fallingBoard[state->rowsToClearArr[0]][0] == 8) {
            for (int i = 0; state->rowsToClearArr[i] != -1 && i < 4; i++) {
                toggleRowColor(state->fallingBoard,state->rowsToClearArr[i],0);
            }
        } else {
            for (int i = 0; state->rowsToClearArr[i] != -1 && i < 4; i++) {
               toggleRowColor(state->fallingBoard,state->rowsToClearArr[i],8);
            }
        }   
    }
    // Animation is done after 50 frames, return to the main game loop.
    if (animationFrameCounter > 50) {
        clearLineRows(state->landedBoard,state->rowsToClearArr);
        for (int i = 0; i < 4; i++) {
                if (state->rowsToClearArr[i] != -1)
                    state->lines++;
                state->rowsToClearArr[i] = -1;
            }
        animationFrameCounter = 0;
        return MAIN_GAME_LOOP;
    }
    
   if (state->score >= state->nextScore)  {
        state->score = state->nextScore;
   } else {
       state->score += state->nextScore / 50;
   }
    

        
    
    return ANIMATION_CLEAR_BLOCKS;
}

/**
* FUNCTION: GameOver() 
* DESCRIPTION: Executes the game over animation. we fill up the
* screen with blocks and iterate the global color variable for each block to create a pattern
* RETURNS: Returns the next state.
*/ 
GameStateEnum GameOver(State* state) {
   animationFrameCounter++;
    if (animationFrameCounter > 140) {
        row = 0; col = 0;
        resetState(state);
        animationFrameCounter = 0;
        return MAIN_GAME_LOOP; 
    } else {
        if (row < ROWS) {
            if (color > 7) {
                color = 1;
            }
            if (state->fallingBoard[row + 3][col] == 0) {
                state->fallingBoard[row + 3][col] = color;
            }
            if (state->fallingBoard[(ROWS - 1) - row][(COLUMNS - 1) - col] == 0) {
                state->fallingBoard[(ROWS - 1) - row][(COLUMNS - 1) - col] = color;   
            }
            (color)++;
            (col)++;
            if (col == COLUMNS) {
                row++;
                col = 0;
            }
        } 
    }
    return GAME_OVER;
}

/**
* FUNCTION: TitleScreen() 
* DESCRIPTION: Executes the game over animation. we fill up the
* screen with blocks and iterate the global color variable for each block to create a pattern.
* RETURNS: Returns the next state.
*/ 
GameStateEnum TitleScreen(State* state) {
    if (IsKeyPressed(KEY_ENTER)) {
        resetState(state);
        return MAIN_GAME_LOOP;
    }
    if (IsKeyPressed(KEY_LEFT)) {
        state->startLevel--;
        if (state->startLevel < 0) 
            state->startLevel= MAX_LEVEL;
    } else if (IsKeyPressed(KEY_RIGHT)) {
        state->startLevel++;
        if (state->startLevel == MAX_LEVEL + 1)
            state->startLevel = 0;
    }
    if (IsKeyPressed(KEY_ESCAPE)) {
        #ifndef PLATFORM_WEB
             return EXIT;
        #endif
    }
    transitionBackgroundHue(&state->titleScreenBackground);
    return TITLE_SCREEN;
}

/**
* FUNCTION: transitionBackgroundHue()
* DESCRIPTION: transitions the background hue to a slightly different color every frame. 
* We use this in the title screen to create an interesting background color effect.
*/ 
void transitionBackgroundHue(Color* backgroundColor) { 
    int MAX_RED = 213;
    int MAX_GREEN = 217;
    int MAX_BLUE = 224;
    int MIN_RED = 100;
    int MIN_GREEN = 120;
    int MIN_BLUE = 180;
    
    if (backgroundColor->r == MAX_RED && backgroundColor->g == MAX_GREEN && backgroundColor->b == MAX_BLUE) {
        colorFadeDir = -1;
    } else if (backgroundColor->r == MIN_RED && backgroundColor->g == MIN_GREEN && backgroundColor->b == MIN_BLUE) {
        colorFadeDir = 1;
    }

    backgroundColor->r += colorFadeDir;
    backgroundColor->g += colorFadeDir;
    backgroundColor->b += colorFadeDir;
    // Clamp our color values to ensure they stay between our desired range.
    backgroundColor->r = Clamp(backgroundColor->r,MIN_RED,MAX_RED);
    backgroundColor->g = Clamp(backgroundColor->g,MIN_GREEN,MAX_GREEN);
    backgroundColor->b = Clamp(backgroundColor->b,MIN_BLUE,MAX_BLUE); 
    
}



/**
* FUNCTION: doScoreCalculations() 
* DESCRIPTION: returns the score to add based on the number of lines the player has cleared.
*/ 
uint32_t doScoreCalculations(uint8_t lines, int level) {
    uint32_t scoreToAdd = 0;
    switch (lines) {
        case 1:
            scoreToAdd = (40 * (level + 1));
            break;
        case 2:
            scoreToAdd = (100 * (level + 1));
            break;
        case 3:
            scoreToAdd = (300 * (level + 1));
            break;
        case 4:
            scoreToAdd = (1200 * (level + 1));
            break;
    }
    return scoreToAdd;
    
}
