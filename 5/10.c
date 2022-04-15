#include <spawn.h> // man posix_spawn
#include <stdio.h>
#include <stdlib.h> // exit
#include <sys/wait.h>

#define errExit(msg)                                                           \
  do {                                                                         \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

char **environ; // man 7 environ

int main() {
  pid_t child_pid;
  char *args[] = {"ls", "-l", "-a", "-h", (char *)NULL};
  if (posix_spawnp(&child_pid, "ls", NULL, NULL, args, environ) != 0)
    errExit("posix_spawn");
  printf("PID of child: %d\n", child_pid);
  if (wait(NULL) == -1)
    errExit("wait");
  printf("Done\n");
  return 0;
}
