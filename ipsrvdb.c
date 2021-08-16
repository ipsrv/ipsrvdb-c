#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <inttypes.h>
#include "ipsrvdb.h"

struct ipaddr {
    unsigned char address[16];
};

typedef struct ipaddr ipaddr_t;

ipaddr_t *ip_to_addr(char *ip) {
    ipaddr_t *addr = (ipaddr_t *)malloc(sizeof(ipaddr_t));
    memset(addr, 0, sizeof(ipaddr_t));
    uint32_t ip4 = inet_addr(ip);
    if (ip4 == INADDR_NONE) {
        struct in6_addr result;
        if (inet_pton(AF_INET6, ip, &result) == 1) { // success!
            memcpy(addr->address, result.s6_addr, 16);
        }
    } else {
        memcpy(addr->address + 12, &ip4, 4);
    }
    return addr;
}

ipsrvdb_t *ipsrv_open_database(char *filename) {
    FILE *fd = fopen(filename, "r");
    if (fd == NULL)
        return NULL;

    fseek(fd, 0, SEEK_END);
    int file_size = ftell(fd);
    rewind(fd);
    
    ipsrvdb_t *db = (ipsrvdb_t *)malloc(sizeof(ipsrvdb_t));
    memset(db, 0, sizeof(ipsrvdb_t));

    fread(&(db->index_size), 8, 1, fd);
    fread(&(db->data_size), 8, 1, fd);
    fread(&(db->header_size), 2, 1, fd);

    db->index = (unsigned char *)malloc(db->index_size * 24);
    db->data = (unsigned char *)malloc(db->data_size);
    db->header = (unsigned char *)malloc(db->header_size);
    db->date = (unsigned char *)malloc(8);

    int des_size = file_size - 26 - db->index_size * 24 - db->data_size - db->header_size + 1;
    db->description = (unsigned char *)malloc(des_size);
              
    fread(db->index, db->index_size * 24, 1, fd);
    fread(db->data, db->data_size, 1, fd);
    fread(db->header, db->header_size, 1, fd);
    fread(db->date, 8, 1, fd);
    fread(db->description, des_size, 1, fd);
    fclose(fd);

    return db;
}

void ipsrv_close_database(ipsrvdb_t *db) {
    if (db == NULL)
        return;
    if (db->index != NULL)
        free(db->index);
    if (db->data != NULL)
        free(db->data);
    if (db->header != NULL)
        free(db->header);
    if (db->date != NULL)
        free(db->date);
    if (db->description != NULL)
        free(db->description);
    free(db);
}

char *ipsrv_find(ipsrvdb_t *db, char *ip) {
    char *ret = NULL;
    ipaddr_t *addr = ip_to_addr(ip);
    uint32_t start = 0, 
             mid = 0, 
             end = (db->index_size) - 1;
    while(start <= end) {
        ipaddr_t unpacked;
        memset(&unpacked, 0, sizeof(ipaddr_t));
        mid = (start + end) / 2;
        memcpy(&(unpacked.address), (db->index)+mid*24, 16);

        int cmp = memcmp(&unpacked, addr, 16);
        if(cmp > 0) {
            end = mid;
        } else if(cmp < 0) {
            start = mid;
            if(start == end - 1) {
                uint32_t offset, len;
                memcpy(&offset, (db->index)+mid*24+16, sizeof(uint32_t));
                memcpy(&len, db->index+mid*24+20, sizeof(uint32_t));
                int datalen = sizeof(char) * (len + 1);
                char *data = (char *)malloc(datalen);
                memset(data, 0, datalen);
                memcpy(data, db->data+offset, len);
                ret = data;
                break;
            }
        } else if(cmp == 0) {
            uint32_t offset, len;
            memcpy(&offset, (db->index)+mid*24+16, sizeof(uint32_t));
            memcpy(&len, db->index+mid*24+20, sizeof(uint32_t));
            int datalen = sizeof(char) * (len + 1);
            char *data = (char *)malloc(datalen);
            memset(data, 0, datalen);
            memcpy(data, db->data+offset, len);
            ret = data;
            break;
        }
    }

    free(addr);
    return ret;
}
