#ifndef __BMP__
#define __BMP__
#include "mat.h"
Mat* read_bmp(char* path);
int write_bmp(Mat* image, char* path);

typedef struct RGBQUAD{
    U8 R;
    U8 G;
    U8 B;
    U8 reserver;
}RGBQUAD;

#endif
