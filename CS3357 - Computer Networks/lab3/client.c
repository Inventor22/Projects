/********************************************************************************
* client.c
*
* Computer Science 3357a
* Lab 3
*
* Author: Dustin Dobransky
* ID:     250575030
*
* Description:
*     Accepts command line arguments for server, port, and message.
* Sends to server, which echo's the message.
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
    openlog("client", LOG_PERROR | LOG_PID | LOG_NDELAY, LOG_USER);
    setlogmask(LOG_UPTO(LOG_DEBUG));
    syslog(LOG_INFO, "Client initialized");

    int c;
    char* server = argv[1];
    char* port   = argv[2];
    char* msg    = argv[3];

    // Connect to the server
    struct addrinfo* results = get_sockaddr(server, port);
    int sockfd = open_connection(results);
    int bytes_read; // Number of bytes read from the server

    char buffer[4096]; // Buffer to store received message

    // Send the message
    if (send(sockfd, msg, strlen(msg), 0) == -1) {
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

    printf("%s\n", msg);

    // Close the connection
    close(sockfd);

    closelog();

    exit(EXIT_SUCCESS);
}
