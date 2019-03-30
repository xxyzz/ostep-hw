#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>
#define NUMTHREADS 4
#define ONE_MILLION 1000000
int threadsID[NUMTHREADS];

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
    int       threads;
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
        if (threadsID[i] == threadID) {
            thread = i;
            break;
        }
        if (threadsID[i] == 0) {
            threadsID[i] = threadID;
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
    pthread_mutex_unlock(&c->llock[thread]);
}

void *thread_function(void *arg) {
    myarg_t *m = (myarg_t *) arg;
    pthread_t threadID = pthread_self();
    int i;
    for(i = 0; i < ONE_MILLION / m->threads; i++) {
        update(m->c, (int) threadID, m->amt);
    }
    pthread_exit(0);
}

int main(int argc, char *argv[]) {
    for (int i = 1; i < 6; i++) {
        int threshold = i;
        for (int j = 1; j <= NUMTHREADS; j++) {
            counter_t *c;
            c = calloc(1, sizeof(counter_t));
            init(c, threshold);
            pthread_t threads[j];
            myarg_t args;
            args.c = c;
            args.threshold = threshold;
            args.amt = 1;
            args.threads = j;
            struct timeval start, end;
            gettimeofday(&start, NULL);
            for (int k = 0; k < j; k++) {
                pthread_create(&threads[k], NULL, &thread_function, &args);
            }
            for (int l = 0; l < j; l++) {
                pthread_join(threads[l], NULL);
            }
            gettimeofday(&end, NULL);
            printf("%d threads, %d threshold\n", j, threshold);
            printf("Time (seconds): %f\n\n", (float) (end.tv_usec - start.tv_usec + (end.tv_sec - start.tv_sec) * ONE_MILLION) / ONE_MILLION);
            free(c);
        }
    }
    return 0;
}
