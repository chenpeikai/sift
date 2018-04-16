#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include "bmp.h"
#include "mat.h"


typedef struct BmpHeader
{
    //bmp header
    //U8  Signatue[2] ;   // B  M
    U32 FileSize;     //size of the file
    U16 Reserv1 ;
    U16 Reserv2 ;
    U32 FileOffset ;   //place where the image begin

    //DIB header
    U32 DIBHeaderSize ; //size of the DIB header
    U32 ImageWidth   ;  //width of the image
    U32 ImageHight   ;  //height of the image
    U16 Planes       ;
    U16 BPP          ;  //the bits of every Pixels
    U32 Compression  ;
    U32 ImageSize    ;  //size of the image
    U32 XPPM ;
    U32 YPPM ;
    U32 CCT ;
    U32 ICC ;
}BmpHeader;

void print_header(BmpHeader header){
    printf(" FileSize         : %d  \n " , header.FileSize     );
    printf(" Reserv1          : %d  \n " , header.Reserv1      );
    printf(" Reserv2          : %d  \n " , header.Reserv2      );
    printf(" FileOffset       : %d  \n " , header.FileOffset   );
    printf(" DIBHeaderSize    : %d  \n " , header.DIBHeaderSize);
    printf(" ImageWidth       : %d  \n " , header.ImageWidth   );
    printf(" ImageHight       : %d  \n " , header.ImageHight   );
    printf(" Planes           : %d  \n " , header.Planes       );
    printf(" BPP              : %d  \n " , header.BPP          );
    printf(" Compression      : %d  \n " , header.Compression  );
    printf(" ImageSize        : %d  \n " , header.ImageSize    );
    printf(" XPPM             : %d  \n " , header.XPPM         );
    printf(" YPPM             : %d  \n " , header.YPPM         );
    printf(" CCT              : %d  \n " , header.CCT          );
    printf(" ICC              : %d  \n " , header.ICC          );
    printf(" \n\n ");
}

Mat* read_bmp(char* path){
    FILE* img_stream = fopen(path, "r");
    if(img_stream == NULL){
        fprintf(stderr, "open image filed");
    }
    struct BmpHeader header;

    fseek(img_stream, 2, SEEK_SET); //skip BM
    fread(&header, sizeof(struct BmpHeader), 1, img_stream); // read the image header
    //print_header(header);


    U16 height = header.ImageHight;
    U16 width = header.ImageWidth;
    U8 bytes = header.BPP / 8;
    Mat* img = init_mat(height, width, 0, bytes);

    fseek(img_stream, header.FileOffset, SEEK_SET);
    fread(img->buffer, sizeof(U8), header.ImageSize, img_stream);


    fclose(img_stream);

    return img;

}
/*the byte of one line must be multiple of 4*/
Mat* four_byte(Mat* image){
    if(image->channels == 1){
        if(image->width %4 != 0){
            U16 width = (image->width + 3)/4*4;
            U16 height = image->height;
            Mat* new_image = init_mat(height, width, 0, image->bytes);
            U8* old_pointer = image->buffer;

            for(int row = 0; row<height; row++){
                for(int col = width - image->width; col < width; col++){
                    U8* to = locate(new_image, row, col);
                    *to = *old_pointer;
                    old_pointer++;
                }
            }
            free_mat(image);
            return new_image;
        }
        else{
            return image;
        }

    }
    else{
        if(image->width %4 != 0){
            U16 width = (image->width + 3)/4*4;
            U16 height = image->height;
            Mat* new_image = init_mat(height, width, 0, image->bytes);
            RGB* old_pointer = image->buffer;

            for(int row = 0; row<height; row++){
                for(int col = width - image->width; col < width; col++){
                    RGB* to = locate(new_image, row, col);
                    *to = *old_pointer;
                    old_pointer++;
                }
            }
            free_mat(image);
            return new_image;
        }
        else{
            return image;
        }
    }
}

int write_bmp(Mat* image, char* path){
    FILE* img_stream = fopen(path, "wb");
    BmpHeader header;
    char BM[2] = "BM";

    header.BPP = 8*image->channels;
    header.CCT = 0;
    header.Compression = 0;
    header.DIBHeaderSize = 40;
    if(image->channels == 3){
        header.FileOffset = 54;
    }
    else{
        header.FileOffset = 1078;
    }
    image = four_byte(image);
    header.ImageHight = image->height;
    header.ImageWidth = image->width;
    header.ImageSize = (image->width)*(image->height)*(image->channels);
    header.FileSize = header.ImageSize + 54;
    header.ICC = 0;
    header.Planes = 1;
    header.Reserv1 = 0;
    header.Reserv2 = 0;
    header.XPPM = 0;
    header.YPPM = 0;

    //print_header(header);
    fwrite(BM,sizeof(U8),2, img_stream);
    fwrite(&header,sizeof(BmpHeader),1, img_stream);
    //for gray image we need to write the Palette
    if(image->channels != 3){
        RGBQUAD* palette = malloc(sizeof(RGBQUAD)*256);
        for(int i =0; i<256;i++){
            palette[i].B = i;
            palette[i].G = i;
            palette[i].R = i;
        }
        fwrite(palette, sizeof(RGBQUAD), 256,img_stream);
    }

    fwrite(image->buffer, sizeof(U8), header.ImageSize,img_stream);

    fclose(img_stream);

    return 0;
}
