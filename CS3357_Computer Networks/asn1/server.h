/**
* File:      server.h
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

#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <syslog.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "calculator.h"

/*
Response Code	    Description
ok	                Request successful; response contains result of evaluating the expression
malformed-req	    Request message was not structured correctly
max-length-exceeded	Expression was longer than 79 characters
mismatch	        Expression contained mismatched parentheses
fp-not-supported	Expression contained a decimal point (.)
invalid-expr        Expression contained an invalid character (e.g. %)
*/

#define OK 0
#define MAX_LENGTH_EXCEEDED -1
#define INVALID_EXPR -2
#define MISMATCH -3
#define FP_NOT_SUPPORTED -4
#define MALFORMED_REQ -5

static const char validChars[] = "1234567890+-*/() \r\n";

struct addrinfo* get_server_sockaddr(const char* port);
int bind_socket(struct addrinfo* addr_list);
int wait_for_connection(int sockfd);
int exprState(char* buffer);
void handle_connection(int connectionfd);

#endif
