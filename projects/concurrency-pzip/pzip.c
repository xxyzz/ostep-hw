#include <stdio.h>
#include <stdlib.h>    // exit
#include <string.h>    // mmecpy
#include <arpa/inet.h> // htonl
#include <unistd.h>    // sysconf
#include <pthread.h>
#include <sys/mman.h>  // mmap
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "thread_helper.h"
#include "rwlock.h"

void *
compress(void *arg)
{
    struct queue *job_queue = (struct queue *) arg;
    pthread_t tid = pthread_self();
    struct job *jp = NULL;
    while (1) {
        do {
            jp = job_find(job_queue, tid);
        } while (jp == NULL);

        char oldBuff;
        int count = 0;
        for (size_t i = 0; i < jp->chunk_size; i++) {
            char character = jp->addr[i];
            if (character == oldBuff) {
                count++;
            } else {
                if (oldBuff != '\0')
                    append_result(jp->r_queue, count, oldBuff);
                count = 1;
                oldBuff = character;
            }
        }
        if (jp->r_queue->q_head == NULL && count > 0)
            append_result(jp->r_queue, count, oldBuff);    // same characters
        jp->j_id = NULL;
    }
}

// Littleendian and Bigendian byte order illustrated
// https://dflund.se/~pi/endian.html
void
writeFile(int count, char *oldBuff)
{
    count = htonl(count);    // write as network byte order
    fwrite(&count, 4, 1, stdout);
    fwrite(oldBuff, 1, 1, stdout);
}

void
writeResult(struct result *result)
{
    while (result != NULL) {
        int count = htonl(result->count);
        fwrite(&count, 4, 1, stdout);
        fwrite(&result->character, 1, 1, stdout);
        result = result->next;
    }
}

int
main(int argc, char *argv[])
{
    off_t page_size = sysconf(_SC_PAGE_SIZE);
    struct queue job_queue;

    if (argc <= 1) {
        fprintf(stderr, "pzip: file1 [file2 ...]\n");
        exit(EXIT_FAILURE);
    }

    // get_nprocs is GNU extension
    int np = sysconf(_SC_NPROCESSORS_ONLN);
    pthread_t threads[np];

    // create workers
    for (size_t i = 0; i < np; i++)
        Pthread_create(&threads[i], NULL, &compress, &job_queue);

    // create jobs
    queue_init(&job_queue);

    for (size_t i = 1; i < argc; i++) {
        int fd = open(argv[i], O_RDONLY);
        struct stat sb;
        if (fd == -1)
            handle_error("open");

        if (fstat(fd, &sb) == -1)
            handle_error("stat");

        int offset = 0;
        int worker = 0;
        while (offset < sb.st_size) {
            struct job new_job;
            new_job.j_id = threads[worker++];
            if (worker >= np)
                worker = 0;
            new_job.offset = offset;
            new_job.chunk_size = page_size;

            offset += page_size;
            if (offset > sb.st_size)
               new_job.chunk_size = sb.st_size - offset + page_size;
            
            char *addr = mmap(NULL, new_job.chunk_size, PROT_READ, MAP_PRIVATE, fd, new_job.offset);
            if (addr == MAP_FAILED)
                handle_error("mmap");

            new_job.addr = addr;
            job_append(&job_queue, &new_job);
        }
        
        close(fd);
    }

    // check jobs are done
    while (1) {
        if (check_job_done(&job_queue) == 0)
            continue;

        // kill workers
        for (size_t i = 0; i < np; i++)
            Pthread_cancel(threads[i]);
        
        // collect results
        pthread_rwlock_rdlock(&job_queue.q_lock);
        struct job *jp;
        int last_count;
        char last_character;
        struct result_queue *last_result;
        for (jp = job_queue.q_head; jp != NULL; jp = jp->j_next) {
            int count = jp->r_queue->q_head->count;
            char first_character = jp->r_queue->q_head->character;
            if (last_character == first_character) {
                last_result->q_tail->prev->next = last_result->q_tail;
                jp->r_queue->q_head = jp->r_queue->q_head->next;
                writeResult(last_result->q_head);
                writeFile(count + last_count, &last_character);
            } else {
                writeResult(last_result->q_head);
            }
            last_count = jp->r_queue->q_tail->count;
            last_character = jp->r_queue->q_tail->character;
            last_result = jp->r_queue;
        }
        writeResult(last_result->q_head);
        pthread_rwlock_unlock(&job_queue.q_lock);
        pthread_rwlock_destroy(&job_queue.q_lock);
        break;
    }

    return 0;
}
