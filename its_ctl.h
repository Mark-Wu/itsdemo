//
// Created by smart on 1/5/18.
//

#ifndef ITSDEMO_ITS_CTL_H
#define ITSDEMO_ITS_CTL_H

#include <stdint.h>
#include "util.h"

#define ITS_DOMAIN      "/tmp/ITS.domain"


#define CTL_MAGIC   0x6600


#define     DATE_TIME           3

#define     DWELL               30
#define     SITE_STATUS         32
#define     STRATEGIC_MONITOR   33
#define     DETECTOR_COUNT      35


#pragma pack(1)

typedef struct{
    uint16_t cmd;
    uint16_t length;
    uint8_t payload[0];
}ctl_message;

#pragma pack()


int ctl_init(void);

void *ctl_thread(void *p);

void ctl_end(void);


#endif //ITSDEMO_ITS_CTL_H
