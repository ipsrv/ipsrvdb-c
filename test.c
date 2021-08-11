#include <stdio.h>
#include "ipsrvdb.h"

int main(int argc, char *argv[]) {
    ipsrvdb_t *db = ipsrv_open_database("/path/to/ipsrvdb.dat");
    if (db == NULL) {
        printf("Failed to open database!\n");
        return 1;
    }
    printf("%s\n", ipsrv_find(db, "8.8.8.255"));
    printf("%s\n", ipsrv_find(db, "2001:250::ffff"));
    printf("%s\n", db->header);
    printf("%s\n", db->date);
    printf("%s\n", db->description);

    return 0;
}
