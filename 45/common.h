#ifndef __COMMON_h__
#define __COMMON_h__

#include <stdio.h>
#include <sys/time.h>
#include <assert.h>

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

// Simple routine to return absolute time (in seconds).
double Time_GetSeconds() {
    struct timeval t;
    int rc = gettimeofday(&t, NULL);
    assert(rc == 0);
    return (double) ((double)t.tv_sec + (double)t.tv_usec / 1e6);
}

#endif
