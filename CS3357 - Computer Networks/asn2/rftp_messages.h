/********************************************************************************
* rftp_messages.h 
*
* Computer Science 3357a
* Assignment 2
*
* Author: Dustin Dobransky, 250575030, ddobran <at> uwo.ca
*
* Contains functions and definitions for packet transmission using UDP
*
* Note: udp_sockets.h was only used for the get_udp_sockaddr function,
* which I ran out of time to migrate/edit makefile to reflect changes.
*******************************************************************************/


#ifndef RFTP_MESSAGES_H
#define RFTP_MESSAGES_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "udp_sockets.h"
#include <poll.h>

#define ACK_TYPE0_SEND   0
#define ACK_TYPE1_ACK    1

#define TYPE1_CTRL_MSG_INITIATION  1
#define TYPE2_CTRL_MSG_TERMINATION 2
#define TYPE3_DATA_MSG             3

#define SEQUENCE_0       0
#define SEQUENCE_1       1

typedef struct
{
	int length;
	uint8_t buffer[1472];
} rftp_message_t;

//1472 + sizeof(int)(=2) = bytes total length
typedef struct
{
	int length;
 	uint8_t type;                
	uint8_t ack;        
	uint16_t sequenceNumber;           
	uint32_t fileSize;
	uint32_t fileNameLength;
	uint8_t fileName[1460];
} rftp_control_message_t;

typedef struct
{
	int length;
 	uint8_t type;                
	uint8_t ack;        
	uint16_t sequenceNumber;           
	uint32_t dataLength;
	uint8_t data[1464];
} rftp_data_message_t;

//struct addrinfo* get_udp_sockaddr(const char* node, const char* port, int flags);

rftp_message_t* create_rftp_message();

rftp_message_t* receive_rftp_message(int sockfd, host_t* source);

rftp_message_t* receive_rftp_message_with_timeout(int sockfd, int timeout, host_t* source);

int send_rftp_message(int sockfd, rftp_message_t* msg, host_t* dest);


rftp_message_t* create_control_message(uint8_t type, uint8_t ack, uint16_t seqNum,
									   uint32_t fileSize, uint32_t fileNameLen,
									   uint32_t* fileName);

rftp_message_t* create_data_message(uint8_t ack, uint16_t seqNum, uint32_t dataLen, uint32_t* data);


#endif