#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <inttypes.h>

struct ipsrvdb {
    int index_size;
    int data_size;
    int header_size;
    unsigned char *index;
    unsigned char *data;
    unsigned char *header;
    unsigned char *date;
    unsigned char *description;
};

typedef struct ipsrvdb ipsrvdb_t;

ipsrvdb_t *ipsrv_open_database(char *filename);
char *ipsrv_find(ipsrvdb_t *db, char *ip);
