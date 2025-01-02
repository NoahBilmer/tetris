/*******************************************************************************************
* Tetris in C by Noah Bilmer
********************************************************************************************/
#include <stdlib.h>
#include "raylib.h"
#include "raymath.h"
#include "headers/board.h"
#include "headers/draw.h"
#include "headers/globals.h"
#include "headers/state.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

// Function prototypes 
void renderGraphics(State* screen);
void getInput(State* state,int* moveFrameCount);
void initGameState(State* state);

// Textures 
Texture2D gameboardUI;
Texture2D blocksSpriteSheet;
Texture2D digitsSpriteSheet;
Texture2D titleText;
Texture2D githubLink;
Texture2D levelSelectInfoText;
Texture2D hitEnterText;
Texture2D levelText;
RenderTexture2D target;

int main(void)
{ 
    int exitFlag = 0;
    int moveFrameCount = 0;  

    // Initialize the game state.
    State* gameState = malloc(sizeof(State));
    initGameState(gameState);

    srand(time(NULL));
    // Get our first piece.
    getNewPiece(gameState->nextPiece);

    // Enable config flags for resizable window and vertical synchro
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(SCREEN_W, SCREEN_H, "Tetris");
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
    SetWindowSize(SCREEN_W,SCREEN_H);
    SetExitKey(KEY_NULL); // we define our own exit behavior in TitleScreen
    
    // Render texture initialization, used to hold the rendering result so we can easily resize it
    target = LoadRenderTexture(SCREEN_W, SCREEN_H); 
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);  // Texture scale filter to use

    SetTargetFPS(60);  // Set our game to run at 60 frames-per-second
    
    // Main game loop
    while (!WindowShouldClose() && !exitFlag)
    {
        gameState->speed = levelSpeedArr[gameState->level];
        // Get all the input for this current frame.
        getInput(gameState,&moveFrameCount);    
        
        // iterate the framecount
        gameState->frameCount++;
        
        switch (gameState->state) {
            case MAIN_GAME_LOOP:
                gameState->state = MainGameLoop(gameState);
                break;
            case ANIMATION_MUZZLE_FLASH:
                gameState->state = MuzzleFlash(gameState);
                break;
            case ANIMATION_CLEAR_BLOCKS:
                gameState->state = ClearBlocks(gameState);
                break;
            case GAME_OVER:
                gameState->state = GameOver(gameState);
                break;
            case TITLE_SCREEN:
                gameState->state = TitleScreen(gameState);
                break;
            case EXIT:
                exitFlag = TRUE;
                break;
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


/**
* FUNCTION: initGameState() 
* DESCRIPTION: Sets up all the initial values for everything in the State struct.
*/ 
void initGameState(State* state) {
    state->moveVec.x = 0;
    state->moveVec.y = 0;
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
    state->level = 7;
    for (int i = 0; i < 4; i++) 
        state->rowsToClearArr[i] = -1;
        
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
            drawTitleScreen(state);
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
            state->speed = levelSpeedArr[state->level];
        }
        if (IsKeyReleased(KEY_DOWN)) {
            state->speed = levelSpeedArr[state->level];
        }
        if (IsKeyReleased(KEY_ESCAPE)) {
            // We want to be able to exit regrardless of the state we are in.
            state->state = TITLE_SCREEN;
        }
}

