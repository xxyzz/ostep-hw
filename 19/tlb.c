#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sched.h>

int main(int argc, char *argv[]) {
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(0, &set);

    if (sched_setaffinity(getpid(), sizeof(cpu_set_t), &set) == -1) {
        printf("Set CPU affinity error\n");
        exit(1);
    }

    if (argc != 3) {
        printf("Need the number of pages and the number of trials\n");
        exit(1);
    }

    int PAGESIZE = getpagesize();
    int jump = PAGESIZE / sizeof(int);
    int NUMPAGES = atoi(argv[1]);
    int count = atoi(argv[2]);
    if (NUMPAGES <= 0 || count <= 0) {
        printf("Invalid input\n");
        exit(1);
    }
    int *a = (int *) calloc(NUMPAGES * jump, sizeof(int));
    struct timeval start, end;
    int startReturn = gettimeofday(&start, NULL);
    for(size_t j = 0; j < count; j++) {
        for (int i = 0; i < NUMPAGES * jump; i += jump) {
            a[i] += 1; 
        }
    }
    int endReturn = gettimeofday(&end, NULL);
    if (startReturn == -1 || endReturn == -1) {
        printf("Gettimeofday() error");
        exit(1);
    }

    int nloops = count * NUMPAGES;
    // nanoseconds
    printf("%f\n", (float) (end.tv_sec * 1000000 + end.tv_usec - start.tv_sec * 1000000 - start.tv_usec) * 1000 / nloops);
    free(a);
    return 0;
}
