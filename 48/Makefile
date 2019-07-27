all: UDP-client UDP-server

UDP-client: UDP-client.c UDP-lib.h
	gcc -o UDP-client.out UDP-client.c -Wall -pthread

UDP-server: UDP-server.c UDP-lib.h
	gcc -o UDP-server.out UDP-server.c -Wall -pthread
