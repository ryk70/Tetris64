#include <stdlib.h>
//include <tgi.h>
#include <cbm.h>
#include <cc65.h>
#include <conio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

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



// Vars
static unsigned char xdim;
static unsigned char ydim;
static unsigned char i;
static unsigned char isGameOver;
static unsigned int linesCleared;
static unsigned char curTet;
static unsigned char xTet;
static unsigned char yTet;
static unsigned char isPlaced;
static unsigned char initPlacement;


// Data
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
}

static void pickTet() {
        _randomize();
        curTet = rand() % 7;
        initPlacement = (rand() % 7) + 6;
}

static void drawTet() {
        // 169 - Block tile
        unsigned char blockTile = 169;
        
        // Tetronomio 0: I piece
        // Spawns in middle column
        // Cyan, COLOR_LIGHTBLUE
        if (curTet == 0) {
                textcolor(COLOR_LIGHTBLUE);
                cputcxy(9, 3, blockTile);
                cputcxy(10, 3, blockTile);
                cputcxy(11, 3, blockTile);
                cputcxy(12, 3, blockTile);
        }

        // Tetronomio 1: O piece
        // Spawns in middle as well
        // Yellow, COLOR_YELLOW

        if (curTet == 1) {
                textcolor(COLOR_YELLOW);
                cputcxy(10, 3, blockTile);
                cputcxy(11, 3, blockTile);
                cputcxy(10, 4, blockTile);
                cputcxy(11, 4, blockTile);
        }

        // Tetronomio 2: T piece
        // Purple, COLOR_PURPLE
        if (curTet == 2) {
                textcolor(COLOR_PURPLE);
                cputcxy(initPlacement, 3, blockTile);
                cputcxy(initPlacement + 1, 3, blockTile);
                cputcxy(initPlacement + 2, 3, blockTile);
                cputcxy(initPlacement + 1, 4, blockTile);
        }
        
        // Tetronomio 3: S piece
        // Green, COLOR_GREEN
        if (curTet == 3) {
                textcolor(COLOR_GREEN);
                cputcxy(initPlacement, 4, blockTile);
                cputcxy(initPlacement + 1, 4, blockTile);
                cputcxy(initPlacement + 1, 3, blockTile);
                cputcxy(initPlacement + 2, 3, blockTile);
        }

        // Tetronomio 4: Z piece
        // Red, COLOR_RED
        if (curTet == 4) {
                textcolor(COLOR_RED);
                cputcxy(initPlacement, 3, blockTile);
                cputcxy(initPlacement + 1, 3, blockTile);
                cputcxy(initPlacement + 1, 4, blockTile);
                cputcxy(initPlacement + 2, 4, blockTile);
        }
        
        // Tetronomio 5: J piece
        // Orange, COLOR_ORANGE
        if (curTet == 5) {
                textcolor(COLOR_ORANGE);
                cputcxy(initPlacement, 3, blockTile);
                cputcxy(initPlacement, 4, blockTile);
                cputcxy(initPlacement + 1, 4, blockTile);
                cputcxy(initPlacement + 2, 4, blockTile);
        }

        // Tetronomio 6: L piece
        // Blue, COLOR_BLUE
        if (curTet == 6) {
                textcolor(COLOR_BLUE);
                cputcxy(initPlacement, 3, blockTile);
                cputcxy(initPlacement, 4, blockTile);
                cputcxy(initPlacement + 1, 4, blockTile);
                cputcxy(initPlacement + 2, 4, blockTile);
        }
}

static void handleTet() {

}

static void game_loop() {
        while (isGameOver != 1) {
                pickTet();
                drawTet();
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

