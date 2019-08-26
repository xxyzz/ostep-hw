#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int
main(int argc, char *argv[]) {
    int x = 100;
    int rc = fork();
    if (rc < 0) {
        // fork failed; exit
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {
        x = 101;
        printf("x in child process: %d\n", x);
    } else {
        x = 102;
        printf("x in parent process: %d\n", x);
    }
    return 0;
}
