#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int *data = (int *) malloc(100);
    // free(data);
    free((int *)data[1]);
    return 0;
}
