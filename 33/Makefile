CFLAGS = -Wall
UNAME := $(shell uname)
ifneq ($(UNAME), Darwin)
	CFLAGS += -lrt
endif

all: TCPServer TCPClient TCPServer-sync

TCPServer: TCPServer.c
	gcc -o TCPServer.out TCPServer.c $(CFLAGS)

TCPClient: TCPClient.c
	gcc -o TCPClient.out TCPClient.c -Wall

TCPServer-sync: TCPServer-sync.c
	gcc -o TCPServer-sync.out TCPServer-sync.c $(CFLAGS)
