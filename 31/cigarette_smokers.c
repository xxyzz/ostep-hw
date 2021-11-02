#include "common_threads.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h> // free, malloc

// Little Book of Semaphores: chapter 4.5
sem_t *tobacco_sem, *paper_sem, *match_sem, *agent_sem, *lock;
bool has_tobacco = false;
bool has_paper = false;
bool has_match = false;

void init_sem() {
#ifdef __APPLE__
  tobacco_sem = Sem_open("/tobacco_sem", 0);
  paper_sem = Sem_open("/paper_sem", 0);
  match_sem = Sem_open("/match_sem", 0);
  agent_sem = Sem_open("/agent_sem", 1);
  lock = Sem_open("/lock", 1);
#else
  tobacco_sem = malloc(sizeof(sem_t));
  Sem_init(tobacco_sem, 0, 0);
  paper_sem = malloc(sizeof(sem_t));
  Sem_init(paper_sem, 0, 0);
  match_sem = malloc(sizeof(sem_t));
  Sem_init(match_sem, 0, 0);
  agent_sem = malloc(sizeof(sem_t));
  Sem_init(agent_sem, 0, 1);
  lock = malloc(sizeof(sem_t));
  Sem_init(lock, 0, 1);
#endif
}

void destroy_sem() {
#ifdef __APPLE__
  Sem_close(tobacco_sem);
  Sem_unlink("/tobacco_sem");
  Sem_close(paper_sem);
  Sem_unlink("/paper_sem");
  Sem_close(match_sem);
  Sem_unlink("/match_sem");
  Sem_close(agent_sem);
  Sem_unlink("/agent_sem");
  Sem_close(lock);
  Sem_unlink("/lock");
#else
  Sem_destroy(tobacco_sem);
  free(tobacco_sem);
  Sem_destroy(paper_sem);
  free(paper_sem);
  Sem_destroy(match_sem);
  free(match_sem);
  Sem_destroy(agent_sem);
  free(agent_sem);
  Sem_destroy(lock);
  free(lock);
#endif
}

void tobacco_pusher() {
  if (has_paper) {
    has_paper = false;
    Sem_post(match_sem);
  } else if (has_match) {
    has_match = false;
    Sem_post(paper_sem);
  } else
    has_tobacco = true;
}

void paper_pusher() {
  if (has_match) {
    has_match = false;
    Sem_post(tobacco_sem);
  } else if (has_tobacco) {
    has_tobacco = false;
    Sem_post(match_sem);
  } else
    has_paper = true;
}

void match_pusher() {
  if (has_paper) {
    has_paper = false;
    Sem_post(tobacco_sem);
  } else if (has_tobacco) {
    has_tobacco = false;
    Sem_post(paper_sem);
  } else
    has_match = true;
}

void *agent(void *arg) {
  int type = *(int *)arg;
  Sem_wait(agent_sem);
  Sem_wait(lock);
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
  Sem_post(lock);
  return NULL;
}

void *smoker(void *arg) {
  int type = *(int *)arg;
  switch (type) {
  case 0:
    Sem_wait(tobacco_sem);
    printf("Smoker with tobacco is getting a little bit of cancer.\n");
    Sem_post(agent_sem);
    break;
  case 1:
    Sem_wait(paper_sem);
    printf("Smoker with paper is getting a little bit of cancer.\n");
    Sem_post(agent_sem);
    break;
  case 2:
    Sem_wait(match_sem);
    printf("Smoker with match is getting a little bit of cancer.\n");
    Sem_post(agent_sem);
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
