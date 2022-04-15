#define _GNU_SOURCE // for execvpe()
#if __has_include(<features.h>)
#  include <features.h> // __GNU_LIBRARY__
#endif
#include <fcntl.h>  // open
#include <stdio.h>  // perror
#include <stdlib.h> // exit
#include <sys/wait.h>
#include <unistd.h> // fork, exec*, fexecve

#define errExit(msg)                                                           \
  do {                                                                         \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

int main() { // APUE 8.10
  char *argv[] = {"ls", "-l", "-a", "-h", (char *)NULL};
  char *envp[] = {"PATH=/bin:/usr/bin", NULL};

  pid_t cpid = fork();
  if (cpid == -1)
    errExit("fork");
  else if (cpid == 0) {
    printf("execl:\n");
    if (execl("/bin/ls", "ls", "-l", "-a", "-h", (char *)NULL) == -1)
      errExit("execl");
  }

  if (wait(NULL) == -1)
    errExit("wait");
  if ((cpid = fork()) == -1)
    errExit("fork");
  else if (cpid == 0) {
    printf("\nexeclp:\n");
    if (execlp("ls", "ls", "-l", "-a", "-h", (char *)NULL) == -1)
      errExit("execlp");
  }

  if (wait(NULL) == -1)
    errExit("wait");
  if ((cpid = fork()) == -1)
    errExit("fork");
  else if (cpid == 0) {
    printf("\nexecle:\n");
    if (execle("/bin/ls", "ls", "-l", "-a", "-h", (char *)NULL, envp) == -1)
      errExit("execle");
  }

  if (wait(NULL) == -1)
    errExit("wait");
  if ((cpid = fork()) == -1)
    errExit("fork");
  else if (cpid == 0) {
    printf("\nexecv:\n");
    if (execv("/bin/ls", argv) == -1)
      errExit("execv");
  }

  if (wait(NULL) == -1)
    errExit("wait");
  if ((cpid = fork()) == -1)
    errExit("fork");
  else if (cpid == 0) {
    printf("\nexecvp:\n");
    if (execvp("ls", argv) == -1)
      errExit("execvp");
  }

  if (wait(NULL) == -1)
    errExit("wait");
  if ((cpid = fork()) == -1)
    errExit("fork");
  else if (cpid == 0) {
    printf("\nexecve:\n");
    if (execve("/bin/ls", argv, envp) == -1) // system call
      errExit("execve");
  }

#ifdef __GNU_LIBRARY__
  if (wait(NULL) == -1)
    errExit("wait");
  if ((cpid = fork()) == -1)
    errExit("fork");
  else if (cpid == 0) {
    printf("\nexecvpe:\n");
    if (execvpe("ls", argv, envp) == -1)
      errExit("execvpe");
  }

  if (wait(NULL) == -1)
    errExit("wait");
  if ((cpid = fork()) == -1)
    errExit("fork");
  else if (cpid == 0) {
    printf("\nfexecve:\n");
    int fd = 0;
    if ((fd = open("/bin/ls", O_PATH)) == -1)
      errExit("open");
    if (fexecve(fd, argv, envp) == -1)
      errExit("fexecve");
  }
#endif

  return 0;
}
