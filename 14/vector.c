#include <stdio.h>
#include <stdlib.h>
#include "vector.h"

int main(int argc, char *argv[]) {
    struct vector v = {.data = (int *)malloc(sizeof(int)), .size = 1, .capacity = 1};
    v.data[0] = 4;
    struct vector *vp = &v;
    vector_insertAtEnd(vp, 5);
    vector_insertAtEnd(vp, 6);
    vector_insertAtEnd(vp, 7);
    vector_deleteAtEnd(vp);

    printf("first value: %d\n", v.data[0]);
    printf("second value: %d\n", v.data[1]);
    printf("third value: %d\n", v.data[2]);
    printf("size: %d\n", v.size);
    printf("capacity: %d\n", v.capacity);

    vector_free(vp);

    return 0;
}
