#ifndef GLOBALS_H
#define GLOBALS_H
#define DEBUG 1
#define COLUMNS 10
#define ROWS 23
#define BOARD_START 3

#define SCREEN_W 640
#define SCREEN_H 740
#define START_X 4;
#define START_Y 0;

#define FALSE 0
#define TRUE 1

#define SHAPE_SIZE 4
#define ROTATION_COUNT 4

#define COLLISION_OUT_OF_BOUNDS 1
#define COLLISION_BLOCK_ALREADY_EXISTS 2

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