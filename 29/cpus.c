#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    printf("Number of cpus: %ld", sysconf(_SC_NPROCESSORS_ONLN));
    return 0;
}
