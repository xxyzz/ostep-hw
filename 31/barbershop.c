#include "common_threads.h"
#include <stdio.h>
#include <stdlib.h> // exit, free, malloc
#include <unistd.h> // getopt

// Little Book of Semaphores: chapter 5.2
sem_t *mutex, *customer_arrives, *barber_wakes, *customer_leaves,
    *barber_sleeps;
int chairs = 4, customers = 0;

void init_sem() {
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
  int total_customers = *(int *)arg;
  int loop = total_customers > chairs ? chairs : total_customers;
  for (int i = 0; i < loop; i++) {
    Sem_wait(customer_arrives);
    Sem_post(barber_wakes);
    Sem_wait(customer_leaves);
    Sem_post(barber_sleeps);
  }
  return NULL;
}

void *customer(void *arg) {
  int index = *(int *)arg;
  Sem_wait(mutex);
  if (customers == chairs) {
    Sem_post(mutex);
    printf("Customer %d balks.\n", index);
    pthread_exit(NULL);
  }
  customers++;
  Sem_post(mutex);

  Sem_post(customer_arrives);
  Sem_wait(barber_wakes);

  printf("Customer %d gets haircut.\n", index);

  Sem_post(customer_leaves);
  Sem_wait(barber_sleeps);

  Sem_wait(mutex);
  customers--;
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

  Pthread_create(&barber_thread, NULL, barber, &total_customers);
  for (int i = 0; i < total_customers; i++) {
    stupid_arr[i] = i;
    Pthread_create(&customer_threads[i], NULL, customer, &stupid_arr[i]);
  }

  Pthread_join(barber_thread, NULL);
  for (int i = 0; i < total_customers; i++)
    Pthread_join(customer_threads[i], NULL);
  destroy_sem();
  return 0;
}
