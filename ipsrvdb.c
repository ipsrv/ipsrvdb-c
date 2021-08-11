#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <inttypes.h>
#include "ipsrvdb.h"

struct ipsrvip {
    uint64_t high;
    uint64_t low;
};

typedef struct ipsrvip ipsrvip_t;

char ipcmp(ipsrvip_t *a, ipsrvip_t *b) {
    if (a->high == b->high) {
        if (a->low == b->low) {
            return 0;
        } else if (a->low > b->low) {
            return 1;
        }
        return -1;
    } else if(a->high > b->high) {
        return 1;
    }
    return -1;
}

ipsrvip_t *ip_to_int(char *ip) {
    ipsrvip_t *ipint = (ipsrvip_t *)malloc(sizeof(ipsrvip_t));
    memset(ipint, 0, sizeof(ipint));
    ipint->low = htonl(inet_addr(ip));
    if (ipint->low == INADDR_NONE) {
        struct in6_addr result;
        if (inet_pton(AF_INET6, ip, &result) == 1) { // success!
            int i;
            for (i=0; i<8; i++) {
                ipint->high = (ipint->high << 8) + result.s6_addr[i];
            }
            for (i=8; i<16; i++) {
                ipint->low = (ipint->low << 8) + result.s6_addr[i];
            }
        }
    }
    return ipint;
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

char *ipsrv_find(ipsrvdb_t *db, char *ip) {
    ipsrvip_t *ipint = ip_to_int(ip);
    uint64_t start = 0, 
             mid = 0, 
             end = (db->index_size) - 1;
    while(start <= end) {
        ipsrvip_t unpacked;
        mid = (start + end) / 2;
        memcpy(&(unpacked.high), db->index+mid*24, sizeof(uint64_t));
        memcpy(&(unpacked.low), db->index+mid*24+8, sizeof(uint64_t));
        if(ipcmp(&unpacked, ipint) > 0) {
            end = mid;
        } else if(ipcmp(&unpacked, ipint) < 0) {
            start = mid;
            if(start == end - 1) {
                uint32_t offset, len;
                memcpy(&offset, (db->index)+mid*24+16, sizeof(uint32_t));
                memcpy(&len, db->index+mid*24+20, sizeof(uint32_t));
                int datalen = sizeof(char) * (len + 1);
                char *data = (char *)malloc(datalen);
                memset(data, 0, datalen);
                memcpy(data, db->data+offset, len);
                return data;
            }
        } else if(ipcmp(&unpacked, ipint) == 0) {
            uint32_t offset, len;
            memcpy(&offset, (db->index)+mid*24+16, sizeof(uint32_t));
            memcpy(&len, db->index+mid*24+20, sizeof(uint32_t));
            int datalen = sizeof(char) * (len + 1);
            char *data = (char *)malloc(datalen);
            memset(data, 0, datalen);
            memcpy(data, db->data+offset, len);
            return data;
        }
    }
}
