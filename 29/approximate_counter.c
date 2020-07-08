#include "thread_helper.h"
#include <math.h> // pow
#include <stdio.h>
#include <stdlib.h> // malloc
#include <sys/time.h>
#ifdef FreeBSD
#include <malloc_np.h>
#endif
#define NUMCPUS 4
#define ONE_MILLION 1000000

typedef struct __counter_t {
  pthread_mutex_t glock;          // global lock
  int local[NUMCPUS];             // local count (per CPU)
  pthread_mutex_t llock[NUMCPUS]; // ... and locks
  int global;                     // global count
  int threshold;                  // update frequency
} counter_t;

typedef struct __myarg_t {
  counter_t *c;
  int threshold;
  int amt;
  int threads;
  char pad[sizeof(counter_t *) - sizeof(int)];
} myarg_t;

// init: record threadhold, init locks, init values
//       of all local counts and global count
static void init(counter_t *c, int threshold) {
  c->threshold = threshold;
  c->global = 0;
  Pthread_mutex_init(&c->glock, NULL);
  for (int i = 0; i < NUMCPUS; i++) {
    c->local[i] = 0;
    Pthread_mutex_init(&c->llock[i], NULL);
  }
}

// update: usually, just grab local lock and update local amount
//         once local count has risen by ’threshold’, grab global
//         lock and transfer local values to it
static void update(counter_t *c, int threadID, int amt) {
  int cpu = threadID % NUMCPUS;
  Pthread_mutex_lock(&c->llock[cpu]);
  c->local[cpu] += amt;
  if (c->local[cpu] >= c->threshold) {
    // transfer to global (assumes amt>0)
    Pthread_mutex_lock(&c->glock);
    c->global += c->local[cpu];
    Pthread_mutex_unlock(&c->glock);
    c->local[cpu] = 0;
  }
  Pthread_mutex_unlock(&c->llock[cpu]);
}

// get: just return global amount (approximate)
static int get(counter_t *c) {
  Pthread_mutex_lock(&c->glock);
  int val = c->global;
  Pthread_mutex_unlock(&c->glock);
  return val; // only approximate!
}

static void *thread_function(void *arg) {
  myarg_t *m = (myarg_t *)arg;
  pthread_t threadID = pthread_self();
  for (int i = 0; i < ONE_MILLION; i++)
    update(m->c, (int)threadID, m->amt);
  pthread_exit(0);
}

int main(int argc, char *argv[]) {
  for (int i = 0; i < 11; i++) {
    double threshold = pow(2.0, (double)i);
    for (int j = 1; j <= NUMCPUS; j++) {
      counter_t *c = malloc(sizeof(counter_t));
      if (c == NULL)
        handle_error_en(errno, "malloc");
      init(c, (int)threshold);
      pthread_t *threads = malloc((size_t)j * sizeof(pthread_t));
      if (threads == NULL)
        handle_error_en(errno, "malloc");
      myarg_t args;
      args.c = c;
      args.threshold = (int)threshold;
      args.amt = 1;
      args.threads = j;
      struct timeval start, end;
      int s = 0;
      s = gettimeofday(&start, NULL);
      if (s != 0)
        handle_error_en(s, "gettimeofday");
      for (int k = 0; k < j; k++)
        Pthread_create(&threads[k], NULL, &thread_function, &args);
      for (int l = 0; l < j; l++)
        Pthread_join(threads[l], NULL);
      s = gettimeofday(&end, NULL);
      if (s != 0)
        handle_error_en(s, "gettimeofday");
      long long startusec, endusec;
      startusec = start.tv_sec * ONE_MILLION + start.tv_usec;
      endusec = end.tv_sec * ONE_MILLION + end.tv_usec;
      printf("%d threads, %d threshold\n", j, (int)threshold);
      printf("%d global counter\n", get(c));
      printf("Time (seconds): %f\n\n",
             ((double)(endusec - startusec) / ONE_MILLION));
      Pthread_mutex_destroy(&c->glock);
      for (int m = 0; m < NUMCPUS; m++)
        Pthread_mutex_destroy(&c->llock[m]);
      free(c);
      free(threads);
    }
  }
  return 0;
}
