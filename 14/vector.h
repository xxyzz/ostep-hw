#include <stdio.h>
#include <stdlib.h>

struct vector
{
    int * data;
    int size;
    int capacity;
};

void vector_insertAtEnd(struct vector * v, int value) {
    if (v->size == v->capacity) {
        v->capacity = v->capacity * 2;
        v->data = (int *) realloc(v->data, v->capacity * sizeof(int));
    }
    (v->data)[v->size++] = value;
}

void vector_deleteAtEnd(struct vector * v) {
    (v->data)[--(v->size)] = 0;
    if (v->size == (int)(v->capacity / 4)) {
        v->capacity = (int)(v->capacity / 2);
        v->data = (int *) realloc(v->data, v->capacity * sizeof(int));
    }
}

void vector_free(struct vector * v) {
    free(v->data);
    v->size = 0;
    v->capacity = 0;
}
