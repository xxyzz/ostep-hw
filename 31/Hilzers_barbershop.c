#include "common_threads.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>    // exit, free, malloc
#include <sys/param.h> // MAX
#include <sys/queue.h>
#include <unistd.h> // getopt

// Operating Systems: Internals and Design Principles, appendix A.2
// Little Book of Semaphores: chapter 5.4
// https://github.com/AllenDowney/LittleBookOfSemaphores/blob/master/code/sync_code/barber4.py
sem_t *cust_num_mutex, *sofaq_mutex, *chairq_mutex, *cust_arrive, *cust_on_sofa,
    *cust_leave_sofa, *payment, *receipt, *cashier_mutex;
int max_capacity = 8, chairs = 3, sofa_size = 4, customer_num = 0;
struct entry {
  sem_t *cust_sem;
  int index;
  STAILQ_ENTRY(entry) entries;
};
STAILQ_HEAD(stailhead, entry);
struct stailhead wait_sofaq_head;
struct stailhead wait_chairq_head;

void init_sem() {
  STAILQ_INIT(&wait_sofaq_head);
  STAILQ_INIT(&wait_chairq_head);
#ifdef __APPLE__
  cust_num_mutex = Sem_open("/cust_num_mutex", 1);
  sofaq_mutex = Sem_open("/sofaq_mutex", 1);
  chairq_mutex = Sem_open("/chairq_mutex", 1);
  cust_arrive = Sem_open("/cust_arrive", 0);
  cust_on_sofa = Sem_open("/cust_on_sofa", 0);
  cust_leave_sofa = Sem_open("/cust_leave_sofa", 0);
  payment = Sem_open("/payment", 0);
  receipt = Sem_open("/receipt", 0);
  cashier_mutex = Sem_open("/cashier_mutex", 1);
#else
  cust_num_mutex = malloc(sizeof(sem_t));
  Sem_init(cust_num_mutex, 0, 1);
  sofaq_mutex = malloc(sizeof(sem_t));
  Sem_init(sofaq_mutex, 0, 1);
  chairq_mutex = malloc(sizeof(sem_t));
  Sem_init(chairq_mutex, 0, 1);
  cust_arrive = malloc(sizeof(sem_t));
  Sem_init(cust_arrive, 0, 0);
  cust_on_sofa = malloc(sizeof(sem_t));
  Sem_init(cust_on_sofa, 0, 0);
  cust_leave_sofa = malloc(sizeof(sem_t));
  Sem_init(cust_leave_sofa, 0, 0);
  payment = malloc(sizeof(sem_t));
  Sem_init(payment, 0, 0);
  receipt = malloc(sizeof(sem_t));
  Sem_init(receipt, 0, 0);
  cashier_mutex = malloc(sizeof(sem_t));
  Sem_init(cashier_mutex, 0, 1);
#endif
}

void destroy_sem() {
#ifdef __APPLE__
  Sem_close(cust_num_mutex);
  Sem_unlink("/cust_num_mutex");
  Sem_close(sofaq_mutex);
  Sem_unlink("/sofaq_mutex");
  Sem_close(chairq_mutex);
  Sem_unlink("/chairq_mutex");
  Sem_close(cust_arrive);
  Sem_unlink("/cust_arrive");
  Sem_close(cust_on_sofa);
  Sem_unlink("/cust_on_sofa");
  Sem_close(cust_leave_sofa);
  Sem_unlink("/cust_leave_sofa");
  Sem_close(payment);
  Sem_unlink("/payment");
  Sem_close(receipt);
  Sem_unlink("/receipt");
  Sem_close(cashier_mutex);
  Sem_unlink("/cashier_mutex");
#else
  Sem_destroy(cust_num_mutex);
  free(cust_num_mutex);
  Sem_destroy(sofaq_mutex);
  free(sofaq_mutex);
  Sem_destroy(chairq_mutex);
  free(chairq_mutex);
  Sem_destroy(cust_arrive);
  free(cust_arrive);
  Sem_destroy(cust_on_sofa);
  free(cust_on_sofa);
  Sem_destroy(cust_leave_sofa);
  free(cust_leave_sofa);
  Sem_destroy(payment);
  free(payment);
  Sem_destroy(receipt);
  free(receipt);
  Sem_destroy(cashier_mutex);
  free(cashier_mutex);
#endif
}

void *barber(void *arg) {
  int index = *(int *)arg;
  struct entry *e;
  while (true) {
    Sem_wait(cust_on_sofa);
    Sem_wait(chairq_mutex);
    e = STAILQ_FIRST(&wait_chairq_head);
    STAILQ_REMOVE_HEAD(&wait_chairq_head, entries);
    Sem_post(chairq_mutex);
    Sem_post(e->cust_sem);
    printf("Barber %d cuts customer %d's hair.\n", index, e->index);

    Sem_wait(payment);
    Sem_wait(cashier_mutex);
    printf("Barber %d accepts payment.\n", index);
    Sem_post(cashier_mutex);
    Sem_post(receipt);
  }
  return NULL;
}

struct entry *init_entry(char *name, int index) {
  sem_t *cust_sem;
#ifdef __APPLE__
  cust_sem = Sem_open(name, 0);
#else
  cust_sem = malloc(sizeof(sem_t));
  Sem_init(cust_sem, 0, 0);
#endif
  struct entry *e = malloc(sizeof(struct entry));
  e->cust_sem = cust_sem;
  e->index = index;
  return e;
}

void free_entry(struct entry *e, char *name) {
#ifdef __APPLE__
  Sem_close(e->cust_sem);
  Sem_unlink(name);
#else
  Sem_destroy(e->cust_sem);
  free(e->cust_sem);
#endif
  free(e);
}

void *customer(void *arg) {
  int index = *(int *)arg;
  Sem_wait(cust_num_mutex);
  if (customer_num == max_capacity) {
    Sem_post(cust_num_mutex);
    printf("Customer %d balks.\n", index);
    pthread_exit(NULL);
  }
  customer_num++;
  printf("Customer %d arrives.\n", index);
  Sem_post(cust_num_mutex);

  Sem_wait(sofaq_mutex);
  char name[BUFSIZ];
  sprintf(name, "/cust_sofa_%d", index);
  struct entry *e = init_entry(name, index);
  STAILQ_INSERT_TAIL(&wait_sofaq_head, e, entries);
  Sem_post(sofaq_mutex);

  Sem_post(cust_arrive);
  Sem_wait(e->cust_sem); // get sofa
  free_entry(e, name);
  printf("Customer %d sits on sofa.\n", index);
  Sem_wait(chairq_mutex);
  sprintf(name, "/cust_chair_%d", index);
  e = init_entry(name, index);
  STAILQ_INSERT_TAIL(&wait_chairq_head, e, entries);
  Sem_post(chairq_mutex);
  Sem_post(cust_on_sofa);
  Sem_wait(e->cust_sem); // get chair
  free_entry(e, name);
  Sem_post(cust_leave_sofa);

  printf("Customer %d gets hair cut.\n", index);
  Sem_post(payment);
  Sem_wait(receipt);

  Sem_wait(cust_num_mutex);
  customer_num--;
  printf("Customer %d leaves.\n", index);
  Sem_post(cust_num_mutex);
  return NULL;
}

void *usher(void *arg) {
  int index = *(int *)arg;
  struct entry *e;
  while (true) {
    Sem_wait(cust_arrive);
    Sem_wait(sofaq_mutex);
    e = STAILQ_FIRST(&wait_sofaq_head);
    STAILQ_REMOVE_HEAD(&wait_sofaq_head, entries);
    Sem_post(sofaq_mutex);
    Sem_post(e->cust_sem);
    printf("Usher %d guides customer %d to sofa.\n", index, e->index);
    Sem_wait(cust_leave_sofa);
  }
  return NULL;
}

int main(int argc, char *argv[]) {
  int opt, total_customers = 9;
  while ((opt = getopt(argc, argv, "h:s:c:m:")) != -1) {
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
    case 's':
      sofa_size = atoi(optarg);
      if (sofa_size <= 0) {
        fprintf(stderr, "Don't mess around.\n");
        exit(EXIT_FAILURE);
      }
    case 'm':
      max_capacity = atoi(optarg);
      if (max_capacity <= 0) {
        fprintf(stderr, "Don't screw around.\n");
        exit(EXIT_FAILURE);
      }
    default:
      fprintf(stderr,
              "Usage: %s [-h chairs] [-s sofa_size] [-c total_customers] [-m "
              "max_capacity]\n",
              argv[0]);
      exit(EXIT_FAILURE);
    }
  }

  pthread_t barber_threads[chairs];
  pthread_t usher_threads[sofa_size];
  pthread_t customer_threads[total_customers];
  int arr_len = MAX(MAX(chairs, total_customers), sofa_size);
  int stupid_arr[arr_len];
  for (int i = 0; i < arr_len; i++)
    stupid_arr[i] = i;
  init_sem();

  for (int i = 0; i < chairs; i++)
    Pthread_create(&barber_threads[i], NULL, barber, &stupid_arr[i]);
  for (int i = 0; i < sofa_size; i++)
    Pthread_create(&usher_threads[i], NULL, usher, &stupid_arr[i]);
  for (int i = 0; i < total_customers; i++)
    Pthread_create(&customer_threads[i], NULL, customer, &stupid_arr[i]);

  for (int i = 0; i < total_customers; i++)
    Pthread_join(customer_threads[i], NULL);
  for (int i = 0; i < chairs; i++)
    Pthread_cancel(barber_threads[i]);
  for (int i = 0; i < sofa_size; i++)
    Pthread_cancel(usher_threads[i]);
  destroy_sem();
  return 0;
}
