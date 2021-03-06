#include <arpa/inet.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include "calc_message.h"
#include "udp_sockets.h"
#include "udp_client.h"

message_t* create_calc_request(uint16_t op1, uint16_t op2, uint16_t op3, uint16_t op4)
{
  // Create a 16-byte message
  calc_message_t* msg = (calc_message_t*)create_message();

  // Store the operand count in the first byte of the message
  msg->operand_count = 4;

  // Convert the operands to network order and store them in the message
  msg->operands[0] = htons(op1);
  msg->operands[1] = htons(op2);
  msg->operands[2] = htons(op3);
  msg->operands[3] = htons(op4);

  // The message is 16 bytes long (8 bytes of headers + 8 bytes of operands)
  msg->length = 16;

  // Return the dynamically-allocated message
  return (message_t*)msg;
}

int main()
{
  host_t server;            // Address of the server
  calc_message_t* response; // Response returned by the server

  // Create a socket to listen on port 5000
  int sockfd = create_client_socket("localhost", "5000", &server);

  // Encode the message to be sent
  message_t* request = create_calc_request(10, 20, 30, 40);

  // Send it and free its memory
  send_message(sockfd, request, &server);
  free(request);

  // Read the response from the server and convert its sum to
  // host order
  response = (calc_message_t*)receive_message(sockfd, &server);
  response->sum = ntohl(response->sum);

  // Print the result and close the socket
  printf("Result: %d\n", response->sum);

  free(response);
  close(sockfd);

  exit(EXIT_SUCCESS);
}

