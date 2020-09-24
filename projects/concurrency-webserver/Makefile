# An admittedly primitive Makefile
# To compile, type "make" or make "all"
# To remove files, type "make clean"

CC = clang
CFLAGS = -Wall -Wextra -pthread
OBJS = wserver.o wclient.o request.o io_helper.o

all: wserver.out wclient.out spin.cgi

wserver.out: wserver.o request.o io_helper.o thread_helper.h
	$(CC) $(CFLAGS) -o $@ wserver.o request.o io_helper.o

wclient.out: wclient.o io_helper.o thread_helper.h
	$(CC) $(CFLAGS) -o $@ wclient.o io_helper.o

spin.cgi: spin.c
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f $(OBJS) wserver.out wclient.out spin.cgi
