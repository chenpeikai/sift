#ifndef __MAT__
#define __MAT__
#include "type.h"
typedef struct RGB{
   U8 R;
   U8 G;
   U8 B;
}RGB;

typedef enum Byte{ Uchar = 1,Float = 4,Rgb = 3}Byte;

typedef struct Mat{
    void* buffer; //buffer to save the image
    U16 height;
    U16 width;
    U8 channels;
    Byte bytes;
} Mat;

void free_mat(Mat* image);
Mat* init_mat(U16 height, U16 width, U8 value, enum Byte byte);
Mat* down_sample(Mat* image, U8 stride);
Mat* RGB2Gray(Mat* image);
Mat* expand_image(Mat* image, U8 padding);
Mat* conv(Mat* image, Mat* patch, U8 stride, U8 padding);
float dot(Mat* image, Mat* patch,U16 row, U16 col);
Mat* image_sub(Mat* mat1, Mat* mat2);
void* locate(Mat* image, U16 row, U16 col);
Mat* uchar2float(Mat* uchar_image);
Mat* float2uchar(Mat* float_image);
void mat_abs(Mat* float_image);
void normalize_image(Mat* image);
void print_mat(Mat* mat);
#endif
