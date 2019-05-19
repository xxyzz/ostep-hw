all: check-xor check-fletcher crc create-csum check-csum

check-xor: check-xor.c common.h
	gcc -o check-xor.out check-xor.c -Wall

check-fletcher: check-fletcher.c common.h
	gcc -o check-fletcher.out check-fletcher.c -Wall

crc: crc.c common.h
	gcc -o crc.out crc.c -Wall

create-csum: create-csum.c common.h
	gcc -o create-csum.out create-csum.c -Wall

check-csum: check-csum.c common.h
	gcc -o check-csum.out check-csum.c -Wall
