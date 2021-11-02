#include "common_threads.h"
#include <stdio.h>
#include <stdlib.h> // exit, free, malloc
#include <unistd.h> // getopt

// Little Book of Semaphores: chapter 5.1
sem_t *empty_pot, *full_pot, *mutex;
int stewed_missionaries = 0, pot_size = 2;

void init_sem() {
#ifdef __APPLE__
  empty_pot = Sem_open("/empty_pot", 0);
  full_pot = Sem_open("/full_pot", 0);
  mutex = Sem_open("/mutex", 1);
#else
  empty_pot = malloc(sizeof(sem_t));
  Sem_init(empty_pot, 0, 0);
  full_pot = malloc(sizeof(sem_t));
  Sem_init(full_pot, 0, 0);
  mutex = malloc(sizeof(sem_t));
  Sem_init(mutex, 0, 1);
#endif
}

void destroy_sem() {
#ifdef __APPLE__
  Sem_close(empty_pot);
  Sem_unlink("/empty_pot");
  Sem_close(full_pot);
  Sem_unlink("/full_pot");
  Sem_close(mutex);
  Sem_unlink("/mutex");
#else
  Sem_destroy(empty_pot);
  free(empty_pot);
  Sem_destroy(full_pot);
  free(full_pot);
  Sem_destroy(mutex);
  free(mutex);
#endif
}

void *cook(void *arg) {
  int savages_num = *(int *)arg;
  int loop = 0;
  if (savages_num % pot_size == 0)
    loop = savages_num / pot_size;
  else
    loop = savages_num / pot_size + 1;
  for (int i = 0; i < loop; i++) {
    Sem_wait(empty_pot);
    printf("Cook fills the pot with %d stewed missionaries\n", pot_size);
    Sem_post(full_pot);
  }
  return NULL;
}

void *savage(void *arg) {
  int index = *(int *)arg;
  Sem_wait(mutex);
  if (stewed_missionaries == 0) {
    Sem_post(empty_pot);
    Sem_wait(full_pot);
    stewed_missionaries = pot_size;
  }
  stewed_missionaries--;
  printf("Savage %d feats on a missionary\n", index);
  Sem_post(mutex);
  return NULL;
}

int main(int argc, char *argv[]) {
  int opt, savages_num = 3;
  while ((opt = getopt(argc, argv, "p:s:")) != -1) {
    switch (opt) {
    case 'p':
      pot_size = atoi(optarg);
      if (pot_size <= 0) {
        fprintf(stderr, "A missionary walks into a pot...\n");
        exit(EXIT_FAILURE);
      }
      break;
    case 's':
      savages_num = atoi(optarg);
      if (savages_num <= 0) {
        fprintf(stderr, "A savage walks into a bar...\n");
        exit(EXIT_FAILURE);
      }
      break;
    default:
      fprintf(stderr, "Usage: %s [-p pot_size] [-s savages_number]\n", argv[0]);
      exit(EXIT_FAILURE);
    }
  }

  pthread_t cook_thread;
  pthread_t savages_threads[savages_num];
  int stupid_arr[savages_num];
  init_sem();

  Pthread_create(&cook_thread, NULL, cook, &savages_num);
  for (int i = 0; i < savages_num; i++) {
    stupid_arr[i] = i;
    Pthread_create(&savages_threads[i], NULL, savage, &stupid_arr[i]);
  }

  Pthread_join(cook_thread, NULL);
  for (int i = 0; i < savages_num; i++)
    Pthread_join(savages_threads[i], NULL);
  destroy_sem();
  return 0;
}
