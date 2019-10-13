#ifndef __REQUEST_H__

#define MAXBUF (8192)

enum result {
    OK,
    NotFound,
    Forbidden
};

typedef struct __Buffer_t {
	int fd;
	int is_static;
	off_t size;
	char pathname[MAXBUF];
	char cgiargs[MAXBUF];
} Buffer_t;

void request_handle(int fd);
int pre_handle_request(int fd, Buffer_t *reqBuf);
void request_serve_dynamic(int fd, char *filename, char *cgiargs);
void request_serve_static(int fd, char *filename, int filesize);


#endif // __REQUEST_H__
