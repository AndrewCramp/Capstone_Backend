#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <glob.h>
#include <errno.h>
#include <math.h>
#include <pthread.h>
#include <time.h>

#define MAX_X 27940
#define MAX_Y 21590

int plotterXPos;
int plotterYPos;

char* findPlotter();
int init();
int getMotorStatus();
int lowerPen();
int raisePen();
int pixelToStep();
int drawImage();
int drawContour();
int movePen();
int serialWrite();
int serialRead();


typedef struct ContourNode{
    int* x_positions;
    int* y_positions;
    int size;
    int width;
    int height;
    int end;
    struct ContourNode* next;
}ContourNode;
