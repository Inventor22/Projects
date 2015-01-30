CC=gcc
CFLAGS=-Wall -g -c -std=c99
LFLAGS=-Wall -g

all: a b c d

a: Read_CommandLine.o
	$(CC) $(LFLAGS) -o $@ $^

a.o: Read_CommandLine.c
	$(CC) $(CFLAGS) -o $@ $<

b: CS3305_Asn1.o StringQueue.o StringArrQueue.o
	$(CC) $(LFLAGS) -o $@ $^

b.o: CS3305_Asn1.c
	$(CC) $(CFLAGS) -o $@ $<

c: PipeInputTest.o
	$(CC) $(LFLAGS) -o $@ $^

c.o: PipeInputTest.c
	$(CC) $(CFLAGS) -o $@ $<

d: Example.o
	$(CC) $(LFLAGS) -o $@ $^

d.o: Example.c
	$(CC) $(CFLAGS) -o $@ $<

StringQueue.o: StringQueue.c StringQueue.h
	$(CC) $(CFLAGS) -o $@ $<

StringArrQueue.o: StringArrQueue.c StringArrQueue.h
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f *.o a b c d

