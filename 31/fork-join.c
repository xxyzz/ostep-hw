#include <stdio.h>
#include <unistd.h>
#include "common_threads.h"

sem_t * s; 

void *child(void *arg) {
    sleep(1);
    printf("child\n");
    // use semaphore here
    Sem_post(s);
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t p;
    char * semaphoreName = "/fork-join-semaphore";
    printf("parent: begin\n");
    // init semaphore here
    s = Sem_open(semaphoreName, 0);
    Pthread_create(&p, NULL, child, NULL);
    // use semaphore here
    Sem_wait(s);
    Sem_close(s);
    Sem_unlink(semaphoreName);
    printf("parent: end\n");
    return 0;
}

