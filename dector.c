#include "mat.h"
#include <math.h>
#include "dector.h"
#include <stdio.h>
#include <stdbool.h>
#include "bmp.h"

Point* init_point(U16 row, U16 col){
    Point* point = malloc(sizeof(Point));
    point->col = col;
    point->row = row;
}

float gaussian(float sigma, float r){
   return exp(-pow(r,2) / (2*pow(sigma,2))) / (2*PI*sigma);
}

Mat* gaussian_kernel(U8 radius, float sigma){
    U16 height = 2*radius + 1;
    U16 width = 2*radius + 1;
    Mat* kernel = init_mat(height, width, 0,Float);
    float* pointer;

    for(int row = 0; row<height;row++){
        for(int col = 0; col < width; col++){
            pointer = locate(kernel, row, col);
            float r = sqrt(pow(row - radius,2) + pow(col - radius,2))/radius;
            *pointer = gaussian(sigma, r);
        }
    }
    return kernel;
}
/*check if this point is extreme point*/
bool check_extreme(Mat* scala_space[],U8 level,U16 row,U16 col){
    if(level == 0 || level == 3){
        fprintf(stderr,"check_max level error");
        return false;
    }
    float threshold = 0.8;
    U8 is_extreme = true;

    float* origin = locate(scala_space[level], row, col);
    /*check maxmum*/
    for(U8 k = level -1;k < level + 2; k++){
        for(U16 i = row - 1;i < row + 2;i++){
            for(U16 j = col - 1;j < col + 2;j++){
                if(i == row && j == col && k == level)
                    continue;
                float* compare = locate(scala_space[k], i, j);
                if(*compare > *origin - threshold)
                    //return false;
                    is_extreme = false;
            }
        }
        if(is_extreme == false)
            break;
    }
    if(is_extreme == false)
        is_extreme = true;
    else
        return is_extreme;
    /*check minmum*/
    for(U8 k = level -1;k < level + 2; k++){
        for(U16 i = row - 1;i < row + 2;i++){
            for(U16 j = col - 1;j < col + 2;j++){
                if(i == row && j == col && k == level)
                    continue;
                float* compare = locate(scala_space[k], i, j);
                if(*compare < *origin + threshold)
                    is_extreme = false;
            }
        }
        if(is_extreme == false)
            break;
    }

    return is_extreme;
}

List* local_max(Mat** scala_space){
    U16 length = 4;
    List* key_points = init_List(sizeof(Point));
    U16 height = scala_space[0]->height;
    U16 width = scala_space[0]->width;
    for(U8 k = 1; k < length - 1; k++){
        for(U16 row = 4; row< height-4; row++){
            for(U16 col=4; col < width-4; col++){
                if(check_extreme(scala_space, k,row,col))
                    push(key_points,init_point(row,col));
            }
        }
    }
    return key_points;
}

Mat* get_dog_kernel(U8 radius, float sigma1, float sigma2){
    Mat* kernel = image_sub(gaussian_kernel(radius,sigma1),
                     gaussian_kernel(radius,sigma2));
    float sum = 0;
    for(U16 row = 0; row < 2*radius + 1; row++){
        for(U16 col=0; col < 2*radius + 1; col++){
            float* pointer = locate(kernel, row, col);
            sum = sum + *pointer;
        }
    }
    return kernel;
}

List* Dog(Mat* image){
    float sigma = 0.3;
    int scale = 4;
    U8 radius = 2;

    Mat** scale_space = malloc(sizeof(Mat*));
    for(int k = 1; k <scale+1; k++){
        Mat* dog_kernel = get_dog_kernel(radius,sigma*2,sigma);
        sigma = sigma*2;
        print_mat(dog_kernel);
        scale_space[k-1] = conv(image, dog_kernel, 1, dog_kernel->height/2); // keep the scale is the same as the origin image
    }
    List* key_point = local_max(scale_space);


    for(int i = 0; i < 4; i++){
        normalize_image(scale_space[i]);
        Mat* img = float2uchar(scale_space[i]);
        char path[20];
        sprintf(path,"level-%d.bmp",(i+1));
        write_bmp(img, path);
    }
    return key_point;
}

void plot_points(Mat* color_image, List* key_points){
    if(color_image->channels != 3){
        return;
    }
    RGB red;
    red.G = 255;
    U16 height = color_image->height;
    U16 width = color_image->width;

    for(Node* pointer = key_points->start; pointer != NULL; pointer = pointer->next){
        Point* point = pointer->data;
        RGB* to = locate(color_image, point->row, point->col);
        if(point->row > 1 && point->row < height- 2
                && point->col > 1&& point->col < width -2){
            *to = red;
            *(to - 1) = red;
            *(to + 1) = red;
            *(to - 2) = red;
            *(to + 2) = red;
            *(to + width) = red;
            *(to - width) = red;
            *(to + width*2) = red;
            *(to - width*2) = red;
        }
    }
}
