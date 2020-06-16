#ifdef Linux
  #define _GNU_SOURCE
  #include <sched.h> // CPU_ZERO
#endif

#ifdef FreeBSD
  #include <malloc_np.h>
  #include <pthread_np.h>
  #include <sys/_cpuset.h>
  #include <sys/cpuset.h>
  #ifndef cpu_set_t
    #define cpu_set_t cpuset_t
  #endif
#endif

#include <errno.h>
#include <pthread.h>   // pthread_self
#include <stdio.h>     // printf, fprintf
#include <stdlib.h>    // exit, malloc, free
#include <sys/times.h> // times
#include <unistd.h>    // sysconf
#define handle_error_en(en, msg)                                               \
  do {                                                                         \
    errno = en;                                                                \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

int main(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(stderr, "Need the number of pages and the number of trials\n");
    exit(EXIT_FAILURE);
  }

  if (argc == 3) {
    cpu_set_t cpuset;
    pthread_t thread = pthread_self();
    CPU_ZERO(&cpuset);
    CPU_SET(0, &cpuset);
    int s;

    s = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
    if (s != 0)
      handle_error_en(s, "pthread_setaffinity_np");
  }

  long PAGESIZE = sysconf(_SC_PAGESIZE);
  long clktck = sysconf(_SC_CLK_TCK);
  long jump = PAGESIZE / (long)sizeof(int);
  int NUMPAGES = atoi(argv[1]);
  int trails = atoi(argv[2]);
  if (NUMPAGES <= 0) {
    fprintf(stderr, "Invalid input\n");
    exit(EXIT_FAILURE);
  }
  int *a = (int *)malloc((size_t)NUMPAGES * (size_t)PAGESIZE);
  struct tms tmsstart, tmsend;
  clock_t start, end;

  if ((int)(start = times(&tmsstart)) == -1)
    handle_error_en(errno, "times");

  for (int j = 0; j < trails; j++) {
    for (int i = 0; i < NUMPAGES * jump; i += jump) {
      a[i] += 1;
    }
  }

  if ((int)(end = times(&tmsend)) == -1)
    handle_error_en(errno, "times");

  int nloops = trails * NUMPAGES;
  // nanoseconds
  printf("%f %f %f\n", (double)(end - start) / (double)clktck * 1000000000 / nloops,
         ((double)(tmsend.tms_utime - tmsstart.tms_utime) / (double)clktck) *
             1000000000 / nloops,
         ((double)(tmsend.tms_stime - tmsstart.tms_stime) / (double)clktck) *
             1000000000 / nloops);
  free(a);
  return 0;
}
