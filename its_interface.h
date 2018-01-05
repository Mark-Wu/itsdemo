//
// Created by silence on 17-12-29.
//

#ifndef ITSDEMO_ITS_INTERFACE_H
#define ITSDEMO_ITS_INTERFACE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <netdb.h>
#include <string.h>

/* message type */
#define     LINCENE_VAKIDATION  100
#define     WATCHDOG            0
#define     DATE_TIME           3

#define     DWELL               30

#define     ERROR               255


#pragma pack(1)

typedef struct {
    uint16_t length;
    uint8_t type;
} its_header;

typedef struct _licence_validation_req {
    its_header msg_header;
    char username_licence[96]; /* Username (up to 63 ASCII characters), followed by a back-slash (\),*/
                                    /* the licence string (32 ASCII characters) */
} licence_validation_req;

typedef struct _licence_validation_res {
    its_header msg_header;
    uint8_t status;
    uint8_t its_protcol_version;
    uint16_t host_version;
    uint8_t  host_build_number;         /* Host build number (0–255) */
} licence_validation_res;

typedef struct _data_time_res {
    its_header msg_header;
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
} data_time_res;

typedef struct _error_res {
    its_header msg_header;
    uint8_t invalid_message_type;
    uint8_t error_code;
} error_res;


typedef struct _dwell_req {
    its_header msg_header;
    uint8_t seq_number;
    uint8_t user_id;
    uint16_t site_id;
    uint8_t prefer_dwell_phase;
    uint8_t alernate_dwell_phase;
    uint16_t dwell_duration;
} dwell_req;



typedef struct dwell_res {
    its_header msg_header;
    uint8_t seq_number;
    uint8_t status;
} dwell_res;



#pragma pack()


size_t create_licence_validate_request_package(uint8_t *pbuffer,size_t max_size,const char *licence,const char *username);
int licence_validation_response_resolver(uint8_t *psource);
int protocol_resolver(uint8_t *psource,int length);
#endif //ITSDEMO_ITS_INTERFACE_H
