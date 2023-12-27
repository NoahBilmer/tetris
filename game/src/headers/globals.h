#ifndef GLOBALS_H
#define GLOBALS_H
#define DEBUG 1
#define COLUMNS 10
#define ROWS 23
#define BOARD_START 3

#define SCREEN_W 640
#define SCREEN_H 740

#define FALSE 0
#define TRUE 1

#define SHAPE_SIZE 4
#define ROTATION_COUNT 4

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
        {0,2,2,0},
        {0,2,2,0},
        {0,0,0,0}
    },
    {
        {0,0,0,0},
        {0,2,2,0},
        {0,2,2,0},
        {0,0,0,0}
    },
    {
        {0,0,0,0},
        {0,2,2,0},
        {0,2,2,0},
        {0,0,0,0}
    },
    {
        {0,0,0,0},
        {0,2,2,0},
        {0,2,2,0},
        {0,0,0,0}
    } 
};  

static int l_block[ROTATION_COUNT][SHAPE_SIZE][SHAPE_SIZE] = {
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

#endif