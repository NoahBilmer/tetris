/*******************************************************************************************
* Tetris in C by Noah Bilmer
********************************************************************************************/
#include <stdlib.h>
#include "raylib.h"
#include "headers/board.h"
#include "headers/draw.h"
#include "headers/globals.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

void updateGameState();
void getInput(int* wishX, int* wishY, int* wishRotate, int *moveFrameCount);
void getNewPiece(uint8_t (*piece)[SHAPE_SIZE][SHAPE_SIZE]);
int doScoreCalculations();
void addScore(int *scoreToAdd,int newScore);

void renderGraphics(enum State screen);
void playClearAnimation(int *color);

// Globals
uint8_t (*landedBoardCpy)[ROWS];
uint8_t (*currentPiece)[SHAPE_SIZE][SHAPE_SIZE];
int8_t x = START_X;
int8_t y = START_Y;
uint16_t score = 0;
uint8_t level = 7;

uint8_t (*fallingBoard)[ROWS];
uint8_t (*landedBoard)[ROWS];
uint8_t (*nextPiece)[SHAPE_SIZE][SHAPE_SIZE];

Texture2D gameboardUI;
Texture2D blocksSpriteSheet;
Texture2D digitsSpriteSheet;
Texture2D backgroundColor;
Texture2D titleText;
Texture2D githubLink;
Texture2D levelSelectText;

// input related globals 
int wishX, wishY, wishRotate;

bool paused = 0;

int moveCooldown = 0;
/* speed determines the number of frames it takes to move
 * the piece down one y coordinate. */
uint8_t baseSpeed = 0;
uint8_t fastFallSpeed = 1;
float speed = SPEED_LV_0;
int frameCount = 0;
int moveFrameCount = 0;
bool newPiece = TRUE;
uint8_t rotation = 0;
uint8_t newRotation = 0;
int animationFrameCount = 0;
State gameState = MAIN_GAME_LOOP;
uint8_t gameStateCpy; // used for pausing mechanism
int rowsToClearArr[4] = {-1,-1,-1,-1};
// used to copy each row for the line clearing animation
int rowCpyArr[4][COLUMNS];
uint8_t lines = 0;
uint8_t rowColor = 0;
bool swap = FALSE;
// used for clearing animation
int clearColor = 0;
uint8_t scoreToAdd = 0;

float scale;

RenderTexture2D target;
RenderTexture2D digitsTarget; 

int main(void)
{
    const int windowWidth = 820;
    const int windowHeight = 760;
    landedBoard = calloc(4,sizeof(int[ROWS][COLUMNS]));
    fallingBoard = calloc(4,sizeof(int[ROWS][COLUMNS]));
    landedBoardCpy = calloc(4,sizeof(int[ROWS][COLUMNS]));
    
    currentPiece = calloc(4,sizeof(int[ROTATION_COUNT][SHAPE_SIZE][SHAPE_SIZE]));
    nextPiece = calloc(4,sizeof(int[ROTATION_COUNT][SHAPE_SIZE][SHAPE_SIZE]));
    srand(time(NULL));
    // Get our first piece.
    getNewPiece(nextPiece);

    // Enable config flags for resizable window and vertical synchro
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(windowWidth, windowHeight, "Tetris");
    // Move from bin/Debug to tetris
    gameboardUI = LoadTexture("resources/tetris-ui.png");
    blocksSpriteSheet = LoadTexture("resources/blocks.png");
    digitsSpriteSheet = LoadTexture("resources/digits.png");
    backgroundColor = LoadTexture("resources/background-color.png");
    titleText = LoadTexture("resources/title.png");
    githubLink = LoadTexture("resources/github-link.png");
    levelSelectText = LoadTexture("resources/select-level.png");
    SetWindowMinSize(320, 240);
    
    // Render texture initialization, used to hold the rendering result so we can easily resize it
    target = LoadRenderTexture(SCREEN_W, SCREEN_H); 
    digitsTarget = LoadRenderTexture(digitsSpriteSheet.width, blocksSpriteSheet.height);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);  // Texture scale filter to use

    SetTargetFPS(60);  // Set our game to run at 60 frames-per-second
    #if DEBUG == 1
    for (int row = 0; row < ROWS; row++) {
        printf("\n");
        for (int col = 0; col < COLUMNS; col++) {
            printf("%d, ", fallingBoard[row][col]);
        }
    }  
    #endif

    /* Our "new score" when a line is cleared. We gradually tally up the score 
    using addScore() rather than instantly adding it. */
    int newScore = 0;
    
    baseSpeed = levelSpeedArr[level];
    uint8_t row, col = 0;
    uint8_t  tempColorVar = 1;
    // Main game loop
    while (!WindowShouldClose())
    {
        // Get all the input for this current frame.
        getInput(&wishX,&wishY,&wishRotate,&moveFrameCount);    
        
        // iterate the framecount
        frameCount++;
        
        switch (gameState) {
            case PAUSED:
                break;
            case MAIN_GAME_LOOP:
                updateGameState();
                break;
            case ANIMATION_MUZZLE_FLASH:
                animationFrameCount++;
                clearBoard(fallingBoard);
                writeBlocks(x,y,currentPiece[rotation],fallingBoard,TRUE);
                if (animationFrameCount > 1) {
                    gameState = MAIN_GAME_LOOP;
                    animationFrameCount = 0;
                    scoreToAdd = 2;
                    newScore = scoreToAdd + score;
                }
                break;
            case ANIMATION_CLEAR_BLOCKS:
                animationFrameCount++;
                playClearAnimation(&clearColor);
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
                        baseSpeed = levelSpeedArr[level];
                    }
                }
                break;
            
            case GAME_OVER:
                // Do the game over animation.
                animationFrameCount++;
                if (animationFrameCount > 140) {
                    clearBoard(fallingBoard);
                    clearBoard(landedBoard);
                    row = 0;
                    col = 0;
                    score = 0;
                    newScore = 0;
                    animationFrameCount = 0;
                    gameState = MAIN_GAME_LOOP; 
                } else {
                    if (row < ROWS) {
                        if (tempColorVar > 7) {
                            tempColorVar = 1;
                        }
                        if (fallingBoard[row + 3][col] == 0) {
                            fallingBoard[row + 3][col] = tempColorVar;
                        }
                        if (fallingBoard[(ROWS - 1) - row][(COLUMNS - 1) - col] == 0) {
                            fallingBoard[(ROWS - 1) - row][(COLUMNS - 1) - col]  = tempColorVar;   
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
                break;
        }
        // Add the score gradually every frame so long as there is still score to add.
        if (newScore > 0) {
            addScore(&newScore,scoreToAdd);    
        }
        
        if (checkLineClears(landedBoard,rowsToClearArr) == TRUE && gameState != ANIMATION_CLEAR_BLOCKS) {
            gameState = ANIMATION_CLEAR_BLOCKS;
        }
       renderGraphics(gameState);      
    }

    // De-Initialization
    UnloadRenderTexture(target);        // Unload render texture
    CloseWindow();                      // Close window and OpenGL context
    free(fallingBoard);
    free(landedBoard);
    return 0;
}


/**
* FUNCTION: renderGraphics() 
* DESCRIPTION: render the graphics to the screen.
*/ 
void renderGraphics(enum State screen) {
    // Compute required framebuffer scaling  
    scale = MIN((float)GetScreenWidth()/SCREEN_W,(float)GetScreenHeight()/SCREEN_H);
    switch (screen) { 
        default:
        // Begin texture mode to draw to the target (essentially acts as a canvas)
        BeginTextureMode(target);
            DrawTexture(gameboardUI,0,0,WHITE); 
            drawGameState();
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
void getInput(int* wishX, int* wishY, int*wishRotate, int *moveFrameCount) {
        // reset input-related variables
        *wishY = 0;
        *wishX = 0;
        *wishRotate = FALSE;
        if (IsKeyPressed(KEY_LEFT)) {
            *wishX -= 1;
            *moveFrameCount = 0;
        }
        else if (IsKeyDown(KEY_LEFT)) {
           (*moveFrameCount)++;
            if (*moveFrameCount > 8) {
                *moveFrameCount = 0;
                *wishX -= 1;
            } else {
                *wishX = 0;
            }
        }
        if (IsKeyPressed(KEY_RIGHT))
        {
            *wishX += 1;
        }
        else if (IsKeyDown(KEY_RIGHT)) {
            (*moveFrameCount)++;
            if (*moveFrameCount > 8) {
                *moveFrameCount = 0;  
                *wishX += 1;
            } else {
                *wishX = 0;
            }
        }

        if (IsKeyPressed(KEY_SPACE)) {
            *wishRotate = TRUE;
            newRotation++;
            if (newRotation > 3) 
                newRotation = 0;
        }
        // If we wish to fastfall only do so when we are not about to collide in the Y direction
        // This is to make last-second adjustments feel consistent 
        if (IsKeyDown(KEY_DOWN) && !(colliding(x,y + 1,currentPiece[rotation],landedBoard) > 0)) {
           speed = fastFallSpeed;
        }
        else {
            speed = baseSpeed;
        }
        if (IsKeyReleased(KEY_DOWN)) {
            speed = baseSpeed;
        }
        if (IsKeyPressed(KEY_TAB)) {
            paused = !paused;
            if (paused == TRUE) {
                gameStateCpy = gameState;
                gameState = PAUSED;
            }
            else {
                gameState = gameStateCpy;
            }
        }
}

/**
* FUNCTION: updateGameState() 
* DESCRIPTION: Updates the board's state for the current frame based on player input.
*/ 
void updateGameState() {
        // move the piece down if enough frames have elapsed.
        if (frameCount > speed) {
            frameCount = 0;
            wishY = 1;
        }

        // Create a new Piece
        if (newPiece) {
            clearBoard(fallingBoard);
            x = START_X;
            y = START_Y;
            wishRotate = FALSE;
            newPiece = FALSE;
            newRotation = 0;
            rotation = newRotation;
            memcpy(currentPiece,nextPiece,sizeof(int[ROTATION_COUNT][SHAPE_SIZE][SHAPE_SIZE]));
            getNewPiece(nextPiece);
            // check if we're colliding at the start x/y; this is the game over condition 
            if (colliding(x,y,currentPiece[newRotation],landedBoard)) {
                gameState = GAME_OVER;
                return;
            }
        }
                
        /*** Do all piece translations and or transformations ***/
        // Rotate the piece if we are not colliding 
        if (wishRotate && colliding(x,y,currentPiece[newRotation],landedBoard) == 0) {
                rotation = newRotation;
        }
        // Move in the x direction if we are not colliding.
        if (colliding(x + wishX,y,currentPiece[rotation],landedBoard) == 0) {
               x = x + wishX;             
        }
        /* Move in the y direction if we are not colliding. */
        if (colliding(x,y + wishY,currentPiece[rotation],landedBoard) > 0) {
            // Place this piece
            // create a new piece next iteration
            gameState = ANIMATION_MUZZLE_FLASH;
            newPiece = TRUE; 
            writeBlocks(x,y,currentPiece[rotation],landedBoard,FALSE);
        }
        /* NOT colliding in the y dimension; move the piece down one.*/
        else {
            y = y + wishY; 
        }
        // Write to the fallingBoard
        if (wishRotate == TRUE || wishX != 0 || wishY != 0) {
            clearBoard(fallingBoard);
            writeBlocks(x,y,currentPiece[rotation],fallingBoard,FALSE);
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
int doScoreCalculations() {
    int lines = 0;
    int scoreToAdd;
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
     printf("Score to add: %d\n",scoreToAdd);
    
    return scoreToAdd;
    
    
}

/**
* FUNCTION: addScore(int* newScore) 
* DESCRIPTION: Gradually adds up our score untill we get to our desired new score value.
*/ 
void addScore(int *newScore,int scoreToAdd) {
    if (score <= *newScore) {
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
            // if the score we want to add is over 10,000, just add the score instantly.
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
void playClearAnimation(int *color) {
    // every 10 frames we should either change all lines to clear white,
    // or change them to 0 which will show the landedBoard. 
    if ((animationFrameCount % 10) == 0) {
        // Change the color of the lines to clear
        if (*color == 8) {
            for (int i = 0; rowsToClearArr[i] != -1 && i < 4; i++) {
                toggleRowColor(fallingBoard,rowsToClearArr[i],*color);
            }
            *color = 0;
        } else {
            for (int i = 0; rowsToClearArr[i] != -1 && i < 4; i++) {
               toggleRowColor(fallingBoard,rowsToClearArr[i],*color);
            }
           *color = 8;
        }   
    }
    // Animation is done after 50 frames, return to the main game loop.
    if (animationFrameCount > 50) {
        clearLineRows(landedBoard,rowsToClearArr);
        animationFrameCount = 0;
        gameState = MAIN_GAME_LOOP;
    }
}


