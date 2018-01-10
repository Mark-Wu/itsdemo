//
// Created by silence on 17-12-29.
//
#include <netdb.h>
#include "client.h"
#include "its_interface.h"

int create_socket(char* server, int port)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sockfd) {
        printf("create socket failed, errno %d\n", errno);
        exit(1);
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(server);
    addr.sin_port = htons(port);
    if (-1 == connect(sockfd, (struct sockaddr *) &addr, sizeof(struct sockaddr))) {
        printf("connect failed, errno %d\n", errno);
        exit(1);
    }
    return sockfd;
};

int send_msg(int sockfd,char * sendBuff,size_t length)
{
    int send_size=0;
    if((send_size=(int)send(sockfd,sendBuff,length,0))<=0){
        herror("Send msg error!");
        return -1;
    }else
        return send_size;
}

int recv_msg(int sockfd,uint8_t *buffer,size_t max_size){
    ssize_t recv_length;
    unsigned int nread = 0,package_size;
    its_header *header = (its_header *)buffer;
    memset(buffer,0x00,max_size);


    if((recv_length = (int)recv(sockfd,buffer,2,0)) != -1){
        if (recv_length == 0) {
            usleep(1000);
            return 0;
        }else{
            header->length = ntohs(header->length);
            for (nread = 2;;) {
                if ((recv_length = recv(sockfd, buffer + nread, header->length, 0)) == -1) {
                    break;
                } else {
                    nread += recv_length;
                    if (nread < (header->length + 2)){
                        continue;
                    }
                    printf(" have received total frame :%d .\r\n ",nread);
                    int i = 0;
                    if(nread < 5)
                        for( i = 0; i < nread; i++ )
                            printf(" %02X",buffer[i]);
                        printf("\r\n");
                    break;
                }
            }
        }
    }

    return (int)nread;
}


int close_socket(int sockfd)
{
    close(sockfd);
    return 0;
}
