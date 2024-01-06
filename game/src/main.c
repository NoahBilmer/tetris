/*******************************************************************************************
* Tetris in C by Noah Bilmer
********************************************************************************************/
#include <stdlib.h>
#include "raylib.h"
#include "headers/board.h"
#include "headers/globals.h"
#include "raymath.h"        // Required for: Vector2Clamp()
#include <stdio.h>

#define MAX(a, b) ((a)>(b)? (a) : (b))
#define MIN(a, b) ((a)<(b)? (a) : (b))

Texture2D gameboardUI;
Texture2D blocksSpriteSheet;
Texture2D digitsSpriteSheet;

void drawBlock(int x, int y, int color, float scale);
void drawBoard(int (*board)[ROWS],float scale);

int main(void)
{

    const int windowWidth = 800;
    const int windowHeight = 800;
    int (*fallingBoard)[ROWS] = calloc(4,sizeof(int[ROWS][COLUMNS]));
    int (*landedBoard)[ROWS] = calloc(4,sizeof(int[ROWS][COLUMNS]));

    int x, y;
    int (*currentBlock)[SHAPE_SIZE][SHAPE_SIZE] =  calloc(4,sizeof(int[ROTATION_COUNT][SHAPE_SIZE][SHAPE_SIZE]));

    // Enable config flags for resizable window and vertical synchro
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(windowWidth, windowHeight, "Tetris");
    // Move from bin/Debug to tetris
    gameboardUI = LoadTexture("resources/tetris-ui.png");
    blocksSpriteSheet = LoadTexture("resources/blocks.png");
    digitsSpriteSheet = LoadTexture("resources/digits.png");
    SetWindowMinSize(320, 240);
    
    // Render texture initialization, used to hold the rendering result so we can easily resize it
    RenderTexture2D target = LoadRenderTexture(SCREEN_W, SCREEN_H);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);  // Texture scale filter to use

    SetTargetFPS(60);  // Set our game to run at 60 frames-per-second
    x = START_X;
    y = START_Y;
    currentBlock = i_block;
    #if DEBUG == 1
    for (int row = 0; row < ROWS; row++) {
        printf("\n");
        for (int col = 0; col < COLUMNS; col++) {
            printf("%d, ", fallingBoard[row][col]);
        }
    }  
    #endif
    
   
    printf("\n");
    
    int moveCooldown = 0;
    /* speed determines the number of frames it takes to move
     * the piece down one y coordinate. */
    int baseSpeed = 48;
    int fastFallSpeed = 1;
    float speed = baseSpeed;
    int frameCount = 0;
    int moveFrameCount = 0;
    int newPiece = TRUE;
    int rotation = 0;
    int newRotation = 0;
    int wishFastFall = FALSE;
    int wishRotate = FALSE;
    int wishX = 0;
    int wishY = 0;
    // Main game loop
    while (!WindowShouldClose())
    {
        // Compute required framebuffer scaling
        float scale = MIN((float)GetScreenWidth()/SCREEN_W, (float)GetScreenHeight()/SCREEN_H);
        // reset x/y movement variables
        wishY = 0;
        wishX = 0;
        wishRotate = FALSE;
        wishFastFall = FALSE;
        frameCount++;
        
        
        // move the piece down if enough frames have elapsed.
        if (frameCount > speed) {
            frameCount = 0;
            wishY = 1;
        }

        // Create a new Piece
        if (newPiece) {
            x = START_X;
            y = START_Y;
            wishRotate = FALSE;
            newRotation = 0;
            rotation = newRotation;
            newPiece = FALSE;
        }

        // Get all the input for this current frame.
        getInput(&wishX,&wishY,&wishRotate,&wishFastFall,&moveFrameCount);   

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
                
        /*** Do all piece translations and or transformations ***/
        // Rotate the piece if we are not colliding 
        if (wishRotate && colliding(x,y,currentBlock[newRotation],landedBoard) == 0) {
                rotation = newRotation;
        }
        // Move in the x direction if we are not colliding.
        if (colliding(x + wishX,y,currentBlock[rotation],landedBoard) == 0) {
               x = x + wishX;             
        }
        /* Move in the y direction if we are not colliding.
         * This is the only instance in the game loop where we actually write the 
         * piece to the landedBoard. */
        if (colliding(x,y + wishY,currentBlock[rotation],landedBoard) > 0) {
            clearBoard(fallingBoard);
            // Place this piece
            writeBlocks(x,y,currentBlock[rotation],landedBoard);
            // create a new piece next iteration
            newPiece = TRUE; 
        }
        /* NOT colliding in the y dimension; move the piece down one.*/
        else {
            y = y + wishY;   
        }
        // Write to the fallingBoard
        if (wishRotate == TRUE || wishX != 0 || wishY != 0) {
            clearBoard(fallingBoard);
            writeBlocks(x,y,currentBlock[rotation],fallingBoard);
        }

        BeginDrawing();
            ClearBackground(BLACK);     // Clear screen background
            // Draw the UI texture to screen, properly scaled
            DrawTexturePro(gameboardUI, (Rectangle){ 0.0f, 0.0f, (float)target.texture.width, (float)target.texture.height }, 
                           (Rectangle){ (GetScreenWidth() - ((float)SCREEN_W*scale))*0.5f, (GetScreenHeight() - ((float)SCREEN_H*scale))*0.5f,
                           (float)SCREEN_W*scale, (float)SCREEN_H*scale }, 
                           (Vector2){ 0.0f, 0.0f }, 
                           0.0f, 
                           WHITE);

        drawBoard(fallingBoard,scale);
        drawBoard(landedBoard,scale);
        EndDrawing();
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
*/ 
void getInput(int* wishX, int* wishY, int*wishRotate, int* wishFastFall,int *moveFrameCount) {
        if (IsKeyPressed(KEY_LEFT)) {
            *wishX -= 1;
            *moveFrameCount = 0;
        }
        else if (IsKeyDown(KEY_LEFT)) {
           *moveFrameCount++;
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
            *moveFrameCount = 0;
        }
        else if (IsKeyDown(KEY_RIGHT)) {
            *moveFrameCount++;
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



