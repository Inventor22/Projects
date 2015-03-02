/********************************************************************************
* rftpd.c
*
* Computer Science 3357a
* Assignment 2
*
* Author: Dustin Dobransky, 250575030, ddobran
*
* The server for the reliable FTP program
*******************************************************************************/

#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "udp_sockets.h"
#include "udp_server.h"
#include "udp_client.h"
#include <stdbool.h>
#include <syslog.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "rftp_messages.h"

rftp_message_t* create_data_ack_message(int seqNumReceived) {
    rftp_data_message_t* msg = (rftp_data_message_t*) create_rftp_message();
    msg->ack = ACK_TYPE1_ACK;
    msg->sequenceNumber = seqNumReceived;
    msg->length = 4;
    return (rftp_message_t*)msg;
}

void getOptions(int argc, char** argv, int* timewait, char** port) {
    int c;
    //bool verbose = false;

    // parse the command line arguments: port, math expression, and server
    while (true) {
        static struct option long_options[] =
        {
            {"verbose", required_argument, 0, 'v'},
            {"timewait", required_argument, 0, 't'},
            {"port", required_argument, 0, 'p'},
            {0, 0, 0, 0}
        };

        int option_index = 0;
        c = getopt_long(argc, argv, "vt:p:", long_options, &option_index);

        if (c == -1)
            break;

        switch (c) {
            case 'p':
                *port = optarg;
                syslog(LOG_DEBUG, "Listening on port: %s", *port);
                break;
            case 't':
                *timewait = atoi(optarg);
                syslog(LOG_DEBUG, "Timeout: %d", *timewait);
                break;
            case 'v':
                //verbose = true;
                setlogmask(LOG_UPTO(LOG_DEBUG));
                syslog(LOG_DEBUG, "Verbose flag set");
                break;
            case '?':
                break;
        }
    }
}

void getNonOptions(char** outputDir, int optind, int argc, char** argv) {
    if (argc-optind != 1) {
        syslog(LOG_DEBUG, "Output directory not specified!");
        exit(EXIT_FAILURE);
    }
    *outputDir = argv[optind];
}

rftp_message_t* create_type1_control_ack_response() {
    rftp_control_message_t* msg = (rftp_control_message_t*) create_rftp_message();
    msg->ack = ACK_TYPE1_ACK;
    msg->sequenceNumber = 0;

    // 32 bits to store type + ack + sequence number
    msg->length = 4;

    return (rftp_message_t*) msg;
}

rftp_message_t* create_type2_control_ack_response(int seqNumOfReceivedTerminationMsg) {
    rftp_control_message_t* msg = (rftp_control_message_t*) create_rftp_message();
    msg->ack = ACK_TYPE1_ACK;
    msg->sequenceNumber = seqNumOfReceivedTerminationMsg;

    // 32 bits to store type + ack + sequence number
    msg->length = 4;

    return (rftp_message_t*) msg;
}

int main(int argc, char** argv) {
    openlog("rftpd", LOG_PERROR | LOG_PID | LOG_NDELAY, LOG_USER);
    setlogmask(LOG_UPTO(LOG_DEBUG));

    static int   timewait  = 0;
    char* port      = NULL;
    char* outputDir = NULL;

    char fileName[1460];
    int  fileSize  = 0;

    getOptions(argc, argv, &timewait, &port);
    getNonOptions(&outputDir, optind, argc, argv);

    if (port == NULL || outputDir == NULL) {
        syslog(LOG_DEBUG, "must supply valid arguments");
        exit(EXIT_FAILURE);
    }
    syslog(LOG_DEBUG, "Args accepted");


    int sockfd = create_server_socket(port);

    rftp_control_message_t* ctrlRequestMsg;
    rftp_data_message_t*    dataMsg;
    rftp_message_t*         dataAckMsg;
    //rftp_data_message_t* dataMsg;
    rftp_message_t* type1CtrlResponse;
    //rftp_message_t* receivedMsg;
    host_t client;

    syslog(LOG_DEBUG, "socket created");
    type1CtrlResponse = create_type1_control_ack_response();

    syslog(LOG_DEBUG, "created type1ctrlresponse");

    while (true) {
        syslog(LOG_DEBUG, "waiting for type1ctrlMsg");

        /*Wait for type 1 control message*/
        ctrlRequestMsg = (rftp_control_message_t*) receive_rftp_message(sockfd, &client);

        if (ctrlRequestMsg->type == TYPE1_CTRL_MSG_INITIATION) {
            send_rftp_message(sockfd, type1CtrlResponse, &client);
            syslog(LOG_DEBUG, "type1ctrlMsg, sent type1CtrlResponse");
            break;
        } else {
            syslog(LOG_DEBUG, "type1ctrlMsg->type != TYPE1_CTRL_MSG_INITIATION");
        }
    }

    memcpy(fileName, ctrlRequestMsg->fileName, ctrlRequestMsg->fileNameLength);

    fileName[ctrlRequestMsg->fileNameLength] = '_';
    fileName[ctrlRequestMsg->fileNameLength+1] = '\0';
    fileSize = ctrlRequestMsg->fileSize;
    syslog(LOG_DEBUG, "filename: %s, fileLength: %d", fileName, fileSize);
    //free(ctrlRequestMsg);
    //free(type1CtrlResponse);

    free(ctrlRequestMsg);

    FILE *fp;
    fp = fopen( fileName , "w+" );

    int numBytesWritten = 0;

    bool transferringFile = true;

    int type2CtrlMsgSeqNum = 0;

    int dataChunkCount = 0;

    syslog(LOG_DEBUG, "reading transfer file");
    while (transferringFile)
    {
        syslog(LOG_DEBUG, "Step 1");

        bool   waitForDataMsgSeq1 = true;
        while (waitForDataMsgSeq1)
        {
            dataMsg = (rftp_data_message_t*) receive_rftp_message(sockfd, &client);
            syslog(LOG_DEBUG, "received data message 1");

            switch (dataMsg->type)
            {
                case TYPE1_CTRL_MSG_INITIATION:
                    send_rftp_message(sockfd, type1CtrlResponse, &client);
                    syslog(LOG_DEBUG, "TYPE1_CTRL_MSG_INITIATION");
                    break;
                case TYPE2_CTRL_MSG_TERMINATION:
                    transferringFile = false;
                    type2CtrlMsgSeqNum = ctrlRequestMsg->sequenceNumber;
                    syslog(LOG_DEBUG, "TYPE2_CTRL_MSG_TERMINATION");
                    waitForDataMsgSeq1 = false;
                    break;
                case TYPE3_DATA_MSG:
                    if (dataMsg->sequenceNumber == 1) {
                        syslog(LOG_DEBUG, "TYPE3_DATA_MSG SEQ1");
                        /*
                        0 -- This is the pointer to the array of elements to be written.
                        1 -- size in bytes of each element to be written.
                        2 -- number of elements, each one with a size of size bytes.
                        3 -- pointer to a FILE object that specifies an output stream.
                        */
                        numBytesWritten +=
                            fwrite(
                                dataMsg->data,
                                sizeof(dataMsg->data[0]),
                                dataMsg->dataLength,
                                fp);
                        waitForDataMsgSeq1 = false;

                        dataAckMsg = create_data_ack_message(dataMsg->sequenceNumber);
                        send_rftp_message(sockfd, dataAckMsg, &client);
                        free(dataAckMsg);
                        free(dataMsg);

                        dataChunkCount++;
                    }
                    else if (dataMsg->sequenceNumber == 0) {
                        syslog(LOG_DEBUG, "TYPE3_DATA_MSG SEQ0");
                        dataAckMsg = create_data_ack_message(dataMsg->sequenceNumber);
                        send_rftp_message(sockfd, dataAckMsg, &client);
                        free(dataAckMsg);
                        free(dataMsg);
                    }
                    break;
                default:
                    syslog(LOG_DEBUG, "unsupported message type: %d", dataMsg->type);
                    exit(EXIT_FAILURE);
                    break;
            }
        }

        if (!transferringFile)
            break;

        syslog(LOG_DEBUG, "Step 2");

        bool   waitForDataMsgSeq2 = true;
        while (waitForDataMsgSeq2)
        {
            dataMsg = (rftp_data_message_t*) receive_rftp_message(sockfd, &client);
            syslog(LOG_DEBUG, "received data message 2");

            switch (dataMsg->type) {
                case TYPE3_DATA_MSG:
                    if (dataMsg->sequenceNumber == 0) {
                        syslog(LOG_DEBUG, "TYPE3_DATA_MSG SEQ0");

                        numBytesWritten +=
                            fwrite(
                                dataMsg->data,
                                sizeof(dataMsg->data[0]),
                                dataMsg->dataLength,
                                fp);

                        dataAckMsg = create_data_ack_message(dataMsg->sequenceNumber);
                        send_rftp_message(sockfd, dataAckMsg, &client);
                        free(dataAckMsg);
                        free(dataMsg);

                        waitForDataMsgSeq2 = false;
                        dataChunkCount++;
                    }
                    else if (dataMsg->sequenceNumber == 1) {
                        syslog(LOG_DEBUG, "TYPE3_DATA_MSG SEQ1");
                        dataAckMsg = create_data_ack_message(dataMsg->sequenceNumber);
                        send_rftp_message(sockfd, dataAckMsg, &client);
                        free(dataAckMsg);
                        free(dataMsg);
                        waitForDataMsgSeq2 = false;
                    }
                    break;
                case TYPE2_CTRL_MSG_TERMINATION:
                    transferringFile = false;
                    type2CtrlMsgSeqNum = ctrlRequestMsg->sequenceNumber;
                    syslog(LOG_DEBUG, "TYPE2_CTRL_MSG_TERMINATION");
                    break;
                default:
                    syslog(LOG_DEBUG, "unsupported message type: %d", dataMsg->type);
                    exit(EXIT_FAILURE);
                    break;
            }
        }

        if (dataChunkCount > 6) {
            char buff[100];
            sprintf(buff, "Num Bytes Transmitted: %d\n"\
                             "Total Num Bytes:       %d\n"\
                             "Percentage Complete:   %d\n",
                             numBytesWritten, 
                             fileSize,
                             (int)(((float)numBytesWritten/(float)fileSize) * 100));
            syslog(LOG_DEBUG, "%s", buff);
            printf("%s", buff);
            dataChunkCount = 0;
        }
    }

    char buff[100];
    sprintf(buff, "Num Bytes Transmitted: %d\n"\
            "Total Num Bytes:       %d\n"\
            "Percentage Complete:   %d\n",
            numBytesWritten,
            fileSize,
            (int)(((float)numBytesWritten/(float)fileSize) * 100));
    syslog(LOG_DEBUG, "%s", buff);
    printf("%s", buff);

    /*
    close file
    send type 2 control ack
    start TIME_WAIT timer
    */
    fclose(fp);

    syslog(LOG_DEBUG, "Termination");

    //bool type2MsgReceived = true;

    rftp_message_t* type2CtrlMsg = create_type2_control_ack_response(type2CtrlMsgSeqNum);
    send_rftp_message(sockfd, type2CtrlMsg, &client);

    //while (type2MsgReceived) {
    //    rftp_control_message_t* endMsg =
    //        (rftp_control_message_t*) receive_rftp_message_with_timeout(sockfd, timewait, &client);
    //    syslog(LOG_DEBUG, "Received termination message");

    //    if (endMsg == NULL) {
    //        type2MsgReceived = true;
    //    } else {
    //        if (endMsg->type == TYPE2_CTRL_MSG_TERMINATION) {
    //            type2MsgReceived = false;
    //            free(endMsg);
    //            syslog(LOG_DEBUG, "TYPE2_CTRL_MSG_TERMINATION");
    //        } else {
    //            send_rftp_message(sockfd, type2CtrlMsg, &client);
    //            free(endMsg);
    //            syslog(LOG_DEBUG, "not termination");
    //        }
    //    }
    //}
    //free(type2CtrlMsg);

    // Close the socket
    close(sockfd);

    syslog(LOG_DEBUG, "File transferred successfully");

    exit(EXIT_SUCCESS);
}


