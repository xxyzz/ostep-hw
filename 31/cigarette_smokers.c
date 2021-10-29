#include "common_threads.h"
#include <stdbool.h>
#include <stdio.h>

// Little Book of Semaphores: chapter 4.5

sem_t tobacco_sem, paper_sem, match_sem, agent_sem, lock;
bool has_tobacco = false;
bool has_paper = false;
bool has_match = false;

void init_sem() {
  Sem_init(&tobacco_sem, 0, 0);
  Sem_init(&paper_sem, 0, 0);
  Sem_init(&match_sem, 0, 0);
  Sem_init(&agent_sem, 0, 1);
  Sem_init(&lock, 0, 1);
}

void destroy_sem() {
  Sem_destroy(&tobacco_sem);
  Sem_destroy(&paper_sem);
  Sem_destroy(&match_sem);
  Sem_destroy(&agent_sem);
  Sem_destroy(&lock);
}

void tobacco_pusher() {
  if (has_paper) {
    has_paper = false;
    Sem_post(&match_sem);
  } else if (has_match) {
    has_match = false;
    Sem_post(&paper_sem);
  } else
    has_tobacco = true;
}

void paper_pusher() {
  if (has_match) {
    has_match = false;
    Sem_post(&tobacco_sem);
  } else if (has_tobacco) {
    has_tobacco = false;
    Sem_post(&match_sem);
  } else
    has_paper = true;
}

void match_pusher() {
  if (has_paper) {
    has_paper = false;
    Sem_post(&tobacco_sem);
  } else if (has_tobacco) {
    has_tobacco = false;
    Sem_post(&paper_sem);
  } else
    has_match = true;
}

void *agent(void *arg) {
  int type = *(int *)arg;
  Sem_wait(&agent_sem);
  Sem_wait(&lock);
  switch (type) {
  case 0:
    tobacco_pusher();
    paper_pusher();
    break;
  case 1:
    paper_pusher();
    match_pusher();
    break;
  case 2:
    tobacco_pusher();
    match_pusher();
  }
  Sem_post(&lock);
  return NULL;
}

void *smoker(void *arg) {
  int type = *(int *)arg;
  switch (type) {
  case 0:
    Sem_wait(&tobacco_sem);
    printf("Smoker with tobacco is getting a little bit of cancer.\n");
    Sem_post(&agent_sem);
    break;
  case 1:
    Sem_wait(&paper_sem);
    printf("Smoker with paper is getting a little bit of cancer.\n");
    Sem_post(&agent_sem);
    break;
  case 2:
    Sem_wait(&match_sem);
    printf("Smoker with match is getting a little bit of cancer.\n");
    Sem_post(&agent_sem);
  }
  return NULL;
}

int main() {
  pthread_t agents[3];
  pthread_t smokers[3];
  int types[] = {0, 1, 2};
  init_sem();
  for (int i = 0; i < 3; i++) {
    Pthread_create(&agents[i], NULL, agent, &types[i]);
    Pthread_create(&smokers[i], NULL, smoker, &types[i]);
  }
  for (int i = 0; i < 3; i++) {
    Pthread_join(agents[i], NULL);
    Pthread_join(smokers[i], NULL);
  }
  destroy_sem();
  return 0;
}
