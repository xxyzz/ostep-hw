#ifndef _thread_helper_h
#define _thread_helper_h

#include <assert.h>
#include <pthread.h>
#include <semaphore.h>

#define handle_error(msg)                                                      \
  do {                                                                         \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

#define Pthread_create(thread, attr, start_routine, arg)                       \
  assert(pthread_create(thread, attr, start_routine, arg) == 0)
#define Pthread_join(thread, value_ptr)                                        \
  assert(pthread_join(thread, value_ptr) == 0)
#define Pthread_cancel(thread) assert(pthread_cancel(thread) == 0)
#define Pthread_mutex_init(m, attr) assert(pthread_mutex_init(m, attr) == 0)
#define Pthread_mutex_lock(m) assert(pthread_mutex_lock(m) == 0)
#define Pthread_mutex_unlock(m) assert(pthread_mutex_unlock(m) == 0)
#define Pthread_mutex_destroy(m) assert(pthread_mutex_destroy(m) == 0)
#define Pthread_cond_init(cond, attr) assert(pthread_cond_init(cond, attr) == 0)
#define Pthread_cond_signal(cond) assert(pthread_cond_signal(cond) == 0)
#define Pthread_cond_wait(cond, mutex)                                         \
  assert(pthread_cond_wait(cond, mutex) == 0)
#define Pthread_cond_destroy(cond) assert(pthread_cond_destroy(cond) == 0)
#define Pthread_setcancelstate(state, oldstate)                                \
  assert(pthread_setcancelstate(state, oldstate) == 0)
#define Pthread_setcanceltype(type, oldtype)                                   \
  assert(pthread_setcanceltype(type, oldtype) == 0)
#define Mutex_init(m) assert(pthread_mutex_init(m, NULL) == 0)
#define Mutex_lock(m) assert(pthread_mutex_lock(m) == 0)
#define Mutex_unlock(m) assert(pthread_mutex_unlock(m) == 0)
#define Cond_init(cond) assert(pthread_cond_init(cond, NULL) == 0)
#define Cond_signal(cond) assert(pthread_cond_signal(cond) == 0)
#define Cond_wait(cond, mutex) assert(pthread_cond_wait(cond, mutex) == 0)
#define Sem_init(sem, pshared, value) assert(sem_init(sem, pshared, value) == 0)
#define Sem_wait(sem) assert(sem_wait(sem) == 0)
#define Sem_post(sem) assert(sem_post(sem) == 0)
#define Sem_destroy(sem) assert(sem_destroy(sem) == 0)
#define Sem_close(sem) assert(sem_close(sem) == 0)
#define Sem_unlink(name) assert(sem_unlink(name) == 0)

#endif // _thread_helper_h
