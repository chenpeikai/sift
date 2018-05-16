#ifndef __TYPE__
#define __TYPE__
#include <stdlib.h>
typedef  unsigned char  U8 ;
typedef  unsigned short U16 ;
typedef  unsigned int   U32 ;
#define MAX(a,b)  (((a)>(b))?(a):(b))
#define MIN(a,b)  (((a)>(b))?(b):(a))
#define ABS(a)    MAX(a,0)
#define PI 3.14



typedef struct Node{
    struct Node* next;
    void* data;
}Node;

typedef struct List{
    struct Node* start;
    struct Node* end;
    U16 length;
    size_t bytes;
}List;
List* init_List(size_t bytes);
int push(List* list, void* data);

#endif
