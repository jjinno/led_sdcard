#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>

#define LED_COUNT 8
#define COLOR_RGB 3

#define RED 0
#define GREEN 1
#define BLUE 2

// R=Right, L=Left, B=Bottom, T=Top, Z=Zigzag, C=Continuous
#define RLTBZ 0
#define RLBTZ 1
#define LRTBZ 2
#define LRBTZ 3
#define TBRLZ 4
#define TBLRZ 5
#define BTRLZ 6
#define BTLRZ 7
#define RLTBC 8
#define RLBTC 9
#define LRTBC 10
#define LRBTC 11
#define TBRLC 12
#define TBLRC 13
#define BTRLC 14
#define BTLRC 15


int fd = 0;

int writeOrientation(int orientation)
{
    int retval = 0, c = 0;
    if (orientation < 0 || orientation > 15) return -1;
    c += retval = write(fd, (const void*) &orientation, 1);
    if (retval == 0) return -1;
    return c;
}

int writeRate(int rate)
{
    int retval = 0, c = 0;
    c += retval = write(fd, (const void*) &rate, 1);
    if (retval == 0) return -1;
    return c;
}

int writeDimensions(int x, int y)
{
    int retval = 0, c = 0;
    char xLow=0, xHigh=0;
    char yLow=0, yHigh=0;

    xLow = (x >> 0) & 255;
    xHigh = (x >> 8) & 255;

    yLow = (y >> 0) & 255;
    yHigh = (y >> 8) & 255;

    /* We write "rows,cols" ... aka: "y,x" instead of "x,y" ... */
    c += retval = write(fd, &yLow, 1);
    if (retval == 0) return -1;
    c += retval = write(fd, &yHigh, 1);
    if (retval == 0) return -1;
    c += retval = write(fd, &xLow, 1);
    if (retval == 0) return -1;
    c += retval = write(fd, &xHigh, 1);
    if (retval == 0) return -1;

    return c;
}

int writeColor(int r, int g, int b)
{
    int retval = 0, c = 0;
    c += retval = write(fd, (const void*) &r, 1);
    if (retval == 0) return -1;
    c += retval = write(fd, (const void*) &g, 1);
    if (retval == 0) return -1;
    c += retval = write(fd, (const void*) &b, 1);
    if (retval == 0) return -1;

    return c;
}

/*===========================================================================*/

int writeHeader(int orientation, int rate, int x, int y)
{
    int retval = 0, c = 0;
    c += retval = writeOrientation(orientation);
    if (retval < 0) return -1;
    c += retval = writeRate(rate);
    if (retval < 0) return -1;
    c += retval = writeDimensions(x,y);
    if (retval < 0) return -1;

    return c;
}

// Write an entire frame of LEDs (all of them)
int writeRandomFrame()
{
    int retval = 0, led = 0, c = 0;
    srand(time(NULL));
    for (led=0; led<LED_COUNT; led++) {
        c += retval = writeColor(rand()%256, rand()%256, rand()%256);
        if (retval < 0) return -1;
    }
    return c;
}

int writeClimbingFrames()
{
    int retval = 0, led = 0, on = 0, c = 0;
    for (on=0; on<LED_COUNT; on++) {
        for (led=0; led<LED_COUNT; led++) {
            if (led == on) {
                c += retval = writeColor(rand()%256, rand()%256, rand()%256);
                if (retval < 0) return -1;
            }
            else {
                c += retval = writeColor(0,0,0);
                if (retval < 0) return -1;
            }
        }
    }
    return c;
}

int main(int argc, char **argv)
{
    int retval = -1;
    fd = open("./output.led", O_CREAT|O_WRONLY );
    if (fd < 0) goto cleanReturn;

    retval = writeHeader(LRTBZ, 50, 1, 8);
    if (retval < 0) goto cleanReturn;
    printf("Wrote %d byte header\n", retval);

    retval = writeClimbingFrames();
    if (retval < 0) goto cleanReturn;
    printf("Wrote %d byte(s)\n", retval);

cleanReturn:
    close(fd);
    return 0;
}
