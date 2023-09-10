#ifndef __common_threads_h__
#define __common_threads_h__

#include <assert.h>
#include <fcntl.h> // For O_* constants
#include <pthread.h>
#include <semaphore.h>
#include <sys/stat.h> // For mode constants
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define Pthread_create(thread, attr, start_routine, arg)                       \
  assert(pthread_create(thread, attr, start_routine, arg) == 0)
#define Pthread_join(thread, value_ptr)                                        \
  assert(pthread_join(thread, value_ptr) == 0)
#define Pthread_cancel(thread) assert(pthread_cancel(thread) == 0)

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

#ifndef __APPLE__
#define Sem_init(sem, pshared, value) assert(sem_init(sem, pshared, value) == 0)
#define Sem_destroy(sem) assert(sem_destroy(sem) == 0)
#endif
#define Sem_wait(sem) assert(sem_wait(sem) == 0)
#define Sem_post(sem) assert(sem_post(sem) == 0)
#define Sem_close(sem) assert(sem_close(sem) == 0)
#define Sem_unlink(sem) assert(sem_unlink(sem) == 0)

/*
If some programs created sem by sem_open but not destroyed it maybe due to the deadlock and Ctrl-C or others,
then the next time "Sem_open" will skip the create operation and not init as expected.
Here assume that sem is accidentally stored in /dev/shm/ with format like /dev/shm/sem.foo
*/
void check_shm(char *obj_name){
  char *shm_path = (char *)malloc(100*sizeof(char));
  strcpy(shm_path,"/dev/shm/sem.");
  strcat(shm_path,obj_name+1);
  if (access(shm_path,F_OK)==0)
    Sem_unlink(obj_name);
  free(shm_path);
}


sem_t *Sem_open(char *name, int value) {
  sem_t *sem;
  check_shm(name);
  /*
  "O_CREAT|O_EXCL" also fine. If "EEXIST" then we recall "Sem_open".
  This has more overheads when failure but less overheads when success.
  */
  sem = sem_open(name, O_CREAT, S_IRWXU, value);
  assert(sem != SEM_FAILED);
  return sem;
}

#endif // __common_threads_h__
