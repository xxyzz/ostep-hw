#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

int main(int argc, char *argv[]) {
    int PAGESIZE = 4;
    int jump = PAGESIZE / sizeof(int);
    if (argc != 3) {
        printf("Need the number of pages and the number of trials\n");
        exit(1);
    }
    int NUMPAGES = atoi(argv[1]);
    int count = atoi(argv[2]);
    if (NUMPAGES <= 0 || count <= 0) {
        printf("Invalid input\n");
        exit(1);
    }
    int a[NUMPAGES * jump];
    struct timeval start, end;
    gettimeofday(&start, NULL);
    for(size_t j = 0; j < count; j++) {
        for (int i = 0; i < NUMPAGES * jump; i += jump) {
            a[i] += 1; 
        }
    }
    gettimeofday(&end, NULL);
    int nloops = count * NUMPAGES * jump;
    printf("The cost of accessing each page: %f microseconds\n", (float) (end.tv_sec * 1000000 + end.tv_usec - start.tv_sec * 1000000 - start.tv_usec) / nloops);
    return 0;
}
