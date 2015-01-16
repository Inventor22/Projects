all: calc-server calc-client

calc-server: calc-server.o Stack.o calculator.o server.o
	gcc calc-server.o Stack.o calculator.o server.o -o calc-server

calc-client: calc-client.o
	gcc calc-client.o -o calc-client

calc-client.o: calc-client.c
	gcc -c calc-client.c

calc-server.o: calc-server.c
	gcc -c calc-server.c

Stack.o: Stack.c
	gcc -c Stack.c

calculator.o: calculator.c
	gcc -c calculator.c

server.o: server.c
	gcc -c server.c

clean:
	rm -rf *o calc-server
	rm -rf *o calc-client
