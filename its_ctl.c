//
// Created by smart on 1/5/18.
//

#include "its_ctl.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <stdbool.h>
#include <stdint.h>
#include <netinet/in.h>

static int listen_fd;
static int com_fd;
extern bool __init;




/* Read a cmd short */
static int read_short(int fh, uint16_t *res)
{
    uint16_t cmd;

    if (read(fh,&cmd,sizeof(cmd))!=sizeof(cmd)) {
        /* print_serr(fh,"Bad arg."); */
        return 0;
    }
    *res= ntohs(cmd);
    return 1;
}

/* Read a cmd long */
static int read_long(int fh, uint32_t *res)
{
    uint32_t cmd;

    if (read(fh,&cmd,sizeof(cmd))!=sizeof(cmd)) {
        /* print_serr(fh,"Bad arg."); */
        return 0;
    }
    *res= ntohl(cmd);
    return 1;
}


int ctl_init(void)
{
    int ret,len;
    struct sockaddr_un clt_addr;
    struct sockaddr_un srv_addr;
    listen_fd=socket(PF_UNIX,SOCK_STREAM,0);
    if(listen_fd < 0) {
        perror("cannot create communication socket");
        return -1;
    }

    /* set server address param */
    srv_addr.sun_family=AF_UNIX;
    strncpy(srv_addr.sun_path,ITS_DOMAIN,sizeof(srv_addr.sun_path)-1);
    unlink(ITS_DOMAIN);

    /* bind socket fd & address */
    ret=bind(listen_fd,(struct sockaddr*)&srv_addr,sizeof(srv_addr));
    if(ret==-1) {
        perror("cannot bind server socket");
        close(listen_fd);
        unlink(ITS_DOMAIN);
        return -1;
    }
    return ret;
}


void *ctl_thread(void *p)
{
    /* listen socket fd */
    if(listen(listen_fd,1)==-1) {
        perror("cannot listen the client connect request");
        close(listen_fd);
        unlink(ITS_DOMAIN);
        return NULL;
    }
    while(__init){
        struct sockaddr_un ra;
        socklen_t res=sizeof(ra);
        int rs;
        if ((rs=accept(listen_fd,(struct sockaddr *)&ra,&res))!=-1){
            uint16_t cmd;
            if (read_short(rs,&cmd)){
                /* Check magic number in command */
                if((cmd & 0xffff) == CTL_MAGIC) {
                    const char *errmsg;
                    cmd &= 0xff;
                    switch(cmd){
                        case DATE_TIME:

                            break;
                        case DWELL:

                            break;
                        case STRATEGIC_MONITOR:

                        default:
                            break;
                    }
                }



                }
        }

    }



}


void ctl_end(void)
{
    close(com_fd);
    close(listen_fd);
    unlink(ITS_DOMAIN);
    return;
}

