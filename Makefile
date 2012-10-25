CC=gcc
CFLAGS=-std=c99 -Wall -Wno-unused-value -g -O3 -march=native
LDFLAGS=-lrt

all: fork 1proc pthread


pthread:
	$(CC) $(CFLAGS) $@.c -pthread $(LDFLAGS) -o bin/$@

openmp:
	$(CC) $(CFLAGS) $@.c -fopenmp $(LDFLAGS) -o bin/$@

.c:
	$(CC) $(CFLAGS) $@.c $(LDFLAGS) -o bin/$@

.PHONY: clean
clean:
	rm bin/*

