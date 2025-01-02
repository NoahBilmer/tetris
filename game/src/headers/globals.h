#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdint.h>
#include "raylib.h"

#define COLUMNS 10
#define ROWS 23
#define BOARD_START 3

#define SCREEN_W 820
#define SCREEN_H 760
#define START_X 4
#define START_Y 0

#define FALSE 0
#define TRUE 1

#define SHAPE_SIZE 4
#define ROTATION_COUNT 4

#define COLLISION_OUT_OF_BOUNDS 1
#define COLLISION_BLOCK_ALREADY_EXISTS 2

#define MAX(a, b) ((a)>(b)? (a) : (b))
#define MIN(a, b) ((a)<(b)? (a) : (b))

typedef enum GameStateEnum {
    MAIN_GAME_LOOP,
    ANIMATION_MUZZLE_FLASH,
    ANIMATION_CLEAR_BLOCKS,
    GAME_OVER,
    TITLE_SCREEN,
} GameStateEnum;

typedef struct State {
    Vector2 moveVec;
    int x;
    int y;
    bool wishRotate;
    bool newPiece;
    uint8_t rotation;
    uint8_t newRotation;
    uint8_t speed;
    uint8_t fastFallSpeed;
    GameStateEnum state;
    Color titleScreenBackground;

    // Board 
    uint8_t (*fallingBoard)[ROWS];
    uint8_t (*landedBoard)[ROWS];
    uint8_t (*nextPiece)[SHAPE_SIZE][SHAPE_SIZE];
    uint8_t (*currentPiece)[SHAPE_SIZE][SHAPE_SIZE];

} State;

#define SPEED_LV_0 48
#define LV_SPEED_ARR_LENGTH 30
#define MAX_LEVEL 29

static uint8_t levelSpeedArr[LV_SPEED_ARR_LENGTH] = {
    48,     // lv 0
    43,     // lv 1
    38,     // lv 2 
    33,     // lv 3
    28,     // lv 4
    23,     // lv 5
    18,     // lv 6
    13,     // lv 7
    8,      // lv 8
    6,      // lv 9
    5,5,5,  // lv 10-12
    4,4,4,  // lv 13-15
    3,3,3,  // lv 16-18
    2,2,2,2,2,2,2,2,2,2, // lv 19-28
    1       // lv 29
    
};

extern int8_t level;
extern uint32_t score;

// Tetris Shapes 
static uint8_t i_block[ROTATION_COUNT][SHAPE_SIZE][SHAPE_SIZE] = {
    {
        {0,0,0,0},
        {0,0,0,0},
        {1,1,1,1},
        {0,0,0,0}
    },
    {
        {0,0,1,0},
        {0,0,1,0},
        {0,0,1,0},
        {0,0,1,0}
    },
    {
        {0,0,0,0},
        {0,0,0,0},
        {1,1,1,1},
        {0,0,0,0}
    },
    {
        {0,0,1,0},
        {0,0,1,0},
        {0,0,1,0},
        {0,0,1,0}
    }
};

static uint8_t o_block[ROTATION_COUNT][SHAPE_SIZE][SHAPE_SIZE] = {
    {
        {0,0,0,0},
        {0,0,0,0},
        {2,2,0,0},
        {2,2,0,0}
    },
    {
        {0,0,0,0},
        {0,0,0,0},
        {2,2,0,0},
        {2,2,0,0}
    },
    {
        {0,0,0,0},
        {0,0,0,0},
        {2,2,0,0},
        {2,2,0,0}
    },
    {
        {0,0,0,0},
        {0,0,0,0},
        {2,2,0,0},
        {2,2,0,0}
    } 
};  

static uint8_t j_block[ROTATION_COUNT][SHAPE_SIZE][SHAPE_SIZE] = {
    {
        {0,0,0,0},
        {0,0,0,0},
        {3,3,3,0},
        {0,0,3,0}
    },
    {
        {0,0,0,0},
        {0,3,0,0},
        {0,3,0,0},
        {3,3,0,0}
    },
    {
        {0,0,0,0},
        {3,0,0,0},
        {3,3,3,0},
        {0,0,0,0}
    },
    {
        {0,0,0,0},
        {0,3,3,0},
        {0,3,0,0},
        {0,3,0,0}
    } 
};

static uint8_t l_block[ROTATION_COUNT][SHAPE_SIZE][SHAPE_SIZE] = {
    {
        {0,0,0,0},
        {0,0,0,0},
        {4,4,4,0},
        {4,0,0,0}
    },
    {
        {0,0,0,0},
        {4,4,0,0},
        {0,4,0,0},
        {0,4,0,0}
    },
    {
        {0,0,0,0},
        {0,0,4,0},
        {4,4,4,0},
        {0,0,0,0}
    },
    {
        {0,0,0,0},
        {0,4,0,0},
        {0,4,0,0},
        {0,4,4,0}
    } 
};

static uint8_t s_block[ROTATION_COUNT][SHAPE_SIZE][SHAPE_SIZE] = {
    {
        {0,0,0,0},
        {0,0,0,0},
        {0,5,5,0},
        {5,5,0,0}
    },
    {
        {0,0,0,0},
        {0,5,0,0},
        {0,5,5,0},
        {0,0,5,0}
    },
    {
        {0,0,0,0},
        {0,0,0,0},
        {0,5,5,0},
        {5,5,0,0}
    },
    {
        {0,0,0,0},
        {0,5,0,0},
        {0,5,5,0},
        {0,0,5,0}
    } 
};

static uint8_t z_block[ROTATION_COUNT][SHAPE_SIZE][SHAPE_SIZE] = {
    {
        {0,0,0,0},
        {0,0,0,0},
        {6,6,0,0},
        {0,6,6,0}
    },
    {
        {0,0,0,0},
        {0,0,6,0},
        {0,6,6,0},
        {0,6,0,0}
    },
    {
        {0,0,0,0},
        {0,0,0,0},
        {6,6,0,0},
        {0,6,6,0}
    },
    {
        {0,0,0,0},
        {0,0,6,0},
        {0,6,6,0},
        {0,6,0,0}
    } 
};

static uint8_t t_block[ROTATION_COUNT][SHAPE_SIZE][SHAPE_SIZE] = {
    {
        {0,0,0,0},
        {0,0,0,0},
        {7,7,7,0},
        {0,7,0,0}
    },
    {
        {0,0,0,0},
        {0,7,0,0},
        {7,7,0,0},
        {0,7,0,0}
    },
    {
        {0,0,0,0},
        {0,7,0,0},
        {7,7,7,0},
        {0,0,0,0}
    },
    {
        {0,0,0,0},
        {0,7,0,0},
        {0,7,7,0},
        {0,7,0,0}
    } 
};


#endif