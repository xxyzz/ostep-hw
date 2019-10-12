# An admittedly primitive Makefile
# To compile, type "make" or make "all"
# To remove files, type "make clean"

CC = gcc
CFLAGS = -Wall -pthread -g
OBJS = wserver.o wclient.o request.o io_helper.o 

.SUFFIXES: .c .o 

all: wserver wclient spin.cgi

wserver: wserver.o request.o io_helper.o thread_helper.h
	$(CC) $(CFLAGS) -o wserver.out wserver.o request.o io_helper.o

wclient: wclient.o io_helper.o thread_helper.h
	$(CC) $(CFLAGS) -o wclient.out wclient.o io_helper.o 

spin.cgi: spin.c
	$(CC) $(CFLAGS) -o spin.cgi spin.c

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	-rm -f $(OBJS) wserver.out wclient.out spin.cgi
