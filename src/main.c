#include <stdlib.h>
#include <tgi.h>
#include <cbm.h>
#include <cc65.h>
#include <conio.h>

#  define BUFFER                0x0400
#  define SCREEN1               0xE000
#  define SCREEN2               0xE400
#  define CHARSET               0xE800
#  define COLORRAM              0xD800
#  define outb(addr,val)        (*(addr) = (val))
#  define inb(addr)             (*(addr))

static unsigned maxX;
static unsigned maxY;
unsigned X;
static const unsigned char Palette[2] = { TGI_COLOR_WHITE, TGI_COLOR_BLACK };

int main (void) {
        tgi_install(tgi_static_stddrv);
        tgi_init();

        maxX = tgi_getmaxx();
        maxY = tgi_getmaxy();

        
        tgi_setpalette(Palette);
        tgi_setcolor(TGI_COLOR_RED);
        tgi_clear();

        for (X = 0; X <= maxY; X += 10) {
                tgi_line (0, 0, maxY, X);
                tgi_line (0, 0, X, maxY);
                tgi_line (maxY, maxY, 0, maxY-X);
                tgi_line (maxY, maxY, maxY-X, 0);
        }

        tgi_uninstall();

        return EXIT_SUCCESS;
}

