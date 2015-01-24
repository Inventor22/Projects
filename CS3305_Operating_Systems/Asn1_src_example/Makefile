CC=gcc
CFLAGS=-Wall -g -c
LFLAGS=-Wall -g

all: 

a: Read_CommandLine.o
	$(CC) $(LFLAGS) -o $@ $^

a.o: Read_CommandLine.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f *.o example

