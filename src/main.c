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
static unsigned char xdim, ydim, isGameOver, curTet;
static unsigned char curPos[4][2];
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
static unsigned char xTet;
static unsigned char yTet;
static unsigned char curColor;
static unsigned char initPlacement;
static unsigned char rotState;

// Data
static unsigned char i = 0;
static unsigned char j = 0;


static unsigned char isPlaced = 0;
static unsigned char blockTile = 169;
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


// TODO: Optimize the array assignments
static void drawTet() {
        // 169 - Block tile
        
        
        // Tetronomio 0: I piece
        // Spawns in middle column
        // Cyan, COLOR_LIGHTBLUE
        if (curTet == 0) {
                textcolor(COLOR_LIGHTBLUE);
                curColor = COLOR_LIGHTBLUE;
                cputcxy(9, 3, blockTile);
                curPos[0][0] = 9;
                curPos[0][1] = 3; 

                cputcxy(10, 3, blockTile);
                curPos[1][0] = 10;
                curPos[1][1] = 3;

                cputcxy(11, 3, blockTile);
                curPos[2][0] = 11;
                curPos[2][1] = 3;

                cputcxy(12, 3, blockTile);
                curPos[3][0] = 12;
                curPos[3][1] = 3;

                xTet = 9;
                yTet = 4;
        }

        // Tetronomio 1: O piece
        // Spawns in middle as well
        // Yellow, COLOR_YELLOW

        if (curTet == 1) {
                textcolor(COLOR_YELLOW);
                curColor = COLOR_YELLOW;
                cputcxy(10, 3, blockTile);
                curPos[0][0] = 10;
                curPos[0][1] = 3;

                cputcxy(11, 3, blockTile);
                curPos[1][0] = 11;
                curPos[1][1] = 3;

                cputcxy(10, 4, blockTile);
                curPos[2][0] = 10;
                curPos[2][1] = 4;

                cputcxy(11, 4, blockTile);
                curPos[3][0] = 11;
                curPos[3][1] = 4;

                xTet = 10;
                yTet = 4;
        }

        // Tetronomio 2: T piece
        // Purple, COLOR_PURPLE
        if (curTet == 2) {
                textcolor(COLOR_PURPLE);
                curColor = COLOR_PURPLE;
                cputcxy(initPlacement, 3, blockTile);
                curPos[0][0] = initPlacement;
                curPos[0][1] = 3;

                cputcxy(initPlacement + 1, 3, blockTile);
                curPos[1][0] = initPlacement + 1;
                curPos[1][1] = 3;

                cputcxy(initPlacement + 2, 3, blockTile);
                curPos[2][0] = initPlacement + 2;
                curPos[2][1] = 3;

                cputcxy(initPlacement + 1, 4, blockTile);
                curPos[3][0] = initPlacement + 1;
                curPos[3][1] = 4;

                xTet = initPlacement;
                yTet = 4;
        }
        
        // Tetronomio 3: S piece
        // Green, COLOR_GREEN
        if (curTet == 3) {
                textcolor(COLOR_GREEN);
                curColor = COLOR_GREEN;
                cputcxy(initPlacement, 4, blockTile);
                curPos[0][0] = initPlacement;
                curPos[0][1] = 4;

                cputcxy(initPlacement + 1, 4, blockTile);
                curPos[1][0] = initPlacement + 1;
                curPos[1][1] = 4;

                cputcxy(initPlacement + 1, 3, blockTile);
                curPos[2][0] = initPlacement + 1;
                curPos[2][1] = 3;

                cputcxy(initPlacement + 2, 3, blockTile);
                curPos[3][0] = initPlacement + 2;
                curPos[3][1] = 3;


                xTet = initPlacement;
                yTet = 4;
        }

        // Tetronomio 4: Z piece
        // Red, COLOR_RED
        if (curTet == 4) {
                textcolor(COLOR_RED);
                curColor = COLOR_RED;
                cputcxy(initPlacement, 3, blockTile);
                curPos[0][0] = initPlacement;
                curPos[0][1] = 3;

                cputcxy(initPlacement + 1, 3, blockTile);
                curPos[1][0] = initPlacement + 1;
                curPos[1][1] = 3;

                cputcxy(initPlacement + 1, 4, blockTile);
                curPos[2][0] = initPlacement + 1;
                curPos[2][1] = 4;

                cputcxy(initPlacement + 2, 4, blockTile);
                curPos[3][0] = initPlacement + 2;
                curPos[3][1] = 4;

                xTet = initPlacement;
                yTet = 4;
        }
        
        // Tetronomio 5: J piece
        // Orange, COLOR_ORANGE
        if (curTet == 5) {
                textcolor(COLOR_ORANGE);
                curColor = COLOR_ORANGE;
                cputcxy(initPlacement, 3, blockTile);
                curPos[0][0] = initPlacement;
                curPos[0][1] = 3;

                cputcxy(initPlacement, 4, blockTile);
                curPos[1][0] = initPlacement;
                curPos[1][1] = 4;

                cputcxy(initPlacement + 1, 4, blockTile);
                curPos[2][0] = initPlacement + 1;
                curPos[2][1] = 4;

                cputcxy(initPlacement + 2, 4, blockTile);
                curPos[3][0] = initPlacement + 2;
                curPos[3][1] = 4;

                xTet = initPlacement;
                yTet = 4;
        }

        // Tetronomio 6: L piece
        // Blue, COLOR_BLUE
        if (curTet == 6) {
                textcolor(COLOR_BLUE);
                curColor = COLOR_BLUE;
                cputcxy(initPlacement, 3, blockTile);
                curPos[0][0] = initPlacement;
                curPos[0][1] = 3;

                cputcxy(initPlacement, 4, blockTile);
                curPos[1][0] = initPlacement;
                curPos[1][1] = 4;

                cputcxy(initPlacement + 1, 4, blockTile);
                curPos[2][0] = initPlacement + 1;
                curPos[2][1] = 4;

                cputcxy(initPlacement + 2, 4, blockTile);
                curPos[3][0] = initPlacement + 2;
                curPos[3][1] = 4;

                xTet = initPlacement;
                yTet = 4;
        }

        rotState = 0;
}

static void pickTet() {
        _randomize();
        curTet = rand() % 7;
        initPlacement = (rand() % 7) + 6;
}

static unsigned char checkCollision() {
        unsigned char botY = 0;

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

static void handleTet() {
        if (checkCollision() == 1) {
                isPlaced = 1;
        } else {
                
                i = 0;
                while (i < 4) {
                        textcolor(COLOR_BLACK);
                        cclearxy(curPos[i][0], curPos[i][1], 1);
                        ++i;
                }

                i = 0;
                while (i < 4) {
                        unsigned char temp = curPos[i][1];
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
}

static void game_loop() {
        pickTet();
        drawTet();

        


        while (isGameOver != 1) {

                if (isPlaced == 1) {
                        pickTet();
                        drawTet();
                        isPlaced = 0;
                }

                handleTet();

                asm("lda #0");
                asm("sta 162");
                asm("wait: lda 162");
                asm("cmp #30");
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

