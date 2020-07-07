#ifndef _thread_helper_h
#define _thread_helper_h

#ifdef FreeBSD
#include <pthread_np.h>
#endif
#include <assert.h>
#include <errno.h>
#include <pthread.h>

#define handle_error_en(en, msg)                                               \
  do {                                                                         \
    errno = en;                                                                \
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
#define Pthread_setaffinity_np(thread, cpusetsize, cpuset)                     \
  assert(pthread_setaffinity_np(thread, cpusetsize, cpuset) == 0)

#endif // _thread_helper_h
