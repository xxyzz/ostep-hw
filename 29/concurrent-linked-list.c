#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>
#define ONE_MILLION 1000000

// basic node structure
typedef struct __node_t {
    int             key;
    struct __node_t *next;
} node_t;

// basic list structure (one used per list)
typedef struct __list_t {
    node_t          *head;
    pthread_mutex_t lock;
} list_t;

typedef struct __myarg_t {
    list_t *L;
} myarg_t;

void List_Init(list_t *L) {
    L->head = NULL;
    pthread_mutex_init(&L->lock, NULL);
}

void List_Insert(list_t *L, int key) {
    // synchronization not needed
    node_t *new = malloc(sizeof(node_t));
    if (new == NULL) {
        perror("malloc");
        return;
    }
    new->key = key;

    // just lock critical section
    pthread_mutex_lock(&L->lock);
    new->next = L->head;
    L->head   = new;
    pthread_mutex_unlock(&L->lock);
}

int List_Lookup(list_t *L, int key) {
    int rv = -1;
    pthread_mutex_lock(&L->lock);
    node_t *curr = L->head;
    while (curr) {
        if (curr->key == key) {
            rv = 0;
            break;
        }
        curr = curr->next;
    }
    pthread_mutex_unlock(&L->lock);
    return rv; // now both success and failure
}

void *thread_function(void *args) {
    myarg_t *m = (myarg_t *) args;
    for(int i = 0; i < ONE_MILLION / 200; i++) {
        List_Insert(m->L, i);
    }
    for(int j = 0; j < ONE_MILLION / 200; j++) {
        List_Lookup(m->L, 1);
    }
    pthread_exit(0);
}

int main(int argc, char *argv[]) {
    for(int i = 1; i < 11; i++) {
        list_t *list = calloc(1, sizeof(list_t));
        List_Init(list);
        myarg_t args;
        args.L = list;
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
        free(list);
    }
    return 0;
}
