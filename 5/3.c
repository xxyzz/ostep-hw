#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void
sigcont_handler (int signum) {
    printf("goodbye\n");
}

int
main(int argc, char const *argv[]) {
    int parent_pid = getpid();
    int rc = fork();
    struct sigaction act;
    act.sa_handler = sigcont_handler;

    if (rc < 0) {
        // fork failed; exit
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {
        kill(parent_pid, SIGCONT);
        printf("hello\n");
    } else {
        sigaction(SIGCONT, &act, NULL);
        pause();
    }
    return 0;
}
