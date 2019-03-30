#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#define ONE_MILLION 1000000

typedef struct __counter_t {
    int             value;
    pthread_mutex_t lock;
} counter_t;

typedef struct __myarg_t {
    counter_t *counter;
    cpu_set_t set;
    int       threads;
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
    myarg_t *m = (myarg_t *) arg;
    if (pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &m->set) == -1) {
        printf("Set CPU affinity error\n");
        pthread_exit(0);
    }
    for (int i = 0; i < ONE_MILLION / m->threads; i++) {
        increment(m->counter);
    }
    pthread_exit(0);
}

int main(int argc, char *argv[]) {
    cpu_set_t set;
    CPU_ZERO(&set);
    for (int i = 1; i < 5; i++) {
        for(int m = 0; m < i; m++) {
            CPU_SET(m, &set);
        }   
        printf("%d CPUs\n", i);
        for (int l = 1; l < 5; l++) {
            pthread_t threads[l - 1];
            struct timeval start, end;
            counter_t *counter = malloc(sizeof(counter_t));
            if (counter == NULL) {
                return -1;
            }
            init(counter);
            myarg_t args;
            args.counter = counter;
            args.set = set;
            args.threads = l;
            gettimeofday(&start, NULL);
            for(int j = 0; j < l; j++) {
                pthread_create(&threads[j], NULL, &thread_function, &args);
            }   
            for(int k = 0; k < l; k++) {
                pthread_join(threads[k], NULL);
            }
            gettimeofday(&end, NULL);
            printf("%d threads\n", l);
            printf("Time (seconds): %f\n\n", (float) (end.tv_usec - start.tv_usec + (end.tv_sec - start.tv_sec) * ONE_MILLION) / ONE_MILLION);
            free(counter);
        }
    }
    return 0;
}
