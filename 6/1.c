#define _GNU_SOURCE
#include <fcntl.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  // clang -o ./1.out ./1.c
  // sudo perf stat ./1.out

  // measure system call
  int fd = open("./1.txt", O_CREAT | O_RDONLY, S_IRWXU), nloops = 1000000;

  struct timeval start, end;
  gettimeofday(&start, NULL);
  for (size_t i = 0; i < nloops; i++) {
    read(fd, NULL, 0);
  }
  gettimeofday(&end, NULL);
  printf("system call: %f microseconds\n\n",
         (float)(end.tv_sec * 1000000 + end.tv_usec - start.tv_sec * 1000000 -
                 start.tv_usec) /
             nloops);
  close(fd);

  // measure context switch
  cpu_set_t set;
  CPU_ZERO(&set);
  CPU_SET(sched_getcpu(), &set);
  if (sched_setaffinity(0, sizeof(cpu_set_t), &set) == -1) {
    perror("sched_setaffinity");
    exit(EXIT_FAILURE);
  }

  int first_pipefd[2], second_pipefd[2];
  if (pipe(first_pipefd) == -1) {
    perror("pipe");
    exit(EXIT_FAILURE);
  }
  if (pipe(second_pipefd) == -1) {
    perror("pipe");
    exit(EXIT_FAILURE);
  }
  pid_t cpid = fork();
  if (cpid == -1) {
    perror("fork");
    exit(EXIT_FAILURE);
  } else if (cpid == 0) { // child
    char buf = '0';
    close(first_pipefd[1]);
    close(second_pipefd[0]);
    for (size_t i = 0; i < nloops; i++) {
      if (read(first_pipefd[0], &buf, 1) == -1) {
        perror("child read");
        _exit(EXIT_FAILURE);
      }
      if (write(second_pipefd[1], &buf, 1) == -1) {
        perror("child write");
        _exit(EXIT_FAILURE);
      }
    }
    close(first_pipefd[0]);
    close(second_pipefd[1]);
    _exit(EXIT_SUCCESS);
  } else { // parent
    gettimeofday(&start, NULL);
    char buf = '0';
    close(first_pipefd[0]);
    close(second_pipefd[1]);
    for (size_t i = 0; i < nloops; i++) {
      if (write(first_pipefd[1], &buf, 1) == -1) {
        perror("parent write");
        exit(EXIT_FAILURE);
      }
      if (read(second_pipefd[0], &buf, 1) == -1) {
        perror("parent read");
        exit(EXIT_FAILURE);
      }
    }
    gettimeofday(&end, NULL);
    printf("context switch: %f microseconds\n",
           (float)(end.tv_sec * 1000000 + end.tv_usec - start.tv_sec * 1000000 -
                   start.tv_usec) /
               nloops / 2);
    wait(NULL);
    close(first_pipefd[1]);
    close(second_pipefd[0]);
    exit(EXIT_SUCCESS);
  }
}
