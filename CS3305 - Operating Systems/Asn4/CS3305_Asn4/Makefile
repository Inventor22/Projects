CC=gcc
CFLAGS=-Wall -g -c -std=c99
LFLAGS=-Wall -g

all: simulator

simulator: simulator.o Deque.o
	$(CC) $(LFLAGS) -o $@ $^

simulator.o: simulator.c Deque.h
	$(CC) $(CFLAGS) -o $@ $<

Deque.o: Deque.c Deque.h
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f *.o simulator