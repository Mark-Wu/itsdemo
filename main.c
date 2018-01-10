#include <stdio.h>
#include <getopt.h>
#include <stdbool.h>
#include <netinet/tcp.h>
#include <pthread.h>
#include "util.h"
#include "client.h"
#include "its_ctl.h"
#include "forwarding.h"
#include "queue_list.h"

bool __init = true;
pthread_attr_t attr;
pthread_t tcps,tctls,tfwd;
global_configs local_configs;
bool pacakge_for_scats = true;
bool global_status = false;

Queue *package_queue;
pthread_mutex_t package_queue_lock;

//int transfer_fd,scats_fd;

void my_quit(int signum)
{
    __init = false;
}

void set_signal(void)
{
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_handler = my_quit;
    sigaction(SIGTERM, &act, NULL);
    sigaction(SIGHUP, &act, NULL);
    sigaction(SIGINT, &act, NULL);
    return;
}

void print_usages(void)
{
    printf("\tusages:\r\n");
    printf("\t-c\t\"config file path.\"\r\n");
    return;
}
void print_versions(void)
{
    printf("\tversion:\r\n");
    printf("\t...\r\n");
    return;
}


static int if_socket_connected(int sock)
{
    if(sock<=0)
        return 0;
    struct tcp_info info;
    int len=sizeof(info);
    getsockopt(sock, IPPROTO_TCP, TCP_INFO, &info, (socklen_t *)&len);
    if((info.tcpi_state==TCP_ESTABLISHED))
        return 1;
    else
        return 0;
}




void *its_controller(void *param)
{

    return NULL;
}



int start_controller(int fd)
{
    if (pthread_create(&tctls,&attr,ctl_thread,&fd)) {
        printf("create ctl thread failed.");
        close_socket(fd);
        exit(1);
    } else {
        printf("ctl thread started.\r\n");
    }
    return (int)tctls;
}

int start_forwarding(void)
{
    if (pthread_create(&tfwd,&attr,forwarding_thread,NULL)) {
        printf("create fwd thread failed.");
        return -1;
    } else {
        printf("forwarding thread started.\r\n");
    }
    return (int)tfwd;
}

int main(int argc,char **argv) {

    struct option options[] = {
            {"config", required_argument, NULL,'c'},
            {"version", no_argument, NULL, 'v'},
            {"daemon", no_argument, NULL, 'd'},
            {"help", no_argument, NULL, 'h'}
    };
    bool run_as_daemon = false;
    bool print_usage = argc == 1;
    bool print_version = false;

    const char *optstring = "c:dvh";
    int opt;
    const char *config_dir = NULL;
    while((opt = getopt_long(argc, argv, optstring, options, NULL)) != -1) {
        switch (opt) {
            case 'c':
                config_dir = optarg;
                break;
            case 'v':
                print_version = true;
                break;
            case 'd':
                run_as_daemon  = true;
                break;
            case 'h':
            default:
                print_usage = true;
        }
    }
    if (print_version) {
        print_versions();
        return 0;
    }

    if (print_usage) {
        print_usages();
        return 0;
    }

    if (run_as_daemon) {
        run_daemon();
    }

    if(json_configs_read(config_dir) == -1) {
        perror("config read error");
        exit(1);
    }
    set_signal();

    /* initialize detach thread attribution */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);        /* PTHREAD_CREATE_JOINABLE  PTHREAD_CREATE_DETACHED*/
    int ctl_fd = ctl_init();
    start_controller(ctl_fd);



    int socket_fd = create_socket(local_configs.server,local_configs.port);

    uint8_t send_buffer[1024];
    uint8_t recv_buffer[1024];
    size_t package_size = create_licence_validate_request_package(send_buffer,1024,local_configs.licence,local_configs.username);
    send_msg(socket_fd,(char *)send_buffer,package_size);
    if (recv_msg(socket_fd,recv_buffer,1024) == -1) {
        perror("socket receive error");
        close(socket_fd);
        exit(1);
    }
    if(licence_validation_response_resolver(recv_buffer) == -1){
        close_socket(socket_fd);
        exit(1);
    }
    global_status = true;

#if 1
    package_size = create_detector_count_pacakge(send_buffer,1024);
    send_msg(socket_fd,(char *)send_buffer,package_size);
    if (recv_msg(socket_fd,recv_buffer,1024) == -1) {
        perror("socket receive error");
        close(socket_fd);
        exit(1);
    }

    uint16_t site_id[1] = {15};
    package_size = create_site_status_package(send_buffer,1024, site_id,1);

    send_msg(socket_fd,(char *)send_buffer,package_size);
    if (recv_msg(socket_fd,recv_buffer,1024) == -1) {
        perror("socket receive error");
        close(socket_fd);
        exit(1);
    }
#endif
    fd_set rfds;
    struct timeval tv;
    int retval, maxfd = -1;

    while(__init){
        if(pacakge_for_scats){      /* local package to scats server */

        }
        maxfd = 0;
        FD_ZERO(&rfds);
        FD_SET(socket_fd, &rfds);
        if (socket_fd > maxfd)
            maxfd = socket_fd;
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        retval = select(maxfd + 1, &rfds, NULL, NULL, &tv);
        if (retval == -1) {
            printf("select error: %s", strerror(errno));
            __init = false;
            break;
        }else if (retval == 0)
            continue;
        else{
            /* receive SCATS package here */
            if (FD_ISSET(socket_fd, &rfds)){
                int rec_len = recv_msg(socket_fd,recv_buffer,1024);
                if ( rec_len == -1) {
                    perror("socket receive error");
                    __init = false;
                }
                protocol_resolver(recv_buffer,rec_len);
            }
        }
    }

    close_socket(socket_fd);

    //pthread_join(tcps,NULL);
    return 0;
}