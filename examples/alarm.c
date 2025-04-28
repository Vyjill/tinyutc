#include <stdio.h>
#include "../tinyutc.h"

int main()
{

    struct TinyUTCTime utc_tm = {0, 55, 19, 28, 4, 2025}; // Example UTC time
    int32_t unix_ts = tinyutc_utc_to_unix(&utc_tm);

    printf("Unix Timestamp: %d\n", unix_ts);
    printf("UTC Time: %02d:%02d:%02d %02d/%02d/%04d\n", utc_tm.hour, utc_tm.minute, utc_tm.second, utc_tm.day, utc_tm.month, utc_tm.year);

    tinyutc_unix_to_utc(unix_ts, &utc_tm);
    printf("UTC Time: %02d:%02d:%02d %02d/%02d/%04d\n", utc_tm.hour, utc_tm.minute, utc_tm.second, utc_tm.day, utc_tm.month, utc_tm.year);

    printf("Setting up an alarm 10 days into the future\n");

    // int32_t alarm_ts = unix_ts + (10); // 10 days in seconds
    int32_t alarm_ts = unix_ts + (10 * _TINYUTC_SECS_PER_DAY); // 10 days in seconds

    tinyutc_unix_to_utc(alarm_ts, &utc_tm);

    printf("Unix Timestamp: %d\n", alarm_ts);
    printf("UTC Time: %02d:%02d:%02d %02d/%02d/%04d\n", utc_tm.hour, utc_tm.minute, utc_tm.second, utc_tm.day, utc_tm.month, utc_tm.year);
    
    
    struct TinyUTCTime current_time = {
        .year = 2025,
        .month = 4,
        .day = 28,
        .hour = 17,
        .minute = 55,
        .second = 19
    };

}