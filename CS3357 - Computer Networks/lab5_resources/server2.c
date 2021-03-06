#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include "calc_message.h"
#include "udp_sockets.h"
#include "udp_server.h"

message_t* create_response_message(calc_message_t* request)
{
  int i;

  // Create a response message and initialize it
  calc_message_t* response = (calc_message_t*)create_message(); 
  response->operand_count = 0;
  response->sum = 0;

  // Compute the sum from the client's request
  for (i = 0; i < request->operand_count; ++i)
    response->sum += request->operands[i];
 
  // Set the length of the message (8 bytes of headers; no operands)
  response->length = 8;

  // Return the dynamically-allocated message
  return (message_t*)response;  
}

int main()
{
  calc_message_t* request;  // Client's request message
  message_t* response;      // Server response message
  host_t client;            // Client's address

  // Create a socket to listen on port 5000
  int sockfd = create_server_socket("5000");

  // Read the request message and generate the response
  request = (calc_message_t*)receive_message(sockfd, &client);
  response = create_response_message(request);

  // Send the response and free the memory allocated to the messages
  send_message(sockfd, response, &client);
  free(request);
  free(response);

  // Close the socket
  close(sockfd);

  exit(EXIT_SUCCESS);
}
