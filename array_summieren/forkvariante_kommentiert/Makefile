CC=gcc
CFLAGS=-std=c99 -Wall -Wno-unused-value -g -O3 -march=native -D_XOPEN_SOURCE=700 -D_SVID_SOURCE
LDFLAGS=-lrt

fork:
	$(CC) $(CFLAGS) $@.c $(LDFLAGS) -o $@

.PHONY: clean
clean:
	rm fork

