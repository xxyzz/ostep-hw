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

// basic list structure (one used per list)
typedef struct __list_t {
    node_t *head;
} list_t;

typedef struct __myarg_t {
    list_t *list;
} myarg_t;

void List_Init(list_t *L) {
    node_t *new = malloc(sizeof(node_t));
    if (new == NULL) {
        perror("malloc");
        return;
    }
    pthread_mutex_init(&new->lock, NULL);
    new->next = NULL;
    new->key = 0;
    L->head = new;
}

void List_Insert(list_t *L, int key) {
    // synchronization not needed
    node_t *new = malloc(sizeof(node_t));
    if (new == NULL) {
        perror("malloc");
        return;
    }
    new->key = key;
    pthread_mutex_init(&new->lock, NULL);

    // just lock critical section
    pthread_mutex_lock(&L->head->lock);
    new->next = L->head;
    L->head   = new;
    pthread_mutex_unlock(&L->head->next->lock);
}

int List_Lookup(list_t *L, int key) {
    int rv = -1;
    pthread_mutex_lock(&L->head->lock);
    node_t *curr = L->head;
    while (curr->next != NULL) {
        if (curr->key == key) {
            rv = 0;
            break;
        }
        pthread_mutex_t *tempLock = &curr->lock;
        curr = curr->next;
        pthread_mutex_lock(&curr->lock);
        pthread_mutex_unlock(tempLock);
    }
    pthread_mutex_unlock(&curr->lock);
    return rv; // now both success and failure
}

void List_Print(list_t *L) {
    pthread_mutex_lock(&L->head->lock);
    node_t *curr = L->head;
    while (curr->next != NULL) {
        printf("%d\n", curr->key);
        pthread_mutex_t *tempLock = &curr->lock;
        curr = curr->next;
        pthread_mutex_lock(&curr->lock);
        pthread_mutex_unlock(tempLock);
    }
    pthread_mutex_unlock(&curr->lock);
}

void *thread_function(void *args) {
    myarg_t *m = (myarg_t *) args;
    for(int i = 0; i < 10; i++) {
        List_Insert(m->list, i);
    }
    // for(int j = 0; j < 100; j++) {
    //     List_Lookup(m->list, 1);
    // }
    List_Print(m->list);
    pthread_exit(0);
}

int main(int argc, char *argv[]) {
    for(int i = 1; i < 11; i++) {
        list_t *list = calloc(1, sizeof(list_t));
        List_Init(list);
        myarg_t args;
        args.list = list;
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
        // printf("Find 0: %d\n", List_Lookup(list, 0));
        free(list);
    }
    return 0;
}
