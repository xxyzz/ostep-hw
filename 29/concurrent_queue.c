#include "thread_helper.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct __node_t {
  struct __node_t *next;
  int value;
  char pad[sizeof(struct __node_t *) - sizeof(int)];
} node_t;

typedef struct __queue_t {
  node_t *head;
  node_t *tail;
  pthread_mutex_t headLock;
  pthread_mutex_t tailLock;
} queue_t;

static void Queue_Init(queue_t *q) {
  node_t *tmp = malloc(sizeof(node_t));
  if (tmp == NULL)
    handle_error_en(errno, "malloc");
  tmp->next = NULL;
  q->head = q->tail = tmp;
  Pthread_mutex_init(&q->headLock, NULL);
  Pthread_mutex_init(&q->tailLock, NULL);
}

static void Queue_Enqueue(queue_t *q, int value) {
  node_t *tmp = malloc(sizeof(node_t));
  if (tmp == NULL)
    handle_error_en(errno, "malloc");
  tmp->value = value;
  tmp->next = NULL;

  Pthread_mutex_lock(&q->tailLock);
  q->tail->next = tmp;
  q->tail = tmp;
  Pthread_mutex_unlock(&q->tailLock);
}

static int Queue_Dequeue(queue_t *q) {
  Pthread_mutex_lock(&q->headLock);
  node_t *tmp = q->head;
  node_t *newHead = tmp->next;
  if (newHead == NULL) {
    Pthread_mutex_unlock(&q->headLock);
    return -1; // queue was empty
  }
  newHead = newHead->next;
  q->head = newHead;
  Pthread_mutex_unlock(&q->headLock);
  free(tmp);
  return 0;
}

static void Queue_Print(queue_t *q) {
  Pthread_mutex_lock(&q->headLock);
  Pthread_mutex_lock(&q->tailLock);
  node_t *node = q->head;
  while (node != NULL) {
    node_t *temp = node;
    printf("%d\n", node->value);
    node = node->next;
    free(temp);
  }
  Pthread_mutex_unlock(&q->headLock);
  Pthread_mutex_unlock(&q->tailLock);
}

int main(int argc, char *argv[]) {
  queue_t *q = malloc(sizeof(queue_t));
  Queue_Init(q);
  Queue_Enqueue(q, 0);
  Queue_Enqueue(q, 1);
  Queue_Enqueue(q, 2);
  Queue_Enqueue(q, 3);
  Queue_Dequeue(q);
  Queue_Print(q);
  Pthread_mutex_destroy(&q->headLock);
  Pthread_mutex_destroy(&q->tailLock);
  free(q);
  return 0;
}
