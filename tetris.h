#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <curses.h>
#include <iostream>
#include <string.h>
#include <sys/time.h> 
#include <signal.h> 
#include <vector>

#define GRID_START 6
#define GRID_END 47

#define SCORE_X 4
#define SCORE_Y 2

#define HOLD_X 6
#define HOLD_Y 7

#define LEVEL_X 5
#define LEVEL_Y 25

#define GOAL_X 6
#define GOAL_Y 36

#define NEXT_X 85
#define NEXT_Y 7

#define X_OFFSET 28
#define Y_OFFSET 7

#define CENTER 54
#define BUFFER 3

typedef enum {LEFT, RIGHT, DOWN} direction;
typedef enum {
   EMPTY, I_BLOCK, J_BLOCK, L_BLOCK, O_BLOCK, S_BLOCK, Z_BLOCK, T_BLOCK, DEFAULT
} tetromino;

typedef struct Piece {
   int x;
   int y;
} Piece;

typedef struct Block {
   tetromino type;
   Piece a;
   Piece b;
   Piece c;
   Piece d;
} Block;

int weights[7];

WINDOW * mainwin;
bool initialized = false;
int debug = 0;
int speed = 1000000; //FLAWED... Sec vs uSec
int grid[20][10];
Block block;

void initGame();
void mainLoop();
void closeGame();

bool canMove(direction dir);
void alarm_wakeup (int i);
void redraw();
bool move(direction);
void print(int x, int y, const char* message);
bool isFilled(Piece, int);
void color(int);
void setColors(bool);
void emptyGrid();
void clearLines();
int pseudoRandom();
void space();

void printGrid();
void printBlocks();
void printWeights();
void refreshBlock();
void updateGrid();
