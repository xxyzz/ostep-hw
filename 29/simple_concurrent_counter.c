#ifdef Linux
#define _GNU_SOURCE
#include <sched.h> // CPU_ZERO
#endif

#ifdef FreeBSD
#include <malloc_np.h>
#include <sys/_cpuset.h>
#include <sys/cpuset.h>
#ifndef cpu_set_t
#define cpu_set_t cpuset_t
#endif
#endif

#include "thread_helper.h"
#include <stdio.h>
#include <stdlib.h>   // malloc, free
#include <sys/time.h> // gettimeofday
#include <unistd.h>   // sysconf
#define ONE_MILLION 1000000

typedef struct __counter_t {
  pthread_mutex_t lock;
  int value;
  char pad[sizeof(pthread_mutex_t) - sizeof(int)];
} counter_t;

typedef struct __thread_info_t {
  counter_t *counter;
  pthread_t thread;
  int cpu_idx;
  char pad[sizeof(counter_t *) - sizeof(int)];
} thread_info_t;

static void init(counter_t *c) {
  c->value = 0;
  Pthread_mutex_init(&c->lock, NULL);
}

static void increment(counter_t *c) {
  Pthread_mutex_lock(&c->lock);
  c->value++;
  Pthread_mutex_unlock(&c->lock);
}

static int get(counter_t *c) {
  Pthread_mutex_lock(&c->lock);
  int rc = c->value;
  Pthread_mutex_unlock(&c->lock);
  return rc;
}

static void *thread_function(void *arg) {
  thread_info_t *m = (thread_info_t *)arg;
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  CPU_SET(m->cpu_idx, &cpuset);
  Pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);

  for (int i = 0; i < ONE_MILLION; i++)
    increment(m->counter);

  pthread_exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
  int cpus = (int)sysconf(_SC_NPROCESSORS_ONLN), s;
  if (cpus == -1)
    handle_error_en(cpus, "sysconf");

  for (int i = 1; i <= cpus; i++) {
    for (int l = 1; l <= cpus; l++) {
      thread_info_t *tinfo = malloc((size_t)l * sizeof(thread_info_t));
      if (tinfo == NULL)
        handle_error_en(errno, "malloc");
      struct timeval start, end;
      counter_t *counter = malloc(sizeof(counter_t));
      if (counter == NULL)
        handle_error_en(errno, "malloc");
      init(counter);

      s = gettimeofday(&start, NULL);
      if (s != 0)
        handle_error_en(s, "gettimeofday");
      for (int j = 0; j < l; j++) {
        tinfo[j].counter = counter;
        tinfo[j].cpu_idx = j % i;
        Pthread_create(&tinfo[j].thread, NULL, &thread_function, &tinfo[j]);
      }
      for (int k = 0; k < l; k++)
        Pthread_join(tinfo[k].thread, NULL);
      s = gettimeofday(&end, NULL);
      if (s != 0)
        handle_error_en(s, "gettimeofday");

      long long startusec, endusec;
      startusec = start.tv_sec * ONE_MILLION + start.tv_usec;
      endusec = end.tv_sec * ONE_MILLION + end.tv_usec;
      printf("%d cpus, %d threads\n", i, l);
      printf("global count: %d\n", get(counter));
      printf("Time (seconds): %f\n\n",
             ((double)(endusec - startusec) / ONE_MILLION));
      Pthread_mutex_destroy(&counter->lock);
      free(counter);
      free(tinfo);
    }
  }
  return 0;
}
