CFLAGS = -Wall
UNAME := $(shell uname)
ifneq ($(UNAME), Darwin)
	CFLAGS += -lrt
endif

all: TCPServer TCPClient

TCPServer: TCPServer.c
	gcc -o TCPServer.out TCPServer.c $(CFLAGS)

TCPClient: TCPClient.c
	gcc -o TCPClient.out TCPClient.c -Wall
