//
// client.c: A very, very primitive HTTP client.
// 
// To run, try: 
//      client hostname portnumber filename
//
// Sends one HTTP request to the specified HTTP server.
// Prints out the HTTP response.
//
// For testing your server, you will want to modify this client.  
// For example:
// You may want to make this multi-threaded so that you can 
// send many requests simultaneously to the server.
//
// You may also want to be able to request different URIs; 
// you may want to get more URIs from the command line 
// or read the list from a file. 
//
// When we test your server, we will be using modifications to this client.
//

#include "io_helper.h"
#include "thread_helper.h"
#include <stdlib.h>    // malloc

#define MAXBUF (8192)

char *host;
int port;

//
// Send an HTTP request for the specified file 
//
void client_send(int fd, char *filename) {
    char buf[MAXBUF];
    char hostname[MAXBUF / 10];
    
    gethostname_or_die(hostname, MAXBUF);
    
    /* Form and send the HTTP request */
    sprintf(buf, "GET %s HTTP/1.1\nhost: %s\n\r\n", filename, hostname);
    write_or_die(fd, buf, strlen(buf));
}

//
// Read the HTTP response and print it out
//
void client_print(int fd) {
    char buf[MAXBUF];  
    int n;
    
    // Read and display the HTTP Header 
    n = readline_or_die(fd, buf, MAXBUF);
    while (strcmp(buf, "\r\n") && (n > 0)) {
        printf("Header: %s", buf);
        n = readline_or_die(fd, buf, MAXBUF);
        
        // If you want to look for certain HTTP tags... 
        // int length = 0;
        //if (sscanf(buf, "Content-Length: %d ", &length) == 1) {
        //    printf("Length = %d\n", length);
        //}
    }
    
    // Read and display the HTTP Body 
    n = readline_or_die(fd, buf, MAXBUF);
    while (n > 0) {
        printf("%s", buf);
        n = readline_or_die(fd, buf, MAXBUF);
    }
}

void *
send_request(void * arg) {
    char *filename = (char *) arg;

    /* Open a single connection to the specified host and port */
    int clientfd = open_client_fd_or_die(host, port); 
    client_send(clientfd, filename);
    client_print(clientfd);
    close_or_die(clientfd);
    return NULL;
}

int main(int argc, char *argv[]) {
    char *filename;
    int threads;
    
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <host> <port> <filename> <threads>\n", argv[0]);
        exit(1);
    }
    
    host = argv[1];
    port = atoi(argv[2]);
    filename = argv[3];
    threads = atoi(argv[4]);
    pthread_t threadsArr[threads];
    char *filenames[threads - 1];

    Pthread_create(&threadsArr[0], NULL, &send_request, "/spin.cgi?1");
    for (size_t i = 0; i < threads - 1; i++) {
        filenames[i] = malloc(MAXBUF);
        sprintf(filenames[i], "%s%zu", filename, threads - 1 - i);
    }

    for (size_t i = 1; i < threads; i++)
        Pthread_create(&threadsArr[i], NULL, &send_request, filenames[i - 1]);

    for (size_t i = 0; i < threads; i++)
        Pthread_join(threadsArr[i], NULL);

    for (size_t i = 0; i < threads - 1; i++)
        free(filenames[i]);

    exit(0);
}
