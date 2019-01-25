#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int *x = (int *) malloc(sizeof(int));
    *x = 1;
    printf("%d\n", *x);
    return 0;
}
