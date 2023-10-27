CC=gcc
CFLAGS=-o

all: fumosay.c
	$(CC) $(CFLAGS) fumosay fumosay.c

segfault: fumosay.c
	$(CC) $(CFLAGS) fumosay fumosay.c -fsanitize=address,undefined

clean: fumosay
	rm fumosay
