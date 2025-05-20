#include <stdio.h>
#include "../tinyutc.h"

int main()
{
    err_t result;
    // Example Unix timestamp
    tinyutc_time_t ts = 1696041600;
    struct TinyUTCTime utc_tm;
    result = tinyutc_unix_to_utc(&utc_tm, ts);
    if (result != 0)
    {
        printf("Error converting Unix timestamp to UTC time: %d\n", result);
    }
    else
    {
        printf("UTC Time: %02d:%02d:%02d %02d/%02d/%04d\n", utc_tm.hour, utc_tm.minute, utc_tm.second, utc_tm.day, utc_tm.month, utc_tm.year);
    }

    // Example UTC time
    struct TinyUTCTime utc_time = {0, 55, 19, 28, 4, 2025};
    int32_t unix_ts;
    result = tinyutc_utc_to_unix(&utc_time, &unix_ts);
    if (result != 0)
    {
        printf("Error converting UTC time to Unix timestamp: %d\n", result);
    }
    else
    {
        printf("Unix Timestamp: %d\n", unix_ts);
    }

    return 0;
}