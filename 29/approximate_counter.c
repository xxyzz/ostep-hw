#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#define NUMTHREADS 4
int threads[NUMTHREADS];

typedef struct __counter_t {
    int             global;            // global count
    pthread_mutex_t glock;             // global lock
    int             local[NUMTHREADS];    // local count (per CPU)
    pthread_mutex_t llock[NUMTHREADS];    // ... and locks
    int             threshold;         // update frequency
} counter_t;

typedef struct __myarg_t {
    counter_t *c;
    int       threshold;
    int       amt;
} myarg_t;


// init: record threadhold, init locks, init values
//       of all local counts and global count
void init(counter_t *c, int threshold) {
    c->threshold = threshold;
    c->global = 0;
    pthread_mutex_init(&c->glock, NULL);
    int i;
    for(i = 0; i < NUMTHREADS; i++) {
        c->local[i] = 0;
        pthread_mutex_init(&c->llock[i], NULL);
    }
}

// get: just return global amount (which may not be perfect)
int get(counter_t *c) {
    pthread_mutex_lock(&c->glock);
    int val = c->global;
    pthread_mutex_unlock(&c->glock);
    return val; // only approximate!
}

// update: usually, just grab local lock and update local amount
//         once local count has risen by ’threshold’, grab global
//         lock and transfer local values to it
void update(counter_t *c, int threadID, int amt) {
    int thread = 0;
    for(int i = 0; i < NUMTHREADS; i++) {
        if (threads[i] == threadID) {
            thread = i;
            break;
        }
        if (threads[i] == 0) {
            threads[i] = threadID;
            thread = i;
            break;
        }
    }

    pthread_mutex_lock(&c->llock[thread]);
    c->local[thread] += amt;                  // assumes amt > 0
    if (c->local[thread] >= c->threshold) {   // transfer to global
        pthread_mutex_lock(&c->glock);
        c->global += c->local[thread];
        pthread_mutex_unlock(&c->glock);
        c->local[thread] = 0;
    }
    printf("Thread %d local count: %d, gobal count: %d\n", thread, c->local[thread], get(c));
    pthread_mutex_unlock(&c->llock[thread]);
}

void *thread_function(void *arg) {
    myarg_t *m = (myarg_t *) arg;
    pthread_t threadID = pthread_self();
    int i;
    for(i = 0; i < 6; i++) {
        update(m->c, (int) threadID, m->amt);
    }
    pthread_exit(0);
}

int main(int argc, char *argv[]) {
    int threshold = 5;
    counter_t *c;
    c = calloc(1, sizeof(counter_t));
    init(c, threshold);
    pthread_t p1, p2, p3, p4;
    myarg_t args;
    args.c = c;
    args.threshold = threshold;
    args.amt = 1;
    pthread_create(&p1, NULL, &thread_function, &args);
    pthread_create(&p2, NULL, &thread_function, &args);
    pthread_create(&p3, NULL, &thread_function, &args);
    pthread_create(&p4, NULL, &thread_function, &args);
    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    pthread_join(p3, NULL);
    pthread_join(p4, NULL);
    return 0;
}
