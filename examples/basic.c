#include <stdio.h>
#include "../tinyutc.h"

int main()
{
    int ts = 1696041600; // Example Unix timestamp
    struct TinyUTCTime utc_tm;
    tinyutc_unix_to_utc(ts, &utc_tm);
    printf("UTC Time: %02d:%02d:%02d %02d/%02d/%04d\n", utc_tm.hour, utc_tm.minute, utc_tm.second, utc_tm.day, utc_tm.month, utc_tm.year);
    
    ts = 1745862878; // Example Unix timestamp
    tinyutc_unix_to_utc(ts, &utc_tm);
    printf("UTC Time: %02d:%02d:%02d %02d/%02d/%04d\n", utc_tm.hour, utc_tm.minute, utc_tm.second, utc_tm.day, utc_tm.month, utc_tm.year);

    ts = 0; // Example Unix timestamp
    tinyutc_unix_to_utc(ts, &utc_tm);
    printf("UTC Time: %02d:%02d:%02d %02d/%02d/%04d\n", utc_tm.hour, utc_tm.minute, utc_tm.second, utc_tm.day, utc_tm.month, utc_tm.year);

    ts = 1; // Example Unix timestamp
    tinyutc_unix_to_utc(ts, &utc_tm);
    printf("UTC Time: %02d:%02d:%02d %02d/%02d/%04d\n", utc_tm.hour, utc_tm.minute, utc_tm.second, utc_tm.day, utc_tm.month, utc_tm.year);

    ts = 24*60*60; // Example Unix timestamp
    tinyutc_unix_to_utc(ts, &utc_tm);
    printf("UTC Time: %02d:%02d:%02d %02d/%02d/%04d\n", utc_tm.hour, utc_tm.minute, utc_tm.second, utc_tm.day, utc_tm.month, utc_tm.year);

    ts = 364*24*60*60; // Example Unix timestamp
    tinyutc_unix_to_utc(ts, &utc_tm);
    printf("UTC Time: %02d:%02d:%02d %02d/%02d/%04d\n", utc_tm.hour, utc_tm.minute, utc_tm.second, utc_tm.day, utc_tm.month, utc_tm.year);

    ts = 365*24*60*60; // Example Unix timestamp
    tinyutc_unix_to_utc(ts, &utc_tm);
    printf("UTC Time: %02d:%02d:%02d %02d/%02d/%04d\n", utc_tm.hour, utc_tm.minute, utc_tm.second, utc_tm.day, utc_tm.month, utc_tm.year);

    ts = 366*24*60*60; // Example Unix timestamp
    tinyutc_unix_to_utc(ts, &utc_tm);
    printf("UTC Time: %02d:%02d:%02d %02d/%02d/%04d\n", utc_tm.hour, utc_tm.minute, utc_tm.second, utc_tm.day, utc_tm.month, utc_tm.year);

    ts = (365*24*60*60)*4; // Example Unix timestamp
    tinyutc_unix_to_utc(ts, &utc_tm);
    printf("UTC Time: %02d:%02d:%02d %02d/%02d/%04d\n", utc_tm.hour, utc_tm.minute, utc_tm.second, utc_tm.day, utc_tm.month, utc_tm.year);

    ts = (365*24*60*60)*4+_TINYUTC_SECS_PER_DAY; // Example Unix timestamp
    tinyutc_unix_to_utc(ts, &utc_tm);
    printf("UTC Time: %02d:%02d:%02d %02d/%02d/%04d\n", utc_tm.hour, utc_tm.minute, utc_tm.second, utc_tm.day, utc_tm.month, utc_tm.year);

    struct TinyUTCTime utc_time = {0, 55, 19, 28, 4, 2025}; // Example UTC time
    int32_t unix_ts = tinyutc_utc_to_unix(&utc_time);
    printf("Unix Timestamp: %d\n", unix_ts);
    return 0;       
}