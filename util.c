//
// Created by silence on 17-12-29.
//
#include "util.h"
#include "jRead.h"
extern global_configs local_configs;
struct FileBuffer{
    unsigned long length;			// length in bytes
    unsigned char *data;			// malloc'd data, free with freeFileBuffer()
};

void signal_handler(int sig, sighandler_t handler) {
    struct sigaction action;
    action.sa_handler = handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(sig, &action, NULL);
}

void run_daemon() {
    pid_t pid;
    if ((pid = fork()) < 0) {
        fprintf(stderr, "daemon can't fork\n");
        exit(1);
    } else if (pid != 0) {
        exit(0);
    }
    setsid();

    signal_handler(SIGHUP, SIG_IGN);
    signal_handler(SIGTERM, SIG_IGN);
    signal_handler(SIGPIPE, SIG_IGN);
}

static unsigned int fast_atoi( char *p )
{
    unsigned int x = 0;
    while (*p >= '0' && *p <= '9') {
        x = (x*10) + (*p - '0');
        ++p;
    }
    return x;
};

void freeFileBuffer( struct FileBuffer *buf )
{
    if( buf->data != NULL )
        free( buf->data );
    buf->data= 0;
    buf->length= 0;
}

unsigned long readFileBuffer( const char *filename, struct FileBuffer *pbuf, unsigned long maxlen )
{
    FILE *fp;
    int i;

    if( (fp=fopen(filename, "rb")) == NULL )
    {
        printf("Can't open file: %s\n", filename );
        return 0;
    }
    // find file size and allocate buffer for JSON file
    fseek(fp, 0L, SEEK_END);
    pbuf->length = ftell(fp);
    if( pbuf->length >= maxlen )
    {
        fclose(fp);
        return 0;
    }
    // rewind and read file
    fseek(fp, 0L, SEEK_SET);
    pbuf->data= (unsigned char *)malloc( pbuf->length + 1 );
    memset( pbuf->data, 0, pbuf->length+1 );	// +1 guarantees trailing \0

    i= fread( pbuf->data, pbuf->length, 1, fp );
    fclose( fp );
    if( i != 1 )
    {
        freeFileBuffer( pbuf );
        return 0;
    }
    return pbuf->length;
}

int json_configs_read(const char *config_path)
{
    int ret = 0;
    struct FileBuffer json;
    struct jReadElement jElement;

    if( readFileBuffer( config_path, &json, FILE_BUFFER_MAXLEN ) == 0 )
    {
        printf("Can't open file: %s\n", config_path );
        ret = -1;
        return ret;
    }
    // perform query on JSON file
    jRead( (char *)json.data, "{'licence'" , &jElement );
    memcpy(local_configs.licence,jElement.pValue,(size_t)jElement.bytelen);
    printf( " licence =%s\r\n",local_configs.licence);

    jRead( (char *)json.data, "{'name'" , &jElement );
    memcpy(local_configs.username,jElement.pValue,(size_t)jElement.bytelen);
    printf( " user name =%s\r\n",local_configs.username);

    jRead( (char *)json.data, "{'server'" , &jElement );
    memcpy(local_configs.server,jElement.pValue,(size_t)jElement.bytelen);
    printf( " server =%s\r\n",local_configs.server);

    jRead( (char *)json.data, "{'port'" , &jElement );
    local_configs.port = fast_atoi(jElement.pValue);
    printf( " port = %d \r\n",local_configs.port);

#if 0
    jRead( (char *)json.data, "{'band rate'" , &jElement );
    band_rate = fast_atoi(jElement.pValue);
    printf( " band rate = %d \r\n",band_rate);
    jRead( (char *)json.data, "{'flow control'" , &jElement );
    flow_ctl = fast_atoi(jElement.pValue);
    printf( " flow control = %d \r\n",flow_ctl);
    jRead( (char *)json.data, "{'data bits'" , &jElement );
    data_bits = fast_atoi(jElement.pValue);
    printf( " data bits = %d \r\n",data_bits);
    jRead( (char *)json.data, "{'stop bits'" , &jElement );
    stop_bits = fast_atoi(jElement.pValue);
    printf( " stop bits = %d \r\n",stop_bits);
    jRead( (char *)json.data, "{'parity'" , &jElement );
    memcpy(parity,jElement.pValue,1);
    printf(" parity = %c\r\n",parity[0]);
    jRead( (char *)json.data, "{'rec delay'" , &jElement );
    rec_delay = fast_atoi(jElement.pValue);
    printf( " rec delay = %d ms\r\n",rec_delay);
#endif
    freeFileBuffer( &json );

    return  ret;

}