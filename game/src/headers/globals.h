#ifndef GLOBALS_H
#define GLOBALS_H
#define DEBUG 1
#define COLUMNS 10
#define ROWS 23
#define BOARD_START 3

#define SCREEN_W 820
#define SCREEN_H 760
#define START_X 4;
#define START_Y 0;

#define FALSE 0
#define TRUE 1

#define SHAPE_SIZE 4
#define ROTATION_COUNT 4

#define COLLISION_OUT_OF_BOUNDS 1
#define COLLISION_BLOCK_ALREADY_EXISTS 2



#define STATE_MAIN_GAME_LOOP 1
#define STATE_ANIMATION_MUZZLE_FLASH 2
#define STATE_ANIMATION_CLEAR_BLOCKS 3
#define STATE_PAUSED 4

#define SPEED_LV_0 48
#define LV_SPEED_ARR_LENGTH 30

static int levelSpeedArr[LV_SPEED_ARR_LENGTH] = {
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

// Tetris Shapes 
static int i_block[ROTATION_COUNT][SHAPE_SIZE][SHAPE_SIZE] = {
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

static int o_block[ROTATION_COUNT][SHAPE_SIZE][SHAPE_SIZE] = {
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

static int j_block[ROTATION_COUNT][SHAPE_SIZE][SHAPE_SIZE] = {
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

static int l_block[ROTATION_COUNT][SHAPE_SIZE][SHAPE_SIZE] = {
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

static int s_block[ROTATION_COUNT][SHAPE_SIZE][SHAPE_SIZE] = {
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

static int z_block[ROTATION_COUNT][SHAPE_SIZE][SHAPE_SIZE] = {
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

static int t_block[ROTATION_COUNT][SHAPE_SIZE][SHAPE_SIZE] = {
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