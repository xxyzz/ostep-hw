#include "common_threads.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h> // exit, free, malloc
#include <sys/queue.h>
#include <unistd.h> // getopt

// Little Book of Semaphores: chapter 5.3
sem_t *mutex, *customer_arrives, *barber_wakes, *customer_leaves,
    *barber_sleeps;
int chairs = 4, customers = 0;
struct entry {
  sem_t *customer_sem;
  STAILQ_ENTRY(entry) entries;
};
STAILQ_HEAD(stailhead, entry);
struct stailhead head;

void init_sem() {
  STAILQ_INIT(&head);
#ifdef __APPLE__
  mutex = Sem_open("/mutex", 1);
  customer_arrives = Sem_open("/customer_arrives", 0);
  barber_wakes = Sem_open("/barber_wakes", 0);
  customer_leaves = Sem_open("/customer_leaves", 0);
  barber_sleeps = Sem_open("/barber_sleeps", 0);
#else
  mutex = malloc(sizeof(sem_t));
  Sem_init(mutex, 0, 1);
  customer_arrives = malloc(sizeof(sem_t));
  Sem_init(customer_arrives, 0, 0);
  barber_wakes = malloc(sizeof(sem_t));
  Sem_init(barber_wakes, 0, 0);
  customer_leaves = malloc(sizeof(sem_t));
  Sem_init(customer_leaves, 0, 0);
  barber_sleeps = malloc(sizeof(sem_t));
  Sem_init(barber_sleeps, 0, 0);
#endif
}

void destroy_sem() {
#ifdef __APPLE__
  Sem_close(mutex);
  Sem_unlink("/mutex");
  Sem_close(customer_arrives);
  Sem_unlink("/customer_arrives");
  Sem_close(barber_wakes);
  Sem_unlink("/barber_wakes");
  Sem_close(customer_leaves);
  Sem_unlink("/customer_leaves");
  Sem_close(barber_sleeps);
  Sem_unlink("/barber_sleeps");
#else
  Sem_destroy(mutex);
  free(mutex);
  Sem_destroy(customer_arrives);
  free(customer_arrives);
  Sem_destroy(barber_wakes);
  free(barber_wakes);
  Sem_destroy(customer_leaves);
  free(customer_leaves);
  Sem_destroy(barber_sleeps);
  free(barber_sleeps);
#endif
}

void *barber(void *arg) {
  struct entry *e;
  while (true) {
    Sem_wait(customer_arrives);
    Sem_wait(mutex);
    e = STAILQ_FIRST(&head);
    STAILQ_REMOVE_HEAD(&head, entries);
    Sem_post(mutex);

    Sem_post(e->customer_sem);

    Sem_wait(customer_leaves);
    Sem_post(barber_sleeps);
  }
  return NULL;
}

void *customer(void *arg) {
  int index = *(int *)arg;
  sem_t *customer_sem;
  Sem_wait(mutex);
  if (customers == chairs) {
    Sem_post(mutex);
    printf("Customer %d balks.\n", index);
    pthread_exit(NULL);
  }
  customers++;
#ifdef __APPLE__
  char name[BUFSIZ];
  sprintf(name, "/customer_%d", index);
  customer_sem = Sem_open(name, 0);
#else
  customer_sem = malloc(sizeof(sem_t));
  Sem_init(customer_sem, 0, 0);
#endif
  struct entry *e = malloc(sizeof(struct entry));
  e->customer_sem = customer_sem;
  STAILQ_INSERT_TAIL(&head, e, entries);
  printf("Customer %d arrives.\n", index);
  Sem_post(mutex);

  Sem_post(customer_arrives);
  Sem_wait(customer_sem);

  printf("Customer %d gets haircut.\n", index);

  Sem_post(customer_leaves);
  Sem_wait(barber_sleeps);

  Sem_wait(mutex);
  customers--;
#ifdef __APPLE__
  Sem_close(e->customer_sem);
  Sem_unlink(name);
#else
  Sem_destroy(e->customer_sem);
  free(e->customer_sem);
#endif
  free(e);
  Sem_post(mutex);
  return NULL;
}

int main(int argc, char *argv[]) {
  int opt, total_customers = 3;
  while ((opt = getopt(argc, argv, "h:c:")) != -1) {
    switch (opt) {
    case 'h':
      chairs = atoi(optarg);
      if (chairs <= 0) {
        fprintf(stderr, "Damn you.\n");
        exit(EXIT_FAILURE);
      }
      break;
    case 'c':
      total_customers = atoi(optarg);
      if (total_customers <= 0) {
        fprintf(stderr, "You're breaking my balls.\n");
        exit(EXIT_FAILURE);
      }
      break;
    default:
      fprintf(stderr, "Usage: %s [-h chairs] [-c total_customers]\n", argv[0]);
      exit(EXIT_FAILURE);
    }
  }

  pthread_t barber_thread;
  pthread_t customer_threads[total_customers];
  int stupid_arr[total_customers];
  init_sem();

  Pthread_create(&barber_thread, NULL, barber, NULL);
  for (int i = 0; i < total_customers; i++) {
    stupid_arr[i] = i;
    Pthread_create(&customer_threads[i], NULL, customer, &stupid_arr[i]);
  }

  for (int i = 0; i < total_customers; i++)
    Pthread_join(customer_threads[i], NULL);
  Pthread_cancel(barber_thread);
  destroy_sem();
  return 0;
}
