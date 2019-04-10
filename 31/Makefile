all: fork-join rendezvous barrier

fork-join: fork-join.c common_threads.h
	gcc -o fork-join.out fork-join.c -Wall -pthread

rendezvous: rendezvous.c common_threads.h
	gcc -o rendezvous.out rendezvous.c -Wall -pthread

barrier: barrier.c common_threads.h
	gcc -o barrier.out barrier.c -Wall -pthread 
