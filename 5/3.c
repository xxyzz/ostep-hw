#include <signal.h> // kill, sigaction
#include <stdio.h>
#include <stdlib.h> // exit
#include <unistd.h> // getppid, fork, pause, pipe
#include <getopt.h>

#define errExit(msg)                                                           \
  do {                                                                         \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

// APUE 8.9, 10.16
// https://www.gnu.org/software/libc/manual/html_node/Sigsuspend.html
static sig_atomic_t sigflag = 0;
static void sig_handler() { sigflag = 1; }

static void wait_with_signal() {
  printf("wait with signal\n");
  struct sigaction act;
  act.sa_handler = sig_handler;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
  if (sigaction(SIGUSR1, &act, NULL) == -1)
    errExit("sigaction");

  sigset_t cont_mask, old_mask;
  sigemptyset(&cont_mask);
  sigaddset(&cont_mask, SIGUSR1);
  if (sigprocmask(SIG_BLOCK, &cont_mask, &old_mask) == -1)
    errExit("sigprocmask");

  pid_t cpid = fork();
  if (cpid < 0)
    errExit("fork");
  else if (cpid == 0) {
    printf("hello\n");
    kill(getppid(), SIGUSR1);
  } else {
    while (sigflag == 0)
      sigsuspend(&old_mask);
    printf("goodbye\n");
  }
}

// APUE 15.2
static void wait_with_pipe() {
  printf("wait with pipe\n");
  int pipefd[2];
  if (pipe(pipefd) == -1)
    errExit("pipe");

  pid_t cpid = fork();
  if (cpid < 0)
    errExit("fork");
  else if (cpid == 0) {
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

_Noreturn static void usage(char *name) {
  fprintf(stderr, "Usage: %s [-s|--signal] [-p|--pipe]\n", name);
  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
  int opt;
  struct option options[] = {
    {"signal", no_argument, NULL, 's'},
    {"pipe", no_argument, NULL, 'p'}
  };
  if ((opt = getopt_long(argc, argv, "sp", options, NULL)) != -1) {
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
