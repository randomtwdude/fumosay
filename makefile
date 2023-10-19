CC=gcc
CFLAGS=-o

all: fumosay.c
	$(CC) $(CFLAGS) fumosay fumosay.c

clean: fumosay
	rm fumosay
