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
#define     SITE_STATUS         32
#define     STRATEGIC_MONITOR   33
#define     DETECTOR_COUNT      35
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

typedef struct {
    its_header msg_header;
    uint8_t invalid_message_type;
    uint8_t error_code;
} error_res;


typedef struct  {
    its_header msg_header;
    uint8_t seq_number;
    uint8_t user_id;
    uint16_t site_id;
    uint8_t prefer_dwell_phase;
    uint8_t alernate_dwell_phase;
    uint16_t dwell_duration;
} dwell_req;


typedef struct {
    its_header msg_header;
    uint8_t seq_number;
    uint8_t status;
} dwell_res;

typedef struct {
    uint16_t site_id;
    uint16_t specific_type;
}site_info;

typedef struct {
    its_header msg_header;
    uint8_t flags;
    uint16_t default_data_type;
    site_info sites_info[0];
} site_status_req;

typedef struct {
    uint16_t site_id;
    uint8_t flags;
    uint16_t site_status;
    uint32_t alarm_flags;
    uint32_t detector_alarms;
    uint8_t plan_number;
    uint8_t special_facilities;
    uint8_t locks;
    uint16_t phase_data;
    uint16_t phase_times;
    uint16_t phase_status;
    uint8_t flags_content;
    uint16_t extra_special_facility_flags;
    uint16_t miscellaneous_status_flags;
    uint16_t remote_special_facility_flags;
    uint8_t Z_flags;
    uint32_t signal_group_data;
    uint8_t phase_demands;
    uint8_t subsystem_number;
    uint8_t Congestion_index_and_marriage;
    uint16_t nominal_cycle_length_seconds;
    uint16_t required_cycle_length_seconds;
}site_status_data;

typedef struct {
    its_header msg_header;
    uint16_t time_stamp;    /* Time stamp in seconds since beginning of current hour (0–3599) */
    site_status_data sites_status_data[0];
} site_status_data_res;


typedef struct {
    uint16_t site_id;
    uint8_t phase_data;
    uint8_t lamp_status;
}site_status_phase_change;
typedef struct {
    its_header msg_header;
    uint16_t time_stamp;    /* Time stamp in seconds since beginning of current hour (0–3599) */
    site_status_phase_change sites_status_phase_change[0];
}site_status_phase_change_res;

typedef struct {
    uint8_t inaccessible_number_sites;
    uint16_t site_id[0];
}inaccessible_sites;

typedef struct {
    uint8_t subtype;
}site_message_subtype;

typedef struct {
    uint8_t unknown_number_sites;
    uint16_t site_id[0];
}unknown_sites;

typedef struct{
    its_header msg_header;
    uint16_t time_stamp;    /* Time stamp in seconds since beginning of current hour (0–3599) */
    uint8_t message_subtype;
    unknown_sites unknown_sites_numbers;
}site_status_acknowledgement_res;



typedef struct {
    its_header msg_header;
    uint8_t flag;           /* Bit 0: Start/stop flag  Bit 1: Single-shot flag  */
    int8_t region_name[6];
    uint8_t subsystem_bitmap[0]; /* up to 32 bytes */
}strategic_monitor_req;

typedef struct {
    its_header msg_header;
    uint8_t status;
    int8_t region_name[6];
    uint8_t record[0];
}strategic_monitor_res;

typedef struct {
    its_header msg_header;
    int8_t flag;
}detector_count_req;
typedef struct {
    uint16_t site_id;
    uint8_t number_of_detector;
    uint8_t detector_id[0];
}detector_site;

typedef struct {
    its_header msg_header;
    uint8_t count_type;
    uint16_t date;
    uint16_t time;
    detector_site detector_sites[0];
}detector_count_res;

#pragma pack()




size_t create_licence_validate_request_package(uint8_t *pbuffer,size_t max_size,const char *licence,const char *username);
int licence_validation_response_resolver(uint8_t *psource);
int protocol_resolver(uint8_t *psource,int length);
size_t create_site_status_package(uint8_t *pbuffer,size_t max_size,uint16_t site_id[],uint8_t count);
int site_status_response_resolver(uint8_t *psource);
size_t create_strategic_monitor_pacakge(uint8_t *pbuffer,size_t max_size);
size_t create_detector_count_pacakge(uint8_t *pbuffer,size_t max_size);

#endif //ITSDEMO_ITS_INTERFACE_H
