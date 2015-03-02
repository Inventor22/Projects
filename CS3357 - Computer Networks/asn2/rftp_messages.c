/********************************************************************************
* rftp_messages.c
*
* Computer Science 3357a
* Assignment 2
*
* Author: Dustin Dobransky, 250575030, ddobran
*
* Contains functions and definitions for packet transmission using UDP
*******************************************************************************/


#include "rftp_messages.h"

//struct addrinfo* get_udp_sockaddr(const char* node, const char* port, int flags) {
//    struct addrinfo hints;
//    struct addrinfo* results;
//    int retval;
//
//    memset(&hints, 0, sizeof(struct addrinfo));
//
//    hints.ai_family = AF_INET;      // Return socket addresses for our local IPv4 addresses
//    hints.ai_socktype = SOCK_DGRAM; // Return UDP socket addresses                                   
//    hints.ai_flags = flags;         // Socket addresses should be listening sockets                     
//
//    retval = getaddrinfo(node, port, &hints, &results);
//
//    if (retval != 0) {
//        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(retval));
//        exit(EXIT_FAILURE);
//    }
//
//    return results;
//}

rftp_message_t* create_rftp_message() {
	return (rftp_message_t*) malloc(sizeof(rftp_message_t));
}

rftp_message_t* receive_rftp_message(int sockfd, host_t* source) {
	rftp_message_t* msg = create_rftp_message();

	  // Length of the remote IP structure
  source->addr_len = sizeof(source->addr);

  // Read message, storing its contents in msg->buffer, and
  // the source address in source->addr
  msg->length = recvfrom(sockfd, msg->buffer, sizeof(msg->buffer), 0,                  
                         (struct sockaddr*)&source->addr,                           
                         &source->addr_len);

  // If a message was read
  if (msg->length > 0)
  {
    // Convert the source address to a human-readable form,
    // storing it in source->friendly_ip
    inet_ntop(source->addr.sin_family, &source->addr.sin_addr,                  
              source->friendly_ip, sizeof(source->friendly_ip));                  

    // Return the message received
    return msg;                                                               
  }
  else
  {
    // Otherwise, free the allocated memory and return NULL
    free(msg);                                                               
    return NULL;                                                               
  }
}

rftp_message_t* receive_rftp_message_with_timeout(int sockfd, int timeout, host_t* source) {
	rftp_message_t* msg;   // Message read from the client
    int retval;       // Return value from poll

    // We will poll sockfd for the POLLIN event
    struct pollfd fd = {
        .fd = sockfd,
        .events = POLLIN
    };

    // Poll the socket for 10 seconds
    retval = poll(&fd, 1, timeout);

    if (retval == 1 && fd.revents == POLLIN) {
        // Read the waiting message
        msg = receive_rftp_message(sockfd, source);
        return msg;
    } else {
        return NULL;
    }
}

int send_rftp_message(int sockfd, rftp_message_t* msg, host_t* dest) {
  return sendto(sockfd, msg->buffer, msg->length, 0,
                (struct sockaddr*)&dest->addr, dest->addr_len);
}


rftp_message_t* create_control_message(uint8_t type, uint8_t ack, uint16_t seqNum,
									   uint32_t fileSize, uint32_t fileNameLen,
									   uint32_t* fileName) {
	rftp_control_message_t* msg = (rftp_control_message_t*) create_rftp_message();
	//msg->type = type;
	//msg->ack = ack;
	//msg->sequenceNumber = seqNum;
	//msg->fileSize = fileSize;
	//msg->fileNameLength = fileNameLen;
	//msg->fileName = fileName;

	return (rftp_message_t*) msg;
}

rftp_message_t* create_data_message(uint8_t ack, uint16_t seqNum,
									uint32_t dataLen, uint32_t* data) {
	rftp_data_message_t* msg = (rftp_data_message_t*) create_rftp_message();
    //msg->type = TYPE3_DATA_MSG;
	//msg->ack = ack;
	//msg->sequenceNumber = seqNum;
	//msg->dataLength = dataLen;
	//msg->data = data;

	return (rftp_message_t*) msg;
}