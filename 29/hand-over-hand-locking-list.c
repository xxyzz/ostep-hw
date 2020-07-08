#include "thread_helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#define ONE_MILLION 1000000

// basic node structure
typedef struct __node_t {
  struct __node_t *next;
  pthread_mutex_t lock;
  int key;
  char pad[sizeof(struct __node_t *) - sizeof(int)];
} node_t;

// basic list structure (one used per list)
typedef struct __list_t {
  node_t *head;
} list_t;

typedef struct __myarg_t {
  list_t *list;
  int count;
  char pad[sizeof(list_t *) - sizeof(int)];
} myarg_t;

static void List_Init(list_t *L) {
  node_t *new = malloc(sizeof(node_t));
  if (new == NULL)
    handle_error_en(errno, "malloc");
  Pthread_mutex_init(&new->lock, NULL);
  new->next = NULL;
  new->key = 0;
  L->head = new;
}

static void List_Insert(list_t *L, int key) {
  // synchronization not needed
  node_t *new = malloc(sizeof(node_t));
  if (new == NULL)
    handle_error_en(errno, "malloc");
  new->key = key;
  Pthread_mutex_init(&new->lock, NULL);

  // just lock critical section
  pthread_mutex_t *tempLock = &L->head->lock;
  Pthread_mutex_lock(tempLock);
  new->next = L->head;
  L->head = new;
  Pthread_mutex_unlock(tempLock);
}

static int List_Lookup(list_t *L, int key) {
  int rv = -1;
  node_t *curr = L->head;
  Pthread_mutex_lock(&curr->lock);
  while (curr->next != NULL) {
    if (curr->key == key) {
      rv = 0;
      break;
    }
    pthread_mutex_t *tempLock = &curr->lock;
    curr = curr->next;
    Pthread_mutex_lock(&curr->lock);
    Pthread_mutex_unlock(tempLock);
  }
  Pthread_mutex_unlock(&curr->lock);
  return rv; // now both success and failure
}

static void List_Print(list_t *L) {
  node_t *curr = L->head;
  Pthread_mutex_lock(&curr->lock);
  while (curr->next != NULL) {
    printf("%d\n", curr->key);
    pthread_mutex_t *tempLock = &curr->lock;
    curr = curr->next;
    Pthread_mutex_lock(&curr->lock);
    Pthread_mutex_unlock(tempLock);
  }
  Pthread_mutex_unlock(&curr->lock);
}

static void List_Free(list_t *L) {
  node_t *curr = L->head;
  Pthread_mutex_lock(&curr->lock);
  while (curr->next != NULL) {
    node_t *tempNode = curr;
    curr = curr->next;
    if (curr != NULL)
      Pthread_mutex_lock(&curr->lock);
    Pthread_mutex_unlock(&tempNode->lock);
    free(tempNode);
  }
  Pthread_mutex_unlock(&curr->lock);
}

static void *thread_function(void *args) {
  myarg_t *m = (myarg_t *)args;
  for (int i = 0; i < m->count; i++)
    List_Insert(m->list, i);
  pthread_exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "usage: ./s.out count print");
    exit(EXIT_FAILURE);
  }
  int count = atoi(argv[1]);
  int print = atoi(argv[2]);
  for (int i = 1; i < 5; i++) {
    int s = 0;
    list_t *list = malloc(sizeof(list_t));
    if (list == NULL)
      handle_error_en(errno, "malloc");
    List_Init(list);
    myarg_t args;
    args.list = list;
    args.count = count;
    pthread_t *threads = malloc((size_t)i * sizeof(pthread_t));
    if (threads == NULL)
      handle_error_en(errno, "malloc");
    struct timeval start, end;
    s = gettimeofday(&start, NULL);
    if (s != 0)
      handle_error_en(s, "gettimeofday");
    for (int j = 0; j < i; j++)
      Pthread_create(&threads[j], NULL, &thread_function, &args);
    for (int k = 0; k < i; k++)
      Pthread_join(threads[k], NULL);
    s = gettimeofday(&end, NULL);
    if (s != 0)
      handle_error_en(s, "gettimeofday");
    long long startusec, endusec;
    startusec = start.tv_sec * ONE_MILLION + start.tv_usec;
    endusec = end.tv_sec * ONE_MILLION + end.tv_usec;
    printf("%d threads\n", i);
    if (print)
      List_Print(list);
    // printf("Search the last number: %d\n", List_Lookup(list, ONE_MILLION - 1));
    printf("Time (seconds): %f\n\n",
           ((double)(endusec - startusec) / ONE_MILLION));
    List_Free(list);
    free(list);
    free(threads);
  }
  return 0;
}
