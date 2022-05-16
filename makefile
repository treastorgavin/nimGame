CC=clang
CFLAGS=-std=c99 -Wall -pedantic

all: nim.o

nim.o: nim.c nimhelp.c nim.h
	$(CC) $(CFLAGS) -c nim.c nimhelp.c

clean:
	rm *.o