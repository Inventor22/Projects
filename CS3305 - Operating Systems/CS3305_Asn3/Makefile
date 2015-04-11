CC=gcc
CFLAGS=-Wall -g -c -std=c99 -lpthread -pthread
LFLAGS=-Wall -g
LIBS ?= -lpthread

all: takeTurns1 takeTurns2 takeTurns3 twoProducers

takeTurns1: takeTurns1.o
	$(CC) $(LFLAGS) -o $@ $^ $(LIBS)

takeTurns2: takeTurns3.o
	$(CC) $(LFLAGS) -o $@ $^ $(LIBS)

takeTurns3: takeTurns3.o
	$(CC) $(LFLAGS) -o $@ $^ $(LIBS)

twoProducers: twoProducers.o
	$(CC) $(LFLAGS) -o $@ $^ $(LIBS)

takeTurns1.o: takeTurns1.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

takeTurns2.o: takeTurns2.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

takeTurns3.o: takeTurns3.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

twoProducers.o: twoProducers.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

clean:
	rm -f *.o takeTurns1 takeTurns2 takeTurns3 twoProducers