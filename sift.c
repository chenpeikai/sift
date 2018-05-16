#include "mat.h"
#include "type.h"
#include "stdio.h"
#include "math.h"
#include "sift.h"
#include "dector.h"
#include "bmp.h"
#include <stdlib.h>

Mat* vertical_diff(Mat* image){
    if(image->channels != 1 || image->bytes != Float){
        fprintf(stderr,"channels or bytes isn't correct");
    }

    Mat* kernel = init_mat(3,3,0,Float);
    float* pointer = kernel->buffer;
    pointer[0] = -1;pointer[1] = -2;pointer[2] = -1;pointer[6] = 1;pointer[7] = 2;pointer[8] = 1;

    Mat* res = conv(image, kernel, 1, 1);
    return res;
}

Mat* horizon_diff(Mat* image){
    if(image->channels != 1 || image->bytes != Float){
        fprintf(stderr,"channels or bytes isn't correct");
    }

    Mat* kernel = init_mat(3,3,0,Float);
    float* pointer = kernel->buffer;
    pointer[0] = -1;pointer[3] = -2;pointer[6] = -1;pointer[2] = 1;pointer[5] = 2;pointer[8] = 1;

    Mat* res = conv(image, kernel, 1, 1);
    return res;
}

Mat* gradient_abs(Mat* image){
    if(image->channels != 1 || image->bytes != Float){
        fprintf(stderr,"channels or bytes isn't correct");
    }
    Mat* h_diff = horizon_diff(image);
    Mat* v_diff = vertical_diff(image);
    U16 height = image->height;
    U16 width = image->width;
    Mat* abs_diff = init_mat(height, width, 0, Float);

    for(U16 row=0;row<height;row++){
        for(U16 col=0;col<width;col++){
            float* src1 = locate(h_diff, row, col);
            float* src2 = locate(v_diff, row, col);
            float* to = locate(abs_diff, row, col);

            *to = sqrt(pow(*src1, 2) + pow(*src2, 2));
        }
    }
    return abs_diff;
}

Mat* gradient_direction(Mat* image){
    if(image->channels != 1 || image->bytes != Float){
        fprintf(stderr,"channels or bytes isn't correct");
    }
    Mat* h_diff = horizon_diff(image);
    Mat* v_diff = vertical_diff(image);
    U16 height = image->height;
    U16 width = image->width;
    Mat* abs_diff = init_mat(height, width, 0, Float);

    for(U16 row=0;row<height;row++){
        for(U16 col=0;col<width;col++){
            float* src1 = locate(h_diff, row, col);
            float* src2 = locate(v_diff, row, col);
            float* to = locate(abs_diff, row, col);

            *to = atan(*src2/(*src1));
            if((*src2) < 0 && (*src1) < 0)
                *to = *to + PI;
            else if(*src1 < 0 ||*src1 < 0 ){
                *to = *to + PI;
            }
        }
    }
    return abs_diff;
}
float main_direction(){
    return 0;
}

void count_small(Mat* gradient, Mat* abs,
                 U16 row, U16 col, U8 k, SiftVector sift){
    for(U16 i = 0; i < 2; i ++ ){
        for(U16 j = 0;j < 2;j ++ ){
            float* src1 = locate(gradient, i,j);
            U8 index = (*src1)/(PI/4);
            index = index + k*8;
            float* src2= locate(abs, i,j);
            sift[index] = *src2;
        }
    }
}

SiftVector sift_point(Mat* gradient, Mat* abs, U16 row, U16 col){
    if(row < 4 || row > abs->height - 4 || col < 4 || col > abs->width - 4){
        fprintf(stderr,"error sift");
        exit(0);
        return NULL;
    }
    SiftVector sift = malloc(128*sizeof(float));
    U8 k = 0;
    for(U16 i = row - 3 ; i < row + 5 ; i = i+ 2){
        for(U16 j = col - 3; j< col + 5; j = j + 2){
            count_small(gradient, abs, i,j,k,sift);
            k++;
        }
    }
    return sift;
}

List* sift_describe(Mat* image, List* key_points){
    static num = 1;
    List* sift_list = init_List(sizeof(SiftVector));
    Mat* abs_diff = gradient_abs(image);
    Mat* direction_diff= gradient_direction(image);

    for(Node* node = key_points;node != NULL; node = node->next){
        Point* point = node->data;
        U16 row = point->row;
        U16 col = point->col;
        if(row < 4 || row > abs_diff->height - 4 || col < 4
                || col > abs_diff->width - 4)
            continue;
        SiftVector sift = sift_point(direction_diff, abs_diff, row,col);
        push(sift_list, sift);
    }
    num++;

    normalize_image(abs_diff);
    Mat* img = float2uchar(abs_diff);
    char path[20];
    sprintf(path,"diff_abs%d.bmp",num);
    write_bmp(img,path);
    free_mat(img);

    normalize_image(direction_diff);
    img = float2uchar(direction_diff);
    sprintf(path,"direction_diff%d.bmp",num);
    write_bmp(img,path);
    free_mat(img);
    free_mat(abs_diff);
    free_mat(direction_diff);

    return sift_list;
}

float vector_dot(SiftVector sift1, SiftVector sift2){
    float dot_sum = 0;
    float sum1 = 0;
    float sum2 = 0;
    for(U8 i=0;i<128;i++){
        dot_sum = dot_sum + sift1[i]*sift2[i];
        sum1 = sum1 + pow(sift1[i],2);
        sum2 = sum2 + pow(sift2[i],2);
    }

    return dot_sum/(sqrt(sum1)*sqrt(sum2));
}

List* match(List* key_points1, List* key_points2,
            List* sift_list1, List* sift_list2){
    List* match_list = init_List(sizeof(Pair));
    Node* l1 = sift_list1->start;
    Node* l2 = sift_list2->start;
    float max = -2;
    //Point* max_point;
    for(Node* p_node1 = key_points1->start; p_node1 != NULL; p_node1=p_node1->next){
        Pair* match_pair;
        for(Node* p_node2 = key_points2->start; p_node2 != NULL; p_node2=p_node2->next){
            float res = vector_dot(l1->data, l2->data);
            match_pair = malloc(sizeof(Pair));
            if(res > max){
                match_pair->p1 = *((Point*)p_node1->data);
                match_pair->p2 = *((Point*)p_node2->data);
            }
            l2 = l2->next;

        }
        push(match_list,match_pair);
        l1 = l1->next;
    }
    return match_list;
}
