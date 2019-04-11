all: fork-join rendezvous barrier reader-writer reader-writer-nostarve mutex-nostarve

fork-join: fork-join.c common_threads.h
	gcc -o fork-join.out fork-join.c -Wall -pthread

rendezvous: rendezvous.c common_threads.h
	gcc -o rendezvous.out rendezvous.c -Wall -pthread

barrier: barrier.c common_threads.h
	gcc -o barrier.out barrier.c -Wall -pthread

reader-writer: reader-writer.c common_threads.h
	gcc -o reader-writer.out reader-writer.c -Wall -pthread

reader-writer-nostarve: reader-writer-nostarve.c common_threads.h
	gcc -o reader-writer-nostarve.out reader-writer-nostarve.c -Wall -pthread

mutex-nostarve: mutex-nostarve.c common_threads.h
	gcc -o mutex-nostarve.out mutex-nostarve.c -Wall -pthread
