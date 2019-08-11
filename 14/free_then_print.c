#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int *data = (int *) malloc(100);
    // free(data);
    free(&data[1]);
    // printf("%d\n", data[0]);
    return 0;
}
