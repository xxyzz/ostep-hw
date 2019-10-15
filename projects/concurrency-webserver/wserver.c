#include <stdio.h>
#include "request.h"
#include "io_helper.h"
#include "thread_helper.h"
#include <limits.h>    // LLONG_MAX

char default_root[] = ".", default_schedalg[] = "FIFO";
Zem_t full, empty, mutex;
int use = 0, buffers = 1;
char *schedalg = default_schedalg;

void *
workerThread (void * arg) {
	while (1) {
		Zem_wait(&full);
		Zem_wait(&mutex);
		Buffer_t *reqBuf = (Buffer_t *) arg;
		int useCopy = use;
		if (strcmp(schedalg, "SFF") == 0) {
			size_t i;
			off_t min_size = LLONG_MAX;
			for (i = 0; i < buffers; i++) {
				if (!reqBuf[i].handling && reqBuf[i].fd && reqBuf[i].size < min_size) {
					min_size = reqBuf[i].size;
					useCopy = i;
				}
			}
			reqBuf[useCopy].handling = 1;
		} else {
			use = (use + 1) % buffers;
		}
		Zem_post(&mutex);
		if (reqBuf[useCopy].is_static) {
			request_serve_static(reqBuf[useCopy].fd, reqBuf[useCopy].pathname, reqBuf[useCopy].size);
		} else {
			request_serve_dynamic(reqBuf[useCopy].fd, reqBuf[useCopy].pathname, reqBuf[useCopy].cgiargs);
		}
		close(reqBuf[useCopy].fd);
		Zem_post(&empty);
	}
}

//
// ./wserver [-d <basedir>] [-p <portnum>] [-t threads] [-b buffers] [-s schedalg]
// 
int main(int argc, char *argv[]) {
    int c, fill;
    char *root_dir = default_root;
    int port = 10000, threads = 1;
    
    while ((c = getopt(argc, argv, "d:p:t:b:s:")) != -1)
		switch (c) {
			case 'd':
				root_dir = optarg;
				break;
			case 'p':
				port = atoi(optarg);
				break;
			case 't':
				threads = atoi(optarg);
				break;
			case 'b':
				buffers = atoi(optarg);
				break;
			case 's':
				schedalg = optarg;
				break;
			default:
				fprintf(stderr, "usage: wserver [-d basedir] [-p port] \
					[-t threads] [-b buffers] [-s schedalg]\n");
				exit(1);
		}

	Buffer_t buffer[buffers];
	Zem_init(&full, 0);
	Zem_init(&empty, buffers);
	Zem_init(&mutex, 1);
	pthread_t threadsArr[threads];

	for (int i = 0; i < threads; i++)
		Pthread_create(&threadsArr[i], NULL, &workerThread, buffer);

    // run out of this directory
    chdir_or_die(root_dir);

    // now, get to work
    int listen_fd = open_listen_fd_or_die(port);
    while (1) {
		struct sockaddr_in client_addr;
		int client_len = sizeof(client_addr);
		Zem_wait(&empty);
		int conn_fd = accept(listen_fd, (sockaddr_t *) &client_addr, (socklen_t *) &client_len);
		if (conn_fd == -1) {
			Zem_post(&empty);
			continue;
		}
		Zem_wait(&mutex);

		if (pre_handle_request(conn_fd, &buffer[fill]) != OK) {
			Zem_post(&mutex);
			Zem_post(&empty);
			continue;
		}

		fill = (fill + 1) % buffers;
		Zem_post(&mutex);
		Zem_post(&full);
    }
    return 0;
}
