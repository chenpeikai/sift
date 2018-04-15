#include "mat.h"
#include <math.h>
#include "sift.h"
#define PI 3.14
float gaussian(float sigma, float r){
   return exp(-pow(r,2) / (2*pow(sigma,2))) / (sqrt(2*PI)*sigma);
}

Mat* gaussian_kernal(U8 radius, float sigma){
    U16 height = 2*radius + 1;
    U16 width = 2*radius + 1;
    Mat* kernal = init_mat(height, width, 0,Float);
    float* pointer;

    for(int row = 0; row<height;row++){
        for(int col = 0; col < width; col++){
            pointer = locate(kernal, row, col);
            float r = sqrt(pow(row - radius,2) + pow(col - radius,2));
            *pointer = gaussian(sigma, r);
        }
    }
    /*Normalized the data*/
    float sum = 0;
    for(int row = 0; row<height;row++){
        for(int col = 0; col < width; col++){
            pointer = locate(kernal, row, col);
            sum = sum + *pointer;
        }
    }

    for(int row = 0; row<height;row++){
        for(int col = 0; col < width; col++){
            pointer = locate(kernal, row, col);
            *pointer = *pointer/sum;
        }
    }
    return kernal;
}
