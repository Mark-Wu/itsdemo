//
// Created by silence on 17-12-29.
//

#ifndef ITSDEMO_UTIL_H
#define ITSDEMO_UTIL_H
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>



#define     FILE_BUFFER_MAXLEN  1024*1024




typedef struct {
    char username[128];     /* 64 bytes */
    char licence[64];       /* 32 bytes */
    char server[64];        /* ip or domain */
    int port;               /* server port number */
} global_configs;


/*
 输入:工作路径
 功能:设置程序后台工作
 */
void run_daemon();

/*
 输入:信号
 功能:设置信号集合
 */
typedef void (*sighandler_t)(int);
void signal_handler(int sig, sighandler_t handler);

int json_configs_read(const char *config_path);

#endif //ITSDEMO_UTIL_H
