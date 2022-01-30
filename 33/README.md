# Event-based Concurrency (Advanced)

## Homework (Code)

In this (short) homework, you’ll gain some experience with event-based code and some of its key concepts. Good luck!

### Questions

1. First, write a simple server that can accept and serve TCP connections. You’ll have to poke around the Internet a bit if you don’t already know how to do this. Build this to serve exactly one request at a time; have each request be very simple, e.g., to get the current time of day.

    [Similar Python 3 approach](https://github.com/xxyzz/cnata/tree/master/2/python3-code)

2. Now, add the `select()` interface. Build a main program that can accept multiple connections, and an event loop that checks which file descriptors have data on them, and then read and process those requests. Make sure to carefully test that you are using `select()` correctly.

    [Server Example](https://www.gnu.org/software/libc/manual/html_node/Server-Example.html)

3. Next, let’s make the requests a little more interesting, to mimic a simple web or file server. Each request should be to read the contents of a file (named in the request), and the server should respond by reading the file into a buffer, and then returning the contents to the client. Use the standard `open()`, `read()`, `close()` system calls to implement this feature. Be a little careful here: if you leave this running for a long time, someone may figure out how to use it to read all the files on your computer!

4. Now, instead of using standard I/O system calls, use the asynchronous I/O interfaces as described in the chapter. How hard was it to incorporate asynchronous interfaces into your program?

5. For fun, add some signal handling to your code. One common use of signals is to poke a server to reload some kind of configuration file, or take some other kind of administrative action. Perhaps one natural way to play around with this is to add a user-level file cache to your server, which stores recently accessed files. Implement a signal handler that clears the cache when the signal is sent to the server process.

6. Finally, we have the hard part: how can you tell if the effort to build an asynchronous, event-based approach are worth it? Can you create an experiment to show the benefits? How much implementation complexity did your approach add?
