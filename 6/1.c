#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sched.h>
#include <sys/wait.h>
#include <string.h>

int
main(int argc, char *argv[]) {
    // measure system call
    int fd = open("./1.txt", O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU), nloops = 1000000;

    struct timeval start, end;
    gettimeofday(&start, NULL);
    for (size_t i = 0; i < nloops; i++) {
        read(fd, NULL, 0);
    }
    gettimeofday(&end, NULL);
    printf("system call: %f microseconds\n\n", (float) (end.tv_sec * 1000000 + end.tv_usec - start.tv_sec * 1000000 - start.tv_usec) / nloops);
    close(fd);

    // measure context switch
    cpu_set_t set;
    int first_pipefd[2], second_pipefd[2];
    char buf;
    char str[] = "Screw you guys, I'm going home!";
    nloops = 1000000;
    CPU_ZERO(&set);
    pid_t cpid = fork();

    if (cpid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (cpid == 0) {    // child
        CPU_SET(0, &set);
        if (sched_setaffinity(getpid(), sizeof(set), &set) == -1) {
            exit(EXIT_FAILURE);
        }
        
        if (nloops < 1) {
            exit(EXIT_SUCCESS);
        }
        printf("child loop: %d\n", nloops);
        nloops--;
        close(first_pipefd[1]);                               // close unused first pipe write end
        while (read(first_pipefd[0], &buf, 1) > 0) {
        }
        close(first_pipefd[0]);

        close(second_pipefd[0]);                              // close unused second pipe read end
        write(second_pipefd[1], str, strlen(str));
        close(second_pipefd[1]);
    } else {           // parent
        CPU_SET(0, &set);
        if (sched_setaffinity(getpid(), sizeof(set), &set) == -1) {
            exit(EXIT_FAILURE);
        }

        gettimeofday(&start, NULL);
        if (nloops > 0) {
            printf("parent loop: %d\n", nloops);
            nloops--;
            close(first_pipefd[0]);                              // close unused first pipe read end
            write(first_pipefd[1], str, strlen(str));
            close(first_pipefd[1]);

            waitpid(cpid, NULL, 0);
            close(second_pipefd[1]);                             // close unused second pipe write end
            while (read(second_pipefd[0], &buf, 1) > 0) {
            }
            close(second_pipefd[0]);
        }
        gettimeofday(&end, NULL);

        printf("context switch: %f microseconds\n", (float) (end.tv_sec * 1000000 + end.tv_usec - start.tv_sec * 1000000 - start.tv_usec) / nloops);
    }
    return 0;
}
