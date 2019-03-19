#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>
#define ONE_MILLION 1000000

typedef struct __counter_t {
    int             value;
    pthread_mutex_t lock;
} counter_t;

typedef struct __myarg_t {
    counter_t *c;
} myarg_t;

void init(counter_t *c) {
    c->value = 0;
    pthread_mutex_init(&c->lock, NULL);
}

void increment(counter_t *c) {
    pthread_mutex_lock(&c->lock);
    c->value++;
    pthread_mutex_unlock(&c->lock);
}

void decrement(counter_t *c) {
    pthread_mutex_lock(&c->lock);
    c->value--;
    pthread_mutex_unlock(&c->lock);
}

int get(counter_t *c) {
    pthread_mutex_lock(&c->lock);
    int rc = c->value;
    pthread_mutex_unlock(&c->lock);
    return rc;
}

void *thread_function(void *arg) {
    myarg_t *counter = (myarg_t *) arg;
    for (int i = 0; i < ONE_MILLION; i++) {
        increment(counter->c);
    }
    pthread_exit(0);
}

int main(int argc, char *argv[]) {
    pthread_t p1, p2, p3, p4;
    struct timeval start, end;
    counter_t *counter = malloc(sizeof(counter_t));
    if (counter == NULL) {
        return -1;
    }
    init(counter);
    myarg_t args;
    args.c = counter;
    gettimeofday(&start, NULL);
    pthread_create(&p1, NULL, &thread_function, &args);
    pthread_create(&p2, NULL, &thread_function, &args);
    pthread_create(&p3, NULL, &thread_function, &args);
    pthread_create(&p4, NULL, &thread_function, &args);
    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    pthread_join(p3, NULL);
    pthread_join(p4, NULL);
    gettimeofday(&end, NULL);
    printf("Counter value: %d\n", get(counter));
    printf("Time (microseconds): %d\n", (int) (end.tv_usec - start.tv_usec + (end.tv_sec - start.tv_sec) * ONE_MILLION));
    free(counter);
    return 0;
}
