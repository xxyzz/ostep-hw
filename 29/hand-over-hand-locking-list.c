#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>
#define ONE_MILLION 1000000

// basic node structure
typedef struct __node_t {
    int             key;
    struct __node_t *next;
    pthread_mutex_t lock;
} node_t;

typedef struct __myarg_t {
    node_t *head;
} myarg_t;

void List_Init(node_t *head) {
    pthread_mutex_init(&head->lock, NULL);
    head->next = NULL;
}

void List_Insert(node_t *head, int key) {
    // synchronization not needed
    node_t *new = malloc(sizeof(node_t));
    if (new == NULL) {
        perror("malloc");
        return;
    }
    new->key = key;
    pthread_mutex_init(&new->lock, NULL);

    // just lock critical section
    pthread_mutex_lock(&head->lock);
    pthread_mutex_lock(&new->lock);
    new->next = head;
    head      = new;
    pthread_mutex_unlock(&new->next->lock);
    pthread_mutex_unlock(&new->lock);
}

int List_Lookup(node_t *head, int key) {
    int rv = -1;
    pthread_mutex_lock(&head->lock);
    node_t *curr = head;
    while (curr) {
        if (curr->key == key) {
            rv = 0;
            pthread_mutex_unlock(&curr->lock);
            break;
        }
        pthread_mutex_lock(&curr->next->lock);
        pthread_mutex_t *tempLock = &curr->lock;
        curr = curr->next;
        pthread_mutex_unlock(tempLock);
    }
    return rv; // now both success and failure
}

void *thread_function(void *args) {
    myarg_t *m = (myarg_t *) args;
    for(int i = 0; i < ONE_MILLION / 200; i++) {
        List_Insert(m->head, i);
    }
    for(int j = 0; j < ONE_MILLION / 200; j++) {
        List_Lookup(m->head, 1);
    }
    pthread_exit(0);
}

int main(int argc, char *argv[]) {
    for(int i = 1; i < 11; i++) {
        node_t *head = calloc(1, sizeof(node_t));
        List_Init(head);
        myarg_t args;
        args.head = head;
        pthread_t threads[i];
        struct timeval start, end;
        gettimeofday(&start, NULL);
        for(int j = 0; j < i; j++) {
            pthread_create(&threads[j], NULL, &thread_function, &args);
        }
        for(int k = 0; k < i; k++) {
            pthread_join(threads[k], NULL);
        }
        gettimeofday(&end, NULL);
        printf("%d threads, time (seconds): %f\n\n", i,
            (float) (end.tv_usec - start.tv_usec + (end.tv_sec - start.tv_sec) * ONE_MILLION) / ONE_MILLION);
        free(head);
    }
    return 0;
}
