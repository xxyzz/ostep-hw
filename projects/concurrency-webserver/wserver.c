#include <stdio.h>
#include "request.h"
#include "io_helper.h"
#include "thread_helper.h"

char default_root[] = ".", default_schedalg[] = "FIFO";
Zem_t full, empty, mutex;
int fill = 0, use = 0, buffers = 1;

void *
workerThread (void * arg) {
	while (1) {
		Zem_wait(&full);
		Zem_wait(&mutex);
		Buffer_t *reqBuf = (Buffer_t *) arg;
		int useCopy = use;
		use = (use + 1) % buffers;
		Zem_post(&mutex);
		if (reqBuf[useCopy].is_static) {
			request_serve_static(reqBuf[useCopy].fd, reqBuf[useCopy].pathname, reqBuf[useCopy].size);
		} else {
			request_serve_dynamic(reqBuf[useCopy].fd, reqBuf[useCopy].pathname, reqBuf[useCopy].cgiargs);
		}
		close_or_die(reqBuf[useCopy].fd);
		Zem_post(&empty);
	}
}

//
// ./wserver [-d <basedir>] [-p <portnum>] [-t threads] [-b buffers] [-s schedalg]
// 
int main(int argc, char *argv[]) {
    int c;
    char *root_dir = default_root;
    int port = 10000, threads = 1;
	char *schedalg = default_schedalg;
    
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
		Zem_wait(&mutex);
		int conn_fd = accept_or_die(listen_fd, (sockaddr_t *) &client_addr, (socklen_t *) &client_len);
		buffer[fill].fd = conn_fd;

		if (pre_handle_request(conn_fd, &buffer[fill]) != OK) {
			Zem_post(&mutex);
			Zem_post(&empty);
			continue;
		}

		fill = (fill + 1) % buffers;
		// sort request by file size if the scheduling policy is SFF when the buffer is full
		if (fill == 0 && strcmp(schedalg, "SFF") == 0) {

		}
		Zem_post(&mutex);
		Zem_post(&full);
    }
    return 0;
}
