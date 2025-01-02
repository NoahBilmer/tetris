#include "headers/globals.h"
#include "headers/board.h"
#include "raymath.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

uint32_t doScoreCalculations(uint8_t lines, int level);
void addScore(State* state);
GameStateEnum playClearAnimation(State* state);
void transitionBackgroundHue(Color* backgroundColor);

// Global state variables )
int colorFadeDir = -1;
uint8_t row = 0;
uint8_t col = 0;
uint8_t color = 1; // start at 1 because 0 will just display nothing.
uint16_t animationFrameCounter = 0;
Color titleScreenBackground;

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
        state->newRotation = 0;
        state->rotation = state->newRotation;
        memcpy(state->currentPiece,state->nextPiece,sizeof(int[ROTATION_COUNT][SHAPE_SIZE][SHAPE_SIZE]));
        getNewPiece(state->nextPiece);
        // check if we're colliding at the start x/y; this is the game over condition 
        if (colliding(state->x,state->y,state->currentPiece[state->newRotation],state->landedBoard)) {
            return GAME_OVER;
        }
    }
            
    /*** Do all piece translations and or transformations ***/
    // Rotate the piece if we are not colliding 
    if (state->wishRotate && colliding(state->x,state->y,state->currentPiece[state->newRotation],state->landedBoard) == 0) {
            state->rotation = state->newRotation;
    }
    // Move in the x direction if we are not colliding.
    if (colliding(state->x + state->moveVec.x,state->y,state->currentPiece[state->rotation],state->landedBoard) == 0) {
            state->x = state->x + state->moveVec.x;             
    }
    /* Move in the y direction if we are not colliding. */
    if (colliding(state->x,state->y + state->moveVec.y,state->currentPiece[state->rotation],state->landedBoard) > 0) {
        // Place this piece
        // create a new piece next iteration
        state->newPiece = TRUE; 
        writeBlocks(state->x,state->y,state->currentPiece[state->rotation],state->landedBoard,FALSE);
        return ANIMATION_MUZZLE_FLASH;
    }
    /* NOT colliding in the y dimension; move the piece down one.*/
    else {
        state->y = state->y + state->moveVec.y; 
    }
    // Write to the fallingBoard
    if (state->wishRotate == TRUE || state->moveVec.x != 0 || state->moveVec.y != 0) {
        clearBoard(state->fallingBoard);
        writeBlocks(state->x,state->y,state->currentPiece[state->rotation],state->fallingBoard,FALSE);
    }

    if (checkLineClears(state->landedBoard,state->rowsToClearArr) == TRUE) {
        // Add up our lines
        for (int i = 0; state->rowsToClearArr[i] != -1 && i < 4; i++) {
            state->lines = i + 1;
        }
        state->scoreToAdd = doScoreCalculations(state->lines,state->level);
        if (state->lines % 10 == 0) {
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
    writeBlocks(state->x,state->y,state->currentPiece[state->rotation],state->fallingBoard,TRUE);
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
        uint8_t row;
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
    addScore(state);
    
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
        clearBoard(state->fallingBoard);
        clearBoard(state->landedBoard);
        row = 0;
        col = 0;
        state->score = 0;
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
    if (IsKeyPressed(KEY_ENTER)) 
        return MAIN_GAME_LOOP;
    if (IsKeyPressed(KEY_LEFT)) {
        state->level--;
        if (state->level < 0) 
            state->level = MAX_LEVEL;
    } else if (IsKeyPressed(KEY_RIGHT)) {
        state->level++;
        if (state->level == MAX_LEVEL + 1)
            state->level = 0;
    }
    if (IsKeyPressed(KEY_ESCAPE)) {
        return EXIT;
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
    uint32_t scoreToAdd;
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


/**
* FUNCTION: addScore(int* newScore) 
* DESCRIPTION: Gradually adds up our score untill we get to our desired new score value.
*/ 
void addScore(State* state) {
    if (state->score < (state->scoreToAdd + state->score)) {
        /* scale the speed at which we tally up our points based on the value of newScore.
        *  We do need to ensure that the time it takes to tally up score is not greater than the time it
        *  takes to complete the line clearing animation, because the player could overwrite newScore while we
        *  are tallying up the points if it is not fast enough. */  
        if (state->scoreToAdd < 10) {
            state->score += 1;
        }
        else if (state->scoreToAdd <= 400) {
            state->score += 6;
        }
        else if (state->scoreToAdd <= 1200) {
            (state->score) += 32;
        }
        else if (state->scoreToAdd < 2000) {
            (state->score) += 100;
        }
        else if (state->scoreToAdd < 10000) {
            (state->score) += 1000;
        }
        else {
            state->score = state->scoreToAdd + state->score;
            state->scoreToAdd = 0;
        }
    }
    printf("Score: %d\nScoreToAdd:  %d\n",state->score,state->scoreToAdd);
}


/**
* FUNCTION: playClearAnimation()
* DESCRIPTION: Plays the clear animation
*/ 
GameStateEnum playClearAnimation(State* state) {
    animationFrameCounter++;
    // every 10 frames we should either change all lines to clear white,
    // or change them to 0 which will show the landedBoard. 
    if ((animationFrameCounter % 10) == 0) {
        uint8_t row;
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
        animationFrameCounter = 0;
        return MAIN_GAME_LOOP;
    }
    addScore(state);
    
    return ANIMATION_CLEAR_BLOCKS;
}
