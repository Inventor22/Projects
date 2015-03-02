/**
 * File:      calc-server.c
 * Name:      Dustin Dobransky
 * ID:        ddobran
 * Student #: 250575030
 * Class:     CS3357 - Computer Networks
 *
 * Created October 9, 2014
 *
 * Description:
 *      This program evaluates simple arithmetic expressions containing only the
 * following operators: '*', '/', '+', '-'.
 *      Supports unary '-' operator and dividing by zero (returns 'inf').
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#include <syslog.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "Stack.h"
#include "calculator.h"
#include "server.h"

#define BACKLOG 25

/*
#define LOG_EMERG 0   // system is unusable
#define LOG_ALERT 1   // action must be taken immediately
#define LOG_CRIT 2    // critical conditions
#define LOG_ERR 3     // error conditions
#define LOG_WARNING 4 // warning conditions
#define LOG_NOTICE 5  // normal but significant condition
#define LOG_INFO 6    // informational
#define LOG_DEBUG 7   // debug-level messages
*/

int main(int argc, char** argv) {

    // open system log and default to recording all logs (debug)
	openlog("calc-server", LOG_PERROR | LOG_PID | LOG_NDELAY, LOG_USER);
    setlogmask(LOG_UPTO(LOG_DEBUG));

	int c;
	char* port = NULL;

	while (true) {
		static struct option long_options[] = {
			{"port", required_argument, 0, 'p'},
			{0, 0, 0, 0}
		};

		int option_index = 0;
		c = getopt_long(argc, argv, "p:", long_options, &option_index);

		if (c == -1)
			break;

		switch (c) {
			case 'p':
				port = optarg;
				if (port == NULL) {
					syslog(LOG_ERR, "Port argument missing!");
					exit(EXIT_FAILURE);
				}
				break;
			case '?':
				break;
		}
	}

	// We want to listen on the specified port
	struct addrinfo* results = get_server_sockaddr(port);
	syslog(LOG_INFO, "Server listening on port %s", port);

	// Create a listening socket
	int sockfd = bind_socket(results);

	// Start listening on the socket
	if (listen(sockfd, BACKLOG) == -1) {
		perror("Unable to listen on socket");
		syslog(LOG_ERR, "Unable to listen on socket");
		exit(EXIT_FAILURE);
	}

	while (true) {
		// Wait for a connection and handle it
		int connectionfd = wait_for_connection(sockfd);
		handle_connection(connectionfd);
	}

	closelog();

	exit(EXIT_SUCCESS);
}
