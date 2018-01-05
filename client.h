//
// Created by silence on 17-12-29.
//

#ifndef ITSDEMO_CLIENT_H
#define ITSDEMO_CLIENT_H
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <inttypes.h>
#include "its_interface.h"

#define REC_BUFFER_SIZE 1024

int create_socket(char* server, int port);
int send_msg(int sockfd,char * sendBuff,size_t length);
int recv_msg(int sockfd,uint8_t *buffer,size_t max_size);
int close_socket(int sockfd);

#endif //ITSDEMO_CLIENT_H
