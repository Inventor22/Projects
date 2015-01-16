CC=gcc
CFLAGS=-Wall -g -c
LFLAGS=-Wall -g

all: rftp rftpd

rftp: rftp.o udp_sockets.o udp_client.o udp_server.o rftp_messages.o
	$(CC) $(LFLAGS) -o $@ $^

rftpd: rftpd.o udp_sockets.o udp_client.o udp_server.o rftp_messages.o
	$(CC) $(LFLAGS) -o $@ $^

rftp.o: rftp.c udp_sockets.h udp_server.h udp_client.h rftp_messages.h
	$(CC) $(CFLAGS) -o $@ $<

rftpd.o: rftpd.c udp_sockets.h udp_server.h udp_client.h rftp_messages.h
	$(CC) $(CFLAGS) -o $@ $<

rftp_messages.o: rftp_messages.c rftp_messages.h udp_sockets.h
	$(CC) $(CFLAGS) -o $@ $<

udp_sockets.o: udp_sockets.c udp_sockets.h
	$(CC) $(CFLAGS) -o $@ $<

udp_server.o: udp_server.c udp_server.h udp_sockets.h
	$(CC) $(CFLAGS) -o $@ $<

udp_client.o: udp_client.c udp_client.h udp_sockets.h
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f *.o rftp rftpd
