/**
* File:      server.c
* Name:      Dustin Dobransky
* ID:        ddobran
* Student #: 250575030
* Class:     CS3357 - Computer Networks
*
* Created on October 10, 2014
*
* Description:
*    Handles the server side communications and
* associated operations, like verifying the message, etc.
*/

#include "server.h"
#include <stdlib.h>

/// <summary>
/// Get_server_sockaddrs the specified port.
/// </summary>
/// <param name="port">The port.</param>
/// <returns>Pointer to address info</returns>
struct addrinfo* get_server_sockaddr(const char* port) {
	struct addrinfo hints;
	struct addrinfo* results;
	int retval;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET; // Return socket addresses for our local IPv4 addresses
	hints.ai_socktype = SOCK_STREAM; // Return TCP socket addresses
	hints.ai_flags = AI_PASSIVE; // Socket addresses should be listening sockets
	retval = getaddrinfo(NULL, port, &hints, &results);
	if (retval != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(retval));
		exit(EXIT_FAILURE);
	}
	return results;
}

/// <summary>
/// Bind_sockets the specified addr_list.
/// </summary>
/// <param name="addr_list">The addr_list.</param>
/// <returns>socket</returns>
int bind_socket(struct addrinfo* addr_list) {
	struct addrinfo* addr;
	int sockfd;
	char yes = '1';
	// Iterate through each addrinfo in the list; stop when we successfully bind
	// to one
	for (addr = addr_list; addr != NULL; addr = addr->ai_next) {
		// Open a socket
		sockfd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
		// Try the next address if we couldn't open a socket
		if (sockfd == -1)
			continue;
		// Allow the port to be re-used if currently in the TIME_WAIT state
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
			perror("Unable to set socket option");
			syslog(LOG_ERR, "Unagle to get socket option\n");
			exit(EXIT_FAILURE);
		}
		// Try to bind the socket to the address/port
		if (bind(sockfd, addr->ai_addr, addr->ai_addrlen) == -1) {
			// If binding fails, close the socket, and try the next address
			syslog(LOG_ERR, "Binding to socket failed\n");
			close(sockfd);
			continue;
		} else {
			// Otherwise, we've bound the address/port to the socket, so stop
			// processing
			break;
		}
	}
	// Free the memory allocated to the addrinfo list
	freeaddrinfo(addr_list);
	// If addr is NULL, we tried every addrinfo and weren't able to bind to any
	if (addr == NULL) {
		perror("Unable to bind");
		syslog(LOG_ERR, "Unable to bind to socket\n");
		exit(EXIT_FAILURE);
	} else {
		// Otherwise, return the socket descriptor
		return sockfd;
	}
}

/// <summary>
/// Wait_for_connections on the specified sockfd.
/// </summary>
/// <param name="sockfd">The socket.</param>
/// <returns>connection</returns>
int wait_for_connection(int sockfd) {
	struct sockaddr_in client_addr; // Remote IP that is connecting to us
	int addr_len = sizeof(struct sockaddr_in); // Length of the remote IP structure
	char ip_address[INET_ADDRSTRLEN]; // Buffer to store human-friendly IP address
	int connectionfd; // Socket file descriptor for the new connection
	// Wait for a new connection
	connectionfd = accept(sockfd, (struct sockaddr*)&client_addr, &addr_len);
	// Make sure the connection was established successfully
	if (connectionfd == -1) {
		perror("Unable to accept connection");
		exit(EXIT_FAILURE);
	}
	// Convert the connecting IP to a human-friendly form and print it
	inet_ntop(client_addr.sin_family, &client_addr.sin_addr, ip_address, sizeof(ip_address));
	syslog(LOG_INFO, "Request received from client %s", ip_address);
	// Return the socket file descriptor for the new connection
	return connectionfd;
}

/// <summary>
/// Exprs the state.
/// </summary>
/// <param name="buffer">The buffer.</param>
/// <returns></returns>
int exprState(char* buffer) {
    //printf("Message to be parsed: %s\n", buffer);

    int end = strlen(buffer);
    if (end < 3 || buffer[end - 1] != '\n' || buffer[end - 2] != '\r') {
        //syslog(LOG_INFO, "len: %d", end);
        //syslog(LOG_INFO, "%d", buffer[end-1]);
        //syslog(LOG_INFO, "%d", buffer[end-2]);
        //syslog(LOG_INFO, "%d", buffer[end]);
        return MALFORMED_REQ;
    }

    // 79 char + '\r' + '\n' + '\0' = 79 + 1 + 1 = 81
    if (strlen(buffer) > 81) {
        return MAX_LENGTH_EXCEEDED;
    }

    if (strchr(buffer, '.') != NULL) {
        return FP_NOT_SUPPORTED;
    }

	int lBracket = 0, rBracket = 0;

    int i;
	for (i = 0; buffer[i] != '\n'; i++) {
		char c = buffer[i];
		if (c == '(') {
			lBracket++;
		} else if (c == ')') {
			rBracket++;
		}
        if (strchr(validChars, c) == NULL) {
            syslog(LOG_ERR, "invalid char: (in decimal) %d , (as char): %c", c, c);
            return INVALID_EXPR;
        }
	}

    if (lBracket != rBracket) {
        return MISMATCH;
    }

    return OK;
}

/// <summary>
/// Handle_connections the specified connectionfd.
/// </summary>
/// <param name="connectionfd">The connectionfd.</param>
void handle_connection(int connectionfd) {
	/*
	Each incoming request should be logged with the client's IP address,
	the expression sent, the status code returned to the client, and the
	result of evaluating the expression, if the expression was valid.
	*/
	char buffer[4096];
	int bytes_read;
	// Read up to 4096 bytes from the client
	bytes_read = recv(connectionfd, buffer, sizeof(buffer), 0);

	// If the data was read successfully
	if (bytes_read > 0) {
		// Add a terminating NULL character and print the message received
		buffer[bytes_read] = '\0';
		syslog(LOG_INFO, "Expression received was: %s", buffer);

        //int i;
        //for (i = 0; i < strlen(buffer); i++) {
        //    if (buffer[i] == '\r') { // because windows
        //        buffer[i] = '\n';    // and telnet'n to test
        //    }
        //}

        const char* status;

        switch (exprState(buffer)) {
            case MAX_LENGTH_EXCEEDED:
                status = "max-length-exceeded";
                syslog(LOG_ERR, "Status: %s", status);
                break;
            case INVALID_EXPR:
                status = "invalid-expr";
                syslog(LOG_ERR, "Status: %s", status);
                break;
            case MISMATCH:
                status = "mismatch";
                syslog(LOG_ERR, "Status: %s", status);
                break;
            case FP_NOT_SUPPORTED:
                status = "fp-not-supported";
                syslog(LOG_ERR, "Status: %s", status);
                break;
            case MALFORMED_REQ:
                status = "malformed-req";
                syslog(LOG_ERR, "Status: %s", status);
                break;
            case OK:
            default:
                status = "ok";
                syslog(LOG_INFO, "Status: %s", status);
                break;
        }

        char response[4096];
        if (strcmp(status, "ok")) { // if != 0
            sprintf(response, "Status: %s\r\n", status);
        } else {
            struct Result result = evaluate_expr(buffer);
            if (result.isInfinite) {
                sprintf(response, "Status: %s\r\nResult: inf\r\n", status);
            } else {
                sprintf(response, "Status: %s\r\nResult: %d\r\n", status, result.result);
            }
        }

		// send the response back to the client
		if (send(connectionfd, response, strlen(response), 0) == -1) {
			perror("Unable to send to socket");
			syslog(LOG_ERR, "Unable to send to socket\n");
			exit(EXIT_FAILURE);
		}
	} else if (bytes_read == -1) {
		// Otherwise, if the read failed,
        syslog(LOG_ERR, "Unable to read from socket\n");
		perror("Unable to read from socket");
		exit(EXIT_FAILURE);
	}
	// Close the connection
	close(connectionfd);
}
