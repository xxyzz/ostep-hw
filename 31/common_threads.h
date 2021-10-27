#ifndef __common_threads_h__
#define __common_threads_h__

#include <assert.h>
#include <fcntl.h> // For O_* constants
#include <pthread.h>
#include <sched.h>
#include <semaphore.h>
#include <sys/stat.h> // For mode constants

#define Pthread_create(thread, attr, start_routine, arg)                       \
  assert(pthread_create(thread, attr, start_routine, arg) == 0)
#define Pthread_join(thread, value_ptr)                                        \
  assert(pthread_join(thread, value_ptr) == 0)

#define Pthread_mutex_lock(m) assert(pthread_mutex_lock(m) == 0)
#define Pthread_mutex_unlock(m) assert(pthread_mutex_unlock(m) == 0)
#define Pthread_cond_signal(cond) assert(pthread_cond_signal(cond) == 0)
#define Pthread_cond_wait(cond, mutex)                                         \
  assert(pthread_cond_wait(cond, mutex) == 0)

#define Mutex_init(m) assert(pthread_mutex_init(m, NULL) == 0)
#define Mutex_lock(m) assert(pthread_mutex_lock(m) == 0)
#define Mutex_unlock(m) assert(pthread_mutex_unlock(m) == 0)
#define Cond_init(cond) assert(pthread_cond_init(cond, NULL) == 0)
#define Cond_signal(cond) assert(pthread_cond_signal(cond) == 0)
#define Cond_wait(cond, mutex) assert(pthread_cond_wait(cond, mutex) == 0)

#define Sem_wait(sem) assert(sem_wait(sem) == 0)
#define Sem_post(sem) assert(sem_post(sem) == 0)
#define Sem_close(sem) assert(sem_close(sem) == 0)
#define Sem_unlink(sem) assert(sem_unlink(sem) == 0)

sem_t *Sem_open(char *sem, int value) {
  sem_t *newSem;
  newSem = sem_open(sem, O_CREAT, S_IRWXU, value);
  assert(newSem != SEM_FAILED);
  return newSem;
}

#endif // __common_threads_h__
