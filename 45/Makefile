CC = clang
CFLAGS = -Wall -Wextra

all: check-xor.out check-fletcher.out crc.out create-csum.out check-csum.out

%.out: %.c common.h
	$(CC) -o $@ $< $(CFLAGS)
