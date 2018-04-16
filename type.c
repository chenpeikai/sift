#include "type.h"
//#include "stdlib.h"

List* init_List(size_t bytes){
    List* list = malloc(sizeof(List));
    list->bytes = bytes;
    list->start = NULL;
    list->end = NULL;
    list->length = 0;
}
/*add a item in the list*/
int push(List* list, void* data){
    Node* node = malloc(sizeof(Node));
    node->data = malloc(list->bytes);
    node->next = NULL;
    memcpy(node->data, data, list->bytes);
    if(list->start == NULL){
        list->start = node;
        list->end = node;
    }
    else{
        list->end->next = node;
        list->end = node;
    }
    list->length++;
    return 1;
}
