#include <stdlib.h>
//include <tgi.h>
#include <cbm.h>
#include <cc65.h>
#include <conio.h>
#include <string.h>
#include <unistd.h>


#  define BUFFER                0x0400
#  define SCREEN1               0xE000
#  define SCREEN2               0xE400
#  define CHARSET               0xE800
#  define COLORRAM              0xD800
#  define outb(addr,val)        (*(addr) = (val))
#  define inb(addr)             (*(addr))

typedef unsigned char BYTE;
// #define DOWN 0
// #define LEFT 1
// #define RIGHT 2
/*
        (6,2)          (16, 2)
        +-------------+
        |             |
        |             |
        |             |
        |             |
        |             |
        |             |
        |             |
        |             |
        |             |
        +-------------+
        (6,23)
 */ 

// Notes:
//      Accessing "fixec" (global) variables in functions that
//      take parameters is expensive -- it is more efficient to
//      copy the global variable to a local variable within that 
//      function. I'm too lazy to do that and most of my vars 
//      are more suited to global vars so none of these functions
//      take parameters lol
//      
//      Using the array operator (e.g. arr[3]) is more efficient
//      than trying to add and dereference using a pointer, which
//      I normally do anyway.
//      
//      Additionally, initializing variables leads to faster code,
//      although it may only apply to local variables.
//      
//      Preincrement and predecrement (++i, --i) produces faster code
//      than postincrement/postdecrement.
//      
//      Try to define things as constant as much as possible, and 
//      remember that math works left to right.
//      
//      Text mode resolution is 40 x 25



// Vars
// Apparently grouping together non-initialized vars
// creates faster code, although that may just be for local variables
static BYTE xdim, ydim, isGameOver, curTet;
static BYTE curPos[4][2];
static unsigned int linesCleared;

// The x and y coordinates of the tetronimos are 
// the bottom left of each tetronimo's "square of influence", for example:
//      ####
//      ####
//      IIII
//      ####
//      ^is counted as the coordinate
//      
//      ##Z
//      #ZZ
//      #Z#
//      ^is counted as the coordinate
static BYTE xTet;
static BYTE yTet;
static BYTE curColor;
static BYTE initPlacement;
static BYTE rotState;
static BYTE dirKey;

// Data
static BYTE i = 0;
static BYTE j = 0;


static BYTE isPlaced = 0;
static BYTE blockTile = 169;
static const char Title[] = "TETRIS64";
static const char Inst[] = "PRESS X TO PLAY";


// Code

static void draw_title(void) {
        clrscr();
        bgcolor(COLOR_BLACK);
        bordercolor(COLOR_BLACK);
        textcolor(COLOR_WHITE);

        gotoxy((xdim - strlen(Title)) / 2, (ydim / 2) - 1);
        cprintf("%s", Title);

        gotoxy((xdim - strlen(Inst)) / 2, ydim/2);
        cprintf("%s", Inst);
}

static void draw_game(void) {
        clrscr(); // This sets cursor back to top left (0,0)
        i = 1;

        // 239: bottom bar
        // 234: right bar
        // 244: left bar
        // 247: top bar
        
        // Draw top
        while (i <= 10) {
                cputcxy(5 + i, 2, 239);
                ++i; // this optimizes for the cc65 compiler
        }

        // Draw bottom
        i = 1;
        while (i <= 10) {
                cputcxy(5 + i, 23, 247);
                ++i;
        }

        // Draw left
        i = 1;
        while (i <= 20) {
                cputcxy(5, 2 + i, 234);
                ++i;
        }

        // Draw right
        i = 1;
        while (i <= 20) {
                cputcxy(16, 2 + i, 244);
                ++i;
        }

        j = 3;
        i = 7;
        while (i < 15) {
                while (j < 23) {
                        cputcxy(i, j, 32);
                        ++j;
                }
                ++i;
        }

}

static BYTE checkIfEmpty (BYTE x, BYTE y) {
        gotoxy(x, y);
        if (cpeekc() != 32) {
                return 1;
        }
        return 0;
}

// TODO: Optimize the array assignments
static void drawTet() {
        // 169 - Block tile
        
        
        // Tetronomio 0: I piece
        // Spawns in middle column
        // Cyan, COLOR_LIGHTBLUE
        if (curTet == 0) {
                curColor = COLOR_LIGHTBLUE;

                curPos[0][0] = 9;
                curPos[0][1] = 3; 

                curPos[1][0] = 10;
                curPos[1][1] = 3;

                curPos[2][0] = 11;
                curPos[2][1] = 3;

                curPos[3][0] = 12;
                curPos[3][1] = 3;

                xTet = 9;
                yTet = 4;
        }

        // Tetronomio 1: O piece
        // Spawns in middle as well
        // Yellow, COLOR_YELLOW

        if (curTet == 1) {
                curColor = COLOR_YELLOW;

                curPos[0][0] = 10;
                curPos[0][1] = 3;

                curPos[1][0] = 11;
                curPos[1][1] = 3;

                curPos[2][0] = 10;
                curPos[2][1] = 4;

                curPos[3][0] = 11;
                curPos[3][1] = 4;

                xTet = 10;
                yTet = 4;
        }

        // Tetronomio 2: T piece
        // Purple, COLOR_PURPLE
        if (curTet == 2) {
                curColor = COLOR_PURPLE;

                curPos[0][0] = initPlacement;
                curPos[0][1] = 3;

                curPos[1][0] = initPlacement + 1;
                curPos[1][1] = 3;

                curPos[2][0] = initPlacement + 2;
                curPos[2][1] = 3;

                curPos[3][0] = initPlacement + 1;
                curPos[3][1] = 4;

                xTet = initPlacement;
                yTet = 4;
        }
        
        // Tetronomio 3: S piece
        // Green, COLOR_GREEN
        if (curTet == 3) {
                curColor = COLOR_GREEN;

                curPos[0][0] = initPlacement;
                curPos[0][1] = 4;

                curPos[1][0] = initPlacement + 1;
                curPos[1][1] = 4;

                curPos[2][0] = initPlacement + 1;
                curPos[2][1] = 3;

                curPos[3][0] = initPlacement + 2;
                curPos[3][1] = 3;


                xTet = initPlacement;
                yTet = 4;
        }

        // Tetronomio 4: Z piece
        // Red, COLOR_RED
        if (curTet == 4) {
                curColor = COLOR_RED;

                curPos[0][0] = initPlacement;
                curPos[0][1] = 3;

                curPos[1][0] = initPlacement + 1;
                curPos[1][1] = 3;

                curPos[2][0] = initPlacement + 1;
                curPos[2][1] = 4;

                curPos[3][0] = initPlacement + 2;
                curPos[3][1] = 4;

                xTet = initPlacement;
                yTet = 4;
        }
        
        // Tetronomio 5: J piece
        // Orange, COLOR_ORANGE
        if (curTet == 5) {
                curColor = COLOR_ORANGE;

                curPos[0][0] = initPlacement;
                curPos[0][1] = 3;

                curPos[1][0] = initPlacement + 1;
                curPos[1][1] = 3;

                curPos[2][0] = initPlacement + 2;
                curPos[2][1] = 3;

                curPos[3][0] = initPlacement + 2;
                curPos[3][1] = 4;

                xTet = initPlacement;
                yTet = 4;
        }

        // Tetronomio 6: L piece
        // Blue, COLOR_BLUE
        if (curTet == 6) {
                curColor = COLOR_BLUE;

                curPos[0][0] = initPlacement;
                curPos[0][1] = 3;

                curPos[1][0] = initPlacement + 1;
                curPos[1][1] = 3;

                curPos[2][0] = initPlacement + 2;
                curPos[2][1] = 3;

                curPos[3][0] = initPlacement;
                curPos[3][1] = 4;

                xTet = initPlacement;
                yTet = 4;
        }

        rotState = 0;

        i = 0;
        while (i < 4) {
                if (checkIfEmpty(curPos[i][0], curPos[i][1]) == 1) {
                        isGameOver = 1;
                        return;
                }
                ++i;
        }

        i = 0;
        textcolor(curColor);
        while (i < 4) {
                cputcxy(curPos[i][0], curPos[i][1], blockTile);
                ++i;
        }
}

static void pickTet() {
        _randomize();
        curTet = rand() % 7;
        initPlacement = (rand() % 7) + 6;
}

static BYTE checkBotCollision() {
        BYTE botY = 0;

        i = 0;
        while (i < 4) {
                if (curPos[i][1] > botY) {
                        botY = curPos[i][1];
                }
                ++i;
        }
        
        i = 0;
        while (i < 4) {
                if (curPos[i][1] < botY) {
                        ++i;
                        continue;
                }
                gotoxy(curPos[i][0], curPos[i][1] + 1);
                if (cpeekc() != 32) {
                        return 1;
                }
                ++i;
        }

        return 0;
}

static void moveTet(BYTE dir) {
        if (dir == 0) { // Move down, positive Y
               i = 0;
                while (i < 4) {
                        textcolor(COLOR_BLACK);
                        cclearxy(curPos[i][0], curPos[i][1], 1);
                        ++i;
                }

                i = 0;
                while (i < 4) {
                        BYTE temp = curPos[i][1];
                        curPos[i][1] = temp + 1;
                        ++i;
                }

                i = 0;
                textcolor(curColor);
                while (i < 4) {
                        cputcxy(curPos[i][0], curPos[i][1], 169);
                        ++i;
                } 
        }

        if (dir == 1) { // Move left, negative X
               i = 0;
                while (i < 4) {
                        textcolor(COLOR_BLACK);
                        cclearxy(curPos[i][0], curPos[i][1], 1);
                        ++i;
                }

                i = 0;
                while (i < 4) {
                        BYTE temp = curPos[i][0];
                        curPos[i][0] = temp - 1;
                        ++i;
                }

                i = 0;
                textcolor(curColor);
                while (i < 4) {
                        cputcxy(curPos[i][0], curPos[i][1], 169);
                        ++i;
                } 
        }

        if (dir == 2) { // Move right, positive X
               i = 0;
                while (i < 4) {
                        textcolor(COLOR_BLACK);
                        cclearxy(curPos[i][0], curPos[i][1], 1);
                        ++i;
                }

                i = 0;
                while (i < 4) {
                        BYTE temp = curPos[i][0];
                        curPos[i][0] = temp + 1;
                        ++i;
                }

                i = 0;
                textcolor(curColor);
                while (i < 4) {
                        cputcxy(curPos[i][0], curPos[i][1], 169);
                        ++i;
                } 
        }
} 

static void handleTet() {
        if (checkBotCollision() == 1) {
                isPlaced = 1;
        } else {
                moveTet(0);
        }
}


static BYTE checkLeftCollision() {
        BYTE leftX = 255;
        i = 0;

        while (i < 4) {
                if (curPos[i][0] < leftX) {
                        leftX = curPos[i][0];
                }
                ++i;
        }

        i = 0;
        while (i < 4) {
                if (curPos[i][0] > leftX) {
                        ++i;
                        continue;
                }
                gotoxy(curPos[i][0] - 1, curPos[i][1]);
                if (cpeekc() != 32) {
                        return 1;
                }
                ++i;
        }
        return 0;
}

static BYTE checkRightCollision() {
        BYTE rightX = 0;
        i = 0;

        while (i < 4) {
                if (curPos[i][0] > rightX) {
                        rightX = curPos[i][0];
                }
                ++i;
        }

        i = 0;
        while (i < 4) {
                if (curPos[i][0] < rightX) {
                        ++i;
                        continue;
                }
                gotoxy(curPos[i][0] + 1, curPos[i][1]);
                if (cpeekc() != 32) {
                        return 1;
                }
                ++i;
        }
        return 0;
}

static BYTE checkRotateCollision(BYTE rotDir) {
        BYTE checker = 0;

        if (curTet == 0) { // I-piece
                if (rotState == 0) {
                        checker += checkIfEmpty(curPos[2][0], curPos[2][1] + 1);
                        checker += checkIfEmpty(curPos[2][0], curPos[2][1] - 1);
                        checker += checkIfEmpty(curPos[2][0], curPos[2][1] - 2);
                        if (checker != 0) {
                                return 1;
                        }
                        return 0;
                }

                checker += checkIfEmpty(curPos[2][0] + 1, curPos[2][1]);
                checker += checkIfEmpty(curPos[2][0] - 1, curPos[2][1]);
                checker += checkIfEmpty(curPos[2][0] - 2, curPos[2][1]);
                if (checker != 0) {
                        return 1;
                }
                return 0;
        }

        // Skip O-piece, does not rotate

        if (curTet == 2) { // T-piece
                if (rotState == 0) {
                        if (checkIfEmpty(curPos[1][0], curPos[1][1] - 1) != 0) {
                                return 1;
                        } 
                        return 0;                  
                }

                if (rotState == 1) {
                        if (checkIfEmpty(curPos[1][0] + 1, curPos[1][1]) != 0) {
                                return 1;
                        }
                        return 0;
                }

                if (rotState == 2) {
                        if (checkIfEmpty(curPos[1][0], curPos[1][1] + 1) != 0) {
                                return 1;
                        }
                        return 0;
                }
                if (rotState == 3) {
                        
                        if (checkIfEmpty(curPos[1][0] - 1, curPos[1][1]) != 0) {
                                return 1;
                        }
                        return 0; 
                       
                }
        }

        if (curTet == 3) { // S-piece
                if (rotState == 0) {
                     checker += checkIfEmpty(curPos[2][0], curPos[2][1] - 1);
                     checker += checkIfEmpty(curPos[2][0] + 1, curPos[2][1] + 1);
                     if (checker != 0) {
                        return 1;
                     }
                     return 0;
                }

                checker += checkIfEmpty(curPos[1][0], curPos[1][1] + 1);
                checker += checkIfEmpty(curPos[1][0] - 1, curPos[1][1] + 1);
                if (checker != 0) {
                        return 1;
                }
                return 0;
        }

        if (curTet == 4) { // Z-piece
                if (rotState == 0) {
                     checker += checkIfEmpty(curPos[1][0] + 1, curPos[1][1] - 1);
                     checker += checkIfEmpty(curPos[1][0] + 1, curPos[1][1]);
                     if (checker != 0) {
                        return 1;
                     }
                     return 0;
                }

                checker += checkIfEmpty(curPos[2][0] - 1, curPos[1][1]);
                checker += checkIfEmpty(curPos[2][0] + 1, curPos[2][1] + 1);
                if (checker != 0) {
                        return 1;
                }
                return 0;
        }

        if (curTet == 5) { // J-piece, some of the cases can be combined but I'm wayyy to lazy
                if (rotState == 0) {
                        if (rotDir == 0) {
                                checker += checkIfEmpty(curPos[1][0], curPos[1][1] - 1);
                                checker += checkIfEmpty(curPos[1][0], curPos[1][1] + 1);
                                checker += checkIfEmpty(curPos[1][0] - 1, curPos[1][1] + 1);
                                if (checker != 0) {
                                        return 1;
                                }
                                return 0;
                        }
                        checker += checkIfEmpty(curPos[1][0], curPos[1][1] - 1);
                        checker += checkIfEmpty(curPos[1][0], curPos[1][1] + 1);
                        checker += checkIfEmpty(curPos[1][0] + 1, curPos[1][1] - 1);
                        if (checker != 0) {
                                return 1;
                        }
                        return 0;
                }

                if (rotState == 1) {
                        if (rotDir == 0) {
                                checker += checkIfEmpty(curPos[1][0] + 1, curPos[1][1]);
                                checker += checkIfEmpty(curPos[1][0] - 1, curPos[1][1]);
                                checker += checkIfEmpty(curPos[1][0] + 1, curPos[1][1] - 1);
                                if (checker != 0) {
                                        return 1;
                                }
                                return 0;
                        }
                        checker += checkIfEmpty(curPos[1][0] - 1, curPos[1][1]);
                        checker += checkIfEmpty(curPos[1][0] + 1, curPos[1][1]);
                        checker += checkIfEmpty(curPos[1][0] + 1, curPos[1][1] + 1);
                        if (checker != 0) {
                                return 1;
                        }
                        return 0;
                }

                if (rotState == 2) {
                        if (rotDir == 0) {
                                checker += checkIfEmpty(curPos[1][0], curPos[1][1] - 1);
                                checker += checkIfEmpty(curPos[1][0], curPos[1][1] + 1);
                                checker += checkIfEmpty(curPos[1][0] + 1, curPos[1][1] - 1);
                                if (checker != 0) {
                                        return 1;
                                }
                                return 0;
                        }
                        checker += checkIfEmpty(curPos[1][0] - 1, curPos[1][1]);
                        checker += checkIfEmpty(curPos[1][0] + 1, curPos[1][1]);
                        checker += checkIfEmpty(curPos[1][0] - 1, curPos[1][1] + 1);
                        if (checker != 0) {
                                return 1;
                        }
                        return 0;
                }

                if (rotState == 3) {
                        if (rotDir == 0) {
                                checker += checkIfEmpty(curPos[1][0] - 1, curPos[1][1]);
                                checker += checkIfEmpty(curPos[1][0] + 1, curPos[1][1]);
                                checker += checkIfEmpty(curPos[1][0] + 1, curPos[1][1] + 1);
                                if (checker != 0) {
                                        return 1;
                                }
                                return 0;
                        }
                        checker += checkIfEmpty(curPos[1][0] - 1, curPos[1][1]);
                        checker += checkIfEmpty(curPos[1][0] + 1, curPos[1][1]);
                        checker += checkIfEmpty(curPos[1][0] - 1, curPos[1][1] - 1);
                        if (checker != 0) {
                                return 1;
                        }
                        return 0;
                }
        }

        if (curTet == 6) { // L-piece, some of the cases can be combined but I'm wayyy to lazy
                if (rotState == 0) {
                        if (rotDir == 0) {
                                checker += checkIfEmpty(curPos[1][0], curPos[1][1] - 1);
                                checker += checkIfEmpty(curPos[1][0], curPos[1][1] + 1);
                                checker += checkIfEmpty(curPos[1][0] - 1, curPos[1][1] - 1);
                                if (checker != 0) {
                                        return 1;
                                }
                                return 0;
                        }
                        checker += checkIfEmpty(curPos[1][0], curPos[1][1] - 1);
                        checker += checkIfEmpty(curPos[1][0], curPos[1][1] + 1);
                        checker += checkIfEmpty(curPos[1][0] + 1, curPos[1][1] + 1);
                        if (checker != 0) {
                                return 1;
                        }
                        return 0;
                }

                if (rotState == 1) {
                        if (rotDir == 0) {
                                checker += checkIfEmpty(curPos[1][0] + 1, curPos[1][1]);
                                checker += checkIfEmpty(curPos[1][0] - 1, curPos[1][1]);
                                checker += checkIfEmpty(curPos[1][0] + 1, curPos[1][1] - 1);
                                if (checker != 0) {
                                        return 1;
                                }
                                return 0;
                        }
                        checker += checkIfEmpty(curPos[1][0] - 1, curPos[1][1]);
                        checker += checkIfEmpty(curPos[1][0] + 1, curPos[1][1]);
                        checker += checkIfEmpty(curPos[1][0] + 1, curPos[1][1] - 1);
                        if (checker != 0) {
                                return 1;
                        }
                        return 0;
                }

                if (rotState == 2) {
                        if (rotDir == 0) {
                                checker += checkIfEmpty(curPos[1][0], curPos[1][1] - 1);
                                checker += checkIfEmpty(curPos[1][0], curPos[1][1] + 1);
                                checker += checkIfEmpty(curPos[1][0] + 1, curPos[1][1] + 1);
                                if (checker != 0) {
                                        return 1;
                                }
                                return 0;
                        }
                        checker += checkIfEmpty(curPos[1][0] - 1, curPos[1][1]);
                        checker += checkIfEmpty(curPos[1][0] + 1, curPos[1][1]);
                        checker += checkIfEmpty(curPos[1][0] - 1, curPos[1][1] - 1);
                        if (checker != 0) {
                                return 1;
                        }
                        return 0;
                }

                if (rotState == 3) {
                        if (rotDir == 0) {
                                checker += checkIfEmpty(curPos[1][0] - 1, curPos[1][1]);
                                checker += checkIfEmpty(curPos[1][0] + 1, curPos[1][1]);
                                checker += checkIfEmpty(curPos[1][0] - 1, curPos[1][1] - 1);
                                if (checker != 0) {
                                        return 1;
                                }
                                return 0;
                        }
                        checker += checkIfEmpty(curPos[1][0] - 1, curPos[1][1]);
                        checker += checkIfEmpty(curPos[1][0] + 1, curPos[1][1]);
                        checker += checkIfEmpty(curPos[1][0] + 1, curPos[1][1] - 1);
                        if (checker != 0) {
                                return 1;
                        }
                        return 0;
                }
        }


}

static void rotClockwise() {
        if (curTet == 0) { // I-piece
                cclearxy(curPos[0][0], curPos[0][1], 1);
                cclearxy(curPos[1][0], curPos[1][1], 1);
                cclearxy(curPos[3][0], curPos[3][1], 1);

                if (rotState == 0) {
                        curPos[0][0] = curPos[2][0];
                        curPos[0][1] = curPos[2][1] - 2;

                        curPos[1][0] = curPos[2][0];
                        curPos[1][1] = curPos[2][1] - 1;

                        curPos[3][0] = curPos[2][0];
                        curPos[3][1] = curPos[2][1] + 1;
                        rotState = 1;
                }
                else if (rotState == 1) {
                        curPos[0][0] = curPos[2][0] - 2;
                        curPos[0][1] = curPos[2][1];

                        curPos[1][0] = curPos[2][0] - 1;
                        curPos[1][1] = curPos[2][1];

                        curPos[3][0] = curPos[2][0] + 1;
                        curPos[3][1] = curPos[2][1];
                        rotState = 0;
                }
        }

        else if (curTet == 2) { // T-piece
                if (rotState == 0) {
                        cclearxy(curPos[2][0], curPos[2][1], 1);

                        curPos[0][0] = curPos[1][0];
                        curPos[0][1] = curPos[1][1] - 1;

                        curPos[2][0] = curPos[1][0] - 1;
                        curPos[2][1] = curPos[1][1];

                        curPos[3][0] = curPos[1][0];
                        curPos[3][1] = curPos[1][1] + 1;

                        rotState = 1;
                }

                else if (rotState == 1) {
                        cclearxy(curPos[3][0], curPos[3][1], 1);

                        curPos[0][0] = curPos[1][0] - 1;
                        curPos[0][1] = curPos[1][1];

                        curPos[2][0] = curPos[1][0] + 1;
                        curPos[2][1] = curPos[1][1];

                        curPos[3][0] = curPos[1][0];
                        curPos[3][1] = curPos[1][1] - 1;

                        rotState = 2;
                }

                else if (rotState == 2) {
                        cclearxy(curPos[0][0], curPos[0][1], 1);

                        curPos[0][0] = curPos[1][0];
                        curPos[0][1] = curPos[1][1] - 1;

                        curPos[2][0] = curPos[1][0];
                        curPos[2][1] = curPos[1][1] + 1;

                        curPos[3][0] = curPos[1][0] + 1;
                        curPos[3][1] = curPos[1][1];

                        rotState = 3;
                }

                else if (rotState == 3) {
                        cclearxy(curPos[0][0], curPos[0][1], 1);

                        curPos[0][0] = curPos[1][0] - 1;
                        curPos[0][1] = curPos[1][1];

                        curPos[2][0] = curPos[1][0] + 1;
                        curPos[2][1] = curPos[1][1];

                        curPos[3][0] = curPos[1][0];
                        curPos[3][1] = curPos[1][1] + 1;

                        rotState = 0;  
                }
        }

        else if (curTet == 3) { // S-piece
                if (rotState == 0) {
                        cclearxy(curPos[0][0], curPos[0][1], 1);
                        cclearxy(curPos[1][0], curPos[1][1], 1);

                        curPos[0][0] = curPos[2][0];
                        curPos[0][1] = curPos[2][1] - 1;

                        curPos[1][0] = curPos[2][0];
                        curPos[1][1] = curPos[2][1];

                        curPos[3][0] = curPos[2][0] + 1;
                        curPos[3][1] = curPos[2][1] + 1;

                        curPos[2][0] = curPos[1][0] + 1;
                        curPos[2][1] = curPos[1][1];

                        rotState = 1;
                }
                else if (rotState == 1) {
                        cclearxy(curPos[0][0], curPos[0][1], 1);
                        cclearxy(curPos[3][0], curPos[3][1], 1);

                        curPos[0][0] = curPos[1][0] - 1;
                        curPos[0][1] = curPos[1][1] + 1;

                        curPos[2][0] = curPos[1][0];
                        curPos[2][1] = curPos[1][1];

                        curPos[3][0] = curPos[1][0] + 1;
                        curPos[3][1] = curPos[1][1];

                        curPos[1][0] = curPos[2][0];
                        curPos[1][1] = curPos[2][1] + 1;

                        rotState = 0;
                }
        }

        else if (curTet == 4) { // Z-piece
                if (rotState == 0) {
                        cclearxy(curPos[0][0], curPos[0][1], 1);
                        cclearxy(curPos[1][0], curPos[1][1], 1);

                        curPos[0][0] = curPos[1][0] + 1;
                        curPos[0][1] = curPos[1][1] - 1;

                        curPos[2][0] = curPos[1][0];
                        curPos[2][1] = curPos[1][1];

                        curPos[3][0] = curPos[1][0];
                        curPos[3][1] = curPos[1][1] + 1;

                        curPos[1][0] = curPos[2][0] + 1;
                        curPos[1][1] = curPos[2][1];

                        rotState = 1;
                }
                else if (rotState == 1) {
                        cclearxy(curPos[0][0], curPos[0][1], 1);
                        cclearxy(curPos[1][0], curPos[1][1], 1);

                        curPos[0][0] = curPos[2][0] - 1;
                        curPos[0][1] = curPos[2][1];

                        curPos[1][0] = curPos[2][0];
                        curPos[1][1] = curPos[2][1];

                        curPos[3][0] = curPos[2][0] + 1;
                        curPos[3][1] = curPos[2][1] + 1;

                        curPos[2][0] = curPos[2][0];
                        curPos[2][1] = curPos[2][1] + 1;

                        rotState = 0;
                }

        }

        else if (curTet == 5) { // J-piece
                cclearxy(curPos[0][0], curPos[0][1], 1);
                cclearxy(curPos[2][0], curPos[2][1], 1);
                cclearxy(curPos[3][0], curPos[3][1], 1);

                if (rotState == 0) {
                        curPos[0][0] = curPos[1][0];
                        curPos[0][1] = curPos[1][1] - 1;

                        curPos[2][0] = curPos[1][0];
                        curPos[2][1] = curPos[1][1] + 1;

                        curPos[3][0] = curPos[1][0] - 1;
                        curPos[3][1] = curPos[1][1] + 1;

                        rotState = 1;
                }

                else if (rotState == 1) {
                        curPos[0][0] = curPos[1][0] + 1;
                        curPos[0][1] = curPos[1][1];

                        curPos[2][0] = curPos[1][0] - 1;
                        curPos[2][1] = curPos[1][1];

                        curPos[3][0] = curPos[1][0] - 1;
                        curPos[3][1] = curPos[1][1] - 1;

                        rotState = 2;
                }

                else if (rotState == 2) {
                        curPos[0][0] = curPos[1][0];
                        curPos[0][1] = curPos[1][1] + 1;

                        curPos[2][0] = curPos[1][0];
                        curPos[2][1] = curPos[1][1] - 1;

                        curPos[3][0] = curPos[1][0] + 1;
                        curPos[3][1] = curPos[1][1] - 1;

                        rotState = 3;
                }

                else if (rotState == 3) {
                        curPos[0][0] = curPos[1][0] - 1;
                        curPos[0][1] = curPos[1][1];

                        curPos[2][0] = curPos[1][0] + 1;
                        curPos[2][1] = curPos[1][1];

                        curPos[3][0] = curPos[1][0] + 1;
                        curPos[3][1] = curPos[1][1] + 1;

                        rotState = 0;
                }
        }

        else if (curTet == 6) { // L-piece      
                cclearxy(curPos[0][0], curPos[0][1], 1);
                cclearxy(curPos[2][0], curPos[2][1], 1);
                cclearxy(curPos[3][0], curPos[3][1], 1);

                if (rotState == 0) {
                        curPos[0][0] = curPos[1][0];
                        curPos[0][1] = curPos[1][1] - 1;

                        curPos[2][0] = curPos[1][0];
                        curPos[2][1] = curPos[1][1] + 1;

                        curPos[3][0] = curPos[1][0] - 1;
                        curPos[3][1] = curPos[1][1] - 1;

                        rotState = 1;
                }

                else if (rotState == 1) {
                        curPos[0][0] = curPos[1][0] + 1;
                        curPos[0][1] = curPos[1][1];

                        curPos[2][0] = curPos[1][0] - 1;
                        curPos[2][1] = curPos[1][1];

                        curPos[3][0] = curPos[1][0] + 1;
                        curPos[3][1] = curPos[1][1] - 1;

                        rotState = 2;
                }

                else if (rotState == 2) {
                        curPos[0][0] = curPos[1][0];
                        curPos[0][1] = curPos[1][1] + 1;

                        curPos[2][0] = curPos[1][0];
                        curPos[2][1] = curPos[1][1] - 1;

                        curPos[3][0] = curPos[1][0] + 1;
                        curPos[3][1] = curPos[1][1] + 1;

                        rotState = 3;
                }

                else if (rotState == 3) {
                        curPos[0][0] = curPos[1][0] - 1;
                        curPos[0][1] = curPos[1][1];

                        curPos[2][0] = curPos[1][0] + 1;
                        curPos[2][1] = curPos[1][1];

                        curPos[3][0] = curPos[1][0] - 1;
                        curPos[3][1] = curPos[1][1] + 1;

                        rotState = 0;
                }
        }

        i = 0;
        textcolor(curColor);
        while (i < 4) {
                cputcxy(curPos[i][0], curPos[i][1], blockTile);
                ++i;
        }
        return; 
}

static void checkMove() {
        if (kbhit() != 1) { 
                return;
        } 
        
        dirKey = cgetc();
        

        if (dirKey == 'a') {
                if (checkLeftCollision() == 0) {
                        moveTet(1); // move left
                }
        }

        if (dirKey == 'd') {
                if (checkRightCollision() == 0) {
                        moveTet(2); // move right
                }
        }

        if (dirKey == 's') {
                if (checkBotCollision() == 0 && isPlaced == 0) {
                        moveTet(0);
                }
        }


        if (dirKey == 'w') {
                if (checkRotateCollision(0) == 0) {
                        rotClockwise();
                }
        }
}

static void game_loop() {
        pickTet();
        drawTet();
        //#define CLK 162

        while (isGameOver != 1) {
                
                if (isPlaced == 1) {
                        pickTet();
                        drawTet();
                        isPlaced = 0;
                }

                checkMove();
                handleTet();


                // I could not think of a different way to 
                // do pauses. 
                // TIMEDELAY / 60 = the delay in seconds. 
                
                
                #define TIMEDELAY 30
                asm("lda #0");
                asm("sta 162");
                asm("wait: lda 162");
                asm("cmp #%b", TIMEDELAY);
                asm("bne wait");
        } 
}

int main (void) {

        /**
         * This was originally here to load and initialize the tgi interface (which is in tgi.h),
         * but I found that conio.h was much faster and suited to text-based Tetris.
         */
        // tgi_install(tgi_static_stddrv);
        // tgi_init();
        cursor(0); // Turn off cursor when checking for input
        screensize(&xdim, &ydim); // Actually doesn't return anything, seems to deref and assign screesize to xdim and ydim

        draw_title();
        while (cgetc() != 'x') {
                ; // Empty loop to check for X key trigger
        }

        //srand(time(1));
        draw_game();
        game_loop();
        
        

        
        // tgi_uninstall();

        return EXIT_SUCCESS;
}

