//
// Created by smart on 1/10/18.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdint.h>
#include <netdb.h>

#define ITS_DOMAIN      "/tmp/ITS.domain"
#define ITS_CMDVERNR 0x6600

#define CMD_HELP            0x01
#define CMD_VERSION         0x02
#define CMD_TIME            0x03
#define CMD_SITE_STATUS     0x04
#define CMD_STRATEGIC_MON   0x05
#define CMD_DETECTOR_COUNT  0x06

typedef struct {
    char *name;
    int  val;
} cmd_s;


#pragma pack(1)
typedef struct{
    uint16_t cmd;
    uint16_t length;
    uint8_t payload[0];
}ctl_message;
#pragma pack()


static short int verbose=1;
static const cmd_s its_cmds[]={
        {"datetime",CMD_TIME},
        {"site-status",CMD_SITE_STATUS},
        {"strategic-monitor",CMD_STRATEGIC_MON},
        {"detector-count",CMD_DETECTOR_COUNT},
        {NULL,0}
};


static void send_long(int fd,uint32_t cmd)
{
    uint32_t nc=htonl(cmd);

    if (write(fd,&nc,sizeof(nc))!=sizeof(nc)) {
        perror("Error: could not write long");
        close(fd);
        exit(2);
    }
}

static void send_short(int fd,uint16_t cmd)
{
    uint16_t nc=htons(cmd);

    if (write(fd,&nc,sizeof(nc))!=sizeof(nc)) {
        perror("Error: could not write short");
        close(fd);
        exit(2);
    }
}


static uint16_t read_short(int fd)
{
    ssize_t err;
    uint16_t nc;

    if ((err=read(fd,&nc,sizeof(nc)))!=sizeof(nc)) {
        fprintf(stderr,"Error: could not read short: %s\n",err<0?strerror(errno):"unexpected EOF");
        close(fd);
        exit(2);
    }
    return ntohs(nc);
}

static int match_cmd(const char *cmd, const cmd_s cmds[])
{
    int i;
    for(i=0; cmds[i].name; ++i) {
        if (strcasecmp(cmd,cmds[i].name)==0)
            return cmds[i].val;
    }
    return -1;
}

static int open_sock(const char *its_dir)
{
    struct sockaddr_un *sa;
    unsigned int sa_len;
    int sock;


    if ((sock=socket(PF_UNIX,SOCK_STREAM,0))==-1) {
        perror("Error: could not open socket");
        exit(2);
    }

    sa=(struct sockaddr_un *)alloca(sa_len+1);
    sa->sun_family=AF_UNIX;
    stpcpy(sa->sun_path,its_dir);

    if (connect(sock,(struct sockaddr *)sa,sa_len)==-1) {
        fprintf(stderr,"Error: could not open socket %s: %s\n",sa->sun_path,strerror(errno));
        close(sock);
        exit(2);
    }
    if(verbose) printf("Opening socket %s\n",sa->sun_path);
#if 0
    /* Send command code */

    nc=htons(cmd|ITS_CMDVERNR); /* Add magic number, convert to network byte order. */

    if (write(sock,&nc,sizeof(nc))!=sizeof(nc)) {
        perror("Error: could not write command code");
        close(sock);
        exit(2);
    }
#endif
    return sock;
}

int date_time_message(void)
{
    uint16_t nc;
    uint8_t buffer[512];
    int sock = open_sock(ITS_DOMAIN);

    /* Send command code */

    nc=htons(CMD_TIME|ITS_CMDVERNR); /* Add magic number, convert to network byte order. */

    if (write(sock,&nc,sizeof(nc))!=sizeof(nc)) {
        perror("Error: could not write command code");
        close(sock);
        exit(2);
    }
    return 0;
}
int main(int argc, char *argv[])
{
    int i;
    for (i=1;i<argc;i++){
        char *arg=argv[i];
        if (strcmp(arg,"-h")==0 || strcmp(arg,"--help")==0) {
            /* show help  */
            exit(1);
        } else if (strcmp(arg,"-v")==0 || strcmp(arg,"--version")==0) {
            /* show version */
            exit(1);
        } else if (strcmp(arg,"-D")==0 || strcmp(arg,"--date-time")==0) {
            /* send data time cmd */




        }else if (strcmp(arg,"-S")==0 || strcmp(arg,"--site-status")==0) {
            /* send site status cmd */
        } else if (strcmp(arg,"-d")==0 || strcmp(arg,"--detector-count")==0) {
            /* send detector count cmd */
        } else if (strcmp(arg,"-s")==0 || strcmp(arg,"--strategic-monitor")==0) {
            /* send strategic monitor cmd */
        } else {
            /* show help */
        }

    }
    return 0;
}



