/*******************************************************************************************
* Tetris in C by Noah Bilmer
********************************************************************************************/
#include <stdlib.h>
#include "raylib.h"
#include "headers/board.h"
#include "headers/globals.h"
#include "raymath.h"        // Required for: Vector2Clamp()
#include <stdio.h>
#include <string.h>
#include <time.h>

#define MAX(a, b) ((a)>(b)? (a) : (b))
#define MIN(a, b) ((a)<(b)? (a) : (b))

Texture2D gameboardUI;
Texture2D blocksSpriteSheet;
Texture2D digitsSpriteSheet;

void updateGameState();
void getInput(int* wishX, int* wishY, int*wishRotate, int* wishFastFall, int *moveFrameCount,int* wishPause);
void processInput();
void drawBlock(int x, int y, int color, float scale);
void drawBoard(int (*board)[ROWS],float scale);
void getNewPiece(int (*piece)[SHAPE_SIZE][SHAPE_SIZE]);
void drawNextPiece(int (*nextPiece)[SHAPE_SIZE][SHAPE_SIZE],int scale);
void drawGameState();

// Globals
int (*fallingBoard)[ROWS];
int (*landedBoard)[ROWS];
int (*currentPiece)[SHAPE_SIZE][SHAPE_SIZE];
int (*nextPiece)[SHAPE_SIZE][SHAPE_SIZE];
int x = START_X;
int y = START_Y;

// input related globals 
int wishX, wishY;
int wishPause,wishRotate,wishFastFall;

int paused = 0;

int moveCooldown = 0;
/* speed determines the number of frames it takes to move
 * the piece down one y coordinate. */
int baseSpeed = SPEED_LV_0;
int fastFallSpeed = 1;
float speed = SPEED_LV_0;
int frameCount = 0;
int moveFrameCount = 0;
int newPiece = TRUE;
int rotation = 0;
int newRotation = 0;
int animationFrameCount = 0;
int gameState = STATE_MAIN_GAME_LOOP;
int rowsToClearArr[4] = {-1,-1,-1,-1};
int lines = 0;



RenderTexture2D target;

int main(void)
{

    const int windowWidth = 800;
    const int windowHeight = 800;
    landedBoard = calloc(4,sizeof(int[ROWS][COLUMNS]));
    fallingBoard = calloc(4,sizeof(int[ROWS][COLUMNS]));

    
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
    SetWindowMinSize(320, 240);
    
    // Render texture initialization, used to hold the rendering result so we can easily resize it
    target = LoadRenderTexture(SCREEN_W, SCREEN_H);
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
    
    // Main game loop
    while (!WindowShouldClose())
    {
        // Get all the input for this current frame.
        getInput(&wishX,&wishY,&wishRotate,&wishFastFall,&moveFrameCount,&wishPause);    
        
        // iterate the framecount
        frameCount++;
        
        switch (gameState) {
            case STATE_MAIN_GAME_LOOP:
                processInput();
                updateGameState();
                break;
            case STATE_ANIMATION_MUZZLE_FLASH:
                animationFrameCount++;
                clearBoard(fallingBoard);
                writeBlocks(x,y,currentPiece[rotation],fallingBoard,TRUE);
                if (animationFrameCount > 1) {
                        gameState = STATE_MAIN_GAME_LOOP;
                        animationFrameCount = 0;
                }
                break;
            case STATE_ANIMATION_CLEAR_BLOCKS:
                clearLineRows(landedBoard,rowsToClearArr);
                gameState = STATE_MAIN_GAME_LOOP;
                for (int i = 0; i < 4; i++) {
                    rowsToClearArr[i] = -1;
                }
                break;
            case STATE_PAUSED:
                processInput();
                break;
        }
       
        drawGameState();
        if (checkLineClears(landedBoard,rowsToClearArr) == TRUE) {
            gameState = STATE_ANIMATION_CLEAR_BLOCKS;
        }
        
        
    }

    // De-Initialization
    UnloadRenderTexture(target);        // Unload render texture
    CloseWindow();                      // Close window and OpenGL context
    free(fallingBoard);
    free(landedBoard);
    return 0;
}

/**
* FUNCTION: getInput()
* DESCRIPTION: Stores the user's current desired actions on the current frame.
* NOTE: moveFrameCount is used to ensure that holding down the left and right keys
*       move the piece at a manageable speed. 
*/ 
void getInput(int* wishX, int* wishY, int*wishRotate, int* wishFastFall, int *moveFrameCount,int* wishPause) {
        // reset input-related variables
        *wishY = 0;
        *wishX = 0;
        *wishRotate = FALSE;
        *wishFastFall = FALSE;
        *wishPause = FALSE;
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
        }
        if (IsKeyDown(KEY_DOWN)) {
            *wishFastFall = TRUE;
        }
        if (IsKeyReleased(KEY_DOWN)) {
           *wishFastFall = FALSE;
        }
        if (IsKeyPressed(KEY_TAB)) {
            *wishPause = TRUE;
        }
}

/**
* FUNCTION: processInput() 
* DESCRIPTION: 
* // todo: is this sub-function a good idea?
*/
void processInput() {
    // Process input
        if (wishFastFall == TRUE) {
            speed = fastFallSpeed;
        }
        else {
            speed = baseSpeed;
        }
        if (wishRotate == TRUE) {
            newRotation++;
            if (newRotation > 3) 
                newRotation = 0;
        }
        if (wishPause) {
            paused = !paused;
            if (paused == TRUE) {
                gameState = STATE_PAUSED;
            }
            else {
                gameState = STATE_MAIN_GAME_LOOP;
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
            writeBlocks(x,y,currentPiece[rotation],landedBoard,FALSE);
            x = START_X;
            y = START_Y;
            wishRotate = FALSE;
            newPiece = FALSE;
            newRotation = 0;
            rotation = newRotation;
            memcpy(currentPiece,nextPiece,sizeof(int[ROTATION_COUNT][SHAPE_SIZE][SHAPE_SIZE]));
            getNewPiece(nextPiece);
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
            gameState = STATE_ANIMATION_MUZZLE_FLASH;
            newPiece = TRUE; 
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
void getNewPiece(int (*piece)[SHAPE_SIZE][SHAPE_SIZE]) {
    int pieceIndex = (rand() % 7) + 1;
    int (*newPiece)[SHAPE_SIZE][SHAPE_SIZE];
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
    memcpy(piece,newPiece,sizeof(int[ROTATION_COUNT][SHAPE_SIZE][SHAPE_SIZE]));
}

/**
* FUNCTION: drawGameState() 
* DESCRIPTION: Draws the state of the game on this current frame. 
*/ 
void drawGameState() {
    // Compute required framebuffer scaling
    float scale = MIN((float)GetScreenWidth()/SCREEN_W, (float)GetScreenHeight()/SCREEN_H);
    BeginDrawing();
            ClearBackground(BLACK);     // Clear screen background
            // Draw the UI texture to screen, properly scaled
            DrawTexturePro(gameboardUI, (Rectangle){ 0.0f, 0.0f, (float)target.texture.width, (float)target.texture.height }, 
                           (Rectangle){ (GetScreenWidth() - ((float)SCREEN_W*scale))*0.5f, (GetScreenHeight() - ((float)SCREEN_H*scale))*0.5f,
                           (float)SCREEN_W*scale, (float)SCREEN_H*scale }, 
                           (Vector2){ 0.0f, 0.0f }, 
                           0.0f, 
                           WHITE);
        
        drawBoard(landedBoard,scale);
        drawBoard(fallingBoard,scale);
        drawNextPiece(nextPiece,scale);
    EndDrawing();
}

/**
* FUNCTION: drawBoard() 
* DESCRIPTION: Draws the provided board to the screen.
*/ 
void drawBoard(int (*board)[ROWS],float scale) {
    for (int row = 3; row < ROWS; row++) {
        for (int col = 0; col < COLUMNS; col++) {
            if (board[row][col] != 0)
                drawBlock(col,row - 3,board[row][col],scale);
        }
    }
}

/**
* FUNCTION: drawBlock() 
* DESCRIPTION: Draws the provided block to the screen.
*/ 
void drawBlock(int x, int y, int color,float scale) {
    float blockSize = 0.05;
    color--; // we start at 0 here but the blocks in the array start at 1, so subtract 1.
    DrawTexturePro(blocksSpriteSheet, (Rectangle){ 0.0f, color*32.0f, 32,32 }, 
                           (Rectangle){ (GetScreenWidth() - ((float)SCREEN_W*scale))*0.5f, (GetScreenHeight() - ((float)SCREEN_H*scale))*0.5f,
                           (float)SCREEN_W*scale*blockSize, (float)SCREEN_H*scale*0.043 }, 
                           (Vector2){ -1*(60.0f + 32*x)*scale, -1*(14.0f + 32*(y))*scale }, 
                           0.0f, 
                           WHITE);
}

/**
* FUNCTION: drawNextPiece()
* DESCRIPTION: Draws the next piece to the screen. 
*/ 
void drawNextPiece(int (*nextPiece)[SHAPE_SIZE][SHAPE_SIZE],int scale) { 
    for (int i = 0; i < SHAPE_SIZE; i++) {
            for (int j = 0; j < SHAPE_SIZE; j++) {
                if (nextPiece[0][i][j] > 0)
                    drawBlock(11 + i,7 + j,nextPiece[0][i][j],scale);
            }
        }
}


