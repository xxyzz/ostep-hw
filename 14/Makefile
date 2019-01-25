all: null forget_free size_100 free_then_print vector

clean:
	rm -f ./*.out

null: null.c
	gcc -o null.out null.c -Wall -g

forget_free: forget_free.c
	gcc -o forget_free.out forget_free.c -Wall -g

size_100: size_100.c
	gcc -o size_100.out size_100.c -Wall -g

free_then_print: free_then_print.c
	gcc -o free_then_print.out free_then_print.c -Wall -g

vector: vector.c vector.h
	gcc -o vector.out vector.c -Wall -g
