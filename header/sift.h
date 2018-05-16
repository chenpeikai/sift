#ifndef __SIFT__
#define __SIFT__
#include "dector.h"
typedef float* SiftVector;
typedef struct Pair{
   Point p1;
   Point p2;
}Pair;

Mat* horizon_diff(Mat* image);
Mat* vertical_diff(Mat* image);
Mat* gradient_abs(Mat* image);
Mat* gradient_direction(Mat* image);
List* sift_describe(Mat* image, List* key_points);
#endif
