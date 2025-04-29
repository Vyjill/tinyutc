#include "../tinyutc.h"

int main(){
    struct TinyUTCTime current_time = {
        .year = 2024,
        .month = 02,
        .day = 29,
        .hour = 17,
        .minute = 05,
        .second = 03,
    };

    // The tinyutc_time_t is described in the `# Types` section
    tinyutc_time_t current_timestamp = tinyutc_utc_to_unix(&current_time);

    // I want an alarm 10 days and 7 seconds in the future
    tinyutc_time_t alarm_timestamp = current_timestamp + 10 * _TINYUTC_SECS_PER_DAY + 7;

    struct TinyUTCTime alarm_time = {0};

    tinyutc_unix_to_utc(alarm_timestamp, &alarm_time);

    // Some microcontrollers needs the week day to exhaustively set it up.
    uint8_t week_day = tinyutc_get_week_day(&current_time);

    // Then, you can set up your RTC struct accordingly, and set up the correct alarm.
}
