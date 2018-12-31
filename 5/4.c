#define _GNU_SOURCE    // for execvpe()
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int
main(int argc, char const *argv[]) {
    int rc = fork();

    if (rc < 0) {
        // fork failed; exit
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {
        char * const argv[] = {"ls", "-l", "-a", "-h", NULL};
        char * const envp[] = {NULL};

        execl("/bin/ls", "ls", "-l", "-a", "-h", NULL);
        execlp("ls", "ls", "-l", "-a", "-h", NULL);
        execle("/bin/ls", "ls", "-l", "-a", "-h", NULL, envp);

        execv("/bin/ls", argv);
        execvp("ls", argv);
        execvpe("ls", argv, envp);    // may not work
    }
    return 0;
}
