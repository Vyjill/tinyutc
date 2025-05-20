/**
 * @file tests_unix_utc.h
 * @brief Test cases for ISO8601 time parsing
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
    {"Zulu time, with separator", "17:05:03Z", {0, 0, 0, 17, 05, 03, 0}, TINYUTC_ISO8601_OK},
    {"Zulu time, no separator", "17:05:03Z", {0, 0, 0, 17, 05, 03, 0}, TINYUTC_ISO8601_OK},
    {"Zulu time, no separator, with T", "T17:05:03Z", {0, 0, 0, 17, 05, 03, 0}, TINYUTC_ISO8601_OK},
    {"Not zulu time", "T17:05:03+01:01", {0, 0, 0, 0, 0, 0, 0}, TINYUTC_ISO8601_UTC_OFFSET_WITHOUT_DATE},
    {"Offset +0000", "17:05:03+0000", {0, 0, 0, 17, 05, 03, 0}, TINYUTC_ISO8601_OK},
    {"Partial time, only hours", "17", {0, 0, 0, 17, 00, 00, 0}, TINYUTC_ISO8601_OK},
    {"Partial time, only hours, with T", "T17", {0, 0, 0, 17, 00, 00, 0}, TINYUTC_ISO8601_OK},
    {"Partial time, separator, hh:mm", "17:05", {0, 0, 0, 17, 05, 00, 0}, TINYUTC_ISO8601_OK},
    {"Partial time, separator, hh:mm:s", "17:05:3", {0, 0, 0, 17, 05, 03, 0}, TINYUTC_ISO8601_OK},
    {"Microseconds, .02", "01:23:45.02", {0, 0, 0, 01, 23, 45, 20000}, TINYUTC_ISO8601_OK},
    {"Microseconds, .3", "01:23:45.3", {0, 0, 0, 01, 23, 45, 300000}, TINYUTC_ISO8601_OK},
    {"Microseconds, 111111", "T01:23:45.111111", {0, 0, 0, 1, 23, 45, 111111}, TINYUTC_ISO8601_OK},
    {"Microseconds, 999999", "01:23:45,999999", {0, 0, 0, 1, 23, 45, 999999}, TINYUTC_ISO8601_OK},
    {"Microseconds, 7777777", "01:23:45.7777777", {0, 0, 0, 1, 23, 45, 0}, TINYUTC_ISO8601_TIME_FRACTION_TOO_LONG},
    {"24:00:00", "24:00:00Z", {0, 0, 0, 0, 0, 0, 0}, TINYUTC_ISO8601_OK},
    {"24:00:01", "24:00:01Z", {0, 0, 0, 0, 0, 0, 0}, TINYUTC_ISO8601_INVALID_TIME},
    {"24:01:00", "24:01:00Z", {0, 0, 0, 0, 0, 0, 0}, TINYUTC_ISO8601_INVALID_TIME},
};

int main()
{
    struct TinyUTCTime utc_tm = {0};
    int sucess_count = 0;

    char spacingdesc[51] = {' '};
    char spacingiso[31] = {' '};

    for (int i = 0; i < sizeof(test_cases) / sizeof(test_cases[0]); i++)
    {
        int parse_result = tinyutc_parse_iso8601_time(&utc_tm, test_cases[i].iso8601);
        int is_expected_time = compare_utc_struct_times(&utc_tm, &test_cases[i].expected);
        if (test_cases[i].expected_code == parse_result && (parse_result != TINYUTC_ISO8601_OK || is_expected_time))
        {
            int desclength = strlen(test_cases[i].description);
            int spacelength = 50 - desclength;
            memset(spacingdesc, ' ', spacelength);
            spacingdesc[spacelength] = '\0';
            spacingdesc[50] = '\0';

            int isolength = strlen(test_cases[i].iso8601);
            spacelength = 30 - isolength;
            memset(spacingiso, ' ', spacelength);
            spacingiso[spacelength] = '\0';
            spacingiso[30] = '\0';

            if (parse_result == TINYUTC_ISO8601_OK)
            {
                printf("\033[38;5;2m\033[1m[SUCCESS]\033[39m\t Test '%s' %s : '%s' %s => %02d:%02d:%02d (+%dus) \n", test_cases[i].description, spacingdesc, test_cases[i].iso8601, spacingiso,
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
            printf("\033[38;5;1m\033[1m[FAILED]\033[39m\t Test '%s': %s : \n\t\t\t - Expected error code %d, got %d\n", test_cases[i].description, test_cases[i].iso8601, test_cases[i].expected_code, parse_result);
            if (!is_expected_time)
            {
                printf("\t\t\t - Expected time T%02d:%02d:%02d (+%dus), got T%02d:%02d:%02d (+%dus)\n",
                       test_cases[i].expected.hour, test_cases[i].expected.minute, test_cases[i].expected.second, test_cases[i].expected.microseconds,
                       utc_tm.hour, utc_tm.minute, utc_tm.second, utc_tm.microseconds);
            }
            // break;
        }
    }
    printf("%d/%llu tests passed.\n", sucess_count, sizeof(test_cases) / sizeof(test_cases[0]));
}
