#include "io_helper.h"
#include "request.h"

//
// Some of this code stolen from Bryant/O'Halloran
// Hopefully this is not a problem ... :)
//

int request_error(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg) {
    char buf[MAXBUF], body[MAXBUF];
    
    // Create the body of error message first (have to know its length for header)
    sprintf(body, ""
	    "<!doctype html>\r\n"
	    "<head>\r\n"
	    "  <title>OSTEP WebServer Error</title>\r\n"
	    "</head>\r\n"
	    "<body>\r\n"
	    "  <h2>%s: %s</h2>\r\n" 
	    "  <p>%s: %s</p>\r\n"
	    "</body>\r\n"
	    "</html>\r\n", errnum, shortmsg, longmsg, cause);
    
    // Write out the header information for this response
    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
    if (write(fd, buf, strlen(buf)) == -1)
        return -1;
    
    sprintf(buf, "Content-Type: text/html\r\n");
    if (write(fd, buf, strlen(buf)) == -1)
        return -1;
    
    sprintf(buf, "Content-Length: %zu\r\n\r\n", strlen(body));
    if (write(fd, buf, strlen(buf)) == -1)
        return -1;
    
    // Write out the body last
    if (write(fd, body, strlen(body)) == -1)
        return -1;

    return 0;
}

//
// Reads and discards everything up to an empty text line
//
int request_read_headers(int fd) {
    char buf[MAXBUF];
    
    do {
        if (readline(fd, buf, MAXBUF) == -1)
            return -1;
    } while (strcmp(buf, "\r\n"));
    return 0;
}

//
// Return 1 if static, 0 if dynamic content
// Calculates filename (and cgiargs, for dynamic) from uri
//
int request_parse_uri(char *uri, char *filename, char *cgiargs) {
    char *ptr;
    
    if (!strstr(uri, "cgi")) { 
        // static
        strcpy(cgiargs, "");
        sprintf(filename, ".%s", uri);
        if (uri[strlen(uri)-1] == '/') {
            strcat(filename, "index.html");
        }
        return 1;
    } else { 
        // dynamic
        ptr = index(uri, '?');
        if (ptr) {
            strcpy(cgiargs, ptr+1);
            *ptr = '\0';
        } else {
            strcpy(cgiargs, "");
        }
        sprintf(filename, ".%s", uri);
        return 0;
    }
}

//
// Fills in the filetype given the filename
//
void request_get_filetype(char *filename, char *filetype) {
    if (strstr(filename, ".html")) 
	    strcpy(filetype, "text/html");
    else if (strstr(filename, ".gif")) 
	    strcpy(filetype, "image/gif");
    else if (strstr(filename, ".jpg")) 
	    strcpy(filetype, "image/jpeg");
    else 
	    strcpy(filetype, "text/plain");
}

int request_serve_dynamic(int fd, char *filename, char *cgiargs) {
    char buf[MAXBUF], *argv[] = { NULL };
    
    // The server does only a little bit of the header.  
    // The CGI script has to finish writing out the header.
    sprintf(buf, ""
	    "HTTP/1.0 200 OK\r\n"
	    "Server: OSTEP WebServer\r\n");
    
    if (write(fd, buf, strlen(buf)) == -1)
        return -1;
    
    if (fork_or_die() == 0) {                        // child
        setenv_or_die("QUERY_STRING", cgiargs, 1);   // args to cgi go here
        dup2_or_die(fd, STDOUT_FILENO);              // make cgi writes go to socket (not screen)
        extern char **environ;                       // defined by libc 
        execve_or_die(filename, argv, environ);
    } else {
	    wait_or_die(NULL);
    }

    return 0;
}

int request_serve_static(int fd, char *filename, int filesize) {
    int srcfd;
    char *srcp, filetype[MAXBUF / 10], buf[MAXBUF];
    
    request_get_filetype(filename, filetype);
    if ((srcfd = open(filename, O_RDONLY, 0)) == -1)
        return -1;
    
    // Rather than call read() to read the file into memory, 
    // which would require that we allocate a buffer, we memory-map the file
    srcp = mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0);
    if (srcp == MAP_FAILED)
        return -1;
    if (close(srcfd) == -1)
        return -1;
    
    // put together response
    sprintf(buf, ""
	    "HTTP/1.0 200 OK\r\n"
	    "Server: OSTEP WebServer\r\n"
	    "Content-Length: %d\r\n"
	    "Content-Type: %s\r\n\r\n", 
	    filesize, filetype);
    
    if (write(fd, buf, strlen(buf)) == -1)
        return -1;
    
    //  Writes out to the client socket the memory-mapped file 
    if (write(fd, srcp, filesize) == -1)
        return -1;
    if (munmap(srcp, filesize) == -1)
        return -1;

    return 0;
}

int
pre_handle_request(int fd, Buffer_t *reqBuf) {
    int is_static;
    struct stat sbuf;
    char buf[MAXBUF], method[MAXBUF], uri[MAXBUF], version[MAXBUF];
    char filename[MAXBUF], cgiargs[MAXBUF];
    
    if (readline(fd, buf, MAXBUF) == -1)
        return Error;

    sscanf(buf, "%s %s %s", method, uri, version);
    printf("method:%s uri:%s version:%s\n", method, uri, version);

    if (strstr(uri, "..")) {
        request_error(fd, uri, "403", "Forbidden", "server could not read this file");
        return Forbidden;
    }

    // if (strcasecmp(method, "GET")) {
    //     request_error(fd, method, "501", "Not Implemented", "server does not implement this method");
    //     return;
    // }
    if (request_read_headers(fd) == -1)
        return Error;

    is_static = request_parse_uri(uri, filename, cgiargs);
    if (stat(filename, &sbuf) < 0) {
        if (request_error(fd, filename, "404", "Not found", "server could not find this file") == -1)
            return Error;
        return NotFound;
    }

    if (is_static) {
        if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)) {
            if (request_error(fd, filename, "403", "Forbidden", "server could not read this file") == -1)
                return Error;
            return Forbidden;
        }
    } else {
        if (!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode)) {
            if (request_error(fd, filename, "403", "Forbidden", "server could not run this CGI program") == -1)
                return Error;
            return Forbidden;
        }
    }

    reqBuf->fd = fd;
    reqBuf->is_static = is_static;
    reqBuf->size = sbuf.st_size;
    reqBuf->handling = 0;
    strcpy(reqBuf->pathname, filename);
    strcpy(reqBuf->cgiargs, cgiargs);

    return OK;
}
