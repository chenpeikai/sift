#include <stdio.h>
#include "bmp.h"
#include "sift.h"
int main()
{
    Mat* image = read_bmp("3.bmp");
    /*kernal is
        1 1 1
        1 2 1
        1 1 1
    */
    Mat* patch = gaussian_kernal(1,1);

    Mat* image1 = uchar2float(image);

    Mat* new_img = conv(image1, patch, 1, 0);
    mat_abs(new_img);
    //Mat* new_img = expand_image(image1, 100);
    //Mat* sub_img = mat_sub(new_img, image);
    //Mat* new_img = down_sample(image,3);

    Mat* image2 = float2uchar(new_img);
    write_bmp(image2, "4.bmp");
    //printf("%f",gaussian(1,4));
    return 0;
}
