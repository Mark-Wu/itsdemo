#include <stdio.h>
#include <getopt.h>
#include <stdbool.h>
#include <netinet/tcp.h>
#include <pthread.h>
#include "util.h"
#include "client.h"
#include "its_interface.h"


bool __init = true;
pthread_attr_t attr;
pthread_t tcps;
global_configs local_configs;

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

static void *package_receiver(void *param)
{
    int tcp_fd = *((int *)param);
    unsigned int nread = 0;
    uint8_t buffer[1024];
    int length = 0;
    unsigned int package_size;


    printf("client connected .\r\n");

    while(__init) {
        if (if_socket_connected(tcp_fd) != 1) {
            printf("connecting  closed.\r\n");
            break;
        }
        length = recv_msg(tcp_fd,buffer,1024);
        protocol_resolver(buffer,length);
    }
    printf("receiver exit.\r\n");
    return NULL;
}

int start_receiver(int fd)
{
    if (pthread_create(&tcps,&attr,package_receiver,&fd)) {
        printf("create thread failed.");
        close_socket(fd);
        exit(1);
    } else {
        printf("rec thread started.\r\n");
    }
    return (int)tcps;
}








static void *its_controller(void *param)
{
    int tcp_fd = *((int *)param);
    unsigned int nread = 0;
    uint8_t buffer[1024];
    int length = 0;
    unsigned int package_size;


    printf("client connected .\r\n");

    while(__init) {
        if (if_socket_connected(tcp_fd) != 1) {
            printf("connecting  closed.\r\n");
            break;
        }
        length = recv_msg(tcp_fd,buffer,1024);
        protocol_resolver(buffer,length);
    }
    printf("receiver exit.\r\n");
    return NULL;
}

int start_controller(int fd)
{
    if (pthread_create(&tcps,&attr,its_controller,&fd)) {
        printf("create thread failed.");
        close_socket(fd);
        exit(1);
    } else {
        printf("rec thread started.\r\n");
    }
    return (int)tcps;
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
        return 0;
    }

    /* initialize detach thread attribution */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);        /* PTHREAD_CREATE_JOINABLE  PTHREAD_CREATE_DETACHED*/

    start_receiver(socket_fd);
    while(__init){
         usleep(1000);

    }

    close_socket(socket_fd);

    pthread_join(tcps,NULL);
    return 0;
}