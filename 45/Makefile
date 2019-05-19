all: check-xor check-fletcher crc

check-xor: check-xor.c common.h
	gcc -o check-xor.out check-xor.c -Wall

check-fletcher: check-fletcher.c common.h
	gcc -o check-fletcher.out check-fletcher.c -Wall

crc: crc.c common.h
	gcc -o crc.out crc.c -Wall
