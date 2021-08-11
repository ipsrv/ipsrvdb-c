# ipsrvdb-c

# Feature
1. Support IPv4 & IPv6.
2. Support output db date, description and header.
3. Now using "find" function will just return a string of the IP info. You should split the string using the header youself.

# Example
```
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
```

# Build
```
gcc -g -std=c99 ipsrvdb.c test.c -o test
```

# Output
```
NA,北美洲,US,美国,,,,,,
AS,亚洲,CN,中国,11,北京市,,,中国教育网,
continent_code,continent_zh,country_iso_code,country_zh,province_iso_code,province_zh,city_code,city_zh,isp_zh,org
20210810
IPSrv, Inc. Dat database.
```
