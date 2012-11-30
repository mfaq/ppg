CC=gcc
CFLAGS=-std=c99 -Wall -Wno-unused-value -g -O3 -march=native -D_XOPEN_SOURCE=700
LDFLAGS=-lrt

all: fork 1proc pthread


fork:
	$(CC) $(CFLAGS) -D_SVID_SOURCE $@.c $(LDFLAGS) -o bin/$@

pthread:
	$(CC) $(CFLAGS) $@.c -pthread $(LDFLAGS) -o bin/$@

openmp:
	$(CC) $(CFLAGS) $@.c -fopenmp $(LDFLAGS) -o bin/$@

.c:
	$(CC) $(CFLAGS) $@.c $(LDFLAGS) -o bin/$@

.PHONY: clean all
clean:
	rm bin/*
