#include <stdio.h>
#include <stdlib.h> // exit
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> // fork, getpid, getppid, sleep

#if __has_include(<sys/prctl.h>)
#include <sys/prctl.h>
#define HAS_PRCTL
#endif

int main() {
  printf("main process id: %d\n", getpid());

#ifdef HAS_PRCTL
  prctl(PR_SET_CHILD_SUBREAPER, 1, 0, 0, 0);
#endif

  pid_t second_pid = fork();
  pid_t third_pid = 0;
  if (second_pid == -1) {
    fprintf(stderr, "fork failed\n");
    exit(EXIT_FAILURE);
  } else if (second_pid == 0) {
    third_pid = fork();
    if (third_pid == -1) {
      fprintf(stderr, "fork failed\n");
      exit(EXIT_FAILURE);
    } else if (third_pid == 0) {
      sleep(1);
      // this orphan process's new parent is systemd/init(pid 1)
      // or the nearest "subreaper" defined by prctl(2)
      printf("third process's parent pid: %d\n", getppid());
    }
    /* waitpid(third_pid, NULL, 0); */
  } else {
    printf("second process id: %d\n", second_pid);
    waitpid(second_pid, NULL, 0);
#ifdef HAS_PRCTL
    waitpid(third_pid, NULL, 0);
#endif
  }
  return 0;
}

/*
 * without prctl:
 * main process id: 41183
 * second process id: 41184
 * third process's parent pid: 1
 *
 * with prctl:
 * main process id: 12778
 * second process id: 12779
 * third process's parent pid: 12778
 *
 * https://en.wikipedia.org/wiki/Orphan_process
 * https://en.wikipedia.org/wiki/Zombie_process
 * wait(2) exit(3) _Exit(2) prctl(2)
 */
