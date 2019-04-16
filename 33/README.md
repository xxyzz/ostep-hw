# Event-based Concurrency (Advanced)

## Homework (Code)

In this (short) homework, you’ll gain some experience with event-based code and some of its key concepts. Good luck!

### Questions

1. First, write a simple server that can accept and serve TCP connections. You’ll have to poke around the Internet a bit if you don’t already know how to do this. Build this to serve exactly one request at a time; have each request be very simple, e.g., to get the current time of day.

    [Similar Python 3 approach](https://github.com/xxyzz/cnata/tree/master/2/python3-code)

2. Now, add the `select()` interface. Build a main program that can accept multiple connections, and an event loop that checks which file descriptors have data on them, and then read and process those requests. Make sure to carefully test that you are using `select()` correctly.

    ```
    $ ./TCPClient.out 1 & ./TCPClient.out 2 & ./TCPClient.out 3 &
    ```

    [Server Example](https://www.gnu.org/software/libc/manual/html_node/Server-Example.html)

3. Next, let’s make the requests a little more interesting, to mimic a simple web or file server. Each request should be to read the contents of a file (named in the request), and the server should respond by reading the file into a buffer, and then returning the contents to the client. Use the standard `open()`, `read()`, `close()` system calls to implement this feature. Be a little careful here: if you leave this running for a long time, someone may figure out how to use it to read all the files on your computer!
