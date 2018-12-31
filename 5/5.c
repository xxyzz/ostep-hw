#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int
main(int argc, char *argv[]) {
    int rc = fork();
    if (rc < 0) {
        // fork failed; exit
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {
       pid_t wait_return = wait(NULL);
       printf("child wait return: %d\n", wait_return);
    } else {
       pid_t wait_return = wait(NULL);
       printf("parent wait return: %d\n", wait_return);
    }
    return 0;
}
