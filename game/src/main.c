/*******************************************************************************************
* Tetris in C by Noah Bilmer
********************************************************************************************/
#include <stdlib.h>
#include "raylib.h"
#include "raymath.h"
#include "headers/board.h"
#include "headers/draw.h"
#include "headers/globals.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

void updateGameState(State* state);
void getNewPiece(uint8_t (*piece)[SHAPE_SIZE][SHAPE_SIZE]);
uint32_t doScoreCalculations();
void addScore(uint32_t *scoreToAdd,uint32_t newScore);
void renderGraphics(State* screen);
void playClearAnimation(State* state);
void transitionBackgroundHue();
void getInput(State* state,int* moveFrameCount);
void initGameState(State* state);

uint32_t score = 0;

// Extern variable declarations 
int8_t level = 7;
uint8_t lines = 0;
Texture2D gameboardUI;
Texture2D blocksSpriteSheet;
Texture2D digitsSpriteSheet;
Texture2D titleText;
Texture2D githubLink;
Texture2D levelSelectInfoText;
Texture2D hitEnterText;
Texture2D levelText;
RenderTexture2D target;

int frameCount = 0;
int animationFrameCount = 0;
int rowsToClearArr[4] = {-1,-1,-1,-1};

int colorFadeDir = -1;

int main(void)
{ 
    const int windowWidth = 820;
    const int windowHeight = 760;
    
    uint32_t scoreToAdd = 0;    

    // Initialize the game state.
    State* gameState = malloc(sizeof(State));
    initGameState(gameState);

    srand(time(NULL));
    // Get our first piece.
    getNewPiece(gameState->nextPiece);

    // Enable config flags for resizable window and vertical synchro
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(windowWidth, windowHeight, "Tetris");
    // Move from bin/Debug to tetris
    gameboardUI = LoadTexture("resources/tetris-ui.png");
    blocksSpriteSheet = LoadTexture("resources/blocks.png");
    digitsSpriteSheet = LoadTexture("resources/digits.png");
    titleText = LoadTexture("resources/title.png");
    githubLink = LoadTexture("resources/github-link.png");
    levelSelectInfoText = LoadTexture("resources/select-level-text.png");
    levelText = LoadTexture("resources/level-text.png");
    hitEnterText = LoadTexture("resources/hit-enter-text.png");
    SetWindowMinSize(320, 240);
    SetExitKey(KEY_NULL);
    
    // Render texture initialization, used to hold the rendering result so we can easily resize it
    target = LoadRenderTexture(SCREEN_W, SCREEN_H); 
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);  // Texture scale filter to use

    SetTargetFPS(60);  // Set our game to run at 60 frames-per-second

    /* Our "new score" when a line is cleared. We gradually tally up the score 
    using addScore() rather than instantly adding it. */
    uint32_t newScore = 0;
    uint8_t row, col = 0;
    uint8_t  tempColorVar = 1;
    int exitFlag = 0;
    int moveFrameCount = 0;
    // Main game loop
    while (!WindowShouldClose() && !exitFlag)
    {
        gameState->speed = levelSpeedArr[level];
        // Get all the input for this current frame.
        getInput(gameState,&moveFrameCount);    
        
        // iterate the framecount
        frameCount++;
        
        switch (gameState->state) {
            case MAIN_GAME_LOOP:
                updateGameState(gameState);
                break;
            case ANIMATION_MUZZLE_FLASH:
                animationFrameCount++;
                clearBoard(gameState->fallingBoard);
                writeBlocks(gameState->x,gameState->y,gameState->currentPiece[gameState->rotation],gameState->fallingBoard,TRUE);
                if (animationFrameCount > 1) {
                    gameState->state = MAIN_GAME_LOOP;
                    animationFrameCount = 0;
                    scoreToAdd = 2;
                    newScore = score + scoreToAdd;
                }
                break;
            case ANIMATION_CLEAR_BLOCKS:
                animationFrameCount++;
                playClearAnimation(gameState);
                // add our score.
                if (animationFrameCount == 0) {
                    scoreToAdd = doScoreCalculations();
                    newScore = scoreToAdd + score;
                    for (int i = 0; i < 4; i++) {
                        if (rowsToClearArr[i] != -1)
                            lines++;
                        rowsToClearArr[i] = -1;
                    }
                    if (lines % 10 == 0) {
                        level++;
                        gameState->speed = levelSpeedArr[level];
                    }
                }
                break;
            
            case GAME_OVER:
                // Do the game over animation.
                animationFrameCount++;
                if (animationFrameCount > 140) {
                    clearBoard(gameState->fallingBoard);
                    clearBoard(gameState->landedBoard);
                    row = 0;
                    col = 0;
                    score = 0;
                    newScore = 0;
                    animationFrameCount = 0;
                    gameState->state = MAIN_GAME_LOOP; 
                } else {
                    if (row < ROWS) {
                        if (tempColorVar > 7) {
                            tempColorVar = 1;
                        }
                        if (gameState->fallingBoard[row + 3][col] == 0) {
                            gameState->fallingBoard[row + 3][col] = tempColorVar;
                        }
                        if (gameState->fallingBoard[(ROWS - 1) - row][(COLUMNS - 1) - col] == 0) {
                            gameState->fallingBoard[(ROWS - 1) - row][(COLUMNS - 1) - col]  = tempColorVar;   
                        }
                        tempColorVar++;
                        col++;
                        if (col == COLUMNS) {
                            row++;
                            col = 0;
                        }
                    } 
                }
                break;
            case TITLE_SCREEN:
                if (IsKeyPressed(KEY_ENTER)) 
                   gameState->state = MAIN_GAME_LOOP;
                if (IsKeyPressed(KEY_LEFT)) {
                    level--;
                    if (level < 0) 
                        level = MAX_LEVEL;
                } else if (IsKeyPressed(KEY_RIGHT)) {
                    level++;
                    if (level == MAX_LEVEL + 1)
                        level = 0;
                }
                if (IsKeyPressed(KEY_ESCAPE)) {
                    exitFlag = 1;
                    break;
                }
                transitionBackgroundHue(&gameState->titleScreenBackground);
                break;
        }
        // Add the score gradually every frame so long as there is still score to add.
        if (newScore > 0) {
            addScore(&newScore,scoreToAdd);    
        } 
        
        if (checkLineClears(gameState->landedBoard,rowsToClearArr) == TRUE && gameState->state != ANIMATION_CLEAR_BLOCKS) {
            gameState->state = ANIMATION_CLEAR_BLOCKS;
        }
       renderGraphics(gameState);      
    }

    // De-Initialization
    UnloadRenderTexture(target);        // Unload render texture
    CloseWindow();                      // Close window and OpenGL context
    free(gameState->fallingBoard);
    free(gameState->landedBoard);
    free(gameState);
    return 0;
}

void initGameState(State* state) {
    state->moveVec.x = 0;
    state->moveVec.y = 0;
    state->x = START_X;
    state->y = START_Y;
    state->wishRotate = 0;
    state->fastFallSpeed = 1;
    state->speed = levelSpeedArr[level];
    state->state = TITLE_SCREEN;
    state->titleScreenBackground = ((Color) {221,208,242,255});

    state->landedBoard = calloc(4,sizeof(int[ROWS][COLUMNS]));
    state->fallingBoard = calloc(4,sizeof(int[ROWS][COLUMNS])); 
    state->currentPiece = calloc(4,sizeof(int[ROTATION_COUNT][SHAPE_SIZE][SHAPE_SIZE]));
    state->nextPiece = calloc(4,sizeof(int[ROTATION_COUNT][SHAPE_SIZE][SHAPE_SIZE]));
}


/**
* FUNCTION: renderGraphics() 
* DESCRIPTION: render the graphics to the screen.
*/ 
void renderGraphics(State* state) {
    // Compute required framebuffer scaling   
    float scale = MIN((float)GetScreenWidth()/SCREEN_W,(float)GetScreenHeight()/SCREEN_H);
    switch (state->state) { 
        case TITLE_SCREEN:
        BeginTextureMode(target);
            ClearBackground(state->titleScreenBackground);
            drawTitleScreen();
        EndTextureMode();
        break;
        default:
        // Begin texture mode to draw to the target (essentially acts as a canvas)
        BeginTextureMode(target);
            DrawTexture(gameboardUI,0,0,WHITE); 
            drawGameState(state);
        EndTextureMode();
        break;
    }
    BeginDrawing();
        DrawTexturePro(target.texture, (Rectangle){ 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height },
            (Rectangle){ (GetScreenWidth() - ((float)SCREEN_W*scale))*0.5f, (GetScreenHeight() - ((float)SCREEN_H*scale))*0.5f,
            (float)SCREEN_W*scale, (float)SCREEN_H*scale }, (Vector2){ 0, 0 }, 0.0f, WHITE);
    ClearBackground(BLACK);     // Clear screen background
    EndDrawing();
}

/**
* FUNCTION: getInput()
* DESCRIPTION: Stores the user's current desired actions on the current frame.
* NOTE: moveFrameCount is used to ensure that holding down the left and right keys
*       move the piece at a manageable speed. 
*/ 
void getInput(State* state,int *moveFrameCount) {
        // reset input-related variables
        state->moveVec.x = 0;
        state->moveVec.y = 0;
        state->wishRotate = FALSE;
        if (IsKeyPressed(KEY_LEFT)) {
            state->moveVec.x -= 1;
            *moveFrameCount = 0;
        }
        else if (IsKeyDown(KEY_LEFT)) {
           (*moveFrameCount)++;
            if (*moveFrameCount > 8) {
                *moveFrameCount = 0;
                state->moveVec.x -= 1;
            } else {
                state->moveVec.x = 0;
            }
        }
        if (IsKeyPressed(KEY_RIGHT))
        {
            state->moveVec.x += 1;
        }
        else if (IsKeyDown(KEY_RIGHT)) {
            (*moveFrameCount)++;
            if (*moveFrameCount > 8) {
                *moveFrameCount = 0;  
                state->moveVec.x += 1;
            } else {
                state->moveVec.x = 0;
            }
        }

        if (IsKeyPressed(KEY_SPACE)) {
            state->wishRotate = TRUE;
            state->newRotation++;
            if (state->newRotation > 3) 
                state->newRotation = 0;
        }
        // If we wish to fastfall only do so when we are not about to collide in the Y direction
        // This is to make last-second adjustments feel consistent 
        if (IsKeyDown(KEY_DOWN) && !(colliding(state->x,state->y + 1,state->currentPiece[state->rotation],state->landedBoard) > 0)) {
           state->speed = state->fastFallSpeed;
        }
        else {
            state->speed = levelSpeedArr[level];
        }
        if (IsKeyReleased(KEY_DOWN)) {
            state->speed = levelSpeedArr[level];
        }
        if (IsKeyReleased(KEY_ESCAPE)) {
            state->state = TITLE_SCREEN;
        }
}

/**
* FUNCTION: updateGameState() 
* DESCRIPTION: Updates the board's state for the current frame based on player input.
*/ 
void updateGameState(State* state) {
        // move the piece down if enough frames have elapsed.
        if (frameCount > state->speed) {
            frameCount = 0;
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
                state->state = GAME_OVER;
                return;
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
            state->state = ANIMATION_MUZZLE_FLASH;
            state->newPiece = TRUE; 
            writeBlocks(state->x,state->y,state->currentPiece[state->rotation],state->landedBoard,FALSE);
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
}

/**
* FUNCTION: getNewPiece() 
* DESCRIPTION: Randomly selects a new piece to copy to the provided memory address.
*/ 
void getNewPiece(uint8_t (*piece)[SHAPE_SIZE][SHAPE_SIZE]) {
    uint8_t pieceIndex = (rand() % 7) + 1;
    uint8_t (*newPiece)[SHAPE_SIZE][SHAPE_SIZE];
    switch (pieceIndex) {
        case 1:
            newPiece = i_block;
            break;
        case 2:
            newPiece = o_block;
            break;
        case 3:
            newPiece = j_block;
            break;
        case 4:
            newPiece = l_block;
            break;
        case 5:
            newPiece = s_block;
            break;
        case 6:
            newPiece = z_block;
            break;
        case 7:
            newPiece = t_block;
            break;
    }
    // Copy the piece
    memcpy(piece,newPiece,sizeof(uint8_t[ROTATION_COUNT][SHAPE_SIZE][SHAPE_SIZE]));
}


/**
* FUNCTION: doScoreCalculations() 
* DESCRIPTION: returns the score to add based on the number of lines the player has cleared.
*/ 
uint32_t doScoreCalculations() {
    int lines = 0;
    uint32_t scoreToAdd;
    // Add up our lines
    for (int i = 0; rowsToClearArr[i] != -1 && i < 4; i++) {
        lines = i + 1;
    }
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
void addScore(uint32_t *newScore,uint32_t scoreToAdd) {
    printf("Score to add :%d\nNewScore: %d\n",scoreToAdd,*newScore);
    if (score < *newScore) {
        /* scale the speed at which we tally up our points based on the value of newScore.
        *  We do need to ensure that the time it takes to tally up score is not greater than the time it
        *  takes to complete the line clearing animation, because the player could overwrite newScore while we
        *  are tallying up the points if it is not fast enough. */  
        if (scoreToAdd < 10) {
            score += 1;
        }
        else if (scoreToAdd <= 400) {
            score += 6;
        }
        else if (scoreToAdd <= 1200) {
            score += 32;
        }
        else if (scoreToAdd < 2000) {
            score += 100;
        }
        else if (scoreToAdd < 10000) {
            score += 1000;
        }
        else {
            score = *newScore;
            *newScore = 0;
        }
    }
    else {
       score = *newScore;
       *newScore = 0;
    }
}

/**
* FUNCTION: playClearAnimation()
* DESCRIPTION: Plays the clear animation
*/ 
void playClearAnimation(State* state) {
    // every 10 frames we should either change all lines to clear white,
    // or change them to 0 which will show the landedBoard. 
    if ((animationFrameCount % 10) == 0) {
        uint8_t row;
        // Change the color of the lines to clear
        if (state->fallingBoard[rowsToClearArr[0]][0] == 8) {
            for (int i = 0; rowsToClearArr[i] != -1 && i < 4; i++) {
                toggleRowColor(state->fallingBoard,rowsToClearArr[i],0);
            }
        } else {
            for (int i = 0; rowsToClearArr[i] != -1 && i < 4; i++) {
               toggleRowColor(state->fallingBoard,rowsToClearArr[i],8);
            }
        }   
    }
    // Animation is done after 50 frames, return to the main game loop.
    if (animationFrameCount > 50) {
        clearLineRows(state->landedBoard,rowsToClearArr);
        animationFrameCount = 0;
        state->state = MAIN_GAME_LOOP;
    }
}


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


