#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int *x = (int *) calloc(1, sizeof(int));
    x = NULL;
    printf("%d\n", *x);
    free(x);
    return 0;
}
