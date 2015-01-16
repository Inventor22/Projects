/********************************************************************************
* calc-client.c
*
* Computer Science 3357a
* Assignment 1
*
* Author: Dustin Dobransky
* ID:     250575030
*
* Description:
*     This program will take command line input of a math expression and return
* the evaluated result, which will be computed on a server.
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

/// <summary>
/// Get_sockaddr gets a linked list of available sockets.
/// </summary>
/// <param name="hostname">The hostname.</param>
/// <param name="port">The port.</param>
/// <returns>LL of socket addresses + infos</returns>
struct addrinfo* get_sockaddr(const char* hostname, const char* port) {
    struct addrinfo hints;
    struct addrinfo* results;
    int retval;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET; // Return socket addresses for the server's IPv4 addresses
    hints.ai_socktype = SOCK_STREAM; // Return TCP socket addresses
    retval = getaddrinfo(NULL, port, &hints, &results);
    if (retval != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(retval));
        exit(EXIT_FAILURE);
    }
    return results;
}

/// <summary>
/// Open_connection for an available socket
/// </summary>
/// <param name="addr_list">The addr_list.</param>
/// <returns>socket</returns>
int open_connection(struct addrinfo* addr_list) {
    struct addrinfo* addr;
    int sockfd;
    // Iterate through each addrinfo in the list; stop when we successfully
    // connect to one
    for (addr = addr_list; addr != NULL; addr = addr->ai_next) {
        // Open a socket
        sockfd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
        // Try the next address if we couldn't open a socket
        if (sockfd == -1)
            continue;
        // Stop iterating if we're able to connect to the server
        if (connect(sockfd, addr->ai_addr, addr->ai_addrlen) != -1)
            break;
    }
    // Free the memory allocated to the addrinfo list
    freeaddrinfo(addr_list);
    // If addr is NULL, we tried every addrinfo and weren't able to connect to any
    if (addr == NULL) {
        perror("Unable to connect");
        exit(EXIT_FAILURE);
    } else {
        return sockfd;
    }
}

int main(int argc, char** argv) {
    openlog("calc-client", LOG_PERROR | LOG_PID | LOG_NDELAY, LOG_USER);
    setlogmask(LOG_UPTO(LOG_DEBUG));

    int c;
    char* port = NULL;
    char* expr = NULL;
    char* server = NULL;

    // parse the command line arguments: port, math expression, and server
    while (true) {
        static struct option long_options[] =
        {
            {"port", required_argument, 0, 'p'},
            {"expr", required_argument, 0, 'e'},
            {"server", required_argument, 0, 's'},
            {0, 0, 0, 0}
        };

        int option_index = 0;
        c = getopt_long(argc, argv, "p:e:s:", long_options, &option_index);

        if (c == -1)
            break;

        switch (c) {
            case 'p':
                port = optarg;
                syslog(LOG_INFO, "Listening on port: %s", port);
                break;
            case 'e':
                expr = optarg;
                int len = strlen(expr);
                expr[len] = '\r'; // must add '\n' as terminating char for expr.
                expr[len + 1] = '\n';
                expr[len + 2] = '\0';
                syslog(LOG_INFO, "Expression to evaluate: %s", expr);
                break;
            case 's':
                server = optarg;
                syslog(LOG_INFO, "Communicating with server: %s", server);
                break;
            case '?':
                break;
        }
    }

    // Connect to the server
    struct addrinfo* results = get_sockaddr(server, port);
    int sockfd = open_connection(results);
    int bytes_read; // Number of bytes read from the server

    char buffer[81]; // Buffer to store received message, leaving
    // space for the NULL terminator

    // Send the message
    if (send(sockfd, expr, strlen(expr), 0) == -1) {
        syslog(LOG_ERR, "Unable to send message");
        perror("Unable to send");
        exit(EXIT_FAILURE);
    }
    syslog(LOG_INFO, "Message sent to server successfully");

    // Read the reply
    bytes_read = recv(sockfd, buffer, sizeof(buffer), 0);

    if (bytes_read == -1) {
        syslog(LOG_ERR, "Error reading server response message");
        perror("Unable to read");
        exit(EXIT_FAILURE);
    }

    // Add the terminating NULL character to the end
    buffer[bytes_read] = '\0';

    syslog(LOG_INFO, "Response received: %s", buffer);

    char* item;
    item = strtok(buffer, " \r\n");
    //syslog(LOG_INFO, "item: %s", item);
    item = strtok(NULL, " \r\n");   // get second element
    //syslog(LOG_INFO, "item: %s", item);

    if (strcmp(item, "ok") == 0) {
        printf("Status Code: %s\r\n", item);
        item = strtok(NULL, " \r\n");
        item = strtok(NULL, " \r\n"); // get 4th element
        printf("Result: %s\r\n", item);
    } else {
        printf("Status Code: %s\r\n", item);
    }

    // Close the connection
    close(sockfd);

    closelog();

    exit(EXIT_SUCCESS);
}

