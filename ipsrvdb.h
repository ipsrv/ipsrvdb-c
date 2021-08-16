#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <inttypes.h>

struct ipsrvdb {
    uint64_t index_size;
    uint64_t data_size;
    uint64_t header_size;
    unsigned char *index;
    unsigned char *data;
    unsigned char *header;
    unsigned char *date;
    unsigned char *description;
};

typedef struct ipsrvdb ipsrvdb_t;

ipsrvdb_t *ipsrv_open_database(char *filename);
void ipsrv_close_database(ipsrvdb_t *db);
char *ipsrv_find(ipsrvdb_t *db, char *ip);
