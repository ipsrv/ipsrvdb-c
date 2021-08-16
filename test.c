#include <stdio.h>
#include "ipsrvdb.h"

int main(int argc, char *argv[]) {
    ipsrvdb_t *db = ipsrv_open_database("/path/to/ipsrv.dat");
    if (db == NULL) {
        printf("Failed to open database!\n");
        return 1;
    }
    char *ret1 = ipsrv_find(db, "8.8.8.255");
    char *ret2 = ipsrv_find(db, "2001:250::ffff");
    printf("%s\n", ret1);
    printf("%s\n", ret2);
    printf("%s\n", db->header);
    printf("%s\n", db->date);
    printf("%s\n", db->description);
    free(ret1);
    free(ret2);
    ipsrv_close_database(db);

    return 0;
}
