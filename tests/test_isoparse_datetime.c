/**
 * @file tests_unix_utc.h
 * @brief Test cases for ISO8601 datetime parsing
 * @author Ulysse Moreau
 * @date 2025-05-20
 * @version 2.0
 * @license WTFPL (Do What The F*ck You Want To Public License)
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://www.wtfpl.net/ for more details.
 *
 */

#include <stdio.h>
#include <string.h>

#include "../iso8601_parser.h"
#include "../tinyutc.h"

#include "tests_common.h"

struct Iso8601TestCase test_cases[] = {
    {"Empty string test", "", {0, 0, 0, 0, 0, 0, 0}, TINYUTC_ISO8601_EMPTY_STRING},
    {"Zulu time, date&time separator", "2024-02-29T17:05:03Z", {2024, 02, 29, 17, 05, 03, 0}, TINYUTC_ISO8601_OK},
    {"Zulu time, no date separator", "20240429T17:05:03Z", {2024, 04, 29, 17, 05, 03, 0}, TINYUTC_ISO8601_OK},
    {"Inconsistent date separator", "202607-08T17:05:03Z", {0, 0, 0, 0, 0, 0, 0}, TINYUTC_ISO8601_INVALID_DATE},
    {"No time separator", "2025-01-02T175503Z", {2025, 01, 02, 17, 55, 03, 0}, TINYUTC_ISO8601_OK},
    {"Inconsistent time separator 1", "2026-07-08T1705:03Z", {0, 0, 0, 0, 0, 0, 0}, TINYUTC_ISO8601_INCONSISTENT_TIME_SEPARATOR},
    {"Inconsistent time separator 2", "1972-12-31T17:0503Z", {0, 0, 0, 0, 0, 0, 0}, TINYUTC_ISO8601_INCONSISTENT_TIME_SEPARATOR},
    {"Time offset +01:01", "1972-12-31T17:05:03+01:01", {1972, 12, 31, 16, 04, 03, 0}, TINYUTC_ISO8601_OK},
    {"Time offset -0102", "1972-12-31T17:05:03-0102", {1972, 12, 31, 18, 07, 03, 0}, TINYUTC_ISO8601_OK},
    {"Time offset +2359", "1972-12-31T17:05:03+2359", {1972, 12, 30, 17, 06, 03, 0}, TINYUTC_ISO8601_OK},
    {"Time offset +2435", "1972-12-31T17:05:03+2435", {0, 0, 0, 0, 0, 0, 0}, TINYUTC_ISO8601_INVALID_OFFSET},
    {"Offset +0000", "1972-12-31T17:05:03+0000", {1972, 12, 31, 17, 05, 03, 0}, TINYUTC_ISO8601_OK},
    {"Offset changing a year", "1972-12-31T23:59:59-0100", {1973, 01, 01, 00, 59, 59, 0}, TINYUTC_ISO8601_OK},
    {"Incomplete offset 1", "1972-12-31T17:05:03+00:1", {0, 0, 0, 0, 0, 0, 0}, TINYUTC_ISO8601_INVALID_OFFSET},
    {"Incomplete offset +004", "1972-12-31T17:05:03+004", {0, 0, 0, 0, 0, 0, 0}, TINYUTC_ISO8601_INVALID_OFFSET},
    {"Leap second, no offset", "1972-12-31T17:05:60+0000", {1972, 12, 31, 17, 05, 60, 0}, TINYUTC_ISO8601_OK},
    {"Offset changing a year with leap second", "1972-12-31T23:59:60-0100", {1973, 01, 01, 00, 59, 60, 0}, TINYUTC_ISO8601_OK},
    {"Uncommon date with week number", "2000-W03-7T01:23:45", {2000, 01, 23, 01, 23, 45, 0}, TINYUTC_ISO8601_OK},
    {"Uncommon date with week number, no separator", "2000W037T01:23:45", {2000, 01, 23, 01, 23, 45, 0}, TINYUTC_ISO8601_OK},
    {"Uncommon date with week number & time offset", "2000-W03-7T01:23:45+09:00", {2000, 01, 22, 16, 23, 45, 0}, TINYUTC_ISO8601_OK},
    {"Uncommon date with ordinal day 23", "2000-023T01:23:45+09:00", {2000, 01, 22, 16, 23, 45, 0}, TINYUTC_ISO8601_OK},
    {"Uncommon date with ordinal day 100", "2000-100T01:23:45+09:00", {2000, 04, 8, 16, 23, 45, 0}, TINYUTC_ISO8601_OK},
    {"Uncommon date, ordinal day, no separator", "1989365T01:23:45Z", {1989, 12, 31, 01, 23, 45, 0}, TINYUTC_ISO8601_OK},
    {"Uncommon date, week & day, no separator", "2013W217T01:23:45", {2013, 05, 26, 01, 23, 45, 0}, TINYUTC_ISO8601_OK},
    {"Partial time, only hours", "1972-12-31T17", {1972, 12, 31, 17, 00, 00, 0}, TINYUTC_ISO8601_OK},
    {"Partial time, separator, hh:mm", "1972-12-31T17:05", {1972, 12, 31, 17, 05, 00, 0}, TINYUTC_ISO8601_OK},
    {"Partial time, separator, hh:mm:s", "1972-12-31T17:05:3", {1972, 12, 31, 17, 05, 03, 0}, TINYUTC_ISO8601_OK},
    {"Microseconds, .02", "2000-W03-7T01:23:45.02", {2000, 01, 23, 01, 23, 45, 20000}, TINYUTC_ISO8601_OK},
    {"Microseconds, .3", "2000-W03-7T01:23:45.3", {2000, 01, 23, 01, 23, 45, 300000}, TINYUTC_ISO8601_OK},
    {"Microseconds, 111111, offset +11:00", "2000-023T01:23:45.111111+11:00", {2000, 01, 22, 14, 23, 45, 111111}, TINYUTC_ISO8601_OK},
    {"Microseconds, 999999, offset +11:00", "2000-023T01:23:45,999999+11:00", {2000, 01, 22, 14, 23, 45, 999999}, TINYUTC_ISO8601_OK},
    // {"Microseconds, 7777777", "2000-100T01:23:45.7777777+09:00", {2024, 02, 29, 17, 05, 03, 0}, TINYUTC_ISO8601_TIME_FRACTION_TOO_LONG},
    {"Microseconds, 7777777", "2000-100T01:23:45.7777777+09:00", {2024, 02, 29, 17, 05, 03, 0}, TINYUTC_ISO8601_TIME_FRACTION_TOO_LONG},
    {"Inconsistent date separator", "2025-0102T17:05:03Z", {0, 0, 0, 0, 0, 0, 0}, TINYUTC_ISO8601_INVALID_MAIN_SEPARATOR},
    {"24:00:00", "2025-01-02T24:00:00Z", {2025, 01, 03, 0, 0, 0, 0}, TINYUTC_ISO8601_OK},
    {"24:00:01", "2025-01-02T24:00:01Z", {0, 0, 0, 0, 0, 0, 0}, TINYUTC_ISO8601_INVALID_TIME},
    {"24:01:00", "2025-01-02T24:01:00Z", {0, 0, 0, 0, 0, 0, 0}, TINYUTC_ISO8601_INVALID_TIME},
};

int main()
{
    struct TinyUTCTime utc_tm = {0};
    int sucess_count = 0;

    char spacingdesc[51] = {' '};
    char spacingiso[36] = {' '};

    for (int i = 0; i < sizeof(test_cases) / sizeof(test_cases[0]); i++)
    {
        int parse_result = tinyutc_parse_iso8601_datetime(&utc_tm, test_cases[i].iso8601, true);
        int is_expected_time = compare_utc_structs_datetimes(&utc_tm, &test_cases[i].expected);
        if (test_cases[i].expected_code == parse_result && (parse_result != TINYUTC_ISO8601_OK || is_expected_time))
        {
            int desclength = strlen(test_cases[i].description);
            int spacelength = 50 - desclength;
            memset(spacingdesc, ' ', spacelength);
            spacingdesc[spacelength] = '\0';
            spacingdesc[50] = '\0';

            int isolength = strlen(test_cases[i].iso8601);
            spacelength = 35 - isolength;
            memset(spacingiso, ' ', spacelength);
            spacingiso[spacelength] = '\0';
            spacingiso[35] = '\0';

            if (parse_result == TINYUTC_ISO8601_OK)
            {
                printf("\033[38;5;2m\033[1m[SUCCESS]\033[39m\t Test '%s' %s : '%s' %s => %04d/%02d/%02d %02d:%02d:%02d (+%dus) \n", test_cases[i].description, spacingdesc, test_cases[i].iso8601, spacingiso, utc_tm.year, utc_tm.month, utc_tm.day,
                       utc_tm.hour, utc_tm.minute, utc_tm.second, utc_tm.microseconds);
            }
            else
            {
                printf("\033[38;5;2m\033[1m[SUCCESS]\033[39m\t Test '%s' %s : '%s' %s => %s\n", test_cases[i].description, spacingdesc, test_cases[i].iso8601, spacingiso, get_err_string(parse_result));
            }
            sucess_count++;
        }
        else
        {
            printf("4\n");
            printf("\033[38;5;1m\033[1m[FAILED]\033[39m\t Test '%s': %s : \n\t\t\t - Expected error code %d, got %d\n", test_cases[i].description, test_cases[i].iso8601, test_cases[i].expected_code, parse_result);
            if (!is_expected_time)
            {
                printf("\t\t\t - Expected time %04d-%02d-%02dT%02d:%02d:%02d (+%dus), got %04d-%02d-%02dT%02d:%02d:%02d (+%dus)\n",
                       test_cases[i].expected.year, test_cases[i].expected.month, test_cases[i].expected.day,
                       test_cases[i].expected.hour, test_cases[i].expected.minute, test_cases[i].expected.second, test_cases[i].expected.microseconds,
                       utc_tm.year, utc_tm.month, utc_tm.day,
                       utc_tm.hour, utc_tm.minute, utc_tm.second, utc_tm.microseconds);
            }
        }
    }
    printf("%d/%llu tests passed.\n", sucess_count, sizeof(test_cases) / sizeof(test_cases[0]));
}
