#include "common_threads.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// If done correctly, each child should print their "before" message
// before either prints their "after" message. Test by adding sleep(1)
// calls in various locations.

// You likely need two semaphores to do this correctly, and some
// other integers to track things.

typedef struct __barrier_t {
  // add semaphores and other information here
  sem_t *mutex;
  sem_t *turnstile1;
  sem_t *turnstile2;
  int num_arrived;
  int num_threads;
} barrier_t;

// the single barrier we are using for this program
barrier_t b;
char *mutex_str = "/mutex";
char *turnstile1_str = "/turnstile1";
char *turnstile2_str = "/turnstile2";

void barrier_init(barrier_t *b, int num_threads) {
  // initialization code goes here
  b->mutex = Sem_open(mutex_str, 1);
  b->turnstile1 = Sem_open(turnstile1_str, 0);
  b->turnstile2 = Sem_open(turnstile2_str, 0);
  b->num_arrived = 0;
  b->num_threads = num_threads;
}

void barrier(barrier_t *b) {
  // reusable barrier code goes here
  Sem_wait(b->mutex);
  b->num_arrived += 1;
  if (b->num_arrived == b->num_threads) {
    for (int i = 0; i < b->num_threads; i++)
      Sem_post(b->turnstile1);
  }
  Sem_post(b->mutex);

  Sem_wait(b->turnstile1);

  // critical point

  Sem_wait(b->mutex);
  b->num_arrived -= 1;
  if (b->num_arrived == 0) {
    for (int i = 0; i < b->num_threads; i++)
      Sem_post(b->turnstile2);
  }
  Sem_post(b->mutex);

  Sem_wait(b->turnstile2);
}

//
// XXX: don't change below here (just run it!)
//
typedef struct __tinfo_t {
  int thread_id;
} tinfo_t;

void *child(void *arg) {
  tinfo_t *t = (tinfo_t *)arg;
  printf("child %d: before\n", t->thread_id);
  barrier(&b);
  printf("child %d: after\n", t->thread_id);
  return NULL;
}

// run with a single argument indicating the number of
// threads you wish to create (1 or more)
int main(int argc, char *argv[]) {
  assert(argc == 2);
  int num_threads = atoi(argv[1]);
  assert(num_threads > 0);

  pthread_t p[num_threads];
  tinfo_t t[num_threads];

  printf("parent: begin\n");
  barrier_init(&b, num_threads);

  int i;
  for (i = 0; i < num_threads; i++) {
    t[i].thread_id = i;
    Pthread_create(&p[i], NULL, child, &t[i]);
  }

  for (i = 0; i < num_threads; i++)
    Pthread_join(p[i], NULL);

  printf("parent: end\n");
  Sem_close(b.mutex);
  Sem_close(b.turnstile1);
  Sem_close(b.turnstile2);
  Sem_unlink(mutex_str);
  Sem_unlink(turnstile1_str);
  Sem_unlink(turnstile2_str);
  return 0;
}
