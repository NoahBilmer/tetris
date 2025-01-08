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

//#define PLATFORM_WEB 1

#if defined(PLATFORM_WEB)
    #include "emscripten/emscripten.h"
#endif


// Function prototypes 
void renderGraphics(void);
void getInput(void);
void update(void);

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

State* state;
int exitFlag = 0;

int main(void)
{ 
    srand(time(NULL));

    state = malloc(sizeof(State));
    state->startLevel = 7;
    initState(state);
    // Get our first piece.

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

    #if defined(PLATFORM_WEB)
        emscripten_set_main_loop(update, 0, 1);
    #else
        // Main game loop
        while (!WindowShouldClose() && !exitFlag)
        {
            update();
        }
    #endif
    if (exitFlag == 1 || WindowShouldClose()) {
        // De-Initialization
        UnloadRenderTexture(target);        // Unload render texture
        CloseWindow();                      // Close window and OpenGL context
    }

    free(state->currentPiece);
    free(state->nextPiece);
    free(state->landedBoard);
    free(state->fallingBoard);
    free(state);
    
    return 0;
}

/**
* FUNCTION: update() 
* DESCRIPTION: Executes one game loop iteration.
* We start by getting input, then calculating the game state and finally rendering the frame.
*/ 
void update(void) {
    // Get all the input for this current frame.
    getInput();    
    // iterate the framecount
    state->frameCount++;
    switch (state->state) {
        case MAIN_GAME_LOOP:
            state->state = MainGameLoop(state);
            break;
        case ANIMATION_MUZZLE_FLASH:
            state->state = MuzzleFlash(state);
            break;
        case ANIMATION_CLEAR_BLOCKS:
            state->state = ClearBlocks(state);
            break;
        case GAME_OVER:
            state->state = GameOver(state);
            break;
        case TITLE_SCREEN:
            state->state = TitleScreen(state);
            // Check the state value immediately, so we don't draw the screen as we're exiting.
            if (state->state == EXIT) {
                exitFlag = 1;
                return;
            } 
            break;
        case EXIT:
            break;
    }
    
    renderGraphics();  
}


/**
* FUNCTION: renderGraphics() 
* DESCRIPTION: render the graphics to the screen.
*/ 
void renderGraphics(void) {
    // Compute required framebuffer scaling   
    float scale = MIN((float)GetScreenWidth()/SCREEN_W,(float)GetScreenHeight()/SCREEN_H);
    switch (state->state) { 
        case TITLE_SCREEN:
            BeginTextureMode(target);
                ClearBackground(state->titleScreenBackground);
                drawTitleScreen(state->startLevel);
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
void getInput(void) {
        // reset input-related variables
        state->moveVec.x = 0;
        state->moveVec.y = 0;
        state->wishRotate = FALSE;
        if (IsKeyPressed(KEY_LEFT)) {
            state->moveVec.x -= 1;
            state->moveFrameCount = 0;
        }
        else if (IsKeyDown(KEY_LEFT)) {
           state->moveFrameCount++;
            if (state->moveFrameCount > 8) {
                state->moveFrameCount = 0;
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
            state->moveFrameCount++;
            if (state->moveFrameCount > 8) {
                state->moveFrameCount = 0;  
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

