#include "thread_helper.h"
#include <arpa/inet.h> // htonl
#include <fcntl.h>     // open, O_* constants
#include <stdio.h>     // fwrite, fprintf
#include <stdlib.h>    // exit, malloc
#include <sys/fcntl.h>
#include <sys/mman.h> // mmap, munmap
#include <sys/stat.h> // fstat, mode constants
#include <sys/types.h>
#include <unistd.h> // sysconf, close

// http://www.catb.org/esr/structure-packing/
// https://en.wikipedia.org/wiki/Data_structure_alignment
typedef struct result {
  struct result *next;
  int count;
  char character;
  char pad[sizeof(struct result *) - sizeof(int) - 1];
} Result;

typedef struct work {
  long long chunk_size;
  char *addr;
  Result *results;
} Work;

typedef struct files {
  int fd;
  char pad[sizeof(off_t) - sizeof(int)];
  off_t size;
} Files;

static long long use_ptr = 0, fill_ptr = 0, chunks = 0;
static sem_t mutex, empty, full;

static Result *create_result(int count, char character) {
  Result *result = malloc(sizeof(Result));
  if (result == NULL)
    handle_error("malloc");
  result->count = count;
  result->character = character;
  result->next = NULL;
  return result;
}

static void *compress(void *arg) {
  Work *works = (Work *)arg;

  while (1) {
    // use semaphore instead of mutex and condition variables
    // because workers will wait for the mutex and
    // pthread_mutex_lock() is not a cancellation point,
    // therefore the main thread can't join the workers
    Sem_wait(&full);
    Sem_wait(&mutex);

    // get work
    Work *current_work = &works[use_ptr];
    use_ptr = (use_ptr + 1) % chunks;

    // do work
    Result *head = NULL;
    Result *previous_result = NULL;
    char previous_character = '\0';
    int previous_count = 0;
    for (long long i = 0; i < current_work->chunk_size; i++) {
      char character = current_work->addr[i];
      if (character == previous_character) {
        previous_count++;
      } else {
        if (previous_count != 0) {
          Result *last_result =
              create_result(previous_count, previous_character);
          if (previous_result != NULL)
            previous_result->next = last_result;
          previous_result = last_result;
          if (head == NULL)
            head = previous_result;
        }
        previous_count = 1;
        previous_character = character;
      }
    }
    if (head == NULL) {
      // same characters
      current_work->results = create_result(previous_count, previous_character);
    } else {
      current_work->results = head;
      previous_result->next = create_result(previous_count, previous_character);
    }

    Sem_post(&mutex);
    Sem_post(&empty);
  }
}

// Littleendian and Bigendian byte order illustrated
// https://dflund.se/~pi/endian.html
static void writeFile(int character_count, char *oldBuff) {
  character_count = htonl(character_count); // write as network byte order
  fwrite(&character_count, sizeof(int), 1, stdout);
  fwrite(oldBuff, sizeof(char), 1, stdout);
}

int main(int argc, char *argv[]) {
  long page_size = sysconf(_SC_PAGE_SIZE);

  if (argc <= 1) {
    fprintf(stdout, "pzip: file1 [file2 ...]\n");
    exit(EXIT_FAILURE);
  }

  // get_nprocs is GNU extension
  long np = sysconf(_SC_NPROCESSORS_ONLN);
  pthread_t *threads = malloc(sizeof(pthread_t) * (unsigned long)np);
  if (threads == NULL)
    handle_error("malloc");

  Files *files = malloc(sizeof(Files) * (unsigned long)(argc - 1));
  if (files == NULL)
    handle_error("malloc");

  // count chunks number
  for (int i = 1; i < argc; i++) {
    int fd = open(argv[i], O_RDONLY);
    struct stat sb;
    if (fd == -1)
      handle_error("open");

    if (fstat(fd, &sb) == -1)
      handle_error("stat");

    files[i - 1].fd = fd;
    files[i - 1].size = sb.st_size;

    chunks += (sb.st_size / page_size + 1);
  }

  // init semaphores
  Sem_init(&mutex, 0, 1);
  // set empty to 1 to prevent main thread cancel
  // workers before they do the work
  Sem_init(&empty, 0, 1);
  Sem_init(&full, 0, 0);

  Work *works = malloc(sizeof(Work) * (unsigned long)chunks);
  if (works == NULL)
    handle_error("malloc");

  // create workers
  for (long i = 0; i < np; i++)
    Pthread_create(&threads[i], NULL, compress, works);

  // create jobs
  for (int i = 0; i < argc - 1; i++) {
    long long offset = 0;
    while (offset < files[i].size) {
      Sem_wait(&empty);
      Sem_wait(&mutex);

      works[fill_ptr].chunk_size = page_size;
      if (offset + page_size > files[i].size)
        works[fill_ptr].chunk_size = files[i].size - offset;

      char *addr = mmap(NULL, (size_t)works[fill_ptr].chunk_size, PROT_READ,
                        MAP_PRIVATE, files[i].fd, offset);
      if (addr == MAP_FAILED)
        handle_error("mmap");

      works[fill_ptr].addr = addr;
      works[fill_ptr].results = NULL;
      offset += page_size;
      fill_ptr = (fill_ptr + 1) % chunks;

      Sem_post(&mutex);
      Sem_post(&full);
    }
    close(files[i].fd);
  }

  // check jobs are done
  Sem_wait(&empty);
  Sem_wait(&mutex);

  // kill and wait workers
  for (long i = 0; i < np; i++) {
    Pthread_cancel(threads[i]);
    Pthread_join(threads[i], NULL);
  }

  // final compress
  int last_count = 0;
  char last_character = '\0';
  for (long long i = 0; i < chunks; i++) {
    Result *result;
    result = works[i].results;
    while (result != NULL) {
      if (result == works[i].results &&
          result->next != NULL) { // first but not last result
        if (result->character == last_character) {
          writeFile(result->count + last_count, &result->character);
        } else {
          if (last_count > 0)
            writeFile(last_count, &last_character);
          writeFile(result->count, &result->character);
        }
      } else if (result->next == NULL) {  // last result
        if (result != works[i].results) { // not first
          if (i == chunks - 1) {          // last chunk
            writeFile(result->count, &result->character);
          } else { // not last chunk
            last_character = result->character;
            last_count = result->count;
          }
        } else {                                     // first result
          if (result->character == last_character) { // same
            if (i != chunks - 1) {                   // not last chunk
              last_count += result->count;
            } else {
              writeFile(result->count + last_count, &result->character);
            }
          } else { // not same
            if (last_count > 0)
              writeFile(last_count, &last_character);
            if (i != chunks - 1) {
              last_character = result->character;
              last_count = result->count;
            } else {
              writeFile(result->count, &result->character);
            }
          }
        }
      } else {
        writeFile(result->count, &result->character);
      }

      Result *tmp = result;
      result = result->next;
      free(tmp);
    }
    if (munmap(works[i].addr, (size_t)works[i].chunk_size) != 0)
      handle_error("munmap");
  }
  Sem_post(&mutex);

  free(threads);
  free(files);
  free(works);
  Sem_destroy(&mutex);
  Sem_destroy(&full);
  Sem_destroy(&empty);

  return 0;
}
