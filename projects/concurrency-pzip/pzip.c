#include <stdio.h>
#include <stdlib.h>    // exit
#include <string.h>
#include <arpa/inet.h> // htonl
#include <unistd.h>    // sysconf
#include <pthread.h>
#include <sys/mman.h>  // mmap
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "thread_helper.h"
#include "rwlock.h"

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

void *
compress(void *arg)
{

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
        Pthread_create(&threads[i], NULL, &compress, NULL);

    // create jobs
    queue_init(&job_queue);

    for (size_t i = 1; i < argc; i++) {
        int fd;
        struct stat sb;
        if ((fd = open(argv[i], O_RDONLY) == -1))
            handle_error("open");

        if (fstat(argv[i], &sb) == -1)
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
               new_job.chunk_size = sb.st_size - offset - page_size;
            
            char *addr;
            if ((addr = mmap(NULL, new_job.chunk_size, PROT_READ, MAP_PRIVATE, fd, new_job.offset)) == NULL)
                handle_error("mmap");

            new_job.addr = addr;
            job_append(&job_queue, &new_job);
        }
        
        close(fd);
    }

    // check jobs are done
    while (1) {
        pthread_rwlock_rdlock(&job_queue.q_lock);
        if (job_queue.q_head != NULL) {
            pthread_rwlock_unlock(&job_queue.q_lock);
            continue;
        }

        pthread_rwlock_unlock(&job_queue.q_lock);
        // kill workers
        // collect results

        pthread_rwlock_destroy(&job_queue.q_lock);
        break;
    }

    return 0;
}
