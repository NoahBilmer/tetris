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

    #if DEBUG == 1
    for (int row = 0; row < ROWS; row++) {
        printf("\n");
        for (int col = 0; col < COLUMNS; col++) {
            printf("%d, ", fallingBoard[row][col]);
        }
    }  
    #endif
   
    writeBlocks(-1,-1,o_block[0],fallingBoard);
    
   
    printf("\n");
    // Main game loop
    while (!WindowShouldClose())
    {
        // Compute required framebuffer scaling
        float scale = MIN((float)GetScreenWidth()/SCREEN_W, (float)GetScreenHeight()/SCREEN_H);
        
        if (IsKeyPressed(KEY_SPACE))
        {
        
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
        EndDrawing();
    }

    // De-Initialization
    UnloadRenderTexture(target);        // Unload render texture
    CloseWindow();                      // Close window and OpenGL context
    free(fallingBoard);
    return 0;
}


/**
* FUNCTION: drawBoard() 
* DESCRIPTION: Draws the provided board to the screen.
*/ 
void drawBoard(int (*board)[ROWS],float scale) {
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLUMNS; col++) {
            if (board[row][col] != 0)
                drawBlock(col,row,board[row][col],scale);
        }
    }
}

/**
* FUNCTION: drawBlock() 
* DESCRIPTION: Draws the provided block to the screen.
*/ 
void drawBlock(int x, int y, int color,float scale) {
    float blockSize = 0.05;
    DrawTexturePro(blocksSpriteSheet, (Rectangle){ 0.0f, color*32.0f, 32,32 }, 
                           (Rectangle){ (GetScreenWidth() - ((float)SCREEN_W*scale))*0.5f, (GetScreenHeight() - ((float)SCREEN_H*scale))*0.5f,
                           (float)SCREEN_W*scale*blockSize, (float)SCREEN_H*scale*0.043 }, 
                           (Vector2){ -1*(60.0f + 32*x)*scale, -1*(14.0f + 32*(y))*scale }, 
                           0.0f, 
                           WHITE);
}



