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

typedef struct __myarg_t {
  counter_t *counter;
  cpu_set_t set;
  int threads;
  char pad[sizeof(counter_t *) - sizeof(int)];
} myarg_t;

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
  myarg_t *m = (myarg_t *)arg;
  int s;
  s = pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &m->set);
  if (s != 0)
    handle_error_en(s, "pthread_setaffinity_np");

  for (int i = 0; i < ONE_MILLION / m->threads; i++)
    increment(m->counter);

  pthread_exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
  cpu_set_t set;
  int cpus = (int)sysconf(_SC_NPROCESSORS_ONLN), s;
  if (cpus == -1)
    handle_error_en(cpus, "sysconf");

  for (int i = 1; i <= cpus; i++) {
    CPU_ZERO(&set);
    for (int m = 0; m < i; m++)
      CPU_SET(m, &set);

    printf("%d CPUs\n", i);
    for (int l = 1; l <= cpus; l++) {
      pthread_t *threads = malloc((size_t)l * sizeof(pthread_t));
      if (threads == NULL)
        handle_error_en(errno, "malloc");
      struct timeval start, end;
      counter_t *counter = malloc(sizeof(counter_t));
      if (counter == NULL)
        handle_error_en(errno, "malloc");
      init(counter);
      myarg_t args;
      args.counter = counter;
      args.set = set;
      args.threads = l;

      s = gettimeofday(&start, NULL);
      if (s != 0)
        handle_error_en(s, "gettimeofday");
      for (int j = 0; j < l; j++)
        pthread_create(&threads[j], NULL, &thread_function, &args);
      for (int k = 0; k < l; k++)
        pthread_join(threads[k], NULL);
      s = gettimeofday(&end, NULL);
      if (s != 0)
        handle_error_en(s, "gettimeofday");

      printf("%d threads\n", l);
      printf("global count: %d\n", get(counter));
      printf("Time (seconds): %f\n\n",
             ((double)(end.tv_usec - start.tv_usec) / ONE_MILLION +
              (double)(end.tv_sec - start.tv_sec)));
      Pthread_mutex_destroy(&counter->lock);
      free(counter);
      free(threads);
    }
  }
  return 0;
}
