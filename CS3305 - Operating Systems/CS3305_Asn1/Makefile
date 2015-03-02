CC=gcc
CFLAGS=-Wall -g -c -std=c99
LFLAGS=-Wall -g

all: shell

shell: shell.o StringQueue.o StringArrQueue.o
	$(CC) $(LFLAGS) -o $@ $^

shell.o: shell.c
	$(CC) $(CFLAGS) -o $@ $<

StringQueue.o: StringQueue.c StringQueue.h
	$(CC) $(CFLAGS) -o $@ $<

StringArrQueue.o: StringArrQueue.c StringArrQueue.h
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f *.o shell