#ifndef __DECTOR__
#define __DECTOR__
#include "type.h"
typedef struct Point{
    U16 row;
    U16 col;
}Point;

float gaussian(float sigma, float r);
Mat* gaussian_kernel(U8 radius, float sigma);
List* Dog(Mat* image);
Mat* get_dog_kernel(U8 radius, float sigma1, float sigma2);
void plot_points(Mat* color_image, List* key_points);
#endif
