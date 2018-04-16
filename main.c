#include <stdio.h>
#include "bmp.h"
#include "sift.h"
int main()
{
    //Mat* image = read_bmp("3.bmp");
    Mat* color_image = read_bmp("1.bmp");
    Mat* gray_image = RGB2Gray(color_image);
    /*kernel is
        1 1 1
        1 2 1
        1 1 1
    */
    Mat* patch = gaussian_kernel(1,1);

    Mat* image1 = uchar2float(gray_image);


    //print_mat(gaussian_kernel(1,0.2));
    //print_mat(gaussian_kernel(1,0.8));
    //print_mat(image_sub(gaussian_kernel(1,0.2), gaussian_kernel(1,0.8)));
    //print_mat(get_dog_kernel(1,3,4));
    List* key_points = Dog(image1);
    //List* key_points = init_List(2);

    plot_points(color_image, key_points);
    write_bmp(color_image,"point.bmp");
    return 0;
}
