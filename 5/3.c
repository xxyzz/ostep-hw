#include <signal.h> // kill
#include <stdio.h>
#include <stdlib.h> // exit
#include <unistd.h> // getpid, fork, pause, pipe, getopt

static void wait_with_signal() {
  printf("wait with signal\n");
  pid_t cpid = fork();
  if (cpid < 0) {
    perror("fork failed\n");
    exit(EXIT_FAILURE);
  } else if (cpid == 0) {
    printf("hello\n");
    kill(getpid(), SIGCONT);
  } else {
    pause();
    printf("goodbye\n");
  }
}

static void wait_with_pipe() {
  printf("wait with pipe\n");
  int pipefd[2];
  if (pipe(pipefd) == -1) {
    perror("pipe failed\n");
    exit(EXIT_FAILURE);
  }
  pid_t cpid = fork();
  if (cpid < 0) {
    perror("fork failed\n");
    exit(EXIT_FAILURE);
  } else if (cpid == 0) {
    printf("hello\n");
    close(pipefd[0]);
    write(pipefd[1], "c", 1);
    close(pipefd[1]);
  } else {
    close(pipefd[1]);
    char c;
    read(pipefd[0], &c, 1); // block
    close(pipefd[0]);
    printf("goodbye\n");
  }
}

static void usage(char *name) {
  fprintf(stderr, "Usage: %s [-s] [-p]\n", name);
  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
  int opt;
  if ((opt = getopt(argc, argv, "sp")) != -1) {
    switch (opt) {
    case 's':
      wait_with_signal();
      break;
    case 'p':
      wait_with_pipe();
      break;
    default:
      usage(argv[0]);
    }
  } else
    usage(argv[0]);

  return 0;
}
