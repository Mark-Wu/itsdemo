//
// Created by silence on 17-12-29.
//

#include "its_interface.h"


#define SITE_STATUS_FLAG                0b111011
#define SITE_DEFAULT_DATA_TYPE          0b1100001100111111
#define SITE_SPECIFIC_DATA_TYPE         0b1000001100111111



size_t create_licence_validate_request_package(uint8_t *pbuffer,size_t max_size,const char *licence,const char *username)
{
    size_t name_length = 0;
    licence_validation_req *ppackage = NULL;
    if(pbuffer == NULL || licence == NULL || username == NULL)
        return 0;

    name_length = strlen(username);
    ppackage = (licence_validation_req *)pbuffer;

    ppackage->msg_header.type = 100;
    memset(ppackage->username_licence,0x00,96);
    memcpy(ppackage->username_licence,username,name_length);
    ppackage->username_licence[name_length] = '\\';
    memcpy(ppackage->username_licence + name_length + 1,licence,32);
    ppackage->msg_header.length = htons((unsigned short)(name_length+34));

    return name_length+36;
}


size_t create_date_time_package(uint8_t *pbuffer,size_t max_size)
{
    its_header *header = (its_header *)pbuffer;
    header->length = htons(0);
    header->type = DATE_TIME;
    return sizeof(its_header);
}
int date_time_response_resolver(uint8_t *psource)
{
    data_time_res *ppayload = (data_time_res *)psource;
    printf("date:%d-%d-%d %d:%d:%d\r\n",ntohs(ppayload->year),ppayload->month,
                ppayload->day,ppayload->hour,ppayload->min,ppayload->sec);

    return 0;
}


int licence_validation_response_resolver(uint8_t *psource)
{
    licence_validation_res *ppayload = (licence_validation_res *)psource;
    if(ppayload->msg_header.type != LINCENE_VAKIDATION ){
        printf(" not a licence validation message.\r\n");
        return -1;
    }

    if(ppayload->status != 0){
        printf(" authorize failed:%d\r\n",ppayload->status);
        return -1;
    }
    printf(" protocol version:%d \r\n",ppayload->its_protcol_version);
    printf(" host version:%d \r\n",ntohs(ppayload->host_version));
    printf(" host build number:%d \r\n",ntohs(ppayload->host_build_number));

    return 0;
}


int error_resolver(uint8_t *psource)
{
    error_res *ppayload = (error_res *)psource;
    printf("message type of invalid message: %d \r\n",ppayload->invalid_message_type);
    printf("error code : %d \r\n",ppayload->error_code);
   return 0;
}



size_t create_dwell_package(uint8_t *pbuffer,size_t max_size)
{
    its_header *header = (its_header *)pbuffer;
    header->length = htons(0);
    header->type = DATE_TIME;
    return sizeof(its_header);
}


size_t create_site_status_package(uint8_t *pbuffer,size_t max_size,uint16_t site_id[],uint8_t count)
{
    size_t  length;
    int i;
    site_status_req *pheader = (site_status_req *)pbuffer;
    pheader->msg_header.type = 32;
    pheader->flags = SITE_STATUS_FLAG;
    pheader->default_data_type = htons(SITE_DEFAULT_DATA_TYPE);
    for(i = 0 ; i < count; i ++ ){
        pheader->sites_info[i].site_id = htons(site_id[i]);
        pheader->sites_info[i].specific_type = htons(SITE_SPECIFIC_DATA_TYPE);
    }
    length = sizeof(site_status_req) + count * sizeof(site_info);
    pheader->msg_header.length = htons((uint16_t)(length-2));
    printf(" site status length : %d \r\n ",length);
    return length;
}

size_t create_detector_count_pacakge(uint8_t *pbuffer,size_t max_size)
{
    detector_count_req *pheader = (detector_count_req *)pbuffer;
    pheader->msg_header.type = DETECTOR_COUNT;
    pheader->msg_header.length = htons(0x02);
    pheader->flag =0x01;
    printf("detector count package size:%d\r\n",sizeof(detector_count_req));
    return sizeof(detector_count_req);
}
size_t create_strategic_monitor_pacakge(uint8_t *pbuffer,size_t max_size)
{
    strategic_monitor_req *pheader = (strategic_monitor_req *)pbuffer;
    pheader->msg_header.type = STRATEGIC_MONITOR;
    pheader->flag = 0b11;
   // pheader->region_name =



    return 0;
}

int site_status_response_resolver(uint8_t *psource)
{
    return 0;
}

int protocol_resolver(uint8_t *psource,int length)
{
    its_header *header = NULL;
    header = (its_header *)psource;
    if(length == 0)
        return 0;
    printf("protocol resolver!\r\n");
    switch(header->type){
        case WATCHDOG:
            printf("watch dog message OK.\r\n");
            break;
        case DATE_TIME:
            date_time_response_resolver(psource);
            break;
        case ERROR:
            error_resolver(psource);
            break;
        case DWELL:
            break;
        case LINCENE_VAKIDATION:
            licence_validation_response_resolver(psource);
            break;
        case SITE_STATUS:
            site_status_response_resolver(psource);
            break;
        case STRATEGIC_MONITOR:
            break;
        case DETECTOR_COUNT:
            break;
        default:
            break;
    }
    printf("message type: %d \r\n",header->type);
    return 0;
}