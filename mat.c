#include <stdlib.h>
#include <stdio.h>
#include "mat.h"
#include "bmp.h"
#include <math.h>
/*only process one channels*/

void free_mat(Mat* image){
    free(image->buffer);
    free(image);
}
void* locate(Mat* image, U16 row, U16 col){
    void* res = (U8*)image->buffer + (row*image->width + col)*image->bytes;
    return res;
}

Mat* init_mat(U16 height, U16 width, U8 value, enum Byte byte){
    Mat* new_image = malloc(sizeof(Mat));
    if(byte == Rgb)
        new_image->channels = 3;
    else
        new_image->channels = 1;
    new_image->height = height;
    new_image->width = width;
    new_image->bytes = byte;
    new_image->buffer = malloc(height*width*byte);
    memset(new_image->buffer, value, height*width*byte);
    return new_image;
}

Mat* RGB2Gray(Mat* image){
    if(image->channels != 3){
        fprintf(stderr,"channels isn't correct");
        return NULL;
    }
    Mat* gray = init_mat(image->height, image->width, 0,Uchar);
    U8* gray_pointer = gray->buffer;
    RGB* color_pointer = (RGB*)image->buffer;

    for(int i=0; i<gray->height*gray->width; i++){
        *gray_pointer = (color_pointer->B + color_pointer->G + color_pointer->R)/3;
        gray_pointer++;
        color_pointer++;
    }

    return gray;
}
/*padding image with zeros*/
Mat* expand_image(Mat* image, U8 padding){
    U16 height = image->height + 2*padding;
    U16 width = image->width + 2*padding;
    Mat* new_image;
    if(image->bytes == Uchar){
        new_image = init_mat(height, width,0, Uchar);
        U8* to;
        for(int row = 0; row<image->height; row++){
            for(int col = 0; col < image->width; col++){
                to = (float*)locate(new_image, row+padding, col+padding);
                U8* from = (U8*)locate(image, row, col);
                *to = *from;
            }
        }
    }
    else if(image->bytes == Float){
        new_image = init_mat(height, width,0, Float);
        float* to;
        float* from;
        for(int row = 0; row<image->height; row++){
            for(int col = 0; col < image->width; col++){
                to = (float*)locate(new_image, row+padding, col+padding);
                from = (float*)locate(image, row, col);
                //*to = *from;
                *to = *from;
            }
        }
    }

    return new_image;
}


float dot(Mat* image, Mat* patch,U16 row, U16 col){

    float* src1;
    float* src2;
    float dot_sum = 0;
    U8 kernal = patch->height;

    for(int i=0; i< patch->height; i++){
        for(int j=0;j<patch->width;j++){
            src1 = locate(image,row - kernal/2 + i,col- kernal/2 + j);
            src2 = locate(patch,i,j);
            dot_sum = dot_sum + (*src1)*(*src2);
        }
    }
    return dot_sum;
}
/*convolution of image and patch(same height and width odd)*/
Mat* conv(Mat* image, Mat* patch, U8 stride, U8 padding){
    if(image->channels != 1){
        return NULL;
    }
    U8 kernal_size = patch->height;
    if(kernal_size%2 == 0){
        return NULL;
    }
    if(patch->bytes != Float || image->bytes != Float){
        fprintf(stderr, "patch or Mat must be float\n");
        return NULL;
    }
    //define the size of the image after convolution

    U16 height = (image->height - kernal_size + 2*padding)/stride + 1;
    U16 width = (image->width - kernal_size + 2*padding)/stride + 1;
    Mat* new_image = init_mat(height, width, 0, Float);

    //expand the image
    Mat* big_image = expand_image(image, padding);

    int col = 0;
    float* pointer = new_image->buffer;
    for(int row=kernal_size/2;row < big_image->height - kernal_size/2; row = row + stride){
        for(col = kernal_size/2; col < big_image->width - kernal_size/2; col = col + stride){
            float res = dot(big_image, patch, row,col);
            *pointer = res;
            pointer++;
        }
    }
    free_mat(big_image);
    return new_image;
}
/*Downsampling a Image*/
Mat* down_sample(Mat* image, U8 stride){
    Mat* patch = init_mat(1,1,1,Float);

    return conv(image,patch,stride, 0);
}
/*Subtraction of two image*/
Mat* image_sub(Mat* mat1, Mat* mat2){
    if((mat1->height != mat2->height) || (mat1->width != mat2->width)){
        fprintf(stderr,"image size must be same");
        return NULL;
    }
    if(mat1->bytes != mat2->bytes){
        fprintf(stderr,"image type must be same");
        return NULL;
    }

    Mat* new_image = init_mat(mat1->height, mat1->width, 0, mat1->bytes);
    if(mat1->bytes == 1){
        for(int row=0; row< mat1->height; row++){
            for(int col=0; col<mat1->width; col++){
                U8* sub1 = locate(mat1, row, col);
                U8* sub2 = locate(mat2, row, col);
                U8* to = locate(new_image, row, col);
                *to = *sub1 - *sub2;
            }
        }
    }
    else if(mat1->bytes == 4){
        for(int row=0; row< mat1->height; row++){
            for(int col=0; col<mat1->width; col++){
                float* sub1 = locate(mat1, row, col);
                float* sub2 = locate(mat2, row, col);
                float* to = locate(new_image, row, col);
                *to = *sub1 - *sub2;
            }
        }
    }
    return new_image;
}

Mat* uchar2float(Mat* uchar_image){
    if(uchar_image->bytes != Uchar){
        fprintf(stderr,"image must be uchar");
        return NULL;
    }
    Mat* float_image = init_mat(uchar_image->height, uchar_image->width, 0, Float);
    U16 height = uchar_image->height;
    U16 width = uchar_image->width;

    float* to = (float*)float_image->buffer;
    U8* from = (U8*)uchar_image->buffer;
    for(U16 i=0;i<height; i++){
        for(U16 j=0; j< width;j++){
            to = locate(float_image,i,j);
            from = locate(uchar_image,i,j);

            *to = *from;
            //to++;
            //from++;
            //printf("%d\n",to);
        }
    }
    return float_image;
}

Mat* float2uchar(Mat* float_image){
    if(float_image->bytes != Float){
        fprintf(stderr,"image must be float");
        return NULL;
    }
    Mat* uchar_image = init_mat(float_image->height, float_image->width, 200, Uchar);
    U16 height = uchar_image->height;
    U16 width = uchar_image->width;

    U8* to;
    float* from;
    for(int i=0;i<height; i++){
        for(int j=0; j< width;j++){
            to = locate(uchar_image,i,j);
            from = locate(float_image,i,j);
            *to = MAX(MIN(*from,255),0);
        }
    }
    return uchar_image;
}

void mat_abs(Mat* float_image){
    U16 height = float_image->height;
    U16 width = float_image->width;
    for(int row=0;row<height; row++){
        for(int col=0; col< width;col++){
            float* pointer = locate(float_image,row,col);
            *pointer = ABS(*pointer);
        }
    }
}

void normalize_image(Mat* image){
    U16 height = image->height;
    U16 width = image->width;
    float max = -5000;
    float min = 5000;
    for(int row=0;row<height; row++){
        for(int col=0; col< width;col++){
            float* pointer = locate(image,row,col);

            if(*pointer > max)
                max = *pointer;
            else if(*pointer < min)
                min = *pointer;
        }
    }

    for(int row=0;row<height; row++){
        for(int col=0; col< width;col++){
            float* pointer = locate(image,row,col);
            *pointer = 255*(*pointer - min)/(max - min);
        }
    }
}

void print_mat(Mat* mat){
    U16 height = mat->height;
    U16 width = mat->width;
 printf("\n\n");
    for(int row=0;row<height; row++){
        for(int col=0; col< width;col++){
            float* pointer = locate(mat,row,col);
            printf("%f \t",*pointer);
        }
        printf("\n");
    }
}
