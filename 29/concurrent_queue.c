#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <assert.h>

typedef struct __node_t {
    int                 value;
    struct __node_t    *next;
} node_t;

typedef struct __queue_t {
    node_t             *head;
    node_t             *tail;
    pthread_mutex_t     headLock;
    pthread_mutex_t     tailLock;
} queue_t;

void Queue_Init(queue_t *q) {
    node_t *tmp = malloc(sizeof(node_t));
    tmp->next = NULL;
    q->head = q->tail = tmp;
    pthread_mutex_init(&q->headLock, NULL);
    pthread_mutex_init(&q->tailLock, NULL);
}

void Queue_Enqueue(queue_t *q, int value) {
    node_t *tmp = malloc(sizeof(node_t));
    assert(tmp != NULL);
    tmp->value = value;
    tmp->next  = NULL;

    pthread_mutex_lock(&q->tailLock);
    q->tail->next = tmp;
    q->tail = tmp;
    pthread_mutex_unlock(&q->tailLock);
}

int Queue_Dequeue(queue_t *q) {
    pthread_mutex_lock(&q->headLock);
    node_t *tmp = q->head;
    node_t *newHead = tmp->next;
    if (newHead == NULL) {
        pthread_mutex_unlock(&q->headLock);
        return -1; // queue was empty
    }
    newHead = newHead->next;
    q->head = newHead;
    pthread_mutex_unlock(&q->headLock);
    free(tmp);
    return 0;
}

void Queue_Print(queue_t *q) {
    pthread_mutex_lock(&q->headLock);
    pthread_mutex_lock(&q->tailLock);
    node_t *tmp = q->head;
    while (tmp != NULL) {
        printf("%d\n", tmp->value);
        tmp = tmp->next;
    }
    free(tmp);
    pthread_mutex_unlock(&q->headLock);
    pthread_mutex_unlock(&q->tailLock);
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
    free(q);
    return 0;
}
